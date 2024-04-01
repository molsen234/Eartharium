
#include <cmath>

#include "../../config.h"
#include "vsop87.h"
#include "../acoordinates.h"

double VSOP87::Calculate(double jd_tt, const VSOP87Coefficient2* pTable, size_t nTableSize, bool bAngle) noexcept
{
    //Validate our parameters
    //assert(pTable);
    
    const double T{ (jd_tt - JD_2000) / 365250 };
    double TTerm{ T };
    double Result{ 0 };
    for (size_t i{ 0 }; i < nTableSize; i++)
    {
        double TempResult{ 0 };
        for (size_t j{ 0 }; j < pTable[i].nCoefficientsSize; j++)
//#ifdef _MSC_VER
//#pragma warning(suppress : 26489)
//#endif //#ifdef _MSC_VER
            TempResult += (pTable[i].pCoefficients[j].A * cos(pTable[i].pCoefficients[j].B + (pTable[i].pCoefficients[j].C * T)));
        if (i)
        {
            TempResult *= TTerm;
            TTerm *= T;
        }
        Result += TempResult;
    }

    if (bAngle)
        Result = ACoord::rangezero2tau(Result);

    return Result;
}

//#ifdef _MSC_VER
//#pragma warning(suppress : 26429)
//#endif //#ifdef _MSC_VER
double VSOP87::Calculate_Dash(double jd_tt, const VSOP87Coefficient2* pTable, size_t nTableSize) noexcept
{
    //Validate our parameters
    //assert(pTable);

    const double T{ (jd_tt - JD_2000) / 365250 };
    double TTerm1{ 1 };
    double TTerm2{ T };
    double Result{ 0 };
    for (size_t i{ 0 }; i < nTableSize; i++)
    {
        double tempPart1{ 0 };
        double tempPart2{ 0 };
        for (size_t j{ 0 }; j < pTable[i].nCoefficientsSize; j++)
        {
//#ifdef _MSC_VER
//#pragma warning(suppress : 26489)
//#endif //#ifdef _MSC_VER
            const double B_CT{ pTable[i].pCoefficients[j].B + (pTable[i].pCoefficients[j].C * T) };
//#ifdef _MSC_VER
//#pragma warning(suppress : 26489)
//#endif //#ifdef _MSC_VER
            tempPart1 += (i * pTable[i].pCoefficients[j].A * cos(B_CT));
//#ifdef _MSC_VER
//#pragma warning(suppress : 26489)
//#endif //#ifdef _MSC_VER
            tempPart2 += (pTable[i].pCoefficients[j].A * pTable[i].pCoefficients[j].C * sin(B_CT));
        }
        if (i)
        {
            tempPart1 *= TTerm1;
            tempPart2 *= TTerm2;
            TTerm1 *= T;
            TTerm2 *= T;
        }
        Result += (tempPart1 - tempPart2);
    }

    //The value returned is in per days
    return Result / 365250;
}
