#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main() {

  pid_t pid = fork();
  if (pid < 0) {
    perror("Fork fail");
    return 1;
  } else if (pid == 0) {
    printf("b\n");
    printf("c\n");
  } else {
    printf("a\n");
  }
}