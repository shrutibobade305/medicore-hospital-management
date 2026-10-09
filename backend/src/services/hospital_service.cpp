#include "../../include/services/hospital_service.hpp"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace medicore {

HospitalService::HospitalService(std::shared_ptr<Database> db) : db_(db) {}

std::string HospitalService::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    std::time_t tt = std::chrono::system_clock::to_time_t(now);
    std::tm tmVal;
#if defined(_WIN32)
    localtime_s(&tmVal, &tt);
#else
    localtime_r(&tt, &tmVal);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tmVal, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void HospitalService::logAction(const std::string& actionType, const std::string& entityName, const std::string& entityId, const std::string& description, bool undoable) {
    AuditLog log;
    log.id = "LOG-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count() % 1000000);
    log.actionType = actionType;
    log.entityName = entityName;
    log.entityId = entityId;
    log.description = description;
    log.undoable = undoable;
    log.timestamp = currentTimestamp();
    db_->insertAuditLog(log);
}

bool HospitalService::init() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    if (!db_->open()) {
        std::cerr << "[HospitalService] Failed to open database!" << std::endl;
        return false;
    }
    db_->initializeSchema();
    db_->seedDemoData();

    // 1. Load Patients into Custom Hash Table
    patientHashTable_.clear();
    auto patients = db_->getAllPatients();
    for (const auto& p : patients) {
        patientHashTable_.insert(p);
    }
    std::cout << "[HospitalService] Loaded " << patientHashTable_.size() << " patients into Custom Hash Table." << std::endl;

    // 2. Load Doctors into Custom BST
    doctorBst_.clear();
    auto doctors = db_->getAllDoctors();
    for (const auto& d : doctors) {
        doctorBst_.insert(d);
    }
    std::cout << "[HospitalService] Loaded " << doctorBst_.size() << " doctors into Custom BST." << std::endl;

    // 3. Load Pending Emergencies into Custom Heap
    emergencyHeap_.clear();
    auto emergencies = db_->getAllEmergencies();
    for (const auto& e : emergencies) {
        if (e.status == "Pending") {
            emergencyHeap_.insert(e);
            if (e.sequenceNum >= emergencySequenceCounter_) {
                emergencySequenceCounter_ = e.sequenceNum + 1;
            }
        }
    }
    std::cout << "[HospitalService] Loaded " << emergencyHeap_.size() << " pending cases into Emergency Priority Queue (Heap)." << std::endl;

    // 4. Load Waiting Appointments into Custom FIFO Queue
    waitingQueue_.clear();
    auto appointments = db_->getAllAppointments();
    for (const auto& a : appointments) {
        if (a.status == "Waiting") {
            waitingQueue_.enqueue(a);
        }
    }
    std::cout << "[HospitalService] Loaded " << waitingQueue_.size() << " waiting appointments into FIFO Queue." << std::endl;

    // 5. Load Visits into Custom Doubly Linked List
    visitLinkedList_.clear();
    auto visits = db_->getAllVisits();
    for (const auto& v : visits) {
        visitLinkedList_.insertTail(v);
    }
    std::cout << "[HospitalService] Loaded " << visitLinkedList_.size() << " clinical visits into Doubly Linked List." << std::endl;

    // 6. Hospital Graph is pre-initialized
    std::cout << "[HospitalService] Hospital Navigation Graph initialized with 10 departments." << std::endl;

    return true;
}

bool HospitalService::resetAllData() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    db_->resetDatabase();

    patientHashTable_.clear();
    doctorBst_.clear();
    emergencyHeap_.clear();
    waitingQueue_.clear();
    visitLinkedList_.clear();
    undoStack_.clear();
    hospitalGraph_.initializeDefaultHospitalMap();

    // Reload structures
    auto patients = db_->getAllPatients();
    for (const auto& p : patients) patientHashTable_.insert(p);

    auto doctors = db_->getAllDoctors();
    for (const auto& d : doctors) doctorBst_.insert(d);

    auto emergencies = db_->getAllEmergencies();
    for (const auto& e : emergencies) {
        if (e.status == "Pending") emergencyHeap_.insert(e);
    }

    auto appointments = db_->getAllAppointments();
    for (const auto& a : appointments) {
        if (a.status == "Waiting") waitingQueue_.enqueue(a);
    }

    auto visits = db_->getAllVisits();
    for (const auto& v : visits) visitLinkedList_.insertTail(v);

    logAction("SYSTEM_RESET", "Hospital", "SYSTEM", "Hospital database reset and reseeded with clean demo data.", false);
    return true;
}

// --- Dashboard Summary ---
nlohmann::json HospitalService::getDashboardSummary() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    nlohmann::json summary;

    summary["totalPatients"] = patientHashTable_.size();
    summary["totalDoctors"] = doctorBst_.size();

    auto appointments = db_->getAllAppointments();
    size_t todayAppts = 0;
    for (const auto& a : appointments) {
        if (a.appointmentDate == "2026-10-09") {
            todayAppts++;
        }
    }
    summary["todayAppointments"] = todayAppts;
    summary["totalAppointments"] = appointments.size();

    auto beds = db_->getAllBeds();
    size_t availableBeds = 0;
    size_t occupiedBeds = 0;
    for (const auto& b : beds) {
        if (b.status == "Available") availableBeds++;
        else if (b.status == "Occupied") occupiedBeds++;
    }
    summary["totalBeds"] = beds.size();
    summary["availableBeds"] = availableBeds;
    summary["occupiedBeds"] = occupiedBeds;
    summary["bedOccupancyRate"] = beds.empty() ? 0.0 : (static_cast<double>(occupiedBeds) / beds.size()) * 100.0;

    summary["pendingEmergencies"] = emergencyHeap_.size();
    summary["waitingAppointments"] = waitingQueue_.size();

    // Emergency queue top preview
    nlohmann::json emgPreview = nlohmann::json::array();
    auto sortedEmg = emergencyHeap_.getSortedOrder();
    for (size_t i = 0; i < std::min<size_t>(5, sortedEmg.size()); ++i) {
        emgPreview.push_back(sortedEmg[i]);
    }
    summary["emergencyPreview"] = emgPreview;

    // Recent appointments
    nlohmann::json recentAppts = nlohmann::json::array();
    for (size_t i = 0; i < std::min<size_t>(5, appointments.size()); ++i) {
        recentAppts.push_back(appointments[i]);
    }
    summary["recentAppointments"] = recentAppts;

    // Ward occupancy breakdown
    nlohmann::json wardBreakdown = nlohmann::json::array();
    auto wards = db_->getAllWards();
    for (const auto& w : wards) {
        nlohmann::json wItem;
        wItem["id"] = w.id;
        wItem["name"] = w.name;
        wItem["totalBeds"] = w.totalBeds;

        size_t occ = 0;
        for (const auto& b : beds) {
            if (b.wardId == w.id && b.status == "Occupied") occ++;
        }
        wItem["occupiedBeds"] = occ;
        wItem["availableBeds"] = (w.totalBeds >= occ) ? (w.totalBeds - occ) : 0;
        wardBreakdown.push_back(wItem);
    }
    summary["wardBreakdown"] = wardBreakdown;

    return summary;
}

// --- Patients ---
std::vector<Patient> HospitalService::getAllPatients(const std::string& sortBy, const std::string& filterQuery) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    std::vector<Patient> result = patientHashTable_.getAll();

    // Filtering
    if (!filterQuery.empty()) {
        std::string queryLower = filterQuery;
        std::transform(queryLower.begin(), queryLower.end(), queryLower.begin(), ::tolower);

        std::vector<Patient> filtered;
        for (const auto& p : result) {
            std::string nameLower = p.name;
            std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);
            std::string idLower = p.id;
            std::transform(idLower.begin(), idLower.end(), idLower.begin(), ::tolower);

            if (idLower.find(queryLower) != std::string::npos ||
                nameLower.find(queryLower) != std::string::npos ||
                p.bloodGroup.find(filterQuery) != std::string::npos) {
                filtered.push_back(p);
            }
        }
        result = std::move(filtered);
    }

    // Custom Sorting with Merge Sort!
    if (sortBy == "name") {
        DsaAlgorithms::mergeSortPatientsByName(result);
    } else if (sortBy == "age") {
        DsaAlgorithms::mergeSortPatientsByAge(result);
    } else {
        // Default sort by ID ascending
        std::sort(result.begin(), result.end(), [](const Patient& a, const Patient& b) {
            return a.id < b.id;
        });
    }

    return result;
}

Patient* HospitalService::getPatientById(const std::string& id) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    // Direct lookup in Custom Hash Table!
    return patientHashTable_.search(id);
}

std::string HospitalService::generateNextPatientId() {
    auto allPatients = db_->getAllPatients();
    int maxNum = 1000;
    for (const auto& p : allPatients) {
        if (p.id.rfind("PAT-", 0) == 0) {
            try {
                int num = std::stoi(p.id.substr(4));
                if (num > maxNum) maxNum = num;
            } catch (...) {}
        }
    }
    return "PAT-" + std::to_string(maxNum + 1);
}

bool HospitalService::registerPatient(const Patient& patient, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (patient.name.empty()) {
        errorMsg = "Patient name is required.";
        return false;
    }
    if (patient.age <= 0 || patient.age > 130) {
        errorMsg = "Invalid age provided. Must be between 1 and 130.";
        return false;
    }
    if (patient.gender.empty()) {
        errorMsg = "Gender is required.";
        return false;
    }
    if (patient.contact.empty()) {
        errorMsg = "Contact phone number is required.";
        return false;
    }

    Patient p = patient;
    if (p.id.empty()) {
        p.id = generateNextPatientId();
    } else if (patientHashTable_.contains(p.id)) {
        errorMsg = "Duplicate Patient ID: " + p.id + " already exists in system.";
        return false;
    }

    if (p.registrationDate.empty()) {
        p.registrationDate = currentTimestamp().substr(0, 10);
    }

    // Persist to SQLite
    if (!db_->insertPatient(p)) {
        errorMsg = "Database error inserting patient record.";
        return false;
    }

    // Insert into Custom Hash Table
    patientHashTable_.insert(p);

    logAction("PATIENT_REGISTER", "Patient", p.id, "Registered patient " + p.name + " (" + p.id + ")", false);
    return true;
}

bool HospitalService::updatePatient(const Patient& patient, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (!patientHashTable_.contains(patient.id)) {
        errorMsg = "Patient with ID " + patient.id + " does not exist.";
        return false;
    }

    if (!db_->updatePatient(patient)) {
        errorMsg = "Failed to update patient in database.";
        return false;
    }

    // Update in Custom Hash Table
    patientHashTable_.insert(patient);

    logAction("PATIENT_UPDATE", "Patient", patient.id, "Updated patient record " + patient.name, false);
    return true;
}

bool HospitalService::deletePatient(const std::string& id, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (!patientHashTable_.contains(id)) {
        errorMsg = "Patient with ID " + id + " does not exist.";
        return false;
    }

    // Relationship check: Check if patient currently occupies a bed!
    auto beds = db_->getAllBeds();
    for (const auto& b : beds) {
        if (b.patientId == id && b.status == "Occupied") {
            errorMsg = "Cannot delete patient " + id + ": currently assigned to active bed " + b.id + ". Discharge first.";
            return false;
        }
    }

    // Relationship check: Check for active/waiting appointments
    auto appts = db_->getAllAppointments();
    for (const auto& a : appts) {
        if (a.patientId == id && (a.status == "Scheduled" || a.status == "Waiting" || a.status == "In Progress")) {
            errorMsg = "Cannot delete patient " + id + ": has active appointment " + a.id + ". Cancel or complete appointment first.";
            return false;
        }
    }

    if (!db_->deletePatient(id)) {
        errorMsg = "Database failure while deleting patient.";
        return false;
    }

    // Remove from Custom Hash Table
    patientHashTable_.remove(id);

    logAction("PATIENT_DELETE", "Patient", id, "Deleted patient record " + id, false);
    return true;
}

// --- Doctors ---
std::vector<Doctor> HospitalService::getAllDoctors(const std::string& department, const std::string& search) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    std::vector<Doctor> docs = doctorBst_.inorder();

    std::vector<Doctor> filtered;
    for (const auto& d : docs) {
        if (!department.empty() && d.department != department) continue;
        if (!search.empty()) {
            std::string q = search;
            std::transform(q.begin(), q.end(), q.begin(), ::tolower);
            std::string n = d.name;
            std::transform(n.begin(), n.end(), n.begin(), ::tolower);
            std::string sp = d.specialization;
            std::transform(sp.begin(), sp.end(), sp.begin(), ::tolower);
            if (n.find(q) == std::string::npos && sp.find(q) == std::string::npos && d.id.find(search) == std::string::npos) {
                continue;
            }
        }
        filtered.push_back(d);
    }
    return filtered;
}

std::vector<Doctor> HospitalService::getDoctorsSorted() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    // Inorder traversal of BST returns doctors sorted by Doctor ID in O(n) time!
    return doctorBst_.inorder();
}

Doctor* HospitalService::getDoctorById(const std::string& id) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return doctorBst_.search(id);
}

std::string HospitalService::generateNextDoctorId() {
    auto allDocs = db_->getAllDoctors();
    int maxNum = 100;
    for (const auto& d : allDocs) {
        if (d.id.rfind("DOC-", 0) == 0) {
            try {
                int num = std::stoi(d.id.substr(4));
                if (num > maxNum) maxNum = num;
            } catch (...) {}
        }
    }
    return "DOC-" + std::to_string(maxNum + 1);
}

bool HospitalService::addDoctor(const Doctor& doctor, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (doctor.name.empty()) {
        errorMsg = "Doctor name is required.";
        return false;
    }
    if (doctor.department.empty()) {
        errorMsg = "Department is required.";
        return false;
    }

    Doctor d = doctor;
    if (d.id.empty()) {
        d.id = generateNextDoctorId();
    } else if (doctorBst_.search(d.id) != nullptr) {
        errorMsg = "Doctor with ID " + d.id + " already exists.";
        return false;
    }

    if (d.availability.empty()) d.availability = "Available";
    if (d.roomNo.empty()) d.roomNo = "OPD-101";

    if (!db_->insertDoctor(d)) {
        errorMsg = "Failed to insert doctor into database.";
        return false;
    }

    // Insert into Custom BST
    doctorBst_.insert(d);

    logAction("DOCTOR_ADD", "Doctor", d.id, "Added doctor " + d.name + " (" + d.department + ")", false);
    return true;
}

bool HospitalService::updateDoctor(const Doctor& doctor, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (!doctorBst_.search(doctor.id)) {
        errorMsg = "Doctor " + doctor.id + " not found.";
        return false;
    }

    if (!db_->updateDoctor(doctor)) {
        errorMsg = "Database error updating doctor.";
        return false;
    }

    // Re-insert into BST updates data
    doctorBst_.insert(doctor);

    logAction("DOCTOR_UPDATE", "Doctor", doctor.id, "Updated doctor profile for " + doctor.name, false);
    return true;
}

bool HospitalService::deleteDoctor(const std::string& id, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (!doctorBst_.search(id)) {
        errorMsg = "Doctor " + id + " does not exist.";
        return false;
    }

    // Check if doctor has active scheduled appointments
    auto appts = db_->getAllAppointments();
    for (const auto& a : appts) {
        if (a.doctorId == id && (a.status == "Scheduled" || a.status == "Waiting" || a.status == "In Progress")) {
            errorMsg = "Cannot delete doctor: currently has active appointments assigned.";
            return false;
        }
    }

    if (!db_->deleteDoctor(id)) {
        errorMsg = "Database error deleting doctor.";
        return false;
    }

    // Remove from Custom BST
    doctorBst_.remove(id);

    logAction("DOCTOR_DELETE", "Doctor", id, "Removed doctor record " + id, false);
    return true;
}

// --- Appointments ---
std::vector<Appointment> HospitalService::getAllAppointments(const std::string& date, const std::string& doctorId, const std::string& status) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    auto list = db_->getAllAppointments();
    std::vector<Appointment> filtered;

    for (const auto& a : list) {
        if (!date.empty() && a.appointmentDate != date) continue;
        if (!doctorId.empty() && a.doctorId != doctorId) continue;
        if (!status.empty() && a.status != status) continue;
        filtered.push_back(a);
    }

    // Custom Merge Sort by Appointment Date & Time!
    DsaAlgorithms::mergeSortAppointmentsByTime(filtered);
    return filtered;
}

bool HospitalService::scheduleAppointment(const Appointment& appt, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (appt.patientId.empty() || appt.doctorId.empty()) {
        errorMsg = "Both Patient and Doctor must be selected.";
        return false;
    }
    if (appt.appointmentDate.empty() || appt.appointmentTime.empty()) {
        errorMsg = "Appointment date and time are required.";
        return false;
    }

    // Check patient existence
    auto patient = patientHashTable_.search(appt.patientId);
    if (!patient) {
        errorMsg = "Selected patient does not exist in registry.";
        return false;
    }

    // Check doctor existence
    auto doctor = doctorBst_.search(appt.doctorId);
    if (!doctor) {
        errorMsg = "Selected doctor does not exist in registry.";
        return false;
    }

    // Check conflict: doctor cannot have overlapping active appointment at same date & time!
    auto existingAppts = db_->getAllAppointments();
    for (const auto& existing : existingAppts) {
        if (existing.doctorId == appt.doctorId &&
            existing.appointmentDate == appt.appointmentDate &&
            existing.appointmentTime == appt.appointmentTime &&
            existing.status != "Cancelled" && existing.status != "Completed") {
            errorMsg = "Conflict: " + doctor->name + " already has an active appointment at " +
                       appt.appointmentTime + " on " + appt.appointmentDate + ".";
            return false;
        }
    }

    Appointment a = appt;
    if (a.id.empty()) {
        int maxNum = 2000;
        for (const auto& ex : existingAppts) {
            if (ex.id.rfind("APT-", 0) == 0) {
                try {
                    int num = std::stoi(ex.id.substr(4));
                    if (num > maxNum) maxNum = num;
                } catch (...) {}
            }
        }
        a.id = "APT-" + std::to_string(maxNum + 1);
    }
    a.patientName = patient->name;
    a.doctorName = doctor->name;
    if (a.status.empty()) a.status = "Scheduled";
    a.createdAt = currentTimestamp();

    if (!db_->insertAppointment(a)) {
        errorMsg = "Database error creating appointment.";
        return false;
    }

    if (a.status == "Waiting") {
        waitingQueue_.enqueue(a);
    }

    logAction("APPOINTMENT_SCHEDULE", "Appointment", a.id, "Scheduled appointment for " + patient->name + " with " + doctor->name, false);
    return true;
}

bool HospitalService::updateAppointment(const Appointment& appt, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    Appointment current;
    if (!db_->getAppointmentById(appt.id, current)) {
        errorMsg = "Appointment " + appt.id + " not found.";
        return false;
    }

    // Check doctor conflict if date/time changed
    if (appt.appointmentDate != current.appointmentDate || appt.appointmentTime != current.appointmentTime) {
        auto all = db_->getAllAppointments();
        for (const auto& existing : all) {
            if (existing.id != appt.id &&
                existing.doctorId == appt.doctorId &&
                existing.appointmentDate == appt.appointmentDate &&
                existing.appointmentTime == appt.appointmentTime &&
                existing.status != "Cancelled" && existing.status != "Completed") {
                errorMsg = "Conflict: Doctor is already booked at that time.";
                return false;
            }
        }
    }

    if (!db_->updateAppointment(appt)) {
        errorMsg = "Database error updating appointment.";
        return false;
    }

    // Rebuild waiting queue
    waitingQueue_.clear();
    auto all = db_->getAllAppointments();
    for (const auto& item : all) {
        if (item.status == "Waiting") {
            waitingQueue_.enqueue(item);
        }
    }

    logAction("APPOINTMENT_UPDATE", "Appointment", appt.id, "Updated appointment status to " + appt.status, false);
    return true;
}

bool HospitalService::cancelAppointment(const std::string& id, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    Appointment a;
    if (!db_->getAppointmentById(id, a)) {
        errorMsg = "Appointment not found.";
        return false;
    }

    a.status = "Cancelled";
    if (!db_->updateAppointment(a)) {
        errorMsg = "Database error cancelling appointment.";
        return false;
    }

    // Rebuild waiting queue
    waitingQueue_.clear();
    auto all = db_->getAllAppointments();
    for (const auto& item : all) {
        if (item.status == "Waiting") waitingQueue_.enqueue(item);
    }

    logAction("APPOINTMENT_CANCEL", "Appointment", id, "Cancelled appointment " + id, false);
    return true;
}

std::vector<Appointment> HospitalService::getWaitingQueue() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return waitingQueue_.getAll();
}

// --- Emergency & Triage ---
std::vector<EmergencyCase> HospitalService::getPendingEmergencies() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    // Returns cases strictly sorted by priority (Critical > High > Medium > Low, then arrival time)
    return emergencyHeap_.getSortedOrder();
}

std::vector<EmergencyCase> HospitalService::getAllEmergencies() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return db_->getAllEmergencies();
}

bool HospitalService::registerEmergency(const EmergencyCase& emg, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (emg.patientName.empty()) {
        errorMsg = "Emergency patient name is required.";
        return false;
    }
    if (emg.condition.empty()) {
        errorMsg = "Clinical condition / symptoms description is required.";
        return false;
    }

    EmergencyCase c = emg;
    if (c.id.empty()) {
        c.id = "EMG-" + std::to_string(5000 + emergencySequenceCounter_);
    }

    c.severityRank = static_cast<int>(stringToSeverity(c.severity));
    if (c.arrivalTime.empty()) {
        auto now = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(now);
        std::tm tmVal;
#if defined(_WIN32)
        localtime_s(&tmVal, &tt);
#else
        localtime_r(&tt, &tmVal);
#endif
        std::ostringstream oss;
        oss << std::put_time(&tmVal, "%I:%M %p");
        c.arrivalTime = oss.str();
    }
    if (c.arrivalEpoch == 0) {
        c.arrivalEpoch = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    c.sequenceNum = emergencySequenceCounter_++;
    c.status = "Pending";

    // Persist to database
    if (!db_->insertEmergency(c)) {
        errorMsg = "Database error recording emergency case.";
        return false;
    }

    // Insert into Custom Binary Max-Heap!
    emergencyHeap_.insert(c);

    logAction("EMERGENCY_REGISTER", "Emergency", c.id, "Registered " + c.severity + " emergency for " + c.patientName + " (" + c.condition + ")", false);
    return true;
}

bool HospitalService::dispatchNextEmergency(EmergencyCase& dispatchedCase, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (emergencyHeap_.isEmpty()) {
        errorMsg = "Emergency priority queue is currently empty. No pending cases to dispatch.";
        return false;
    }

    // Extract maximum priority case from Custom Binary Heap
    dispatchedCase = emergencyHeap_.extractMax();
    dispatchedCase.status = "Dispatched";

    // Update in SQLite
    if (!db_->updateEmergency(dispatchedCase)) {
        // Rollback heap
        emergencyHeap_.insert(dispatchedCase);
        errorMsg = "Database error updating dispatched emergency case.";
        return false;
    }

    // Record action in Undo Stack
    ActionRecord act;
    act.id = "ACT-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count() % 1000000);
    act.actionType = "EMERGENCY_DISPATCH";
    act.entityId = dispatchedCase.id;
    nlohmann::json d;
    d["caseId"] = dispatchedCase.id;
    d["priorStatus"] = "Pending";
    act.details = d.dump();
    act.timestamp = currentTimestamp();
    undoStack_.push(act);

    logAction("EMERGENCY_DISPATCH", "Emergency", dispatchedCase.id, "Dispatched highest-priority emergency " + dispatchedCase.id + " (" + dispatchedCase.patientName + ", " + dispatchedCase.severity + ")", true);
    return true;
}

bool HospitalService::completeEmergency(const std::string& caseId, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    auto all = db_->getAllEmergencies();
    EmergencyCase target;
    bool found = false;
    for (const auto& e : all) {
        if (e.id == caseId) {
            target = e;
            found = true;
            break;
        }
    }

    if (!found) {
        errorMsg = "Emergency case not found.";
        return false;
    }

    target.status = "Completed";
    if (!db_->updateEmergency(target)) {
        errorMsg = "Database error completing emergency case.";
        return false;
    }

    // Remove from in-memory heap if still there
    emergencyHeap_.removeById(caseId);

    logAction("EMERGENCY_COMPLETE", "Emergency", caseId, "Completed emergency triage care for case " + caseId, false);
    return true;
}

// --- Bed & Ward Management ---
std::vector<Bed> HospitalService::getAllBeds(const std::string& wardId, const std::string& status) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    auto list = db_->getAllBeds();
    std::vector<Bed> filtered;

    for (const auto& b : list) {
        if (!wardId.empty() && b.wardId != wardId) continue;
        if (!status.empty() && b.status != status) continue;
        filtered.push_back(b);
    }
    return filtered;
}

std::vector<Ward> HospitalService::getAllWards() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return db_->getAllWards();
}

bool HospitalService::allocateBed(const std::string& bedId, const std::string& patientId, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    Bed bed;
    if (!db_->getBedById(bedId, bed)) {
        errorMsg = "Bed " + bedId + " not found.";
        return false;
    }

    if (bed.status == "Occupied") {
        errorMsg = "Bed " + bedId + " is already occupied by " + bed.patientName + ".";
        return false;
    }
    if (bed.status == "Out of Service") {
        errorMsg = "Bed " + bedId + " is currently out of service for maintenance.";
        return false;
    }

    // Check if patient exists
    auto patient = patientHashTable_.search(patientId);
    if (!patient) {
        errorMsg = "Patient " + patientId + " does not exist.";
        return false;
    }

    // Check if patient is already assigned to another active bed!
    auto allBeds = db_->getAllBeds();
    for (const auto& b : allBeds) {
        if (b.patientId == patientId && b.status == "Occupied") {
            errorMsg = "Patient " + patient->name + " is already admitted in " + b.wardName + " (" + b.bedNumber + "). Cannot assign multiple beds.";
            return false;
        }
    }

    // Save prior state for UNDO STACK!
    ActionRecord act;
    act.id = "ACT-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count() % 1000000);
    act.actionType = "BED_ALLOCATE";
    act.entityId = bedId;
    nlohmann::json d;
    d["bedId"] = bedId;
    d["priorStatus"] = bed.status;
    d["priorPatientId"] = bed.patientId;
    d["priorPatientName"] = bed.patientName;
    d["priorAdmittedAt"] = bed.admittedAt;
    d["assignedPatientId"] = patientId;
    act.details = d.dump();
    act.timestamp = currentTimestamp();

    bed.status = "Occupied";
    bed.patientId = patientId;
    bed.patientName = patient->name;
    bed.admittedAt = currentTimestamp();

    if (!db_->updateBed(bed)) {
        errorMsg = "Database error assigning bed.";
        return false;
    }

    // Push action onto Custom Undo Stack!
    undoStack_.push(act);

    logAction("BED_ALLOCATE", "Bed", bedId, "Allocated " + bed.wardName + " " + bed.bedNumber + " to " + patient->name, true);
    return true;
}

bool HospitalService::releaseBed(const std::string& bedId, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    Bed bed;
    if (!db_->getBedById(bedId, bed)) {
        errorMsg = "Bed not found.";
        return false;
    }

    if (bed.status != "Occupied") {
        errorMsg = "Bed is not currently occupied.";
        return false;
    }

    // Save prior state for UNDO STACK!
    ActionRecord act;
    act.id = "ACT-" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count() % 1000000);
    act.actionType = "BED_RELEASE";
    act.entityId = bedId;
    nlohmann::json d;
    d["bedId"] = bedId;
    d["priorStatus"] = bed.status;
    d["priorPatientId"] = bed.patientId;
    d["priorPatientName"] = bed.patientName;
    d["priorAdmittedAt"] = bed.admittedAt;
    act.details = d.dump();
    act.timestamp = currentTimestamp();

    std::string releasedPatient = bed.patientName;
    bed.status = "Available";
    bed.patientId = "";
    bed.patientName = "";
    bed.admittedAt = "";

    if (!db_->updateBed(bed)) {
        errorMsg = "Database error discharging patient from bed.";
        return false;
    }

    // Push action onto Custom Undo Stack!
    undoStack_.push(act);

    logAction("BED_RELEASE", "Bed", bedId, "Discharged patient " + releasedPatient + " and released " + bed.bedNumber, true);
    return true;
}

bool HospitalService::undoLastAdministrativeAction(std::string& feedbackMsg, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (undoStack_.isEmpty()) {
        errorMsg = "Undo stack is empty. No reversible administrative actions available.";
        return false;
    }

    ActionRecord lastAction = undoStack_.pop();
    nlohmann::json details = nlohmann::json::parse(lastAction.details);

    if (lastAction.actionType == "BED_ALLOCATE") {
        // Revert bed allocation -> release bed back to Available
        std::string bedId = details["bedId"];
        Bed bed;
        if (db_->getBedById(bedId, bed)) {
            bed.status = details.value("priorStatus", "Available");
            bed.patientId = details.value("priorPatientId", "");
            bed.patientName = details.value("priorPatientName", "");
            bed.admittedAt = details.value("priorAdmittedAt", "");
            db_->updateBed(bed);
            feedbackMsg = "Undone bed allocation for " + bedId + ". Bed restored to Available.";
            logAction("UNDO_BED_ALLOCATE", "Bed", bedId, "Undid bed allocation for " + bedId, false);
            return true;
        }
    } else if (lastAction.actionType == "BED_RELEASE") {
        // Revert bed release -> restore patient back to bed
        std::string bedId = details["bedId"];
        Bed bed;
        if (db_->getBedById(bedId, bed)) {
            bed.status = "Occupied";
            bed.patientId = details.value("priorPatientId", "");
            bed.patientName = details.value("priorPatientName", "");
            bed.admittedAt = details.value("priorAdmittedAt", currentTimestamp());
            db_->updateBed(bed);
            feedbackMsg = "Undone bed release for " + bedId + ". Restored patient " + bed.patientName + " to bed.";
            logAction("UNDO_BED_RELEASE", "Bed", bedId, "Undid bed release for " + bedId, false);
            return true;
        }
    } else if (lastAction.actionType == "EMERGENCY_DISPATCH") {
        // Revert emergency dispatch -> restore case back into pending Priority Queue
        std::string caseId = details["caseId"];
        auto all = db_->getAllEmergencies();
        for (auto e : all) {
            if (e.id == caseId) {
                e.status = "Pending";
                db_->updateEmergency(e);
                emergencyHeap_.insert(e);
                feedbackMsg = "Undone emergency dispatch for " + caseId + ". Re-inserted into priority heap.";
                logAction("UNDO_EMERGENCY_DISPATCH", "Emergency", caseId, "Undid emergency dispatch for " + caseId, false);
                return true;
            }
        }
    }

    errorMsg = "Unrecognized or corrupted action type in undo stack.";
    return false;
}

// --- Hospital Navigation ---
nlohmann::json HospitalService::getHospitalMap() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return hospitalGraph_.getGraphData();
}

DijkstraResult HospitalService::findRoute(const std::string& from, const std::string& to) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    // Calculates shortest path via Dijkstra's algorithm on the C++ weighted graph!
    return hospitalGraph_.findShortestPath(from, to);
}

TraversalResult HospitalService::traverseGraph(const std::string& startId, const std::string& type) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    if (type == "DFS") {
        return hospitalGraph_.dfs(startId);
    }
    return hospitalGraph_.bfs(startId);
}

bool HospitalService::toggleCorridorClosure(const std::string& from, const std::string& to, bool closed) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return hospitalGraph_.setEdgeClosure(from, to, closed);
}

// --- Clinical Visits ---
std::vector<VisitRecord> HospitalService::getPatientVisits(const std::string& patientId) {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return visitLinkedList_.getByPatientId(patientId);
}

bool HospitalService::addVisitRecord(const VisitRecord& record, std::string& errorMsg) {
    std::lock_guard<std::mutex> lock(serviceMutex_);

    if (record.patientId.empty() || record.doctorId.empty()) {
        errorMsg = "Patient and Doctor must be specified for clinical note.";
        return false;
    }
    if (record.diagnosis.empty()) {
        errorMsg = "Clinical diagnosis is required.";
        return false;
    }

    VisitRecord v = record;
    if (v.id.empty()) {
        v.id = "VST-" + std::to_string(9000 + visitLinkedList_.size() + 1);
    }
    if (v.visitDate.empty()) v.visitDate = currentTimestamp().substr(0, 10);
    v.createdAt = currentTimestamp();

    if (!db_->insertVisit(v)) {
        errorMsg = "Database error inserting clinical visit.";
        return false;
    }

    // Insert into Custom Doubly Linked List
    visitLinkedList_.insertTail(v);

    logAction("VISIT_ADD", "Visit", v.id, "Added clinical consultation for patient " + v.patientId, false);
    return true;
}

std::vector<AuditLog> HospitalService::getActivityLogs() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return db_->getAllAuditLogs(100);
}

// --- DSA Visualizer Data Feeds ---
nlohmann::json HospitalService::getHashTableVisualizerData() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return patientHashTable_.getDebugStructure();
}

nlohmann::json HospitalService::getBstVisualizerData() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return doctorBst_.getDebugStructure();
}

nlohmann::json HospitalService::getHeapVisualizerData() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return emergencyHeap_.getDebugStructure();
}

nlohmann::json HospitalService::getQueueVisualizerData() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return waitingQueue_.getDebugStructure();
}

nlohmann::json HospitalService::getStackVisualizerData() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return undoStack_.getDebugStructure();
}

nlohmann::json HospitalService::getLinkedListVisualizerData() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return visitLinkedList_.getDebugStructure();
}

nlohmann::json HospitalService::getGraphVisualizerData() {
    std::lock_guard<std::mutex> lock(serviceMutex_);
    return hospitalGraph_.getGraphData();
}

nlohmann::json HospitalService::runMergeSortDemo(const std::vector<int>& inputArr) {
    std::vector<int> arr = inputArr;
    auto steps = DsaAlgorithms::traceMergeSort(arr);
    nlohmann::json j;
    j["steps"] = steps;
    j["sortedArray"] = arr;
    return j;
}

nlohmann::json HospitalService::runBinarySearchDemo(const std::vector<int>& sortedArr, int target) {
    auto result = DsaAlgorithms::binarySearch(sortedArr, target);
    nlohmann::json j;
    j["found"] = result.found;
    j["index"] = result.index;
    j["target"] = result.target;
    j["totalComparisons"] = result.totalComparisons;
    j["steps"] = result.steps;
    return j;
}

} // namespace medicore
