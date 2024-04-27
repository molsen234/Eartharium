
#pragma once

#include "../config.h"

struct LLD {
public:
    double lat{ 0.0 };
    double lon{ 0.0 };
    double dst{ 0.0 };
    void print() const {
        std::cout << lat << "," << lon << ", " << dst << '\n';
    }
    std::string str() {
        char buff[100];
        snprintf(buff, sizeof(buff), "%03.9f,%03.9f,%03.9f", lat, lon, dst);
        std::string dstring = buff;
        return dstring;
    }
    std::string str_EQ() {  // Equatorial is Dec(deg), RA(hrs), dst(AU)
        char buff[100];
        //snprintf(buff, sizeof(buff), "%03.9f,%03.9f,%03.9f", rad2deg * lat, rad2hrs * lon, dst);
        snprintf(buff, sizeof(buff), "%03.14f,%03.14f,%03.14f", rad2deg * lat, rad2hrs * lon, dst);
        std::string dstring = buff;
        return dstring;
    }
    std::string str_EC() {  // Equatorial is Dec(deg), RA(hrs), dst(AU)
        char buff[100];
        //snprintf(buff, sizeof(buff), "%03.9f,%03.9f,%03.9f", rad2deg * lat, rad2deg * lon, dst);
        snprintf(buff, sizeof(buff), "%03.14f,%03.14f,%03.14f", rad2deg * lat, rad2deg * lon, dst);
        std::string dstring = buff;
        return dstring;
    }
    bool operator==(LLD& other) {
        return (lat == other.lat && lon == other.lon && dst == other.dst);
    }
    LLD& operator+=(LLD& other) {
        lat += other.lat;
        lon += other.lon;
        dst += other.dst;
        return *this;
    }
    LLD& operator+=(LLD other) {
        lat += other.lat;
        lon += other.lon;
        dst += other.dst;
        return *this;
    }
    friend std::ostream& operator<<(std::ostream& os, LLD& lld) {
        os << "Lat,Lon,Dst: " << lld.lat << "," << lld.lon << "," << lld.dst;
        return os;
    }
};

//using XYZ = glm::dvec3;
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
    static LLD Equatorial2Galactic(double Alpha, double Delta) noexcept;
    static LLD Galactic2Equatorial(double l, double b) noexcept;

    static glm::dvec3 Spherical2Rectangular(LLD spherical) {  // defined in .h to incourage inlining
        const double cosB = cos(spherical.lat);
        const double cosL = cos(spherical.lon);
        return { spherical.dst * cosB * cosL, spherical.dst * cosB * sin(spherical.lon), spherical.dst * sin(spherical.lat) };
    }
    static LLD Rectangular2Spherical(glm::dvec3 rectangular) {  // defined in .h to incourage inlining
        const double x2{ rectangular.x * rectangular.x };
        const double y2{ rectangular.y * rectangular.y };
        return { atan2(rectangular.z, sqrt(x2 + y2)),
                  ACoord::rangezero2tau(atan2(rectangular.y, rectangular.x)),
                  sqrt(x2 + y2 + (rectangular.z * rectangular.z)) };
    }
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
