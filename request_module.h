#ifndef SALBAR_REQUEST_MODULE_H
#define SALBAR_REQUEST_MODULE_H

#include "models.h"

// ========================================================================================
// RELIEF REQUEST & ESCALATION MODULE (Case 4)
// ========================================================================================
void handlePendingRequestEscalation(ReliefRequest requests[], int& requestCount, int& nextRequestNumber,
                                    const Family families[], int familyCount);

#endif // SALBAR_REQUEST_MODULE_H
