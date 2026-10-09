#pragma once

#include <vector>
#include <string>
#include <functional>
#include "../models/models.hpp"
#include "../third_party/json.hpp"

namespace medicore {

struct SortStep {
    std::string description;
    std::vector<int> currentArray;
    int leftIndex = -1;
    int rightIndex = -1;
    int midIndex = -1;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(SortStep, description, currentArray, leftIndex, rightIndex, midIndex)
};

struct BinarySearchStep {
    int low;
    int mid;
    int high;
    int midValue;
    std::string comparison; // "less", "greater", "equal"
    std::string explanation;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(BinarySearchStep, low, mid, high, midValue, comparison, explanation)
};

struct BinarySearchResult {
    bool found = false;
    int index = -1;
    int target = 0;
    int totalComparisons = 0;
    std::vector<BinarySearchStep> steps;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(BinarySearchResult, found, index, target, totalComparisons, steps)
};

class DsaAlgorithms {
public:
    // Merge Sort for Patient list by Age or Name
    static void mergeSortPatientsByAge(std::vector<Patient>& patients);
    static void mergeSortPatientsByName(std::vector<Patient>& patients);

    // Merge Sort for Appointment list by Date/Time
    static void mergeSortAppointmentsByTime(std::vector<Appointment>& appointments);

    // Educational Merge Sort with Step Tracing for Visualizer
    static std::vector<SortStep> traceMergeSort(std::vector<int>& arr);

    // Educational Binary Search with Step Tracing for Visualizer
    static BinarySearchResult binarySearch(const std::vector<int>& sortedArr, int target);

private:
    static void mergePatientsByAge(std::vector<Patient>& arr, int left, int mid, int right);
    static void mergeSortPatientsByAgeRec(std::vector<Patient>& arr, int left, int right);

    static void mergePatientsByName(std::vector<Patient>& arr, int left, int mid, int right);
    static void mergeSortPatientsByNameRec(std::vector<Patient>& arr, int left, int right);

    static void mergeAppointments(std::vector<Appointment>& arr, int left, int mid, int right);
    static void mergeSortAppointmentsRec(std::vector<Appointment>& arr, int left, int right);

    static void traceMergeSortRec(std::vector<int>& arr, int left, int right, std::vector<SortStep>& steps);
    static void traceMerge(std::vector<int>& arr, int left, int mid, int right, std::vector<SortStep>& steps);
};

} // namespace medicore
