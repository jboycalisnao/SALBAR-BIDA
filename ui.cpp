#include "ui.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

// Initialize console mode for ANSI Escape Sequence processing on Windows
void initConsoleUI() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#endif
    cout << "\033[2J\033[H" << flush;
}

void pauseConsole(const string& message) {
    cout << "\n" << Color::DIM << message << Color::RESET;
    cin.clear();
    string dummy;
    getline(cin, dummy);
}

void printHeaderBanner(const string& title, const string& subtitle, const string& facilityInfo) {
    cout << Color::BRIGHT_CYAN << "===========================================================================\n";
    cout << " " << Color::BOLD << Color::BRIGHT_WHITE << title << Color::RESET << "\n";
    if (!subtitle.empty()) {
        cout << " " << Color::DIM << Color::CYAN << subtitle << Color::RESET << "\n";
    }
    if (!facilityInfo.empty()) {
        cout << " " << Color::BOLD << Color::BRIGHT_WHITE << "Current Facility: " << Color::RESET << facilityInfo << "\n";
    }
    cout << Color::BRIGHT_CYAN << "===========================================================================" << Color::RESET << "\n";
}

void printSectionHeader(const string& title) {
    cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << ">> " << title << Color::RESET << "\n";
    printDivider(75, '-');
}

void printDivider(int width, char ch) {
    cout << Color::DIM << string(width, ch) << Color::RESET << "\n";
}

void printSuccess(const string& message) {
    cout << Color::BOLD << Color::BRIGHT_GREEN << "[OK] " << Color::RESET << message << "\n";
}

void printError(const string& message) {
    cout << Color::BOLD << Color::BRIGHT_RED << "[!] " << Color::RESET << Color::BRIGHT_RED << message << Color::RESET << "\n";
}

void printWarning(const string& message) {
    cout << Color::BOLD << Color::BRIGHT_YELLOW << "[WARN] " << Color::RESET << message << "\n";
}

void printInfo(const string& message) {
    cout << Color::BOLD << Color::BRIGHT_CYAN << "[i] " << Color::RESET << message << "\n";
}

// Color badges for Vulnerability
string badgeVulnerability(const string& level) {
    if (level == "HIGH") {
        return Color::BOLD + Color::BRIGHT_RED + "[HIGH]" + Color::RESET;
    } else if (level == "MODERATE") {
        return Color::BOLD + Color::BRIGHT_YELLOW + "[MODERATE]" + Color::RESET;
    } else {
        return Color::BOLD + Color::BRIGHT_GREEN + "[LOW]" + Color::RESET;
    }
}

// Color badges for Priority Level
string badgePriority(int priority) {
    switch (priority) {
        case 1:
            return Color::BOLD + Color::BRIGHT_RED + "L1-EMERGENCY" + Color::RESET;
        case 2:
            return Color::BOLD + Color::BRIGHT_YELLOW + "L2-HIGH" + Color::RESET;
        case 3:
            return Color::BOLD + Color::BRIGHT_CYAN + "L3-MODERATE" + Color::RESET;
        case 4:
        default:
            return Color::DIM + "L4-ROUTINE" + Color::RESET;
    }
}

// Color badges for Request Status
string badgeStatus(const string& status) {
    if (status == "RESOLVED") {
        return Color::BOLD + Color::BRIGHT_GREEN + status + Color::RESET;
    } else if (status == "APPROVED") {
        return Color::BOLD + Color::BRIGHT_CYAN + status + Color::RESET;
    } else if (status == "IN_PROGRESS") {
        return Color::BOLD + Color::BRIGHT_YELLOW + status + Color::RESET;
    } else if (status == "PENDING") {
        return Color::BOLD + Color::BRIGHT_MAGENTA + status + Color::RESET;
    } else if (status == "REJECTED") {
        return Color::DIM + Color::RED + status + Color::RESET;
    }
    return status;
}

// Format codes (FAM-0001, REQ-0001, EVAC-0001) in cyan
string colorCode(const string& code) {
    return Color::BOLD + Color::BRIGHT_CYAN + code + Color::RESET;
}

// Format center occupancy color based on how close to maximum capacity
string colorOccupancy(int current, int max) {
    ostringstream oss;
    if (max <= 0) {
        oss << current << " / " << max;
        return oss.str();
    }
    double ratio = static_cast<double>(current) / static_cast<double>(max);
    if (ratio >= 1.0) {
        oss << Color::BOLD << Color::BRIGHT_RED << current << Color::RESET << " / " << max << Color::BRIGHT_RED << " (FULL)" << Color::RESET;
    } else if (ratio >= 0.8) {
        oss << Color::BOLD << Color::BRIGHT_YELLOW << current << Color::RESET << " / " << max;
    } else {
        oss << Color::BOLD << Color::BRIGHT_GREEN << current << Color::RESET << " / " << max;
    }
    return oss.str();
}
