---
title   : CS-323 Operating Systems
subtitle: Virtual Memory (Segmentation)
author  : Mathias Payer
date    : EPFL, Fall 2019
---

# Q&A from the feedback round

* Extra material
* Project concepts
* Heap allocators

---

# Extra material

* The class follows the Operating Systems: Three Easy Pieces (OSTEP)
* Relevant chapters are referenced on second slide (on overview)
* Read the chapters along with the slides to prepare

---

# Project concepts

* The project covers concepts, policies, and mechanisms
* You are supposed to implement complex aspects of an OS
* Leverage the existing code base (read and understand it)
* Extend it according to the questions (and given the relevant pointers)

---

# Dynamic data structure: heap

A heap of randomly allocated memory objects with *statically unknown size* and
*statically unknown allocation patterns*. The size and lifetime of each
allocated object is unknown.

* How would you manage such a data structure?
    * List of free blocks
    * `alloc`: take a free block, split, put remainder back on free list
    * `free`: add block to free list (option: search for adjacent blocks, merge)

---

# Allocator: question 1

> Consider a heap allocator that implements a first-fit allocation approach with
> re-usable memory regions. It uses a singly-linked list to track free memory
> regions in the heap area. It also supports merging adjacent free regions. What
> is the worst-case complexity of the free() operation (n is the number of nodes
> in the list)?

. . .

O(n). Merging a free block with adjacent blocks requires inserting it into the
linked list where the nodes in the list are sorted by the addresses of their
memory regions, then checking if the nodes represent contiguous memory regions
(2 checks). Inserting a node into a sorted list is a O(n) operation.

---

# Allocator: question 2

> Consider a heap allocator that implements a first-fit allocation approach with
> re-usable memory regions. It uses a singly-linked list to track free memory
> regions in the heap area. It DOES NOT support merging adjacent free regions.
> What is the worst-case complexity of the free() operation (n is the number of
> nodes in the list)?

. . .

O(1). Since merging is not supported, it suffices to insert the freed region at
the head of the linked list, which is a O(1) operation.
