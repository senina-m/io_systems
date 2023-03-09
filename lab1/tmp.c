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
