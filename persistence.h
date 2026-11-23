#ifndef SALBAR_PERSISTENCE_H
#define SALBAR_PERSISTENCE_H

#include "models.h"

void saveData(const Family families[], int familyCount,
              const EvacuationCenter centers[], int centerCount,
              const ReliefRequest requests[], int requestCount,
              const Evacuee evacuees[], int evacueeCount,
              int nextFamilyNumber, int nextRequestNumber, int nextEvacueeNumber,
              int activeCenterIndex = 0);

void loadData(Family families[], int& familyCount,
              EvacuationCenter centers[], int& centerCount,
              ReliefRequest requests[], int& requestCount,
              Evacuee evacuees[], int& evacueeCount,
              int& nextFamilyNumber, int& nextRequestNumber, int& nextEvacueeNumber,
              int& activeCenterIndex);

#endif // SALBAR_PERSISTENCE_H
