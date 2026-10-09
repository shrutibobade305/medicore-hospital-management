# MediCore Data Structures & Algorithms Complexity Analysis

This document provides academic, hardware-level, and theoretical explanations of the custom Data Structures and Algorithms implemented in C++17 for the MediCore Smart Hospital Management System.

---

## 1. Summary Matrix

| Data Structure / Algorithm | Source File | Core Operation | Best Case | Average Case | Worst Case | Space Complexity |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Custom Patient Hash Table** | `patient_hash_table.cpp` | Search (by ID)<br>Insert<br>Delete | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(N)$<br>$O(N)$<br>$O(N)$ | $O(N + B)$ where $B$ is buckets |
| **Doctor Binary Search Tree** | `doctor_bst.cpp` | Search (by ID)<br>Insert<br>In-Order Traversal | $O(1)$<br>$O(1)$<br>$O(N)$ | $O(\log N)$<br>$O(\log N)$<br>$O(N)$ | $O(N)$<br>$O(N)$<br>$O(N)$ | $O(N)$ tree nodes |
| **Emergency Binary Max-Heap** | `emergency_heap.cpp` | Peek Max<br>Insert (Heapify-up)<br>Extract Max | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(1)$<br>$O(\log N)$<br>$O(\log N)$ | $O(1)$<br>$O(\log N)$<br>$O(\log N)$ | $O(N)$ contiguous buffer |
| **Administrative Undo Stack** | `undo_stack.cpp` | Push<br>Pop<br>Top | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(K)$ where $K$ is history depth |
| **Appointment FIFO Queue** | `appointment_queue.cpp` | Enqueue<br>Dequeue<br>Front | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(N)$ linked nodes |
| **Patient Visit Doubly Linked List**| `visit_linked_list.cpp` | Append / Prepend<br>Forward Scan<br>Reverse Scan | $O(1)$<br>$O(1)$<br>$O(1)$ | $O(1)$<br>$O(N)$<br>$O(N)$ | $O(1)$<br>$O(N)$<br>$O(N)$ | $O(N)$ node pointers |
| **Hospital Graph Dijkstra** | `hospital_graph.cpp` | Shortest Path | $O(V \log V)$ | $O((V+E)\log V)$| $O((V+E)\log V)$| $O(V + E)$ graph representation |
| **Graph BFS / DFS Traversal** | `hospital_graph.cpp` | Breadth / Depth Scan | $O(V)$ | $O(V + E)$ | $O(V + E)$ | $O(V)$ visited map & queue/callstack |
| **Merge Sort Algorithm** | `dsa_algorithms.cpp` | Stable Array Sorting | $O(N \log N)$ | $O(N \log N)$ | $O(N \log N)$ | $O(N)$ auxiliary work array |
| **Binary Search Algorithm** | `dsa_algorithms.cpp` | Search sorted list | $O(1)$ | $O(\log N)$ | $O(\log N)$ | $O(1)$ iterative pointers |

---

## 2. In-Depth Technical Details

### A. Patient Hash Table
- **Collision Resolution:** Separate chaining utilizing singly linked bucket nodes.
- **Hash Function:** Polynomial rolling hash with prime base $p = 31$ and prime modulo $M = 101$:
  $$\text{hash}(S) = \left(\sum_{i=0}^{L-1} S[i] \cdot 31^i\right) \pmod M$$
- **Load Factor:** Kept $\le 0.75$. On exceeding threshold, buckets are automatically doubled to the next prime size, maintaining $O(1)$ amortized lookups.

### B. Doctor Binary Search Tree (BST)
- **Ordering Property:** For every node $X$, all keys in $X.\text{left} < X.\text{key} < X.\text{right}$.
- **In-Order Traversal:** Recursively executing `InOrder(node->left)`, `visit(node)`, `InOrder(node->right)` produces a lexicographically sorted list of physicians without external sorting algorithms in $O(N)$ time.

### C. Emergency Binary Max-Heap
- **Representation:** 0-indexed contiguous array (`std::vector`).
  - Parent of index $i$: $\lfloor(i - 1) / 2\rfloor$
  - Left child: $2i + 1$
  - Right child: $2i + 2$
- **Dual-Criterion Priority Comparator:**
  ```cpp
  bool isHigherPriority(const EmergencyCase& a, const EmergencyCase& b) {
      if (a.severity != b.severity)
          return severityWeight(a.severity) > severityWeight(b.severity);
      return a.arrival_time < b.arrival_time; // Earlier arrival tie-breaker
  }
  ```

### D. Administrative Undo Stack
- **Role:** Guarantees atomicity and administrative safety. If a hospital bed is mistakenly assigned or a patient prematurely discharged, the administrator can trigger an instantaneous rollback.
- **Memory Footprint:** Each operation stores an `UndoAction` struct with state tokens.

### E. Hospital Campus Weighted Graph & Dijkstra's Algorithm
- **Graph Representation:** Adjacency List: `std::vector<std::vector<Edge>> adj`.
- **Dijkstra Formulation:**
  - Maintains `dist[u]` initialized to $\infty$, with $\text{dist}[\text{start}] = 0$.
  - Uses a min-priority heap of pairs `(dist, vertex)` to select the closest unvisited department.
  - Dynamically supports road closures by masking or zeroing edge availability during relaxation.
