

#include <cmath>

#include "acoordinates.h"


double ACoord::secs2deg(double seconds) {
	return seconds / 3600.0;
}
double ACoord::rangezero2tau(double rad) { // snap radian value to 0 to 2pi range
	double fResult = fmod(rad, tau);
	if (fResult < 0)
		fResult += tau;
	return fResult;
}
double ACoord::rangemhalfpi2halfpi(double rad) { // snap radian value to -pi/2 to pi/2 range
	double fResult = rangezero2tau(rad);
	if (fResult > 1.5 * pi)
		fResult = fResult - tau;
	else if (fResult > pi)
		fResult = pi - fResult;
	else if (fResult > pi2)
		fResult = pi - fResult;
	return fResult;
}
double ACoord::rangezero2threesixty(double deg) { // snap degree value to 0 to 360 range
	double fResult = fmod(deg, 360.0);
	if (fResult < 0)
		fResult += 360.0;
	return fResult;
}
double ACoord::rangemoneeighty2oneeighty(double deg) { // snap degree value to 0 to 360 range
	double fResult = fmod(deg + 180.0, 360.0);
	if (fResult < 0)
		fResult += 360.0;
	return fResult - 180.0;
}

double ACoord::rangezero2twentyfour(double deg) { // snap hours value to 0 to 24 range
	double fResult = fmod(deg, 24.0);
	if (fResult < 0)
		fResult += 24.0;
	return fResult;
}

double FK5::CorrectionInLongitude(double lon, double lat, double jd_tt, bool rad) noexcept {
	if (!rad) lat *= deg2rad;
	if (rad) lat *= rad2deg;

	const double T{ (jd_tt - 2451545) / 36525 };
	const double Ldash{ deg2rad * (lon - (1.397 * T) - (0.00031 * T * T)) };
	const double value{ 3600.0 * ( -0.09033 + (0.03916 * (cos(Ldash) + sin(Ldash))) * tan(lat))};
	return rad ? deg2rad * value : value;
}

double FK5::CorrectionInLatitude(double lon, double jd_tt, bool rad) noexcept {
	if (rad) lon *= rad2deg;

	const double T{ (jd_tt - 2451545) / 36525 };
	double Ldash{ deg2rad * (lon - (1.397 * T) - (0.00031 * T * T)) };
	const double value{ 3600.0 * (0.03916 * (cos(Ldash) - sin(Ldash))) };
	return rad ? deg2rad * value : value;
}

glm::dvec3 FK5::getVSOP2FK5_J2000(const glm::dvec3& value) noexcept {
	glm::dvec3 result{
		value.x + (0.000000440360 * value.y) - (0.000000190919 * value.z),
		(-0.000000479966 * value.x) + (0.917482137087 * value.y) - (0.397776982902 * value.z),
		(0.397776982902 * value.y) + (0.917482137087 * value.z)
	};
	return result;
}

glm::dvec3 FK5::getVSOP2FK5_B1950(const glm::dvec3& value) noexcept
{
	glm::dvec3 result{
		(0.999925702634 * value.x) + (0.012189716217 * value.y) + (0.000011134016 * value.z),
		(-0.011179418036 * value.x) + (0.917413998946 * value.y) - (0.397777041885 * value.z),
		(-0.004859003787 * value.x) + (0.397747363646 * value.y) + (0.917482111428 * value.z)
	};
	return result;
}

glm::dvec3 FK5::getVSOP2FK5_AnyEquinox(const glm::dvec3& value, double JDEquinox) noexcept {
	// takes geometric rectangular ecliptical coordinates to FK5 rectangular coordinates at given equinox
	const double t{ (JDEquinox - 2451545) / 36525 };
	const double t2{ t * t };
	const double t3{ t2 * t };

	double sigma{ (2306.2181 * t) + (0.30188 * t2) + (0.017988 * t3) };
	sigma = deg2rad * 3600.0 * sigma;

	double zeta{ (2306.2181 * t) + (1.09468 * t2) + (0.018203 * t3) };
	zeta = deg2rad * 3600.0 * zeta;

	double phi{ (2004.3109 * t) - (0.42665 * t2) - (0.041833 * t3) };
	phi = deg2rad * 3600.0 * phi;

	const double cossigma{ cos(sigma) };
	const double coszeta{ cos(zeta) };
	const double cosphi{ cos(phi) };
	const double sinsigma{ sin(sigma) };
	const double sinzeta{ sin(zeta) };
	const double sinphi{ sin(phi) };

	const double xx{ (cossigma * coszeta * cosphi) - (sinsigma * sinzeta) };
	const double xy{ (sinsigma * coszeta) + (cossigma * sinzeta * cosphi) };
	const double xz{ cossigma * sinphi };
	const double yx{ (-cossigma * sinzeta) - (sinsigma * coszeta * cosphi) };
	const double yy{ (cossigma * coszeta) - (sinsigma * sinzeta * cosphi) };
	const double yz{ -sinsigma * sinphi };
	const double zx{ -coszeta * sinphi };
	const double zy{ -sinzeta * sinphi };
	const double zz{ cosphi };

	glm::dvec3 result{
		(xx * value.x) + (yx * value.y) + (zx * value.z),
		(xy * value.x) + (yy * value.y) + (zy * value.z),
		(xz * value.x) + (yz * value.y) + (zz * value.z)
	};
	return result;
}
