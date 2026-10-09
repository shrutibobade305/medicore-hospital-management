#include "../../include/repositories/database.hpp"
#include <iostream>
#include <sstream>

namespace medicore {

Database::Database(const std::string& dbPath) : dbPath_(dbPath), db_(nullptr) {}

Database::~Database() {
    close();
}

bool Database::open() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    if (db_) return true;

    int rc = sqlite3_open(dbPath_.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to open SQLite database: " << sqlite3_errmsg(db_) << std::endl;
        close();
        return false;
    }

    // Enable foreign keys
    char* errMsg = nullptr;
    rc = sqlite3_exec(db_, "PRAGMA foreign_keys = ON; PRAGMA journal_mode = WAL;", nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to set PRAGMA: " << (errMsg ? errMsg : "") << std::endl;
        sqlite3_free(errMsg);
    }

    return true;
}

void Database::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

bool Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL execution error: " << (errMsg ? errMsg : "") << "\nQuery: " << sql << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::beginTransaction() {
    return execute("BEGIN TRANSACTION;");
}

bool Database::commitTransaction() {
    return execute("COMMIT;");
}

bool Database::rollbackTransaction() {
    return execute("ROLLBACK;");
}

bool Database::initializeSchema() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    if (!db_) return false;

    const std::string schemaSql = R"(
        CREATE TABLE IF NOT EXISTS patients (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            age INTEGER NOT NULL,
            gender TEXT NOT NULL,
            contact TEXT NOT NULL,
            address TEXT,
            blood_group TEXT NOT NULL,
            registration_date TEXT NOT NULL,
            notes TEXT
        );

        CREATE TABLE IF NOT EXISTS doctors (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            department TEXT NOT NULL,
            specialization TEXT NOT NULL,
            contact TEXT NOT NULL,
            email TEXT,
            availability TEXT NOT NULL,
            room_no TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS appointments (
            id TEXT PRIMARY KEY,
            patient_id TEXT NOT NULL,
            patient_name TEXT,
            doctor_id TEXT NOT NULL,
            doctor_name TEXT,
            appointment_date TEXT NOT NULL,
            appointment_time TEXT NOT NULL,
            reason TEXT,
            status TEXT NOT NULL,
            created_at TEXT NOT NULL,
            FOREIGN KEY (patient_id) REFERENCES patients(id) ON DELETE CASCADE,
            FOREIGN KEY (doctor_id) REFERENCES doctors(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS wards (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            department TEXT NOT NULL,
            total_beds INTEGER NOT NULL,
            occupied_beds INTEGER NOT NULL DEFAULT 0
        );

        CREATE TABLE IF NOT EXISTS beds (
            id TEXT PRIMARY KEY,
            ward_id TEXT NOT NULL,
            ward_name TEXT,
            room_no TEXT NOT NULL,
            bed_number TEXT NOT NULL,
            status TEXT NOT NULL,
            patient_id TEXT,
            patient_name TEXT,
            admitted_at TEXT,
            FOREIGN KEY (ward_id) REFERENCES wards(id) ON DELETE CASCADE,
            FOREIGN KEY (patient_id) REFERENCES patients(id) ON DELETE SET NULL
        );

        CREATE TABLE IF NOT EXISTS emergencies (
            id TEXT PRIMARY KEY,
            patient_id TEXT,
            patient_name TEXT NOT NULL,
            age INTEGER NOT NULL,
            gender TEXT NOT NULL,
            condition TEXT NOT NULL,
            severity TEXT NOT NULL,
            severity_rank INTEGER NOT NULL,
            arrival_time TEXT NOT NULL,
            arrival_epoch INTEGER NOT NULL,
            sequence_num INTEGER NOT NULL,
            status TEXT NOT NULL,
            notes TEXT
        );

        CREATE TABLE IF NOT EXISTS visit_history (
            id TEXT PRIMARY KEY,
            patient_id TEXT NOT NULL,
            doctor_id TEXT NOT NULL,
            doctor_name TEXT,
            visit_date TEXT NOT NULL,
            diagnosis TEXT NOT NULL,
            prescription TEXT,
            notes TEXT,
            created_at TEXT NOT NULL,
            FOREIGN KEY (patient_id) REFERENCES patients(id) ON DELETE CASCADE,
            FOREIGN KEY (doctor_id) REFERENCES doctors(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS audit_logs (
            id TEXT PRIMARY KEY,
            action_type TEXT NOT NULL,
            entity_name TEXT NOT NULL,
            entity_id TEXT NOT NULL,
            description TEXT NOT NULL,
            undoable INTEGER NOT NULL DEFAULT 0,
            timestamp TEXT NOT NULL
        );
    )";

    return execute(schemaSql);
}

bool Database::seedDemoData() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    if (!db_) return false;

    // Check if patients table already has data
    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, "SELECT COUNT(*) FROM patients;", -1, &stmt, nullptr);
    if (rc != SQLITE_OK) return false;

    int patientCount = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        patientCount = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (patientCount > 0) {
        // Data already seeded
        return true;
    }

    std::cout << "[Database] Seeding realistic demo records..." << std::endl;
    execute("BEGIN TRANSACTION;");

    // 1. Seed Patients
    const std::vector<Patient> seedPatients = {
        {"PAT-1001", "Arun Kumar", 45, "Male", "+91 98450 11223", "Flat 302, Green Glen Layout, Bangalore", "O+", "2026-09-10", "Hypertension history; on regular monitoring."},
        {"PAT-1002", "Priya Sharma", 32, "Female", "+91 97412 88344", "12/A, Indiranagar 100ft Rd, Bangalore", "B+", "2026-09-15", "Routine prenatal checkup, gestational week 24."},
        {"PAT-1003", "Rohan Mehta", 58, "Male", "+91 94481 77655", "54, Koramangala 4th Block, Bangalore", "A+", "2026-09-18", "Type-2 Diabetes mellitus, diabetic retinopathy assessment."},
        {"PAT-1004", "Sunita Nair", 27, "Female", "+91 98860 22334", "Plot 88, HSR Layout Sector 2, Bangalore", "AB+", "2026-09-22", "Acute migraine with aura, prescribed triptans."},
        {"PAT-1005", "David Fernandes", 64, "Male", "+91 98440 99887", "45, Richmond Road, Bangalore", "O-", "2026-09-25", "Post-coronary stenting rehabilitation review."},
        {"PAT-1006", "Ananya Deshmukh", 19, "Female", "+91 97310 55443", "Hostel B, Malleshwaram, Bangalore", "A-", "2026-10-01", "Ankling sprain sustained during basketball practice."},
        {"PAT-1007", "Vikram Rathore", 51, "Male", "+91 98801 66778", "21B, Whitefield Main Road, Bangalore", "B-", "2026-10-03", "Chronic bronchitis exacerbation; spirometry pending."},
        {"PAT-1008", "Meera Iyer", 39, "Female", "+91 94490 33445", "104, Jayanagar 7th Block, Bangalore", "O+", "2026-10-05", "Hypothyroidism follow-up, thyroid profile normal."}
    };

    for (const auto& p : seedPatients) {
        sqlite3_prepare_v2(db_, "INSERT INTO patients VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, p.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, p.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 3, p.age);
        sqlite3_bind_text(stmt, 4, p.gender.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, p.contact.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, p.address.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, p.bloodGroup.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 8, p.registrationDate.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 9, p.notes.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // 2. Seed Doctors
    const std::vector<Doctor> seedDoctors = {
        {"DOC-101", "Dr. Rajesh Rao", "Cardiology", "Interventional Cardiology", "+91 98451 00001", "dr.rajesh@medicore.org", "Available", "OPD-101"},
        {"DOC-102", "Dr. Shalini Gupta", "Neurology", "Stroke & Neurovascular", "+91 98451 00002", "dr.shalini@medicore.org", "Available", "OPD-102"},
        {"DOC-103", "Dr. Amitava Roy", "Orthopedics", "Joint Replacement & Trauma", "+91 98451 00003", "dr.amitava@medicore.org", "Available", "OPD-103"},
        {"DOC-104", "Dr. Preethi Hegde", "Obstetrics & Gyn", "Maternal-Fetal Medicine", "+91 98451 00004", "dr.preethi@medicore.org", "Available", "OPD-104"},
        {"DOC-105", "Dr. Farhan Siddiqui", "Emergency Medicine", "Trauma Resuscitation", "+91 98451 00005", "dr.farhan@medicore.org", "Available", "EMG-Bay 1"},
        {"DOC-106", "Dr. Kavita Menon", "Pulmonology", "Critical Care & Asthma", "+91 98451 00006", "dr.kavita@medicore.org", "Available", "OPD-105"}
    };

    for (const auto& d : seedDoctors) {
        sqlite3_prepare_v2(db_, "INSERT INTO doctors VALUES (?, ?, ?, ?, ?, ?, ?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, d.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, d.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, d.department.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, d.specialization.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, d.contact.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, d.email.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, d.availability.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 8, d.roomNo.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // 3. Seed Wards
    const std::vector<Ward> seedWards = {
        {"WARD-GEN-A", "General Ward A", "Internal Medicine", 6, 2},
        {"WARD-ICU-1", "Intensive Care Unit", "Critical Care", 4, 2},
        {"WARD-SURG", "Post-Surgical Ward", "General Surgery", 4, 1},
        {"WARD-CARD", "Cardiac Care Ward", "Cardiology", 4, 1}
    };

    for (const auto& w : seedWards) {
        sqlite3_prepare_v2(db_, "INSERT INTO wards VALUES (?, ?, ?, ?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, w.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, w.name.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, w.department.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, w.totalBeds);
        sqlite3_bind_int(stmt, 5, w.occupiedBeds);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // 4. Seed Beds
    const std::vector<Bed> seedBeds = {
        {"BED-GA-01", "WARD-GEN-A", "General Ward A", "101", "Bed 1", "Occupied", "PAT-1003", "Rohan Mehta", "2026-10-06 09:30"},
        {"BED-GA-02", "WARD-GEN-A", "General Ward A", "101", "Bed 2", "Occupied", "PAT-1007", "Vikram Rathore", "2026-10-07 11:15"},
        {"BED-GA-03", "WARD-GEN-A", "General Ward A", "102", "Bed 3", "Available", "", "", ""},
        {"BED-GA-04", "WARD-GEN-A", "General Ward A", "102", "Bed 4", "Available", "", "", ""},
        {"BED-GA-05", "WARD-GEN-A", "General Ward A", "103", "Bed 5", "Available", "", "", ""},
        {"BED-GA-06", "WARD-GEN-A", "General Ward A", "103", "Bed 6", "Out of Service", "", "", ""},

        {"BED-ICU-01", "WARD-ICU-1", "Intensive Care Unit", "ICU-A", "ICU Bed 1", "Occupied", "PAT-1001", "Arun Kumar", "2026-10-08 04:00"},
        {"BED-ICU-02", "WARD-ICU-1", "Intensive Care Unit", "ICU-A", "ICU Bed 2", "Occupied", "PAT-1005", "David Fernandes", "2026-10-07 16:45"},
        {"BED-ICU-03", "WARD-ICU-1", "Intensive Care Unit", "ICU-B", "ICU Bed 3", "Available", "", "", ""},
        {"BED-ICU-04", "WARD-ICU-1", "Intensive Care Unit", "ICU-B", "ICU Bed 4", "Available", "", "", ""},

        {"BED-SRG-01", "WARD-SURG", "Post-Surgical Ward", "201", "Surg Bed 1", "Occupied", "PAT-1006", "Ananya Deshmukh", "2026-10-08 08:20"},
        {"BED-SRG-02", "WARD-SURG", "Post-Surgical Ward", "201", "Surg Bed 2", "Available", "", "", ""},
        {"BED-SRG-03", "WARD-SURG", "Post-Surgical Ward", "202", "Surg Bed 3", "Available", "", "", ""},
        {"BED-SRG-04", "WARD-SURG", "Post-Surgical Ward", "202", "Surg Bed 4", "Available", "", "", ""},

        {"BED-CRD-01", "WARD-CARD", "Cardiac Care Ward", "301", "Cardiac Bed 1", "Available", "", "", ""},
        {"BED-CRD-02", "WARD-CARD", "Cardiac Care Ward", "301", "Cardiac Bed 2", "Available", "", "", ""},
        {"BED-CRD-03", "WARD-CARD", "Cardiac Care Ward", "302", "Cardiac Bed 3", "Available", "", "", ""},
        {"BED-CRD-04", "WARD-CARD", "Cardiac Care Ward", "302", "Cardiac Bed 4", "Available", "", "", ""}
    };

    for (const auto& b : seedBeds) {
        sqlite3_prepare_v2(db_, "INSERT INTO beds VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, b.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, b.wardId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, b.wardName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, b.roomNo.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, b.bedNumber.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, b.status.c_str(), -1, SQLITE_TRANSIENT);
        if (b.patientId.empty()) {
            sqlite3_bind_null(stmt, 7);
        } else {
            sqlite3_bind_text(stmt, 7, b.patientId.c_str(), -1, SQLITE_TRANSIENT);
        }
        sqlite3_bind_text(stmt, 8, b.patientName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 9, b.admittedAt.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // 5. Seed Appointments
    const std::vector<Appointment> seedAppointments = {
        {"APT-2001", "PAT-1002", "Priya Sharma", "DOC-104", "Dr. Preethi Hegde", "2026-10-09", "10:00", "Second Trimester Routine Ultrasound Review", "Scheduled", "2026-10-08 14:00"},
        {"APT-2002", "PAT-1004", "Sunita Nair", "DOC-102", "Dr. Shalini Gupta", "2026-10-09", "10:30", "Neurological Workup for Refractory Migraines", "Waiting", "2026-10-08 15:30"},
        {"APT-2003", "PAT-1008", "Meera Iyer", "DOC-101", "Dr. Rajesh Rao", "2026-10-09", "11:15", "Cardiovascular Checkup & Lipid Panel Discussion", "Scheduled", "2026-10-08 17:10"},
        {"APT-2004", "PAT-1006", "Ananya Deshmukh", "DOC-103", "Dr. Amitava Roy", "2026-10-09", "12:00", "Ligament Tear Ultrasound & Cast Removal", "Waiting", "2026-10-09 08:00"},
        {"APT-2005", "PAT-1005", "David Fernandes", "DOC-106", "Dr. Kavita Menon", "2026-10-10", "09:30", "Pulmonary Function Test Follow-up", "Scheduled", "2026-10-08 11:00"}
    };

    for (const auto& a : seedAppointments) {
        sqlite3_prepare_v2(db_, "INSERT INTO appointments VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, a.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, a.patientId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, a.patientName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, a.doctorId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, a.doctorName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, a.appointmentDate.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, a.appointmentTime.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 8, a.reason.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 9, a.status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 10, a.createdAt.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // 6. Seed Emergencies
    const std::vector<EmergencyCase> seedEmergencies = {
        {"EMG-5001", "PAT-1005", "David Fernandes", 64, "Male", "Severe Crushing Retrosternal Chest Pain with ST Elevation", "Critical", 4, "10:15 AM", 1791530100, 1, "Pending", "Transferred from triage, ECG confirms STEMI."},
        {"EMG-5002", "", "Karan Singhania", 23, "Male", "Road Traffic Accident with Compound Femur Fracture", "Critical", 4, "10:20 AM", 1791530400, 2, "Pending", "Immobilized, hypovolemic shock precautions."},
        {"EMG-5003", "", "Lalitha Prasad", 71, "Female", "Acute Ischemic Stroke with Left-sided Hemiparesis", "High", 3, "10:05 AM", 1791529500, 3, "Pending", "Onset 45 mins ago, within thrombolysis window."},
        {"EMG-5004", "", "Sanjay Verma", 48, "Male", "Uncontrolled Epistaxis and Blood Pressure 210/120 mmHg", "High", 3, "10:25 AM", 1791530700, 4, "Pending", "Hypertensive crisis, anterior nasal packing."},
        {"EMG-5005", "PAT-1006", "Ananya Deshmukh", 19, "Female", "Severe Acute Asthma Attack refractory to inhaler", "Medium", 2, "09:50 AM", 1791528600, 5, "Pending", "Nebulization in progress, SpO2 93%."}
    };

    for (const auto& e : seedEmergencies) {
        sqlite3_prepare_v2(db_, "INSERT INTO emergencies VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, e.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, e.patientId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, e.patientName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 4, e.age);
        sqlite3_bind_text(stmt, 5, e.gender.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, e.condition.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, e.severity.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 8, e.severityRank);
        sqlite3_bind_text(stmt, 9, e.arrivalTime.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int64(stmt, 10, e.arrivalEpoch);
        sqlite3_bind_int64(stmt, 11, e.sequenceNum);
        sqlite3_bind_text(stmt, 12, e.status.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 13, e.notes.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // 7. Seed Visit History
    const std::vector<VisitRecord> seedVisits = {
        {"VST-9001", "PAT-1001", "DOC-101", "Dr. Rajesh Rao", "2026-09-12", "Essential Hypertension, Grade II", "Tab Telmisartan 40mg OD, Tab Amlodipine 5mg OD", "Low sodium diet advised, follow-up in 4 weeks.", "2026-09-12 11:30"},
        {"VST-9002", "PAT-1003", "DOC-101", "Dr. Rajesh Rao", "2026-09-20", "Type-2 DM with Mild Peripheral Neuropathy", "Tab Metformin 500mg BD, Tab Pregabalin 75mg HS", "Strict glycemic monitoring; HbA1c 7.6%.", "2026-09-20 12:15"},
        {"VST-9003", "PAT-1004", "DOC-102", "Dr. Shalini Gupta", "2026-09-23", "Migraine with Aura (Visual Scintillating)", "Tab Zolmitriptan 2.5mg PRN, Tab Propranolol 40mg OD", "Sleep hygiene recommended.", "2026-09-23 15:00"},
        {"VST-9004", "PAT-1005", "DOC-101", "Dr. Rajesh Rao", "2026-09-28", "Post-PCI Follow-up (LAD Stent)", "Tab Aspirin 75mg OD, Tab Ticagrelor 90mg BD, Tab Atorvastatin 40mg", "Echocardiogram LVEF 55%, stent patent.", "2026-09-28 10:45"}
    };

    for (const auto& v : seedVisits) {
        sqlite3_prepare_v2(db_, "INSERT INTO visit_history VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, v.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, v.patientId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, v.doctorId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, v.doctorName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, v.visitDate.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 6, v.diagnosis.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 7, v.prescription.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 8, v.notes.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 9, v.createdAt.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    // 8. Seed Audit Logs
    const std::vector<AuditLog> seedLogs = {
        {"LOG-001", "SYSTEM_INIT", "Database", "SYSTEM", "MediCore Database initialized with clinical demo schema.", false, "2026-10-09 08:00:00"},
        {"LOG-002", "BED_ALLOCATE", "Bed", "BED-GA-01", "Allocated Bed 1 (Ward A) to patient Rohan Mehta (PAT-1003).", true, "2026-10-09 08:15:00"},
        {"LOG-003", "BED_ALLOCATE", "Bed", "BED-ICU-01", "Allocated ICU Bed 1 to patient Arun Kumar (PAT-1001).", true, "2026-10-09 08:30:00"},
        {"LOG-004", "EMERGENCY_ADMIT", "Emergency", "EMG-5001", "Registered critical triage case EMG-5001 (David Fernandes) into Min-Heap.", false, "2026-10-09 10:15:00"}
    };

    for (const auto& l : seedLogs) {
        sqlite3_prepare_v2(db_, "INSERT INTO audit_logs VALUES (?, ?, ?, ?, ?, ?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, l.id.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, l.actionType.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, l.entityName.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 4, l.entityId.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 5, l.description.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 6, l.undoable ? 1 : 0);
        sqlite3_bind_text(stmt, 7, l.timestamp.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    execute("COMMIT;");
    std::cout << "[Database] Successfully seeded demo records." << std::endl;
    return true;
}

bool Database::resetDatabase() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    if (!db_) return false;

    execute("PRAGMA foreign_keys = OFF;");
    execute("DROP TABLE IF EXISTS audit_logs;");
    execute("DROP TABLE IF EXISTS visit_history;");
    execute("DROP TABLE IF EXISTS emergencies;");
    execute("DROP TABLE IF EXISTS beds;");
    execute("DROP TABLE IF EXISTS wards;");
    execute("DROP TABLE IF EXISTS appointments;");
    execute("DROP TABLE IF EXISTS doctors;");
    execute("DROP TABLE IF EXISTS patients;");
    execute("PRAGMA foreign_keys = ON;");

    initializeSchema();
    return seedDemoData();
}

// --- Patient CRUD ---
bool Database::insertPatient(const Patient& p) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO patients (id, name, age, gender, contact, address, blood_group, registration_date, notes) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, p.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, p.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, p.age);
    sqlite3_bind_text(stmt, 4, p.gender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, p.contact.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, p.address.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, p.bloodGroup.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, p.registrationDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, p.notes.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::updatePatient(const Patient& p) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE patients SET name=?, age=?, gender=?, contact=?, address=?, blood_group=?, notes=? WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, p.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 2, p.age);
    sqlite3_bind_text(stmt, 3, p.gender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, p.contact.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, p.address.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, p.bloodGroup.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, p.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, p.id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::deletePatient(const std::string& id) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM patients WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Patient> Database::getAllPatients() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<Patient> list;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, name, age, gender, contact, address, blood_group, registration_date, notes FROM patients ORDER BY id ASC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Patient p;
        p.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        p.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        p.age = sqlite3_column_int(stmt, 2);
        p.gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        p.contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const char* addr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        p.address = addr ? addr : "";
        p.bloodGroup = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        p.registrationDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        const char* n = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        p.notes = n ? n : "";
        list.push_back(p);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool Database::getPatientById(const std::string& id, Patient& outPatient) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, name, age, gender, contact, address, blood_group, registration_date, notes FROM patients WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outPatient.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        outPatient.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        outPatient.age = sqlite3_column_int(stmt, 2);
        outPatient.gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        outPatient.contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const char* addr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        outPatient.address = addr ? addr : "";
        outPatient.bloodGroup = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        outPatient.registrationDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        const char* n = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        outPatient.notes = n ? n : "";
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

// --- Doctor CRUD ---
bool Database::insertDoctor(const Doctor& d) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO doctors VALUES (?, ?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, d.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, d.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, d.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, d.specialization.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, d.contact.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, d.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, d.availability.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, d.roomNo.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::updateDoctor(const Doctor& d) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE doctors SET name=?, department=?, specialization=?, contact=?, email=?, availability=?, room_no=? WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, d.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, d.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, d.specialization.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, d.contact.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, d.email.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, d.availability.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, d.roomNo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, d.id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::deleteDoctor(const std::string& id) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM doctors WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Doctor> Database::getAllDoctors() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<Doctor> list;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, name, department, specialization, contact, email, availability, room_no FROM doctors ORDER BY id ASC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Doctor d;
        d.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        d.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        d.department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        d.specialization = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        d.contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const char* em = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        d.email = em ? em : "";
        d.availability = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        d.roomNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        list.push_back(d);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool Database::getDoctorById(const std::string& id, Doctor& outDoctor) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, name, department, specialization, contact, email, availability, room_no FROM doctors WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outDoctor.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        outDoctor.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        outDoctor.department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        outDoctor.specialization = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        outDoctor.contact = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        const char* em = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        outDoctor.email = em ? em : "";
        outDoctor.availability = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        outDoctor.roomNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

// --- Appointment CRUD ---
bool Database::insertAppointment(const Appointment& a) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO appointments VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, a.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, a.patientId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, a.patientName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, a.doctorId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, a.doctorName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, a.appointmentDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, a.appointmentTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, a.reason.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, a.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 10, a.createdAt.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::updateAppointment(const Appointment& a) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE appointments SET patient_id=?, patient_name=?, doctor_id=?, doctor_name=?, appointment_date=?, appointment_time=?, reason=?, status=? WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, a.patientId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, a.patientName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, a.doctorId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, a.doctorName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, a.appointmentDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, a.appointmentTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, a.reason.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, a.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, a.id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::deleteAppointment(const std::string& id) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "DELETE FROM appointments WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Appointment> Database::getAllAppointments() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<Appointment> list;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, patient_id, patient_name, doctor_id, doctor_name, appointment_date, appointment_time, reason, status, created_at FROM appointments ORDER BY appointment_date ASC, appointment_time ASC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Appointment a;
        a.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        a.patientId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* pn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        a.patientName = pn ? pn : "";
        a.doctorId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* dn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        a.doctorName = dn ? dn : "";
        a.appointmentDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        a.appointmentTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        const char* r = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        a.reason = r ? r : "";
        a.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        a.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        list.push_back(a);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool Database::getAppointmentById(const std::string& id, Appointment& outAppt) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, patient_id, patient_name, doctor_id, doctor_name, appointment_date, appointment_time, reason, status, created_at FROM appointments WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outAppt.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        outAppt.patientId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* pn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        outAppt.patientName = pn ? pn : "";
        outAppt.doctorId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        const char* dn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        outAppt.doctorName = dn ? dn : "";
        outAppt.appointmentDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        outAppt.appointmentTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        const char* r = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        outAppt.reason = r ? r : "";
        outAppt.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        outAppt.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 9));
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

// --- Emergencies ---
bool Database::insertEmergency(const EmergencyCase& e) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO emergencies VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, e.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, e.patientId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, e.patientName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, e.age);
    sqlite3_bind_text(stmt, 5, e.gender.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, e.condition.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, e.severity.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 8, e.severityRank);
    sqlite3_bind_text(stmt, 9, e.arrivalTime.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int64(stmt, 10, e.arrivalEpoch);
    sqlite3_bind_int64(stmt, 11, e.sequenceNum);
    sqlite3_bind_text(stmt, 12, e.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 13, e.notes.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::updateEmergency(const EmergencyCase& e) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE emergencies SET status=?, notes=? WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, e.status.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, e.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, e.id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<EmergencyCase> Database::getAllEmergencies() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<EmergencyCase> list;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, patient_id, patient_name, age, gender, condition, severity, severity_rank, arrival_time, arrival_epoch, sequence_num, status, notes FROM emergencies ORDER BY severity_rank DESC, arrival_epoch ASC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        EmergencyCase e;
        e.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        const char* pid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        e.patientId = pid ? pid : "";
        e.patientName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        e.age = sqlite3_column_int(stmt, 3);
        e.gender = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        e.condition = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        e.severity = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        e.severityRank = sqlite3_column_int(stmt, 7);
        e.arrivalTime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        e.arrivalEpoch = sqlite3_column_int64(stmt, 9);
        e.sequenceNum = sqlite3_column_int64(stmt, 10);
        e.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 11));
        const char* n = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 12));
        e.notes = n ? n : "";
        list.push_back(e);
    }
    sqlite3_finalize(stmt);
    return list;
}

// --- Wards & Beds ---
bool Database::insertWard(const Ward& w) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO wards VALUES (?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, w.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, w.name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, w.department.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 4, w.totalBeds);
    sqlite3_bind_int(stmt, 5, w.occupiedBeds);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Ward> Database::getAllWards() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<Ward> list;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, name, department, total_beds, occupied_beds FROM wards ORDER BY id ASC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Ward w;
        w.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        w.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        w.department = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        w.totalBeds = sqlite3_column_int(stmt, 3);
        w.occupiedBeds = sqlite3_column_int(stmt, 4);
        list.push_back(w);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool Database::insertBed(const Bed& b) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO beds VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, b.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, b.wardId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, b.wardName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, b.roomNo.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, b.bedNumber.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, b.status.c_str(), -1, SQLITE_TRANSIENT);
    if (b.patientId.empty()) {
        sqlite3_bind_null(stmt, 7);
    } else {
        sqlite3_bind_text(stmt, 7, b.patientId.c_str(), -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_text(stmt, 8, b.patientName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, b.admittedAt.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

bool Database::updateBed(const Bed& b) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "UPDATE beds SET status=?, patient_id=?, patient_name=?, admitted_at=? WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, b.status.c_str(), -1, SQLITE_TRANSIENT);
    if (b.patientId.empty()) {
        sqlite3_bind_null(stmt, 2);
    } else {
        sqlite3_bind_text(stmt, 2, b.patientId.c_str(), -1, SQLITE_TRANSIENT);
    }
    sqlite3_bind_text(stmt, 3, b.patientName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, b.admittedAt.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, b.id.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<Bed> Database::getAllBeds() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<Bed> list;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, ward_id, ward_name, room_no, bed_number, status, patient_id, patient_name, admitted_at FROM beds ORDER BY ward_id ASC, bed_number ASC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Bed b;
        b.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        b.wardId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* wn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        b.wardName = wn ? wn : "";
        b.roomNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        b.bedNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        b.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        const char* pid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        b.patientId = pid ? pid : "";
        const char* pn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        b.patientName = pn ? pn : "";
        const char* adm = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        b.admittedAt = adm ? adm : "";
        list.push_back(b);
    }
    sqlite3_finalize(stmt);
    return list;
}

bool Database::getBedById(const std::string& id, Bed& outBed) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, ward_id, ward_name, room_no, bed_number, status, patient_id, patient_name, admitted_at FROM beds WHERE id=?;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, id.c_str(), -1, SQLITE_TRANSIENT);
    bool found = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        outBed.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        outBed.wardId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        const char* wn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        outBed.wardName = wn ? wn : "";
        outBed.roomNo = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        outBed.bedNumber = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        outBed.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        const char* pid = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        outBed.patientId = pid ? pid : "";
        const char* pn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        outBed.patientName = pn ? pn : "";
        const char* adm = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        outBed.admittedAt = adm ? adm : "";
        found = true;
    }
    sqlite3_finalize(stmt);
    return found;
}

// --- Visit History ---
bool Database::insertVisit(const VisitRecord& v) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO visit_history VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, v.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, v.patientId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, v.doctorId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, v.doctorName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, v.visitDate.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, v.diagnosis.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 7, v.prescription.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 8, v.notes.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 9, v.createdAt.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<VisitRecord> Database::getVisitsByPatient(const std::string& patientId) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<VisitRecord> list;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, patient_id, doctor_id, doctor_name, visit_date, diagnosis, prescription, notes, created_at FROM visit_history WHERE patient_id=? ORDER BY visit_date DESC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    sqlite3_bind_text(stmt, 1, patientId.c_str(), -1, SQLITE_TRANSIENT);
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        VisitRecord v;
        v.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        v.patientId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        v.doctorId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* dn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        v.doctorName = dn ? dn : "";
        v.visitDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        v.diagnosis = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        const char* pr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        v.prescription = pr ? pr : "";
        const char* n = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        v.notes = n ? n : "";
        v.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        list.push_back(v);
    }
    sqlite3_finalize(stmt);
    return list;
}

std::vector<VisitRecord> Database::getAllVisits() {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<VisitRecord> list;
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "SELECT id, patient_id, doctor_id, doctor_name, visit_date, diagnosis, prescription, notes, created_at FROM visit_history ORDER BY created_at DESC;";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        VisitRecord v;
        v.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        v.patientId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        v.doctorId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        const char* dn = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        v.doctorName = dn ? dn : "";
        v.visitDate = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        v.diagnosis = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        const char* pr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        v.prescription = pr ? pr : "";
        const char* n = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
        v.notes = n ? n : "";
        v.createdAt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 8));
        list.push_back(v);
    }
    sqlite3_finalize(stmt);
    return list;
}

// --- Audit Logs ---
bool Database::insertAuditLog(const AuditLog& l) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    sqlite3_stmt* stmt = nullptr;
    const char* sql = "INSERT INTO audit_logs VALUES (?, ?, ?, ?, ?, ?, ?);";
    if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, l.id.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, l.actionType.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, l.entityName.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, l.entityId.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, l.description.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 6, l.undoable ? 1 : 0);
    sqlite3_bind_text(stmt, 7, l.timestamp.c_str(), -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    return rc == SQLITE_DONE;
}

std::vector<AuditLog> Database::getAllAuditLogs(int limit) {
    std::lock_guard<std::mutex> lock(dbMutex_);
    std::vector<AuditLog> list;
    sqlite3_stmt* stmt = nullptr;
    std::string sql = "SELECT id, action_type, entity_name, entity_id, description, undoable, timestamp FROM audit_logs ORDER BY timestamp DESC LIMIT " + std::to_string(limit) + ";";
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return list;

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        AuditLog l;
        l.id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        l.actionType = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        l.entityName = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        l.entityId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        l.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        l.undoable = (sqlite3_column_int(stmt, 5) == 1);
        l.timestamp = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
        list.push_back(l);
    }
    sqlite3_finalize(stmt);
    return list;
}

} // namespace medicore
