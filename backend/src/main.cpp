#define ASIO_STANDALONE
#define CROW_MAIN

#include <iostream>
#include <memory>
#include <string>

#include "third_party/crow_all.h"
#include "third_party/json.hpp"
#include "repositories/database.hpp"
#include "services/hospital_service.hpp"

using json = nlohmann::json;

// Helper to construct standardized JSON HTTP responses with CORS enabled
inline crow::response json_response(const json& data, int status = 200) {
    crow::response res(status, data.dump());
    res.set_header("Content-Type", "application/json");
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    return res;
}

inline crow::response error_response(const std::string& message, int status = 400) {
    json err;
    err["success"] = false;
    err["error"] = message;
    return json_response(err, status);
}

inline crow::response success_response(const json& data = json::object(), const std::string& message = "") {
    json res;
    res["success"] = true;
    if (!message.empty()) res["message"] = message;
    res["data"] = data;
    return json_response(res, 200);
}

// CORS struct middleware
struct CorsMiddleware {
    struct context {};
    void before_handle(crow::request&, crow::response&, context&) {}
    void after_handle(crow::request&, crow::response& res, context&) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
    }
};

int main() {
    std::cout << "========================================================" << std::endl;
    std::cout << "  MediCore: Smart Hospital Management System (C++ DSA)" << std::endl;
    std::cout << "========================================================" << std::endl;

    std::string dbFile = "data/hospital.db";
    auto db = std::make_shared<medicore::Database>(dbFile);
    auto service = std::make_shared<medicore::HospitalService>(db);

    if (!service->init()) {
        std::cerr << "CRITICAL: Hospital service initialization failed!" << std::endl;
        return 1;
    }

    crow::App<CorsMiddleware> app;

    // --- OPTIONS pre-flight handler ---
    CROW_ROUTE(app, "/api/<path>").methods("OPTIONS"_method)([](const crow::request&, std::string) {
        crow::response res(200);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return res;
    });

    CROW_ROUTE(app, "/api/<path>/<path>").methods("OPTIONS"_method)([](const crow::request&, std::string, std::string) {
        crow::response res(200);
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return res;
    });

    // --- Root Welcome Route ---
    CROW_ROUTE(app, "/").methods("GET"_method)([]() {
        json j;
        j["name"] = "MediCore Smart Hospital Management System API";
        j["status"] = "online";
        j["service"] = "MediCore C++ Backend";
        j["version"] = "2.0.0";
        j["message"] = "Welcome to MediCore Smart Hospital Management System API server.";
        j["endpoints"] = json::array({
            "/api/health",
            "/api/dashboard",
            "/api/patients",
            "/api/doctors",
            "/api/appointments",
            "/api/emergencies",
            "/api/beds",
            "/api/wards",
            "/api/navigation/map",
            "/api/activity"
        });
        return json_response(j);
    });

    // --- Health Check ---
    CROW_ROUTE(app, "/api/health").methods("GET"_method)([]() {
        json j;
        j["name"] = "MediCore Smart Hospital Management System API";
        j["status"] = "online";
        j["service"] = "MediCore C++ Backend";
        j["version"] = "2.0.0";
        j["timestamp"] = std::to_string(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
        return json_response(j);
    });

    // --- Dashboard Overview ---
    CROW_ROUTE(app, "/api/dashboard").methods("GET"_method)([service]() {
        return success_response(service->getDashboardSummary());
    });

    // --- Patients Management ---
    CROW_ROUTE(app, "/api/patients").methods("GET"_method)([service](const crow::request& req) {
        std::string sortBy = req.url_params.get("sortBy") ? req.url_params.get("sortBy") : "";
        std::string query = req.url_params.get("q") ? req.url_params.get("q") : "";
        auto patients = service->getAllPatients(sortBy, query);
        return success_response(patients);
    });

    CROW_ROUTE(app, "/api/patients").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            medicore::Patient p = body.get<medicore::Patient>();
            std::string errMsg;
            if (!service->registerPatient(p, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json(p), "Patient registered successfully.");
        } catch (const std::exception& e) {
            return error_response(std::string("Malformed JSON: ") + e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/patients/<string>").methods("GET"_method)([service](std::string id) {
        auto p = service->getPatientById(id);
        if (!p) {
            return error_response("Patient " + id + " not found in Custom Hash Table.", 404);
        }
        return success_response(*p);
    });

    CROW_ROUTE(app, "/api/patients/<string>").methods("PUT"_method)([service](const crow::request& req, std::string id) {
        try {
            auto body = json::parse(req.body);
            medicore::Patient p = body.get<medicore::Patient>();
            p.id = id;
            std::string errMsg;
            if (!service->updatePatient(p, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json(p), "Patient updated successfully.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/patients/<string>").methods("DELETE"_method)([service](std::string id) {
        std::string errMsg;
        if (!service->deletePatient(id, errMsg)) {
            return error_response(errMsg, 400);
        }
        return success_response(json::object(), "Patient deleted successfully.");
    });

    // Patient Clinical Visits
    CROW_ROUTE(app, "/api/patients/<string>/visits").methods("GET"_method)([service](std::string id) {
        auto visits = service->getPatientVisits(id);
        return success_response(visits);
    });

    CROW_ROUTE(app, "/api/patients/<string>/visits").methods("POST"_method)([service](const crow::request& req, std::string id) {
        try {
            auto body = json::parse(req.body);
            medicore::VisitRecord v = body.get<medicore::VisitRecord>();
            v.patientId = id;
            std::string errMsg;
            if (!service->addVisitRecord(v, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json(v), "Visit record added to Doubly Linked List & database.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    // --- Doctors Management ---
    CROW_ROUTE(app, "/api/doctors").methods("GET"_method)([service](const crow::request& req) {
        std::string dept = req.url_params.get("dept") ? req.url_params.get("dept") : "";
        std::string search = req.url_params.get("q") ? req.url_params.get("q") : "";
        auto doctors = service->getAllDoctors(dept, search);
        return success_response(doctors);
    });

    CROW_ROUTE(app, "/api/doctors/sorted").methods("GET"_method)([service]() {
        // BST Inorder Traversal
        auto doctors = service->getDoctorsSorted();
        return success_response(doctors);
    });

    CROW_ROUTE(app, "/api/doctors").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            medicore::Doctor d = body.get<medicore::Doctor>();
            std::string errMsg;
            if (!service->addDoctor(d, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json(d), "Doctor added to BST & database.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/doctors/<string>").methods("GET"_method)([service](std::string id) {
        auto d = service->getDoctorById(id);
        if (!d) return error_response("Doctor " + id + " not found in Custom BST.", 404);
        return success_response(*d);
    });

    CROW_ROUTE(app, "/api/doctors/<string>").methods("PUT"_method)([service](const crow::request& req, std::string id) {
        try {
            auto body = json::parse(req.body);
            medicore::Doctor d = body.get<medicore::Doctor>();
            d.id = id;
            std::string errMsg;
            if (!service->updateDoctor(d, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json(d), "Doctor updated successfully.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/doctors/<string>").methods("DELETE"_method)([service](std::string id) {
        std::string errMsg;
        if (!service->deleteDoctor(id, errMsg)) {
            return error_response(errMsg, 400);
        }
        return success_response(json::object(), "Doctor removed successfully.");
    });

    // --- Appointments Management ---
    CROW_ROUTE(app, "/api/appointments").methods("GET"_method)([service](const crow::request& req) {
        std::string date = req.url_params.get("date") ? req.url_params.get("date") : "";
        std::string doctorId = req.url_params.get("doctorId") ? req.url_params.get("doctorId") : "";
        std::string status = req.url_params.get("status") ? req.url_params.get("status") : "";
        auto appts = service->getAllAppointments(date, doctorId, status);
        return success_response(appts);
    });

    CROW_ROUTE(app, "/api/appointments").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            medicore::Appointment a = body.get<medicore::Appointment>();
            std::string errMsg;
            if (!service->scheduleAppointment(a, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json(a), "Appointment scheduled successfully.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/appointments/<string>").methods("PUT"_method)([service](const crow::request& req, std::string id) {
        try {
            auto body = json::parse(req.body);
            medicore::Appointment a = body.get<medicore::Appointment>();
            a.id = id;
            std::string errMsg;
            if (!service->updateAppointment(a, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json(a), "Appointment updated.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/appointments/<string>").methods("DELETE"_method)([service](std::string id) {
        std::string errMsg;
        if (!service->cancelAppointment(id, errMsg)) {
            return error_response(errMsg, 400);
        }
        return success_response(json::object(), "Appointment cancelled.");
    });

    CROW_ROUTE(app, "/api/appointments/waiting").methods("GET"_method)([service]() {
        auto waiting = service->getWaitingQueue();
        return success_response(waiting);
    });

    // --- Emergency & Triage Module ---
    CROW_ROUTE(app, "/api/emergencies").methods("GET"_method)([service](const crow::request& req) {
        bool all = req.url_params.get("all") != nullptr;
        if (all) {
            return success_response(service->getAllEmergencies());
        }
        return success_response(service->getPendingEmergencies());
    });

    CROW_ROUTE(app, "/api/emergencies").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            medicore::EmergencyCase e = body.get<medicore::EmergencyCase>();
            std::string errMsg;
            if (!service->registerEmergency(e, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json(e), "Emergency case registered into Min/Max-Heap priority queue.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/emergencies/dispatch").methods("POST"_method)([service]() {
        medicore::EmergencyCase dispatched;
        std::string errMsg;
        if (!service->dispatchNextEmergency(dispatched, errMsg)) {
            return error_response(errMsg, 400);
        }
        return success_response(json(dispatched), "Dispatched highest-priority emergency case: " + dispatched.patientName);
    });

    CROW_ROUTE(app, "/api/emergencies/<string>/complete").methods("POST"_method)([service](std::string id) {
        std::string errMsg;
        if (!service->completeEmergency(id, errMsg)) {
            return error_response(errMsg, 400);
        }
        return success_response(json::object(), "Emergency case " + id + " marked completed.");
    });

    // --- Bed & Ward Management ---
    CROW_ROUTE(app, "/api/beds").methods("GET"_method)([service](const crow::request& req) {
        std::string wardId = req.url_params.get("wardId") ? req.url_params.get("wardId") : "";
        std::string status = req.url_params.get("status") ? req.url_params.get("status") : "";
        auto beds = service->getAllBeds(wardId, status);
        return success_response(beds);
    });

    CROW_ROUTE(app, "/api/wards").methods("GET"_method)([service]() {
        return success_response(service->getAllWards());
    });

    CROW_ROUTE(app, "/api/beds/<string>/allocate").methods("POST"_method)([service](const crow::request& req, std::string bedId) {
        try {
            auto body = json::parse(req.body);
            std::string patientId = body.value("patientId", "");
            if (patientId.empty()) {
                return error_response("patientId is required.", 400);
            }
            std::string errMsg;
            if (!service->allocateBed(bedId, patientId, errMsg)) {
                return error_response(errMsg, 400);
            }
            return success_response(json::object(), "Bed allocated. Action pushed to Undo Stack.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/beds/<string>/release").methods("POST"_method)([service](std::string bedId) {
        std::string errMsg;
        if (!service->releaseBed(bedId, errMsg)) {
            return error_response(errMsg, 400);
        }
        return success_response(json::object(), "Bed released. Action pushed to Undo Stack.");
    });

    CROW_ROUTE(app, "/api/beds/undo").methods("POST"_method)([service]() {
        std::string feedbackMsg;
        std::string errMsg;
        if (!service->undoLastAdministrativeAction(feedbackMsg, errMsg)) {
            return error_response(errMsg, 400);
        }
        return success_response(json::object(), feedbackMsg);
    });

    // --- Hospital Navigation Graph & Pathfinding ---
    CROW_ROUTE(app, "/api/navigation/map").methods("GET"_method)([service]() {
        return success_response(service->getHospitalMap());
    });

    CROW_ROUTE(app, "/api/navigation/route").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            std::string from = body.value("from", "");
            std::string to = body.value("to", "");
            if (from.empty() || to.empty()) {
                return error_response("Both 'from' and 'to' department IDs required.", 400);
            }
            auto result = service->findRoute(from, to);
            return success_response(json(result));
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/navigation/traverse").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            std::string startId = body.value("startId", "ENTRANCE");
            std::string type = body.value("type", "BFS");
            auto result = service->traverseGraph(startId, type);
            return success_response(json(result));
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/navigation/toggle-corridor").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            std::string from = body.value("from", "");
            std::string to = body.value("to", "");
            bool closed = body.value("closed", true);
            bool ok = service->toggleCorridorClosure(from, to, closed);
            if (!ok) return error_response("Corridor between departments not found.", 404);
            return success_response(service->getHospitalMap(), closed ? "Corridor closed for maintenance." : "Corridor reopened.");
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    // --- Audit & Activity Logs ---
    CROW_ROUTE(app, "/api/activity").methods("GET"_method)([service]() {
        return success_response(service->getActivityLogs());
    });

    // --- System Control ---
    CROW_ROUTE(app, "/api/system/reset").methods("POST"_method)([service]() {
        if (!service->resetAllData()) {
            return error_response("Failed to reset system data.", 500);
        }
        return success_response(json::object(), "Database reset and demo data successfully reseeded.");
    });

    // --- DSA Visualizer Educational API ---
    CROW_ROUTE(app, "/api/dsa/hash-table").methods("GET"_method)([service]() {
        return success_response(service->getHashTableVisualizerData());
    });

    CROW_ROUTE(app, "/api/dsa/bst").methods("GET"_method)([service]() {
        return success_response(service->getBstVisualizerData());
    });

    CROW_ROUTE(app, "/api/dsa/heap").methods("GET"_method)([service]() {
        return success_response(service->getHeapVisualizerData());
    });

    CROW_ROUTE(app, "/api/dsa/queue").methods("GET"_method)([service]() {
        return success_response(service->getQueueVisualizerData());
    });

    CROW_ROUTE(app, "/api/dsa/stack").methods("GET"_method)([service]() {
        return success_response(service->getStackVisualizerData());
    });

    CROW_ROUTE(app, "/api/dsa/linked-list").methods("GET"_method)([service]() {
        return success_response(service->getLinkedListVisualizerData());
    });

    CROW_ROUTE(app, "/api/dsa/graph").methods("GET"_method)([service]() {
        return success_response(service->getGraphVisualizerData());
    });

    CROW_ROUTE(app, "/api/dsa/sort-demo").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            std::vector<int> arr = body.value("array", std::vector<int>{48, 12, 75, 33, 91, 24, 60, 18});
            return success_response(service->runMergeSortDemo(arr));
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    CROW_ROUTE(app, "/api/dsa/search-demo").methods("POST"_method)([service](const crow::request& req) {
        try {
            auto body = json::parse(req.body);
            std::vector<int> arr = body.value("array", std::vector<int>{12, 18, 24, 33, 48, 60, 75, 91});
            int target = body.value("target", 48);
            return success_response(service->runBinarySearchDemo(arr, target));
        } catch (const std::exception& e) {
            return error_response(e.what(), 400);
        }
    });

    std::cout << "[MediCore C++] Server listening on http://localhost:8080" << std::endl;
    app.port(8080).multithreaded().run();

    return 0;
}
