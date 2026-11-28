# `family_module.h` & `family_module.cpp` — Case 1 & Case 2 (Family Registration & View)

## Plain-English Purpose
This module handles the two core household screens:
- **Case 1 (`handleFamilyRegistration`)**: Signing up a new family into the currently active evacuation center, checking capacity, computing their vulnerability score, and updating occupancy.
- **Case 2 (`handleViewPopulation`)**: Showing a clean, aligned table of all **active** registered families.

---

## Naming Conventions
- **`handle*`**: Top-level menu handler functions called by `salbar.cpp`.
- **`availableSlots`**: Computed locally as `maximumCapacity - currentOccupancy` so you can't overfill a shelter.
- **`vulnerableCount`**: Automatically computed sum of the 5 vulnerable categories (capped at `numberOfFamilyMembers` in case one person belongs to multiple categories).

---

## Actual Code & Explanation

### Case 1: Pre-Registration Guards & Household Inputs
Blocks registration if no center exists yet (`centerCount == 0`) or if the active center is full (`availableSlots <= 0`), then asks for non-empty Head of Family name and member count:

```cpp
    clearScreen();
    printSectionHeader("FAMILY REGISTRATION");

    if (centerCount == 0) {
        printError("Registration blocked! No evacuation facility is registered yet.");
        printWarning("Barangay disaster guidelines require registering an Evacuation Center first.");
        printInfo("Please select Option [3] (Evacuation Center Management) to register a facility.");
        pauseConsole();
        return;
    }

    int availableSlots = centers[activeCenterIndex].maximumCapacity - centers[activeCenterIndex].currentOccupancy;
    if (availableSlots <= 0) {
        printError("Facility '" + centers[activeCenterIndex].facilityName + "' is currently at full capacity!");
        pauseConsole();
        return;
    }

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

    if (numberOfFamilyMembers > availableSlots) {
        printError("Registration blocked! Insufficient capacity in '" + centers[activeCenterIndex].facilityName + "'.");
        pauseConsole();
        return;
    }
```

### Case 1: Auto-Scoring, Occupancy Update & Evacuee Linking
Computes `vulnerableCount` automatically from the 5 demographic inputs, scores vulnerability, adds the family to `families[]`, increases `currentOccupancy`, and registers the head in `evacuees[]`:

```cpp
    int vulnerableCount = details.children + details.seniorCitizens + details.personsWithDisabilities + details.pregnantMembers + details.medicalNeeds;
    if (vulnerableCount > numberOfFamilyMembers) {
        vulnerableCount = numberOfFamilyMembers;
    }

    int vulnerabilityScore = calculateVulnerabilityScore(details);
    string vulnerabilityLevel = determineVulnerabilityLevel(vulnerabilityScore);

    // Store family record
    newFamily.isEvacuated = true;
    newFamily.isActive = true;
    families[familyCount] = newFamily;
    familyCount++;

    // Update facility occupancy with newly registered family members
    centers[activeCenterIndex].currentOccupancy += numberOfFamilyMembers;

    // Evacuee registration linking for the head of family
    if (evacueeCount < MAX_EVACUEES) {
        Evacuee headEvacuee;
        headEvacuee.evacueeId = formatCode("EVAC-", nextEvacueeNumber, 4);
        headEvacuee.familyCode = familyCode;
        headEvacuee.fullName = headOfFamily;
        headEvacuee.age = 35;
        headEvacuee.assignedCenter = centers[activeCenterIndex].facilityName;
        headEvacuee.specialNeeds = (details.medicalNeeds > 0) ? "Medical Attention" : "None";
        evacuees[evacueeCount++] = headEvacuee;
        nextEvacueeNumber++;
    }
    nextFamilyNumber++;
    pauseConsole();
```

### Case 2: `handleViewPopulation(...)`
Skips disabled (`!isActive`) families and uses manual badge padding (`16 - vulnBadge.length()`) so ANSI color codes don't misalign the `Status` column:

```cpp
    for (int i = 0; i < familyCount; ++i) {
        if (!families[i].isActive) {
            continue; // Do not show disabled records in the family list
        }

        cout << Color::BOLD << Color::BRIGHT_CYAN << left << setw(14) << families[i].familyCode << Color::RESET;
        cout << left << setw(26) << families[i].headOfFamily;
        cout << right << setw(10) << families[i].numberOfFamilyMembers << "   ";

        string vulnBadge = "[" + families[i].vulnerabilityLevel + "]";
        int vulnPadLen = 16 - static_cast<int>(vulnBadge.length());
        if (vulnPadLen < 1) vulnPadLen = 1;
        cout << badgeVulnerability(families[i].vulnerabilityLevel) << string(vulnPadLen, ' ');

        cout << Color::BOLD << Color::BRIGHT_GREEN << "ACTIVE" << Color::RESET << "\n";
    }
```
