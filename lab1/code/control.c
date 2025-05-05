#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#define F_LOCK 1
#define F_ULOCK 0
int main() {
  pid_t pid;
  int fd;
  fd = open("control.txt", O_RDWR | O_CREAT, 0666);

  if (fd < 0) {
    perror("Faail open file");
    return 1;
  }

  // lock fd
  if (lockf(fd, 1, 0) < 0) {
    perror("Fail LOCK file");
    close(fd);
    return 1;
  }

  pid = fork();
  if (pid < 0) {
    // fork 失败也要解锁 和close fd
    perror("Fail Fork");
    lockf(fd, 0, 0);
    close(fd);
    return 1;
  } else if (pid == 0) {
    printf("Child p\n");
    printf("Cpid=%d\n", getpid());
    lockf(fd, 0, 0);
    close(fd);
  } else {
    printf("Father p\n");
    printf("Fpid=%d\n", getpid());
    lockf(fd, 0, 0);
    close(fd);
  }
}