#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "../models/models.hpp"
#include "../third_party/sqlite3.h"

namespace medicore {

class Database {
public:
    explicit Database(const std::string& dbPath);
    ~Database();

    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    bool open();
    void close();

    // Schema & Demo Seeding
    bool initializeSchema();
    bool seedDemoData();
    bool resetDatabase();

    // Patient CRUD
    bool insertPatient(const Patient& patient);
    bool updatePatient(const Patient& patient);
    bool deletePatient(const std::string& id);
    std::vector<Patient> getAllPatients();
    bool getPatientById(const std::string& id, Patient& outPatient);

    // Doctor CRUD
    bool insertDoctor(const Doctor& doctor);
    bool updateDoctor(const Doctor& doctor);
    bool deleteDoctor(const std::string& id);
    std::vector<Doctor> getAllDoctors();
    bool getDoctorById(const std::string& id, Doctor& outDoctor);

    // Appointment CRUD
    bool insertAppointment(const Appointment& appt);
    bool updateAppointment(const Appointment& appt);
    bool deleteAppointment(const std::string& id);
    std::vector<Appointment> getAllAppointments();
    bool getAppointmentById(const std::string& id, Appointment& outAppt);

    // Emergency Cases
    bool insertEmergency(const EmergencyCase& emg);
    bool updateEmergency(const EmergencyCase& emg);
    std::vector<EmergencyCase> getAllEmergencies();

    // Bed & Ward Management
    bool insertWard(const Ward& ward);
    std::vector<Ward> getAllWards();
    bool insertBed(const Bed& bed);
    bool updateBed(const Bed& bed);
    std::vector<Bed> getAllBeds();
    bool getBedById(const std::string& id, Bed& outBed);

    // Visit History
    bool insertVisit(const VisitRecord& visit);
    std::vector<VisitRecord> getVisitsByPatient(const std::string& patientId);
    std::vector<VisitRecord> getAllVisits();

    // Audit Logs
    bool insertAuditLog(const AuditLog& log);
    std::vector<AuditLog> getAllAuditLogs(int limit = 100);

    // Transaction utilities
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

private:
    std::string dbPath_;
    sqlite3* db_ = nullptr;
    std::mutex dbMutex_;

    bool execute(const std::string& sql);
};

} // namespace medicore
