#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>


int main(int argc, char const *argv[]){

  FILE* f = fopen("/dev/mychardev", "w+");
  if (f == NULL){
    printf("Oh dear, something went wrong with open()! %s\n", strerror(errno));
    return -1;
  }
  char buf[4] = "1+2";
  printf("OPEN: %p\n",f);
  fwrite(buf, 4, 1, f);
  fclose(f);
  return 0;
}