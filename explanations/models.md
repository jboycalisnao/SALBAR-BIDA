# `models.h` — Data Blueprints & System Constants

## Plain-English Purpose
Think of `models.h` as the **blank forms and filing cabinet rules** of the barangay office. Before the program can store any families, evacuation centers, relief requests, or evacuees, it needs to know *what information belongs on each form* and *how many forms the cabinet can hold*. This file has no `.cpp` counterpart because it only defines shapes and constants, not actions.

---

## Naming Conventions
- **`MAX_*` (`UPPER_SNAKE_CASE`)**: Maximum capacity limits for arrays (e.g., `MAX_FAMILIES = 200`).
- **`FILE_*` (`UPPER_SNAKE_CASE`)**: Exact `.txt` filenames used to save data on disk (e.g., `FILE_FAMILIES = "families.txt"`).
- **`PascalCase` Structs**: `VulnerabilityDetails`, `Family`, `EvacuationCenter`, `ReliefRequest`, `Evacuee`.
- **`camelCase` Fields**: Descriptive names inside each struct (e.g., `headOfFamily`, `currentOccupancy`).
- **`is*` / `has*` Booleans**: Yes/no flags always start with `is` or `has` (e.g., `isActive`, `isEvacuated`, `isEmergency`).

---

## Actual Code & Explanation

### 1. Global Constants & Filenames
These fixed numbers prevent arrays from overflowing and keep `.txt` filenames in one place:

```cpp
const int MAX_FAMILIES = 200;
const int MAX_CENTERS  = 50;
const int MAX_REQUESTS = 200;
const int MAX_EVACUEES = 200;

// Multi-dimensional array dimensions for demographic matrix (Unit 6: Multi-dimensional Arrays)
// Rows: 3 (0 = LOW, 1 = MODERATE, 2 = HIGH)
// Columns: 5 (0 = Children, 1 = Seniors, 2 = PWDs, 3 = Pregnant, 4 = Medical Needs)
const int VULN_LEVEL_ROWS = 3;
const int DEMO_COLUMNS   = 5;

// Data persistence directory and filenames (Case 7: Save and Exit)
const std::string DATA_DIR      = "data";
const std::string FILE_FAMILIES = "data/families.txt";
const std::string FILE_CENTERS  = "data/centers.txt";
const std::string FILE_REQUESTS = "data/requests.txt";
const std::string FILE_EVACUEES = "data/evacuees.txt";
const std::string FILE_CONFIG   = "data/config.txt";
```

### 2. `VulnerabilityDetails` & `Family` Structs
`VulnerabilityDetails` is a mini-form nested inside `Family` to track the 5 vulnerable groups. `Family` includes `isActive` for soft-deletes (disabling without erasing history):

```cpp
struct VulnerabilityDetails {
    int children;
    int seniorCitizens;
    int personsWithDisabilities;
    int pregnantMembers;
    int medicalNeeds;
};

struct Family {
    std::string familyCode;             // e.g., "FAM-0001"
    std::string headOfFamily;           // Full Name
    int numberOfFamilyMembers;          // Total family size
    int numberOfVulnerableMembers;      // Count of vulnerable individuals
    int vulnerabilityScore;             // Computed vulnerability score
    std::string vulnerabilityLevel;     // "HIGH", "MODERATE", "LOW"
    VulnerabilityDetails details;       // Sub-demographic breakdown
    bool isEvacuated;                   // Status whether relocated to center
    bool isActive;                      // Soft-delete flag: true = active, false = disabled
};
```

### 3. `EvacuationCenter`, `ReliefRequest`, & `Evacuee` Structs
These represent shelters, aid requests (linked to a family via `familyCode`), and individual evacuees (linked to a shelter via `assignedCenter`):

```cpp
struct EvacuationCenter {
    std::string facilityName;           // Name of gym, hall, school, etc.
    std::string facilityType;           // "Barangay Hall", "Gymnasium", "School", etc.
    int maximumCapacity;                // Max persons allowable
    int currentOccupancy;               // Current persons housed
};

struct ReliefRequest {
    std::string requestId;              // e.g., "REQ-0001"
    std::string familyCode;             // Foreign key link to Family
    std::string requestType;            // "Food Pack", "Medical", "Rescue", etc.
    std::string requestDescription;     // Detailed description
    bool isEmergency;                   // Emergency flag
    bool hasMitigatingCircumstances;    // Mitigating circumstances flag
    int priorityLevel;                  // 1 (Highest) to 4 (Lowest)
    std::string requestStatus;          // "PENDING", "APPROVED", "IN_PROGRESS", "RESOLVED"
};

struct Evacuee {
    std::string evacueeId;              // e.g., "EVAC-0001"
    std::string familyCode;             // Reference to Family
    std::string fullName;               // Evacuee Name
    int age;                            // Age
    std::string assignedCenter;         // Name of Evacuation Center
    std::string specialNeeds;           // Medical/dietary needs
};
```
