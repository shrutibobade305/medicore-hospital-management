# MediCore SQLite Database Schema

## 1. Overview
The MediCore persistence engine uses SQLite 3 with strict foreign key constraints enabled:
```sql
PRAGMA foreign_keys = ON;
```

---

## 2. Table Definitions

### `patients`
Stores all registered patients.
```sql
CREATE TABLE IF NOT EXISTS patients (
    patient_id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    age INTEGER NOT NULL,
    gender TEXT NOT NULL,
    contact TEXT NOT NULL,
    address TEXT,
    blood_group TEXT,
    registration_date TEXT,
    notes TEXT
);
```

### `doctors`
Stores hospital medical practitioners.
```sql
CREATE TABLE IF NOT EXISTS doctors (
    doctor_id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    department TEXT NOT NULL,
    specialization TEXT NOT NULL,
    contact TEXT NOT NULL,
    is_available INTEGER DEFAULT 1
);
```

### `appointments`
Outpatient bookings and consultation statuses.
```sql
CREATE TABLE IF NOT EXISTS appointments (
    appointment_id TEXT PRIMARY KEY,
    patient_id TEXT NOT NULL,
    doctor_id TEXT NOT NULL,
    appointment_date TEXT NOT NULL,
    appointment_time TEXT NOT NULL,
    status TEXT NOT NULL,
    reason TEXT,
    FOREIGN KEY(patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE,
    FOREIGN KEY(doctor_id) REFERENCES doctors(doctor_id) ON DELETE CASCADE
);
```

### `beds`
Hospital bed inventory and room allocation tracking.
```sql
CREATE TABLE IF NOT EXISTS beds (
    bed_id TEXT PRIMARY KEY,
    ward TEXT NOT NULL,
    room_number TEXT NOT NULL,
    status TEXT NOT NULL,
    patient_id TEXT,
    allocated_at TEXT,
    FOREIGN KEY(patient_id) REFERENCES patients(patient_id) ON DELETE SET NULL
);
```

### `emergency_cases`
Triage admissions prioritized by medical severity.
```sql
CREATE TABLE IF NOT EXISTS emergency_cases (
    case_id TEXT PRIMARY KEY,
    patient_id TEXT NOT NULL,
    patient_name TEXT NOT NULL,
    severity TEXT NOT NULL,
    arrival_time TEXT NOT NULL,
    status TEXT NOT NULL,
    assigned_doctor_id TEXT,
    notes TEXT,
    FOREIGN KEY(patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE
);
```

### `visits`
Chronological clinical visit notes for admitted/consulted patients.
```sql
CREATE TABLE IF NOT EXISTS visits (
    visit_id TEXT PRIMARY KEY,
    patient_id TEXT NOT NULL,
    doctor_id TEXT NOT NULL,
    visit_date TEXT NOT NULL,
    diagnosis TEXT NOT NULL,
    prescription TEXT,
    FOREIGN KEY(patient_id) REFERENCES patients(patient_id) ON DELETE CASCADE
);
```

### `activities`
System audit and administrative action logging.
```sql
CREATE TABLE IF NOT EXISTS activities (
    activity_id TEXT PRIMARY KEY,
    timestamp TEXT NOT NULL,
    action_type TEXT NOT NULL,
    entity_id TEXT NOT NULL,
    description TEXT NOT NULL
);
```
