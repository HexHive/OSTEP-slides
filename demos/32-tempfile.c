#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

int main(int argc __attribute__((unused)),
         char* argv[] __attribute__((unused))) {
  int fd = open("test", O_CREAT | O_RDWR, 0600);
  //unlink(FILENAME);
  char *data = "test";
  char rdata[64];
  write(fd, data, strlen(data));
  sleep(10);
  lseek(fd, 0, SEEK_SET);
  read(fd, &rdata, 64);
  rdata[63] = 0;
  printf("We read '%s'\n", rdata);
  close(fd);
  return 0;
}
