#include "operations.h"
#include "utils.h"
#include "ui.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ========================================================================================
// RECURSIVE SEARCH FUNCTIONS (LO2, LO3, Unit 5: Recursion)
// ========================================================================================

int recursiveSearchFamily(const Family families[], int size, const string& code, int index) {
    // Base case 1: reached end of array without match
    if (index >= size) {
        return -1;
    }
    // Base case 2: found match
    if (families[index].familyCode == code) {
        return index;
    }
    // Recursive step
    return recursiveSearchFamily(families, size, code, index + 1);
}

int recursiveSearchCenter(const EvacuationCenter centers[], int size, const string& name, int index) {
    if (index >= size) {
        return -1;
    }
    if (toUpperCase(centers[index].facilityName) == toUpperCase(name)) {
        return index;
    }
    return recursiveSearchCenter(centers, size, name, index + 1);
}

int recursiveSearchRequest(const ReliefRequest requests[], int size, const string& reqId, int index) {
    if (index >= size) {
        return -1;
    }
    if (requests[index].requestId == reqId) {
        return index;
    }
    return recursiveSearchRequest(requests, size, reqId, index + 1);
}

int recursiveSearchEvacuee(const Evacuee evacuees[], int size, const string& id, int index) {
    if (index >= size) {
        return -1;
    }
    if (evacuees[index].evacueeId == id) {
        return index;
    }
    return recursiveSearchEvacuee(evacuees, size, id, index + 1);
}

// ========================================================================================
// RECURSIVE AGGREGATION FUNCTIONS (LO2, LO3, Unit 5: Recursion)
// ========================================================================================

int recursiveCountHighVulnerability(const Family families[], int n) {
    // Base case
    if (n <= 0) {
        return 0;
    }
    // Recursive calculation: count in remaining elements + current element if active and HIGH
    int count = (families[n - 1].isActive && families[n - 1].vulnerabilityLevel == "HIGH") ? 1 : 0;
    return count + recursiveCountHighVulnerability(families, n - 1);
}

int recursiveSumOccupancy(const EvacuationCenter centers[], int n) {
    if (n <= 0) {
        return 0;
    }
    return centers[n - 1].currentOccupancy + recursiveSumOccupancy(centers, n - 1);
}

int recursiveSumPopulation(const Family families[], int n) {
    if (n <= 0) {
        return 0;
    }
    int count = families[n - 1].isActive ? families[n - 1].numberOfFamilyMembers : 0;
    return count + recursiveSumPopulation(families, n - 1);
}

int recursiveCountPendingRequests(const ReliefRequest requests[], int n) {
    if (n <= 0) {
        return 0;
    }
    int isPending = (requests[n - 1].requestStatus == "PENDING") ? 1 : 0;
    return isPending + recursiveCountPendingRequests(requests, n - 1);
}

// ========================================================================================
// RECURSIVE SORTING (Unit 6: Sorting & Unit 5: Recursion)
// Sorts Relief Requests by PriorityLevel in ascending numerical order (Priority 1 = Highest)
// ========================================================================================
void recursiveSortRequestsByPriority(ReliefRequest requests[], int n) {
    // Base case: 1 or 0 elements are already sorted
    if (n <= 1) {
        return;
    }

    // Find the minimum priority level (highest urgency: 1 is highest, 4 is lowest) in requests[0..n-1]
    int minIndex = 0;
    for (int i = 1; i < n; ++i) {
        if (requests[i].priorityLevel < requests[minIndex].priorityLevel) {
            minIndex = i;
        }
    }

    // Swap the highest priority request to the first position
    if (minIndex != 0) {
        ReliefRequest temp = requests[0];
        requests[0] = requests[minIndex];
        requests[minIndex] = temp;
    }

    // Recursively sort the remaining (n - 1) requests
    recursiveSortRequestsByPriority(requests + 1, n - 1);
}

// ========================================================================================
// FUNCTION OVERLOADING (Unit 5: Function Overloading)
// ========================================================================================

void displayRecord(const Family& family, bool detailed) {
    cout << Color::CYAN << "  Family Code    : " << Color::RESET << colorCode(family.familyCode) << "\n";
    cout << Color::CYAN << "  Head of Family : " << Color::RESET << Color::BOLD << family.headOfFamily << Color::RESET << "\n";
    cout << Color::CYAN << "  Total Members  : " << Color::RESET << family.numberOfFamilyMembers << "\n";
    cout << Color::CYAN << "  Vulnerability  : " << Color::RESET << badgeVulnerability(family.vulnerabilityLevel)
         << " (Score: " << Color::BOLD << family.vulnerabilityScore << Color::RESET << ")\n";
    cout << Color::CYAN << "  Record Status  : " << Color::RESET
         << (family.isActive ? (Color::BOLD + Color::BRIGHT_GREEN + "ACTIVE" + Color::RESET) : (Color::BOLD + Color::BRIGHT_RED + "DISABLED" + Color::RESET)) << "\n";

    if (detailed) {
        cout << Color::DIM << "  - Children (<12)        : " << Color::RESET << family.details.children << "\n";
        cout << Color::DIM << "  - Senior Citizens (60+) : " << Color::RESET << family.details.seniorCitizens << "\n";
        cout << Color::DIM << "  - Persons with Disability: " << Color::RESET << family.details.personsWithDisabilities << "\n";
        cout << Color::DIM << "  - Pregnant Members      : " << Color::RESET << family.details.pregnantMembers << "\n";
        cout << Color::DIM << "  - Medical Needs         : " << Color::RESET << family.details.medicalNeeds << "\n";
        cout << Color::DIM << "  - Evacuated Status      : " << Color::RESET
             << (family.isEvacuated ? (Color::BRIGHT_GREEN + "YES" + Color::RESET) : (Color::DIM + "NO" + Color::RESET)) << "\n";
    }
}

void displayRecord(const EvacuationCenter& center) {
    int availableSlots = center.maximumCapacity - center.currentOccupancy;
    cout << "  " << left << setw(26) << center.facilityName
         << setw(18) << center.facilityType
         << right << setw(10) << center.maximumCapacity
         << setw(12) << center.currentOccupancy
         << "   " << (availableSlots <= 0 ? Color::BRIGHT_RED : availableSlots < 20 ? Color::BRIGHT_YELLOW : Color::BRIGHT_GREEN)
         << right << setw(14) << availableSlots << Color::RESET << "\n";
}

void displayRecord(const ReliefRequest& request) {
    string prioStr;
    switch (request.priorityLevel) {
        case 1: prioStr = "L1-EMERGENCY"; break;
        case 2: prioStr = "L2-HIGH"; break;
        case 3: prioStr = "L3-MODERATE"; break;
        default: prioStr = "L4-ROUTINE"; break;
    }
    int prioPad = 18 - static_cast<int>(prioStr.length());
    if (prioPad < 1) prioPad = 1;

    int statPad = 16 - static_cast<int>(request.requestStatus.length());
    if (statPad < 1) statPad = 1;

    cout << "  " << left << setw(12) << request.requestId
         << setw(12) << request.familyCode
         << setw(20) << request.requestType
         << "  " << badgePriority(request.priorityLevel) << string(prioPad, ' ')
         << " " << badgeStatus(request.requestStatus) << string(statPad, ' ')
         << " " << Color::DIM << request.requestDescription << Color::RESET << "\n";
}

void displayRecord(const Evacuee& evacuee) {
    cout << "  " << left << setw(12) << evacuee.evacueeId
         << setw(12) << evacuee.familyCode
         << setw(24) << evacuee.fullName
         << right << setw(6) << evacuee.age
         << "   " << left << setw(20) << evacuee.assignedCenter
         << setw(20) << evacuee.specialNeeds << "\n";
}
