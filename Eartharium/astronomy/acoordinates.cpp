

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
double ACoord::rangemninety2ninety(double deg) { // snap degree value to -90.0 to 90.0 range
	double fResult = rangezero2threesixty(deg);
	if (fResult > 1.5 * 180.0)
		fResult = fResult - tau;
	else if (fResult > 180.0)
		fResult = 180.0 - fResult;
	else if (fResult > 90.0)
		fResult = 180.0 - fResult;
	return fResult;
}
double ACoord::rangezero2threesixty(double deg) { // snap degree value to 0 to 360 range
	double fResult = fmod(deg, 360.0);
	if (fResult < 0)
		fResult += 360.0;
	return fResult;
}
double ACoord::rangemoneeighty2oneeighty(double deg) { // snap degree value to -180 to 180 range
	double fResult = fmod(deg + 180.0, 360.0);
	if (fResult < 0)
		fResult += 360.0;
	return fResult - 180.0;
}
double ACoord::rangezero2twentyfour(double deg) { // snap hours value to 0 to 24 range
	double fResult = fmod(deg, 24.0);
	if (fResult < 0.0)
		fResult += 24.0;
	return fResult;
}

double ACoord::DMS2degrees(double deg, double min, double sec) {
	return deg + (min / 60.0) + (sec / 3600.0);
}

// Angular calculations

double AAngularSeparation::Separation(double Alpha1, double Delta1, double Alpha2, double Delta2) noexcept {
	// Angular distance between the two coordinates, all angles in radians
	// Meeus98 chapter 17
	// AA+: CAAAngularSeparation
	const double x{ (cos(Delta1) * sin(Delta2)) - (sin(Delta1) * cos(Delta2) * cos(Alpha2 - Alpha1)) };
	const double y{ cos(Delta2) * sin(Alpha2 - Alpha1) };
	const double z{ (sin(Delta1) * sin(Delta2)) + (cos(Delta1) * cos(Delta2) * cos(Alpha2 - Alpha1)) };
	double value{ atan2(sqrt((x * x) + (y * y)), z) };
	if (value < 0) value += pi;  // !!! FIX: Isn't this the same as ACoord::rangezero2tau() ?
	return value; // separation in radians
}
double AAngularSeparation::PositionAngle(double Alpha1, double Delta1, double Alpha2, double Delta2) noexcept {
	// The position angle is the direction in which to move, relative to the celestial north pole, to get from position 1 to position 2
	// It is akin to a bearing in terrestial navigation
	// All positions and angles are in radians
	// Meeus98 chapter 17
	// AA+: CAAAngularSeparation
	const double DeltaAlpha{ Alpha1 - Alpha2 };
	double value{ atan2(sin(DeltaAlpha), (cos(Delta2) * tan(Delta1)) - (sin(Delta2) * cos(DeltaAlpha))) };
	if (value < 0) value += pi;  // !!! FIX: Isn't this the same as ACoord::rangezero2tau() ?
	return value;
}
double AAngularSeparation::DistanceFromGreatArc(double Alpha1, double Delta1, double Alpha2, double Delta2, double Alpha3, double Delta3) noexcept {
	// Meeus98 chapter 19
	// AA+: CAAAngularSeparation
	const double X1{ cos(Delta1) * cos(Alpha1) };
	const double X2{ cos(Delta2) * cos(Alpha2) };
	const double Y1{ cos(Delta1) * sin(Alpha1) };
	const double Y2{ cos(Delta2) * sin(Alpha2) };
	const double Z1{ sin(Delta1) };
	const double Z2{ sin(Delta2) };

	const double A{ (Y1 * Z2) - (Z1 * Y2) };
	const double B{ (Z1 * X2) - (X1 * Z2) };
	const double C{ (X1 * Y2) - (Y1 * X2) };

	const double m{ tan(Alpha3) };
	const double n{ tan(Delta3) / cos(Alpha3) };

	double value{ asin((A + (B * m) + (C * n)) / (sqrt((A * A) + (B * B) + (C * C)) * sqrt(1 + (m * m) + (n * n)))) };
	if (value < 0) value = fabs(value);
	return value;
}
double AAngularSeparation::SmallestCircle(double Alpha1, double Delta1, double Alpha2, double Delta2, double Alpha3, double Delta3, bool& bType1) noexcept {
	// !!! FIX: It is inconsistent to pass parameter reference for return value. Consider if it is offending enough to create a custom return type
	// Meeus98 chapter 19
	// AA+: CAAAngularSeparation
	// There are two types of solutions to the smallest circle containing 3 points:
	// - Type 1: The smallest circle has a diameter equal to the longest side of the triangle, one point lies within the circle
	// - Type 2: The smallest circle has all 3 points on the perimeter, and is thus a circumscribed circle of the triangle
	const double d1{ Separation(Alpha1, Delta1, Alpha2, Delta2) };
	const double d2{ Separation(Alpha1, Delta1, Alpha3, Delta3) };
	const double d3{ Separation(Alpha2, Delta2, Alpha3, Delta3) };

	// Sort side lengths descending into a, b, c (i.e. make a the largest side, c the smallest)
	double a{ d1 };
	double b{ d2 };
	double c{ d3 };
	if (b > a) {
		a = d2;
		b = d1;
		c = d3;
	}
	if (c > a) {
		a = d3;
		b = d1;
		c = d2;
	}

	double value{ 0 };
	if (a > sqrt((b * b) + (c * c))) { // is angle A greater than a right angle?
		bType1 = true;
		value = a;
	}
	else {
		bType1 = false;
		value = (2 * a * b * c) / (sqrt((a + b + c) * (a + b - c) * (b + c - a) * (a + c - b)));
	}
	return value;
}

LLD Spherical::Equatorial2Ecliptic(double Alpha, double Delta, double Epsilon, bool rad) noexcept {
	double alpha{ rad ? Alpha : Alpha * hrs2rad };
	double delta{ rad ? Delta : Delta * deg2rad };
	double epsilon{ rad ? Epsilon : Epsilon * deg2rad };

	LLD Ecliptic{ 0.0, 0.0, 0.0 };
	const double cosEpsilon = cos(epsilon);
	const double sinEpsilon = sin(epsilon);
	const double sinAlpha = sin(alpha);
	Ecliptic.lon = atan2((sinAlpha * cosEpsilon) + (tan(delta) * sinEpsilon), cos(alpha));
	if (Ecliptic.lon < 0.0)
		Ecliptic.lon += tau;
	Ecliptic.lat = asin((sin(delta) * cosEpsilon) - (cos(delta) * sinEpsilon * sinAlpha));

	if (!rad) {
		Ecliptic.lon *= rad2deg;
		Ecliptic.lat *= rad2deg;
	}
	return Ecliptic;
}
LLD Spherical::Equatorial2Ecliptic(LLD decra, double Epsilon, bool rad) noexcept {
	double alpha{ rad ? decra.lon : decra.lon * hrs2rad };
	double delta{ rad ? decra.lat : decra.lat * deg2rad };
	double epsilon{ rad ? Epsilon : Epsilon * deg2rad };

	LLD Ecliptic{ 0.0, 0.0, 0.0 };
	const double cosEpsilon = cos(epsilon);
	const double sinEpsilon = sin(epsilon);
	const double sinAlpha = sin(alpha);
	Ecliptic.lon = atan2((sinAlpha * cosEpsilon) + (tan(delta) * sinEpsilon), cos(alpha));
	if (Ecliptic.lon < 0.0)
		Ecliptic.lon += tau;
	Ecliptic.lat = asin((sin(delta) * cosEpsilon) - (cos(delta) * sinEpsilon * sinAlpha));

	if (!rad) {
		Ecliptic.lon *= rad2deg;
		Ecliptic.lat *= rad2deg;
	}
	return Ecliptic;
}
LLD Spherical::Ecliptic2Equatorial(double Lambda, double Beta, double Epsilon, bool rad) noexcept {
	//std::cout << "Spherical::Ecliptic2Equatorial(" << Lambda << ", " << Beta << ", " << Epsilon << ", " << rad << ")\n";
	double lambda{ rad ? Lambda : Lambda * deg2rad };
	double beta{ rad ? Beta : Beta * deg2rad };
	double epsilon{ rad ? Epsilon : Epsilon * deg2rad };

	LLD Equatorial{ 0.0, 0.0, 0.0 };
	const double cosEpsilon = cos(epsilon);
	const double sinEpsilon = sin(epsilon);
	const double sinLambda = sin(lambda);
	Equatorial.lon = atan2((sinLambda * cosEpsilon) - (tan(beta) * sinEpsilon), cos(lambda));
	if (Equatorial.lon < 0.0)
		Equatorial.lon += tau;
	Equatorial.lat = asin((sin(beta) * cosEpsilon) + (cos(beta) * sinEpsilon * sinLambda));
	//std::cout << " - Equatorial: " << Equatorial << std::endl;
	if (!rad) {
		Equatorial.lon *= rad2hrs;
		Equatorial.lat *= rad2deg;
	}
	return Equatorial;
}
LLD Spherical::Ecliptic2Equatorial(LLD latlon, double Epsilon, bool rad) noexcept {
	//std::cout << "Spherical::Ecliptic2Equatorial(" << Lambda << ", " << Beta << ", " << Epsilon << ", " << rad << ")\n";
	double lambda{ rad ? latlon.lon : latlon.lat * deg2rad };
	double beta{ rad ? latlon.lat : latlon.lat * deg2rad };
	double epsilon{ rad ? Epsilon : Epsilon * deg2rad };

	LLD Equatorial{ 0.0, 0.0, 0.0 };
	const double cosEpsilon = cos(epsilon);
	const double sinEpsilon = sin(epsilon);
	const double sinLambda = sin(lambda);
	Equatorial.lon = atan2((sinLambda * cosEpsilon) - (tan(beta) * sinEpsilon), cos(lambda));
	if (Equatorial.lon < 0.0)
		Equatorial.lon += tau;
	Equatorial.lat = asin((sin(beta) * cosEpsilon) + (cos(beta) * sinEpsilon * sinLambda));
	//std::cout << " - Equatorial: " << Equatorial << std::endl;
	if (!rad) {
		Equatorial.lon *= rad2hrs;
		Equatorial.lat *= rad2deg;
	}
	return Equatorial;
}
LLD Spherical::Equatorial2Horizontal(double LocalHourAngle, double Delta, double Latitude, bool rad) noexcept {
	double lha{ rad ? LocalHourAngle : hrs2rad * LocalHourAngle };
	double delta{ rad ? Delta : Delta * deg2rad };
	double lat{ rad ? Latitude : Latitude * deg2rad };

	LLD Horizontal{ 0.0, 0.0, 0.0 };
	const double cosLatitude = cos(lat);
	const double cosLocalHourAngle = cos(lha);
	const double sinLatitude = sin(lat);
	Horizontal.lon = atan2(sin(lha), (cosLocalHourAngle * sinLatitude) - (tan(delta) * cosLatitude));
	if (Horizontal.lon < 0.0)
		Horizontal.lon += tau;
	Horizontal.lat = asin((sinLatitude * sin(delta)) + (cosLatitude * cos(delta) * cosLocalHourAngle));

	if (!rad) {
		Horizontal.lon *= rad2deg;
		Horizontal.lat *= rad2deg;
	}
	return Horizontal;
}
LLD Spherical::Horizontal2Equatorial(double Azimuth, double Altitude, double Latitude, bool rad) noexcept {
	double azi{ rad ? Azimuth : deg2rad * Azimuth };
	double alt{ rad ? Altitude : deg2rad * Altitude };
	double lat{ rad ? Latitude : deg2rad * Latitude };

	LLD Equatorial{ 0.0, 0.0, 0.0 };
	const double cosAzimuth = cos(azi);
	const double sinLatitude = sin(lat);
	const double cosLatitude = cos(lat);
	Equatorial.lon = atan2(sin(azi), (cosAzimuth * sinLatitude) + (tan(alt) * cosLatitude));
	if (Equatorial.lon < 0.0)
		Equatorial.lon += tau;
	Equatorial.lat = asin((sinLatitude * sin(alt)) - (cosLatitude * cos(alt) * cosAzimuth));

	if (!rad){
		Equatorial.lon *= rad2hrs;
		Equatorial.lat *= rad2deg;
	}
	return Equatorial;
}


double FK5::CorrectionInLongitude(double lon, double lat, double jd_tt) noexcept {
	const double T{ (jd_tt - JD_2000) / JD_CENTURY };
	const double Ldash{ deg2rad * ((rad2deg * lon) - (1.397 * T) - (0.00031 * T * T)) };
	const double value{ -0.09033 + (0.03916 * (cos(Ldash) + sin(Ldash))) * tan(lat) };
	return deg2rad * value / 3600.0; // radians
}
double FK5::CorrectionInLatitude(double lon, double jd_tt) noexcept {
	const double T{ (jd_tt - JD_2000) / JD_CENTURY };
	const double Ldash{ deg2rad * ((rad2deg * lon) - (1.397 * T) - (0.00031 * T * T)) };
	const double value{ 0.03916 * (cos(Ldash) - sin(Ldash)) };
	return deg2rad * value / 3600.0; // radians
}
LLD FK5::CorrectionInLonLat(LLD latlon, double jd_tt) noexcept {
	const double T{ (jd_tt - JD_2000) / JD_CENTURY };
	const double Ldash{ deg2rad * ((rad2deg * latlon.lon) - (1.397 * T) - (0.00031 * T * T)) };
	return { deg2rad * (0.03916 * (cos(Ldash) - sin(Ldash))) / 3600.0,                                  //latitude correction
			deg2rad * (- 0.09033 + (0.03916 * (cos(Ldash) + sin(Ldash))) * tan(latlon.lat)) / 3600.0,   //longitude correction
			0.0};                                                                                       //no distance correction
}

glm::dvec3 FK5::getVSOP2FK5_J2000(const glm::dvec3& value) noexcept {
	glm::dvec3 result{
		value.x + (0.000000440360 * value.y) - (0.000000190919 * value.z),
		(-0.000000479966 * value.x) + (0.917482137087 * value.y) - (0.397776982902 * value.z),
		(0.397776982902 * value.y) + (0.917482137087 * value.z)
	};
	return result;
}
glm::dvec3 FK5::getVSOP2FK5_B1950(const glm::dvec3& value) noexcept {
	glm::dvec3 result{
		(0.999925702634 * value.x) + (0.012189716217 * value.y) + (0.000011134016 * value.z),
		(-0.011179418036 * value.x) + (0.917413998946 * value.y) - (0.397777041885 * value.z),
		(-0.004859003787 * value.x) + (0.397747363646 * value.y) + (0.917482111428 * value.z)
	};
	return result;
}
glm::dvec3 FK5::getVSOP2FK5_AnyEquinox(const glm::dvec3& value, double JDEquinox) noexcept {
	// takes geometric rectangular ecliptical coordinates to FK5 rectangular coordinates at given equinox
	const double t{ (JDEquinox - JD_2000) / JD_CENTURY };
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
