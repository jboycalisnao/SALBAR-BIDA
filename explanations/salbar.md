# `salbar.cpp` — The Main Program Loop (Front Desk)

## Plain-English Purpose
`salbar.cpp` is the **entry point** (`main()`) — the "front desk" of the application. When you double-click `salbar.exe`, this is the file that wakes up, loads your saved `.txt` files from the hard drive, shows you the top banner with your current evacuation center, and keeps showing the 7-option main menu until you choose `[7] Save Data & Exit`.

---

## Naming Conventions
- **`*List` Arrays**: `familyList`, `evacuationCenterList`, `requestList`, `evacueeList` hold all records in RAM while the app is open.
- **`*Count` Counters**: `familyCount`, `centerCount`, `requestCount`, `evacueeCount` track how many slots in each array are currently used.
- **`next*Number` Counters**: `nextFamilyNumber`, `nextRequestNumber`, `nextEvacueeNumber` ensure IDs (`FAM-0001`, `REQ-0001`, `EVAC-0001`) keep counting upward and never repeat even if a record is deleted.
- **`activeCenterIndex`**: Remembers which evacuation center is currently selected as the default facility.

---

## Actual Code & Explanation

### 1. Startup, Array Initialization & Loading Saved Data
At startup, the console enables ANSI colors, sets up the 4 arrays and counters, and loads saved data from disk:

```cpp
    initConsoleUI();

    Family familyList[MAX_FAMILIES];
    EvacuationCenter evacuationCenterList[MAX_CENTERS];
    ReliefRequest requestList[MAX_REQUESTS];
    Evacuee evacueeList[MAX_EVACUEES];

    int familyCount = 0;
    int centerCount = 0;
    int requestCount = 0;
    int evacueeCount = 0;

    int nextFamilyNumber = 1;
    int nextRequestNumber = 1;
    int nextEvacueeNumber = 1;
    int activeCenterIndex = 0;

    loadData(familyList, familyCount,
             evacuationCenterList, centerCount,
             requestList, requestCount,
             evacueeList, evacueeCount,
             nextFamilyNumber, nextRequestNumber, nextEvacueeNumber,
             activeCenterIndex);
```

### 2. Main Menu Loop & Dynamic Facility Header
Every time the menu loops, `clearScreen()` pushes the menu to the top of the terminal, and `facilityInfo` dynamically shows either a warning or the active center's live occupancy:

```cpp
    do {
        clearScreen();

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
```

### 3. Input Validation & Routing (`switch (choice)`) with Auto-Save
If a user types invalid letters, it catches the error and pauses. For Cases 1, 3, 4, and 5 (any case that modifies data), `saveData(...)` runs immediately afterward so nothing is lost if the console window is closed:

```cpp
        if (!(cin >> choice)) {
            printError("Invalid input. Please enter a valid number.");
            clearInputBuffer();
            choice = 0;
            pauseConsole("Press Enter to continue...");
            continue;
        }
        clearInputBuffer();

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
    } while (choice != 7);
```
