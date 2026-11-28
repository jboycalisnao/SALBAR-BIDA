# `record_module.h` & `record_module.cpp` — Case 5 (Edit, Disable & Delete Records)

## Plain-English Purpose
People make typos, family sizes change, or a family moves back home. `record_module` is the **records maintenance desk** where you can edit family/evacuee info, disable/re-enable a family (soft delete instead of erasing history), or permanently remove an individual evacuee record.

---

## Naming Conventions
- **`recordChoice`**: Submenu selection (`0–4`).
- **Soft Delete (`isActive`)**: Instead of removing a `Family` from the array, `isActive` is flipped between `true` (`ACTIVE`) and `false` (`DISABLED`).
- **`newHead`, `newMembers`, `newName`, `newCenter`, `newNeeds`**: Temporary variables holding updated values; pressing Enter keeps the existing value.

---

## Actual Code & Explanation

### 1. Retry Loop Pattern Used Across All 4 Sub-Options
Every lookup in Case 5 uses `promptNormalizedCodeOrCancel` (so typing `1` searches for `FAM-0001` or `EVAC-0001`) inside a `while (true)` loop so a typo lets you retry immediately:

```cpp
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
```

### 2. Sub-Option `[1]`: Editing a Family & Recomputing Vulnerability
Updates head name/member count, and if the user answers `Y` to re-assess vulnerability, recalculates `vulnerabilityScore` and `vulnerabilityLevel`:

```cpp
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
            if (vulnTotal > currentFamilyMembers) vulnTotal = currentFamilyMembers;

            families[index].numberOfVulnerableMembers = vulnTotal;
            families[index].details = d;
            families[index].vulnerabilityScore = calculateVulnerabilityScore(d);
            families[index].vulnerabilityLevel = determineVulnerabilityLevel(families[index].vulnerabilityScore);
        }
```

### 3. Sub-Option `[2]`: Soft-Deleting (Disabling / Re-enabling) a Family
Toggles `families[index].isActive` after `(Y/N)` confirmation instead of erasing the record:

```cpp
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
```

### 4. Sub-Options `[3]` & `[4]`: Editing or Deleting an Evacuee
- **`[3]`** updates `fullName`, `assignedCenter`, or `specialNeeds` (pressing Enter keeps the current value).
- **`[4]`** deletes the evacuee after `(Y/N)` confirmation by shifting remaining elements left:

```cpp
        if (promptConfirmation("CONFIRM: Are you sure you want to delete this evacuee record?")) {
            for (int i = index; i < evacueeCount - 1; ++i) {
                evacuees[i] = evacuees[i + 1];
            }
            evacueeCount--;
            printSuccess("Evacuee record deleted successfully.");
        }
```
