
#pragma once

#include "../config.h"

class ACoord {
public:
	static double secs2deg(double seconds);
	static double rangezero2tau(double rad);
	static double rangemhalfpi2halfpi(double rad);
	static double rangemninety2ninety(double deg);
	static double rangezero2threesixty(double deg);
	static double rangemoneeighty2oneeighty(double deg);
	static double rangezero2twentyfour(double hrs);

	static double DMS2degrees(double deg, double min, double sec);
	// !!! FIX: Add the rest of the conversion functions (HMS, DM, HM, 2Radians, ...)

	constexpr static double DistanceToLightTime(double Distance) {
		return Distance * 0.0057755183;
	}
};

class AAngularSeparation { // !!! FIX: Move to Spherical ?
	static double Separation(double Alpha1, double Delta1, double Alpha2, double Delta2) noexcept;
	static double PositionAngle(double Alpha1, double Delta1, double Alpha2, double Delta2) noexcept;
	static double DistanceFromGreatArc(double Alpha1, double Delta1, double Alpha2, double Delta2, double Alpha3, double Delta3) noexcept;
	static double SmallestCircle(double Alpha1, double Delta1, double Alpha2, double Delta2, double Alpha3, double Delta3, bool& bType1) noexcept;
};

class Spherical {
	// !!! FIX: Maybe add the angle functions from CAAAngularSeparation etc.
public:

	static LLD Equatorial2Ecliptic(double Alpha, double Delta, double Epsilon, bool rad = false) noexcept;
	static LLD Equatorial2Ecliptic(LLD latlon, double Epsilon, bool rad = false) noexcept;
	static LLD Ecliptic2Equatorial(double Lambda, double Beta, double Epsilon, bool rad = false) noexcept;
	static LLD Ecliptic2Equatorial(LLD decra, double Epsilon, bool rad = false) noexcept;
	static LLD Equatorial2Horizontal(double LocalHourAngle, double Delta, double Latitude, bool rad = false) noexcept;
	static LLD Horizontal2Equatorial(double Azimuth, double Altitude, double Latitude, bool rad = false) noexcept;
	// There are also Galactic conversions in AACoordinateTransformation
};

class FK5 {
public:
	//Static methods
	static double CorrectionInLongitude(double lon, double lat, double jd_tt) noexcept;
	static double CorrectionInLatitude(double lon, double jd_tt) noexcept;
	static LLD CorrectionInLonLat(LLD latlon, double jd_tt) noexcept;

	static glm::dvec3 getVSOP2FK5_J2000(const glm::dvec3& value) noexcept;
	static glm::dvec3 getVSOP2FK5_B1950(const glm::dvec3& value) noexcept;
	static glm::dvec3 getVSOP2FK5_AnyEquinox(const glm::dvec3& value, double JDEquinox) noexcept;
};
