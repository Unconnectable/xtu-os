
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#define READ_ 0
#define WRITE_ 1
//参考本人xv6代码
/*
                  0:read                    1:write

            child (read)  <---------- Parent(write)  this is pipe1
            Parent(read)  <---------- Child (write)  this is pipe2
*/
int main() {
  int Pipe2[2];
  int Pipe1[2];
  pipe(Pipe1);
  pipe(Pipe2);
  pid_t p1 = fork();
  if (p1 == 0) {
    const char *msg1 = "Child 1 is sending a message!";
    write(Pipe1[WRITE_], msg1, strlen(msg1) + 1); //末尾是'0'结束符 多一个
    close(Pipe1[WRITE_]);
    exit(0);
  }
  pid_t p2 = fork();
  if (p2 == 0) {
    const char *msg2 = "Child 2 is sending a message!";
    write(Pipe2[WRITE_], msg2, strlen(msg2) + 1); //末尾是'0'结束符 多一个
    close(Pipe2[WRITE_]);
    exit(0);
  }

  else {
    close(Pipe1[WRITE_]);
    close(Pipe2[WRITE_]);

    char buf1[100], buf2[100];
    read(Pipe1[READ_], buf1, sizeof(buf1));
    printf("From P1: %s\n", buf1);

    read(Pipe2[READ_], buf2, sizeof(buf2));
    printf("From P2: %s\n", buf2);

    close(Pipe1[READ_]);
    close(Pipe2[READ_]);

    wait(NULL);
    wait(NULL);
    return 0;
  }
}