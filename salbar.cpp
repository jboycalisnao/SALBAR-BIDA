#include <iostream>
#include "models.h"
#include "utils.h"
#include "handlers.h"
#include "persistence.h"
#include "ui.h"

using namespace std;

// ========================================================================================
// MAIN FUNCTION (Entry Point & Main Loop: Case Switch Construct)
// ========================================================================================
int main() {
    // Initialize ANSI color support in Windows terminal
    initConsoleUI();

    // Array declarations (Unit 6: Declaring and Initializing Arrays)
    Family familyList[MAX_FAMILIES];
    EvacuationCenter evacuationCenterList[MAX_CENTERS];
    ReliefRequest requestList[MAX_REQUESTS];
    Evacuee evacueeList[MAX_EVACUEES];

    // Counters and tracking state (Unit 1: Variables and Initialization)
    int familyCount = 0;
    int centerCount = 0;
    int requestCount = 0;
    int evacueeCount = 0;

    int nextFamilyNumber = 1;
    int nextRequestNumber = 1;
    int nextEvacueeNumber = 1;
    int activeCenterIndex = 0;

    // Load any existing saved data from disk
    loadData(familyList, familyCount,
             evacuationCenterList, centerCount,
             requestList, requestCount,
             evacueeList, evacueeCount,
             nextFamilyNumber, nextRequestNumber, nextEvacueeNumber,
             activeCenterIndex);

    int choice = 0;

    // REPEAT ... UNTIL Choice = 7 (Unit 4: Do-While Iterative Structure)
    do {
        clearScreen();

        // Construct Current Facility Information for Header Display
        string facilityInfo;
        if (centerCount == 0) {
            facilityInfo = Color::BOLD + Color::BRIGHT_RED + "[NO FACILITY REGISTERED - Add in Option 3]" + Color::RESET;
        } else {
            if (activeCenterIndex < 0 || activeCenterIndex >= centerCount) {
                activeCenterIndex = 0;
            }
            int availableSlots = evacuationCenterList[activeCenterIndex].maximumCapacity - evacuationCenterList[activeCenterIndex].currentOccupancy;
            facilityInfo = Color::BOLD + Color::BRIGHT_GREEN + evacuationCenterList[activeCenterIndex].facilityName + Color::RESET
                         + " (" + evacuationCenterList[activeCenterIndex].facilityType + " | Occupancy: "
                         + to_string(evacuationCenterList[activeCenterIndex].currentOccupancy) + "/"
                         + to_string(evacuationCenterList[activeCenterIndex].maximumCapacity) + " | Available: "
                         + to_string(availableSlots) + ")";
        }

        printHeaderBanner("SALBAR: BARANGAY DISASTER RELIEF", "Emergency Operations, Evacuation & Resource Escalation System", facilityInfo);
        cout << "\n";
        cout << "  " << Color::BOLD << Color::BRIGHT_CYAN << "[1]" << Color::RESET << "  Family Registration\n";
        cout << "  " << Color::BOLD << Color::BRIGHT_CYAN << "[2]" << Color::RESET << "  View Population & Family Registry\n";
        cout << "  " << Color::BOLD << Color::BRIGHT_CYAN << "[3]" << Color::RESET << "  Evacuation Center Management\n";
        cout << "  " << Color::BOLD << Color::BRIGHT_CYAN << "[4]" << Color::RESET << "  Pending Request Escalation Queue\n";
        cout << "  " << Color::BOLD << Color::BRIGHT_CYAN << "[5]" << Color::RESET << "  Edit or Delete Records\n";
        cout << "  " << Color::BOLD << Color::BRIGHT_CYAN << "[6]" << Color::RESET << "  Population Summary & Demographic Matrix\n";
        cout << "  " << Color::BOLD << Color::BRIGHT_RED  << "[7]" << Color::RESET << "  Save Data & Exit\n";
        cout << "\n";
        printDivider(75, '-');
        cout << Color::BOLD << Color::BRIGHT_WHITE << "Enter Choice [1-7]: " << Color::RESET;

        if (!(cin >> choice)) {
            printError("Invalid input. Please enter a valid number.");
            clearInputBuffer();
            choice = 0;
            pauseConsole("Press Enter to continue...");
            continue;
        }
        clearInputBuffer();

        cout << "\n";

        // SWITCH Choice (Unit 3: Switch Statements for Multi-way Selection)
        switch (choice) {
            case 1:
                handleFamilyRegistration(familyList, familyCount, nextFamilyNumber,
                                         evacueeList, evacueeCount, nextEvacueeNumber,
                                         evacuationCenterList, centerCount, activeCenterIndex);
                saveData(familyList, familyCount, evacuationCenterList, centerCount,
                         requestList, requestCount, evacueeList, evacueeCount,
                         nextFamilyNumber, nextRequestNumber, nextEvacueeNumber, activeCenterIndex);
                break;

            case 2:
                handleViewPopulation(familyList, familyCount);
                break;

            case 3:
                handleEvacuationCenterManagement(evacuationCenterList, centerCount, activeCenterIndex,
                                                 evacueeList, evacueeCount,
                                                 familyList, familyCount);
                saveData(familyList, familyCount, evacuationCenterList, centerCount,
                         requestList, requestCount, evacueeList, evacueeCount,
                         nextFamilyNumber, nextRequestNumber, nextEvacueeNumber, activeCenterIndex);
                break;

            case 4:
                handlePendingRequestEscalation(requestList, requestCount, nextRequestNumber,
                                               familyList, familyCount);
                saveData(familyList, familyCount, evacuationCenterList, centerCount,
                         requestList, requestCount, evacueeList, evacueeCount,
                         nextFamilyNumber, nextRequestNumber, nextEvacueeNumber, activeCenterIndex);
                break;

            case 5:
                handleEditOrDeleteRecords(familyList, familyCount, evacueeList, evacueeCount);
                saveData(familyList, familyCount, evacuationCenterList, centerCount,
                         requestList, requestCount, evacueeList, evacueeCount,
                         nextFamilyNumber, nextRequestNumber, nextEvacueeNumber, activeCenterIndex);
                break;

            case 6:
                handlePopulationSummary(familyList, familyCount, evacuationCenterList,
                                        centerCount, requestList, requestCount);
                break;

            case 7:
                handleSaveAndExit(familyList, familyCount,
                                  evacuationCenterList, centerCount,
                                  requestList, requestCount,
                                  evacueeList, evacueeCount,
                                  nextFamilyNumber, nextRequestNumber, nextEvacueeNumber,
                                  activeCenterIndex);
                break;

            default:
                printError("Invalid choice. Please select an option between 1 and 7.");
                pauseConsole("Press Enter to continue...");
                break;
        }

        cout << "\n";
    } while (choice != 7);

    return 0;
}
