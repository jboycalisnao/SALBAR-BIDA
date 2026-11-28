#include "request_module.h"
#include "utils.h"
#include "operations.h"
#include "ui.h"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

// ========================================================================================
// CASE 4: PENDING REQUEST ESCALATION
// ========================================================================================
void handlePendingRequestEscalation(ReliefRequest requests[], int& requestCount, int& nextRequestNumber,
                                    const Family families[], int familyCount) {
    while (true) {
        clearScreen();
        printSectionHeader("RELIEF REQUEST & ESCALATION QUEUE");
        cout << "  [1] Submit New Relief Request\n";
        cout << "  [2] View Escalation Queue (Sorted by Urgency / Priority)\n";
        cout << "  [3] Update Request Resolution Status\n";
        cout << "  [0] Back to Main Menu\n";
        cout << Color::BOLD << "Enter Choice [0-3]: " << Color::RESET;

        int requestChoice;
        if (!(cin >> requestChoice)) {
            printError("Invalid selection.");
            clearInputBuffer();
            pauseConsole();
            continue;
        }
        clearInputBuffer();

        cout << "\n";

        if (requestChoice == 0) {
            printInfo("Returning to main menu.");
            return;
        }

        if (requestChoice == 1) {
            if (requestCount >= MAX_REQUESTS) {
                printError("Request queue is full (" + to_string(MAX_REQUESTS) + ").");
                pauseConsole();
                continue;
            }

            if (familyCount == 0) {
                printWarning("No families registered yet. Please register a family under Option [1] first.");
                pauseConsole();
                continue;
            }

            string familyCode;
            int familyIndex = -1;
            bool cancelled = false;
            while (true) {
                familyCode = promptNormalizedCodeOrCancel("Enter Family ID number (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "FAM-", 4);
                if (familyCode.empty()) {
                    printInfo("Relief request submission cancelled.");
                    cancelled = true;
                    break;
                }

                // VERIFY FamilyCode exists (Recursive search)
                familyIndex = recursiveSearchFamily(families, familyCount, familyCode);
                if (familyIndex == -1) {
                    printError("Family record [" + familyCode + "] not found. Please try again (or enter '0' to cancel).");
                    continue;
                }

                if (!families[familyIndex].isActive) {
                    printError("Family record " + colorCode(familyCode) + " is currently DISABLED.");
                    printWarning("Cannot submit relief requests for disabled family records. Please re-enable it first or choose another family.");
                    continue;
                }

                cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "--- FAMILY RECORD FOUND ---\n" << Color::RESET;
                displayRecord(families[familyIndex]);
                cout << "\n";

                bool isCorrectFamily = false;
                if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_YELLOW + "Is this the correct family?" + Color::RESET, isCorrectFamily)) {
                    printInfo("Relief request submission cancelled.");
                    cancelled = true;
                    break;
                }
                if (!isCorrectFamily) {
                    printInfo("Please enter another Family ID number.\n");
                    continue;
                }

                break;
            }
            if (cancelled) {
                pauseConsole("Press Enter to return to queue menu...");
                continue;
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
            if (typeChoice == 0) {
                printInfo("Relief request submission cancelled.");
                pauseConsole("Press Enter to return to queue menu...");
                continue;
            }

            string requestType;
            switch (typeChoice) {
                case 1: requestType = "Food Pack"; break;
                case 2: requestType = "Medical Assistance"; break;
                case 3: requestType = "Rescue Transport"; break;
                case 4: requestType = "Shelter Kit"; break;
                case 5: requestType = "Hygiene Kit"; break;
                case 6: requestType = "Drinking Water"; break;
            }

            string requestDescription = promptNonEmptyStringOrCancel("Enter Request Description (or '0' to cancel): ");
            if (requestDescription.empty()) {
                printInfo("Relief request submission cancelled.");
                pauseConsole("Press Enter to return to queue menu...");
                continue;
            }

            bool isEmergency = false;
            if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_RED + "Is this an Emergency?" + Color::RESET, isEmergency)) {
                printInfo("Relief request submission cancelled.");
                pauseConsole("Press Enter to return to queue menu...");
                continue;
            }

            bool hasMitigatingCircumstances = false;
            if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_YELLOW + "Has Mitigating Circumstances?" + Color::RESET, hasMitigatingCircumstances)) {
                printInfo("Relief request submission cancelled.");
                pauseConsole("Press Enter to return to queue menu...");
                continue;
            }

            // Priority calculation logic directly from pseudocode
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

            string requestId = formatCode("REQ-", nextRequestNumber, 4);

            // Store request
            ReliefRequest newRequest;
            newRequest.requestId = requestId;
            newRequest.familyCode = familyCode;
            newRequest.requestType = requestType;
            newRequest.requestDescription = requestDescription;
            newRequest.isEmergency = isEmergency;
            newRequest.hasMitigatingCircumstances = hasMitigatingCircumstances;
            newRequest.priorityLevel = priorityLevel;
            newRequest.requestStatus = "PENDING";

            requests[requestCount] = newRequest;
            requestCount++;
            nextRequestNumber++;

            cout << "\n";
            printSuccess("Relief request submitted successfully.");
            cout << "  Request ID       : " << colorCode(requestId) << "\n";
            cout << "  Assigned Priority: " << badgePriority(priorityLevel) << "\n";
            cout << "  Status           : " << badgeStatus("PENDING") << "\n";
            pauseConsole("Press Enter to continue...");
        } else if (requestChoice == 2) {
            if (requestCount == 0) {
                printWarning("No relief requests currently in queue.");
                pauseConsole("Press Enter to continue...");
                continue;
            }

            // Recursive sort
            recursiveSortRequestsByPriority(requests, requestCount);

            cout << Color::BOLD << Color::BRIGHT_CYAN << "--- ESCALATION QUEUE (SORTED BY HIGHEST URGENCY) ---\n" << Color::RESET;
            printDivider(95, '=');
            cout << "  " << left << setw(12) << "Request ID"
                 << setw(12) << "Family Code"
                 << setw(20) << "Request Type"
                 << "  " << left << setw(18) << "Priority"
                 << " " << left << setw(16) << "Status"
                 << " " << "Description\n";
            printDivider(95, '-');

            for (int i = 0; i < requestCount; ++i) {
                displayRecord(requests[i]);
            }
            printDivider(95, '=');
            pauseConsole("Press Enter to continue...");
        } else if (requestChoice == 3) {
            if (requestCount == 0) {
                printWarning("No relief requests currently in queue.");
                pauseConsole("Press Enter to continue...");
                continue;
            }

            int reqIndex = -1;
            string reqId;
            bool cancelled = false;
            while (true) {
                reqId = promptNormalizedCodeOrCancel("Enter Request ID number to Update (e.g., 1 or 0001, or '0' / 'exit' to cancel): ", "REQ-", 4);
                if (reqId.empty()) {
                    printInfo("Operation cancelled.");
                    cancelled = true;
                    break;
                }

                reqIndex = recursiveSearchRequest(requests, requestCount, reqId);
                if (reqIndex == -1) {
                    printError("Request [" + reqId + "] not found. Please try again (or enter '0' to cancel).");
                    continue;
                }

                cout << "\n";
                printInfo("Found Request: [" + requests[reqIndex].requestId + "] for Family " + requests[reqIndex].familyCode);
                cout << "  Current Status: " << badgeStatus(requests[reqIndex].requestStatus) << "\n";

                int famIdx = recursiveSearchFamily(families, familyCount, requests[reqIndex].familyCode);
                if (famIdx != -1) {
                    cout << "\n" << Color::BOLD << Color::BRIGHT_CYAN << "--- ASSOCIATED FAMILY RECORD ---\n" << Color::RESET;
                    displayRecord(families[famIdx]);
                    cout << "\n";
                } else {
                    printWarning("Associated family record [" + requests[reqIndex].familyCode + "] was not found in active records.");
                }

                bool isCorrectFamily = false;
                if (!promptYesNoOrCancel(Color::BOLD + Color::BRIGHT_YELLOW + "Is this the correct family?" + Color::RESET, isCorrectFamily)) {
                    printInfo("Operation cancelled.");
                    cancelled = true;
                    break;
                }
                if (!isCorrectFamily) {
                    printInfo("Please enter another Request ID number.\n");
                    continue;
                }

                break;
            }
            if (cancelled) {
                pauseConsole("Press Enter to return to queue menu...");
                continue;
            }

            cout << "\nSelect New Status:\n";
            cout << "  [1] PENDING\n";
            cout << "  [2] APPROVED\n";
            cout << "  [3] IN_PROGRESS\n";
            cout << "  [4] RESOLVED\n";
            cout << "  [5] REJECTED\n";
            cout << "  [0] Cancel (Keep Current Status)\n";
            cout << Color::BOLD << "Enter status option [0-5]: " << Color::RESET;

            int statusChoice;
            if (cin >> statusChoice) {
                clearInputBuffer();
                if (statusChoice == 0) {
                    printInfo("Update cancelled. Status unchanged.");
                    pauseConsole("Press Enter to continue...");
                    continue;
                }
                switch (statusChoice) {
                    case 1: requests[reqIndex].requestStatus = "PENDING"; break;
                    case 2: requests[reqIndex].requestStatus = "APPROVED"; break;
                    case 3: requests[reqIndex].requestStatus = "IN_PROGRESS"; break;
                    case 4: requests[reqIndex].requestStatus = "RESOLVED"; break;
                    case 5: requests[reqIndex].requestStatus = "REJECTED"; break;
                    default:
                        printError("Invalid status choice.");
                        pauseConsole("Press Enter to continue...");
                        continue;
                }
                printSuccess("Request status updated to " + requests[reqIndex].requestStatus);
            } else {
                clearInputBuffer();
                printError("Invalid input.");
            }
            pauseConsole("Press Enter to continue...");
        } else {
            printError("Invalid selection.");
            pauseConsole("Press Enter to continue...");
        }
    }
}
