#pragma once

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "../models/models.hpp"
#include "../third_party/json.hpp"

namespace medicore {

/**
 * Custom FIFO Queue for Ordinary Patient Waiting Lists
 * Time Complexity:
 *   - Enqueue: O(1)
 *   - Dequeue: O(1)
 *   - Peek: O(1)
 *   - Size / IsEmpty: O(1)
 * Space Complexity: O(n)
 */
class AppointmentQueue {
public:
    struct Node {
        Appointment data;
        std::shared_ptr<Node> next;

        explicit Node(const Appointment& appt) : data(appt), next(nullptr) {}
    };

    AppointmentQueue();
    ~AppointmentQueue() = default;

    AppointmentQueue(const AppointmentQueue&) = delete;
    AppointmentQueue& operator=(const AppointmentQueue&) = delete;

    void enqueue(const Appointment& appointment);
    Appointment dequeue();
    const Appointment& peek() const;
    bool isEmpty() const { return size_ == 0; }
    size_t size() const { return size_; }
    void clear();

    std::vector<Appointment> getAll() const;
    nlohmann::json getDebugStructure() const;

private:
    std::shared_ptr<Node> front_;
    std::shared_ptr<Node> rear_;
    size_t size_;
};

} // namespace medicore
