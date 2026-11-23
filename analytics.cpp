#include "analytics.h"
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
