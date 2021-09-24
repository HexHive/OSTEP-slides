---
title   : CS-323 Operating Systems
subtitle: Processes (and threads)
author  : Mathias Payer
date    : EPFL, Fall 2019
---

# Q&A from the feedback round

* Differences between program / process / thread?
* How does the process API work?
* Can a child process have children?

---

# Program / process / thread

* Program: consists of an executable on disk. Contains all information to become
  a process
* Process: a running instance of a program; has data section and stack
  initialized
* Thread: a process can have multiple threads in the same address space
  (computing on the same data)

---

# Process API

The process API enables a process to control itself and other processes through
a set of system calls:

* `fork()` creates a new child process (a copy of the process)
* `exec()` executes a new program
* `exit()` terminates the current process
* `wait()` blocks the parent until the child terminates
* This is a small subset of the complex process API (more later)

---

# Process API: `fork()` demo!

```.C
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
  printf("Hello, I'm PID %d (%d, %s)\n", (int)getpid(),
         argc, argv[0]);
  int pid = fork();
  if (pid < 0) exit(-1);  // fork failed
  if (pid == 0) {
    printf("o/ I'm PID %d\n", (int)getpid());
  } else {
    printf("\\o, my child is PID %d\n", pid);
  }
  return 0;
}
```

---

# A tree of processes

* Each process has a parent process
* A process can have many child process
* Each process again can have child processes

```
 3621  ?        Ss   \_ tmux
 3645  pts/2    Ss+  |   \_ -zsh
 3673  pts/3    Ss+  |   \_ -zsh
 4455  pts/4    Ss+  |   \_ -zsh
27124  pts/1    Ss+  |   \_ -zsh
21093  pts/5    Ss   |   \_ -zsh
10589  pts/5    T    |   |   \_ vim 02-review.md
10882  pts/5    R+   |   |   \_ ps -auxwf
10883  pts/5    S+   |   |   \_ less
21264  pts/7    Ss   |   \_ -zsh
 1382  pts/7    T    |   |   \_ vim /home/gannimo/notes.txt
14368  pts/9    Ss   |   \_ -zsh
29963  pts/9    S+   |       \_ python
```

