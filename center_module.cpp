#include "center_module.h"
#include "utils.h"
#include "operations.h"
#include "ui.h"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

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
