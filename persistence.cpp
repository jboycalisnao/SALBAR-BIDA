#include "persistence.h"
#include <fstream>
#include <string>

using namespace std;

// ========================================================================================
// DATA PERSISTENCE IMPLEMENTATION (<fstream>)
// ========================================================================================

void saveData(const Family families[], int familyCount,
              const EvacuationCenter centers[], int centerCount,
              const ReliefRequest requests[], int requestCount,
              const Evacuee evacuees[], int evacueeCount,
              int nextFamilyNumber, int nextRequestNumber, int nextEvacueeNumber,
              int activeCenterIndex) {
    // 1. Save Config IDs and Active Center Index
    ofstream cfgOut(FILE_CONFIG.c_str());
    if (cfgOut.is_open()) {
        cfgOut << nextFamilyNumber << " " << nextRequestNumber << " " << nextEvacueeNumber << " " << activeCenterIndex << "\n";
        cfgOut.close();
    }

    // 2. Save FamilyList
    ofstream famOut(FILE_FAMILIES.c_str());
    if (famOut.is_open()) {
        famOut << familyCount << "\n";
        for (int i = 0; i < familyCount; ++i) {
            famOut << families[i].familyCode << "\n"
                   << families[i].headOfFamily << "\n"
                   << families[i].numberOfFamilyMembers << "\n"
                   << families[i].numberOfVulnerableMembers << "\n"
                   << families[i].vulnerabilityScore << "\n"
                   << families[i].vulnerabilityLevel << "\n"
                   << families[i].details.children << "\n"
                   << families[i].details.seniorCitizens << "\n"
                   << families[i].details.personsWithDisabilities << "\n"
                   << families[i].details.pregnantMembers << "\n"
                   << families[i].details.medicalNeeds << "\n"
                   << families[i].isEvacuated << "\n"
                   << families[i].isActive << "\n";
        }
        famOut.close();
    }

    // 3. Save EvacuationCenterList
    ofstream centOut(FILE_CENTERS.c_str());
    if (centOut.is_open()) {
        centOut << centerCount << "\n";
        for (int i = 0; i < centerCount; ++i) {
            centOut << centers[i].facilityName << "\n"
                    << centers[i].facilityType << "\n"
                    << centers[i].maximumCapacity << "\n"
                    << centers[i].currentOccupancy << "\n";
        }
        centOut.close();
    }

    // 4. Save RequestList
    ofstream reqOut(FILE_REQUESTS.c_str());
    if (reqOut.is_open()) {
        reqOut << requestCount << "\n";
        for (int i = 0; i < requestCount; ++i) {
            reqOut << requests[i].requestId << "\n"
                   << requests[i].familyCode << "\n"
                   << requests[i].requestType << "\n"
                   << requests[i].requestDescription << "\n"
                   << requests[i].isEmergency << "\n"
                   << requests[i].hasMitigatingCircumstances << "\n"
                   << requests[i].priorityLevel << "\n"
                   << requests[i].requestStatus << "\n";
        }
        reqOut.close();
    }

    // 5. Save EvacueeList
    ofstream evacOut(FILE_EVACUEES.c_str());
    if (evacOut.is_open()) {
        evacOut << evacueeCount << "\n";
        for (int i = 0; i < evacueeCount; ++i) {
            evacOut << evacuees[i].evacueeId << "\n"
                    << evacuees[i].familyCode << "\n"
                    << evacuees[i].fullName << "\n"
                    << evacuees[i].age << "\n"
                    << evacuees[i].assignedCenter << "\n"
                    << evacuees[i].specialNeeds << "\n";
        }
        evacOut.close();
    }
}

void loadData(Family families[], int& familyCount,
              EvacuationCenter centers[], int& centerCount,
              ReliefRequest requests[], int& requestCount,
              Evacuee evacuees[], int& evacueeCount,
              int& nextFamilyNumber, int& nextRequestNumber, int& nextEvacueeNumber,
              int& activeCenterIndex) {
    // 1. Load Config IDs and Active Center Index
    ifstream cfgIn(FILE_CONFIG.c_str());
    if (cfgIn.is_open()) {
        cfgIn >> nextFamilyNumber >> nextRequestNumber >> nextEvacueeNumber;
        if (!(cfgIn >> activeCenterIndex)) {
            activeCenterIndex = 0;
        }
        cfgIn.close();
    }

    // 2. Load FamilyList
    ifstream famIn(FILE_FAMILIES.c_str());
    if (famIn.is_open()) {
        if (!(famIn >> familyCount)) {
            familyCount = 0;
        }
        famIn.ignore();
        for (int i = 0; i < familyCount && i < MAX_FAMILIES; ++i) {
            getline(famIn, families[i].familyCode);
            getline(famIn, families[i].headOfFamily);
            famIn >> families[i].numberOfFamilyMembers;
            famIn >> families[i].numberOfVulnerableMembers;
            famIn >> families[i].vulnerabilityScore;
            famIn.ignore();
            getline(famIn, families[i].vulnerabilityLevel);
            famIn >> families[i].details.children;
            famIn >> families[i].details.seniorCitizens;
            famIn >> families[i].details.personsWithDisabilities;
            famIn >> families[i].details.pregnantMembers;
            famIn >> families[i].details.medicalNeeds;
            famIn >> families[i].isEvacuated;
            if (!(famIn >> families[i].isActive)) {
                families[i].isActive = true;
            }
            famIn.ignore();
        }
        famIn.close();
    }

    // 3. Load EvacuationCenterList
    ifstream centIn(FILE_CENTERS.c_str());
    if (centIn.is_open()) {
        if (!(centIn >> centerCount)) {
            centerCount = 0;
        }
        centIn.ignore();
        for (int i = 0; i < centerCount && i < MAX_CENTERS; ++i) {
            getline(centIn, centers[i].facilityName);
            getline(centIn, centers[i].facilityType);
            centIn >> centers[i].maximumCapacity;
            centIn >> centers[i].currentOccupancy;
            centIn.ignore();
        }
        centIn.close();
    }

    // 4. Load RequestList
    ifstream reqIn(FILE_REQUESTS.c_str());
    if (reqIn.is_open()) {
        if (!(reqIn >> requestCount)) {
            requestCount = 0;
        }
        reqIn.ignore();
        for (int i = 0; i < requestCount && i < MAX_REQUESTS; ++i) {
            getline(reqIn, requests[i].requestId);
            getline(reqIn, requests[i].familyCode);
            getline(reqIn, requests[i].requestType);
            getline(reqIn, requests[i].requestDescription);
            reqIn >> requests[i].isEmergency;
            reqIn >> requests[i].hasMitigatingCircumstances;
            reqIn >> requests[i].priorityLevel;
            reqIn.ignore();
            getline(reqIn, requests[i].requestStatus);
        }
        reqIn.close();
    }

    // 5. Load EvacueeList
    ifstream evacIn(FILE_EVACUEES.c_str());
    if (evacIn.is_open()) {
        if (!(evacIn >> evacueeCount)) {
            evacueeCount = 0;
        }
        evacIn.ignore();
        for (int i = 0; i < evacueeCount && i < MAX_EVACUEES; ++i) {
            getline(evacIn, evacuees[i].evacueeId);
            getline(evacIn, evacuees[i].familyCode);
            getline(evacIn, evacuees[i].fullName);
            evacIn >> evacuees[i].age;
            evacIn.ignore();
            getline(evacIn, evacuees[i].assignedCenter);
            getline(evacIn, evacuees[i].specialNeeds);
        }
        evacIn.close();
    }
}
