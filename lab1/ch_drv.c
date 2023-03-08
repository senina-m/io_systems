#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/proc_fs.h> 
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#define PROCFS_NAME "var2"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0) 
  #define HAVE_PROC_OPS 
#endif 

static struct proc_dir_entry* our_proc_file;

static ssize_t procfile_read(struct file *filePointer, char __user *buffer, 
                             size_t buffer_length, loff_t *offset) 
{ 
    pr_info("Procfile read\n");
    res_buffer[res_end] = '\0';
    return simple_read_from_buffer(buf, len, off, res_buffer, res_end + 1);
}

#ifdef HAVE_PROC_OPS 
static const struct proc_ops proc_file_fops = { 
    .proc_read = procfile_read, 
}; 
#else 
static const struct file_operations proc_file_fops = { 
    .read = procfile_read, 
}; 
#endif 

static dev_t first;
static struct cdev c_dev; 
static struct class *cl;

static char res_buffer[4096];
static int res_end = 0;

enum operation{
  plus=0, 
  minus=1, 
  mul=2,
  del=3
};

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

void itoa(int num, char* str){
  int i = 0;
  int base = 10;
  bool isNegative = false;
  
  if (num == 0){
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  if (num < 0){
    isNegative = true;
    num = -num;
  }
  
  while (num != 0){
    int rem = num % base;
    str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
    num = num/base;
  }

  if (isNegative)
    str[i++] = '-';

  str[i] = '\0';

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

static int my_open(struct inode *i, struct file *f){
  printk(KERN_INFO "Driver: open()\n");
  return 0;
}

static int my_close(struct inode *i, struct file *f){
  printk(KERN_INFO "Driver: close()\n");
  return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off){
  printk(KERN_INFO "Driver: read()\n");
  res_buffer[res_end] = '\0';
  return simple_read_from_buffer(buf, len, off, res_buffer, res_end + 1);
}

static ssize_t my_write(struct file *file, const char __user *user_buffer, size_t len, loff_t * offset){
  char str[len];
  int res;
  char str_res[10];
  printk(KERN_INFO "Driver: write()\n");
  if (copy_from_user(str, user_buffer, len) != 0) return -EFAULT;
  printk(KERN_INFO "Writen buffer %s", str);

  if(read_equatuion(str, &res)) printk(KERN_INFO "Read \'%s\', didn't manage to count result!\n");
  itoa(res, str_res);
  int str_res_len = strlen(str_res) - 1;
  copy_str(res_buffer, str_res, res_end, str_res_len);
  res_end += str_res_len;
  return len;
}

static int my_dev_uevent(struct device *dev, struct kobj_uevent_env *env)
{
  add_uevent_var(env, "DEVMODE=%#o", 0777);
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

    //chrdev init
    if (alloc_chrdev_region(&first, 0, 1, "ch_dev") < 0) {
		  return -1;
	  }
    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL) {
		  unregister_chrdev_region(first, 1);
		  return -1;
	  }

  cl->dev_uevent = my_dev_uevent;

    if (device_create(cl, NULL, first, NULL, "mychdev") == NULL) {
		  class_destroy(cl);
		  unregister_chrdev_region(first, 1);
		  return -1;
	  }
    cdev_init(&c_dev, &mychdev_fops);
    if (cdev_add(&c_dev, first, 1) == -1) {
		  device_destroy(cl, first);
		  class_destroy(cl);
		  unregister_chrdev_region(first, 1);
		  return -1;
	  }

    // procfs init
    our_proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_fops); 
    if (NULL == our_proc_file) { 
        proc_remove(our_proc_file); 
        pr_alert("Error:Could not initialize /proc/%s\n", PROCFS_NAME); 
        return -ENOMEM; 
    } 
 
    pr_info("/proc/%s created\n", PROCFS_NAME); 

    return 0;
}
 
static void __exit ch_drv_exit(void)
{
    // chrdev deatroy
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);

    //procfs destory
    proc_remove(our_proc_file); 
    pr_info("Bye!!!\n");
}
 
module_init(ch_drv_init);
module_exit(ch_drv_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sennik");
MODULE_DESCRIPTION("Kernel calculator");

