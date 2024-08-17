
#pragma once

#include "config.h"

struct LLD {
public:
    double lat{ 0.0 };
    double lon{ 0.0 };
    double dst{ 0.0 };
    // Uncomment these two constructors to test if there is any code defining an LLD with { val, val, val }
    // in preparation to swap lat and lon
    //LLD(double lat, double lon, double dst) = delete;
    //LLD() {}
    void print() const {
        std::cout << "lat=" << lat << ", lon=" << lon << ", dst=" << dst << '\n';
    }
    std::string str() const {
        char buff[100];
        snprintf(buff, sizeof(buff), "%03.14f,%03.14f,%03.14f", lat, lon, dst);
        std::string dstring = buff;
        return dstring;
    }
    std::string str_EQ() const {  // Equatorial is Dec(deg), RA(hrs), dst(AU)
        char buff[100];
        //snprintf(buff, sizeof(buff), "%03.9f,%03.9f,%03.9f", rad2deg * lat, rad2hrs * lon, dst);
        snprintf(buff, sizeof(buff), "%03.14f,%03.14f,%03.14f", rad2deg * lat, rad2hrs * lon, dst);
        std::string dstring = buff;
        return dstring;
    }
    std::string str_EC() const {  // Ecliptic is Lat(deg), Lon(deg), dst(AU)
        char buff[100];
        //snprintf(buff, sizeof(buff), "%03.9f,%03.9f,%03.9f", rad2deg * lat, rad2deg * lon, dst);
        snprintf(buff, sizeof(buff), "%03.14f,%03.14f,%03.14f", rad2deg * lat, rad2deg * lon, dst);
        std::string dstring = buff;
        return dstring;
    }
    bool operator==(LLD& other) {
        return (lat == other.lat && lon == other.lon && dst == other.dst);
    }
    LLD& operator+=(const LLD& other) {
        lat += other.lat;
        lon += other.lon;
        dst += other.dst;
        return *this;
    }
    //LLD& operator+=(LLD other) {
    //    lat += other.lat;
    //    lon += other.lon;
    //    dst += other.dst;
    //    return *this;
    //}
    LLD& operator*=(double other) {
        lat *= other;
        lon *= other;
        //dst *= other.dst;
        return *this;
    }
    LLD& operator*(double other) {
        lat *= other;
        lon *= other;
        //dst *= other.dst;
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
	static double rangezero2tau(double rad);
    static double rangempi2pi(double rad);
	static double rangemhalfpi2halfpi(double rad);
	static double rangemninety2ninety(double deg);
	static double rangezero2threesixty(double deg);
	static double rangemoneeighty2oneeighty(double deg);
	static double rangezero2twentyfour(double hrs);

	static double secs2deg(double seconds);
	// !!! FIX: Add the rest of the conversion functions (HMS, DM, HM, 2hrs, ...)
    static double dms2rad(double d, double m, double s);
    static double dms2deg(double d, double m, double s);
    static double hms2rad(double h, double m, double s);
    static double hms2deg(double h, double m, double s);

    // Friendly string formatters
    static std::string angle2DMSstring(double angle);
    static std::string angle2DMSstringPython(double angle);
    static std::string angle2uDMSstring(double angle);
    static std::string angle2uDMSstringPython(double angle);
    static std::string angle2uHMSstring(double angle);
    static std::string angle2DMstring(double angle);
    static std::string angle2DMstringPython(double angle);
    //static std::string angle2uDMstring(double angle, bool rad);
    static std::string formatLatLon(LLD latlon);
    static std::string formatLatLonPython(LLD latlon);
    static std::string formatLatLon2(double lat, double lon);
    static std::string formatDecRA(LLD decra);
    static std::string formatDecRAPython(LLD decra);
    static std::string formatDecRA2(double dec, double ra);
    //static std::string formatDecRADst(LLD decra);
    static std::string formatEleAz(LLD eleaz);
    static std::string formatEleAzPython(LLD eleaz);
    static std::string formatEleAz2(double ele, double az);
    //static std::string formatEleAzDst(LLD eleaz);

	constexpr static double DistanceToLightTime(double Distance) {
		return Distance * 0.0057755183;  // Convert distance in AU to light time in days.
	}
};

class AAngularSeparation { // !!! FIX: Move to Spherical ?
public:
    static double Separation(double Alpha1, double Delta1, double Alpha2, double Delta2) noexcept;
	static double PositionAngle(double Alpha1, double Delta1, double Alpha2, double Delta2) noexcept;
	static double DistanceFromGreatArc(double Alpha1, double Delta1, double Alpha2, double Delta2, double Alpha3, double Delta3) noexcept;
	static double SmallestCircle(double Alpha1, double Delta1, double Alpha2, double Delta2, double Alpha3, double Delta3, bool& bType1) noexcept;
};

class Spherical {
public:
	static LLD Equatorial2Ecliptic(LLD decra, double trueobliq) noexcept;
	static LLD Equatorial2Ecliptic2(double Alpha, double Delta, double Epsilon) noexcept;
	static LLD Ecliptic2Equatorial(LLD latlon, double trueobliq) noexcept;
	static LLD Ecliptic2Equatorial2(double Lambda, double Beta, double Epsilon) noexcept;
	static LLD Equatorial2Horizontal(double LocalHourAngle, double Delta, double Latitude) noexcept;
	static LLD Horizontal2Equatorial(double Azimuth, double Altitude, double Latitude) noexcept;
    static LLD Equatorial2Galactic(double Alpha, double Delta) noexcept;
    static LLD Galactic2Equatorial(double l, double b) noexcept;

    static glm::dvec3 Spherical2Rectangular(LLD spherical) {  // defined in .h to incourage inlining
        const double cosB = cos(spherical.lat);
        const double cosL = cos(spherical.lon);
        return { spherical.dst * cosB * cosL, spherical.dst * cosB * sin(spherical.lon), spherical.dst * sin(spherical.lat) };
    }
    static LLD Rectangular2Spherical(glm::dvec3 rectangular) {  // defined in .h to incourage inlining
        LLD retval{};
        const double x2{ rectangular.x * rectangular.x };
        const double y2{ rectangular.y * rectangular.y };
        retval.lat = atan2(rectangular.z, sqrt(x2 + y2));
        retval.lon = ACoord::rangezero2tau(atan2(rectangular.y, rectangular.x));
        retval.dst = sqrt(x2 + y2 + (rectangular.z * rectangular.z));
        return retval;
    }
};

class FK5 {
public:
	//Static methods
	static double CorrectionInLongitude(double lon, double lat, double jd_tt) noexcept;
	static double CorrectionInLatitude(double lon, double jd_tt) noexcept;
	static LLD CorrectionInLonLat(LLD latlon, double jd_tt) noexcept;

	static glm::dvec3 VSOP2FK5_J2000(const glm::dvec3& value) noexcept;
	static glm::dvec3 VSOP2FK5_B1950(const glm::dvec3& value) noexcept;
	static glm::dvec3 VSOP2FK5_AnyEquinox(const glm::dvec3& value, double JDEquinox) noexcept;
};

class AIllumination {
public:
    static double PhaseAngle(double r, double R, double Delta) noexcept;
    static double PhaseAngle2(double R, double R0, double B, double L, double L0, double Delta) noexcept;
    static double PhaseAngleRectangular(double x, double y, double z, double B, double L, double Delta) noexcept;
    static double IlluminatedFraction(double PhaseAngle) noexcept;
    static double IlluminatedFraction2(double r, double R, double Delta) noexcept;
};
