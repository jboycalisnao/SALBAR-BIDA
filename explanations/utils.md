# `utils.h` & `utils.cpp` — Input Guards & Helper Tools

## Plain-English Purpose
Users accidentally press Enter on blank lines, type letters when a number is asked for, or forget to type `FAM-` before an ID. `utils.h` and `utils.cpp` act like **bouncers at the door** — they clean up messy input, block empty submissions, let you type `0`/`exit` to back out anytime, and automatically turn `4` into `FAM-0004`.

---

## Naming Conventions
- **`prompt*` Functions**: Interactive helpers that keep asking the user until valid input is given.
- **`*OrCancel` Suffix**: Indicates the function recognizes cancel words (`0`, `exit`, `quit`, `cancel`, `back`) and returns a special cancel signal (`0`, `-1`, `""`, or `false`).
- **`to*` / `format*` / `normalize*`**: Pure text-transformation helpers.

---

## Actual Code & Explanation

### 1. Buffer Clearing, Code Formatting & Exit Detection
`clearInputBuffer()` throws away leftover characters/newlines after `cin >>`. `formatCode()` pads numbers with zeroes (`1` → `FAM-0001`). `isExitCommand()` checks if the user wants to cancel:

```cpp
void clearInputBuffer() {
    cin.clear();
    while (cin.peek() != '\n' && cin.peek() != EOF) {
        cin.get();
    }
    if (cin.peek() == '\n') {
        cin.get();
    }
}

string formatCode(const string& prefix, int number, int width) {
    ostringstream oss;
    oss << prefix << setfill('0') << setw(width) << number;
    return oss.str();
}

bool isExitCommand(const string& input) {
    if (input.empty()) return false;
    string upper = toUpperCase(input);
    return (upper == "0" || upper == "EXIT" || upper == "QUIT" || upper == "CANCEL" || upper == "BACK");
}
```

### 2. Bounded Integer Prompts (`promptBoundedInt` & `promptBoundedIntOrCancel`)
Loops until the user enters a whole number inside `[minVal, maxVal]` (or `cancelVal` if cancellation is allowed):

```cpp
int promptBoundedIntOrCancel(const string& promptLabel, int minVal, int maxVal, int cancelVal, const string& errorHint) {
    int value = 0;
    while (true) {
        cout << promptLabel << " [" << minVal << " - " << maxVal << "]: ";
        if (cin >> value) {
            clearInputBuffer();
            if (value == cancelVal) {
                return cancelVal;
            }
            if (value >= minVal && value <= maxVal) {
                return value;
            }
            if (!errorHint.empty() && value > maxVal) {
                printError("Value cannot exceed " + to_string(maxVal) + " (" + errorHint + ").");
            } else {
                printError("Value must be between " + to_string(minVal) + " and " + to_string(maxVal) + " (or " + to_string(cancelVal) + " to cancel).");
            }
        } else {
            clearInputBuffer();
            printError("Invalid input. Please enter a valid whole number.");
        }
    }
}
```

### 3. Non-Empty String & Yes/No Guards
Prevents skipping required fields by just pressing Enter:

```cpp
string promptNonEmptyStringOrCancel(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        if (isExitCommand(input)) {
            return "";
        }
        if (input.find_first_not_of(" \t\r\n") != string::npos) {
            return input;
        }
        printError("This field cannot be empty. Please enter a valid input (or '0' to cancel).");
    }
}

bool promptYesNoOrCancel(const string& prompt, bool& result) {
    while (true) {
        cout << prompt << " (Y/N, or '0' to cancel): ";
        string input;
        getline(cin, input);
        if (isExitCommand(input)) {
            return false;
        }
        if (!input.empty()) {
            char ch = input[0];
            if (ch == 'Y' || ch == 'y') {
                result = true;
                return true;
            } else if (ch == 'N' || ch == 'n') {
                result = false;
                return true;
            }
        }
        printError("Invalid input. Please enter 'Y' for Yes, 'N' for No, or '0' to cancel.");
    }
}
```

### 4. Prefix-Optional Code Normalization (`normalizeCode` & `promptNormalizedCodeOrCancel`)
Extracts digits from whatever the user typed (`"4"`, `"0004"`, or `"FAM-0004"`) and formats it cleanly:

```cpp
string normalizeCode(const string& input, const string& prefix, int width) {
    if (input.empty()) return "";
    string digitsOnly = "";
    for (size_t i = 0; i < input.length(); ++i) {
        if (input[i] >= '0' && input[i] <= '9') {
            digitsOnly += input[i];
        }
    }
    if (!digitsOnly.empty()) {
        try {
            int num = stoi(digitsOnly);
            return formatCode(prefix, num, width);
        } catch (...) {
            // fallback
        }
    }
    return toUpperCase(input);
}

string promptNormalizedCodeOrCancel(const string& prompt, const string& prefix, int width) {
    string raw = promptNonEmptyStringOrCancel(prompt);
    if (raw.empty()) {
        return "";
    }
    return normalizeCode(raw, prefix, width);
}
```
