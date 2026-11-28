#include "family_module.h"
#include "utils.h"
#include "analytics.h"
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
    displayVulnerabilityScoreBreakdown(details, vulnerabilityScore);

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
