#pragma once

#include <vector>
#include <string>
#include <stdexcept>
#include "../models/models.hpp"
#include "../third_party/json.hpp"

namespace medicore {

/**
 * Custom Binary Max-Heap Priority Queue for Emergency Triage
 *
 * Priority Policy:
 * 1. Higher Severity Rank (Critical=4 > High=3 > Medium=2 > Low=1)
 * 2. Deterministic FIFO Tie-Breaker: Earlier Arrival Epoch (smaller epoch timestamp)
 * 3. Stable Tie-Breaker: Monotonic Sequence Number (smaller sequence number)
 *
 * Time Complexity:
 *   - Insert (Push): O(log n)
 *   - Extract Highest Priority: O(log n)
 *   - Peek Root: O(1)
 *   - Build Heap: O(n)
 * Space Complexity: O(n)
 */
class EmergencyPriorityQueue {
public:
    EmergencyPriorityQueue();
    ~EmergencyPriorityQueue() = default;

    void insert(const EmergencyCase& item);
    EmergencyCase extractMax();
    const EmergencyCase& peek() const;
    bool isEmpty() const { return heap_.empty(); }
    size_t size() const { return heap_.size(); }
    void clear() { heap_.clear(); }

    bool removeById(const std::string& caseId);
    std::vector<EmergencyCase> getSortedOrder() const;
    std::vector<EmergencyCase> getHeapArray() const { return heap_; }

    nlohmann::json getDebugStructure() const;

    // Comparator function
    static bool hasHigherPriority(const EmergencyCase& a, const EmergencyCase& b);

private:
    void heapifyUp(size_t index);
    void heapifyDown(size_t index);

    static size_t parentIndex(size_t i) { return (i - 1) / 2; }
    static size_t leftChildIndex(size_t i) { return 2 * i + 1; }
    static size_t rightChildIndex(size_t i) { return 2 * i + 2; }

    std::vector<EmergencyCase> heap_;
};

} // namespace medicore
