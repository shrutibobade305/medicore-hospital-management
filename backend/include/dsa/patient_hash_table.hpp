#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../models/models.hpp"
#include "../third_party/json.hpp"

namespace medicore {

/**
 * Custom Hash Table with Separate Chaining for Patient Lookup
 * Time Complexity:
 *   - Search: Average O(1), Worst O(n)
 *   - Insert: Average O(1), Worst O(n)
 *   - Delete: Average O(1), Worst O(n)
 * Space Complexity: O(n + m) where m is bucket capacity
 */
class PatientHashTable {
public:
    struct Node {
        std::string key;
        Patient value;
        std::shared_ptr<Node> next;

        Node(const std::string& k, const Patient& v) : key(k), value(v), next(nullptr) {}
    };

    explicit PatientHashTable(size_t initialCapacity = 16);
    ~PatientHashTable() = default;

    // Prevent accidental copying
    PatientHashTable(const PatientHashTable&) = delete;
    PatientHashTable& operator=(const PatientHashTable&) = delete;

    void insert(const Patient& patient);
    Patient* search(const std::string& patientId);
    const Patient* search(const std::string& patientId) const;
    bool remove(const std::string& patientId);
    bool contains(const std::string& patientId) const;
    std::vector<Patient> getAll() const;
    void clear();

    size_t size() const { return count_; }
    size_t capacity() const { return capacity_; }
    double loadFactor() const { return static_cast<double>(count_) / capacity_; }

    // Educational / Visualizer method
    nlohmann::json getDebugStructure() const;

private:
    size_t hashFunction(const std::string& key) const;
    void rehash();

    std::vector<std::shared_ptr<Node>> table_;
    size_t capacity_;
    size_t count_;
    static constexpr double MAX_LOAD_FACTOR = 0.75;
};

} // namespace medicore
