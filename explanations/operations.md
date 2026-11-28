# `operations.h` & `operations.cpp` — Recursive Search, Sort, Totals & Display

## Plain-English Purpose
This module contains the **behind-the-scenes math and filing helpers**. Instead of using `for` loops everywhere, it uses **recursion** (functions that solve one item and call themselves on the rest) to search records, count totals, and sort emergency requests, plus **function overloading** (giving 4 different print functions the exact same name `displayRecord`).

---

## Naming Conventions
- **`recursiveSearch*`**: Search an array one index at a time by calling themselves with `index + 1`. Return the array index if found, or `-1` if not found.
- **`recursiveCount*` / `recursiveSum*`**: Tally totals by checking the last element (`n - 1`) and adding the result of `n - 1` remaining elements.
- **`displayRecord(...)`**: Overloaded function name — C++ picks the right version automatically depending on the struct type passed in.

---

## Actual Code & Explanation

### 1. Recursive Search Functions
Each search checks if `index >= size` (not found → `-1`), checks if the current element matches (found → `index`), or calls itself on `index + 1`. Center search uses `toUpperCase()` so `"evac 1"` matches `"Evac 1"`:

```cpp
int recursiveSearchFamily(const Family families[], int size, const string& code, int index) {
    if (index >= size) {
        return -1;
    }
    if (families[index].familyCode == code) {
        return index;
    }
    return recursiveSearchFamily(families, size, code, index + 1);
}

int recursiveSearchCenter(const EvacuationCenter centers[], int size, const string& name, int index) {
    if (index >= size) {
        return -1;
    }
    if (toUpperCase(centers[index].facilityName) == toUpperCase(name)) {
        return index;
    }
    return recursiveSearchCenter(centers, size, name, index + 1);
}
```
*(Same pattern is used for `recursiveSearchRequest` and `recursiveSearchEvacuee`.)*

### 2. Recursive Aggregation Functions
Counts active `HIGH`-vulnerability families, total occupancy, total active population, and pending requests recursively:

```cpp
int recursiveCountHighVulnerability(const Family families[], int n) {
    if (n <= 0) {
        return 0;
    }
    int count = (families[n - 1].isActive && families[n - 1].vulnerabilityLevel == "HIGH") ? 1 : 0;
    return count + recursiveCountHighVulnerability(families, n - 1);
}

int recursiveCountPendingRequests(const ReliefRequest requests[], int n) {
    if (n <= 0) {
        return 0;
    }
    int isPending = (requests[n - 1].requestStatus == "PENDING") ? 1 : 0;
    return isPending + recursiveCountPendingRequests(requests, n - 1);
}
```

### 3. Recursive Sorting (`recursiveSortRequestsByPriority`)
Finds the lowest `priorityLevel` (where `1` = Emergency is highest urgency), swaps it to position `0`, and recursively sorts the remaining `n - 1` requests:

```cpp
void recursiveSortRequestsByPriority(ReliefRequest requests[], int n) {
    if (n <= 1) {
        return;
    }

    int minIndex = 0;
    for (int i = 1; i < n; ++i) {
        if (requests[i].priorityLevel < requests[minIndex].priorityLevel) {
            minIndex = i;
        }
    }

    if (minIndex != 0) {
        ReliefRequest temp = requests[0];
        requests[0] = requests[minIndex];
        requests[minIndex] = temp;
    }

    recursiveSortRequestsByPriority(requests + 1, n - 1);
}
```

### 4. Function Overloading (`displayRecord`)
Four overloaded `displayRecord` functions format `Family`, `EvacuationCenter`, `ReliefRequest`, and `Evacuee`. Notice how `displayRecord(const ReliefRequest&)` calculates manual padding (`prioPad`, `statPad`) so invisible ANSI color bytes don't break table alignment:

```cpp
void displayRecord(const ReliefRequest& request) {
    string prioStr;
    switch (request.priorityLevel) {
        case 1: prioStr = "L1-EMERGENCY"; break;
        case 2: prioStr = "L2-HIGH"; break;
        case 3: prioStr = "L3-MODERATE"; break;
        default: prioStr = "L4-ROUTINE"; break;
    }
    int prioPad = 18 - static_cast<int>(prioStr.length());
    if (prioPad < 1) prioPad = 1;

    int statPad = 16 - static_cast<int>(request.requestStatus.length());
    if (statPad < 1) statPad = 1;

    cout << "  " << left << setw(12) << request.requestId
         << setw(12) << request.familyCode
         << setw(20) << request.requestType
         << "  " << badgePriority(request.priorityLevel) << string(prioPad, ' ')
         << " " << badgeStatus(request.requestStatus) << string(statPad, ' ')
         << " " << Color::DIM << request.requestDescription << Color::RESET << "\n";
}
```
