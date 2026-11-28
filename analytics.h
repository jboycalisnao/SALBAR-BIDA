#ifndef SALBAR_ANALYTICS_H
#define SALBAR_ANALYTICS_H

#include <string>
#include "models.h"

// Vulnerability Assessment (Unit 1, 2, 5: Computation and Logic)
int calculateVulnerabilityScore(const VulnerabilityDetails& details);
std::string determineVulnerabilityLevel(int score);
void displayVulnerabilityScoreBreakdown(const VulnerabilityDetails& details, int totalScore);

// Multi-dimensional Array Demographic Analysis (Unit 6: Multi-dimensional Arrays)
void generateDemographicMatrix(const Family families[], int familyCount, int matrix[VULN_LEVEL_ROWS][DEMO_COLUMNS]);
void displayDemographicMatrix(const int matrix[VULN_LEVEL_ROWS][DEMO_COLUMNS]);

// Case 6: Population Summary & Demographic Report
void handlePopulationSummary(const Family families[], int familyCount,
                             const EvacuationCenter centers[], int centerCount,
                             const ReliefRequest requests[], int requestCount);

#endif // SALBAR_ANALYTICS_H