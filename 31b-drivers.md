---
title   : CS-323 Operating Systems
subtitle: Drivers and IO Q&A
author  : Mathias Payer
date    : EPFL, Fall 2019
---

# Lab 1 regrade

* Grading seemed odd
    * We used a linear grading scheme where all parts were treated the same, otherwise lack of completion and bugs in part 3 would have been disastrous.
* We'll update the grades later this week

---

# Lab 1 regrade

* It's unfair if many lines need to be changed
    * The lab was supposed to be completed by the first deadline
    * This was an opportunity to fix small mistakes, not a deadline extension
    * The grading scheme is based on balancing between those that already did well and those that have a few mistakes

---

# Lab tests

* There are not enough test cases
* We provided a set of simple test cases
* You were supposed to add your own
* We now include more test cases that cover a broader set of features
* We still use other test cases to test ;)

---

# Midterm difficulty

* Exams are designed to *allow students to show what they know*
* There are more questions than you have time for
* You are not expected to complete all for a good grade
* Focus on/start with those that you know best :)

---

# Midterm difficulty

* We want mock exams
    * Next year there will be!
    * This year there was no time :(

---

# RAID: Redundant Array of Inexpensive Disks

* Idea: build logical disk from (many) physical disks

. . .

* RAID0: Striping (no mirroring or parity)
    * n performance, n capacity, 0/n can fail
* RAID1: Data mirroring (no parity or striping)
    * n performance, (n-1)/n can fail
* RAID2: bit level striping (historic, sync'd, one parity drive)
* RAID3: byte level striping (historic, sync'd, one parity drive)
* RAID4: block level striping (historic, one drive holds parity)
* RAID5: block level striping, distributed parity
    * n performance, n-1 capacity, 1/n can fail
* RAID6: block level striping, distributed parity
    * n performance, n-2 capacity, 2/n can fail

---

# Quiz

* 61/81 completed the quiz.

---

# Quiz question 3, 4, 5

IO speeds vary by many orders of magnitude. NICs in servers might have a peak bandwidth of 100Gbps. 4k video output might be 10-20Gbps. Typical WiFi speeds go up to 100 Mbps. Your keyboard transmits a few bytes per second (depending on your typing speed). As a result, each device uses different IO busses, each of which support different speeds. Your gaming PC (with fancy environmental lights) might have a RGB controller for cycling LEDs through the spectrum of colors every second. W

* What bus does your GPU use? PCIe (see slide #7)
* What 'bus' is DRAM connected to? Memory Bus (see slide #7)
* Which of the following busses would the LED controller use to connect to the CPU? UART (slow speed, little data)

---

# Quiz question 6

Sort the devices in increasing order of speed (specifically bandwidth): USB-2.0 hard-disk (1), UART (2), Keyboard (3), datacenter NIC - Intel 82599 (4), SATA-3 SSD (5), CD-ROM (6)
Hint: UART is a serial IO protocol. Find standard baud rates (transmission rates) online.

* Keyboard
* UART
* CD-ROM
* USB-2.0 hard-disk
* SATA-3 SSD
* datacenter NIC - Intel 82599

---

# Disc drives

\begin{tikzpicture}

\node [draw, circle, ultra thick, minimum width=1cm] at (0,0) {};
\node [draw, circle, ultra thick, minimum width=2cm] at (0,0) {};
\node [draw, circle, ultra thick, minimum width=3cm] at (0,0) {};
\node [draw, circle, ultra thick, minimum width=4cm] at (0,0) {};
\node [draw, circle, ultra thick, minimum width=5cm] at (0,0) {};
\node [draw, circle, ultra thick, minimum width=6cm] at (0,0) {};

\draw [ultra thick] (-3,0) -- (3,0);
\draw [ultra thick] (0,-3) -- (0,3);
\draw [ultra thick, rotate=45] (0,-3) -- (0,3);
\draw [ultra thick, rotate=135] (0,-3) -- (0,3);


\node [draw, fill, color=teal, rectangle, ultra thick, minimum width=4cm, minimum height=0.3cm, rotate=-44] at (3.05,0.2) {};
\node [color=red, thick] at (1.85,1.4) {A};

\end{tikzpicture}

* [Hard drive spinning up](https://www.youtube.com/watch?v=D-EtrC32wrU)
* [Open hard drive spinning up](https://www.youtube.com/watch?v=zMpa4h_SRmI)
* [Floppy drives](https://www.youtube.com/watch?v=Oym7B7YidKs)

---

# Feedback

* Thanks for all the feedback you provide on Moodle/email!
* We want to provide the best learning environment for you!
    * Goal: understand how an OS ticks (see lectures)
    * Goal: become a better programmer (see labs)
    * Goal: learn to abstract but get feedback (see quizzes)

. . .

* Please *ALL* fill out EPFL feedback
    * This is important for me!