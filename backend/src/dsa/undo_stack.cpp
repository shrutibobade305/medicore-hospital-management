#include "../../include/dsa/undo_stack.hpp"

namespace medicore {

UndoStack::UndoStack(size_t maxCapacity) : top_(nullptr), size_(0), maxCapacity_(maxCapacity) {}

void UndoStack::push(const ActionRecord& action) {
    auto newNode = std::make_shared<Node>(action);
    newNode->next = top_;
    top_ = newNode;
    size_++;

    // Enforce max capacity to avoid unbounded memory usage
    if (size_ > maxCapacity_) {
        // Prune bottom element
        auto curr = top_;
        for (size_t i = 1; i < maxCapacity_; ++i) {
            if (curr) curr = curr->next;
        }
        if (curr) curr->next = nullptr;
        size_ = maxCapacity_;
    }
}

ActionRecord UndoStack::pop() {
    if (isEmpty()) {
        throw std::runtime_error("Cannot pop from an empty undo stack");
    }

    auto removed = top_;
    ActionRecord data = removed->action;
    top_ = top_->next;
    size_--;
    return data;
}

const ActionRecord& UndoStack::top() const {
    if (isEmpty()) {
        throw std::runtime_error("Cannot peek top of an empty undo stack");
    }
    return top_->action;
}

void UndoStack::clear() {
    top_ = nullptr;
    size_ = 0;
}

std::vector<ActionRecord> UndoStack::getAll() const {
    std::vector<ActionRecord> list;
    list.reserve(size_);
    auto current = top_;
    while (current) {
        list.push_back(current->action);
        current = current->next;
    }
    return list;
}

nlohmann::json UndoStack::getDebugStructure() const {
    nlohmann::json j;
    j["size"] = size_;
    j["maxCapacity"] = maxCapacity_;
    j["isEmpty"] = isEmpty();

    nlohmann::json frames = nlohmann::json::array();
    auto current = top_;
    size_t index = 0;
    while (current) {
        nlohmann::json f;
        f["depth"] = index++;
        f["actionType"] = current->action.actionType;
        f["entityId"] = current->action.entityId;
        f["timestamp"] = current->action.timestamp;
        f["isTop"] = (index == 1);
        frames.push_back(f);
        current = current->next;
    }
    j["frames"] = frames;
    return j;
}

} // namespace medicore
