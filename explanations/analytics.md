# `analytics.h` & `analytics.cpp` — Vulnerability Scoring & Case 6 Summary Report

## Plain-English Purpose
This module is the **statistician** of SALBAR. First, it grades how vulnerable a family is based on who is in the household (children, seniors, PWDs, pregnant mothers, medical needs). Second, it powers **Case 6 (Population Summary & Demographic Matrix)**, giving barangay officials an instant big-picture report.

---

## Naming Conventions
- **`calculate*` / `determine*`**: Math and classification helpers that take data in and return a score or label.
- **`generate*` / `display*`**: Two-step pattern for the 2D array — `generateDemographicMatrix` fills the grid with numbers, and `displayDemographicMatrix` prints it nicely.
- **`handlePopulationSummary`**: `handle*` prefix marks it as a top-level menu screen called from `salbar.cpp`.

---

## Actual Code & Explanation

### 1. Vulnerability Scoring & Classification
Weights each vulnerable group (Children `×1`, Seniors `×2`, PWD/Pregnant/Medical `×3`) and classifies the total score:

```cpp
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
```

### 2. 2D Demographic Matrix (`generateDemographicMatrix`)
Initializes a `3 × 5` grid to zeroes, skips disabled (`!isActive`) families, and totals each vulnerable group by risk level (`0 = LOW`, `1 = MODERATE`, `2 = HIGH`):

```cpp
void generateDemographicMatrix(const Family families[], int familyCount, int matrix[VULN_LEVEL_ROWS][DEMO_COLUMNS]) {
    for (int r = 0; r < VULN_LEVEL_ROWS; ++r) {
        for (int c = 0; c < DEMO_COLUMNS; ++c) {
            matrix[r][c] = 0;
        }
    }

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
```

### 3. Case 6: `handlePopulationSummary(...)`
Clears the screen, counts active/disabled families, population, center occupancy, available slots, and recursive alert totals, then displays the 2D demographic matrix and pauses:

```cpp
void handlePopulationSummary(const Family families[], int familyCount,
                             const EvacuationCenter centers[], int centerCount,
                             const ReliefRequest requests[], int requestCount) {
    clearScreen();
    printSectionHeader("BARANGAY POPULATION & DISASTER SUMMARY");

    int totalFamilies = 0, activeFamilies = 0, disabledFamilies = 0;
    int totalPopulation = 0, totalEvacuees = 0, totalVulnerableMembers = 0, totalAvailableSlots = 0;

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
    if (populationNotCurrentlyInCenters < 0) populationNotCurrentlyInCenters = 0;

    int pendingRequestsCount = recursiveCountPendingRequests(requests, requestCount);
    int highVulnerabilityFamiliesCount = recursiveCountHighVulnerability(families, familyCount);
    ...
    int demoMatrix[VULN_LEVEL_ROWS][DEMO_COLUMNS];
    generateDemographicMatrix(families, familyCount, demoMatrix);
    displayDemographicMatrix(demoMatrix);
    pauseConsole();
}
```
