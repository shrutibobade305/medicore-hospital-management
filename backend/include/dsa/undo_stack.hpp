#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include "../models/models.hpp"
#include "../third_party/json.hpp"

namespace medicore {

/**
 * Custom Stack for Administrative Undo Operations (LIFO)
 *
 * Supported Undo Operations:
 * 1. BED_ALLOCATE: Reverses patient bed allocation (marks bed Available, clears patient assignment).
 * 2. BED_RELEASE: Reverses patient discharge (restores patient to the released bed).
 * 3. EMERGENCY_DISPATCH: Reverses emergency dispatch (restores case back to Pending in Priority Queue).
 *
 * Time Complexity:
 *   - Push: O(1)
 *   - Pop: O(1)
 *   - Top: O(1)
 *   - IsEmpty / Size: O(1)
 * Space Complexity: O(k) where k is max stack history size
 */
class UndoStack {
public:
    struct Node {
        ActionRecord action;
        std::shared_ptr<Node> next;

        explicit Node(const ActionRecord& act) : action(act), next(nullptr) {}
    };

    explicit UndoStack(size_t maxCapacity = 50);
    ~UndoStack() = default;

    UndoStack(const UndoStack&) = delete;
    UndoStack& operator=(const UndoStack&) = delete;

    void push(const ActionRecord& action);
    ActionRecord pop();
    const ActionRecord& top() const;
    bool isEmpty() const { return size_ == 0; }
    size_t size() const { return size_; }
    void clear();

    std::vector<ActionRecord> getAll() const;
    nlohmann::json getDebugStructure() const;

private:
    std::shared_ptr<Node> top_;
    size_t size_;
    size_t maxCapacity_;
};

} // namespace medicore
