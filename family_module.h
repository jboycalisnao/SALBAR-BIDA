#ifndef SALBAR_FAMILY_MODULE_H
#define SALBAR_FAMILY_MODULE_H

#include "models.h"

// ========================================================================================
// FAMILY REGISTRATION & POPULATION VIEW MODULE (Cases 1 & 2)
// ========================================================================================
void handleFamilyRegistration(Family families[], int& familyCount, int& nextFamilyNumber,
                              Evacuee evacuees[], int& evacueeCount, int& nextEvacueeNumber,
                              EvacuationCenter centers[], int centerCount, int activeCenterIndex);

void handleViewPopulation(const Family families[], int familyCount);

#endif // SALBAR_FAMILY_MODULE_H
