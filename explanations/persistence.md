# `persistence.h` & `persistence.cpp` — Saving, Loading & Case 7 Exit

## Plain-English Purpose
Without `persistence`, everything you type would vanish the moment `salbar.exe` closes. This module is the **filing clerk** that writes all arrays to plain `.txt` files inside the `data/` folder (`data/families.txt`, `data/centers.txt`, `data/requests.txt`, `data/evacuees.txt`, `data/config.txt`) and reads them back when the program starts. It also houses **Case 7 (`handleSaveAndExit`)**.

---

## Naming Conventions
- **`*Out` Streams** (`cfgOut`, `famOut`, `centOut`, `reqOut`, `evacOut`): `ofstream` (output file stream) objects used for **writing** to disk.
- **`*In` Streams** (`cfgIn`, `famIn`, `centIn`, `reqIn`, `evacIn`): `ifstream` (input file stream) objects used for **reading** from disk.
- **Const Arrays in `saveData`**: Marked `const` so saving can never accidentally modify the data in memory; **Reference (`&`) Counters in `loadData`**: Passed by reference so loading updates the counters in `main()`.

---

## Actual Code & Explanation

### 1. Saving Config & Families (`saveData`)
Writes the next ID counters and `activeCenterIndex` to `config.txt`, then writes `familyCount` followed by every `Family` field one line at a time (including `isEvacuated` and `isActive`):

```cpp
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
```
*(Centers, requests, and evacuees are saved using the exact same `count` + loop pattern into `centers.txt`, `requests.txt`, and `evacuees.txt`.)*

### 2. Fail-Safe Loading (`loadData`)
Reads the files back in the exact same order. Notice the `if (!(stream >> count))` guards — if a `.txt` file is empty or cleared manually, the count safely resets to `0` instead of crashing or reading garbage:

```cpp
    // 1. Load Config IDs and Active Center Index
    ifstream cfgIn(FILE_CONFIG.c_str());
    if (cfgIn.is_open()) {
        cfgIn >> nextFamilyNumber >> nextRequestNumber >> nextEvacueeNumber;
        if (!(cfgIn >> activeCenterIndex)) {
            activeCenterIndex = 0;
        }
        cfgIn.close();
    }

    // 5. Load EvacueeList (same fail-safe pattern used for Families, Centers, Requests)
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
```

### 3. Case 7: `handleSaveAndExit(...)` (Exit Confirmation & Data Reset)
Instead of closing immediately, selecting Option `[7]` opens a confirmation menu with `[1] Confirm Save & Exit System`, `[2] Clear All System Data (Reset Database)`, and `[0] Cancel / Return to Main Menu`. Returning `false` keeps the main loop running; returning `true` exits the program. Choosing `[2]` opens its own dedicated confirmation page before resetting all counters and saving empty `.txt` files:

```cpp
bool handleSaveAndExit(Family families[], int& familyCount,
                       EvacuationCenter centers[], int& centerCount,
                       ReliefRequest requests[], int& requestCount,
                       Evacuee evacuees[], int& evacueeCount,
                       int& nextFamilyNumber, int& nextRequestNumber, int& nextEvacueeNumber,
                       int& activeCenterIndex) {
    clearScreen();
    printSectionHeader("SYSTEM EXIT & DATA MANAGEMENT");
    cout << "  [1]  Confirm Save & Exit System\n";
    cout << "  [2]  Clear All System Data (Reset Database)\n";
    cout << "  [0]  Cancel / Return to Main Menu\n\n";

    int exitChoice = promptBoundedIntOrCancel("Enter Choice (or '0' to cancel)", 1, 2, 0, "Please select 1, 2, or 0");
    if (exitChoice == 0) return false;
    if (exitChoice == 1) {
        saveData(...);
        return true;
    }

    // Dedicated Confirmation Page for Clearing All System Data
    clearScreen();
    printSectionHeader("CLEAR ALL SYSTEM DATA (CONFIRMATION)");
    bool confirmClear = false;
    if (!promptYesNoOrCancel("Are you SURE you want to permanently clear all system data?", confirmClear) || !confirmClear) {
        return false;
    }

    familyCount = centerCount = requestCount = evacueeCount = 0;
    nextFamilyNumber = nextRequestNumber = nextEvacueeNumber = 1;
    activeCenterIndex = 0;
    saveData(...);
    return false;
}
```
