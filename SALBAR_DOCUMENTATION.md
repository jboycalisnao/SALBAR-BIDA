# SALBAR: Barangay Disaster Relief Management System
**Course**: CC 202 – Computer Programming I  
**Department**: College of Information and Communications Technology (CICT)  
**Institution**: West Visayas State University (WVSU), Iloilo City  
**Reference Textbook**: Malik, D. S. (2017). *C++ Programming: From Problem Analysis to Program Design*. Cengage Learning.

---

## 1. System Overview

The **SALBAR** (*Salbar: Barangay Disaster Relief*) system is a console-based disaster management and emergency relief escalation program designed to streamline disaster response operations at the barangay level. It translates the algorithm specified in the project pseudocode into robust, modular, and standards-compliant C++ code.

- **Universal Cancellation & Exit (`0` / `exit` / `cancel` / `back`)**: Users who accidentally enter any menu case or input prompt can safely cancel the operation and return to the main menu without entering dummy data or losing progress.

---

## 2. Course Syllabus Alignment Matrix (CC 202)

The program directly satisfies the **Course Learning Outcomes (LO1–LO4)** and incorporates all core units from the **CC 202 syllabus**:

| Syllabus Unit / Outcome | Specific Topics | Implementation in `salbar.cpp` |
| :--- | :--- | :--- |
| **LO1: Problem Solving & Algorithms** | Algorithmic flow, structured problem-solving | Complete translation of all 7 pseudocode cases into structured program flow. |
| **LO2 & LO3: Core Components** | Primitive types, basic computation, simple I/O, control flow, functions, recursion | All 6 required components are fully utilized across records, operations, and analysis. |
| **Unit 1: Introduction to Programming** | Variables, constants, primitive types (`int`, `char`, `bool`, `string`), arithmetic, casting, simple I/O | Constants (`MAX_FAMILIES`, `MAX_CENTERS`), structured records, type casting (`static_cast<char>`), `cin`/`cout` with `<iomanip>`. |
| **Unit 2: Basic Programming Constructs** | Expressions, relational (`<`, `>=`, `==`), logical (`&&`, `\|\|`, `!`), control flow | Multi-variable conditional expressions (e.g. `headOfFamily.empty() \|\| numberOfFamilyMembers < 1`), boundary checks. |
| **Unit 3: Decision Making** | `if`, `if-else`, nested `if-else`, `switch` statements | Main menu driven by `switch (choice)`, nested priority assessment logic in Case 4. |
| **Unit 4: Looping Constructs** | `while`, `do-while`, `for`, `break`, `continue` | Main loop using `do-while (choice != 7)`, array traversals using `for`, buffer clearing using `while`. |
| **Unit 5: Functions & Modular Programming** | Declarations/prototypes, parameter passing (by value & by reference `&`), function overloading, recursion, multi-file modular architecture | Modular organization across dedicated header (`.h`) and implementation (`.cpp`) files (`models.h`, `utils.h/cpp`, `analytics.h/cpp`, `operations.h/cpp`, `persistence.h/cpp`, `handlers.h/cpp`, and `salbar.cpp`), separated prototypes, pass-by-reference counters (`int& familyCount`), overloaded `displayRecord()`. |
| **Unit 5: Recursion Requirement** | Base cases, recursive calls, call stack | 1. `recursiveSearchFamily()`<br>2. `recursiveSearchCenter()`<br>3. `recursiveSearchRequest()`<br>4. `recursiveCountHighVulnerability()`<br>5. `recursiveSumOccupancy()`<br>6. `recursiveSortRequestsByPriority()` |
| **Unit 6: Arrays** | 1D arrays, array traversal, searching, sorting, arrays as parameters | Parallel/record arrays for families, centers, requests, and evacuees passed as function parameters. |
| **Unit 6: Multi-Dimensional Arrays** | 2D array declaration, nested row-column indexing | 2D Demographic Matrix `matrix[3][5]` (Vulnerability Level vs. Demographic Group). |
| **File I/O (Persistence)** | `<fstream>`, file writing and reading | Automatic file storage and recovery on launch (`data/families.txt`, `data/centers.txt`, `data/requests.txt`, `data/evacuees.txt`, `data/config.txt`). |

---

## 3. Detailed Feature Breakdown (Pseudocode Mapping)

### Case 1: Family Registration
- **Pre-condition Facility Guard**: Checks that at least one evacuation facility is registered. If no facility exists, family registration is blocked and the user is directed to Option 3.
- **Dynamic Capacity & Occupancy Update**: Automatically increases the active evacuation facility's `currentOccupancy` by the registered family members, updating available slots in real time in the main menu header and persistent database.
- Assigns registered family members directly to the currently active evacuation center.
- Accepts head of family and member count with input validation (checks for non-empty string and size $\ge 1$).
- Formats 4-digit code (e.g., `FAM-0001`).
- Collects counts for children, seniors, PWDs, pregnant mothers, and medical needs.
- **Vulnerability Scoring**:
  $$\text{Score} = (\text{Children} \times 1) + (\text{Seniors} \times 2) + (\text{PWD} \times 3) + (\text{Pregnant} \times 3) + (\text{Medical} \times 3)$$
- Assigns Level:
  - $\text{Score} \ge 8 \implies \textbf{HIGH}$
  - $\text{Score} \ge 4 \implies \textbf{MODERATE}$
  - $\text{Otherwise} \implies \textbf{LOW}$

### Case 2: View Population
- Displays all currently active registered families in a structured roster (Code, Head of Family, Total Members, Vulnerability Level, and Status). Disabled (soft-deleted) records are automatically excluded from the view.

### Case 3: Evacuation Center Management
- **Add Center**: Validates capacity ($> 0$) and current occupancy ($0 \le \text{occupancy} \le \text{capacity}$).
- **View Centers**: Computes Available Slots ($\text{Capacity} - \text{Occupancy}$) dynamically.
- **Update Center**: Uses recursive search to locate facility by name and updates details.
- **Delete Center**: Uses recursive search with confirmation prompt (`Y/N`), element shifting, and **cascading deletion** that automatically removes all evacuee records assigned to the deleted center and updates family evacuation statuses.
- **Select Active Facility**: Allows switching the current active facility displayed in the main menu header and assigned to new evacuees.

### Case 4: Pending Request Escalation
- Submits relief requests linked to an active `FamilyCode`.
- **Multiple-Choice Request Types**: Eliminates freestyle typing by providing standardized categories (Food Pack, Medical Assistance, Rescue Transport, Shelter Kit, Hygiene Kit, Drinking Water).
- **Strict Non-Empty Field Validation**: Prevents blank inputs across all fields with re-prompting and cancellation guards (`0`/`exit`).
- Evaluates **Priority Level**:
  - Priority 1: Emergency (`IsEmergency == true`)
  - Priority 2: High Vulnerability OR Mitigating Circumstances
  - Priority 3: Moderate Vulnerability
  - Priority 4: Standard / Low Urgency
- **Priority Sorting**: Uses recursive sorting algorithm to order requests by highest urgency.
- **Status Updates**: Updates request lifecycle status (`PENDING`, `APPROVED`, `IN_PROGRESS`, `RESOLVED`, `REJECTED`).

### Case 5: Edit or Delete Records
- Edits family and evacuee records.
- **Soft-Delete for Families**: Instead of permanently deleting family records, families are marked as `DISABLED` (soft delete) to preserve historical data. Disabled records can also be re-enabled.
- Prompts for explicit user confirmation before disabling or editing records.

### Case 6: View Population Summary
- Aggregates:
  - Total Families Registered
  - Total Barangay Population
  - Total Evacuees in Centers
  - Total Vulnerable Individuals
  - Population Not Currently In Centers ($\text{Total Population} - \text{Total Evacuees}$)
  - Total Available Evacuation Center Slots
  - Number of Pending Requests (Recursive tally)
  - Number of HIGH Vulnerability Families (Recursive tally)
- Generates a **2D Multi-dimensional Demographic Cross-Tabulation Matrix** breaking down vulnerable counts by vulnerability level.

### Case 7: Save and Exit
- Persists all database records and tracking counters to disk.
- Automatically reloads state on subsequent program launches.

---

## 4. Modular File Architecture

The system is organized into modular header (`.h`) and implementation (`.cpp`) files adhering to standard C++ modular programming practices:

| Module | Files | Description |
| :--- | :--- | :--- |
| **Data Models & Constants** | `models.h` | Defines global limits (`MAX_FAMILIES`, etc.), filenames, and data structures (`Family`, `EvacuationCenter`, `ReliefRequest`, `Evacuee`, `VulnerabilityDetails`). |
| **UI Styling & Terminal Colors** | `ui.h`, `ui.cpp` | ANSI color codes, Windows Virtual Terminal initialization, visual badges (`[HIGH]`, `[L1: EMERGENCY]`), status badges, and styled table formatting. |
| **System Utilities** | `utils.h`, `utils.cpp` | Screen header, buffer flushing, uppercase conversion, code formatter, and user confirmation prompts. |
| **Demographic Analytics** | `analytics.h`, `analytics.cpp` | Vulnerability scoring, level classification, 2D demographic cross-tabulation matrix, and Case 6 summary report. |
| **Record Operations** | `operations.h`, `operations.cpp` | Overloaded `displayRecord()` functions, recursive searches, recursive aggregations, and recursive sorting. |
| **Data Persistence** | `persistence.h`, `persistence.cpp` | File I/O functions (`loadData()`, `saveData()`) and Case 7 (`handleSaveAndExit()`). |
| **Family Registry Module** | `family_module.h`, `family_module.cpp` | Case 1 (Family Registration) and Case 2 (View Population & Family Registry). |
| **Evacuation Center Module** | `center_module.h`, `center_module.cpp` | Case 3 (Evacuation Center Management & cascading evacuee removal). |
| **Relief Request Module** | `request_module.h`, `request_module.cpp` | Case 4 (Relief Request submission, priority escalation, and status updates). |
| **Record Maintenance Module** | `record_module.h`, `record_module.cpp` | Case 5 (Edit, disable/re-enable families, and edit/delete evacuees). |
| **Main Driver** | `salbar.cpp` | Program entry point (`main()`), main menu presentation, and do-while control loop. |
| **Monolithic Backup** | `salbar_monolithic.cpp.bak` | Preserved single-file version for archive/submission reference. |

---

## 5. How to Compile and Run

### Option A: Using MinGW / GCC via Command Prompt or Terminal (Recommended)
```bash
# Navigate to project folder
cd "c:\Users\Calisnao\OneDrive - Leon National High School\JBOY\COLLEGE LIFE IS REAL\1ST YEAR - 1ST SEM\CC202\FINAL"

# Compile all modular files with C++11 or higher
g++ -std=c++11 -Wall *.cpp -o salbar.exe

# Or specify individual modules:
g++ -std=c++11 -Wall ui.cpp utils.cpp analytics.cpp operations.cpp persistence.cpp family_module.cpp center_module.cpp request_module.cpp record_module.cpp salbar.cpp -o salbar.exe

# Run the program
./salbar.exe
```

### Option B: Using Dev-C++ or Code::Blocks
1. Open Dev-C++ or Code::Blocks.
2. Create a new **Console Project** (or add all `.h` and `.cpp` files to the project).
3. Add all `.h` and `.cpp` files (`models.h`, `ui.*`, `utils.*`, `analytics.*`, `operations.*`, `persistence.*`, `family_module.*`, `center_module.*`, `request_module.*`, `record_module.*`, and `salbar.cpp`).
4. Press **F9** (or **Execute -> Compile & Run**).

### Option C: Using Visual Studio / VS Code
1. Open the folder in Visual Studio or VS Code with the C/C++ Runner extension.
2. Press **Ctrl+Shift+B** or use the Runner's Debug/Run button (it automatically compiles all `.cpp` files in the directory).
