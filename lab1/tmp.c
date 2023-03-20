#include <stddef.h>
#include <stdio.h>

#define BUFFER_SIZE 4906
static char res_buffer[BUFFER_SIZE];
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

void swap(char *xp, char *yp)
{
  char temp = *xp;
  *xp = *yp;
  *yp = temp;
}

void reverse(char str[], int length){
  int start = 0;
  int end = length -1;
  while (start < end){
    swap((str+start), (str+end));
    start++;
    end--;
  }
}

void itoa(int num, char* str){
  int i = 0;
  int base = 10;
  int isNegative = 0;
  
  if (num == 0){
    str[i++] = '0';
    str[i] = '\0';
    return;
  }

  if (num < 0){
    isNegative = 1;
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
    int i = 1;
    int r_int = 0;
    int k = 0;
    // printf("Parse buffer: %s, offset=%ld\n", buffer, *offset);
    char cur = buffer[*offset];
    int is_negative = 0;
    

    if(buffer[*offset] == '-') {
        is_negative = 1;
        (*offset)++;
        cur = buffer[*offset];
        // printf("HEEEEY ITs NEGATIVE\n");
    }

    // printf("Read int: neg=%i, fst chr=%c, i_c=%i offset=%ld, res=%i\n", is_negative, cur, cur, *offset, r_int);
    if (!(cur >= '1' && cur <= '9')) return 1; //if num starts with 0 and has to be digit

    while(cur >= '0' && cur <= '9' && k < 10){ //k < 10 to get integer not longer
        d = cur - '0';
        r_int = r_int * 10;
        r_int += d;

        // printf("Read int: neg=%i, fst chr=%i, i=%i offset=%ld, res=%i\n", is_negative, d, i, *offset, r_int);
        (*offset)++;
        k++;

        cur = buffer[*offset];
  }

  if (is_negative) r_int *= -1;
  *res = r_int; 
  return 0;
}

int read_operation(char* buffer, size_t *offset, enum operation* op){
  char ch = buffer[*offset];
//   printf("Read op: buff=%s, ch=%c, off=%ld\n", buffer, ch, *offset);
  (*offset)++;
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
    printf("WRONG OPERATION!");
    return 1;
  }
}

int read_equatuion(char* buffer, int* result){
  size_t offset = 0;
  int int1;
  int int2;
  enum operation op;

  if(read_int(buffer, &offset, &int1)) return 1; //exeption
  printf("Readed int1: %d\n", int1);
  if(read_operation(buffer, &offset, &op)) return 1; //exeption
  printf("Operation: %i\n", op);
  if(read_int(buffer, &offset, &int2)) return 1; //exeption
  printf("Readed int2: %d\n", int2);


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


static void my_read(){
  printf("Driver: read()\n");
  int tmp = res_buffer[res_end];
  res_buffer[res_end] = '\0';
  printf("RESULT: %s\n", res_buffer);
  int i = 0;
  printf("res_end=%i:::::::\n", res_end); 
  for(i; i < res_end; i++){
    printf("\'%c\'", res_buffer[i]);
  }
  printf("\n");
  res_buffer[res_end] = tmp;
}

static int my_write(char* str, int len){
  int res;
  char str_res[10];
  printf("Driver: write()\n");
  printf("Writen buffer %s\n", str);

  if(read_equatuion(str, &res)) printf("Read \'%s\', didn't manage to count result!\n", str);

  itoa(res, str_res);
  int str_res_len = strlen(str_res);

  printf("res=%s res_len=%i\n", str_res, str_res_len);

  copy_str(res_buffer, str_res, res_end, str_res_len);

  res_end += str_res_len;
  res_buffer[res_end] = ';';
  
  if(res_end > BUFFER_SIZE){
    printf("Buffer out of bounds!");
    res_end = 0;
  }

  return len;
}

int main(int argc, char const *argv[]){
  char str[7] = "-35+462";
  my_write(str, 7);
  my_read();
  return 0;
}
