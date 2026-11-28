# `request_module.h` & `request_module.cpp` — Case 4 (Relief Request & Escalation Queue)

## Plain-English Purpose
This module is the **emergency dispatch desk**. Families can request food, medicine, rescue, shelter kits, hygiene kits, or water. The system automatically ranks requests from Priority 1 (most urgent) to Priority 4 (routine) and sorts the queue so responders help the most urgent cases first.

---

## Naming Conventions
- **`requestChoice`**: Submenu selection (`0–3`).
- **`typeChoice`**: Multiple-choice category selector (`1–6`) replacing free-text typing.
- **`priorityLevel`**: Integer `1` (L1-EMERGENCY) through `4` (L4-ROUTINE).
- **`reqId` / `reqIndex`**: Request ID string (`REQ-0001`) and its index in `requests[]`.

---

## Actual Code & Explanation

### 1. Sub-Option `[1]`: Family Lookup (Number-Only + Retry) & Multiple-Choice Type
Typing `4` normalizes to `FAM-0004`. If the family isn't found or is disabled, it loops so you can try again. Then it shows a 6-option menu instead of freestyle typing:

```cpp
        string familyCode;
        int familyIndex = -1;
        while (true) {
            familyCode = promptNormalizedCodeOrCancel("Enter Family ID number (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "FAM-", 4);
            if (familyCode.empty()) {
                printInfo("Relief request submission cancelled.");
                return;
            }

            familyIndex = recursiveSearchFamily(families, familyCount, familyCode);
            if (familyIndex == -1) {
                printError("Family record [" + familyCode + "] not found. Please try again (or enter '0' to cancel).");
                continue;
            }
            if (!families[familyIndex].isActive) {
                printError("Family record " + colorCode(familyCode) + " is currently DISABLED.");
                continue;
            }

            displayRecord(families[familyIndex]);
            bool isCorrectFamily = false;
            if (!promptYesNoOrCancel("Is this the correct family?", isCorrectFamily)) return;
            if (!isCorrectFamily) continue;
            break;
        }

        cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "--- SELECT RELIEF REQUEST TYPE ---\n" << Color::RESET;
        cout << "  [1] Food Pack / Emergency Rations\n";
        cout << "  [2] Medical Assistance & Supplies\n";
        cout << "  [3] Search, Rescue & Evacuation Transport\n";
        cout << "  [4] Temporary Shelter & Bedding Kit\n";
        cout << "  [5] Hygiene & Sanitation Kit\n";
        cout << "  [6] Potable Drinking Water\n";
        cout << "  [0] Cancel Request\n";

        int typeChoice = promptBoundedIntOrCancel("Choose Request Type", 1, 6, 0, "Please select an option between 1 and 6");
        if (typeChoice == 0) return;

        string requestType;
        switch (typeChoice) {
            case 1: requestType = "Food Pack"; break;
            case 2: requestType = "Medical Assistance"; break;
            case 3: requestType = "Rescue Transport"; break;
            case 4: requestType = "Shelter Kit"; break;
            case 5: requestType = "Hygiene Kit"; break;
            case 6: requestType = "Drinking Water"; break;
        }
```

### 2. Description, Y/N Guards & Priority Escalation Logic
Prevents blank descriptions or skipped `Y/N` prompts, then assigns `priorityLevel` (`1` = highest urgency, `4` = lowest):

```cpp
        string requestDescription = promptNonEmptyStringOrCancel("Enter Request Description (or '0' to cancel): ");
        if (requestDescription.empty()) return;

        bool isEmergency = false;
        if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_RED + "Is this an Emergency?" + Color::RESET, isEmergency)) return;

        bool hasMitigatingCircumstances = false;
        if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_YELLOW + "Has Mitigating Circumstances?" + Color::RESET, hasMitigatingCircumstances)) return;

        int priorityLevel = 4;
        if (isEmergency) {
            priorityLevel = 1;
        } else if (families[familyIndex].vulnerabilityLevel == "HIGH") {
            priorityLevel = 2;
        } else if (hasMitigatingCircumstances) {
            priorityLevel = 2;
        } else if (families[familyIndex].vulnerabilityLevel == "MODERATE") {
            priorityLevel = 3;
        } else {
            priorityLevel = 4;
        }
```

### 3. Sub-Options `[2]` & `[3]`: Sorted Queue & Status Updates
- **`[2]`** calls `recursiveSortRequestsByPriority(requests, requestCount)` and prints every request.
- **`[3]`** loops with `promptNormalizedCodeOrCancel(..., "REQ-", 4)` until a valid request is found (or `'0'` cancels), then updates `requestStatus` (`PENDING`, `APPROVED`, `IN_PROGRESS`, `RESOLVED`, `REJECTED`).
