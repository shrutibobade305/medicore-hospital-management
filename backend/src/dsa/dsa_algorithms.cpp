#include "../../include/dsa/dsa_algorithms.hpp"
#include <sstream>

namespace medicore {

// --- Merge Sort for Patients by Age ---
void DsaAlgorithms::mergeSortPatientsByAge(std::vector<Patient>& patients) {
    if (patients.size() <= 1) return;
    mergeSortPatientsByAgeRec(patients, 0, static_cast<int>(patients.size()) - 1);
}

void DsaAlgorithms::mergeSortPatientsByAgeRec(std::vector<Patient>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortPatientsByAgeRec(arr, left, mid);
        mergeSortPatientsByAgeRec(arr, mid + 1, right);
        mergePatientsByAge(arr, left, mid, right);
    }
}

void DsaAlgorithms::mergePatientsByAge(std::vector<Patient>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<Patient> L(arr.begin() + left, arr.begin() + mid + 1);
    std::vector<Patient> R(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].age <= R[j].age) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// --- Merge Sort for Patients by Name ---
void DsaAlgorithms::mergeSortPatientsByName(std::vector<Patient>& patients) {
    if (patients.size() <= 1) return;
    mergeSortPatientsByNameRec(patients, 0, static_cast<int>(patients.size()) - 1);
}

void DsaAlgorithms::mergeSortPatientsByNameRec(std::vector<Patient>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortPatientsByNameRec(arr, left, mid);
        mergeSortPatientsByNameRec(arr, mid + 1, right);
        mergePatientsByName(arr, left, mid, right);
    }
}

void DsaAlgorithms::mergePatientsByName(std::vector<Patient>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<Patient> L(arr.begin() + left, arr.begin() + mid + 1);
    std::vector<Patient> R(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].name <= R[j].name) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// --- Merge Sort for Appointments by Time ---
void DsaAlgorithms::mergeSortAppointmentsByTime(std::vector<Appointment>& appointments) {
    if (appointments.size() <= 1) return;
    mergeSortAppointmentsRec(appointments, 0, static_cast<int>(appointments.size()) - 1);
}

void DsaAlgorithms::mergeSortAppointmentsRec(std::vector<Appointment>& arr, int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSortAppointmentsRec(arr, left, mid);
        mergeSortAppointmentsRec(arr, mid + 1, right);
        mergeAppointments(arr, left, mid, right);
    }
}

void DsaAlgorithms::mergeAppointments(std::vector<Appointment>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<Appointment> L(arr.begin() + left, arr.begin() + mid + 1);
    std::vector<Appointment> R(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        // Compare date first, then time
        std::string dtL = L[i].appointmentDate + " " + L[i].appointmentTime;
        std::string dtR = R[j].appointmentDate + " " + R[j].appointmentTime;
        if (dtL <= dtR) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

// --- Educational Merge Sort with Step Tracing ---
std::vector<SortStep> DsaAlgorithms::traceMergeSort(std::vector<int>& arr) {
    std::vector<SortStep> steps;
    if (arr.empty()) return steps;

    SortStep initStep;
    initStep.description = "Initial Array";
    initStep.currentArray = arr;
    steps.push_back(initStep);

    traceMergeSortRec(arr, 0, static_cast<int>(arr.size()) - 1, steps);

    SortStep finalStep;
    finalStep.description = "Merge Sort Complete (Sorted Array)";
    finalStep.currentArray = arr;
    steps.push_back(finalStep);

    return steps;
}

void DsaAlgorithms::traceMergeSortRec(std::vector<int>& arr, int left, int right, std::vector<SortStep>& steps) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        std::ostringstream oss;
        oss << "Dividing array range [" << left << "..." << right << "] into [" << left << "..." << mid << "] and [" << (mid + 1) << "..." << right << "]";
        SortStep step;
        step.description = oss.str();
        step.currentArray = arr;
        step.leftIndex = left;
        step.rightIndex = right;
        step.midIndex = mid;
        steps.push_back(step);

        traceMergeSortRec(arr, left, mid, steps);
        traceMergeSortRec(arr, mid + 1, right, steps);
        traceMerge(arr, left, mid, right, steps);
    }
}

void DsaAlgorithms::traceMerge(std::vector<int>& arr, int left, int mid, int right, std::vector<SortStep>& steps) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(arr.begin() + left, arr.begin() + mid + 1);
    std::vector<int> R(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];

    std::ostringstream oss;
    oss << "Merged subarrays [" << left << "..." << mid << "] and [" << (mid + 1) << "..." << right << "] into sorted range [" << left << "..." << right << "]";
    SortStep step;
    step.description = oss.str();
    step.currentArray = arr;
    step.leftIndex = left;
    step.rightIndex = right;
    step.midIndex = mid;
    steps.push_back(step);
}

// --- Educational Binary Search with Step Tracing ---
BinarySearchResult DsaAlgorithms::binarySearch(const std::vector<int>& sortedArr, int target) {
    BinarySearchResult result;
    result.target = target;
    result.found = false;
    result.index = -1;
    result.totalComparisons = 0;

    int low = 0;
    int high = static_cast<int>(sortedArr.size()) - 1;

    while (low <= high) {
        result.totalComparisons++;
        int mid = low + (high - low) / 2;
        int midVal = sortedArr[mid];

        BinarySearchStep step;
        step.low = low;
        step.mid = mid;
        step.high = high;
        step.midValue = midVal;

        std::ostringstream oss;
        if (midVal == target) {
            step.comparison = "equal";
            oss << "Target " << target << " matches element at index " << mid << "!";
            step.explanation = oss.str();
            result.steps.push_back(step);
            result.found = true;
            result.index = mid;
            return result;
        } else if (midVal < target) {
            step.comparison = "less";
            oss << "Element at index " << mid << " (" << midVal << ") is less than target " << target
                << ". Eliminating left half [" << low << "..." << mid << "], searching right half.";
            step.explanation = oss.str();
            result.steps.push_back(step);
            low = mid + 1;
        } else {
            step.comparison = "greater";
            oss << "Element at index " << mid << " (" << midVal << ") is greater than target " << target
                << ". Eliminating right half [" << mid << "..." << high << "], searching left half.";
            step.explanation = oss.str();
            result.steps.push_back(step);
            high = mid - 1;
        }
    }

    return result;
}

} // namespace medicore
