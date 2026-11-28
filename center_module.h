#ifndef SALBAR_CENTER_MODULE_H
#define SALBAR_CENTER_MODULE_H

#include "models.h"

// ========================================================================================
// EVACUATION CENTER MANAGEMENT MODULE (Case 3)
// ========================================================================================
void handleEvacuationCenterManagement(EvacuationCenter centers[], int& centerCount, int& activeCenterIndex,
                                     Evacuee evacuees[], int& evacueeCount,
                                     Family families[], int familyCount);

#endif // SALBAR_CENTER_MODULE_H
