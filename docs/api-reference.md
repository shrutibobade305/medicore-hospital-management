# MediCore REST API Reference

The MediCore backend exposes a clean, predictable RESTful JSON API running on `http://localhost:8080`.

---

## 1. System Endpoints

### `GET /api/health`
Checks server responsiveness and SQLite database connection.
- **Response:**
  ```json
  {
    "status": "ok",
    "timestamp": 1740000000,
    "version": "1.0.0"
  }
  ```

### `GET /api/dashboard`
Returns aggregated hospital metrics computed directly from database and in-memory DSA structures.
- **Response:**
  ```json
  {
    "total_patients": 8,
    "total_doctors": 6,
    "today_appointments": 5,
    "available_beds": 14,
    "occupied_beds": 4,
    "pending_emergencies": 3,
    "recent_appointments": [ ... ],
    "emergency_queue": [ ... ],
    "bed_occupancy_by_ward": { ... }
  }
  ```

---

## 2. Patients API (Custom Hash Table)

### `GET /api/patients`
Retrieves all patients.
- **Query params:** `?search={query}&blood_group={group}`

### `POST /api/patients`
Registers a new patient. Auto-generates ID if not supplied (`P-100X`).
- **Body:**
  ```json
  {
    "name": "Jane Doe",
    "age": 32,
    "gender": "Female",
    "contact": "+1-555-0199",
    "address": "456 Oak Lane",
    "blood_group": "A+",
    "notes": "No allergies"
  }
  ```

### `GET /api/patients/{id}`
Instant $O(1)$ lookup using `PatientHashTable`.

### `PUT /api/patients/{id}`
Updates existing patient records.

### `DELETE /api/patients/{id}`
Deletes patient after checking active bed and appointment relationships.

### `GET /api/patients/{id}/visits`
Retrieves patient clinical visit history using the custom **Doubly Linked List**.

---

## 3. Doctors API (Custom Binary Search Tree)

### `GET /api/doctors`
Returns all doctors ordered by ID via BST In-Order traversal.
- **Query params:** `?department={dept}&search={name}`

### `POST /api/doctors`
Registers a doctor and inserts into `DoctorBST`.

### `PUT /api/doctors/{id}`
Updates doctor availability or department.

### `DELETE /api/doctors/{id}`
Removes doctor from BST and SQLite.

---

## 4. Appointments API (Custom FIFO Queue)

### `GET /api/appointments`
Returns appointments list.
- **Query params:** `?doctor_id={id}&status={status}&date={yyyy-mm-dd}`

### `POST /api/appointments`
Books an appointment. Enforces collision detection against doctor overlapping slots.

### `PUT /api/appointments/{id}`
Updates appointment status (`Scheduled`, `In Progress`, `Completed`, `Cancelled`).

### `DELETE /api/appointments/{id}`
Cancels and purges appointment record.

---

## 5. Emergency & Triage API (Custom Binary Max-Heap)

### `GET /api/emergencies`
Returns current pending triage queue in strict descending priority order.

### `POST /api/emergencies`
Registers an acute emergency case. Case is inserted into `EmergencyHeap` with $O(\log N)$ heapify-up.
- **Body:**
  ```json
  {
    "patient_name": "Marcus Vance",
    "severity": "Critical",
    "notes": "Severe chest pain"
  }
  ```

### `POST /api/emergencies/dispatch`
Extracts the highest-priority pending case from the root of the heap ($O(\log N)$ heapify-down).

### `POST /api/emergencies/{id}/complete`
Marks emergency case as completed/resolved.

---

## 6. Bed Management API (Custom Undo Stack)

### `GET /api/beds`
Returns all beds grouped by ward with current occupancy and assigned patient details.

### `POST /api/beds/{id}/allocate`
Allocates a bed to a patient. Pushes an `UndoAction` onto `UndoStack`.
- **Body:** `{ "patient_id": "P-1001" }`

### `POST /api/beds/{id}/release`
Discharges patient and frees bed. Pushes release rollback onto `UndoStack`.

### `POST /api/beds/undo`
Pops the most recent bed operation from `UndoStack` ($O(1)$) and reverses the state.

---

## 7. Hospital Navigation API (Custom Weighted Graph & Dijkstra)

### `GET /api/departments`
Lists all vertices in the hospital graph campus.

### `POST /api/navigation/route`
Computes the shortest path using Dijkstra's algorithm.
- **Body:**
  ```json
  {
    "from": "Main Entrance",
    "to": "Operation Theatre",
    "closed_edges": [
      { "u": "Reception", "v": "Emergency Department" }
    ]
  }
  ```
- **Response:**
  ```json
  {
    "success": true,
    "path": ["Main Entrance", "Radiology", "Operation Theatre"],
    "total_distance": 65,
    "distance_unit": "meters",
    "estimated_time_seconds": 52
  }
  ```

### `POST /api/navigation/traverse`
Executes BFS or DFS exploration from a starting department.
- **Body:** `{ "start": "Main Entrance", "algorithm": "BFS" }`
- **Response:** `{ "order": ["Main Entrance", "Reception", "Pharmacy", ...], "visited_count": 9 }`

---

## 8. Reports & Audit Log API

### `GET /api/activity`
Returns administrative audit logs recorded across operations.
