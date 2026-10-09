#include "../../include/dsa/patient_hash_table.hpp"

namespace medicore {

PatientHashTable::PatientHashTable(size_t initialCapacity)
    : capacity_(initialCapacity < 4 ? 4 : initialCapacity), count_(0) {
    table_.resize(capacity_, nullptr);
}

size_t PatientHashTable::hashFunction(const std::string& key) const {
    size_t hash = 0;
    const size_t prime = 31;
    for (char c : key) {
        hash = hash * prime + static_cast<unsigned char>(c);
    }
    return hash % capacity_;
}

void PatientHashTable::insert(const Patient& patient) {
    if (patient.id.empty()) return;

    // Check if rehash needed
    if (static_cast<double>(count_ + 1) / capacity_ > MAX_LOAD_FACTOR) {
        rehash();
    }

    size_t index = hashFunction(patient.id);
    auto current = table_[index];

    // Check if key already exists, if so update
    while (current) {
        if (current->key == patient.id) {
            current->value = patient;
            return;
        }
        current = current->next;
    }

    // Insert at front of bucket chain (O(1))
    auto newNode = std::make_shared<Node>(patient.id, patient);
    newNode->next = table_[index];
    table_[index] = newNode;
    count_++;
}

Patient* PatientHashTable::search(const std::string& patientId) {
    if (patientId.empty() || capacity_ == 0) return nullptr;

    size_t index = hashFunction(patientId);
    auto current = table_[index];

    while (current) {
        if (current->key == patientId) {
            return &current->value;
        }
        current = current->next;
    }
    return nullptr;
}

const Patient* PatientHashTable::search(const std::string& patientId) const {
    if (patientId.empty() || capacity_ == 0) return nullptr;

    size_t index = hashFunction(patientId);
    auto current = table_[index];

    while (current) {
        if (current->key == patientId) {
            return &current->value;
        }
        current = current->next;
    }
    return nullptr;
}

bool PatientHashTable::remove(const std::string& patientId) {
    if (patientId.empty() || capacity_ == 0) return false;

    size_t index = hashFunction(patientId);
    auto current = table_[index];
    std::shared_ptr<Node> prev = nullptr;

    while (current) {
        if (current->key == patientId) {
            if (prev) {
                prev->next = current->next;
            } else {
                table_[index] = current->next;
            }
            count_--;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

bool PatientHashTable::contains(const std::string& patientId) const {
    return search(patientId) != nullptr;
}

std::vector<Patient> PatientHashTable::getAll() const {
    std::vector<Patient> result;
    result.reserve(count_);
    for (size_t i = 0; i < capacity_; ++i) {
        auto current = table_[i];
        while (current) {
            result.push_back(current->value);
            current = current->next;
        }
    }
    return result;
}

void PatientHashTable::clear() {
    for (size_t i = 0; i < capacity_; ++i) {
        table_[i] = nullptr;
    }
    count_ = 0;
}

void PatientHashTable::rehash() {
    size_t oldCapacity = capacity_;
    capacity_ = oldCapacity * 2;
    std::vector<std::shared_ptr<Node>> oldTable = std::move(table_);

    table_.clear();
    table_.resize(capacity_, nullptr);
    count_ = 0;

    for (size_t i = 0; i < oldCapacity; ++i) {
        auto current = oldTable[i];
        while (current) {
            insert(current->value);
            current = current->next;
        }
    }
}

nlohmann::json PatientHashTable::getDebugStructure() const {
    nlohmann::json j;
    j["capacity"] = capacity_;
    j["count"] = count_;
    j["loadFactor"] = loadFactor();
    j["maxLoadFactor"] = MAX_LOAD_FACTOR;

    nlohmann::json buckets = nlohmann::json::array();
    size_t collisions = 0;

    for (size_t i = 0; i < capacity_; ++i) {
        nlohmann::json bucket;
        bucket["index"] = i;
        nlohmann::json chain = nlohmann::json::array();

        auto current = table_[i];
        size_t chainLen = 0;
        while (current) {
            nlohmann::json node;
            node["key"] = current->key;
            node["name"] = current->value.name;
            node["age"] = current->value.age;
            node["bloodGroup"] = current->value.bloodGroup;
            chain.push_back(node);
            chainLen++;
            current = current->next;
        }
        if (chainLen > 1) {
            collisions += (chainLen - 1);
        }
        bucket["chainLength"] = chainLen;
        bucket["items"] = chain;
        buckets.push_back(bucket);
    }

    j["totalCollisions"] = collisions;
    j["buckets"] = buckets;
    return j;
}

} // namespace medicore
