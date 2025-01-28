#pragma once

#include <cmath>

//#include "acoordinates.h"
struct LLD;

// Hold various 2-axis ellipsoids
struct Ellipsoid_2axis {
	double flattening{ 0.0 };
	double semimajor_axis{ 0.0 };
	double semiminor_axis{ 0.0 };
	double eccentricity{ 0.0 };  // store both flattening and eccentricity to avoid lots of sqrt() calls in user code
	// these can't be constructors as they take same argument types
	void fromSemimajorSemiminor(double semimajor, double semiminor) {
		semimajor_axis = semimajor;
		semiminor_axis = semiminor;
		// f = 1-(b/a) = a/a - b/a = (a-b)/a   Which is faster / more accurate?
		// flattening = 1.0 - (semiminor_axis / semimajor_axis);
		flattening = (semimajor_axis - semiminor_axis) / semimajor_axis;
		// e = sqrt( 2*f - f*f ) = sqrt( f*(2-f) )
		eccentricity = sqrt(flattening * (2.0 - flattening));
	}
	void fromSemimajorFlattening(double semimajor, double flattening) {
		semimajor_axis = semimajor;
		this->flattening = flattening;
		semiminor_axis = semimajor_axis * (1.0 - flattening);
		// e = sqrt( 2*f - f*f ) = sqrt( f*(2-f) )
		eccentricity = sqrt(flattening * (2.0 - flattening));
	}
	void fromSemimajorEccentricity(double semimajor, double eccentricity) {
		semimajor_axis = semimajor;
		this->eccentricity = eccentricity;
		flattening = 1.0 - sqrt(1.0 - eccentricity * eccentricity);
		semiminor_axis = semimajor_axis - (semimajor_axis * flattening); // b = a*(1-f) = a-(a*f)
	}
};

// Define some standard 2-axis ellipsoids commonly used in astronomy and navigation

// NOTE: There might be a better way to do this. Current C++ (incl. 23) does not allow sqrt() in constexpr
constexpr double Earth_IAU76_f{ 1.0 / 298.257 };
constexpr double Earth_IAU76_a{ 6378.140 };
constexpr double Earth_IAU76_b{ Earth_IAU76_a * (1 - Earth_IAU76_f) };
constexpr double Earth_IAU76_e{ 0.08181922145552321 };  // = sqrt(2.0 * Earth_IAU76_f - Earth_IAU76_f * Earth_IAU76_f);
//static Ellipsoid_2axis Earth_IAU76{ Earth_IAU76_f, Earth_IAU76_a , Earth_IAU76_b, Earth_IAU76_e };

constexpr double Earth_WGS84_f{ 1.0 / 298.257223563 };
constexpr double Earth_WGS84_a{ 6378.137 };
constexpr double Earth_WGS84_b{ Earth_WGS84_a * (1 - Earth_WGS84_f) };
constexpr double Earth_WGS84_e{ 0.08181919084262149 };  // = sqrt(2.0 * Earth_WGS84_f - Earth_WGS84_f * Earth_WGS84_f);
//static Ellipsoid_2axis Earth_WGS84{ Earth_WGS84_f, Earth_WGS84_a , Earth_WGS84_b, Earth_WGS84_e };


//static struct EARTH_IAU76 : public Ellipsoid_2axis {  // IAU 1976
//	double flattening{ 1.0 / 298.257 };
//	double major_axis{ 6378.140 };
//	double minor_axis{ major_axis * (1 - flattening) };
//	//double eccentricity{ sqrt(2 * flattening - flattening * flattening) };
//	double eccentricity{ sqrt(flattening * (2.0 - flattening)) };
//};
//static struct EARTH_WGS84 : public Ellipsoid_2axis {  // WGS-84
//	double flattening{ 1.0 / 298.257223563 };
//	double major_axis{ 6378.137 };
//	double minor_axis{ major_axis * (1 - flattening) };
//	//double eccentricity{ sqrt(2 * flattening - flattening * flattening) };
//	double eccentricity{ sqrt(flattening * (2.0 - flattening)) };
//};
//static struct EARTH_IERS89 : public Ellipsoid_2axis {  // IERS 1989
//	double flattening{ 1.0 / 298.257 };
//	double major_axis{ 6378.136 };
//	double minor_axis{ major_axis * (1 - flattening) };
//	//double eccentricity{ sqrt(2 * flattening - flattening * flattening) };
//	double eccentricity{ sqrt(flattening * (2.0 - flattening)) };
//};
//static struct EARTH_IERS03 : public Ellipsoid_2axis {  // IERS 2003
//	double flattening{ 1.0 / 298.25642 };
//	double major_axis{ 6378.1366 };
//	double minor_axis{ major_axis * (1 - flattening) };
//	//double eccentricity{ sqrt(2 * flattening - flattening * flattening) };
//	double eccentricity{ sqrt(flattening * (2.0 - flattening)) };
//};
// Mars https://www.isprs.org/proceedings/xxxiv/part4/pdfpapers/521.pdf
//      https://nssdc.gsfc.nasa.gov/planetary/factsheet/marsfact.html
// 

class AEllipsoid {
public:
	// Figure of 2-axis ellipsoid - Adapted from MEEUS98 Chapter 11
	static LLD Planetocentric2Planetographic(LLD planetocentric, Ellipsoid_2axis& ellipsoid);
	static LLD Planetographic2Planetocentric(LLD planetographic, Ellipsoid_2axis& ellipsoid);
	static double RhoSinPhiPrime(double GeographicalLatitude, double Height, Ellipsoid_2axis& ellipsoid) noexcept;
	static double RhoCosPhiPrime(double GeographicalLatitude, double Height, Ellipsoid_2axis& ellipsoid) noexcept;
	static double RadiusOfParallelOfLatitude(double GeographicalLatitude, Ellipsoid_2axis& ellipsoid) noexcept;
	static double RadiusOfCurvature(double GeographicalLatitude, Ellipsoid_2axis& ellipsoid) noexcept;
	static double RadiusOfPrimeVertical(double GeographicalLatitude, Ellipsoid_2axis ellipsoid);
	static double DistanceBetweenPoints(double GeographicalLatitude1, double GeographicalLongitude1, double GeographicalLatitude2, double GeographicalLongitude2, Ellipsoid_2axis& ellipsoid) noexcept;
	//static double GeographicLatitude2rho(double geolat);

	static Ellipsoid_2axis Earth_IAU76;
	static Ellipsoid_2axis Earth_WGS84;
};
