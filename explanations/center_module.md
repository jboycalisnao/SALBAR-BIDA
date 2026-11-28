# `center_module.h` & `center_module.cpp` — Case 3 (Evacuation Center Management)

## Plain-English Purpose
This module manages the **shelters / facilities** (schools, gyms, barangay halls). Through its 5 sub-options, you can add a new center, view all centers and their remaining slots, update capacity, delete a center (which also automatically cleans up any evacuees assigned to it), or switch which center is currently active.

---

## Naming Conventions
- **`centerChoice`**: Integer (`0–5`) selecting the submenu action.
- **`deletedCenterName`**: Saved before shifting the array so we can match and remove evacuees who were housed under that center's name.
- **`writeIdx`**: A "write pointer" used to compact the `evacuees[]` array in-place when removing evacuees from a deleted center.

---

## Actual Code & Explanation

### 1. Sub-Option `[1]`: Adding a Center with Non-Empty & Bounded Validation
Uses `promptNonEmptyStringOrCancel` and `promptBoundedIntOrCancel` so no field can be skipped with Enter:

```cpp
        string facilityName = promptNonEmptyStringOrCancel("Enter Facility Name (or '0' / 'exit' to cancel): ");
        if (facilityName.empty()) {
            printInfo("Operation cancelled. Returning to menu.");
            return;
        }

        string facilityType = promptNonEmptyStringOrCancel("Enter Facility Type (e.g., Gym, School, Barangay Hall, or '0' to cancel): ");
        if (facilityType.empty()) {
            printInfo("Operation cancelled. Returning to menu.");
            return;
        }

        int maximumCapacity = promptBoundedIntOrCancel("Enter Maximum Capacity", 1, 100000, 0, "Capacity must be greater than 0");
        if (maximumCapacity == 0) return;

        int currentOccupancy = promptBoundedIntOrCancel("Enter Current Occupancy", 0, maximumCapacity, -1, "Cannot exceed maximum capacity");
        if (currentOccupancy == -1) return;

        centers[centerCount].facilityName = facilityName;
        centers[centerCount].facilityType = facilityType;
        centers[centerCount].maximumCapacity = maximumCapacity;
        centers[centerCount].currentOccupancy = currentOccupancy;
        centerCount++;
```

### 2. Sub-Option `[3]`: Updating a Center (with Retry Loop)
If the typed center name isn't found, the `while (true)` loop lets you try another name instead of closing back to the menu:

```cpp
        int index = -1;
        while (true) {
            string searchName = promptNonEmptyStringOrCancel("Enter Facility Name to Update (or '0' / 'exit' to cancel): ");
            if (searchName.empty()) {
                printInfo("Operation cancelled.");
                return;
            }

            index = recursiveSearchCenter(centers, centerCount, searchName);
            if (index != -1) {
                break;
            }
            printError("Evacuation center '" + searchName + "' not found. Please try again (or enter '0' to cancel).");
        }
```

### 3. Sub-Option `[4]`: Deleting a Center & Cascading Evacuee Removal
After finding the center (with retry loop) and confirming `(Y/N)`, removes all evacuees assigned to that center, resets their family's `isEvacuated` flag, shifts `centers[]`, and safely adjusts `activeCenterIndex`:

```cpp
        if (promptConfirmation("Are you sure you want to delete this center and remove all evacuees under it?")) {
            // 1. Remove all evacuees assigned to this center
            int removedEvacueesCount = 0;
            int writeIdx = 0;
            for (int i = 0; i < evacueeCount; ++i) {
                if (toUpperCase(evacuees[i].assignedCenter) == toUpperCase(deletedCenterName)) {
                    for (int f = 0; f < familyCount; ++f) {
                        if (families[f].familyCode == evacuees[i].familyCode) {
                            families[f].isEvacuated = false;
                        }
                    }
                    removedEvacueesCount++;
                } else {
                    evacuees[writeIdx++] = evacuees[i];
                }
            }
            evacueeCount = writeIdx;

            // 2. Delete the center from centers list
            for (int i = index; i < centerCount - 1; ++i) {
                centers[i] = centers[i + 1];
            }
            centerCount--;

            // 3. Adjust activeCenterIndex safely
            if (activeCenterIndex == index) {
                activeCenterIndex = 0;
            } else if (activeCenterIndex > index) {
                activeCenterIndex--;
            }
            if (activeCenterIndex >= centerCount && centerCount > 0) {
                activeCenterIndex = 0;
            }
        }
```

### 4. Sub-Option `[5]`: Selecting the Active Facility
Lists all centers with `[CURRENT ACTIVE]` next to the selected one and updates `activeCenterIndex = sel - 1`.
