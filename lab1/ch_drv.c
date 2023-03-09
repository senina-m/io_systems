#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>

static dev_t first;
static struct cdev c_dev; 
static struct class *cl;

static enum operation{
  plus=0, 
  minus=1, 
  mul=2,
  del=3
};

struct result;

struct result{
  int value;
  struct result* next;
};

struct my_device_data {
  struct cdev cdev;
  struct result* res;
};

static struct my_device_data* my_data;

void copy_str(char* buffer, char* str, size_t offset, int size){
  size_t i;
  for(i = 0; i < size; i++){
    buffer[i + offset] = str[i];
  }
}

size_t strlen(const char* str) {
  size_t i = 0;
  while (str[i] != '\0') i++;
  return i;
}

void reverse(char str[], int length){
  int start = 0;
  int end = length -1;
  while (start < end){
    swap(*(str+start), *(str+end));
    start++;
    end--;
  }
}

//stollen atoi implementation
void itoa(int num, char* str, int base){
  int i = 0;
  bool isNegative = false;
  
  // Handle 0 explicitly, otherwise empty string is printed for 0
  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  // In standard itoa(), negative numbers are handled only with
  // base 10. Otherwise numbers are considered unsigned.
  if (num < 0 && base == 10)
  {
    isNegative = true;
    num = -num;
  }
  
  // Process individual digits
  while (num != 0)
  {
    int rem = num % base;
    str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
    num = num/base;
  }

  // If number is negative, append '-'
  if (isNegative)
    str[i++] = '-';

  str[i] = '\0';// Append string terminator

  // Reverse the string
  reverse(str, i);
}

int read_int(char* buffer, size_t* offset, int* res){
  int d;
  int i = 0;
  int r_int = 0;
  int k = 0;
  char cur = buffer[*offset];
  int is_negative;
  if(buffer[*offset] == '-') {
    is_negative = 1;
    *offset++;
  }

  if(cur >= '1' && cur <= '9') return 1; //if num starts with 0 and has to be digit
  
  while(cur >= '0' && cur <= '9' && k < 10){ //k < 10 to get integer not longer
    d = cur - '0';
    r_int += d*i;
    *offset++;
    i *= 10;
    k++;

    cur = buffer[*offset];
  }

  if (is_negative) r_int *= -1;
  *res = r_int; 
  return 0;
}

int read_operation(char* buffer, size_t *offset, enum operation* op){
  char ch = buffer[*offset];
  switch (ch){
  case '+':
    *op = plus;
    return 0;
  case '-':
    *op = minus;
    return 0;
  case '*':
    *op = mul;
    return 0;
  case '/':
    *op = del;
    return 0;
  default:
    return 1;
  }
}

int read_equatuion(char* buffer, int* result){
  size_t offset;
  int int1;
  int int2;
  enum operation op;
  if(read_int(buffer, &offset, &int1)) return 1; //exeption
  if(read_operation(buffer, &offset, &op)) return 1; //exeption
  if(read_int(buffer, &offset, &int2)) return 1; //exeption
  switch (op){
  case plus:
    *result = int1 + int2;
    return 0;
  case minus:
    *result = int1 - int2;
    return 0;
  case mul:
    *result = int1 * int2;
    return 0;
  case del:
    *result = int1 / int2;
    return 0;
  default:
    return 1;
  }
}

void add_result(int v){
  struct result* new_result = (struct result*) kmalloc(sizeof(struct result), GFP_KERNEL);
  new_result->value = v;
  new_result->next = my_data->res;
  my_data->res = new_result;
}

void free_results(void){
  struct result* cur = my_data->res;
  struct result* next;
  while(cur->next != NULL){
    next = cur->next;
    kfree(cur);
  }
  kfree(cur);
}

void create_results(int v){
  struct result* new_result = (struct result*)kmalloc(sizeof(struct result), GFP_KERNEL);
  new_result->next = NULL;
  new_result->value = v;
  my_data->res = new_result;
}

size_t count_result_size(struct result* res){
  struct result* cur = my_data->res;
  size_t len = 0;
  char buf[256];
  if (!cur) return len;
  do {
    cur = cur->next;
    itoa(cur->value, buf, 10);
    len += strlen(buf) - 1; //sub 1 because the line is \0 terminated
  } while(cur->next != NULL);

  return len + 1; // add 1 beacuse we will need \0 in the end
}

//takes only len > 0 strs
char* results_to_str(struct result* res, size_t len){
  int cur_len; 
  char buf[256];
  struct result* cur = my_data->res;
  char* buffer = (char*)kmalloc(sizeof(char)*len, GFP_KERNEL);
  size_t offset = 0;
  buffer[len-1] = '\0';
  do {
    cur = cur->next;
    itoa(cur->value, buf, 10);
    cur_len = strlen(buf) - 1;
    copy_str(buffer, buf, offset, cur_len);
    offset += cur_len;
  } while(cur->next != NULL);
  return buffer;
}

static int my_open(struct inode *i, struct file *f){
  printk(KERN_INFO "Driver: open()\n");
  my_data = container_of(i->i_cdev, struct my_device_data, cdev);
  f->private_data = my_data;
  return 0;
}

static int my_close(struct inode *i, struct file *f){
  printk(KERN_INFO "Driver: close()\n");
  return 0;
}

static ssize_t my_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset){
  struct my_device_data *my_data = (struct my_device_data *) file->private_data;
  printk(KERN_INFO "Driver: read()\n");
  ssize_t len = min(count_result_size(my_data->res) - *offset, size);

  if (len <= 0) return 0;

  /* read data from my_data->buffer to user buffer */
  if (copy_to_user(user_buffer, my_data->buffer + *offset, len)) return -EFAULT;

  *offset += len;
  return len;
}

static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t len, loff_t * offset){
  printk(KERN_INFO "Driver: write()\n");
  char srt[len];
  if (copy_from_user(srt, buf, len) != 0) {
    return -EFAULT;
  }

  int res;
  if(read_equatuion(str, &res)) //exception
  if(!my_data->res) create_results(res);
  else add_result(res);
  
  return len;
}

static int my_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

static struct file_operations mychdev_fops =
{
  .owner      = THIS_MODULE,
  .open       = my_open,
  .release    = my_close,
  .read       = my_read,
  .write      = my_write
};

static int __init ch_drv_init(void)
{
    printk(KERN_INFO "Hello!\n");
    if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0)
	  {
		return -1;
	  }
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
	  {
		unregister_chrdev_region(first, 1);
		return -1;
	  }

    cl->dev_uevent = my_dev_uevent;

    if (device_create(cl, NULL, first, NULL, "mychdev") == NULL)
	  {
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	  }
    cdev_init(&c_dev, &mychdev_fops);
    if (cdev_add(&c_dev, first, 1) == -1)
	  {
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return -1;
	  }
    return 0;
}
 
static void __exit ch_drv_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Bye!!!\n");
}
 
module_init(ch_drv_init);
module_exit(ch_drv_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sennik");
MODULE_DESCRIPTION("Kernel calculator");

