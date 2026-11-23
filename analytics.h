#ifndef SALBAR_ANALYTICS_H
#define SALBAR_ANALYTICS_H

#include <string>
#include "models.h"

// Vulnerability Assessment (Unit 1, 2, 5: Computation and Logic)
int calculateVulnerabilityScore(const VulnerabilityDetails& details);
std::string determineVulnerabilityLevel(int score);

// Multi-dimensional Array Demographic Analysis (Unit 6: Multi-dimensional Arrays)
void generateDemographicMatrix(const Family families[], int familyCount, int matrix[VULN_LEVEL_ROWS][DEMO_COLUMNS]);
void displayDemographicMatrix(const int matrix[VULN_LEVEL_ROWS][DEMO_COLUMNS]);

#endif // SALBAR_ANALYTICS_H