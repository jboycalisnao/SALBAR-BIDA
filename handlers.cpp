#include "handlers.h"
#include "utils.h"
#include "analytics.h"
#include "operations.h"
#include "persistence.h"
#include "ui.h"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ========================================================================================
// CASE 1: FAMILY REGISTRATION
// ========================================================================================
void handleFamilyRegistration(Family families[], int& familyCount, int& nextFamilyNumber,
                              Evacuee evacuees[], int& evacueeCount, int& nextEvacueeNumber,
                              EvacuationCenter centers[], int centerCount, int activeCenterIndex) {
    clearScreen();
    printSectionHeader("FAMILY REGISTRATION");

    // Guard: Do not allow adding family members if the facility is not registered yet
    if (centerCount == 0) {
        printError("Registration blocked! No evacuation facility is registered yet.");
        printWarning("Barangay disaster guidelines require registering an Evacuation Center first.");
        printInfo("Please select Option [3] (Evacuation Center Management) to register a facility.");
        pauseConsole();
        return;
    }

    if (familyCount >= MAX_FAMILIES) {
        printError("Family capacity reached (" + to_string(MAX_FAMILIES) + " families).");
        pauseConsole();
        return;
    }

    // Ensure activeCenterIndex is valid
    if (activeCenterIndex < 0 || activeCenterIndex >= centerCount) {
        activeCenterIndex = 0;
    }

    int availableSlots = centers[activeCenterIndex].maximumCapacity - centers[activeCenterIndex].currentOccupancy;
    if (availableSlots <= 0) {
        printError("Facility '" + centers[activeCenterIndex].facilityName + "' is currently at full capacity ("
                   + to_string(centers[activeCenterIndex].maximumCapacity) + "/" + to_string(centers[activeCenterIndex].maximumCapacity) + ")!");
        printWarning("Please select another evacuation facility in Option [3] or expand facility capacity.");
        pauseConsole();
        return;
    }

    printInfo("Assigning to Current Facility: " + centers[activeCenterIndex].facilityName + " (" + centers[activeCenterIndex].facilityType
              + " | Available Slots: " + to_string(availableSlots) + ")");

    string headOfFamily = promptNonEmptyStringOrCancel(Color::CYAN + "Enter Name of the Head of the Family (or '0' / 'exit' to cancel): " + Color::RESET);
    if (headOfFamily.empty()) {
        printInfo("Family registration cancelled. Returning to main menu.");
        return;
    }

    int numberOfFamilyMembers = promptBoundedIntOrCancel("Enter Number of Family Members (or '0' to cancel)", 1, 100, 0, "Must be at least 1");
    if (numberOfFamilyMembers == 0) {
        printInfo("Family registration cancelled. Returning to main menu.");
        return;
    }

    // Capacity check: Ensure facility can accommodate the new family
    if (numberOfFamilyMembers > availableSlots) {
        printError("Registration blocked! Insufficient capacity in '" + centers[activeCenterIndex].facilityName + "'.");
        printWarning("Available slots: " + to_string(availableSlots) + ", but family has " + to_string(numberOfFamilyMembers) + " members.");
        printInfo("Please select another facility in Option [3] or expand facility capacity.");
        pauseConsole();
        return;
    }

    // GENERATE FamilyCode USING "FAM-" + NextFamilyNumber FORMAT NUMBER AS 4 DIGITS
    string familyCode = formatCode("FAM-", nextFamilyNumber, 4);

    VulnerabilityDetails details;

    // 1. Children must not exceed total family members
    details.children = promptBoundedInt("Enter Number of Children (<12 yrs)", 0, numberOfFamilyMembers, "Cannot exceed total family members");

    // 2. Seniors must not exceed remaining family members (total members - children)
    int maxSeniors = numberOfFamilyMembers - details.children;
    details.seniorCitizens = promptBoundedInt("Enter Number of Senior Citizens (60+ yrs)", 0, maxSeniors, "Remaining non-child family members");

    // 3. PWD must not exceed total family members
    details.personsWithDisabilities = promptBoundedInt("Enter Number of Persons with Disabilities", 0, numberOfFamilyMembers, "Cannot exceed total family members");

    // 4. Pregnant members must not exceed total family members
    details.pregnantMembers = promptBoundedInt("Enter Number of Pregnant Members", 0, numberOfFamilyMembers, "Cannot exceed total family members");

    // 5. Medical needs must not exceed total family members
    details.medicalNeeds = promptBoundedInt("Enter Number of Members with Medical Needs", 0, numberOfFamilyMembers, "Cannot exceed total family members");

    // Automatically compute total vulnerable members from demographic breakdown
    int vulnerableCount = details.children + details.seniorCitizens + details.personsWithDisabilities + details.pregnantMembers + details.medicalNeeds;
    if (vulnerableCount > numberOfFamilyMembers) {
        vulnerableCount = numberOfFamilyMembers;
    }

    // CALCULATE VulnerabilityScore
    int vulnerabilityScore = calculateVulnerabilityScore(details);
    string vulnerabilityLevel = determineVulnerabilityLevel(vulnerabilityScore);

    // STORE family record
    Family newFamily;
    newFamily.familyCode = familyCode;
    newFamily.headOfFamily = headOfFamily;
    newFamily.numberOfFamilyMembers = numberOfFamilyMembers;
    newFamily.numberOfVulnerableMembers = vulnerableCount;
    newFamily.vulnerabilityScore = vulnerabilityScore;
    newFamily.vulnerabilityLevel = vulnerabilityLevel;
    newFamily.details = details;
    newFamily.isEvacuated = true;
    newFamily.isActive = true;

    families[familyCount] = newFamily;
    familyCount++;

    // Update facility occupancy with newly registered family members
    centers[activeCenterIndex].currentOccupancy += numberOfFamilyMembers;

    // Optional evacuee registration linking for the head of family
    if (evacueeCount < MAX_EVACUEES) {
        Evacuee headEvacuee;
        headEvacuee.evacueeId = formatCode("EVAC-", nextEvacueeNumber, 4);
        headEvacuee.familyCode = familyCode;
        headEvacuee.fullName = headOfFamily;
        headEvacuee.age = 35; // Default representative age
        headEvacuee.assignedCenter = centers[activeCenterIndex].facilityName;
        headEvacuee.specialNeeds = (details.medicalNeeds > 0) ? "Medical Attention" : "None";

        evacuees[evacueeCount] = headEvacuee;
        evacueeCount++;
        nextEvacueeNumber++;
    }

    // DISPLAY confirmation
    cout << "\n";
    printSuccess("Family registered successfully!");
    cout << "  Family Code        : " << colorCode(familyCode) << "\n";
    cout << "  Assigned Facility  : " << Color::BOLD << Color::BRIGHT_GREEN << centers[activeCenterIndex].facilityName << Color::RESET << "\n";
    cout << "  Updated Occupancy  : " << Color::BOLD << centers[activeCenterIndex].currentOccupancy << "/"
         << centers[activeCenterIndex].maximumCapacity << Color::RESET
         << " (Available Slots: " << Color::BOLD << Color::BRIGHT_CYAN
         << (centers[activeCenterIndex].maximumCapacity - centers[activeCenterIndex].currentOccupancy) << Color::RESET << ")\n";
    cout << "  Vulnerability Level: " << badgeVulnerability(vulnerabilityLevel)
         << " (Computed Score: " << Color::BOLD << vulnerabilityScore << Color::RESET << ")\n";

    // INCREMENT NextFamilyNumber
    nextFamilyNumber++;
    pauseConsole();
}

// ========================================================================================
// CASE 2: VIEW POPULATION
// ========================================================================================
void handleViewPopulation(const Family families[], int familyCount) {
    clearScreen();
    printSectionHeader("POPULATION & FAMILY REGISTRY");

    if (familyCount == 0) {
        printWarning("No families registered yet.");
        pauseConsole();
        return;
    }

    // Check count of active families
    int activeCount = 0;
    for (int i = 0; i < familyCount; ++i) {
        if (families[i].isActive) {
            activeCount++;
        }
    }

    if (activeCount == 0) {
        printWarning("No active families found (all registered records are currently disabled).");
        pauseConsole();
        return;
    }

    cout << Color::BOLD << Color::BRIGHT_CYAN << "--- ALL REGISTERED FAMILIES ---\n" << Color::RESET;
    printDivider(78, '=');
    cout << left << setw(14) << "Family Code"
         << setw(26) << "Head of Family"
         << right << setw(10) << "Members"
         << "   " << left << setw(16) << "Vulnerability"
         << "Status\n";
    printDivider(78, '-');

    // FOR EACH Family IN FamilyList (only active records shown)
    for (int i = 0; i < familyCount; ++i) {
        if (!families[i].isActive) {
            continue; // Do not show disabled records in the family list
        }

        // 1. Family Code (styled, width 14)
        cout << Color::BOLD << Color::BRIGHT_CYAN << left << setw(14) << families[i].familyCode << Color::RESET;

        // 2. Head of Family (width 26)
        cout << left << setw(26) << families[i].headOfFamily;

        // 3. Members (width 10 right aligned + 3 spaces)
        cout << right << setw(10) << families[i].numberOfFamilyMembers << "   ";

        // 4. Vulnerability Badge (visible width 16)
        string vulnBadge = "[" + families[i].vulnerabilityLevel + "]";
        int vulnPadLen = 16 - static_cast<int>(vulnBadge.length());
        if (vulnPadLen < 1) vulnPadLen = 1;
        cout << badgeVulnerability(families[i].vulnerabilityLevel) << string(vulnPadLen, ' ');

        // 5. Status Badge
        cout << Color::BOLD << Color::BRIGHT_GREEN << "ACTIVE" << Color::RESET << "\n";
    }
    printDivider(78, '=');
    pauseConsole();
}

// ========================================================================================
// CASE 3: EVACUATION CENTER MANAGEMENT
// ========================================================================================
void handleEvacuationCenterManagement(EvacuationCenter centers[], int& centerCount, int& activeCenterIndex,
                                     Evacuee evacuees[], int& evacueeCount,
                                     Family families[], int familyCount) {
    clearScreen();
    printSectionHeader("EVACUATION CENTER MANAGEMENT");
    cout << "  [1] Add Evacuation Center\n";
    cout << "  [2] View Evacuation Centers Roster\n";
    cout << "  [3] Update Center Capacity & Type\n";
    cout << "  [4] Delete Evacuation Center\n";
    cout << "  [5] Select Current / Active Facility\n";
    cout << "  [0] Back to Main Menu\n";
    cout << Color::BOLD << "Enter Choice [0-5]: " << Color::RESET;

    int centerChoice;
    if (!(cin >> centerChoice)) {
        printError("Invalid selection.");
        clearInputBuffer();
        pauseConsole();
        return;
    }
    clearInputBuffer();

    cout << "\n";

    if (centerChoice == 0) {
        printInfo("Returning to main menu.");
        return;
    }

    if (centerChoice == 1) {
        if (centerCount >= MAX_CENTERS) {
            printError("Maximum evacuation centers reached (" + to_string(MAX_CENTERS) + ").");
            pauseConsole();
            return;
        }

        string facilityName = promptNonEmptyStringOrCancel("Enter Facility Name (or '0' / 'exit' to cancel): ");
        if (facilityName.empty()) {
            printInfo("Operation cancelled. Returning to menu.");
            return;
        }

        string facilityType = promptNonEmptyStringOrCancel("Enter Facility Type (e.g., Gym, School, Barangay Hall, or '0' to cancel): ");
        if (facilityType.empty()) {
            printInfo("Operation cancelled. Returning to menu.");
            return;
        }

        int maximumCapacity = promptBoundedIntOrCancel("Enter Maximum Capacity", 1, 100000, 0, "Capacity must be greater than 0");
        if (maximumCapacity == 0) {
            printInfo("Operation cancelled. Returning to menu.");
            return;
        }

        int currentOccupancy = promptBoundedIntOrCancel("Enter Current Occupancy", 0, maximumCapacity, -1, "Cannot exceed maximum capacity");
        if (currentOccupancy == -1) {
            printInfo("Operation cancelled. Returning to menu.");
            return;
        }

        centers[centerCount].facilityName = facilityName;
        centers[centerCount].facilityType = facilityType;
        centers[centerCount].maximumCapacity = maximumCapacity;
        centers[centerCount].currentOccupancy = currentOccupancy;
        centerCount++;
        printSuccess("Evacuation center added successfully.");
        pauseConsole();
    } else if (centerChoice == 2) {
        if (centerCount == 0) {
            printWarning("No evacuation centers registered yet.");
            pauseConsole();
            return;
        }

        cout << Color::BOLD << Color::BRIGHT_CYAN << "--- EVACUATION CENTERS ROSTER ---\n" << Color::RESET;
        printDivider(85, '=');
        cout << "  " << left << setw(26) << "Facility Name"
             << setw(18) << "Facility Type"
             << right << setw(10) << "Max Cap"
             << setw(12) << "Occupancy"
             << setw(16) << "Available Slots\n";
        printDivider(85, '-');

        for (int i = 0; i < centerCount; ++i) {
            displayRecord(centers[i]);
        }
        printDivider(85, '=');
        pauseConsole();
    } else if (centerChoice == 3) {
        if (centerCount == 0) {
            printWarning("No evacuation centers registered yet.");
            pauseConsole();
            return;
        }

        int index = -1;
        while (true) {
            string searchName = promptNonEmptyStringOrCancel("Enter Facility Name to Update (or '0' / 'exit' to cancel): ");
            if (searchName.empty()) {
                printInfo("Operation cancelled.");
                return;
            }

            index = recursiveSearchCenter(centers, centerCount, searchName);
            if (index != -1) {
                break;
            }
            printError("Evacuation center '" + searchName + "' not found. Please try again (or enter '0' to cancel).");
        }

        cout << "\n";
        printInfo("Center Located: " + centers[index].facilityName + " (" + centers[index].facilityType + ")");
        cout << "Enter New Facility Type (press Enter to keep current): ";
        string newType;
        getline(cin, newType);
        if (!newType.empty()) {
            centers[index].facilityType = newType;
        }

        cout << "Enter New Maximum Capacity (Current: " << centers[index].maximumCapacity << "): ";
        int newMax;
        if (cin >> newMax && newMax > 0) {
            cout << "Enter New Current Occupancy (Current: " << centers[index].currentOccupancy << "): ";
            int newOcc;
            if (cin >> newOcc && newOcc >= 0 && newOcc <= newMax) {
                centers[index].maximumCapacity = newMax;
                centers[index].currentOccupancy = newOcc;
                printSuccess("Center information updated successfully.");
            } else {
                printError("Invalid occupancy value. Update aborted.");
            }
        } else {
            printError("Invalid maximum capacity. Update aborted.");
        }
        clearInputBuffer();
        pauseConsole();
    } else if (centerChoice == 4) {
        if (centerCount == 0) {
            printWarning("No evacuation centers registered yet.");
            pauseConsole();
            return;
        }

        int index = -1;
        string deletedCenterName;
        while (true) {
            string searchName = promptNonEmptyStringOrCancel("Enter Facility Name to Delete (or '0' / 'exit' to cancel): ");
            if (searchName.empty()) {
                printInfo("Operation cancelled.");
                return;
            }

            index = recursiveSearchCenter(centers, centerCount, searchName);
            if (index != -1) {
                deletedCenterName = centers[index].facilityName;
                break;
            }
            printError("Evacuation center '" + searchName + "' not found. Please try again (or enter '0' to cancel).");
        }

        printWarning("Found Center: " + deletedCenterName);
        if (promptConfirmation("Are you sure you want to delete this center and remove all evacuees under it?")) {
            // 1. Remove all evacuees assigned to this center
            int removedEvacueesCount = 0;
            int writeIdx = 0;
            for (int i = 0; i < evacueeCount; ++i) {
                if (toUpperCase(evacuees[i].assignedCenter) == toUpperCase(deletedCenterName)) {
                    // Mark linked family as no longer evacuated
                    for (int f = 0; f < familyCount; ++f) {
                        if (families[f].familyCode == evacuees[i].familyCode) {
                            families[f].isEvacuated = false;
                        }
                    }
                    removedEvacueesCount++;
                } else {
                    evacuees[writeIdx++] = evacuees[i];
                }
            }
            evacueeCount = writeIdx;

            // 2. Delete the center from centers list
            for (int i = index; i < centerCount - 1; ++i) {
                centers[i] = centers[i + 1];
            }
            centerCount--;

            // Adjust activeCenterIndex
            if (activeCenterIndex == index) {
                activeCenterIndex = 0;
            } else if (activeCenterIndex > index) {
                activeCenterIndex--;
            }
            if (activeCenterIndex >= centerCount && centerCount > 0) {
                activeCenterIndex = 0;
            }

            printSuccess("Evacuation center '" + deletedCenterName + "' deleted successfully.");
            if (removedEvacueesCount > 0) {
                printInfo("Removed " + to_string(removedEvacueesCount) + " evacuee(s) formerly housed under this center.");
            } else {
                printInfo("No evacuees were registered under this center.");
            }
        } else {
            printInfo("Deletion cancelled.");
        }
        pauseConsole();
    } else if (centerChoice == 5) {
        if (centerCount == 0) {
            printWarning("No evacuation centers registered yet.");
            pauseConsole();
            return;
        }
        cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "--- SELECT ACTIVE FACILITY ---\n" << Color::RESET;
        for (int i = 0; i < centerCount; ++i) {
            cout << "  [" << (i + 1) << "] " << centers[i].facilityName << " (" << centers[i].facilityType << ")"
                 << (i == activeCenterIndex ? (Color::BOLD + Color::BRIGHT_GREEN + " [CURRENT ACTIVE]" + Color::RESET) : "") << "\n";
        }
        cout << "  [0] Cancel / Keep Current\n";
        cout << Color::BOLD << "Choose Facility [1-" << centerCount << ", or 0 to cancel]: " << Color::RESET;
        int sel;
        if (cin >> sel) {
            if (sel == 0) {
                printInfo("Selection cancelled. Keeping current active facility.");
            } else if (sel >= 1 && sel <= centerCount) {
                activeCenterIndex = sel - 1;
                printSuccess("Current active facility set to: " + centers[activeCenterIndex].facilityName);
            } else {
                printError("Invalid selection.");
            }
        } else {
            printError("Invalid input.");
        }
        clearInputBuffer();
        pauseConsole();
    } else {
        printError("Invalid selection.");
        pauseConsole();
    }
}

// ========================================================================================
// CASE 4: PENDING REQUEST ESCALATION
// ========================================================================================
void handlePendingRequestEscalation(ReliefRequest requests[], int& requestCount, int& nextRequestNumber,
                                    const Family families[], int familyCount) {
    clearScreen();
    printSectionHeader("RELIEF REQUEST & ESCALATION QUEUE");
    cout << "  [1] Submit New Relief Request\n";
    cout << "  [2] View Escalation Queue (Sorted by Urgency / Priority)\n";
    cout << "  [3] Update Request Resolution Status\n";
    cout << "  [0] Back to Main Menu\n";
    cout << Color::BOLD << "Enter Choice [0-3]: " << Color::RESET;

    int requestChoice;
    if (!(cin >> requestChoice)) {
        printError("Invalid selection.");
        clearInputBuffer();
        pauseConsole();
        return;
    }
    clearInputBuffer();

    cout << "\n";

    if (requestChoice == 0) {
        printInfo("Returning to main menu.");
        return;
    }

    if (requestChoice == 1) {
        if (requestCount >= MAX_REQUESTS) {
            printError("Request queue is full (" + to_string(MAX_REQUESTS) + ").");
            pauseConsole();
            return;
        }

        if (familyCount == 0) {
            printWarning("No families registered yet. Please register a family under Option [1] first.");
            pauseConsole();
            return;
        }

        string familyCode;
        int familyIndex = -1;
        while (true) {
            familyCode = promptNormalizedCodeOrCancel("Enter Family ID number (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "FAM-", 4);
            if (familyCode.empty()) {
                printInfo("Relief request submission cancelled.");
                return;
            }

            // VERIFY FamilyCode exists (Recursive search)
            familyIndex = recursiveSearchFamily(families, familyCount, familyCode);
            if (familyIndex == -1) {
                printError("Family record [" + familyCode + "] not found. Please try again (or enter '0' to cancel).");
                continue;
            }

            if (!families[familyIndex].isActive) {
                printError("Family record " + colorCode(familyCode) + " is currently DISABLED.");
                printWarning("Cannot submit relief requests for disabled family records. Please re-enable it first or choose another family.");
                continue;
            }

            break;
        }

        cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "--- SELECT RELIEF REQUEST TYPE ---\n" << Color::RESET;
        cout << "  [1] Food Pack / Emergency Rations\n";
        cout << "  [2] Medical Assistance & Supplies\n";
        cout << "  [3] Search, Rescue & Evacuation Transport\n";
        cout << "  [4] Temporary Shelter & Bedding Kit\n";
        cout << "  [5] Hygiene & Sanitation Kit\n";
        cout << "  [6] Potable Drinking Water\n";
        cout << "  [0] Cancel Request\n";

        int typeChoice = promptBoundedIntOrCancel("Choose Request Type", 1, 6, 0, "Please select an option between 1 and 6");
        if (typeChoice == 0) {
            printInfo("Relief request submission cancelled.");
            return;
        }

        string requestType;
        switch (typeChoice) {
            case 1: requestType = "Food Pack"; break;
            case 2: requestType = "Medical Assistance"; break;
            case 3: requestType = "Rescue Transport"; break;
            case 4: requestType = "Shelter Kit"; break;
            case 5: requestType = "Hygiene Kit"; break;
            case 6: requestType = "Drinking Water"; break;
        }

        string requestDescription = promptNonEmptyStringOrCancel("Enter Request Description (or '0' to cancel): ");
        if (requestDescription.empty()) {
            printInfo("Relief request submission cancelled.");
            return;
        }

        bool isEmergency = false;
        if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_RED + "Is this an Emergency?" + Color::RESET, isEmergency)) {
            printInfo("Relief request submission cancelled.");
            return;
        }

        bool hasMitigatingCircumstances = false;
        if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_YELLOW + "Has Mitigating Circumstances?" + Color::RESET, hasMitigatingCircumstances)) {
            printInfo("Relief request submission cancelled.");
            return;
        }

        // Priority calculation logic directly from pseudocode
        int priorityLevel = 4;
        if (isEmergency) {
            priorityLevel = 1;
        } else if (families[familyIndex].vulnerabilityLevel == "HIGH") {
            priorityLevel = 2;
        } else if (hasMitigatingCircumstances) {
            priorityLevel = 2;
        } else if (families[familyIndex].vulnerabilityLevel == "MODERATE") {
            priorityLevel = 3;
        } else {
            priorityLevel = 4;
        }

        string requestId = formatCode("REQ-", nextRequestNumber, 4);

        // Store request
        ReliefRequest newRequest;
        newRequest.requestId = requestId;
        newRequest.familyCode = familyCode;
        newRequest.requestType = requestType;
        newRequest.requestDescription = requestDescription;
        newRequest.isEmergency = isEmergency;
        newRequest.hasMitigatingCircumstances = hasMitigatingCircumstances;
        newRequest.priorityLevel = priorityLevel;
        newRequest.requestStatus = "PENDING";

        requests[requestCount] = newRequest;
        requestCount++;
        nextRequestNumber++;

        cout << "\n";
        printSuccess("Relief request submitted successfully.");
        cout << "  Request ID       : " << colorCode(requestId) << "\n";
        cout << "  Assigned Priority: " << badgePriority(priorityLevel) << "\n";
        cout << "  Status           : " << badgeStatus("PENDING") << "\n";
        pauseConsole();
    } else if (requestChoice == 2) {
        if (requestCount == 0) {
            printWarning("No relief requests currently in queue.");
            pauseConsole();
            return;
        }

        // Recursive sort
        recursiveSortRequestsByPriority(requests, requestCount);

        cout << Color::BOLD << Color::BRIGHT_CYAN << "--- ESCALATION QUEUE (SORTED BY HIGHEST URGENCY) ---\n" << Color::RESET;
        printDivider(95, '=');
        cout << "  " << left << setw(12) << "Request ID"
             << setw(12) << "Family Code"
             << setw(20) << "Request Type"
             << "  " << left << setw(18) << "Priority"
             << " " << left << setw(16) << "Status"
             << " " << "Description\n";
        printDivider(95, '-');

        for (int i = 0; i < requestCount; ++i) {
            displayRecord(requests[i]);
        }
        printDivider(95, '=');
        pauseConsole();
    } else if (requestChoice == 3) {
        if (requestCount == 0) {
            printWarning("No relief requests currently in queue.");
            pauseConsole();
            return;
        }

        int reqIndex = -1;
        string reqId;
        while (true) {
            reqId = promptNormalizedCodeOrCancel("Enter Request ID number to Update (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "REQ-", 4);
            if (reqId.empty()) {
                printInfo("Operation cancelled.");
                return;
            }

            reqIndex = recursiveSearchRequest(requests, requestCount, reqId);
            if (reqIndex != -1) {
                break;
            }
            printError("Request [" + reqId + "] not found. Please try again (or enter '0' to cancel).");
        }

        cout << "\n";
        printInfo("Found Request: [" + requests[reqIndex].requestId + "] for Family " + requests[reqIndex].familyCode);
        cout << "  Current Status: " << badgeStatus(requests[reqIndex].requestStatus) << "\n\n";

            cout << "Select New Status:\n";
            cout << "  [1] PENDING\n";
            cout << "  [2] APPROVED\n";
            cout << "  [3] IN_PROGRESS\n";
            cout << "  [4] RESOLVED\n";
            cout << "  [5] REJECTED\n";
            cout << "  [0] Cancel (Keep Current Status)\n";
            cout << Color::BOLD << "Enter status option [0-5]: " << Color::RESET;

            int statusChoice;
            if (cin >> statusChoice) {
                if (statusChoice == 0) {
                    printInfo("Update cancelled. Status unchanged.");
                    clearInputBuffer();
                    return;
                }
                switch (statusChoice) {
                    case 1: requests[reqIndex].requestStatus = "PENDING"; break;
                    case 2: requests[reqIndex].requestStatus = "APPROVED"; break;
                    case 3: requests[reqIndex].requestStatus = "IN_PROGRESS"; break;
                    case 4: requests[reqIndex].requestStatus = "RESOLVED"; break;
                    case 5: requests[reqIndex].requestStatus = "REJECTED"; break;
                    default:
                        printError("Invalid status choice.");
                        clearInputBuffer();
                        pauseConsole();
                        return;
                }
                printSuccess("Request status updated to " + requests[reqIndex].requestStatus);
            } else {
                printError("Invalid input.");
            }
            clearInputBuffer();
            pauseConsole();
    } else {
        printError("Invalid selection.");
        pauseConsole();
    }
}

// ========================================================================================
// CASE 5: EDIT OR DELETE RECORDS
// ========================================================================================
void handleEditOrDeleteRecords(Family families[], int& familyCount,
                               Evacuee evacuees[], int& evacueeCount) {
    clearScreen();
    printSectionHeader("EDIT OR DELETE RECORDS");
    cout << "  [1] Edit Family Record\n";
    cout << "  [2] Disable / Re-enable Family Record (Soft Delete)\n";
    cout << "  [3] Edit Evacuee Information\n";
    cout << "  [4] Delete Evacuee Record\n";
    cout << "  [0] Back to Main Menu\n";
    cout << Color::BOLD << "Enter Record Choice [0-4]: " << Color::RESET;

    int recordChoice;
    if (!(cin >> recordChoice)) {
        printError("Invalid selection.");
        clearInputBuffer();
        pauseConsole();
        return;
    }
    clearInputBuffer();

    cout << "\n";

    if (recordChoice == 0) {
        printInfo("Returning to main menu.");
        return;
    }

    if (recordChoice == 1) {
        if (familyCount == 0) {
            printWarning("No families registered yet.");
            pauseConsole();
            return;
        }

        int index = -1;
        while (true) {
            string code = promptNormalizedCodeOrCancel("Enter Family ID number to Edit (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "FAM-", 4);
            if (code.empty()) {
                printInfo("Operation cancelled.");
                return;
            }

            index = recursiveSearchFamily(families, familyCount, code);
            if (index != -1) {
                break;
            }
            printError("Family record [" + code + "] not found. Please try again (or enter '0' to cancel).");
        }

        cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "--- CURRENT FAMILY DETAILS ---\n" << Color::RESET;
        displayRecord(families[index], true);

        cout << "\nEnter Updated Head of Family (Press Enter to keep current): ";
        string newHead;
        getline(cin, newHead);
        if (!newHead.empty()) {
            families[index].headOfFamily = newHead;
        }

        cout << "Enter Updated Total Members (Current: " << families[index].numberOfFamilyMembers << "): ";
        int newMembers;
        if (cin >> newMembers && newMembers > 0) {
            families[index].numberOfFamilyMembers = newMembers;
        }
        clearInputBuffer();

        cout << "Do you want to re-assess vulnerability counts? (Y/N): ";
        char reAssess;
        cin >> reAssess;
        clearInputBuffer();

        if (reAssess == 'Y' || reAssess == 'y') {
            VulnerabilityDetails d;
            int currentFamilyMembers = families[index].numberOfFamilyMembers;

            d.children = promptBoundedInt("Children (<12 yrs)", 0, currentFamilyMembers, "Cannot exceed total family members");

            int maxSeniors = currentFamilyMembers - d.children;
            d.seniorCitizens = promptBoundedInt("Senior Citizens (60+ yrs)", 0, maxSeniors, "Remaining non-child family members");

            d.personsWithDisabilities = promptBoundedInt("Persons with Disabilities", 0, currentFamilyMembers, "Cannot exceed total family members");
            d.pregnantMembers = promptBoundedInt("Pregnant Members", 0, currentFamilyMembers, "Cannot exceed total family members");
            d.medicalNeeds = promptBoundedInt("Medical Needs", 0, currentFamilyMembers, "Cannot exceed total family members");

            int vulnTotal = d.children + d.seniorCitizens + d.personsWithDisabilities + d.pregnantMembers + d.medicalNeeds;
            if (vulnTotal > currentFamilyMembers) {
                vulnTotal = currentFamilyMembers;
            }

            families[index].numberOfVulnerableMembers = vulnTotal;
            families[index].details = d;
            families[index].vulnerabilityScore = calculateVulnerabilityScore(d);
            families[index].vulnerabilityLevel = determineVulnerabilityLevel(families[index].vulnerabilityScore);
            printInfo("Vulnerability score recomputed: " + families[index].vulnerabilityLevel);
        }
        printSuccess("Family record updated successfully.");
        pauseConsole();
    } else if (recordChoice == 2) {
        if (familyCount == 0) {
            printWarning("No families registered yet.");
            pauseConsole();
            return;
        }

        int index = -1;
        while (true) {
            string code = promptNormalizedCodeOrCancel("Enter Family ID number to Disable / Re-enable (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "FAM-", 4);
            if (code.empty()) {
                printInfo("Operation cancelled.");
                return;
            }

            index = recursiveSearchFamily(families, familyCount, code);
            if (index != -1) {
                break;
            }
            printError("Family record [" + code + "] not found. Please try again (or enter '0' to cancel).");
        }

        printWarning("Located: " + families[index].familyCode + " (" + families[index].headOfFamily + ")");
        cout << "  Current Status: "
             << (families[index].isActive ? (Color::BOLD + Color::BRIGHT_GREEN + "ACTIVE" + Color::RESET)
                                          : (Color::BOLD + Color::BRIGHT_RED + "DISABLED" + Color::RESET)) << "\n\n";

        if (families[index].isActive) {
            if (promptConfirmation("CONFIRM: Are you sure you want to disable this family record?")) {
                families[index].isActive = false;
                printSuccess("Family record " + colorCode(families[index].familyCode) + " disabled successfully (record preserved in registry).");
            } else {
                printInfo("Operation cancelled.");
            }
        } else {
            if (promptConfirmation("Family record is currently disabled. Do you want to re-enable it?")) {
                families[index].isActive = true;
                printSuccess("Family record " + colorCode(families[index].familyCode) + " re-enabled successfully.");
            } else {
                printInfo("Operation cancelled.");
            }
        }
        pauseConsole();
    } else if (recordChoice == 3) {
        if (evacueeCount == 0) {
            printWarning("No evacuees registered yet.");
            pauseConsole();
            return;
        }

        int index = -1;
        while (true) {
            string id = promptNormalizedCodeOrCancel("Enter Evacuee ID number to Edit (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "EVAC-", 4);
            if (id.empty()) {
                printInfo("Operation cancelled.");
                return;
            }

            index = recursiveSearchEvacuee(evacuees, evacueeCount, id);
            if (index != -1) {
                break;
            }
            printError("Evacuee record [" + id + "] not found. Please try again (or enter '0' to cancel).");
        }

        printInfo("Current Evacuee: " + evacuees[index].fullName + " (" + evacuees[index].assignedCenter + ")");

        cout << "Enter Updated Full Name (Enter to keep): ";
        string newName;
        getline(cin, newName);
        if (!newName.empty()) evacuees[index].fullName = newName;

        cout << "Enter Updated Assigned Center (Enter to keep): ";
        string newCenter;
        getline(cin, newCenter);
        if (!newCenter.empty()) evacuees[index].assignedCenter = newCenter;

        cout << "Enter Updated Special Needs (Enter to keep): ";
        string newNeeds;
        getline(cin, newNeeds);
        if (!newNeeds.empty()) evacuees[index].specialNeeds = newNeeds;

        printSuccess("Evacuee information updated successfully.");
        pauseConsole();
    } else if (recordChoice == 4) {
        if (evacueeCount == 0) {
            printWarning("No evacuees registered yet.");
            pauseConsole();
            return;
        }

        int index = -1;
        while (true) {
            string id = promptNormalizedCodeOrCancel("Enter Evacuee ID number to Delete (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "EVAC-", 4);
            if (id.empty()) {
                printInfo("Operation cancelled.");
                return;
            }

            index = recursiveSearchEvacuee(evacuees, evacueeCount, id);
            if (index != -1) {
                break;
            }
            printError("Evacuee record [" + id + "] not found. Please try again (or enter '0' to cancel).");
        }

        printWarning("Located: " + evacuees[index].evacueeId + " (" + evacuees[index].fullName + ")");
        if (promptConfirmation("CONFIRM: Are you sure you want to delete this evacuee record?")) {
            for (int i = index; i < evacueeCount - 1; ++i) {
                evacuees[i] = evacuees[i + 1];
            }
            evacueeCount--;
            printSuccess("Evacuee record deleted successfully.");
        } else {
            printInfo("Deletion cancelled.");
        }
        pauseConsole();
    } else {
        printError("Invalid selection.");
        pauseConsole();
    }
}

// ========================================================================================
// CASE 6: VIEW POPULATION SUMMARY
// ========================================================================================
void handlePopulationSummary(const Family families[], int familyCount,
                             const EvacuationCenter centers[], int centerCount,
                             const ReliefRequest requests[], int requestCount) {
    clearScreen();
    printSectionHeader("BARANGAY POPULATION & DISASTER SUMMARY");

    int totalFamilies = 0;
    int activeFamilies = 0;
    int disabledFamilies = 0;
    int totalPopulation = 0;
    int totalEvacuees = 0;
    int totalVulnerableMembers = 0;
    int totalAvailableSlots = 0;

    for (int i = 0; i < familyCount; ++i) {
        totalFamilies++;
        if (families[i].isActive) {
            activeFamilies++;
            totalPopulation += families[i].numberOfFamilyMembers;
            totalVulnerableMembers += families[i].numberOfVulnerableMembers;
        } else {
            disabledFamilies++;
        }
    }

    for (int i = 0; i < centerCount; ++i) {
        totalEvacuees += centers[i].currentOccupancy;
        totalAvailableSlots += (centers[i].maximumCapacity - centers[i].currentOccupancy);
    }

    int populationNotCurrentlyInCenters = totalPopulation - totalEvacuees;
    if (populationNotCurrentlyInCenters < 0) {
        populationNotCurrentlyInCenters = 0;
    }

    int pendingRequestsCount = recursiveCountPendingRequests(requests, requestCount);
    int highVulnerabilityFamiliesCount = recursiveCountHighVulnerability(families, familyCount);

    printDivider(65, '=');
    cout << "  " << left << setw(44) << "Total Families Registered"
         << ": " << Color::BOLD << Color::BRIGHT_CYAN << right << setw(6) << totalFamilies << Color::RESET
         << " (" << activeFamilies << " Active, " << disabledFamilies << " Disabled)\n";
    cout << "  " << left << setw(44) << "Total Barangay Population"
         << ": " << Color::BOLD << right << setw(6) << totalPopulation << Color::RESET << "\n";
    cout << "  " << left << setw(44) << "Total Evacuees in Centers"
         << ": " << Color::BOLD << Color::BRIGHT_YELLOW << right << setw(6) << totalEvacuees << Color::RESET << "\n";
    cout << "  " << left << setw(44) << "Total Vulnerable Members"
         << ": " << Color::BOLD << Color::BRIGHT_RED << right << setw(6) << totalVulnerableMembers << Color::RESET << "\n";
    cout << "  " << left << setw(44) << "Population Not In Centers"
         << ": " << Color::BOLD << right << setw(6) << populationNotCurrentlyInCenters << Color::RESET << "\n";
    cout << "  " << left << setw(44) << "Total Available Evacuation Slots"
         << ": " << Color::BOLD << Color::BRIGHT_GREEN << right << setw(6) << totalAvailableSlots << Color::RESET << "\n";
    cout << "  " << left << setw(44) << "Pending Relief Requests (Recursive)"
         << ": " << (pendingRequestsCount > 0 ? Color::BRIGHT_YELLOW : Color::DIM) << right << setw(6) << pendingRequestsCount << Color::RESET << "\n";
    cout << "  " << left << setw(44) << "HIGH Vulnerability Families (Recursive)"
         << ": " << (highVulnerabilityFamiliesCount > 0 ? Color::BRIGHT_RED : Color::DIM) << right << setw(6) << highVulnerabilityFamiliesCount << Color::RESET << "\n";
    printDivider(65, '=');

    // Unit 6 Topic 7: Multi-dimensional array breakdown demonstration
    int demoMatrix[VULN_LEVEL_ROWS][DEMO_COLUMNS];
    generateDemographicMatrix(families, familyCount, demoMatrix);
    displayDemographicMatrix(demoMatrix);
    pauseConsole();
}

// ========================================================================================
// CASE 7: SAVE AND EXIT (File Persistence)
// ========================================================================================
void handleSaveAndExit(const Family families[], int familyCount,
                       const EvacuationCenter centers[], int centerCount,
                       const ReliefRequest requests[], int requestCount,
                       const Evacuee evacuees[], int evacueeCount,
                       int nextFamilyNumber, int nextRequestNumber, int nextEvacueeNumber,
                       int activeCenterIndex) {
    clearScreen();
    saveData(families, familyCount,
             centers, centerCount,
             requests, requestCount,
             evacuees, evacueeCount,
             nextFamilyNumber, nextRequestNumber, nextEvacueeNumber,
             activeCenterIndex);

    cout << "\n";
    printSuccess("SALBAR database state saved successfully to text files.");
    cout << Color::BOLD << Color::BRIGHT_CYAN << "Thank you for using SALBAR. Stay safe!\n" << Color::RESET;
}
