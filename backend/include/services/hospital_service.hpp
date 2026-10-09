#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "../models/models.hpp"
#include "../repositories/database.hpp"
#include "../dsa/patient_hash_table.hpp"
#include "../dsa/visit_linked_list.hpp"
#include "../dsa/appointment_queue.hpp"
#include "../dsa/emergency_heap.hpp"
#include "../dsa/doctor_bst.hpp"
#include "../dsa/undo_stack.hpp"
#include "../dsa/hospital_graph.hpp"
#include "../dsa/dsa_algorithms.hpp"
#include "../third_party/json.hpp"

namespace medicore {

class HospitalService {
public:
    explicit HospitalService(std::shared_ptr<Database> db);
    ~HospitalService() = default;

    // Initialize service and load databases into DSA structures
    bool init();
    bool resetAllData();

    // Dashboard
    nlohmann::json getDashboardSummary();

    // Patients
    std::vector<Patient> getAllPatients(const std::string& sortBy = "", const std::string& filterQuery = "");
    Patient* getPatientById(const std::string& id);
    bool registerPatient(const Patient& patient, std::string& errorMsg);
    bool updatePatient(const Patient& patient, std::string& errorMsg);
    bool deletePatient(const std::string& id, std::string& errorMsg);
    std::string generateNextPatientId();

    // Doctors
    std::vector<Doctor> getAllDoctors(const std::string& department = "", const std::string& search = "");
    std::vector<Doctor> getDoctorsSorted(); // BST Inorder traversal!
    Doctor* getDoctorById(const std::string& id);
    bool addDoctor(const Doctor& doctor, std::string& errorMsg);
    bool updateDoctor(const Doctor& doctor, std::string& errorMsg);
    bool deleteDoctor(const std::string& id, std::string& errorMsg);
    std::string generateNextDoctorId();

    // Appointments
    std::vector<Appointment> getAllAppointments(const std::string& date = "", const std::string& doctorId = "", const std::string& status = "");
    bool scheduleAppointment(const Appointment& appt, std::string& errorMsg);
    bool updateAppointment(const Appointment& appt, std::string& errorMsg);
    bool cancelAppointment(const std::string& id, std::string& errorMsg);
    std::vector<Appointment> getWaitingQueue();

    // Emergency & Triage
    std::vector<EmergencyCase> getPendingEmergencies();
    std::vector<EmergencyCase> getAllEmergencies();
    bool registerEmergency(const EmergencyCase& emg, std::string& errorMsg);
    bool dispatchNextEmergency(EmergencyCase& dispatchedCase, std::string& errorMsg);
    bool completeEmergency(const std::string& caseId, std::string& errorMsg);

    // Bed & Ward Management
    std::vector<Bed> getAllBeds(const std::string& wardId = "", const std::string& status = "");
    std::vector<Ward> getAllWards();
    bool allocateBed(const std::string& bedId, const std::string& patientId, std::string& errorMsg);
    bool releaseBed(const std::string& bedId, std::string& errorMsg);
    bool undoLastAdministrativeAction(std::string& feedbackMsg, std::string& errorMsg);

    // Hospital Navigation & Graph
    nlohmann::json getHospitalMap();
    DijkstraResult findRoute(const std::string& from, const std::string& to);
    TraversalResult traverseGraph(const std::string& startId, const std::string& type); // BFS or DFS
    bool toggleCorridorClosure(const std::string& from, const std::string& to, bool closed);

    // Clinical Visit History
    std::vector<VisitRecord> getPatientVisits(const std::string& patientId);
    bool addVisitRecord(const VisitRecord& record, std::string& errorMsg);

    // Activity Logs
    std::vector<AuditLog> getActivityLogs();

    // DSA Visualizer Data Feeds
    nlohmann::json getHashTableVisualizerData();
    nlohmann::json getBstVisualizerData();
    nlohmann::json getHeapVisualizerData();
    nlohmann::json getQueueVisualizerData();
    nlohmann::json getStackVisualizerData();
    nlohmann::json getLinkedListVisualizerData();
    nlohmann::json getGraphVisualizerData();
    nlohmann::json runMergeSortDemo(const std::vector<int>& inputArr);
    nlohmann::json runBinarySearchDemo(const std::vector<int>& sortedArr, int target);

private:
    void logAction(const std::string& actionType, const std::string& entityName, const std::string& entityId, const std::string& description, bool undoable);
    std::string currentTimestamp() const;

    std::shared_ptr<Database> db_;
    std::mutex serviceMutex_;

    // Custom DSA structures in active memory
    PatientHashTable patientHashTable_;
    DoctorBST doctorBst_;
    EmergencyPriorityQueue emergencyHeap_;
    AppointmentQueue waitingQueue_;
    VisitLinkedList visitLinkedList_;
    UndoStack undoStack_;
    HospitalGraph hospitalGraph_;

    uint64_t emergencySequenceCounter_ = 100;
};

} // namespace medicore
