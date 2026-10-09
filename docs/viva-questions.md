# MediCore DSA Project Viva Questions & Answers

This document serves as an exhaustive guide for professors, evaluators, and examiners during project demonstrations and viva sessions.

---

### Q1. What is the architecture of your application, and why didn't you write it purely in Python or JavaScript?
**Answer:**
MediCore uses a decoupled client-server architecture:
- **Frontend:** React 18 SPA built with Vite for an interactive, professional healthcare SaaS UI.
- **Backend:** High-performance C++17 microservice using the Crow framework and standalone Asio.
- **Persistence:** SQLite3 relational database with prepared statements and foreign key integrity.

We explicitly implemented the backend in C++ because C++ offers direct hardware control, deterministic memory management, zero-overhead abstractions, and is the industry gold standard for systems programming and DSA implementation. Implementing custom data structures in C++ proves our understanding of pointers, dynamic memory allocation, and algorithmic mechanics without relying on language-level garbage collection.

---

### Q2. How is patient lookup optimized, and what collision resolution technique is used?
**Answer:**
Patient lookups are powered by a custom **Hash Table (`PatientHashTable`)**.
- **Hashing Function:** Polynomial rolling hash with prime multiplier 31 and prime bucket size $M = 101$.
- **Collision Resolution:** **Separate Chaining** using linked lists in each bucket slot.
- **Time Complexity:** Average case $O(1)$ for lookup, insertion, and deletion. Worst case $O(N)$ if all keys collide into the same bucket.
- **Why not linear search?** Searching an array of $N$ patients takes $O(N)$ time. In an emergency room lookup, $O(1)$ constant time lookup is critical.

---

### Q3. Explain your Emergency Triage Priority Queue. Why is a Binary Heap chosen over a sorted array or list?
**Answer:**
In emergency triage, patients must be treated strictly by acuity:
$$\text{Critical} > \text{High} > \text{Medium} > \text{Low}$$
Equal severities are broken deterministically by earliest arrival time (FIFO).

- **Sorted Array:** Insertion takes $O(N)$ time because elements must be shifted.
- **Unsorted Array:** Insertion is $O(1)$, but extracting the highest priority case takes $O(N)$ time.
- **Binary Heap:** 
  - Insertion (`heapifyUp`): $O(\log N)$
  - Extract Max (`heapifyDown`): $O(\log N)$
  - Peek Max: $O(1)$
This ensures consistent logarithmic performance even with high incoming casualty volumes.

---

### Q4. How does the hospital navigation calculate the shortest path, and how are closed corridors handled?
**Answer:**
Hospital departments and hallways are modeled as a **weighted undirected graph** $G = (V, E)$ using an **Adjacency List**.
- **Dijkstra's Algorithm** is used to find the shortest travel route from a start department to a destination.
- **Time Complexity:** $O((V + E) \log V)$ using a min-priority queue.
- **Corridor Closure Simulation:** When a hallway is blocked (e.g. for sterilization or maintenance), the edge is flagged as inactive in the adjacency list. Dijkstra then reroutes around the blockage to provide the optimal detour.

---

### Q5. Why is a Binary Search Tree (BST) used for Doctors? What happens if the tree becomes degenerate?
**Answer:**
Doctor records are indexed by Doctor ID in a **Binary Search Tree (`DoctorBST`)**.
- The BST maintains ordering: left child keys are smaller than parent, right child keys are greater.
- **In-Order Traversal** (Left, Root, Right) yields the entire physician list in ascending alphabetical/lexicographical order in $O(N)$ time without an extra sorting step.
- **Search & Insertion:** Average $O(\log N)$, worst-case $O(N)$ if inserted in strictly sorted order (skewed/degenerate tree). In production, an AVL or Red-Black self-balancing tree resolves skewing.

---

### Q6. How does the Administrative Undo feature work?
**Answer:**
The undo feature is implemented using a custom **Stack (`UndoStack`)** following the LIFO (Last-In, First-Out) principle.
- When an administrative action occurs (e.g., bed allocation or patient discharge), an `UndoAction` snapshot containing the action type, bed ID, and previous patient ID is pushed onto the stack ($O(1)$).
- Clicking "Undo" pops the top action ($O(1)$) and reverses the state both in SQLite and in-memory bed structures.

---

### Q7. How does the backend maintain synchronization between SQLite and in-memory data structures?
**Answer:**
On server start (`HospitalService::initialize()`), tables are validated and existing rows are loaded into in-memory custom data structures:
- Patients $\to$ Hash Table
- Doctors $\to$ BST
- Pending Emergencies $\to$ Max-Heap
- Hospital Campus $\to$ Graph

During live operations, every modifying API request executes a database prepared statement transaction first. Once SQLite confirms success, the in-memory data structure is synchronously updated. If the server is restarted, all data is reloaded with 100% fidelity.
