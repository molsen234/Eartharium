#pragma once
#include "../config.h"
#include "aconfig.h"

class APluto {
public:
	static double EclipticLongitude(double jd_tt) noexcept;
	static double EclipticLatitude(double jd_tt) noexcept;
	static double RadiusVector(double jd_tt) noexcept;
	static LLD EclipticCoordinates(double jd_tt); //, Ephemeris eph = VSOP87_FULL);

};