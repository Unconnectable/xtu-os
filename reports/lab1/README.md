# lab1

> ## **以下代码如果都是忽略各种错误的,基于健壮性的带有检查错误的我会以附录给出**

## (1)

`fork()`来自`<unistd.h>`头文件

## (2)

> ### 没啥好说的

## (3)

**目标**:程序创建一个父进程和两个子进程.父进程等待用户按 Ctrl+C(触发 SIGINT 信号),然后向两个子进程发送 SIGUSR1 信号,子进程接收信号后打印信息并退出,父进程等待子进程结束后再退出.

关键

- **进程创建**:使用 fork()创建子进程.
- **信号处理**:使用 signal()注册信号处理函数,处理 SIGINT(父进程)和 SIGUSR1(子进程).
- **信号发送**:父进程使用 kill()向子进程发送信号.
- **进程同步**:使用 pause()让进程等待信号,wait()避免僵尸进程.

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
    exit(0);
  }
}
int main() {
  pid1 = fork();
  if (pid1 == 0) {
    // 忽略 SIGINT 避免子进程处理 Ctrl+C 中断
    signal(SIGINT, SIG_IGN);
    //子进程信号处理函数  捕捉 handle2的函数指针
    signal(SIGUSR1, handle1);
    while (1)
      pause();
  }

  pid2 = fork();

  if (pid2 == 0) {
    signal(SIGINT, SIG_IGN);
    //子进程信号处理函数  捕捉handle2的函数指针
    signal(SIGUSR1, handle2);

    while (1)
      pause();
  }
  //注册父进程的信号处理函数,捕捉 SIGINT
  signal(SIGINT, handle_parent);

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

## **注意的问题 🔴🔴🔴**

根据不同的需求,父进程的信号处理函数的位置会不同

如果在 fork 之前启动了`signal(SIGINT, handle_parent);`,fork 的子线程会继承这个的状态

放在前后有两种观点:

1.**父进程必须处理 SIGINT:** 父进程需要接收 SIGINT 信号(由 Ctrl+C 触发)并采取行动,例如杀死子进程.如果在 fork() 之后才注册,那么父进程可能在 Ctrl+C 到来之前就已经创建了子进程,而此时子进程的 SIGINT 处理行为是不确定的.

2.因为 `fork()` 会复制当前进程的所有内容(包括信号处理方式),如果你在 `fork()` 之前设置信号处理函数:

- 子进程也会继承这个设置;
- 如果你不希望子进程也响应 `SIGINT`,那就会出问题;
- 比如:按下 `Ctrl + C` 后多个进程都进入信号处理函数 → 输出混乱或程序崩溃.

### 对于不同的位置没有在自线程忽略`ctrl+V`的输出如下,包括且不仅限制于

1.多次收到`SIGINT`

2.未能 kill 两个子线程就结束

```sh
#1
🔴Parent receive SIGINT
🔴Parent receive SIGINT
🟢Child Process1 PID=51137 is killed by parent!
🟢Child Process1 PID=51136 is killed by parent!
User defined signal 1

#2
🔴Parent receive SIGINT
Parent  sent signals to child
🟢Child Process1 PID=51177 is killed by parent!

#3
🔴Parent receive SIGINT
🔴Parent receive SIGINT
Parent  sent signals to child
🟢Child Process1 PID=51518 is killed by parent!
🟢Child Process1 PID=51517 is killed by parent!
User defined signal 1

#4
🔴Parent receive SIGINT
🔴Parent receive SIGINT
🔴Parent receive SIGINT
🟢Child Process1 PID=51541 is killed by parent!
🟢Child Process1 PID=51540 is killed by parent!
User defined signal 1
```

```c
signal(SIGINT, SIG_IGN);
signal(SIGQUIT, SIG_IGN);
```

![1.3(2)](</assets/1.3(2).png>)

**让当前进程忽略 `SIGINT` 和 `SIGQUIT` 信号,防止程序被用户中断(如按下 Ctrl + C 或 Ctrl + \)**

## (4)

#### **一个管道可能会竞争,建议还是两个**

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

# 附录

## ( 3)

```c
// 信号处理在fork之前
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// 全局变量,存储子进程的 PID
pid_t child1_pid, child2_pid;

// 子进程 1 的信号处理函数
void sig_handler_child1(int signo) {
  if (signo == SIGUSR1) {
    printf("🟢Child Process 1 (PID: %d) is killed by parent!\n", getpid());
    exit(0);
  }
}

void sig_handler_child2(int signo) {
  if (signo == SIGUSR1) {
    printf("🟢Child Process 2 (PID: %d) is killed by parent!\n", getpid());
    exit(0);
  }
}

// 父进程的信号处理函数
void sig_handler_parent(int signo) {
  if (signo == SIGINT) {
    printf("🔴Parent process (PID: %d) received SIGINT (DEL key).\n", getpid());
    if (kill(child1_pid, SIGUSR1) == -1) {
      perror("kill (child1)");
    }
    if (kill(child2_pid, SIGUSR1) == -1) {
      perror("kill (child2)");
    }
    printf("Parent process sent signals to child processes.\n");
    wait(NULL);
    wait(NULL);
		printf("Parent Process is killed!\n");

    exit(0);
  }
}

int main() {
  // 注册父进程的信号处理函数,捕捉 SIGINT
  if (signal(SIGINT, sig_handler_parent) == SIG_ERR) {
    perror("signal (SIGINT)");
    exit(1);
  }

  // 创建第一个子进程
  child1_pid = fork();
  if (child1_pid == -1) {
    perror("fork (child1)");
    exit(1);
  }

  if (child1_pid == 0) { // 子进程 1
    // 忽略 SIGINT 避免子进程处理 Ctrl+C
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
      perror("signal (SIGINT - child1)");
      exit(1);
    }
    // 注册子进程 1 的信号处理函数,捕捉 SIGUSR1
    if (signal(SIGUSR1, sig_handler_child1) == SIG_ERR) {
      perror("signal (SIGUSR1 - child1)");
      exit(1);
    }
    sleep(1);
    printf("Child Process 1 (PID: %d) started, waiting for signal...\n",
           getpid());
    while (1) {
      pause();
    }
  }

  // 创建第二个子进程
  child2_pid = fork();
  if (child2_pid == -1) {
    perror("fork (child2)");
    kill(child1_pid, SIGKILL);
    wait(NULL);
    exit(1);
  }

  if (child2_pid == 0) { // 子进程 2
    // 忽略 SIGINT,避免子进程处理 Ctrl+C
    if (signal(SIGINT, SIG_IGN) == SIG_ERR) {
      perror("signal (SIGINT - child2)");
      exit(1);
    }
    // 注册子进程 2 的信号处理函数,捕捉 SIGUSR1
    if (signal(SIGUSR1, sig_handler_child2) == SIG_ERR) {
      perror("signal (SIGUSR1 - child2)");
      exit(1);
    }
    sleep(1);
    printf("Child Process 2 (PID: %d) started, waiting for signal...\n",
           getpid());
    while (1) {
      pause();
    }
  }

  // 父进程
  printf("Parent process (PID: %d) created Child Process 1 (PID: %d) and Child "
         "Process 2 (PID: %d).\n",
         getpid(), child1_pid, child2_pid);
  printf("Press DEL key (Ctrl+C) to send signals to child processes.\n");

  while (1) {
    pause();
  }

  return 0;
}
```

## (4)

### 一个管道实现

```c
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
  close(fd[1]); // 父进程只读,关闭写端

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
```
