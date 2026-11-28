#include "analytics.h"
#include "operations.h"
#include "ui.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ========================================================================================
// VULNERABILITY ASSESSMENT LOGIC (Unit 1 & 2: Expressions, Arithmetic, Logic)
// ========================================================================================

/*
   Vulnerability Assessment Weight Rules:
   - Children (< 12 yrs): 1 point each
   - Senior Citizens (60+ yrs): 2 points each
   - Persons with Disabilities (PWD): 3 points each
   - Pregnant / Lactating Mothers: 3 points each
   - Members with Chronic / Urgent Medical Needs: 3 points each
*/
int calculateVulnerabilityScore(const VulnerabilityDetails& details) {
    int score = (details.children * 1) +
                (details.seniorCitizens * 2) +
                (details.personsWithDisabilities * 3) +
                (details.pregnantMembers * 3) +
                (details.medicalNeeds * 3);
    return score;
}

string determineVulnerabilityLevel(int score) {
    if (score >= 8) {
        return "HIGH";
    } else if (score >= 4) {
        return "MODERATE";
    } else {
        return "LOW";
    }
}

void displayVulnerabilityScoreBreakdown(const VulnerabilityDetails& details, int totalScore) {
    int childPts    = details.children * 1;
    int seniorPts   = details.seniorCitizens * 2;
    int pwdPts      = details.personsWithDisabilities * 3;
    int pregnantPts = details.pregnantMembers * 3;
    int medicalPts  = details.medicalNeeds * 3;

    cout << "\n  " << Color::BOLD << Color::BRIGHT_CYAN << "--- Vulnerability Score Breakdown ---" << Color::RESET << "\n";
    cout << "  " << left << setw(27) << "- Children (<12 yrs)"      << ": " << right << setw(3) << details.children                << " x 1 pt  = " << Color::BOLD << setw(3) << childPts    << Color::RESET << "\n";
    cout << "  " << left << setw(27) << "- Senior Citizens (60+)"   << ": " << right << setw(3) << details.seniorCitizens          << " x 2 pts = " << Color::BOLD << setw(3) << seniorPts   << Color::RESET << "\n";
    cout << "  " << left << setw(27) << "- Persons with Disability" << ": " << right << setw(3) << details.personsWithDisabilities << " x 3 pts = " << Color::BOLD << setw(3) << pwdPts      << Color::RESET << "\n";
    cout << "  " << left << setw(27) << "- Pregnant Members"        << ": " << right << setw(3) << details.pregnantMembers         << " x 3 pts = " << Color::BOLD << setw(3) << pregnantPts << Color::RESET << "\n";
    cout << "  " << left << setw(27) << "- Medical Needs"           << ": " << right << setw(3) << details.medicalNeeds            << " x 3 pts = " << Color::BOLD << setw(3) << medicalPts  << Color::RESET << "\n";
    cout << "  " << Color::DIM << "-------------------------------------------" << Color::RESET << "\n";
    cout << "  " << left << setw(27) << "Total Computed Score"      << ": " << Color::BOLD << Color::BRIGHT_WHITE << totalScore << Color::RESET
         << Color::DIM << "  (>=8 HIGH | >=4 MODERATE | <4 LOW)" << Color::RESET << "\n";
}

// ========================================================================================
// MULTI-DIMENSIONAL ARRAY DEMOGRAPHICS (Unit 6: Multi-dimensional Arrays)
// ========================================================================================
void generateDemographicMatrix(const Family families[], int familyCount, int matrix[VULN_LEVEL_ROWS][DEMO_COLUMNS]) {
    // Initialize matrix with zeroes
    for (int r = 0; r < VULN_LEVEL_ROWS; ++r) {
        for (int c = 0; c < DEMO_COLUMNS; ++c) {
            matrix[r][c] = 0;
        }
    }

    // Accumulate demographic counts into 2D grid based on vulnerability level
    for (int i = 0; i < familyCount; ++i) {
        if (!families[i].isActive) {
            continue;
        }

        int rowIndex = 0;
        if (families[i].vulnerabilityLevel == "MODERATE") {
            rowIndex = 1;
        } else if (families[i].vulnerabilityLevel == "HIGH") {
            rowIndex = 2;
        }

        matrix[rowIndex][0] += families[i].details.children;
        matrix[rowIndex][1] += families[i].details.seniorCitizens;
        matrix[rowIndex][2] += families[i].details.personsWithDisabilities;
        matrix[rowIndex][3] += families[i].details.pregnantMembers;
        matrix[rowIndex][4] += families[i].details.medicalNeeds;
    }
}

void displayDemographicMatrix(const int matrix[VULN_LEVEL_ROWS][DEMO_COLUMNS]) {
    cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "DEMOGRAPHIC BREAKDOWN MATRIX (Vulnerability Level vs. Vulnerable Groups)\n" << Color::RESET;
    printDivider(82, '=');
    cout << left << setw(14) << "Level"
         << right << setw(12) << "Children"
         << setw(12) << "Seniors"
         << setw(12) << "PWD"
         << setw(14) << "Pregnant"
         << setw(16) << "Medical Needs\n";
    printDivider(82, '-');

    const string levelNames[VULN_LEVEL_ROWS] = {"LOW", "MODERATE", "HIGH"};
    for (int r = 0; r < VULN_LEVEL_ROWS; ++r) {
        if (r == 0) cout << Color::BRIGHT_GREEN;
        else if (r == 1) cout << Color::BRIGHT_YELLOW;
        else cout << Color::BRIGHT_RED;

        cout << left << setw(14) << levelNames[r] << Color::RESET;

        for (int c = 0; c < DEMO_COLUMNS; ++c) {
            cout << right << setw(12 + (c >= 3 ? 2 : 0)) << matrix[r][c];
        }
        cout << "\n";
    }
    printDivider(82, '=');
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
