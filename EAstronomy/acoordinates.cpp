

#include <cmath>

#include "acoordinates.h"


double ACoord::rangezero2tau(double rad) { // snap radian value to 0 to 2pi range
	double fResult = fmod(rad, tau);
	if (fResult < 0)
		fResult += tau;
	return fResult;
}
double ACoord::rangempi2pi(double rad) { // snap radian value to -pi to pi range
	//double fResult = rangezero2tau(rad);
	//if (fResult > pi)
	//	fResult = pi - fResult;
	//return fResult;
	double fResult = fmod(rad + pi, tau);
	if (fResult < 0)
		fResult += tau;
	return fResult - pi;
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
		fResult = fResult - 360.0;
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

double ACoord::secs2deg(double seconds) {
	return seconds / 3600.0;
}
double ACoord::dms2rad(double d, double m, double s) {
	if (d >= 0.0) return deg2rad * (d + m / 60.0 + s / 3600.0);
	return deg2rad * (d - m / 60.0 - s / 3600.0);
}
double ACoord::dms2deg(double d, double m, double s) {
	if (d >= 0.0) return d + m / 60.0 + s / 3600.0;
	return d - m / 60.0 - s / 3600.0;
}
double ACoord::hms2rad(double h, double m, double s) {
	if (h >= 0.0) return hrs2rad * (h + m / 60.0 + s / 3600.0);
	return hrs2rad * (h - m / 60.0 - s / 3600.0);
}
double ACoord::hms2deg(double h, double m, double s) {
	if (h >= 0.0) return hrs2deg * (h + m / 60.0 + s / 3600.0);  // = h * 15 + m / 4.0 + s / 240.0
	return hrs2deg * (h - m / 60.0 - s / 3600.0);                // = h * 15 - m / 4.0 - s / 240.0
}

std::string ACoord::angle2DMSstring(double angle) {
	angle *= rad2deg;
	std::string dstring;
	double deg = abs(angle);
	double dg = floor(deg) * std::copysign(1.0, angle);
	double min = 60 * (deg - abs(dg));
	double mi = floor(min);
	double sec = 60 * (min - mi);
	int d = (int)dg;
	int m = (int)mi;
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%03d\xF8%02d\'%06.3f\"", angle < 0 ? "-" :"+", abs(d), m, sec);
	dstring = buff;
	//std::cout << " string=" << dstring << std::endl;
	return dstring;
}
std::string ACoord::angle2DMSstringPython(double angle) {
	angle *= rad2deg;
	std::string dstring;
	double deg = abs(angle);
	double dg = floor(deg) * std::copysign(1.0, angle);
	double min = 60 * (deg - abs(dg));
	double mi = floor(min);
	double sec = 60 * (min - mi);
	int d = (int)dg;
	int m = (int)mi;
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%03d\xC2\xB0%02d\'%06.3f\"", angle < 0 ? "-" : "+", d, m, sec);  // Python wants UTF-8 unicode
	dstring = buff;
	return dstring;
}
std::string ACoord::angle2uDMSstring(double angle) { // Unsigned version, used by latlonFormat() which will insert N/S or E/W instead of sign
	angle *= rad2deg;
	std::string dstring;
	double deg = abs(angle);
	double dg = floor(deg);
	double min = 60 * (deg - abs(dg));
	double mi = floor(min);
	double sec = 60 * (min - mi);
	int d = (int)dg;
	int m = (int)mi;
	char buff[100];
	snprintf(buff, sizeof(buff), "%03d\xF8%02d\'%06.3f\"", d, m, sec);
	dstring = buff;
	return dstring;
}
std::string ACoord::angle2uDMSstringPython(double angle) { // Unsigned version, used by latlonFormat() which will insert N/S or E/W instead of sign
	angle *= rad2deg;
	std::string dstring;
	double deg = abs(angle);
	double dg = floor(deg);
	double min = 60 * (deg - abs(dg));
	double mi = floor(min);
	double sec = 60 * (min - mi);
	int d = (int)dg;
	int m = (int)mi;
	char buff[100];
	snprintf(buff, sizeof(buff), "%03d\xC2\xB0%02d\'%06.3f\"", d, m, sec);
	dstring = buff;
	return dstring;
}
std::string ACoord::angle2uHMSstring(double angle) { // Hour angles are always given in [0;24], thus unsigned
	angle *= rad2hrs;
	//angle = ACoord::rangezero2twentyfour(angle);
	std::string dstring;
	double hrs = abs(angle);
	double hr = floor(hrs);  // Does not give correct result when angle is negative
	double min = 60 * (hrs - abs(hr));
	double mi = floor(min);
	double sec = 60 * (min - mi);
	int h = (int)hr;
	int m = (int)mi;
	char buff[100];
	snprintf(buff, sizeof(buff), " %03dh%02dm%06.3fs", h, m, sec);
	dstring = buff;
	return dstring;
}
std::string ACoord::angle2DMstring(double angle) {
	// In celestial navigation it is tradition to use XXX°xx.xxx' format
	// Since these are often deltas, use signed notation.
	angle *= rad2deg;
	std::string dstring;
	double deg = abs(angle);
	double dg = floor(deg) * std::copysign(1.0, angle);
	double min = 60 * (deg - abs(dg));
	//double mi = floor(min);
	//double sec = 60 * (min - mi);
	int d = (int)dg;
	//int m = (int)min;
	char buff[100];
	snprintf(buff, sizeof(buff), "%03d\xF8%06.3f\'", d, min);
	dstring = buff;
	return dstring;
}
std::string ACoord::angle2DMstringPython(double angle) {
	// In celestial navigation it is tradition to use XXX°xx.xxx' format
	// Since these are often deltas, use signed notation.
	angle *= rad2deg;
	std::string dstring;
	double deg = abs(angle);
	double dg = floor(deg) * std::copysign(1.0, angle);
	double min = 60 * (deg - abs(dg));
	//double mi = floor(min);
	//double sec = 60 * (min - mi);
	int d = (int)dg;
	//int m = (int)min;
	char buff[100];
	snprintf(buff, sizeof(buff), "%03d\xC2\xB0%06.3f\'", d, min);  // Python wants UTF-8
	dstring = buff;
	return dstring;
}

std::string ACoord::formatLatLon(LLD latlon) {
	std::string dstring;
	std::string slat = angle2uDMSstring(latlon.lat);  // Unsigned version of angle to DMS string
	std::string slon = angle2uDMSstring(latlon.lon);  // ditto, because we do N/S and E/W below
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%s , %s%s", latlon.lat >= 0.0 ? "N" : "S", slat.c_str(), latlon.lon >= 0.0 ? "E" : "W", slon.c_str());
	dstring = buff;
	return dstring;
}
std::string ACoord::formatLatLonPython(LLD latlon) {
	std::string dstring;
	std::string slat = angle2uDMSstringPython(latlon.lat);  // Unsigned version of angle to DMS string
	std::string slon = angle2uDMSstringPython(latlon.lon);  // ditto, because we do N/S and E/W below
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%s , %s%s", latlon.lat >= 0.0 ? "N" : "S", slat.c_str(), latlon.lon >= 0.0 ? "E" : "W", slon.c_str());
	dstring = buff;
	//std::cout << "ACoord::formatLatLonPython() string=" << dstring << std::endl;
	return dstring;
}
std::string ACoord::formatLatLon2(double lat, double lon) {
	std::string dstring;
	std::string slat = angle2uDMSstring(lat);  // Unsigned version of angle to DMS string
	std::string slon = angle2uDMSstring(lon);  // ditto, because we do N/S and E/W below
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%s , %s%s", lat >= 0.0 ? "N" : "S", slat.c_str(), lon >= 0.0 ? "E" : "W", slon.c_str());
	dstring = buff;
	return dstring;
}
std::string ACoord::formatDecRA(LLD decra) {
	std::string dstring;
	std::string sra = angle2uHMSstring(decra.lon);
	std::string sdec = angle2DMSstring(decra.lat);
	char buff[100];
	//snprintf(buff, sizeof(buff), "%s%s / %s", decra.lat > 0.0 ? "+" : "-", sdec.c_str(), sra.c_str());
	snprintf(buff, sizeof(buff), "%s / %s", sdec.c_str(), sra.c_str());
	dstring = buff;
	return dstring;
}
std::string ACoord::formatDecRAPython(LLD decra) {
	std::string dstring;
	std::string sra = angle2uHMSstring(decra.lon);
	std::string sdec = angle2DMSstringPython(decra.lat);
	char buff[100];
	//snprintf(buff, sizeof(buff), "%s%s / %s", decra.lat > 0.0 ? "+" : "-", sdec.c_str(), sra.c_str());
	snprintf(buff, sizeof(buff), "%s / %s", sdec.c_str(), sra.c_str());
	dstring = buff;
	return dstring;
}
std::string ACoord::formatDecRA2(double dec, double ra) {
	std::string dstring;
	std::string sra = angle2uHMSstring(ra);
	std::string sdec = angle2DMSstring(dec);
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%s / %s", dec > 0.0 ? "+" : "-", sdec.c_str(), sra.c_str());
	dstring = buff;
	return dstring;
}
std::string ACoord::formatEleAz(LLD eleaz) {
	std::string dstring;
	// Unsigned strings here, sign is added below
	std::string sele = angle2uDMSstring(eleaz.lat);  // Unsigned vwersion of angle to DMS string
	std::string saz = angle2uDMSstring(eleaz.lon);   // ditto
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%s / %s%s", eleaz.lat >= 0.0 ? "+" : "-", sele.c_str(), eleaz.lon >= 0.0 ? "+" : "-", saz.c_str());
	dstring = buff;
	return dstring;
}
std::string ACoord::formatEleAzPython(LLD eleaz) {
	std::string dstring;
	std::string sele = angle2uDMSstringPython(eleaz.lat);  // Unsigned vwersion of angle to DMS string
	std::string saz = angle2uDMSstringPython(eleaz.lon);   // ditto
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%s / %s%s", eleaz.lat >= 0.0 ? "+" : "-", sele.c_str(), eleaz.lon >= 0.0 ? "+" : "-", saz.c_str());
	dstring = buff;
	return dstring;
}
std::string ACoord::formatEleAz2(double ele, double az) {
	std::string dstring;
	std::string sele = angle2uDMSstring(ele);  // Unsigned vwersion of angle to DMS string
	std::string saz = angle2uDMSstring(az);  // ditto
	char buff[100];
	snprintf(buff, sizeof(buff), "%s%s / %s%s", ele >= 0.0 ? "+" : "-", sele.c_str(), az >= 0.0 ? "+" : "-", saz.c_str());
	dstring = buff;
	return dstring;
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

LLD Spherical::Equatorial2Ecliptic(LLD decra, double trueobliq) noexcept {
	// AA+: CAACoordinateTransformation::Equatorial2Ecliptic()
	// MEEUS98: This refers to algorithm 13.1 and 13.2 on page 93
	LLD retval{};
	const double cosEpsilon{ cos(trueobliq) };
	const double sinEpsilon{ sin(trueobliq) };
	const double sinAlpha{ sin(decra.lon) };
	retval.lat = asin((sin(decra.lat) * cosEpsilon) - (cos(decra.lat) * sinEpsilon * sinAlpha));
	retval.lon = ACoord::rangezero2tau(atan2((sinAlpha * cosEpsilon) + (tan(decra.lat) * sinEpsilon), cos(decra.lon)));
	retval.dst = decra.dst;  // same distance
	return retval;
}
LLD Spherical::Equatorial2Ecliptic2(double Alpha, double Delta, double Epsilon) noexcept {
	// AA+: CAACoordinateTransformation::Equatorial2Ecliptic()
	// MEEUS98: This refers to algorithm 13.1 and 13.2 on page 93
	LLD retval{};
	const double cosEpsilon{ cos(Epsilon) };
	const double sinEpsilon{ sin(Epsilon) };
	const double sinAlpha{ sin(Alpha) };
	retval.lat = asin((sin(Delta) * cosEpsilon) - (cos(Delta) * sinEpsilon * sinAlpha));
	retval.lon = ACoord::rangezero2tau(atan2((sinAlpha * cosEpsilon) + (tan(Delta) * sinEpsilon), cos(Alpha)));
	retval.dst = 0.0;
	return retval;
}
LLD Spherical::Ecliptic2Equatorial(LLD latlon, double trueobliq) noexcept {
	// AA+: CAACoordinateTransformation::Ecliptic2Equatorial()
	// MEEUS98: This refers to algorithm 13.3 and 13.4 on page 93
	LLD retval{};
	const double cosEpsilon{ cos(trueobliq) };
	const double sinEpsilon{ sin(trueobliq) };
	const double sinLambda{ sin(latlon.lon) };
	retval.lat = asin((sin(latlon.lat) * cosEpsilon) + (cos(latlon.lat) * sinEpsilon * sinLambda));
	retval.lon = ACoord::rangezero2tau(atan2((sinLambda * cosEpsilon) - (tan(latlon.lat) * sinEpsilon), cos(latlon.lon)));
	retval.dst = latlon.dst; // same distance
	return retval;
}
LLD Spherical::Ecliptic2Equatorial2(double Lambda, double Beta, double Epsilon) noexcept {
	// AA+: CAACoordinateTransformation::Ecliptic2Equatorial()
	// MEEUS98: This refers to algorithm 13.3 and 13.4 on page 93
	LLD retval{};
	const double cosEpsilon = cos(Epsilon);
	const double sinEpsilon = sin(Epsilon);
	const double sinLambda = sin(Lambda);
	retval.lat = asin((sin(Beta) * cosEpsilon) + (cos(Beta) * sinEpsilon * sinLambda));
	retval.lon = ACoord::rangezero2tau(atan2((sinLambda * cosEpsilon) - (tan(Beta) * sinEpsilon), cos(Lambda)));
	retval.dst = 0.0; // No distance
	return retval;
}
LLD Spherical::Equatorial2Horizontal(double LocalHourAngle, double Delta, double Latitude) noexcept {
	LLD retval{};
	const double cosLocalHourAngle = cos(LocalHourAngle);
	const double cosLatitude = cos(Latitude);
	const double sinLatitude = sin(Latitude);
	retval.lat = asin((sinLatitude * sin(Delta)) + (cosLatitude * cos(Delta) * cosLocalHourAngle));
	// Use this if reckoning Azimuth from the south
	//retval.lon = ACoord::rangezero2tau(atan2(sin(LocalHourAngle), (cosLocalHourAngle * sinLatitude) - (tan(Delta) * cosLatitude)));
	// Reckon Azimuth from the North (by adding pi before ranging):
	retval.lon = ACoord::rangezero2tau(pi + atan2(sin(LocalHourAngle), (cosLocalHourAngle * sinLatitude) - (tan(Delta) * cosLatitude)));
	retval.dst = 0.0;  // no distance
	return retval;
}
LLD Spherical::Horizontal2Equatorial(double Azimuth, double Altitude, double Latitude) noexcept {
	LLD retval{};
	// Provided Azimuth is reckoned from North, adjust to south:
	Azimuth -= pi;
	const double cosAzimuth = cos(Azimuth);
	const double sinLatitude = sin(Latitude);
	const double cosLatitude = cos(Latitude);
	retval.lat = asin((sinLatitude * sin(Altitude)) - (cosLatitude * cos(Altitude) * cosAzimuth));
	retval.lon = ACoord::rangezero2tau(atan2(sin(Azimuth), (cosAzimuth * sinLatitude) + (tan(Altitude) * cosLatitude)));
	retval.dst = 0.0;  // no distance
	return retval;
}
LLD Spherical::Equatorial2Galactic(double Alpha, double Delta) noexcept {
	// Meeus98 eqn 13.7
	Alpha = (deg2rad * 192.25) - Alpha;
	LLD Galactic;
	const double cosAlpha = cos(Alpha);
	const double sin274 = sin(deg2rad * 27.4);
	const double cos274 = cos(deg2rad * 27.4);
	Galactic.lon = atan2(sin(Alpha), (cosAlpha * sin274) - (tan(Delta) * cos274));
	Galactic.lon = (deg2rad * 303.0) - Galactic.lon;
	Galactic.lon = ACoord::rangezero2tau(Galactic.lon);
	Galactic.lat = asin((sin(Delta) * sin274) + (cos(Delta) * cos274 * cosAlpha));
	return Galactic;
}
LLD Spherical::Galactic2Equatorial(double l, double b) noexcept {
	// Meeus98 eqn 13.8
	l -= deg2rad * 123.0;
	LLD Equatorial;
	const double cosl = cos(l);
	const double sin274 = sin(deg2rad * 27.4);
	const double cos274 = cos(deg2rad * 27.4);
	Equatorial.lon = atan2(sin(l), (cosl * sin274) - (tan(b) * cos274));
	Equatorial.lon += deg2rad * 12.25;
	Equatorial.lon = ACoord::rangezero2tau(Equatorial.lon);
	Equatorial.lat = asin((sin(b) * sin274) + (cos(b) * cos274 * cosl));
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
	LLD fk5corr{};
	const double T{ (jd_tt - JD_2000) / JD_CENTURY };
	const double Ldash{ deg2rad * ((rad2deg * latlon.lon) - (1.397 * T) - (0.00031 * T * T)) };
	fk5corr.lat = deg2rad * (0.03916 * (cos(Ldash) - sin(Ldash))) / 3600.0;                                 //latitude correction
	fk5corr.lon = deg2rad * (-0.09033 + (0.03916 * (cos(Ldash) + sin(Ldash))) * tan(latlon.lat)) / 3600.0;  //longitude correction
	fk5corr.dst = 0.0;                                                                                      //no distance correction
	return fk5corr;
}

glm::dvec3 FK5::VSOP2FK5_J2000(const glm::dvec3& value) noexcept {
	glm::dvec3 result{
		value.x + (0.000000440360 * value.y) - (0.000000190919 * value.z),
		(-0.000000479966 * value.x) + (0.917482137087 * value.y) - (0.397776982902 * value.z),
		(0.397776982902 * value.y) + (0.917482137087 * value.z)
	};
	return result;
}
glm::dvec3 FK5::VSOP2FK5_B1950(const glm::dvec3& value) noexcept {
	glm::dvec3 result{
		(0.999925702634 * value.x) + (0.012189716217 * value.y) + (0.000011134016 * value.z),
		(-0.011179418036 * value.x) + (0.917413998946 * value.y) - (0.397777041885 * value.z),
		(-0.004859003787 * value.x) + (0.397747363646 * value.y) + (0.917482111428 * value.z)
	};
	return result;
}
glm::dvec3 FK5::VSOP2FK5_AnyEquinox(const glm::dvec3& value, double JDEquinox) noexcept {
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

// General Illumination
double AIllumination::PhaseAngle(double r, double R, double Delta) noexcept {
	return ACoord::rangezero2tau(acos(((r * r) + (Delta * Delta) - (R * R)) / (2 * r * Delta)));
}

double AIllumination::PhaseAngle2(double R, double R0, double B, double L, double L0, double Delta) noexcept {
	return ACoord::rangezero2tau(acos((R - (R0 * cos(B) * cos(L - L0))) / Delta));
}

double AIllumination::PhaseAngleRectangular(double x, double y, double z, double B, double L, double Delta) noexcept {
	const double cosB{ cos(B) };
	return ACoord::rangezero2tau(acos(((x * cosB * cos(L)) + (y * cosB * sin(L)) + (z * sin(B))) / Delta));
}

double AIllumination::IlluminatedFraction(double PhaseAngle) noexcept {
	return (1 + cos(PhaseAngle)) / 2;
}
double AIllumination::IlluminatedFraction2(double r, double R, double Delta) noexcept {
	return (((r + Delta) * (r + Delta) - (R * R)) / (4 * r * Delta));
}