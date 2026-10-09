#include "../../include/dsa/appointment_queue.hpp"

namespace medicore {

AppointmentQueue::AppointmentQueue() : front_(nullptr), rear_(nullptr), size_(0) {}

void AppointmentQueue::enqueue(const Appointment& appointment) {
    auto newNode = std::make_shared<Node>(appointment);
    if (rear_) {
        rear_->next = newNode;
        rear_ = newNode;
    } else {
        front_ = newNode;
        rear_ = newNode;
    }
    size_++;
}

Appointment AppointmentQueue::dequeue() {
    if (isEmpty()) {
        throw std::runtime_error("Cannot dequeue from an empty queue");
    }

    auto removed = front_;
    Appointment data = removed->data;
    front_ = front_->next;

    if (!front_) {
        rear_ = nullptr;
    }
    size_--;
    return data;
}

const Appointment& AppointmentQueue::peek() const {
    if (isEmpty()) {
        throw std::runtime_error("Cannot peek into an empty queue");
    }
    return front_->data;
}

void AppointmentQueue::clear() {
    front_ = nullptr;
    rear_ = nullptr;
    size_ = 0;
}

std::vector<Appointment> AppointmentQueue::getAll() const {
    std::vector<Appointment> list;
    list.reserve(size_);
    auto current = front_;
    while (current) {
        list.push_back(current->data);
        current = current->next;
    }
    return list;
}

nlohmann::json AppointmentQueue::getDebugStructure() const {
    nlohmann::json j;
    j["size"] = size_;
    j["isEmpty"] = isEmpty();

    nlohmann::json items = nlohmann::json::array();
    auto current = front_;
    size_t index = 0;
    while (current) {
        nlohmann::json item;
        item["position"] = index++;
        item["id"] = current->data.id;
        item["patientName"] = current->data.patientName;
        item["doctorName"] = current->data.doctorName;
        item["time"] = current->data.appointmentTime;
        item["status"] = current->data.status;
        item["isFront"] = (index == 1);
        item["isRear"] = (index == size_);
        items.push_back(item);
        current = current->next;
    }
    j["elements"] = items;
    return j;
}

} // namespace medicore
