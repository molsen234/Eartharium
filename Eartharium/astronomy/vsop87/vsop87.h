#pragma once

struct VSOP87Coefficient
{
	double A;
	double B;
	double C;
};

struct VSOP87Coefficient2
{
	const VSOP87Coefficient* pCoefficients;
	size_t nCoefficientsSize;
};

class VSOP87 {
public:
	// Angles are always in radians
	// Distances are always in VSOP87 AU (9 meters shorter than current IAU AU)
	static double Calculate(double jd_tt, const VSOP87Coefficient2* pTable, size_t nTableSize, bool bAngle) noexcept;
	// Derivatives are always in above units per day
	static double Calculate_Dash(double jd_tt, const VSOP87Coefficient2* pTable, size_t nTableSize) noexcept;
};