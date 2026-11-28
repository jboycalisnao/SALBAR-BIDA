#ifndef SALBAR_RECORD_MODULE_H
#define SALBAR_RECORD_MODULE_H

#include "models.h"

// ========================================================================================
// RECORD MAINTENANCE MODULE: EDIT / DISABLE / DELETE (Case 5)
// ========================================================================================
void handleEditOrDeleteRecords(Family families[], int& familyCount,
                               Evacuee evacuees[], int& evacueeCount);

#endif // SALBAR_RECORD_MODULE_H
