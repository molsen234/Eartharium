
#include <cmath>

#include "../config.h"
#include "vsop87.h"
#include "../acoordinates.h"

double VSOP87::Calculate(double jd_tt, const VSOP87Coefficient2* pTable, size_t nTableSize, bool bAngle) noexcept {
    double Result{ 0.0 };
    const double T{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    double TTerm{ T };
    for (size_t i{ 0 }; i < nTableSize; i++) {
        double TempResult{ 0 };
        for (size_t j{ 0 }; j < pTable[i].nCoefficientsSize; j++)
            TempResult += (pTable[i].pCoefficients[j].A * cos(pTable[i].pCoefficients[j].B + (pTable[i].pCoefficients[j].C * T)));
        if (i) {
            TempResult *= TTerm;
            TTerm *= T;
        }
        Result += TempResult;
    }
    if (bAngle) Result = ACoord::rangezero2tau(Result);
    return Result;
}
double VSOP87::Calculate_Dash(double jd_tt, const VSOP87Coefficient2* pTable, size_t nTableSize) noexcept {
    double Result{ 0.0 };
    const double T{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    double TTerm1{ 1 };
    double TTerm2{ T };
    for (size_t i{ 0 }; i < nTableSize; i++) {
        double tempPart1{ 0 };
        double tempPart2{ 0 };
        for (size_t j{ 0 }; j < pTable[i].nCoefficientsSize; j++) {
            const double B_CT{ pTable[i].pCoefficients[j].B + (pTable[i].pCoefficients[j].C * T) };
            tempPart1 += (i * pTable[i].pCoefficients[j].A * cos(B_CT));
            tempPart2 += (pTable[i].pCoefficients[j].A * pTable[i].pCoefficients[j].C * sin(B_CT));
        }
        if (i) {
            tempPart1 *= TTerm1;
            tempPart2 *= TTerm2;
            TTerm1 *= T;
            TTerm2 *= T;
        }
        Result += (tempPart1 - tempPart2);
    }
    return Result / JD_MILLENNIUM;
}
