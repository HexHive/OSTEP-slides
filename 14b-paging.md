---
title   : CS-323 Operating Systems
subtitle: Q&A Virtual Memory (Paging and Swapping)
author  : Mathias Payer
date    : EPFL, Fall 2019
---

# Questions about the paging class

* What is the difference between virtual and physical address space
* What is a page?
* Page table entries

---

# Address spaces

* The address space encapsulates all addressable memory
* A system has a certain amount of physical memory, this results in the available physical memory
* Address spaces may be of different sizes. Virtual and physical address space must not be the same size
* The physically present memory is smaller than the physical address space size

Example: modern 64-bit CPUs have a 48-bit virtual address space (in 64 bit pointers) and map to 48 bit of physical address space

---

# Page

* A page is the minimal unit to break up an address space
    * For processes (virtual address space) this is called a page (or virtual page)
    * As part of the physical address space this is called a frame (or physical page)

---

# Paging

\begin{tikzpicture}

\draw [ultra thick, fill=gray] (0,0) rectangle (2,1);
\draw [ultra thick, fill=green] (0,1) rectangle (2,2);
\draw [ultra thick, fill=teal] (0,2) rectangle (2,3);
\draw [ultra thick, fill=green] (0,3) rectangle (2,4);
\draw [ultra thick, fill=gray] (0,4) rectangle (2,5);
\draw [ultra thick, fill=gray] (0,5) rectangle (2,6);
\node at (1, 7) {Process A};
\node at (1, 6.5) {(logical view)};

\draw [ultra thick, fill=gray] (4,0) rectangle (6,1);
\draw [ultra thick, fill=gray] (4,1) rectangle (6,2);
\draw [ultra thick, fill=gray] (4,2) rectangle (6,3);
\draw [ultra thick, fill=green] (4,3) rectangle (6,4);
\draw [ultra thick, fill=gray] (4,4) rectangle (6,5);
\draw [ultra thick, fill=teal] (4,5) rectangle (6,6);
\draw [ultra thick, fill=gray] (4,6) rectangle (6,7);
\draw [ultra thick, fill=gray] (4,7) rectangle (6,8);
\node at (7.5, 0.2) {(physical view)};

\draw [ultra thick, ->] (2, 1.5) -- (4, 3.5);
\draw [ultra thick, ->] (2, 3.5) -- (4, 3.5);
\draw [ultra thick, ->] (2, 2.5) -- (4, 5.5);

\end{tikzpicture}

---

# Paging: address translation

* How can the MMU translate virtual to physical addresses?
    * High order bits designate page number
    * Low order bits designate offset in page
    * Note: size of virtual and physical address space may be different

\begin{tikzpicture}
\draw [ultra thick, fill=blue] (0,4) rectangle (5,5) node[pos=.5] {page number};
\draw [ultra thick, fill=gray] (5,4) rectangle (7,5) node[pos=.5] {page offset};

\draw [ultra thick] (1.5,2) rectangle (3.5,3) node[pos=.5] {translation};

\draw [ultra thick, fill=teal] (0,0) rectangle (5,1) node[pos=.5] {frame number};
\draw [ultra thick, fill=gray] (5,0) rectangle (7,1) node[pos=.5] {page offset};


\draw [ultra thick, ->] (6, 4) -- (6, 1);

\draw [ultra thick, ->] (2.5, 4) -- (2.5, 3);
\draw [ultra thick, ->] (2.5, 2) -- (2.5, 1);

\end{tikzpicture}


---

# Internal fragmentation: struct

```.C
struct foo {
  char a;   // padded to 8b, 7b int frag
  double b; // 8 b
}
```
