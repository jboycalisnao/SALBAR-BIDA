#include "persistence.h"
#include "utils.h"
#include "ui.h"
#include <fstream>
#include <iostream>
#include <string>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

using namespace std;

// ========================================================================================
// DATA PERSISTENCE IMPLEMENTATION (<fstream>)
// ========================================================================================

void saveData(const Family families[], int familyCount,
              const EvacuationCenter centers[], int centerCount,
              const ReliefRequest requests[], int requestCount,
              const Evacuee evacuees[], int evacueeCount,
              int nextFamilyNumber, int nextRequestNumber, int nextEvacueeNumber,
              int activeCenterIndex) {
    // Ensure data directory exists before writing files
#ifdef _WIN32
    _mkdir(DATA_DIR.c_str());
#else
    mkdir(DATA_DIR.c_str(), 0755);
#endif

    // 1. Save Config IDs and Active Center Index
    ofstream cfgOut(FILE_CONFIG.c_str());
    if (cfgOut.is_open()) {
        cfgOut << nextFamilyNumber << " " << nextRequestNumber << " " << nextEvacueeNumber << " " << activeCenterIndex << "\n";
        cfgOut.close();
    }

    // 2. Save FamilyList
    ofstream famOut(FILE_FAMILIES.c_str());
    if (famOut.is_open()) {
        famOut << familyCount << "\n";
        for (int i = 0; i < familyCount; ++i) {
            famOut << families[i].familyCode << "\n"
                   << families[i].headOfFamily << "\n"
                   << families[i].numberOfFamilyMembers << "\n"
                   << families[i].numberOfVulnerableMembers << "\n"
                   << families[i].vulnerabilityScore << "\n"
                   << families[i].vulnerabilityLevel << "\n"
                   << families[i].details.children << "\n"
                   << families[i].details.seniorCitizens << "\n"
                   << families[i].details.personsWithDisabilities << "\n"
                   << families[i].details.pregnantMembers << "\n"
                   << families[i].details.medicalNeeds << "\n"
                   << families[i].isEvacuated << "\n"
                   << families[i].isActive << "\n";
        }
        famOut.close();
    }

    // 3. Save EvacuationCenterList
    ofstream centOut(FILE_CENTERS.c_str());
    if (centOut.is_open()) {
        centOut << centerCount << "\n";
        for (int i = 0; i < centerCount; ++i) {
            centOut << centers[i].facilityName << "\n"
                    << centers[i].facilityType << "\n"
                    << centers[i].maximumCapacity << "\n"
                    << centers[i].currentOccupancy << "\n";
        }
        centOut.close();
    }

    // 4. Save RequestList
    ofstream reqOut(FILE_REQUESTS.c_str());
    if (reqOut.is_open()) {
        reqOut << requestCount << "\n";
        for (int i = 0; i < requestCount; ++i) {
            reqOut << requests[i].requestId << "\n"
                   << requests[i].familyCode << "\n"
                   << requests[i].requestType << "\n"
                   << requests[i].requestDescription << "\n"
                   << requests[i].isEmergency << "\n"
                   << requests[i].hasMitigatingCircumstances << "\n"
                   << requests[i].priorityLevel << "\n"
                   << requests[i].requestStatus << "\n";
        }
        reqOut.close();
    }

    // 5. Save EvacueeList
    ofstream evacOut(FILE_EVACUEES.c_str());
    if (evacOut.is_open()) {
        evacOut << evacueeCount << "\n";
        for (int i = 0; i < evacueeCount; ++i) {
            evacOut << evacuees[i].evacueeId << "\n"
                    << evacuees[i].familyCode << "\n"
                    << evacuees[i].fullName << "\n"
                    << evacuees[i].age << "\n"
                    << evacuees[i].assignedCenter << "\n"
                    << evacuees[i].specialNeeds << "\n";
        }
        evacOut.close();
    }
}

void loadData(Family families[], int& familyCount,
              EvacuationCenter centers[], int& centerCount,
              ReliefRequest requests[], int& requestCount,
              Evacuee evacuees[], int& evacueeCount,
              int& nextFamilyNumber, int& nextRequestNumber, int& nextEvacueeNumber,
              int& activeCenterIndex) {
    // 1. Load Config IDs and Active Center Index
    ifstream cfgIn(FILE_CONFIG.c_str());
    if (cfgIn.is_open()) {
        cfgIn >> nextFamilyNumber >> nextRequestNumber >> nextEvacueeNumber;
        if (!(cfgIn >> activeCenterIndex)) {
            activeCenterIndex = 0;
        }
        cfgIn.close();
    }

    // 2. Load FamilyList
    ifstream famIn(FILE_FAMILIES.c_str());
    if (famIn.is_open()) {
        if (!(famIn >> familyCount)) {
            familyCount = 0;
        }
        famIn.ignore();
        for (int i = 0; i < familyCount && i < MAX_FAMILIES; ++i) {
            getline(famIn, families[i].familyCode);
            getline(famIn, families[i].headOfFamily);
            famIn >> families[i].numberOfFamilyMembers;
            famIn >> families[i].numberOfVulnerableMembers;
            famIn >> families[i].vulnerabilityScore;
            famIn.ignore();
            getline(famIn, families[i].vulnerabilityLevel);
            famIn >> families[i].details.children;
            famIn >> families[i].details.seniorCitizens;
            famIn >> families[i].details.personsWithDisabilities;
            famIn >> families[i].details.pregnantMembers;
            famIn >> families[i].details.medicalNeeds;
            famIn >> families[i].isEvacuated;
            if (!(famIn >> families[i].isActive)) {
                families[i].isActive = true;
            }
            famIn.ignore();
        }
        famIn.close();
    }

    // 3. Load EvacuationCenterList
    ifstream centIn(FILE_CENTERS.c_str());
    if (centIn.is_open()) {
        if (!(centIn >> centerCount)) {
            centerCount = 0;
        }
        centIn.ignore();
        for (int i = 0; i < centerCount && i < MAX_CENTERS; ++i) {
            getline(centIn, centers[i].facilityName);
            getline(centIn, centers[i].facilityType);
            centIn >> centers[i].maximumCapacity;
            centIn >> centers[i].currentOccupancy;
            centIn.ignore();
        }
        centIn.close();
    }

    // 4. Load RequestList
    ifstream reqIn(FILE_REQUESTS.c_str());
    if (reqIn.is_open()) {
        if (!(reqIn >> requestCount)) {
            requestCount = 0;
        }
        reqIn.ignore();
        for (int i = 0; i < requestCount && i < MAX_REQUESTS; ++i) {
            getline(reqIn, requests[i].requestId);
            getline(reqIn, requests[i].familyCode);
            getline(reqIn, requests[i].requestType);
            getline(reqIn, requests[i].requestDescription);
            reqIn >> requests[i].isEmergency;
            reqIn >> requests[i].hasMitigatingCircumstances;
            reqIn >> requests[i].priorityLevel;
            reqIn.ignore();
            getline(reqIn, requests[i].requestStatus);
        }
        reqIn.close();
    }

    // 5. Load EvacueeList
    ifstream evacIn(FILE_EVACUEES.c_str());
    if (evacIn.is_open()) {
        if (!(evacIn >> evacueeCount)) {
            evacueeCount = 0;
        }
        evacIn.ignore();
        for (int i = 0; i < evacueeCount && i < MAX_EVACUEES; ++i) {
            getline(evacIn, evacuees[i].evacueeId);
            getline(evacIn, evacuees[i].familyCode);
            getline(evacIn, evacuees[i].fullName);
            evacIn >> evacuees[i].age;
            evacIn.ignore();
            getline(evacIn, evacuees[i].assignedCenter);
            getline(evacIn, evacuees[i].specialNeeds);
        }
        evacIn.close();
    }
}

// ========================================================================================
// CASE 7: SYSTEM EXIT & DATA MANAGEMENT (Confirm Exit / Clear System Data)
// ========================================================================================
bool handleSaveAndExit(Family families[], int& familyCount,
                       EvacuationCenter centers[], int& centerCount,
                       ReliefRequest requests[], int& requestCount,
                       Evacuee evacuees[], int& evacueeCount,
                       int& nextFamilyNumber, int& nextRequestNumber, int& nextEvacueeNumber,
                       int& activeCenterIndex) {
    clearScreen();
    printSectionHeader("SYSTEM EXIT & DATA MANAGEMENT");
    cout << "  " << Color::BOLD << Color::BRIGHT_GREEN << "[1]" << Color::RESET << "  Confirm Save & Exit System\n";
    cout << "  " << Color::BOLD << Color::BRIGHT_RED   << "[2]" << Color::RESET << "  Clear All System Data (Reset Database)\n";
    cout << "  " << Color::BOLD << Color::BRIGHT_CYAN  << "[0]" << Color::RESET << "  Cancel / Return to Main Menu\n\n";

    int exitChoice = promptBoundedIntOrCancel("Enter Choice (or '0' to cancel)", 1, 2, 0, "Please select 1, 2, or 0");

    if (exitChoice == 0) {
        printInfo("Exit cancelled. Returning to main menu.");
        return false;
    }

    if (exitChoice == 1) {
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
        return true;
    }

    // Option 2: Dedicated Confirmation Page for Clearing All System Data
    clearScreen();
    printSectionHeader("CLEAR ALL SYSTEM DATA (CONFIRMATION)");
    printWarning("This action will permanently erase ALL stored records from memory and disk:");
    cout << "  - Families Registered   : " << Color::BOLD << familyCount << Color::RESET << "\n";
    cout << "  - Evacuation Centers    : " << Color::BOLD << centerCount << Color::RESET << "\n";
    cout << "  - Relief Requests       : " << Color::BOLD << requestCount << Color::RESET << "\n";
    cout << "  - Individual Evacuees   : " << Color::BOLD << evacueeCount << Color::RESET << "\n\n";

    bool confirmClear = false;
    if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_RED + "Are you SURE you want to permanently clear all system data?" + Color::RESET, confirmClear) || !confirmClear) {
        printInfo("Data clear cancelled. No records were deleted.");
        pauseConsole();
        return false;
    }

    // Reset all counts, IDs, and active center index
    familyCount = 0;
    centerCount = 0;
    requestCount = 0;
    evacueeCount = 0;
    nextFamilyNumber = 1;
    nextRequestNumber = 1;
    nextEvacueeNumber = 1;
    activeCenterIndex = 0;

    saveData(families, familyCount,
             centers, centerCount,
             requests, requestCount,
             evacuees, evacueeCount,
             nextFamilyNumber, nextRequestNumber, nextEvacueeNumber,
             activeCenterIndex);

    cout << "\n";
    printSuccess("All system data has been permanently cleared and reset.");
    pauseConsole();
    return false;
}
