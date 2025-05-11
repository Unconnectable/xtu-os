# 实验报告

## 基本信息

- **班级**: 计科 23 级 1 班
- **学号**: 202305133352
- **姓名**: 周潍可
- **实验名称**: 创建进程

---

## 1.实验目的

> 本次实验旨在掌握操作系统中进程的创建、通信及信号处理的基本原理与实现方法.通过编写 C 语言程序,使用系统调用函数如 fork() 创建子进程,signal() 注册信号处理函数,以及 kill() 发送信号,深入理解多进程并发执行、父子进程之间的协作机制和进程同步问题.此外,还通过管道实现父子进程间的双向通信,进一步掌握进程间数据传输的方法.

---

## 2.数据结构说明

> 实验中使用了以下数据结构:

> - pid_t: 用于保存进程标识符(PID),在创建子进程时由 fork() 返回.
> - int Pipe[2]: 表示一个管道文件描述符数组,其中 Pipe[0] 为读端,Pipe[1] 为写端,用于实现父子进程之间的通信.
> - 函数指针:用于注册信号处理函数,例如 signal(SIGINT, handle_parent) 将 SIGINT 信号绑定到指定的处理函数上.

---

## 3.处理过程(流程图或盒图)

![1.3(2)](</assets/1.3(2).png>)
![lab1](/assets/lab1.png)

## 4.源代码

### (1)

```c
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
```

### (2)

```c
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
```

### (3)

```c
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
  //注册父进程的信号处理函数,捕捉 SIGINT

  printf("Parent PID=%d created Child1 PID=%d"
         "and Child2 PID=%d.\n",
         getpid(), pid1, pid2);

  printf("按下 Ctrl+C to send signals to child processes.\n");
  printf("\n\n");

  while (1)
    pause();
  return 0;
}
```

### (4)

```c

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
```

## 5.实验总结

次实验让我深刻理解了 Linux 系统中进程的创建、通信与信号处理机制.在实验过程中,我掌握了如何使用 fork() 创建子进程,利用 signal() 注册信号处理函数来响应特定事件(如 Ctrl+C),并通过 kill() 向子进程发送自定义信号 SIGUSR1,从而实现父子进程之间的协调控制.

在实现过程中,我遇到了多个问题,例如:

子进程对 SIGINT 信号的默认处理导致程序行为混乱;
父进程未正确等待子进程导致僵尸进程;
信号处理函数位置不当引发的竞争条件.
通过合理设置信号处理方式(如在子进程中忽略 SIGINT)、在适当的位置注册信号处理函数,以及使用 wait() 回收子进程资源,最终成功解决了这些问题.

此外,在管道通信部分,我认识到单个管道存在竞争风险,因此采用了两个独立管道进行父子进程的双向通信,提高了程序的稳定性和可预测性.
