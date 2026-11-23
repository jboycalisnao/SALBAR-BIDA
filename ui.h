#ifndef SALBAR_UI_H
#define SALBAR_UI_H

#include <string>

// ========================================================================================
// ANSI COLOR CODES & TEXT STYLES
// ========================================================================================
namespace Color {
    const std::string RESET         = "\033[0m";
    const std::string BOLD          = "\033[1m";
    const std::string DIM           = "\033[2m";
    const std::string UNDERLINE     = "\033[4m";

    // Standard Colors
    const std::string RED           = "\033[31m";
    const std::string GREEN         = "\033[32m";
    const std::string YELLOW        = "\033[33m";
    const std::string BLUE          = "\033[34m";
    const std::string MAGENTA       = "\033[35m";
    const std::string CYAN          = "\033[36m";
    const std::string WHITE         = "\033[37m";

    // Bright Colors
    const std::string BRIGHT_RED    = "\033[91m";
    const std::string BRIGHT_GREEN  = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE   = "\033[94m";
    const std::string BRIGHT_MAGENTA= "\033[95m";
    const std::string BRIGHT_CYAN   = "\033[96m";
    const std::string BRIGHT_WHITE  = "\033[97m";
}

// ========================================================================================
// UI INITIALIZATION & PRESENTATION HELPERS
// ========================================================================================
void initConsoleUI();
void clearScreen();
void pauseConsole(const std::string& message = "Press Enter to return to menu...");

void printHeaderBanner(const std::string& title, const std::string& subtitle = "", const std::string& facilityInfo = "");
void printSectionHeader(const std::string& title);
void printDivider(int width = 75, char ch = '-');

void printSuccess(const std::string& message);
void printError(const std::string& message);
void printWarning(const std::string& message);
void printInfo(const std::string& message);

// Visual Badges and Formatted Strings
std::string badgeVulnerability(const std::string& level);
std::string badgePriority(int priority);
std::string badgeStatus(const std::string& status);
std::string colorCode(const std::string& code);
std::string colorOccupancy(int current, int max);

#endif // SALBAR_UI_H
