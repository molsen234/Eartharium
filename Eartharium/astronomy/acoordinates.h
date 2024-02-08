
#pragma once

#include "../config.h"

class ACoord {
public:
	static double secs2deg(double seconds);
	static double rangezero2tau(double rad);
	static double rangemhalfpi2halfpi(double rad);
	static double rangezero2threesixty(double deg);
	static double rangemoneeighty2oneeighty(double deg);
	static double rangezero2twentyfour(double hrs);
};

class FK5 {
public:
	//Static methods
	static double CorrectionInLongitude(double lon, double lat, double jd_tt, bool rad = false) noexcept;
	static double CorrectionInLatitude(double lon, double jd_tt, bool rad = false) noexcept;
	static glm::dvec3 getVSOP2FK5_J2000(const glm::dvec3& value) noexcept;
	static glm::dvec3 getVSOP2FK5_B1950(const glm::dvec3& value) noexcept;
	static glm::dvec3 getVSOP2FK5_AnyEquinox(const glm::dvec3& value, double JDEquinox) noexcept;
};
