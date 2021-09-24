---
title   : CS-323 Operating Systems
subtitle: Filesystem (2/2)
author  : Mathias Payer
date    : EPFL, Fall 2019
---

# Topics covered in this lecture

* Filesystem implementation
* Special requirements (write back)
* Crash resistance
* Journaling

This slide deck covers chapters 42, 43 in OSTEP.

---

# Different names for different use cases

* inode/device id (`53135/2`)
    * Unique internal name
    * Records metadata about the file (size, permissions, owner)
* path (`/foo/bar/baz`)
    * Human readable name
    * Organizes files in a hierarchical layout
* file descriptor (`5`)
    * Process internal view
    * Avoids frequent path to inode traversal
    * Remembers offset for next read/write

---

# Common file API

* `int open(char *path, int flag, mode_t mode)`
* `size_t read(int fd, char *buf, size_t nbyte)`
* `size_t write(int fd, char *buf, size_t nbyte)`
* `int close(int fd)`

What kind of on disk data structures do we need?
How is data accessed?

---

# Virtual File System

* File systems (EXT4, NTFS, FAT) use different data structures
* A Virtual File System (VFS) abstracts from the real filesystem
* VFS abstracts the FS as objects with specific operations
    * Superblock (mount): a life filesystem
    * File (open): a file opened by a process ("open file description")
    * Directory entry cache: speeds up path to inode translation
    * Inode (lookup): a filesystem object (e.g., file or directory)
* System call logic (open, seek, ...) maps to VFS operations
* When implementing a new FS, implement the VFS API
* System calls are now independent of FS implementation

---

# Challenge: renaming files

* How would you implement `rename`?

. . .

* Renaming only changes the name of the file
* Directory contains the name of the file
* No data needs to be moved, inode remains unchanged

. . .

* Note, you may need to move the data if it is on another disk/partition!

---

# Filesystem implementation

* A filesystem is an exercise in data management
* Given: a large set (N) of blocks
* Need: data structures to encode (i) file hierarchy and (ii) per file metadata
    * Overhead (metadata size versus file data) should be low
    * Internal fragmentation should be low
    * File contents must be accessed efficiently (external fragmentation, number of metadata accesses)
    * Define operations for file API

. . .

* Many different choices are possible!
    * Similar to virtual memory!

---

# Allocating file data

* Contiguous
* Linked blocks (blocks end with a next pointer)
* File-allocation tables (table that contains block references)
* Indexed (inode contains data pointers)
* Multi-level indexed (tree of pointers)

For each approach, think about fragmentation, ability to grow/shrink files, sequential access performance, random access performance, overhead of meta data.

---

# File allocation: contiguous

Each file is allocated contiguously

\begin{tikzpicture}
\node (F1) at (0,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (0.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (1,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (F1) at (1.5,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (F1) at (2,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (F1) at (2.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (3,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {B};
\node (F1) at (3.5,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {B};
\node (F1) at (4,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (4.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\end{tikzpicture}

. . .

* Terrible external fragmentation (OS must anticipate)
* Likely unable to grow file
* Excellent read and seek performance
* Small overhead for metadata
* Great for read-only file systems (CD/DVD/BlueRay)

---

# File allocation: linked blocks

Each file consists of a linked list of blocks

\begin{tikzpicture}
\node (F1) at (0,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (0.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (A1) at (1,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (A2) at (1.5,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (B2) at (2,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {B};
\node (F1) at (2.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (B1) at (3,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {B};
\node (A3) at (3.5,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (F1) at (4,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (4.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\draw [->,thick] (A1.north) to [out=30,in=150] (A2.north);
\draw [->,thick] (A2.north) to [out=30,in=150] (A3.north);
\draw [->,thick] (B1.south) to [out=-150,in=-30] (B2.south);

\end{tikzpicture}

. . .

* No external fragmentation
* Files can grow easily
* Reasonable read cost (depending on layout), high seek cost
* One pointer per block metadata overhead

---

# File allocation: File Allocation Table (FAT)

Idea: keep linked list information in a single table.
Instead of storing the next pointer at the end of the block, store all next pointers in a central table

\begin{tikzpicture}
\node (F1) at (0,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (0.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (A1) at (1,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (A2) at (1.5,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (B2) at (2,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {B};
\node (F1) at (2.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (B1) at (3,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {B};
\node (A3) at (3.5,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (F1) at (4,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (4.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (0,-0.5) {-1};
\node (F1) at (0.5,-0.5) {-1};
\node (A1) at (1,-0.5) {3};
\node (A2) at (1.5,-0.5) {7};
\node (B2) at (2,-0.5) {0};
\node (F1) at (2.5,-0.5) {-1};
\node (B1) at (3,-0.5) {4};
\node (A3) at (3.5,-0.5) {0};
\node (F1) at (4,-0.5) {-1};
\node (F1) at (4.5,-0.5) {-1};
\node (F1) at (6.1,-0.5) {Block pointer};

\node (F1) at (0,0.5) {0};
\node (F1) at (0.5,0.5) {1};
\node (A1) at (1,0.5) {2};
\node (A2) at (1.5,0.5) {3};
\node (B2) at (2,0.5) {4};
\node (F1) at (2.5,0.5) {5};
\node (B1) at (3,0.5) {6};
\node (A3) at (3.5,0.5) {7};
\node (F1) at (4,0.5) {8};
\node (F1) at (4.5,0.5) {9};
\node (F1) at (6.1,0.5) {Block number};

\end{tikzpicture}

. . .

* No external fragmentation
* Files can grow easily
* Reasonable read and seek cost
* One pointer per block metadata overhead

---

# File allocation: indexed

Idea: metadata contains an array of block pointers

\begin{tikzpicture}
\node (F1) at (0,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (0.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (A1) at (1,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (A2) at (1.5,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (B2) at (2,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {B};
\node (F1) at (2.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (B1) at (3,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {B};
\node (A3) at (3.5,0) [draw,thick, fill=gray, rectangle,minimum width=0.5cm, minimum height=0.5cm] {A};
\node (F1) at (4,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (F1) at (4.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\node (F1) at (0,0.5) {0};
\node (F1) at (0.5,0.5) {1};
\node (A1) at (1,0.5) {2};
\node (A2) at (1.5,0.5) {3};
\node (B2) at (2,0.5) {4};
\node (F1) at (2.5,0.5) {5};
\node (B1) at (3,0.5) {6};
\node (A3) at (3.5,0.5) {7};
\node (F1) at (4,0.5) {8};
\node (F1) at (4.5,0.5) {9};
\node (F1) at (6.1,0.5) {Block number};

\node (F1) at (8,-0.2) [draw,thick, rectangle,minimum width=3.5cm, minimum height=0.5cm] {File A: 2, 3, 7, -1};
\node (F1) at (8,-1.2) [draw,thick, rectangle,minimum width=3.5cm, minimum height=0.5cm] {File B: 6, 4, -1, -1};

\end{tikzpicture}

. . .

* No external fragmentation
* Files can grow easily up to maximum size
* Reasonable read and low seek cost
* *Large metadata overhead* (wastes space as most files are small)

---

# File allocation: multi-level indexing (1/3)

Idea: have a mix of direct, indirect, double indirect, and triple indirect pointers

\begin{tikzpicture}
\node (D1) at (0,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (D2) at (0.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (D3) at (1,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (D4) at (1.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (D5) at (2,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (D6) at (2.5,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (D7) at (3,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (I1) at (3.5,0) [draw,thick, fill=red, rectangle,minimum width=0.5cm, minimum height=0.5cm] {I};
\node (II1) at (4,0) [draw,thick, fill=blue, rectangle,minimum width=0.5cm, minimum height=0.5cm] {D};
\node (III1) at (4.5,0) [draw,thick, fill=green, rectangle,minimum width=0.5cm, minimum height=0.5cm] {T};

\node (DB1) at (0,-1) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (DB2) at (0.8,-1) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (DB3) at (3,-1) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\draw [->,thick] (D1.south) to (DB1.north);
\draw [->,thick] (D2.south) to (DB2.north);
\draw [->,thick] (D7.south) to (DB3.north);

\node (IB1) at (3.5,-2) [draw,thick, fill=red, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\node (IDB1) at (3.5,-3) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (IDB2) at (4.2,-3) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\draw [->,thick] (I1.south) to (IB1.north);
\draw [->,thick] (IB1.south) to (IDB1.north);
\draw [->,thick] (IB1.south) to (IDB2.north);


\node (IIB1) at (5,-2) [draw,thick, fill=blue, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\node (IB2) at (5,-4) [draw,thick, fill=red, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\node (IDB3) at (5,-5) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (IDB4) at (5.8,-5) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\node (IB3) at (7,-4) [draw,thick, fill=red, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\node (IDB5) at (7,-5) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};
\node (IDB6) at (7.8,-5) [draw,thick, fill=brown, rectangle,minimum width=0.5cm, minimum height=0.5cm] {};

\draw [->,thick] (II1.south) to (IIB1.north);

\draw [->,thick] (IIB1.south) to (IB2.north);
\draw [->,thick] (IIB1.south) to (IB3.north);

\draw [->,thick] (IB2.south) to (IDB3.north);
\draw [->,thick] (IB2.south) to (IDB4.north);

\draw [->,thick] (IB3.south) to (IDB5.north);
\draw [->,thick] (IB3.south) to (IDB6.north);

\node (IIIB1) at (8,-2) [draw,thick, fill=green, rectangle,minimum width=0.5cm, minimum height=0.5cm] {triple indirect};
\draw [->,thick] (III1.south) to (IIIB1.north);

\end{tikzpicture}

---

# File allocation: multi-level indexing (2/3)

Idea: have a mix of direct, indirect, double indirect, and triple indirect pointers

```.C
struct inode {
  umode_t        i_mode;
  unsigned short i_opflags;
  kuid_t         i_uid;
  kgid_t         i_gid;
  unsigned int   i_flags;
  ...
  // direct pointers to data blocks
  struct dblock  *direct[10];
  // block of N ptrs to data blocks
  struct dblock  **dindirect;
  // block of N ptrs to each N ptrs to data blocks
  struct dblock  ***tindirect; 
};
```

---

# File allocation: multi-level indexing (3/3)

Idea: have a mix of direct, indirect, double indirect, and triple indirect pointers

* No external fragmentation
* Files can grow easily up to maximum size
* Reasonable read and low seek cost
* Low metadata overhead but needs extra reads for indirect/double indirect access

---

# Simple FS

\begin{tikzpicture}
\node (F1) at (0,0) [draw,thick, rectangle,minimum width=0.5cm, minimum height=0.5cm] {S};
\node (F1) at (0.5,0) [draw,thick, fill=teal, rectangle,minimum width=0.5cm, minimum height=0.5cm] {i};
\node (F1) at (1,0) [draw,thick, fill=cyan, rectangle,minimum width=0.5cm, minimum height=0.5cm] {d};
\foreach \x in {0,...,4} {
    \node (F1) at (0.5*\x+1.5,0) [draw,thick,fill=green,rectangle,minimum width=0.5cm, minimum height=0.5cm] {I};
}
\foreach \x in {0,...,8} {%
    \node (F1) at (0.5*\x+3.5,0) [draw,thick,fill=blue,rectangle,minimum width=0.5cm, minimum height=0.5cm] {D};
}
\end{tikzpicture}

* Superblock (S): file system metadata
* Bitmaps (i, d): indicates free blocks
* Inodes (I): hold file/directory metadata, reference data blocks
* Data blocks (D): file contents, referenced by an inode

The inode size may be different (smaller) from the data block size.

---

# Simple FS: superblock

* The superblock stores the characteristics of the filesystem
* What do you store in the superblock?

. . .

* Magic number and revision level
* Mount count and maximum mount count
* Block size of the filesystem (1, 2, 4, 8, 16, 32, 64K for ext4)
* Name of the filesystem
* Number of inodes/data blocks
* Number of free inodes/data blocks
* Number of "first" inode (i.e., root directory)

---

# Simple FS: inode

* The inode stores all file metadata
* What would you store in an inode?

. . .

* File type
* File uid, gid
* File permissions (for user, group, others)
* Size
* Access time
* Create time
* Number of links

---

# Maximum file size

* Maximum file size is related to
    * Block size
    * Number of direct inodes
    * Number of indirect inodes
    * Number of double indirect inodes
    * Number of triple indirect inodes
* `blocksize * (direct + inodeblock + inodeblock^2 + inodeblock^3)`

---

# Directories

* Directories are special files (`inode->type`)
* Store a set of file name to inode mappings
* Special entries `.` for current directory and `..` for parent directory

---

# File operation: create /foo/bar

* Read root inode (locate directory data)
* Read root data (read directory)
* Read foo inode (locate directory data)
* Read foo data (read directory)
* Read/write inode bitmap (allocate inode)
* Write foo data (add file name)
* Read/write bar inode (create file)
* Write foo inode (extend data block, update time)

---

# File operation: open /foo/bar

* Read root inode (locate directory data)
* Read root data (read directory)
* Read foo inode (locate directory data)
* Read foo data (read directory)
* Read bar inode (read file metadata)

---

# File operation: write to /foo/bar

* First: `open("/foo/bar")`
* Read bar inode (read file metadata)
* Read/write data bitmap (allocate data blocks)
* Write bar data (write data)
* Write bar inode (update inode)

---

# File operation: read from /foo/bar

* First: `open("/foo/bar")`
* Read bar inode (read file metadata)
* Read bar data (read data)
* Write bar inode (update time)

---

# File operation: close /foo/bar

* No disk I/O

---

# File operation: observations

* Path traversal and translation is costly
    * Reduce number lookups (file descriptors!)
    * Introduce caching (dcache)
* Lookup aside, operations are cheap and local

---

# Challenge: atomic file update

* Assume you want to update `important.txt` atomically

. . .

* If the application or the system crashes, the old version must remain
    * Write data to `./gener8 > important.txt.tmp`
    * Flush data to disk: `fsync important.txt.tmp`
    * Rename atomically: `mv important.txt.tmp important.txt`, replacing it
* What could still go wrong?

. . .

* File system metadata may not be written back to disk!

---

# Crash resistance

* Power loss during writing
* Mechanical failure
* Magnetization failure
* [Mechanical destruction (link)](https://www.youtube.com/watch?v=-bpX8YvNg6Y&t=1815)

---

# Redundancy

> Given A and B. If knowing A allows you to infer some or all values of B then
> there is redundancy between A and B.

* RAID1: mirrored disks (complete redundancy)
* RAID5 or 6: parity blocks (partial redundancy)

---

# Redundancy in a filesystem

* Directory entries and inode table
* Directory entries and inode link count
* Data bitmap and inode pointers
* Data bitmap and group descriptor (for sets of blocks)
* Inode file size and inode/indirect pointers

---

# Advantages of redundancy

* Can improve reliability (recover from failures)
* Can improve performance (easier to read file size from inode than parsing
  the full structure)
* Requires more storage (inefficient encoding)
* Requires consistent updates (all sides must agree)

---

# Consistency

> Redundant data must be consistent to ensure correct functionality

* Keeping redundant data consistent is challenging
* Filesystem may perform several writes to redundant blocks
* The sequence of writes is not atomic
* Interrupts due to power loss, kernel bugs, hardware failure

---

# Consistency scenario (1/2)

* Filesystem appends to a file
* Must write to inode, data bitmap, data block
* What happens if only some writes succeed?
    * Bitmap
    * Data
    * Inode
    * Bitmap and data
    * Bitmap and inode
    * Data and inode

---

# Consistency scenario (2/2)

* Filesystem appends to a file
* Must write to inode, data bitmap, data block
* What happens if only some writes succeed?
    * Bitmap: lost block
    * Data: lost data write (i.e., file is not updated)
    * Inode: references garbage (another file may use)
    * Bitmap and data: lost block
    * Bitmap and inode: reference garbage (from previous usage)
    * Data and inode: another file may grab the block
* How would you order the writes?

. . .

* Data (nothing bad happens), bitmap (lost block is detectable), then inode

---

# Consistency through filesystem check (1/4)

* After a certain number of mount operations (remember the mount count in the
  super block?) or after a crash, check the consistency of the filesystem!
* Hundreds of consistency checks across different fields
    * Do superblocks match?
    * Are all '.' and '..' linked correctly?
    * Are link counts equal to number of directory entries?
    * Do different inodes point to the same block?

---

# Consistency through filesystem check (2/4)

* Two directory entries point to the same inode, link count is 1
* Inode link count is 1 but no directory links this file
* A referenced block is marked as free in the bitmap
* Two inodes reference the same data block
* An inode points to an inexistent block

---

# Consistency through filesystem check (3/4)

* Two directory entries point to the same inode, link count is 1
    * Update the link count to 2    
* Inode link count is 1 but no directory links this file
    * Link the file in a `lost+found` directory
* A referenced block is marked as free in the bitmap
    * Update the bitmap to 1
* Two inodes reference the same data block
    * Make a copy of the data block
* An inode points to an inexistent block
    * Remove the reference

* Are these operations correct?

---

# Consistency through filesystem check (4/4)

* The file system is inconsistent, so all we do is best effort!
* It's not obvious how to fix filesystem corruption
* Correct state is unkown, just that it is inconsistent
* FSCK is slow and may take hours (must read full disk)
* Are there better approaches?

---

# Consistency through journaling

* Goal: limit the amount of required work after crash
* Goal: get correct state, not just consistent state
* Strategy: atomicity

* Atomicity: being composed of indivisible units
    * *Concurrency*: operations in critical sections are not interrupted
    * *Persistence*: collections of writes are not interrupted by crashes 
      (i.e., either all new or all old data is visible)

---

# Consistency versus correctness

* Given: filesystem in state A, set of writes, resulting in state B
* Assume it crashes somewhere between the writes from A to B
    * Filesystem check (FSCK) gives consistency
    * Atomicity gives A or B

\begin{tikzpicture}

\draw (0,-0.2) [fill=gray] ellipse (3cm and 1.3cm);
\node at (0,0.8) {consistent states};

\draw (0,0) ellipse (4cm and 2cm);
\node at (0,1.8) {all states};

\node at (-1.5,-0.5) [draw,fill=red,circle] (0.5cm) {};
\node at (-1.5,-0.9) {empty};

\node at (0,-0.5) [draw,fill=blue,circle] (0.5cm) {A};
\node at (1.5,-0.5) [draw,fill=green,circle] (0.5cm) {B};


\end{tikzpicture}

---

# Journaling strategy

* Never delete (or overwrite) ANY old data until you have received confirmation
  that ALL new data is committed
    * Add redundancy to fix the problem with redundancy

. . .

![](./figures/33-journaling.jpg){width=350px}

---

# Journaling strategy (1/4)

* Goal update file X with contents Y
    * Write Y, update metadata f(Y)

. . .

* Classic strategy
    * Overwrite f(X) with f(Y), overwrite X with Y
    * Overwrite X with Y, overwrite f(X) with f(Y)
    * Crash in the middle is bad!

. . .

* Journaling strategy
    * Commit Y and f(Y) to journal
    * Update X with Y
    * Update f(X) with f(Y)
    * Delete journal entries
    * Resilient to crash in the middle, journal allows recovery

---

# Journaling strategy (2/4)

* Goal: write 10 to block 0 and 5 to block 1 *atomically*

| Time | Block 0 | Block 1 | Extra | Extra | Extra |
|-----:|--------:|--------:|------:|------:|------:|
|    0 |      12 |       3 |     0 |     0 |     0 |
|    1 |      10 |       3 |     0 |     0 |     0 |
|    2 |      10 |       5 |     0 |     0 |     0 |

. . .

* This does not work! Must not crash between 1 and 2!

---

# Journaling strategy (3/4)

* Goal: write 10 to block 0 and 5 to block 1 *atomically*

| Time | Block 0 | Block 1 | Block 0' | Block 1' | Valid? |
|-----:|--------:|--------:|---------:|---------:|-------:|
|    0 |      12 |       3 |        0 |        0 |      0 |
|    1 |      12 |       3 |       10 |        0 |      0 |
|    2 |      12 |       3 |       10 |        5 |      0 |
|    3 |      12 |       3 |       10 |        5 |      1 |
|    4 |      10 |       3 |       10 |        5 |      1 |
|    5 |      10 |       5 |       10 |        5 |      1 |
|    6 |      10 |       5 |       10 |        5 |      0 |

. . .

* Crash before 3: old data
* Crash after 3: new data (need recovery)
* Crash after 6: new data

---

# Journaling strategy (4/4)

```.C
// Pseudocode, assume we operate on blocks
void recovery() {
    if (*valid == 1) {
        *block0 = *block0p;
        *block1 = *block1p;
        *valid = 0;
        fsync();
    }
}
```

---

# Journaling terminology

* Extra blocks are called 'journal'
* Writes to the journal are a 'journal transaction'
* The valid bit is a 'journal commit block'

---

# Journal optimizations

* Dedicated (small) journal area
* Write barriers
* Checksums
* Circular journal
* Logical journal
* Ordered journal

---

# Journal optimization: small journal

* Allocating a shadow block per data block is wasteful 
    * Recovery cost and lost storage

. . .

* Dedicate a small area of blocks to the journal
    * Store block number along with data
    * At the start of the transaction, mark which blocks are modified
    * Store the data blocks in the journal
    * Commit the transaction

---

# Journal optimization: write barriers

* Enforcing total write order is costly (remember seek cost?)
* Idea: only wait until blocks of writes have completed
    * Wait before journal commit (journal data blocks were written)
    * Wait after journal commit (journal was committed)
    * Wait after data blocks are written (journal can be freed)

---

# Journal optimization: checksums

* Can we get rid of the write barrier after journal commit?
* Idea: replace valid/invalid bit with checksum of written blocks
    * Checksum mismatch: one of the blocks was not written
    * Checksum match: all blocks were committed correctly
* We now only have two write barriers for each transaction
    * After writing the journal (make sure data ended up in journal)
    * Before clearing the journal entry (data was written to disk)

---

# Journal optimization: circular buffer

* After data is written to journal, there is no rush to update/write back
    * Journaled data can be recovered
* Delay journaling for some time for better performance
    * Keep journal transactions in circular buffer
    * Flush when buffer space is used up

---

# Journal optimization: logical journal

* Appending a block to the file causes writes to the data block, the inode,
  the data bitmap
    * Many small writes
    * Writing full blocks to journal is wasteful
* Idea: keep track how data changed (diff between old and new)
    * Logical journals record changes to bytes, not blocks
    * Save lots of journal space
    * Must read original block during recovery

---

# Journal optimization: ordered journal

* How can we avoid writing all data twice?
* Idea: store only metadata in journal
    * Write data to new block
    * Store updates to metadata in logical journal
    * Commit journal (and new data blocks)
    * Update metadata
    * Free journal

---

# Summary

* Filesystem implementation
    * Inodes for metadata
    * Bitmaps for inodes/data blocks
    * Superblock for global metadata
* Crash resistance: filesystem check (FSCK)
* Journaling: keep track of metadata, enforce atomicity
    * All modern filesystems use journaling
    * FSCK still useful due to bitflips/bugs

Don't forget to get your learning feedback through the Moodle quiz!
