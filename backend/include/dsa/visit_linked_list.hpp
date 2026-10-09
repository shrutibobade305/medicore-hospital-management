#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../models/models.hpp"
#include "../third_party/json.hpp"

namespace medicore {

/**
 * Custom Doubly Linked List for Patient Visit Records and Clinical History
 * Time Complexity:
 *   - Insert Head: O(1)
 *   - Insert Tail: O(1)
 *   - Delete by ID: O(n)
 *   - Traversal (Forward / Backward): O(n)
 * Space Complexity: O(n)
 */
class VisitLinkedList {
public:
    struct Node {
        VisitRecord data;
        std::shared_ptr<Node> next;
        std::weak_ptr<Node> prev; // weak_ptr avoids cyclic reference memory leaks

        explicit Node(const VisitRecord& record) : data(record), next(nullptr) {}
    };

    VisitLinkedList();
    ~VisitLinkedList();

    // Prevent copy
    VisitLinkedList(const VisitLinkedList&) = delete;
    VisitLinkedList& operator=(const VisitLinkedList&) = delete;

    void insertHead(const VisitRecord& record);
    void insertTail(const VisitRecord& record);
    bool removeById(const std::string& visitId);
    void clear();

    size_t size() const { return size_; }
    bool isEmpty() const { return size_ == 0; }

    std::vector<VisitRecord> getForward() const;
    std::vector<VisitRecord> getBackward() const;
    std::vector<VisitRecord> getByPatientId(const std::string& patientId) const;

    nlohmann::json getDebugStructure() const;

private:
    std::shared_ptr<Node> head_;
    std::shared_ptr<Node> tail_;
    size_t size_;
};

} // namespace medicore
