#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
pid_t pid1, pid2;

void handle1(int sig) {
  if (sig == SIGUSR1) {
    printf("🟢Child Process1 PID=%d is killed by parent!\n", getpid());
    exit(0);
  }
}
void handle2(int sig) {
  if (sig == SIGUSR1) {
    printf("🟢Child Process1 PID=%d is killed by parent!\n", getpid());
    exit(0);
  }
}
void handle_parent(int sig) {
  // Ctrl + C 时 操作系统会向进程发送 SIGINT
  if (sig == SIGINT) {
    printf("🔴Parent receive SIGINT\n");
    // kill 然后发送SIGUSR信号
    if (kill(pid1, SIGUSR1) == -1) {
      perror("kill (child1)");
    }
    if (kill(pid2, SIGUSR1) == -1) {
      perror("kill (child2)");
    }
    printf("Parent  sent signals to child\n");
    //回收资源 防止僵尸进程
    wait(NULL);
    wait(NULL);
    //退出
    printf("Parent Process is killed!\n");
    exit(0);
  }
}
int main() {

  // signal(SIGINT, handle_parent);
  signal(SIGINT, handle_parent);
  pid1 = fork();
  if (pid1 == 0) {
    // 忽略 SIGINT 避免子进程处理 Ctrl+C 中断
    // signal(SIGINT, SIG_IGN);
    //子进程信号处理函数  捕捉 handle2的函数指针
    signal(SIGUSR1, handle1);
    while (1)
      pause();
  }

  pid2 = fork();

  if (pid2 == 0) {
    // signal(SIGINT, SIG_IGN);
    //子进程信号处理函数  捕捉handle2的函数指针
    signal(SIGUSR1, handle2);

    while (1)
      pause();
  }
  //注册父进程的信号处理函数，捕捉 SIGINT

  printf("Parent PID=%d created Child1 PID=%d"
         "and Child2 PID=%d.\n",
         getpid(), pid1, pid2);

  printf("按下 Ctrl+C to send signals to child processes.\n");
  printf("\n\n");

  while (1)
    pause();
  return 0;
}