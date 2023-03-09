#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

//stollen atoi implementation
char* itoa(int num, char* str, int base){
  int i = 0;
  int isNegative = 0;
  
  // Handle 0 explicitly, otherwise empty string is printed for 0
  if (num == 0)
  {
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }

  // In standard itoa(), negative numbers are handled only with
  // base 10. Otherwise numbers are considered unsigned.
  if (num < 0 && base == 10)
  {
    isNegative = 1;
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
  return str;
}


int main(int argc, char const *argv[]){

    char* str = "-123";
    atoi(str);

    // FILE* f = fopen("/dev/mychardev", "w+");
    // if (f == NULL){
    //     printf("Oh dear, something went wrong with open()! %s\n", strerror(errno));
    //     exit();
    // }
    // char buf[4] = "1+2";
    // printf("OPEN: %p\n",f);
    // fwrite(buf, 4, 1, f);
    fclose(f);

    return 0;
}