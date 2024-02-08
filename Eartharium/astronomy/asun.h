#pragma once

#include "../config.h"

class ASun {
public:
	static double GeometricEclipticLongitude(double jd_tt, bool hi) noexcept;
	static double GeometricEclipticLatitude(double jd_tt, bool hi) noexcept;

	static double GeometricEclipticLongitudeJ2000(double jd_tt) noexcept;
	static double GeometricEclipticLatitudeJ2000(double jd_tt) noexcept;

	static double GeometricFK5EclipticLongitude(double jd_tt) noexcept;
	static double GeometricFK5EclipticLatitude(double jd_tt) noexcept;

	static double ApparentEclipticLongitude(double jd_tt) noexcept;
	static double ApparentEclipticLatitude(double jd_tt) noexcept;

	static double VariationGeometricEclipticLongitude(double jd_tt, bool rad = false) noexcept;

	static glm::dvec3 EquatorialRectangularCoordinatesMeanEquinox(double jd_tt) noexcept;
	static glm::dvec3 EclipticRectangularCoordinatesJ2000(double jd_tt) noexcept;
	static glm::dvec3 EquatorialRectangularCoordinatesJ2000(double jd_tt) noexcept;
	static glm::dvec3 EquatorialRectangularCoordinatesB1950(double jd_tt) noexcept;
	static glm::dvec3 EquatorialRectangularCoordinatesAnyEquinox(double jd_tt, double JDEquinox) noexcept;

};