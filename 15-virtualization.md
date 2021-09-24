---
subtitle: Virtualization Summary
---

# Virtualization

\begin{tikzpicture}
\draw [fill, orange, ultra thick] (0,0) rectangle (2,4);
\node[text width=3cm, rotate=90] at (1, 2.5) {Virtualization};
\draw [blue, ultra thick] (3,0) rectangle (5,4);
\node[text width=3cm, rotate=90] at (4, 2.5) {Concurrency};
\draw [green, ultra thick] (6,0) rectangle (8,4);
\node[text width=3cm, rotate=90] at (7, 2.5) {Persistence};

\draw [red, ultra thick] (0,4.5) -- (8,4.5);
\draw [red, ultra thick] (0,4.5) -- (4,6);
\draw [red, ultra thick] (4,6) -- (8,4.5);
\node[text width=3cm] at (5, 5) {Security};

\end{tikzpicture}

---

# Virtualization: Summary

\begin{tikzpicture}[level distance=1.5cm,
  level 1/.style={sibling distance=6cm},
  level 2/.style={sibling distance=2.5cm}]
  \node {Virtualization}
    child {node {CPU}
      child {node {Processes}}
      child {node {Scheduling}}
    }
    child {node {Memory}
      child {node {Allocation}}
      child {node {Segmentation}}
      child {node {Paging}}
    };
\end{tikzpicture}

---

# CPU Virtualization: Processes

* Processes are a purely virtual concept
* Separating policies and mechanisms enables modularity
* OS is a server, reacts to requests from hardware and processes
* Processes are isolated from the OS/other processes
    * Processes have no direct hardware access
    * Processes run in virtual memory
    * OS provides functionality through system calls
* A process consists of an address space, associated kernel state (e.g., open
  files, network channels) and one or more threads of execution

---

# CPU Virtualization: Scheduling

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
* Past behavior is good predictor for future behavior

---

# Memory Virtualization: Segmentation

* OS manages access to constrained resources
    * Principle: limited direct execution (bare metal when possible, intercept
      when needed)
    * CPU: time sharing between processes (low switching cost)
    * Memory: space sharing (disk I/O is slow, so time sharing is expensive)
* Programs use dynamic data
    * Stack: program invocation frames
    * Heap: unordered data, managed by user-space library (allocator)
* Time sharing: one process uses all of memory
* Base register: share space, calculate process address through offset
* Base + bounds: share space, limit process' address space
* Segments: movable segments, virtual offsets to segment base

---

# Memory Virtualization: Paging

* Fragmentation: space lost due to internal or external padding
* Paging: MMU fully translates between virtual and physical addresses
    * One flat page table (array)
    * Multi-level page table
    * Pros? Cons? What are size requirements?
* Paging and swapping allows process to execute with only the working set
  resident in memory, remaining pages can be stored on disk

---

# Book chapters

* Virtual CPU (Processes and Threads): OSTEP 4--6
* Virtual CPU (Scheduling): OSTEP 7--10
* Virtual Memory (Segmentation): OSTEP 13--17
* Virtual Memory (Paging and Swapping): OSTEP 18--22

This concludes the first pillar of OS.
