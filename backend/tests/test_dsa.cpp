#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "../include/dsa/patient_hash_table.hpp"
#include "../include/dsa/visit_linked_list.hpp"
#include "../include/dsa/appointment_queue.hpp"
#include "../include/dsa/emergency_heap.hpp"
#include "../include/dsa/doctor_bst.hpp"
#include "../include/dsa/undo_stack.hpp"
#include "../include/dsa/hospital_graph.hpp"
#include "../include/dsa/dsa_algorithms.hpp"

using namespace medicore;

int testsPassed = 0;
int testsTotal = 0;

#define TEST_ASSERT(cond, msg) \
    do { \
        testsTotal++; \
        if (cond) { \
            testsPassed++; \
            std::cout << "  [PASS] " << msg << std::endl; \
        } else { \
            std::cerr << "  [FAIL] " << msg << " (" << #cond << ")" << std::endl; \
        } \
    } while (0)

void testHashTable() {
    std::cout << "\n=== 1. Testing Custom Patient Hash Table ===" << std::endl;
    PatientHashTable ht(4); // Start with small capacity to trigger rehashing

    TEST_ASSERT(ht.size() == 0, "Initial hash table is empty");
    TEST_ASSERT(ht.search("PAT-999") == nullptr, "Search non-existent key returns nullptr");

    Patient p1{"PAT-1001", "Arun Kumar", 45, "Male", "+91 9845011223", "Bangalore", "O+", "2026-09-10", "Hypertension"};
    Patient p2{"PAT-1002", "Priya Sharma", 32, "Female", "+91 9741288344", "Bangalore", "B+", "2026-09-15", "Routine"};
    Patient p3{"PAT-1003", "Rohan Mehta", 58, "Male", "+91 9448177655", "Bangalore", "A+", "2026-09-18", "Diabetes"};
    Patient p4{"PAT-1004", "Sunita Nair", 27, "Female", "+91 9886022334", "Bangalore", "AB+", "2026-09-22", "Migraine"};
    Patient p5{"PAT-1005", "David Fernandes", 64, "Male", "+91 9844099887", "Bangalore", "O-", "2026-09-25", "Cardiac"};

    ht.insert(p1);
    ht.insert(p2);
    ht.insert(p3);
    ht.insert(p4);
    ht.insert(p5);

    TEST_ASSERT(ht.size() == 5, "Size is 5 after inserting 5 patients");
    TEST_ASSERT(ht.capacity() > 4, "Capacity dynamically resized/rehashed");

    Patient* found = ht.search("PAT-1003");
    TEST_ASSERT(found != nullptr && found->name == "Rohan Mehta", "Lookup by ID found correct patient");

    // Update patient
    p3.name = "Rohan Mehta Jr.";
    ht.insert(p3);
    found = ht.search("PAT-1003");
    TEST_ASSERT(found != nullptr && found->name == "Rohan Mehta Jr." && ht.size() == 5, "Insert with existing key updates record in-place");

    // Removal
    bool removed = ht.remove("PAT-1002");
    TEST_ASSERT(removed == true && ht.size() == 4, "Removal of existing key succeeds");
    TEST_ASSERT(ht.search("PAT-1002") == nullptr, "Deleted key is no longer searchable");
    TEST_ASSERT(ht.remove("PAT-9999") == false, "Removal of non-existent key returns false");

    // Clear
    ht.clear();
    TEST_ASSERT(ht.size() == 0, "Clear empties the table");
}

void testVisitLinkedList() {
    std::cout << "\n=== 2. Testing Custom Visit Doubly Linked List ===" << std::endl;
    VisitLinkedList list;

    TEST_ASSERT(list.isEmpty() == true, "New list is empty");

    VisitRecord v1{"VST-1", "PAT-1001", "DOC-101", "Dr. Rajesh", "2026-09-12", "Hypertension", "Meds A", "Note 1", "2026-09-12 10:00"};
    VisitRecord v2{"VST-2", "PAT-1001", "DOC-101", "Dr. Rajesh", "2026-09-20", "Diabetes", "Meds B", "Note 2", "2026-09-20 11:00"};
    VisitRecord v3{"VST-3", "PAT-1002", "DOC-102", "Dr. Shalini", "2026-09-25", "Migraine", "Meds C", "Note 3", "2026-09-25 12:00"};

    list.insertTail(v1);
    list.insertTail(v2);
    list.insertHead(v3); // v3 is now at front

    TEST_ASSERT(list.size() == 3, "List size is 3");

    auto forward = list.getForward();
    TEST_ASSERT(forward.size() == 3 && forward[0].id == "VST-3" && forward[2].id == "VST-2", "Forward traversal order verified");

    auto backward = list.getBackward();
    TEST_ASSERT(backward.size() == 3 && backward[0].id == "VST-2" && backward[2].id == "VST-3", "Backward traversal order verified");

    auto p1Visits = list.getByPatientId("PAT-1001");
    TEST_ASSERT(p1Visits.size() == 2, "Filtering by patient ID returns correct count");

    // Remove middle node
    bool del = list.removeById("VST-1");
    TEST_ASSERT(del == true && list.size() == 2, "Node deleted by ID");
    TEST_ASSERT(list.getByPatientId("PAT-1001").size() == 1, "Remaining count verified after deletion");
}

void testAppointmentQueue() {
    std::cout << "\n=== 3. Testing Custom Appointment FIFO Queue ===" << std::endl;
    AppointmentQueue q;

    TEST_ASSERT(q.isEmpty() == true, "New queue is empty");

    Appointment a1{"APT-1", "PAT-1", "P1", "DOC-1", "D1", "2026-10-09", "10:00", "Checkup", "Waiting", ""};
    Appointment a2{"APT-2", "PAT-2", "P2", "DOC-2", "D2", "2026-10-09", "10:15", "Consult", "Waiting", ""};
    Appointment a3{"APT-3", "PAT-3", "P3", "DOC-1", "D1", "2026-10-09", "10:30", "Followup", "Waiting", ""};

    q.enqueue(a1);
    q.enqueue(a2);
    q.enqueue(a3);

    TEST_ASSERT(q.size() == 3, "Queue size is 3");
    TEST_ASSERT(q.peek().id == "APT-1", "Peek returns front element (APT-1)");

    Appointment deq1 = q.dequeue();
    TEST_ASSERT(deq1.id == "APT-1" && q.size() == 2, "First dequeue returns APT-1 (FIFO order)");

    Appointment deq2 = q.dequeue();
    TEST_ASSERT(deq2.id == "APT-2" && q.size() == 1, "Second dequeue returns APT-2 (FIFO order)");

    Appointment deq3 = q.dequeue();
    TEST_ASSERT(deq3.id == "APT-3" && q.isEmpty() == true, "Third dequeue empties the queue");
}

void testEmergencyHeap() {
    std::cout << "\n=== 4. Testing Custom Emergency Priority Queue (Binary Heap) ===" << std::endl;
    EmergencyPriorityQueue pq;

    TEST_ASSERT(pq.isEmpty() == true, "New priority queue is empty");

    // Case A: High, arrival epoch 1000, seq 1
    EmergencyCase cA{"EMG-A", "P1", "Patient A", 40, "Male", "Stroke", "High", 3, "10:00", 1000, 1, "Pending", ""};
    // Case B: Critical, arrival epoch 1050, seq 2 -> Higher severity than A
    EmergencyCase cB{"EMG-B", "P2", "Patient B", 55, "Male", "Cardiac Arrest", "Critical", 4, "10:05", 1050, 2, "Pending", ""};
    // Case C: Critical, arrival epoch 1020, seq 3 -> Equal severity to B, but arrived EARLIER! (FIFO tie-breaker)
    EmergencyCase cC{"EMG-C", "P3", "Patient C", 30, "Female", "Trauma STEMI", "Critical", 4, "10:02", 1020, 3, "Pending", ""};
    // Case D: Low, arrival epoch 900, seq 4
    EmergencyCase cD{"EMG-D", "P4", "Patient D", 20, "Female", "Minor Burn", "Low", 1, "09:50", 900, 4, "Pending", ""};

    pq.insert(cA);
    pq.insert(cB);
    pq.insert(cC);
    pq.insert(cD);

    TEST_ASSERT(pq.size() == 4, "Heap contains 4 emergency cases");

    // Highest priority case must be cC (Critical AND arrived at 1020, earlier than cB at 1050)
    EmergencyCase top1 = pq.extractMax();
    TEST_ASSERT(top1.id == "EMG-C", "First dispatched is EMG-C (Critical + earlier arrival time tie-break)");

    // Next must be cB (Critical at 1050)
    EmergencyCase top2 = pq.extractMax();
    TEST_ASSERT(top2.id == "EMG-B", "Second dispatched is EMG-B (Critical)");

    // Next must be cA (High)
    EmergencyCase top3 = pq.extractMax();
    TEST_ASSERT(top3.id == "EMG-A", "Third dispatched is EMG-A (High severity)");

    // Last must be cD (Low)
    EmergencyCase top4 = pq.extractMax();
    TEST_ASSERT(top4.id == "EMG-D" && pq.isEmpty(), "Last dispatched is EMG-D (Low severity)");
}

void testDoctorBST() {
    std::cout << "\n=== 5. Testing Custom Doctor Binary Search Tree ===" << std::endl;
    DoctorBST bst;

    TEST_ASSERT(bst.isEmpty() == true, "New BST is empty");

    Doctor d3{"DOC-103", "Dr. Amitava", "Orthopedics", "Trauma", "+91 1", "", "Available", "OPD-103"};
    Doctor d1{"DOC-101", "Dr. Rajesh", "Cardiology", "Cardio", "+91 2", "", "Available", "OPD-101"};
    Doctor d5{"DOC-105", "Dr. Farhan", "Emergency", "Trauma", "+91 3", "", "Available", "OPD-105"};
    Doctor d2{"DOC-102", "Dr. Shalini", "Neurology", "Neuro", "+91 4", "", "Available", "OPD-102"};
    Doctor d4{"DOC-104", "Dr. Preethi", "Gyn", "Fetal", "+91 5", "", "Available", "OPD-104"};

    // Insert out of order
    bst.insert(d3);
    bst.insert(d1);
    bst.insert(d5);
    bst.insert(d2);
    bst.insert(d4);

    TEST_ASSERT(bst.size() == 5, "BST contains 5 doctors");
    TEST_ASSERT(bst.height() >= 3, "BST height calculated correctly");

    Doctor* found = bst.search("DOC-102");
    TEST_ASSERT(found != nullptr && found->name == "Dr. Shalini", "Search found DOC-102");

    // Inorder traversal must return elements in strictly ascending sorted order!
    auto inorderDocs = bst.inorder();
    TEST_ASSERT(inorderDocs.size() == 5, "Inorder traversal returned all 5 doctors");
    TEST_ASSERT(inorderDocs[0].id == "DOC-101" &&
                inorderDocs[1].id == "DOC-102" &&
                inorderDocs[2].id == "DOC-103" &&
                inorderDocs[3].id == "DOC-104" &&
                inorderDocs[4].id == "DOC-105", "Inorder traversal is strictly sorted by Doctor ID");

    // Deletion: node with 2 children (DOC-103 is root)
    bool deleted = bst.remove("DOC-103");
    TEST_ASSERT(deleted == true && bst.size() == 4, "Root node with 2 children deleted successfully");
    TEST_ASSERT(bst.search("DOC-103") == nullptr, "Deleted doctor is no longer found");

    auto inorderAfter = bst.inorder();
    TEST_ASSERT(inorderAfter[0].id == "DOC-101" && inorderAfter[1].id == "DOC-102" && inorderAfter[2].id == "DOC-104",
                "BST preserves sorted order after deletion");
}

void testUndoStack() {
    std::cout << "\n=== 6. Testing Custom Administrative Action Undo Stack ===" << std::endl;
    UndoStack stack(10);

    TEST_ASSERT(stack.isEmpty() == true, "New stack is empty");

    ActionRecord act1{"ACT-1", "BED_ALLOCATE", "BED-01", "{\"bedId\":\"BED-01\"}", "10:00"};
    ActionRecord act2{"ACT-2", "BED_RELEASE", "BED-02", "{\"bedId\":\"BED-02\"}", "10:10"};

    stack.push(act1);
    stack.push(act2);

    TEST_ASSERT(stack.size() == 2, "Stack size is 2");
    TEST_ASSERT(stack.top().id == "ACT-2", "Top is most recently pushed action (LIFO)");

    ActionRecord popped = stack.pop();
    TEST_ASSERT(popped.id == "ACT-2" && stack.size() == 1, "Pop retrieved ACT-2");

    ActionRecord popped2 = stack.pop();
    TEST_ASSERT(popped2.id == "ACT-1" && stack.isEmpty(), "Second pop retrieved ACT-1 and emptied stack");
}

void testHospitalGraph() {
    std::cout << "\n=== 7. Testing Custom Hospital Navigation Graph & Dijkstra ===" << std::endl;
    HospitalGraph graph;

    // Check default vertices
    auto vertices = graph.getAllVertices();
    TEST_ASSERT(vertices.size() == 10, "Default graph has 10 hospital department vertices");

    // Shortest path from Entrance to ICU
    auto pathRes = graph.findShortestPath("ENTRANCE", "ICU");
    TEST_ASSERT(pathRes.reachable == true, "ICU is reachable from Main Entrance");
    TEST_ASSERT(pathRes.totalCost > 0.0, "Path distance cost is greater than zero");
    TEST_ASSERT(pathRes.path.front() == "ENTRANCE" && pathRes.path.back() == "ICU", "Path start and end endpoints match");

    // Test BFS traversal
    auto bfsRes = graph.bfs("ENTRANCE");
    TEST_ASSERT(bfsRes.visitOrder.size() == 10 && bfsRes.visitOrder[0] == "ENTRANCE", "BFS visits all connected departments");

    // Test DFS traversal
    auto dfsRes = graph.dfs("ENTRANCE");
    TEST_ASSERT(dfsRes.visitOrder.size() == 10 && dfsRes.visitOrder[0] == "ENTRANCE", "DFS visits all connected departments");

    // Test Corridor Closure Simulation
    // Close edge between ENTRANCE and RECEPTION
    graph.setEdgeClosure("ENTRANCE", "RECEPTION", true);
    auto closedRoute = graph.findShortestPath("ENTRANCE", "RECEPTION");
    TEST_ASSERT(closedRoute.reachable == false, "Route handles disconnected corridor after simulated closure");

    // Reopen corridor
    graph.setEdgeClosure("ENTRANCE", "RECEPTION", false);
    auto reopenedRoute = graph.findShortestPath("ENTRANCE", "RECEPTION");
    TEST_ASSERT(reopenedRoute.reachable == true, "Route restored after corridor reopened");
}

void testSortingAndSearching() {
    std::cout << "\n=== 8. Testing Merge Sort and Binary Search Algorithms ===" << std::endl;

    // Merge sort integers
    std::vector<int> arr = {64, 34, 25, 12, 22, 11, 90};
    auto steps = DsaAlgorithms::traceMergeSort(arr);
    TEST_ASSERT(!steps.empty(), "Merge sort generates step traces");
    TEST_ASSERT(arr == std::vector<int>({11, 12, 22, 25, 34, 64, 90}), "Array is correctly sorted by Merge Sort");

    // Binary search
    auto bsFound = DsaAlgorithms::binarySearch(arr, 25);
    TEST_ASSERT(bsFound.found == true && bsFound.index == 3, "Binary Search found 25 at index 3");

    auto bsNotFound = DsaAlgorithms::binarySearch(arr, 99);
    TEST_ASSERT(bsNotFound.found == false && bsNotFound.index == -1, "Binary Search correctly reported element 99 not found");
}

int main() {
    std::cout << "=================================================" << std::endl;
    std::cout << "  MediCore Custom C++ DSA Automated Test Suite   " << std::endl;
    std::cout << "=================================================" << std::endl;

    testHashTable();
    testVisitLinkedList();
    testAppointmentQueue();
    testEmergencyHeap();
    testDoctorBST();
    testUndoStack();
    testHospitalGraph();
    testSortingAndSearching();

    std::cout << "\n=================================================" << std::endl;
    std::cout << "Test Summary: " << testsPassed << " / " << testsTotal << " passed." << std::endl;
    if (testsPassed == testsTotal) {
        std::cout << "ALL DSA TESTS PASSED SUCCESSFULLY! (100% PASS)" << std::endl;
        std::cout << "=================================================" << std::endl;
        return 0;
    } else {
        std::cerr << "SOME TESTS FAILED!" << std::endl;
        return 1;
    }
}
