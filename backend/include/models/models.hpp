#pragma once

#include <string>
#include <vector>
#include "../third_party/json.hpp"

namespace medicore {

struct Patient {
    std::string id;             // e.g. "PAT-1001"
    std::string name;
    int age = 0;
    std::string gender;         // "Male", "Female", "Other"
    std::string contact;
    std::string address;
    std::string bloodGroup;     // "A+", "O-", etc.
    std::string registrationDate;
    std::string notes;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Patient, id, name, age, gender, contact, address, bloodGroup, registrationDate, notes)
};

struct Doctor {
    std::string id;             // e.g. "DOC-101"
    std::string name;
    std::string department;     // "Cardiology", "Neurology", "Orthopedics", etc.
    std::string specialization;
    std::string contact;
    std::string email;
    std::string availability;   // "Available", "In Consultation", "Off Duty"
    std::string roomNo;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Doctor, id, name, department, specialization, contact, email, availability, roomNo)
};

struct Appointment {
    std::string id;             // e.g. "APT-2001"
    std::string patientId;
    std::string patientName;    // Cached for convenience
    std::string doctorId;
    std::string doctorName;     // Cached for convenience
    std::string appointmentDate;// "YYYY-MM-DD"
    std::string appointmentTime;// "HH:MM"
    std::string reason;
    std::string status;         // "Scheduled", "Waiting", "In Progress", "Completed", "Cancelled"
    std::string createdAt;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Appointment, id, patientId, patientName, doctorId, doctorName, appointmentDate, appointmentTime, reason, status, createdAt)
};

enum class EmergencySeverity {
    LOW = 1,
    MEDIUM = 2,
    HIGH = 3,
    CRITICAL = 4
};

inline std::string severityToString(EmergencySeverity sev) {
    switch (sev) {
        case EmergencySeverity::CRITICAL: return "Critical";
        case EmergencySeverity::HIGH: return "High";
        case EmergencySeverity::MEDIUM: return "Medium";
        case EmergencySeverity::LOW: return "Low";
        default: return "Medium";
    }
}

inline EmergencySeverity stringToSeverity(const std::string& str) {
    if (str == "Critical") return EmergencySeverity::CRITICAL;
    if (str == "High") return EmergencySeverity::HIGH;
    if (str == "Medium") return EmergencySeverity::MEDIUM;
    if (str == "Low") return EmergencySeverity::LOW;
    return EmergencySeverity::MEDIUM;
}

struct EmergencyCase {
    std::string id;             // e.g. "EMG-5001"
    std::string patientId;
    std::string patientName;
    int age = 0;
    std::string gender;
    std::string condition;
    std::string severity;       // "Critical", "High", "Medium", "Low"
    int severityRank = 2;       // 4 for Critical down to 1 for Low
    std::string arrivalTime;    // ISO string or formatted time
    int64_t arrivalEpoch = 0;   // Epoch timestamp for stable FIFO tie-breaking
    uint64_t sequenceNum = 0;   // Deterministic sequence counter
    std::string status;         // "Pending", "Dispatched", "Completed"
    std::string notes;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(EmergencyCase, id, patientId, patientName, age, gender, condition, severity, severityRank, arrivalTime, arrivalEpoch, sequenceNum, status, notes)
};

struct Bed {
    std::string id;             // e.g. "BED-W1-01"
    std::string wardId;         // e.g. "WARD-1"
    std::string wardName;
    std::string roomNo;
    std::string bedNumber;
    std::string status;         // "Available", "Occupied", "Out of Service"
    std::string patientId;      // Empty if Available
    std::string patientName;    // Empty if Available
    std::string admittedAt;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Bed, id, wardId, wardName, roomNo, bedNumber, status, patientId, patientName, admittedAt)
};

struct Ward {
    std::string id;             // e.g. "WARD-1"
    std::string name;
    std::string department;
    int totalBeds = 0;
    int occupiedBeds = 0;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(Ward, id, name, department, totalBeds, occupiedBeds)
};

struct VisitRecord {
    std::string id;             // e.g. "VST-9001"
    std::string patientId;
    std::string doctorId;
    std::string doctorName;
    std::string visitDate;
    std::string diagnosis;
    std::string prescription;
    std::string notes;
    std::string createdAt;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(VisitRecord, id, patientId, doctorId, doctorName, visitDate, diagnosis, prescription, notes, createdAt)
};

struct ActionRecord {
    std::string id;
    std::string actionType;     // "BED_ALLOCATE", "BED_RELEASE", "EMERGENCY_DISPATCH"
    std::string entityId;
    std::string details;        // JSON formatted payload to restore prior state
    std::string timestamp;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ActionRecord, id, actionType, entityId, details, timestamp)
};

struct AuditLog {
    std::string id;
    std::string actionType;
    std::string entityName;
    std::string entityId;
    std::string description;
    bool undoable = false;
    std::string timestamp;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AuditLog, id, actionType, entityName, entityId, description, undoable, timestamp)
};

} // namespace medicore
