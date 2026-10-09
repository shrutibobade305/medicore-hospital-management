# MediCore Architecture Documentation

## 1. High-Level Architecture

MediCore is built with a separated client-server model designed for performance, educational clarity, and deterministic data structures demonstration:

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
│   • Patient Hash Table (djb2 + Separate Chaining)      │
│   • Doctor Binary Search Tree (BST)                    │
│   • Emergency Priority Queue (Binary Max-Heap)         │
│   • Administrative Undo History (Stack)                │
│   • Outpatient Waitlist (FIFO Queue)                   │
│   • Clinical Visit Logs (Doubly Linked List)           │
│   • Campus Route Navigation (Weighted Graph)           │
│   • Merge Sort & Binary Search Algorithms              │
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

## 2. Component Breakdown

### Frontend (Client Tier)
- **Framework:** React 18 / 19 with Vite bundler.
- **Styling:** Custom CSS design system adhering to modern healthcare SaaS aesthetics (soft slates, deep teal `#0f766e`, vibrant emeralds, high-contrast typography, cards, badges, and modals).
- **Navigation:** React Router v7 driving 10 dedicated routes.
- **Visualization:** SVG hospital interactive campus map, Recharts bed occupancy and activity distributions, step-by-step DSA visualizer.

### Backend (Server Tier)
- **Framework:** Crow C++ microframework (v1.3.4) over standalone Asio.
- **Language Standard:** C++17.
- **Networking:** Multi-threaded asynchronous request routing with Cross-Origin Resource Sharing (CORS) headers.
- **Serialization:** `nlohmann/json` v3.11.3 for type-safe model transformations.

### Data Storage & Synchronization
- Every entity (Patient, Doctor, Appointment, Bed, Emergency, Visit, Audit) is stored in SQLite.
- On startup, `HospitalService::initialize()` executes table schema migrations and loads all records into active custom C++ in-memory data structures.
- On mutations (e.g. bed allocation, triage dispatch, patient creation), SQLite write occurs transactionally alongside in-memory index updates.

---

## 3. Communication Contract

All endpoints adhere to predictable JSON structures:

- **Success:**
  ```json
  {
    "status": "success",
    "message": "Resource created / updated",
    "data": { ... }
  }
  ```
- **Error:**
  ```json
  {
    "error": "Descriptive reason for failure (e.g. Bed already occupied)"
  }
  ```
