#include <iostream>
#include <cassert>
#include <memory>
#include <cstdio>

#include "../include/repositories/database.hpp"
#include "../include/services/hospital_service.hpp"

using namespace medicore;

int beTestsPassed = 0;
int beTestsTotal = 0;

#define BE_TEST(cond, msg) \
    do { \
        beTestsTotal++; \
        if (cond) { \
            beTestsPassed++; \
            std::cout << "  [PASS] " << msg << std::endl; \
        } else { \
            std::cerr << "  [FAIL] " << msg << " (" << #cond << ")" << std::endl; \
        } \
    } while (0)

int main() {
    std::cout << "=================================================" << std::endl;
    std::cout << "  MediCore Backend & Database Automated Tests    " << std::endl;
    std::cout << "=================================================" << std::endl;

    std::string testDbFile = "test_hospital.db";
    std::remove(testDbFile.c_str()); // Clean test db

    auto db = std::make_shared<Database>(testDbFile);
    auto service = std::make_shared<HospitalService>(db);

    BE_TEST(service->init() == true, "Service and SQLite database initialized");

    // 1. Check Seeding
    auto summary = service->getDashboardSummary();
    BE_TEST(summary["totalPatients"] >= 8, "Initial patients seeded into database & Hash Table");
    BE_TEST(summary["totalDoctors"] >= 6, "Initial doctors seeded into database & BST");
    BE_TEST(summary["totalBeds"] >= 16, "Initial beds and wards seeded");
    BE_TEST(summary["pendingEmergencies"] >= 5, "Initial emergency cases loaded into Priority Queue");

    // 2. Duplicate Patient Prevention
    std::string err;
    Patient dupP{"PAT-1001", "Arun Imposter", 30, "Male", "123", "", "O+", "2026-10-09", ""};
    bool resDup = service->registerPatient(dupP, err);
    BE_TEST(resDup == false && !err.empty(), "Prevented registration with duplicate Patient ID");

    // 3. New Patient Registration
    Patient newP{"PAT-7777", "Test Patient", 29, "Female", "+91 9999900000", "Koramangala", "B+", "2026-10-09", "Healthy"};
    BE_TEST(service->registerPatient(newP, err) == true, "Registered new unique patient PAT-7777");
    BE_TEST(service->getPatientById("PAT-7777") != nullptr, "Newly registered patient retrievable from Hash Table");

    // 4. Appointment Doctor Conflict Detection
    // Dr. Rajesh Rao already has appointment at 11:15 on 2026-10-09
    Appointment conflictAppt{"", "PAT-7777", "Test Patient", "DOC-101", "Dr. Rajesh Rao", "2026-10-09", "11:15", "Checkup", "Scheduled", ""};
    bool schedRes = service->scheduleAppointment(conflictAppt, err);
    BE_TEST(schedRes == false, "Detected and prevented doctor appointment time conflict");

    // Non-conflicting appointment schedules properly
    Appointment validAppt{"", "PAT-7777", "Test Patient", "DOC-101", "Dr. Rajesh Rao", "2026-10-09", "16:00", "Evening Checkup", "Scheduled", ""};
    BE_TEST(service->scheduleAppointment(validAppt, err) == true, "Scheduled valid non-conflicting appointment");

    // 5. Bed Allocation & Duplicate Bed Prevention
    // Bed BED-GA-03 is Available
    BE_TEST(service->allocateBed("BED-GA-03", "PAT-7777", err) == true, "Allocated available bed BED-GA-03 to patient PAT-7777");

    // Try allocating second bed to PAT-7777
    bool doubleBed = service->allocateBed("BED-GA-04", "PAT-7777", err);
    BE_TEST(doubleBed == false, "Prevented assigning multiple active beds to the same patient");

    // Try allocating already occupied bed BED-GA-03
    bool occupiedBed = service->allocateBed("BED-GA-03", "PAT-1004", err);
    BE_TEST(occupiedBed == false, "Prevented allocating already occupied bed");

    // 6. Undo Administrative Bed Allocation
    std::string feedback;
    BE_TEST(service->undoLastAdministrativeAction(feedback, err) == true, "Undid bed allocation using custom Undo Stack");

    auto bedsAfterUndo = service->getAllBeds();
    for (const auto& b : bedsAfterUndo) {
        if (b.id == "BED-GA-03") {
            BE_TEST(b.status == "Available", "Bed status reverted back to Available after Undo");
            break;
        }
    }

    // 7. Emergency Dispatch and Complete
    EmergencyCase dispatched;
    BE_TEST(service->dispatchNextEmergency(dispatched, err) == true, "Successfully dispatched highest-priority emergency case");
    BE_TEST(dispatched.status == "Dispatched", "Dispatched case has status 'Dispatched'");

    BE_TEST(service->completeEmergency(dispatched.id, err) == true, "Completed emergency case");

    // 8. Delete Patient Relationship Check
    // Patient PAT-1003 is currently occupying Bed BED-GA-01. Deleting them must be blocked!
    bool delBlocked = service->deletePatient("PAT-1003", err);
    BE_TEST(delBlocked == false && !err.empty(), "Prevented deleting patient who currently occupies an active bed");

    // Clean up test db
    db->close();
    std::remove(testDbFile.c_str());

    std::cout << "\n=================================================" << std::endl;
    std::cout << "Backend Tests Summary: " << beTestsPassed << " / " << beTestsTotal << " passed." << std::endl;
    if (beTestsPassed == beTestsTotal) {
        std::cout << "ALL BACKEND TESTS PASSED SUCCESSFULLY! (100% PASS)" << std::endl;
        std::cout << "=================================================" << std::endl;
        return 0;
    } else {
        std::cerr << "SOME BACKEND TESTS FAILED!" << std::endl;
        return 1;
    }
}
