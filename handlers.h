#ifndef SALBAR_HANDLERS_H
#define SALBAR_HANDLERS_H

#include "models.h"

// ========================================================================================
// MENU HANDLER FUNCTIONS (Modular Decomposition)
// ========================================================================================
void handleFamilyRegistration(Family families[], int& familyCount, int& nextFamilyNumber,
                              Evacuee evacuees[], int& evacueeCount, int& nextEvacueeNumber,
                              EvacuationCenter centers[], int centerCount, int activeCenterIndex);

void handleViewPopulation(const Family families[], int familyCount);

void handleEvacuationCenterManagement(EvacuationCenter centers[], int& centerCount, int& activeCenterIndex,
                                     Evacuee evacuees[], int& evacueeCount,
                                     Family families[], int familyCount);

void handlePendingRequestEscalation(ReliefRequest requests[], int& requestCount, int& nextRequestNumber,
                                    const Family families[], int familyCount);

void handleEditOrDeleteRecords(Family families[], int& familyCount,
                               Evacuee evacuees[], int& evacueeCount);

void handlePopulationSummary(const Family families[], int familyCount,
                             const EvacuationCenter centers[], int centerCount,
                             const ReliefRequest requests[], int requestCount);

void handleSaveAndExit(const Family families[], int familyCount,
                       const EvacuationCenter centers[], int centerCount,
                       const ReliefRequest requests[], int requestCount,
                       const Evacuee evacuees[], int evacueeCount,
                       int nextFamilyNumber, int nextRequestNumber, int nextEvacueeNumber,
                       int activeCenterIndex = 0);

#endif // SALBAR_HANDLERS_H
