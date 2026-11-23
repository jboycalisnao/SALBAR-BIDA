#include "utils.h"
#include "ui.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

void displayHeader() {
    printHeaderBanner("SALBAR: BARANGAY DISASTER RELIEF", "Emergency Operations & Evacuation Management");
}

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

string toUpperCase(string str) {
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = static_cast<char>(str[i] - ('a' - 'A'));
        }
    }
    return str;
}

bool promptConfirmation(const string& actionPrompt) {
    char confirm;
    cout << Color::BOLD << Color::BRIGHT_YELLOW << actionPrompt << " (Y/N): " << Color::RESET;
    cin >> confirm;
    clearInputBuffer();
    return (confirm == 'Y' || confirm == 'y');
}

bool isExitCommand(const string& input) {
    if (input.empty()) return false;
    string upper = toUpperCase(input);
    return (upper == "0" || upper == "EXIT" || upper == "QUIT" || upper == "CANCEL" || upper == "BACK");
}

int promptBoundedInt(const string& promptLabel, int minVal, int maxVal, const string& errorHint) {
    int value = 0;
    while (true) {
        cout << promptLabel << " [" << minVal << " - " << maxVal << "]: ";
        if (cin >> value) {
            clearInputBuffer();
            if (value >= minVal && value <= maxVal) {
                return value;
            }
            if (!errorHint.empty() && value > maxVal) {
                printError("Value cannot exceed " + to_string(maxVal) + " (" + errorHint + ").");
            } else {
                printError("Value must be between " + to_string(minVal) + " and " + to_string(maxVal) + ".");
            }
        } else {
            clearInputBuffer();
            printError("Invalid input. Please enter a valid whole number.");
        }
    }
}

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
