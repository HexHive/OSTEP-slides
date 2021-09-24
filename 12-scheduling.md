---
subtitle: Virtual CPU (Scheduling)
---

# Topics covered in this lecture

Scheduling has two aspects: 1) how to switch from one process to another and
2) what process should run next

* Mechanism: context switch (how to switch)
* Mechanism: preemption (keeping control)
* Policy: scheduling (where to switch to)

This slide deck covers chapters 7--10 in OSTEP.

---

# Scheduling

How does the kernel switch from one
process to the other?

* System has several `ready` processes
* For simplicity, we assume one CPU

How does the kernel stay in control? 

. . .

* Configurable timers allow the OS to regain control

. . .

How does the kernel switch from one process to anther?

* Context switch saves running process' state in kernel structure
* Context switch restores state of next process
* Context switch transfers control to next process and "returns"

. . .

Note: a context switch is transparent to the process

---

# Mechanism: context switch

A context switch is a mechanism that allows the OS to store the current process
state and switch to some other, previously stored context.

Reasons for a context switch:

* The process completes/exits
* The process executes a slow HW operation (loading from disk) and the OS
  switches to another task that is ready
* The hardware requires OS help and issues an interrupt
* The OS decides to preempt the task and switch to another task (i.e., the
  processes has used up its time slice)

---

# Mechanism: context switch (pseudo code)

* A function call that returns asynchronously: process A starts the execution
  of the context switch but process B continues execution after the return of
  the function.
    * The function saves all registers in a scratch area
      (on the process' kernel stack or in
      a predefined area of the `task struct`).
    * The OS switches address spaces.
    * The function restores all registers from the scratch area.
    * The OS returns to process B.

---

# Mechanism: context switch (example, one address space)

```.ASM
# void ctx_swtch(struct context *old, struct context *new)
# Save old registers
movl 4(%esp), %eax  # load ptr to old into eax
popl 0(%eax)        # store old IP to old
movl %esp, 4(%eax)  # store stack pointer
movl %ebx, 8(%eax)  # store other registers
...
movl %ebp, 28(%eax)

# Load new registers
movl 4(%esp), %eax  # load ptr to new into eax
movl 28(%eax), %ebp # restore other registers
...
movl 8(%eax), %ebx
movl 4(%eax), %esp  # stack switch (from now on new stack)
pushl 0(%eax)       # store return addr
ret                 # finally return into new ctxt
```

---

# Mechanism: preemption

If a task never gives up control (`yield()`), exits, or performs I/O 
then it could run forever and the OS could not gain control.

. . .

The OS therefore sets a timer before scheduling a process. If the timer
expires, the hardware interrupts the execution of the process and switches to
the kernel. The kernel then decides if the process may continue.

---

# What is a scheduling policy?

The context switch mechanism takes care of ***how*** the kernel switches from 
one process to another, namely by storing its context and restoring the
context of the other process.

The scheduling policy determines ***which*** process should run next.
If there is only one "ready' process then the answer is easy. If there are more
processes then the policy decides in which order processes execute.

![Scheduling](./figures/12-timetable.png){ width=100px }

---

# Scheduler metrics

When analyzing scheduler policies, we use the following terms:

* **Utilization:** what fraction of time is the CPU executing a program
  (goal: maximize)
* **Turnaround time:** total global time from process creation to process exit
  (goal: minimize)
* **Response time:** time from becoming ready to being scheduled (goal:
  minimize)
* **Fairness:** all processes get same amount of CPU over time (goal: no
  starvation)
* **Progress:** allow processes to make forward progress (goal: minimize
  kernel interrupts)

---

# Reminder: process states

\begin{tikzpicture}

\node [draw, circle, ultra thick, minimum width=2cm] at (0,0) {Blocked};
\node [draw, circle, ultra thick, minimum width=2cm] at (-3,3) {Running};
\node [draw, circle, ultra thick, minimum width=2cm] at (3,3) {Ready};

\draw [ultra thick, ->] (-2.3, 2.3) -- (-0.7, 0.7);
\node at (-2.8, 1.2) {I/O: start};
\draw [ultra thick, ->] (0.7, 0.7) -- (2.3, 2.3);
\node at (2.5, 1.2) {I/O: done};
\draw [ultra thick, ->] (-2, 3.1) -- (2, 3.1);
\node at (0, 3.5) {Deschedule};
\draw [ultra thick, ->] (2, 2.9) -- (-2, 2.9);
\node at (0, 2.5) {Schedule};

\end{tikzpicture}

---

# Scheduler implementation

Simplest form: each state has an associated queue of tasks.

```.C
task_struct_t *get_next_task() {
  // consult task queues to find next runnable task
}

void enqueue_task(task_struct_t *task) {
  // set task to ready
  
  // update ready queue so that it can run at its turn
}
```

---

# Scheduling assumptions

Let's understand scheduler policies step by step. We start with some
simplifying assumptions

* Each job runs for the same amount of time
* All jobs arrive at the same time
* All jobs only use the CPU (no I/O)
* Run-time of jobs is known
* For now, we assume a single CPU

---

# First In, First Out (FIFO)

\Begin{multicols}{2}

* Tasks A, B, C of `len=2` arrive at `T=0` (0,2)
* Average turnaround
    * `(2+4+6)/3 = 4`
* Average response
    * `(0+2+4)/3 = 2`

\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick] (0,0) rectangle (2,4) node[pos=.5] {A};
\draw [green, ultra thick] (2.1,0) rectangle (4,4) node[pos=.5] {B};
\draw [blue, ultra thick] (4.1,0) rectangle (6,4) node[pos=.5] {C};

\draw [thin, gray, -stealth] (0,0) -- (10,0);
\foreach \x in {0,...,10} {%
    \draw ($(\x,0) + (0,-2pt)$) -- ($(\x,0) + (0,2pt)$)
        node [below] {$\x$};
}
\end{tikzpicture}

\End{multicols}

---

# Scheduling assumptions

* ~~Each job runs for the same amount of time~~
* All jobs arrive at the same time
* All jobs only use the CPU (no I/O)
* Run-time of jobs is known

---

# FIFO challenge: long running task

\Begin{multicols}{2}

* Task A is now of `len=6`
* Average turnaround
    * `(6+8+10)/3 = 8`
* Average response
    * `(0+6+8)/3 = 4.7`
* Turnaround and response time suffers when short jobs wait for long jobs!

\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick] (0,0) rectangle (6,4) node[pos=.5] {A};
\draw [green, ultra thick] (6.1,0) rectangle (8,4) node[pos=.5] {B};
\draw [blue, ultra thick] (8.1,0) rectangle (10,4) node[pos=.5] {C};

\draw [thin, gray, -stealth] (0,0) -- (10,0);
\foreach \x in {0,...,10} {%
    \draw ($(\x,0) + (0,-2pt)$) -- ($(\x,0) + (0,2pt)$)
        node [below] {$\x$};
}
\end{tikzpicture}

\End{multicols}

---

# SJF: Shortest Job First

* Long running tasks delay other tasks (convoy effect: one long
  running task delays many short running tasks like a truck followed by
  many cars)
* Short jobs must wait for completion of long task

New scheduler: choose ready job with shortest runtime!

![](./figures/12-tractor.png){ width=75% }

---

# SJF: turnaround

\Begin{multicols}{2}

* Task A is now of `len=6`
* Average turnaround
    * `(2+4+10)/3 = 5.3`
* Average response
    * `(0+2+4)/3 = 2`

\begin{tikzpicture}[scale=0.5]

\draw [green, ultra thick] (0,0) rectangle (2,4) node[pos=.5] {B};
\draw [blue, ultra thick] (2.1,0) rectangle (4,4) node[pos=.5] {C};
\draw [orange, ultra thick] (4.1,0) rectangle (10,4) node[pos=.5] {A};

\draw [thin, gray, -stealth] (0,0) -- (10,0);
\foreach \x in {0,...,10} {%
    \draw ($(\x,0) + (0,-2pt)$) -- ($(\x,0) + (0,2pt)$)
        node [below] {$\x$};
}
\end{tikzpicture}

\End{multicols}

---

# Scheduling assumptions

* ~~Each job runs for the same amount of time~~
* ~~All jobs arrive at the same time~~
* All jobs only use the CPU (no I/O)
* Run-time of jobs is known

---

# SJF: another convoy!

\Begin{multicols}{2}

* Tasks B, C now arrive at 1
* Average turnaround
    * `(6+7+9)/3 = 7.3`
* Average response
    * `(0+5+7)/3 = 4`

\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick] (0,0) rectangle (6,4) node[pos=.5] {A};
\draw [green, ultra thick] (6.1,0) rectangle (8,4) node[pos=.5] {B};
\draw [blue, ultra thick] (8.1,0) rectangle (10,4) node[pos=.5] {C};

\draw [thin, gray, -stealth] (0,0) -- (10,0);
\foreach \x in {0,...,10} {%
    \draw ($(\x,0) + (0,-2pt)$) -- ($(\x,0) + (0,2pt)$)
        node [below] {$\x$};
}
\end{tikzpicture}

\End{multicols}

---

# Preemptive scheduling

* Previous schedulers (FIFO, SJF) are non-preemptive. Non-preemptive schedulers
  only switch to another process if the current process gives up the CPU
  voluntarily.
* Preemptive schedulers may take control of the CPU at any time, switching to
  another process according to the scheduling policy.
* New scheduler: shortest time to completion first (STCF), always run the job
  that will complete the fastest.

---

# Preemptive scheduling: STCF

\Begin{multicols}{2}

* Tasks B, C now arrive at 1
* Average turnaround
    * `(2+4+10)/3 = 5.3`
* Average response
    * `(0+0+2)/3 = 0.7`

\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick] (0,0) rectangle (1,4) node[pos=.5] {A};
\draw [green, ultra thick] (1.1,0) rectangle (3,4) node[pos=.5] {B};
\draw [blue, ultra thick] (3.1,0) rectangle (5,4) node[pos=.5] {C};
\draw [orange, ultra thick] (5.1,0) rectangle (10,4) node[pos=.5] {A};


\draw [thin, gray, -stealth] (0,0) -- (10,0);
\foreach \x in {0,...,10} {%
    \draw ($(\x,0) + (0,-2pt)$) -- ($(\x,0) + (0,2pt)$)
        node [below] {$\x$};
}
\end{tikzpicture}

\End{multicols}

---

# Next metric: response time

* So far, we have optimized for turnaround time (i.e., completing the tasks as
  fast as possible).

* On an interactive system, response time is equally important, i.e., how long
  it takes until a task is scheduled.

---

# Turnaround versus response time

\Begin{multicols}{2}

* Tasks A (2,0) and B (1, 1)
* B turnaround: 2
* B response time: 1

\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick] (0,0) rectangle (2,4) node[pos=.5] {A};
\draw [green, ultra thick] (2.1,0) rectangle (3,4) node[pos=.5] {B};


\draw [thin, gray, -stealth] (0,0) -- (10,0);
\foreach \x in {0,...,10} {%
    \draw ($(\x,0) + (0,-2pt)$) -- ($(\x,0) + (0,2pt)$)
        node [below] {$\x$};
}
\end{tikzpicture}

\End{multicols}


---

# Round robin (RR)

* Previous schedulers optimize for turnaround.

* Optimize response time: alternate ready processes every fixed-length
  time slice.

---

# Round robin

\Begin{multicols}{2}

* Tasks A, B, C (3, 0)
* Average response time
    * (0+1+2)/3 = 1
* Compare to FIFO where average response time is 3

\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick] (0,0) rectangle (1,4) node[pos=.5] {A};
\draw [green, ultra thick] (1.1,0) rectangle (2,4) node[pos=.5] {B};
\draw [blue, ultra thick] (2.1,0) rectangle (3,4) node[pos=.5] {C};

\draw [orange, ultra thick] (3.1,0) rectangle (4,4) node[pos=.5] {A};
\draw [green, ultra thick] (4.1,0) rectangle (5,4) node[pos=.5] {B};
\draw [blue, ultra thick] (5.1,0) rectangle (6,4) node[pos=.5] {C};

\draw [orange, ultra thick] (6.1,0) rectangle (7,4) node[pos=.5] {A};
\draw [green, ultra thick] (7.1,0) rectangle (8,4) node[pos=.5] {B};
\draw [blue, ultra thick] (8.1,0) rectangle (9,4) node[pos=.5] {C};


\draw [thin, gray, -stealth] (0,0) -- (10,0);
\foreach \x in {0,...,10} {%
    \draw ($(\x,0) + (0,-2pt)$) -- ($(\x,0) + (0,2pt)$)
        node [below] {$\x$};
}
\end{tikzpicture}

\End{multicols}

---

# Scheduling assumptions

* ~~Each job runs for the same amount of time~~
* ~~All jobs arrive at the same time~~
* ~~All jobs only use the CPU (no I/O)~~
* Run-time of jobs is known

---

# I/O awareness

* So far, the scheduler only considers preemptive events (i.e., the timer runs
  out) or process termination to reschedule.

* If the scheduler is aware of I/O (e.g., loading data from disk) then
  another process can execute until the data is fetched. I/O operations are
  incredibly slow and can be carried out asynchronously.

---

# Scheduling assumptions

* ~~Each job runs for the same amount of time~~
* ~~All jobs arrive at the same time~~
* ~~All jobs only use the CPU (no I/O)~~
* ~~Run-time of jobs is known~~

---

# Advanced scheduling: multi-level feedback queue (MLFQ)

* Goal: general purpose scheduling

The scheduler must support both long running background
tasks (batch processes) and low latency foreground tasks (interactive
processes).

* Batch process: response time not important, cares for long run times
  (reduce the cost of context switches, cares for lots of CPU, not when)
* Interactive process: response time critical, short bursts
  (context switching cost not important, not much CPU needed but frequently)

---

# MLFQ: basics

***Approach:*** multiple levels of round robin

* Each level has higher priority and preempts all lower levels
* Process at higher level will always be scheduled first
* High levels have short time slices, lower levels run for longer

\begin{center}
\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick](0,0) rectangle (1,1) node[pos=.5] {};
\draw [green, ultra thick](1.1,0) rectangle (2,2) node[pos=.5] {};
\draw [blue, ultra thick](2.1,0) rectangle (3,3) node[pos=.5] {};
\draw [red, ultra thick](3.1,0) rectangle (4,4) node[pos=.5] {};

\node at (2.5, -1) {Decreasing Priority};
\draw [ultra thick, ->](0,-0.5) -- (4,-0.5);

\node [rotate=90] at (-1, 2) {Time Slice};
\draw [ultra thick, ->](-0.5,0) -- (-0.5,4);

\end{tikzpicture}
\end{center}

Set of rules adjusts priorities dynamically.

* Rule 1: if `priority(A) > priority(B)` then A runs.
* Rule 2: if `priority(A) == priority(B)` then A, B run in RR


---

# MLFQ: priority adjustments

Goal: use past behavior as predictor for future behavior.

* Rule 3: processes start at top priority
* Rule 4: if process uses up whole time slice, demote it to lower priority

---

# MLFQ challenges: starvation

Low priority tasks may never run on a busy system.

* Rule 5: periodically move all jobs to the topmost queue

---

# MLFQ challenges: gaming the scheduler

High priority process could yield before its time slice is up, remaining
at high priority.

* Rule 4': account for total time at priority level (and not just time
  of the last time slice)

---

# MLFQ summary

* Rule 1: if `priority(A) > priority(B)` then A runs.
* Rule 2: if `priority(A) == priority(B)` A, B run in RR
* Rule 3: new processes start with top priority
* Rule 4: demote process to lower priority after whole time slice is used
* Rule 5: periodically move all jobs to the topmost queue

\begin{center}
\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick](0,0) rectangle (1,1) node[pos=.5] {};
\draw [green, ultra thick](1.1,0) rectangle (2,2) node[pos=.5] {};
\draw [blue, ultra thick](2.1,0) rectangle (3,3) node[pos=.5] {};
\draw [red, ultra thick](3.1,0) rectangle (4,4) node[pos=.5] {};

\node at (2.5, -1) {Decreasing Priority};
\draw [ultra thick, ->](0,-0.5) -- (4,-0.5);

\node [rotate=90] at (-1, 2) {Time Slice};
\draw [ultra thick, ->](-0.5,0) -- (-0.5,4);

\end{tikzpicture}
\end{center}

---

# CFS: Completely Fair Scheduler


* Idea: each task runs in parallel and consumes equal CPU share
* Approach: calculate time process receives on ideal processor
* Example: assume 4 processes are ready, so they would receive 1/4 of the
  CPU each (add this time to the book keeping)

\begin{center}
\begin{tikzpicture}[scale=0.5]

\draw [orange, ultra thick](0,0) rectangle (6,8) node[pos=.5] {};
\draw [green, ultra thick](7,0) rectangle (13,8) node[pos=.5] {};
\draw [blue, ultra thick](14,0) rectangle (20,8) node[pos=.5] {};

\node at (3, 7.5) {Single Task};
\node at (3, -0.5) {100\% CPU};
\draw[->,decorate,decoration={snake,amplitude=.4mm,segment length=2mm,post length=1mm}](3,6) -- (3,1.5);

\node at (10, 7.5) {Two Tasks};
\node at (10, -0.5) {50\% CPU/Task};
\draw [dashed](10,0.5) -- (10,6.5);
\draw[->,decorate,decoration={snake,amplitude=.4mm,segment length=2mm,post length=1mm}](8.5,6) -- (8.5,1.5);
\draw[->,decorate,decoration={snake,amplitude=.4mm,segment length=2mm,post length=1mm}](11.5,6) -- (11.5,1.5);

\node at (17, 7.5) {Four Tasks};
\draw [dashed](15.5,0.5) -- (15.5,6.5);
\draw [dashed](17,0.5) -- (17,6.5);
\draw [dashed](18.5,0.5) -- (18.5,6.5);
\node at (17, -0.5) {25\% CPU/Task};
\draw[->,decorate,decoration={snake,amplitude=.4mm,segment length=2mm,post length=1mm}](14.75,6) -- (14.75,1.5);
\draw[->,decorate,decoration={snake,amplitude=.4mm,segment length=2mm,post length=1mm}](16.25,6) -- (16.25,1.5);
\draw[->,decorate,decoration={snake,amplitude=.4mm,segment length=2mm,post length=1mm}](17.75,6) -- (17.75,1.5);
\draw[->,decorate,decoration={snake,amplitude=.4mm,segment length=2mm,post length=1mm}](19.25,6) -- (19.25,1.5);

\end{tikzpicture}
\end{center}

---

# CFS: virtual time

> On real hardware, we can run only a single task at once, so we have to
> introduce the concept of "virtual runtime."  The virtual runtime of a task
> specifies when its next timeslice would start execution on the ideal
> multi-tasking CPU described above.  In practice, the virtual runtime of a task
> is its actual runtime normalized to the total number of running tasks.

* CFS keeps track of how long each process should have executed on an ideal
  processor.
* For each time slice, it calculates the fraction each process would have
  received and keeps these balances in a tree.
* The process with the highest balance is then scheduled

* Linux used an O(1) scheduler based on multi-level feedback queues but switched
  to a [completely fair scheduler in 2007](https://www.kernel.org/doc/Documentation/scheduler/sched-design-CFS.txt)

---

# CFS: implementation

* Implementation: keep all processes in a red-black tree, sorted by maximum
  execution time (keep track of their positive balance)
* Scheduling
    * Schedule leftmost process (the one with the highest balance)
    * If the process exits, remove it from the scheduling tree
    * On interrupt (end of time slice or I/O), reinsert the process into the tree at its new position
    * Repeat

---

# Flexible scheduling

![Scheduling](./figures/12-scheduling.jpg)

---

# Summary

* Context switch and preemption are fundamental mechanisms that allow the
  OS to remain in control and to implement higher level scheduling policies.
* Schedulers need to optimize for different metrics: utilization, turnaround,
  response time, fairness and forward progress
    * FIFO: simple, non-preemptive scheduler
    * SJF: non-preemptive, prevents process jams
    * STFC: preemptive, prevents jams of late processes
    * RR: preemptive, great response time, bad turnaround
    * MLFQ: preemptive, most realistic
    * CFS: fair scheduler by virtualizing time
* Insight: past behavior is good predictor for future behavior

Don't forget to fill out the Moodle quiz and to submit lab0!
