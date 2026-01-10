#pragma once

struct LLD;

class APluto {
public:
	static double EclipticLongitude(double jd_tt) noexcept;
	static double EclipticLatitude(double jd_tt) noexcept;
	static double RadiusVector(double jd_tt) noexcept;
	static LLD EclipticCoordinates(double jd_tt); //, Ephemeris eph = VSOP87_FULL);

	// Illumination
	static double MagnitudeAA(double r, double Delta) noexcept;

	// Diameter - From AA+ v2.55 CAADiameters
	constexpr static double PlutoSemidiameterB(double Delta) noexcept;

};