#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  int fd[2];
  pid_t p1, p2;

  if (pipe(fd) == -1) {
    perror("Pipe failed");
    exit(1);
  }

  p1 = fork();
  if (p1 == 0) {
    const char *msg = "Child 1 is sending a message!";
    write(fd[1], msg, strlen(msg) + 1);
    close(fd[1]);
    exit(0);
  }

  p2 = fork();
  if (p2 == 0) {
    const char *msg = "Child 2 is sending a message!";
    write(fd[1], msg, strlen(msg) + 1);
    close(fd[1]);
    exit(0);
  }

  // 父进程
  close(fd[1]); // 父进程只读，关闭写端

  char buffer[100];

  // 读 p1
  read(fd[0], buffer, sizeof(buffer));
  printf("From P1: %s\n", buffer);

  // 读 P2
  read(fd[0], buffer, sizeof(buffer));
  printf("From P2: %s\n", buffer);

  close(fd[0]); // 关闭读端

  wait(NULL);
  wait(NULL);

  return 0;
}