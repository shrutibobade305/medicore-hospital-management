#include "../../include/dsa/doctor_bst.hpp"
#include <algorithm>

namespace medicore {

DoctorBST::DoctorBST() : root_(nullptr), count_(0) {}

void DoctorBST::clear() {
    root_ = nullptr;
    count_ = 0;
}

void DoctorBST::insert(const Doctor& doctor) {
    if (doctor.id.empty()) return;
    root_ = insertRec(root_, doctor);
}

std::shared_ptr<DoctorBST::Node> DoctorBST::insertRec(std::shared_ptr<Node> node, const Doctor& doc) {
    if (!node) {
        count_++;
        return std::make_shared<Node>(doc);
    }

    if (doc.id < node->data.id) {
        node->left = insertRec(node->left, doc);
    } else if (doc.id > node->data.id) {
        node->right = insertRec(node->right, doc);
    } else {
        // Update existing record
        node->data = doc;
    }
    return node;
}

Doctor* DoctorBST::search(const std::string& doctorId) {
    auto node = searchRec(root_, doctorId);
    return node ? &node->data : nullptr;
}

const Doctor* DoctorBST::search(const std::string& doctorId) const {
    auto node = searchRec(root_, doctorId);
    return node ? &node->data : nullptr;
}

std::shared_ptr<DoctorBST::Node> DoctorBST::searchRec(std::shared_ptr<Node> node, const std::string& doctorId) const {
    if (!node || node->data.id == doctorId) {
        return node;
    }
    if (doctorId < node->data.id) {
        return searchRec(node->left, doctorId);
    }
    return searchRec(node->right, doctorId);
}

bool DoctorBST::remove(const std::string& doctorId) {
    if (doctorId.empty() || !root_) return false;
    bool deleted = false;
    root_ = removeRec(root_, doctorId, deleted);
    if (deleted) count_--;
    return deleted;
}

std::shared_ptr<DoctorBST::Node> DoctorBST::findMin(std::shared_ptr<Node> node) const {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

std::shared_ptr<DoctorBST::Node> DoctorBST::removeRec(std::shared_ptr<Node> node, const std::string& doctorId, bool& deleted) {
    if (!node) return nullptr;

    if (doctorId < node->data.id) {
        node->left = removeRec(node->left, doctorId, deleted);
    } else if (doctorId > node->data.id) {
        node->right = removeRec(node->right, doctorId, deleted);
    } else {
        deleted = true;
        // Node found
        // Case 1: No child
        if (!node->left && !node->right) {
            return nullptr;
        }
        // Case 2: One child
        if (!node->left) {
            return node->right;
        }
        if (!node->right) {
            return node->left;
        }
        // Case 3: Two children -> Inorder successor (minimum in right subtree)
        auto successor = findMin(node->right);
        node->data = successor->data;
        node->right = removeRec(node->right, successor->data.id, deleted);
    }
    return node;
}

int DoctorBST::height() const {
    return heightRec(root_);
}

int DoctorBST::heightRec(const std::shared_ptr<Node>& node) const {
    if (!node) return 0;
    return 1 + std::max(heightRec(node->left), heightRec(node->right));
}

std::vector<Doctor> DoctorBST::inorder() const {
    std::vector<Doctor> result;
    result.reserve(count_);
    inorderRec(root_, result);
    return result;
}

void DoctorBST::inorderRec(const std::shared_ptr<Node>& node, std::vector<Doctor>& result) const {
    if (!node) return;
    inorderRec(node->left, result);
    result.push_back(node->data);
    inorderRec(node->right, result);
}

std::vector<Doctor> DoctorBST::preorder() const {
    std::vector<Doctor> result;
    result.reserve(count_);
    preorderRec(root_, result);
    return result;
}

void DoctorBST::preorderRec(const std::shared_ptr<Node>& node, std::vector<Doctor>& result) const {
    if (!node) return;
    result.push_back(node->data);
    preorderRec(node->left, result);
    preorderRec(node->right, result);
}

std::vector<Doctor> DoctorBST::postorder() const {
    std::vector<Doctor> result;
    result.reserve(count_);
    postorderRec(root_, result);
    return result;
}

void DoctorBST::postorderRec(const std::shared_ptr<Node>& node, std::vector<Doctor>& result) const {
    if (!node) return;
    postorderRec(node->left, result);
    postorderRec(node->right, result);
    result.push_back(node->data);
}

nlohmann::json DoctorBST::nodeToJson(const std::shared_ptr<Node>& node) const {
    if (!node) return nullptr;
    nlohmann::json j;
    j["id"] = node->data.id;
    j["name"] = node->data.name;
    j["department"] = node->data.department;
    j["specialization"] = node->data.specialization;
    j["availability"] = node->data.availability;
    j["left"] = nodeToJson(node->left);
    j["right"] = nodeToJson(node->right);
    return j;
}

nlohmann::json DoctorBST::getDebugStructure() const {
    nlohmann::json j;
    j["size"] = count_;
    j["height"] = height();
    j["isEmpty"] = isEmpty();
    j["root"] = nodeToJson(root_);
    j["inorderCount"] = count_;
    return j;
}

} // namespace medicore
