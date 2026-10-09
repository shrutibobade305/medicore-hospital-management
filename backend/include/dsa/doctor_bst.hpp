#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../models/models.hpp"
#include "../third_party/json.hpp"

namespace medicore {

/**
 * Custom Binary Search Tree (BST) for Doctor Records
 * Ordered by Doctor ID (or Name)
 *
 * Time Complexity:
 *   - Search: Average O(log n), Worst O(n)
 *   - Insert: Average O(log n), Worst O(n)
 *   - Delete: Average O(log n), Worst O(n)
 *   - Inorder Traversal (Sorted Output): O(n)
 * Space Complexity: O(n)
 */
class DoctorBST {
public:
    struct Node {
        Doctor data;
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;

        explicit Node(const Doctor& doc) : data(doc), left(nullptr), right(nullptr) {}
    };

    DoctorBST();
    ~DoctorBST() = default;

    DoctorBST(const DoctorBST&) = delete;
    DoctorBST& operator=(const DoctorBST&) = delete;

    void insert(const Doctor& doctor);
    Doctor* search(const std::string& doctorId);
    const Doctor* search(const std::string& doctorId) const;
    bool remove(const std::string& doctorId);
    void clear();

    bool isEmpty() const { return root_ == nullptr; }
    size_t size() const { return count_; }
    int height() const;

    std::vector<Doctor> inorder() const;
    std::vector<Doctor> preorder() const;
    std::vector<Doctor> postorder() const;

    nlohmann::json getDebugStructure() const;

private:
    std::shared_ptr<Node> insertRec(std::shared_ptr<Node> node, const Doctor& doc);
    std::shared_ptr<Node> searchRec(std::shared_ptr<Node> node, const std::string& doctorId) const;
    std::shared_ptr<Node> removeRec(std::shared_ptr<Node> node, const std::string& doctorId, bool& deleted);
    std::shared_ptr<Node> findMin(std::shared_ptr<Node> node) const;
    int heightRec(const std::shared_ptr<Node>& node) const;

    void inorderRec(const std::shared_ptr<Node>& node, std::vector<Doctor>& result) const;
    void preorderRec(const std::shared_ptr<Node>& node, std::vector<Doctor>& result) const;
    void postorderRec(const std::shared_ptr<Node>& node, std::vector<Doctor>& result) const;

    nlohmann::json nodeToJson(const std::shared_ptr<Node>& node) const;

    std::shared_ptr<Node> root_;
    size_t count_;
};

} // namespace medicore
