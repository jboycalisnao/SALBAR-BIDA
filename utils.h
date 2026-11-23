#ifndef SALBAR_UTILS_H
#define SALBAR_UTILS_H

#include <string>

// System & Utility Helpers
void displayHeader();
void clearInputBuffer();
std::string formatCode(const std::string& prefix, int number, int width);
std::string toUpperCase(std::string str);
bool promptConfirmation(const std::string& actionPrompt);
bool isExitCommand(const std::string& input);

// Input Validation Helper with dynamic bounds
int promptBoundedInt(const std::string& promptLabel, int minVal, int maxVal, const std::string& errorHint = "");
int promptBoundedIntOrCancel(const std::string& promptLabel, int minVal, int maxVal, int cancelVal = 0, const std::string& errorHint = "");

// Non-empty string and boolean prompt guards (prevents proceeding without input)
std::string promptNonEmptyStringOrCancel(const std::string& prompt);
bool promptYesNoOrCancel(const std::string& prompt, bool& result);

// Code Normalization Helpers (accepts raw number e.g. 1 or 0001 and formats to FAM-0001 / REQ-0001)
std::string normalizeCode(const std::string& input, const std::string& prefix, int width = 4);
std::string promptNormalizedCodeOrCancel(const std::string& prompt, const std::string& prefix, int width = 4);

#endif // SALBAR_UTILS_H
