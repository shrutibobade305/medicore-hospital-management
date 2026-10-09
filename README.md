# MediCore — Smart Hospital Management System
### Full-Stack Data Structures & Algorithms (DSA) Mini Project

MediCore is a complete, production-grade, full-stack hospital management web application designed and built for college **Data Structures and Algorithms (DSA)** mini projects, viva evaluations, and portfolio demonstrations.

Unlike typical web projects that rely exclusively on database queries or generic high-level language frameworks, MediCore combines a modern **React 18 / Vite** healthcare SaaS user interface with a real, high-performance **C++17 Crow REST backend** powered by **8 custom-engineered in-memory data structures and algorithmic engines** synchronized with an **SQLite3** relational persistence layer.

---

## 🌟 Key Highlights & Features

1. **High-Performance C++17 Core:** Crow REST microframework over standalone Asio, zero external runtime dependencies on Node.js/Python for the backend.
2. **8 Custom In-Memory DSA Implementations:**
   - **Custom Hash Table:** $O(1)$ patient lookups using polynomial rolling hash and separate chaining.
   - **Binary Search Tree (BST):** Physician registry maintaining ordered records and $O(N)$ sorted in-order traversal.
   - **Binary Max-Heap Priority Queue:** Multi-criteria Emergency Triage engine prioritizing Critical > High > Medium > Low with deterministic FIFO arrival-time tie-breaking.
   - **Administrative Undo Stack:** $O(1)$ LIFO undo tracking for bed allocations and discharges.
   - **FIFO Queue:** Consultation room waiting list.
   - **Doubly Linked List:** Patient clinical visit history with bidirectional chronological traversal.
   - **Weighted Graph & Dijkstra's Algorithm:** Adjacency-list hospital floor routing with live corridor closure simulation and rerouting.
   - **Merge Sort & Binary Search:** Logarithmic search and stable sorting for medical analytics.
3. **Interactive DSA Visualizer:** Dedicated visual simulator allowing students, professors, and evaluators to interactively inspect queue operations, heap tree states, BST node insertions, graph traversals, and Dijkstra shortest paths.
4. **Relational SQLite3 Engine:** Automatic schema creation, relational integrity (`PRAGMA foreign_keys = ON`), prepared statements, and persistent data across server reboots.
5. **Modern Healthcare SaaS UI:** Built with React, Vite, Lucide icons, and Recharts, offering responsive layouts, collapsible navigation, real-time toast alerts, and confirmation dialogs.
6. **10 Complete Navigation Pages:**
   - 📊 **Dashboard:** Real-time patient counts, bed occupancy charts, and emergency alerts.
   - 🧑‍🤝‍🧑 **Patients:** Full CRUD, search, filter, and clinical visit history viewer.
   - 🩺 **Doctors:** Physician directory, department filters, and BST ordered display.
   - 📅 **Appointments:** Booking management, status updates, and conflict avoidance.
   - 🚨 **Emergency & Triage:** Visual Priority Queue, triage level tags, and dispatch workflow.
   - 🛏️ **Bed Management:** Ward occupancy, allocation, release, and single-click administrative Undo.
   - 🗺️ **Hospital Navigation:** SVG interactive floor plan with Dijkstra shortest path calculation.
   - 🧠 **DSA Visualizer:** Real-time visual demonstrator for all 8 data structures.
   - 📈 **Reports & Activity:** Real-time system audit logs and medical metrics.
   - ⚙️ **Settings & Viva Guide:** Technical diagnostics and comprehensive college viva Q&A cheat-sheet.

---

## 🏗️ System Architecture

```
┌────────────────────────────────────────────────────────┐
│             Presentation Tier: React SPA               │
│   (Vite + React Router + Lucide Icons + Recharts)      │
│               http://localhost:5173                    │
└───────────────────────────┬────────────────────────────┘
                            │
               REST API (HTTP / JSON)
                            │
┌───────────────────────────▼────────────────────────────┐
│              Application Tier: C++17 Crow              │
│       (Standalone Asio + nlohmann/json + MinGW)        │
│               http://localhost:8080                    │
├────────────────────────────────────────────────────────┤
│                  Core Service Layer                    │
│               (HospitalService & API)                  │
├────────────────────────────────────────────────────────┤
│                 Custom DSA In-Memory                   │
│   • Patient Hash Table (Separate Chaining)             │
│   • Doctor Binary Search Tree (BST)                    │
│   • Emergency Priority Queue (Binary Max-Heap)         │
│   • Administrative Undo History (Stack)                │
│   • Outpatient Waitlist (FIFO Queue)                   │
│   • Clinical Visit Logs (Doubly Linked List)           │
│   • Campus Route Navigation (Weighted Graph)           │
│   • Merge Sort & Binary Search Engines                 │
└───────────────────────────┬────────────────────────────┘
                            │
                 SQL Prepared Statements
                            │
┌───────────────────────────▼────────────────────────────┐
│             Persistence Tier: SQLite3 Engine           │
│                  data/hospital.db                      │
│        (PRAGMA foreign_keys = ON; Transactions)        │
└────────────────────────────────────────────────────────┘
```

---

## 📊 Data Structures & Algorithm Complexity Reference

| Module | Data Structure / Algorithm | Source File | Average Time | Worst Time | Space | Role in Hospital Workflow |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Patients** | Custom Hash Table | `patient_hash_table.cpp` | $O(1)$ | $O(N)$ | $O(N + B)$ | Instant patient lookup by ID with polynomial hashing & separate chaining |
| **Doctors** | Binary Search Tree (BST) | `doctor_bst.cpp` | $O(\log N)$ | $O(N)$ | $O(N)$ | Hierarchical doctor catalog with sorted in-order traversal |
| **Emergency** | Binary Max-Heap | `emergency_heap.cpp` | $O(\log N)$ | $O(\log N)$ | $O(N)$ | Priority triage queue (Critical > High > Medium > Low with FIFO tie-breaker) |
| **Beds** | Administrative Stack | `undo_stack.cpp` | $O(1)$ | $O(1)$ | $O(K)$ | LIFO rollback for erroneous bed allocation/release events |
| **Appointments**| FIFO Queue | `appointment_queue.cpp` | $O(1)$ | $O(1)$ | $O(N)$ | Strict first-come-first-served consultation queue |
| **Visits** | Doubly Linked List | `visit_linked_list.cpp` | $O(1)$ append | $O(N)$ scan | $O(N)$ | Bidirectional traversal of patient chronological visit histories |
| **Navigation** | Weighted Graph (Dijkstra) | `hospital_graph.cpp` | $O((V+E)\log V)$ | $O((V+E)\log V)$| $O(V + E)$ | Shortest hallway routing and real-time corridor closure detour |
| **Analytics** | Merge Sort | `dsa_algorithms.cpp` | $O(N \log N)$ | $O(N \log N)$ | $O(N)$ | Stable sorting of patient and appointment records |
| **Search** | Binary Search | `dsa_algorithms.cpp` | $O(\log N)$ | $O(\log N)$ | $O(1)$ | Fast logarithmic record query on sorted collections |

---

## 🗄️ Relational Database Schema Overview

The database file is stored in `data/hospital.db` and features strict foreign keys:
- `patients`: `patient_id` (PK), `name`, `age`, `gender`, `contact`, `address`, `blood_group`, `registration_date`, `notes`
- `doctors`: `doctor_id` (PK), `name`, `department`, `specialization`, `contact`, `is_available`
- `appointments`: `appointment_id` (PK), `patient_id` (FK), `doctor_id` (FK), `appointment_date`, `appointment_time`, `status`, `reason`
- `beds`: `bed_id` (PK), `ward`, `room_number`, `status`, `patient_id` (FK), `allocated_at`
- `emergency_cases`: `case_id` (PK), `patient_id` (FK), `patient_name`, `severity`, `arrival_time`, `status`, `assigned_doctor_id`, `notes`
- `visits`: `visit_id` (PK), `patient_id` (FK), `doctor_id`, `visit_date`, `diagnosis`, `prescription`
- `activities`: `activity_id` (PK), `timestamp`, `action_type`, `entity_id`, `description`

---

## ⚙️ Prerequisites & Toolchain Setup

### Windows
- **C++ Compiler:** MinGW-w64 GCC (supports C++17) installed at `C:\MinGW\bin` or on system PATH.
- **CMake:** Version 3.20 or newer (`python -m pip install cmake` or official installer).
- **Node.js:** v18, v20, or v22 with `npm`.

### Linux / macOS
- **Compiler:** `g++` or `clang++` (C++17 compliant).
- **Packages:** `cmake`, `make`, `sqlite3`, `libsqlite3-dev` (if using system SQLite).
- **Node.js:** `nodejs` and `npm`.

---

## 🚀 Quick Start Guide

### Automated Launch (Windows)
Double-click `scripts\run_all.bat` or run:
```cmd
.\scripts\run_all.bat
```
This automatically launches the C++ backend on port `8080` and the React frontend on port `5173`.

---

### Manual Two-Terminal Workflow

#### 1. Compile & Start the C++ Backend
In your first terminal:
```cmd
cd backend
mkdir build
cd build
cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER="C:/MinGW/bin/g++.exe" -DCMAKE_C_COMPILER="C:/MinGW/bin/gcc.exe" ..
cmake --build . --config Release
cd ..\..
.\backend\build\medicore_server.exe
```
*The backend server will start listening on `http://localhost:8080` and automatically verify or seed `data/hospital.db`.*

#### 2. Start the React Frontend
In your second terminal:
```cmd
cd frontend
npm install
npm run dev
```
*Open your browser and navigate to `http://localhost:5173`.*

---

## 🧪 Automated Testing

MediCore includes comprehensive automated test suites written in C++ that can be executed independently of the frontend:

### Running the Data Structures Suite
```cmd
cd backend\build
.\test_dsa.exe
```
**Test Results:** **55 / 55 tests passed (100% PASS)**
- Hash table collision resolution and resizing.
- Doubly linked list bidirectional iteration.
- Queue FIFO property verification.
- Emergency heap property restoration after insertion and extraction.
- Priority comparator with deterministic arrival-time tie-breaking.
- BST in-order sorting and search.
- Administrative stack push/pop and rollback state accuracy.
- Graph adjacency list, BFS, DFS, and Dijkstra shortest path calculation.
- Road closure rerouting.
- Merge sort stability and binary search correctness.

### Running the Backend Integration Suite
```cmd
cd backend\build
.\test_backend.exe
```
**Test Results:** **19 / 19 tests passed (100% PASS)**
- SQLite database initial schema generation and demo seeding.
- Atomic CRUD operations on patients, doctors, appointments, and beds.
- Foreign key constraints enforcement.
- Bed allocation and stack-based undo verification.

---

## 🌐 REST API Endpoints

| Method | Endpoint | Description |
| :--- | :--- | :--- |
| `GET` | `/api/health` | Server and database health check |
| `GET` | `/api/dashboard` | Aggregated metrics and triage summaries |
| `GET` | `/api/patients` | Retrieve all patients |
| `POST`| `/api/patients` | Register new patient (Custom Hash Table) |
| `GET` | `/api/patients/{id}` | Instant $O(1)$ patient lookup |
| `PUT` | `/api/patients/{id}` | Update patient record |
| `DELETE`| `/api/patients/{id}` | Remove patient record |
| `GET` | `/api/patients/{id}/visits` | Patient visit log (Doubly Linked List) |
| `GET` | `/api/doctors` | List doctors in BST sorted order |
| `POST`| `/api/doctors` | Register doctor |
| `GET` | `/api/appointments` | List outpatient appointments |
| `POST`| `/api/appointments` | Book appointment (Slot collision check) |
| `PUT` | `/api/appointments/{id}` | Update appointment status |
| `GET` | `/api/emergencies` | List active emergency queue in priority order |
| `POST`| `/api/emergencies` | Add emergency case (Binary Max-Heap insert) |
| `POST`| `/api/emergencies/dispatch` | Dispatch highest priority case (Heap extract-max) |
| `GET` | `/api/beds` | Bed status inventory by ward |
| `POST`| `/api/beds/{id}/allocate` | Allocate bed (Pushes to Undo Stack) |
| `POST`| `/api/beds/{id}/release` | Discharge bed (Pushes to Undo Stack) |
| `POST`| `/api/beds/undo` | Undo last bed operation (Pops from Undo Stack) |
| `GET` | `/api/departments` | List hospital departments (Graph vertices) |
| `POST`| `/api/navigation/route` | Compute shortest route (Dijkstra's Algorithm) |
| `POST`| `/api/navigation/traverse` | Run BFS or DFS graph traversal |
| `GET` | `/api/activity` | System audit logs |

---

## 🎓 College Viva & Examination Prep

### Core Questions Often Asked by Evaluators:
1. **Why use a custom Hash Table instead of std::unordered_map or a relational SQL query for patient lookup?**
   - In clinical emergencies, lookups must execute with deterministic $O(1)$ time complexity without database query latency. Our custom implementation demonstrates the mathematical mechanics of polynomial rolling hashes and separate chaining collision resolution.
2. **How does the Emergency Heap handle two patients with the same "Critical" severity?**
   - MediCore uses a dual-key priority comparator. If `severity_A == severity_B`, it compares `arrival_time_A < arrival_time_B`, guaranteeing that earlier arrivals are treated first while preserving medical priority.
3. **What is the complexity of Dijkstra's Algorithm in your hospital map?**
   - Modeled as an adjacency list graph with $V$ departments and $E$ corridors, Dijkstra runs in $O((V + E) \log V)$ time using a min-priority queue. Closed corridors are masked dynamically in $O(1)$ time during edge relaxation.
4. **How does administrative Undo work?**
   - The LIFO Stack holds snapshots of operations (`action_type`, `bed_id`, `previous_patient_id`, `previous_status`). Clicking Undo pops the top action and executes the exact inverse transaction on both SQLite and the in-memory bed structures.

---

## 📁 Project Directory Structure

```
medicore/
├── backend/
│   ├── include/
│   │   ├── dsa/                     # Custom Data Structures (Header files)
│   │   │   ├── patient_hash_table.hpp
│   │   │   ├── doctor_bst.hpp
│   │   │   ├── emergency_heap.hpp
│   │   │   ├── undo_stack.hpp
│   │   │   ├── appointment_queue.hpp
│   │   │   ├── visit_linked_list.hpp
│   │   │   ├── hospital_graph.hpp
│   │   │   └── dsa_algorithms.hpp
│   │   ├── models/                  # Hospital data models
│   │   ├── repositories/            # SQLite repository & persistence
│   │   ├── services/                # Core hospital service layer
│   │   └── third_party/             # Crow, Asio, json.hpp, sqlite3
│   ├── src/
│   │   ├── dsa/                     # DSA Implementations (.cpp)
│   │   ├── repositories/            # SQLite database logic
│   │   ├── services/                # Business logic & synchronization
│   │   └── main.cpp                 # Crow HTTP REST API & route handlers
│   ├── tests/
│   │   ├── test_dsa.cpp             # 55 automated unit tests for DSA
│   │   └── test_backend.cpp         # 19 automated tests for DB and business logic
│   └── CMakeLists.txt
├── frontend/
│   ├── src/
│   │   ├── components/              # Sidebar, Navbar, Modal, StatCard, Toast
│   │   ├── pages/                   # 10 Application pages
│   │   │   ├── Dashboard.jsx
│   │   │   ├── Patients.jsx
│   │   │   ├── Doctors.jsx
│   │   │   ├── Appointments.jsx
│   │   │   ├── EmergencyTriage.jsx
│   │   │   ├── BedManagement.jsx
│   │   │   ├── HospitalNavigation.jsx
│   │   │   ├── DsaVisualizer.jsx
│   │   │   ├── ReportsActivity.jsx
│   │   │   └── SettingsAbout.jsx
│   │   ├── services/                # REST API client (api.js)
│   │   ├── index.css                # Healthcare design system CSS
│   │   ├── App.jsx                  # React Router & shell
│   │   └── main.jsx                 # Client entry point
│   ├── package.json
│   └── vite.config.js
├── docs/                            # In-depth architectural & viva documentation
│   ├── architecture.md
│   ├── database-schema.md
│   ├── dsa-complexities.md
│   ├── api-reference.md
│   └── viva-questions.md
├── scripts/                         # Windows launch & build scripts
│   ├── build.bat
│   ├── run_backend.bat
│   ├── run_frontend.bat
│   └── run_all.bat
├── data/
│   └── hospital.db                  # Relational SQLite database
├── .gitignore
└── README.md
```

---

## 📸 Screenshots Section (Demonstration Placeholders)

- **Hospital Overview Dashboard:** Visual metrics cards, bed occupancy breakdown, and emergency alerts.
- **Emergency Triage Module:** Priority queue visualization displaying heap state and active dispatches.
- **Hospital Navigation:** Interactive campus floor plan with real-time Dijkstra shortest-path route highlighting.
- **DSA Visualizer:** Interactive educational playground for step-by-step traversal and tree inspections.

---

## ⚠️ Known Limitations & Future Enhancements

- **Single Hospital Campus:** The navigation graph is configured for a representative multi-story hospital complex. Multi-facility hospital campuses can be added by partitioning graph namespaces.
- **AVL / Red-Black Balancing:** The doctor BST uses standard binary search tree logic. While optimal for diverse doctor IDs, adding self-balancing rotations would prevent degenerate tree behavior in adversarial insertion cases.

---

## 📄 License
This project is created for academic and educational purposes under the MIT License.
