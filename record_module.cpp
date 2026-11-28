#include "record_module.h"
#include "utils.h"
#include "analytics.h"
#include "operations.h"
#include "ui.h"

#include <iostream>
#include <string>

using namespace std;

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
