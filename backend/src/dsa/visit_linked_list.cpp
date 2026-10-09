#include "../../include/dsa/visit_linked_list.hpp"

namespace medicore {

VisitLinkedList::VisitLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

VisitLinkedList::~VisitLinkedList() {
    clear();
}

void VisitLinkedList::clear() {
    auto current = head_;
    while (current) {
        auto nextNode = current->next;
        current->next = nullptr;
        current = nextNode;
    }
    head_ = nullptr;
    tail_ = nullptr;
    size_ = 0;
}

void VisitLinkedList::insertHead(const VisitRecord& record) {
    auto newNode = std::make_shared<Node>(record);
    if (!head_) {
        head_ = newNode;
        tail_ = newNode;
    } else {
        newNode->next = head_;
        head_->prev = newNode;
        head_ = newNode;
    }
    size_++;
}

void VisitLinkedList::insertTail(const VisitRecord& record) {
    auto newNode = std::make_shared<Node>(record);
    if (!tail_) {
        head_ = newNode;
        tail_ = newNode;
    } else {
        tail_->next = newNode;
        newNode->prev = tail_;
        tail_ = newNode;
    }
    size_++;
}

bool VisitLinkedList::removeById(const std::string& visitId) {
    if (visitId.empty() || !head_) return false;

    auto current = head_;
    while (current) {
        if (current->data.id == visitId) {
            auto prevNode = current->prev.lock();
            auto nextNode = current->next;

            if (prevNode) {
                prevNode->next = nextNode;
            } else {
                head_ = nextNode;
            }

            if (nextNode) {
                nextNode->prev = prevNode;
            } else {
                tail_ = prevNode;
            }

            current->next = nullptr;
            size_--;
            return true;
        }
        current = current->next;
    }
    return false;
}

std::vector<VisitRecord> VisitLinkedList::getForward() const {
    std::vector<VisitRecord> result;
    result.reserve(size_);
    auto current = head_;
    while (current) {
        result.push_back(current->data);
        current = current->next;
    }
    return result;
}

std::vector<VisitRecord> VisitLinkedList::getBackward() const {
    std::vector<VisitRecord> result;
    result.reserve(size_);
    auto current = tail_;
    while (current) {
        result.push_back(current->data);
        current = current->prev.lock();
    }
    return result;
}

std::vector<VisitRecord> VisitLinkedList::getByPatientId(const std::string& patientId) const {
    std::vector<VisitRecord> result;
    auto current = head_;
    while (current) {
        if (current->data.patientId == patientId) {
            result.push_back(current->data);
        }
        current = current->next;
    }
    return result;
}

nlohmann::json VisitLinkedList::getDebugStructure() const {
    nlohmann::json j;
    j["size"] = size_;
    j["isEmpty"] = isEmpty();

    nlohmann::json nodes = nlohmann::json::array();
    auto current = head_;
    size_t index = 0;
    while (current) {
        nlohmann::json node;
        node["index"] = index++;
        node["id"] = current->data.id;
        node["patientId"] = current->data.patientId;
        node["diagnosis"] = current->data.diagnosis;
        node["visitDate"] = current->data.visitDate;
        node["hasPrev"] = !current->prev.expired();
        node["hasNext"] = (current->next != nullptr);
        nodes.push_back(node);
        current = current->next;
    }
    j["nodes"] = nodes;
    return j;
}

} // namespace medicore
