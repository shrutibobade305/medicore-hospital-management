#include "../../include/dsa/emergency_heap.hpp"
#include <algorithm>

namespace medicore {

EmergencyPriorityQueue::EmergencyPriorityQueue() = default;

bool EmergencyPriorityQueue::hasHigherPriority(const EmergencyCase& a, const EmergencyCase& b) {
    // 1. Primary: Higher severity rank
    if (a.severityRank != b.severityRank) {
        return a.severityRank > b.severityRank;
    }
    // 2. Secondary: Earlier arrival epoch (FIFO tie-breaker)
    if (a.arrivalEpoch != b.arrivalEpoch) {
        return a.arrivalEpoch < b.arrivalEpoch;
    }
    // 3. Tertiary: Monotonic sequence number
    return a.sequenceNum < b.sequenceNum;
}

void EmergencyPriorityQueue::insert(const EmergencyCase& item) {
    heap_.push_back(item);
    if (heap_.size() > 1) {
        heapifyUp(heap_.size() - 1);
    }
}

EmergencyCase EmergencyPriorityQueue::extractMax() {
    if (isEmpty()) {
        throw std::runtime_error("Cannot extract from an empty emergency priority queue");
    }

    EmergencyCase maxItem = heap_[0];
    if (heap_.size() == 1) {
        heap_.pop_back();
        return maxItem;
    }

    // Move last element to root and sift down
    heap_[0] = heap_.back();
    heap_.pop_back();
    heapifyDown(0);

    return maxItem;
}

const EmergencyCase& EmergencyPriorityQueue::peek() const {
    if (isEmpty()) {
        throw std::runtime_error("Cannot peek into an empty emergency priority queue");
    }
    return heap_[0];
}

void EmergencyPriorityQueue::heapifyUp(size_t index) {
    while (index > 0) {
        size_t parent = parentIndex(index);
        if (hasHigherPriority(heap_[index], heap_[parent])) {
            std::swap(heap_[index], heap_[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void EmergencyPriorityQueue::heapifyDown(size_t index) {
    size_t size = heap_.size();
    while (true) {
        size_t left = leftChildIndex(index);
        size_t right = rightChildIndex(index);
        size_t highest = index;

        if (left < size && hasHigherPriority(heap_[left], heap_[highest])) {
            highest = left;
        }
        if (right < size && hasHigherPriority(heap_[right], heap_[highest])) {
            highest = right;
        }

        if (highest != index) {
            std::swap(heap_[index], heap_[highest]);
            index = highest;
        } else {
            break;
        }
    }
}

bool EmergencyPriorityQueue::removeById(const std::string& caseId) {
    if (caseId.empty() || isEmpty()) return false;

    auto it = std::find_if(heap_.begin(), heap_.end(), [&](const EmergencyCase& c) {
        return c.id == caseId;
    });

    if (it == heap_.end()) return false;

    size_t index = std::distance(heap_.begin(), it);
    if (index == heap_.size() - 1) {
        heap_.pop_back();
        return true;
    }

    heap_[index] = heap_.back();
    heap_.pop_back();

    // Re-heapify in both directions if needed
    if (index > 0 && hasHigherPriority(heap_[index], heap_[parentIndex(index)])) {
        heapifyUp(index);
    } else {
        heapifyDown(index);
    }
    return true;
}

std::vector<EmergencyCase> EmergencyPriorityQueue::getSortedOrder() const {
    // Clone heap and extract one by one to get strict priority sorted list
    EmergencyPriorityQueue clone = *this;
    std::vector<EmergencyCase> sorted;
    sorted.reserve(clone.size());

    while (!clone.isEmpty()) {
        sorted.push_back(clone.extractMax());
    }
    return sorted;
}

nlohmann::json EmergencyPriorityQueue::getDebugStructure() const {
    nlohmann::json j;
    j["size"] = heap_.size();
    j["isEmpty"] = isEmpty();

    nlohmann::json nodes = nlohmann::json::array();
    for (size_t i = 0; i < heap_.size(); ++i) {
        const auto& c = heap_[i];
        nlohmann::json node;
        node["index"] = i;
        node["id"] = c.id;
        node["patientName"] = c.patientName;
        node["condition"] = c.condition;
        node["severity"] = c.severity;
        node["severityRank"] = c.severityRank;
        node["arrivalTime"] = c.arrivalTime;
        node["sequenceNum"] = c.sequenceNum;

        if (i > 0) {
            node["parentIndex"] = parentIndex(i);
        } else {
            node["parentIndex"] = nullptr;
        }

        size_t left = leftChildIndex(i);
        size_t right = rightChildIndex(i);
        node["leftChildIndex"] = (left < heap_.size()) ? nlohmann::json(left) : nlohmann::json(nullptr);
        node["rightChildIndex"] = (right < heap_.size()) ? nlohmann::json(right) : nlohmann::json(nullptr);

        nodes.push_back(node);
    }

    j["nodes"] = nodes;
    j["sortedOrder"] = getSortedOrder();
    return j;
}

} // namespace medicore
