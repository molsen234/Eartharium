#pragma once

#include "config.h" // to get glm::dvec3
#include "aconfig.h"   // to get Ephemeris enum

class ASun {
public:
	static double GeometricEclipticLongitude(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
	static double GeometricEclipticLatitude(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;

	static double GeometricEclipticLongitudeJ2000(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
	static double GeometricEclipticLatitudeJ2000(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;

	static double GeometricFK5EclipticLongitude(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
	static double GeometricFK5EclipticLatitude(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;

	static double ApparentEclipticLongitude(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
	static double ApparentEclipticLatitude(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;

	static double VariationGeometricEclipticLongitude(double jd_tt) noexcept;

	static glm::dvec3 EquatorialRectangularCoordinatesMeanEquinox(double jd_tt) noexcept;
	static glm::dvec3 EclipticRectangularCoordinatesJ2000(double jd_tt) noexcept;
	static glm::dvec3 EquatorialRectangularCoordinatesJ2000(double jd_tt) noexcept;
	static glm::dvec3 EquatorialRectangularCoordinatesB1950(double jd_tt) noexcept;
	static glm::dvec3 EquatorialRectangularCoordinatesAnyEquinox(double jd_tt, double JDEquinox) noexcept;

	// VSOP87 - Ephemeris E - Rectangular Barycentric Ecliptic coordinates & velocities
	//          (The other Ephemeris series are Heliocentric)
	static double VSOP87_E_X(double jd_tt);
	static double VSOP87_E_Y(double jd_tt);
	static double VSOP87_E_Z(double jd_tt);
	static double VSOP87_E_dX(double jd_tt);
	static double VSOP87_E_dY(double jd_tt);
	static double VSOP87_E_dZ(double jd_tt);

	// From AA+ v2.55 CAADiameters
	constexpr static double SunSemidiameterA(double Delta) { return 3600.0 * deg2rad * 959.63 / Delta; }
};