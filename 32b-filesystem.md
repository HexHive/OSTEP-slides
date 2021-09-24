---
title   : CS-323 Operating Systems
subtitle: Filesystem (1/2) Q&A and Class Evaluation
author  : Mathias Payer
date    : EPFL, Fall 2019
---

# Sharing and concurrency is hard!

* Consider file permissions change *after* file is opened
* Consider a file is moved *after* it is opened
* Consider file owner changes *after* it is opened
* A process forks, what happens to open files (e.g., read position in file)
* What happens if two processes concurrently write to the same file?
* ...

---

# The `file` abstraction

* Different perspectives
    * File name (human readable)
    * Inode and device number (persistent ID)
    * File descriptor (process view)
* More later in today's lecture!

---

# Quizzo: cat!

![](./figures/33-nyancat.gif){width=200px}

* It is possible for `cat` to run infinitely
    `cat /dev/zero`

---

# Quizzo: matching abstractions

Consider Humans (H), Processes (P), Operating Systems (OS), and Disks (D).
Which of the following matching between the above and FS abstractions is correct?

```
1 (H, names)
2 (P, file descriptors)
3 (OS, inodes)
4 (D, blocks)
```

---

# Quizzo: APIs and abstractions

Complete the following sentence: the functions create, read, and rmdir work on
names for the first and the third, and file descriptors for the second

---

# Feedback discussion

* 52/81 answered

![](./figures/33-thankyou.jpg){width=200px}

. . .

* Overall the course is good: 
    * Strongly agree: 15 / 29%
    * Agree 25 / 48%
    * Disagree 5 / 10%
    * Strongly disagree 6 / 12%

---

# Feedback discussion

* Midterm
* Labs

---

# Feedback discussion: midterm

* Not enough study material
    * More next year (at least one mock exam)
    * For this year: adapting quiz questions to make them more exam like

* Questions too tough / not multiple choice
    * Will specify more clearly for next year
    * Final: same as midterm

* Not enough time
    * My exams are designed to *help you show what you know*
    * Focus on those questions that you can answer well!
    * Spend 5 min to read through the questions

---

# Feedback discussion: labs

* We will integrate a session on writing test cases into lab 0 (next year)
* We've improved the lab descriptions and highlight what and how you need to implement it
* We've added more test cases

* Asking great questions is an art: help us by saying which part of the handout was unclear!

---

# Feedback discussion: labs

* lab 0: refresh C programming (next year: add tests)
* lab 1: difference between mechanism and policy, policy can be changed without touching (or having to understand) the mechanism
* lab 2: concurrency primitives and working with different threads, interaction between architecture primitives and high level constructs
* lab 3: managing unstructured data (blocks on disk)
* lab 4: understanding security flaws, then finding and fixing them

---

# Feedback discussion

![](./figures/33-minion.jpg){width=300px}

* Thanks again and keep the feedback coming!