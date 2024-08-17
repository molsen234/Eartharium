#pragma once

//#include <cmath>

//#include "acoordinates.h"
struct LLD;

// Various 2-axis ellipsoids
struct Ellipsoid_2axis {
	double flattening;
	double semimajor_axis;
	double semiminor_axis;
	double eccentricity;
	// !!! FIX: Add member functions to configure custom ellipsoids from
	//          - major and minor axes
	//          - major axis and flattening
	//          (ctors will not work, as both have 2 doubles as arguments)
};

// NOTE: There might be a better way to do this. Current C++ (incl. 23) does not allow sqrt() in constexpr
constexpr double Earth_IAU76_f{ 1.0 / 298.257 };
constexpr double Earth_IAU76_a{ 6378.140 };
constexpr double Earth_IAU76_b{ Earth_IAU76_a * (1 - Earth_IAU76_f) };
constexpr double Earth_IAU76_e{ 0.08181922145552321 };  // = sqrt(2.0 * Earth_IAU76_f - Earth_IAU76_f * Earth_IAU76_f);
static Ellipsoid_2axis Earth_IAU76{ Earth_IAU76_f, Earth_IAU76_a , Earth_IAU76_b, Earth_IAU76_e };

constexpr double Earth_WGS84_f{ 1.0 / 298.257223563 };
constexpr double Earth_WGS84_a{ 6378.137 };
constexpr double Earth_WGS84_b{ Earth_WGS84_a * (1 - Earth_WGS84_f) };
constexpr double Earth_WGS84_e{ 0.08181919084262149 };  // = sqrt(2.0 * Earth_WGS84_f - Earth_WGS84_f * Earth_WGS84_f);
static Ellipsoid_2axis Earth_WGS84{ Earth_WGS84_f, Earth_WGS84_a , Earth_WGS84_b, Earth_WGS84_e };


//static struct EARTH_IAU76 : public Ellipsoid_2axis {  // IAU 1976
//	double flattening{ 1.0 / 298.257 };
//	double major_axis{ 6378.140 };
//	double minor_axis{ major_axis * (1 - flattening) };
//	double eccentricity{ sqrt(2 * flattening - flattening * flattening) };
//	// Something indicating the orientation of planetocentric and planetographic coordinates
//};
//static struct EARTH_WGS84 : public Ellipsoid_2axis {  // WGS-84
//	double flattening{ 1.0 / 298.257223563 };
//	double major_axis{ 6378.137 };
//	double minor_axis{ major_axis * (1 - flattening) };
//	double eccentricity{ sqrt(2 * flattening - flattening * flattening) };
//	// Something indicating the orientation of planetocentric and planetographic coordinates
//};
//static struct EARTH_IERS89 : public Ellipsoid_2axis {  // IERS 1989
//	double flattening{ 1.0 / 298.257 };
//	double major_axis{ 6378.136 };
//	double minor_axis{ major_axis * (1 - flattening) };
//	double eccentricity{ sqrt(2 * flattening - flattening * flattening) };
//	// Something indicating the orientation of planetocentric and planetographic coordinates
//};
//static struct EARTH_IERS03 : public Ellipsoid_2axis {  // IERS 2003
//	double flattening{ 1.0 / 298.25642 };
//	double major_axis{ 6378.1366 };
//	double minor_axis{ major_axis * (1 - flattening) };
//	double eccentricity{ sqrt(2 * flattening - flattening * flattening) };
//	// Something indicating the orientation of planetocentric and planetographic coordinates
//};
// Mars https://www.isprs.org/proceedings/xxxiv/part4/pdfpapers/521.pdf
//      https://nssdc.gsfc.nasa.gov/planetary/factsheet/marsfact.html
// 

class AEllipsoid {
public:
	// Figure of 2-axis ellipsoid - Adapted from MEEUS98 Chapter 11
	//static double RhoSinThetaPrime(double GeographicalLatitude, double Height) noexcept;
	//static double RhoCosThetaPrime(double GeographicalLatitude, double Height) noexcept;
	//static double RadiusOfParallelOfLatitude(double GeographicalLatitude) noexcept;
	//static double RadiusOfCurvature(double GeographicalLatitude) noexcept;
	//static double DistanceBetweenPoints(double GeographicalLatitude1, double GeographicalLongitude1, double GeographicalLatitude2, double GeographicalLongitude2) noexcept;
	//static double GeographicLatitude2rho(double geolat);
	static LLD Planetocentric2Planetographic(LLD planetocentric, Ellipsoid_2axis& ellipsoid);
	static LLD Planetographic2Planetocentric(LLD planetographic, Ellipsoid_2axis& ellipsoid);
};
