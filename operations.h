#ifndef SALBAR_OPERATIONS_H
#define SALBAR_OPERATIONS_H

#include <string>
#include "models.h"

// ========================================================================================
// FUNCTION OVERLOADING (Unit 5: Overloading Functions)
// ========================================================================================
void displayRecord(const Family& family, bool detailed = false);
void displayRecord(const EvacuationCenter& center);
void displayRecord(const ReliefRequest& request);
void displayRecord(const Evacuee& evacuee);

// ========================================================================================
// SEARCHING WITH RECURSION (LO2 & LO3: Recursive Functions)
// ========================================================================================
int recursiveSearchFamily(const Family families[], int size, const std::string& code, int index = 0);
int recursiveSearchCenter(const EvacuationCenter centers[], int size, const std::string& name, int index = 0);
int recursiveSearchRequest(const ReliefRequest requests[], int size, const std::string& reqId, int index = 0);
int recursiveSearchEvacuee(const Evacuee evacuees[], int size, const std::string& id, int index = 0);

// ========================================================================================
// RECURSIVE AGGREGATIONS (Unit 5: Recursion & Scope)
// ========================================================================================
int recursiveCountHighVulnerability(const Family families[], int n);
int recursiveSumOccupancy(const EvacuationCenter centers[], int n);
int recursiveSumPopulation(const Family families[], int n);
int recursiveCountPendingRequests(const ReliefRequest requests[], int n);

// ========================================================================================
// RECURSIVE SORTING (Unit 6: Sorting & Unit 5: Recursion)
// ========================================================================================
void recursiveSortRequestsByPriority(ReliefRequest requests[], int n);

#endif // SALBAR_OPERATIONS_H
