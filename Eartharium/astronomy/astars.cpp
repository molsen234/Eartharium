
#include "astars.h"
#include "acoordinates.h"

LLD AProperMotion::AdjustPositionUsingUniformProperMotion(double t, double Alpha, double Delta, double PMAlpha, double PMDelta) noexcept {
	// This is from AA+ CAAPrecession::AdjustPositionUsingUniformProperMotion(), but NOTE: the parameter units are different!
	// Meeus: chapter 21
	// PM values are in mas/yr (milli arc seconds per year, with PMAlpha premultiplied by cos(PMDelta)
	// t is in years
	//  converted to use radians instead of hrs or deg and accept mas/yr instead of as/yr
	LLD value;
	value.lon = ACoord::rangezero2tau(Alpha + ((PMAlpha * t) / 3'600'000));
	value.lat = ACoord::rangezero2tau(Delta + ((PMDelta * t) / 3'600'000));
	return value;
}

LLD AProperMotion::applyProperMotion(LLD decra, LLD propermotion, double jd_tt, double jd_epoch) {
	// apply Proper Motion - decra is in radians, propermotion is in milliarcseconds per year, from stellarobject db.
	double elapsedyears = (jd_tt - jd_epoch) / 365.25;
	decra.lat += propermotion.lat * elapsedyears / 3'600'000.0;
	decra.lon += propermotion.lon * elapsedyears / 3'600'000.0;
	return decra;
}

LLD AProperMotion::AdjustPositionUsingMotionInSpace(double r, double DeltaR, double t, double Alpha, double Delta, double PMAlpha, double PMDelta) noexcept {
	// r = parsec, deltaR = km/s, Alpha & Delta = radians, PMAlpha & PMDelta = mas/yr, t = yr since epoch of star catalogue
	// From AA+ CAAPrecession::AdjustPositionUsingMotionInSpace()
	// In AA+ it was: r = distance in parsec, t in years from epoch, DeltaR in km/s, PMAlpha in seconds of time per year, PMDelta in seconds of arc per year, Alpha in hours, Delta in degrees

	//Convert DeltaR from km/s to Parsecs / Year
	DeltaR /= 977'792;

	//Convert from milliarcseconds to Radians / Year
	PMAlpha /= 206'265'000;

	//Convert from milliarcseconds to Radians / Year
	PMDelta /= 206'265'000;

	const double rDeltaR{ r * DeltaR };

	const double cosAlpha{ cos(Alpha) };
	const double sinAlpha{ sin(Alpha) };
	const double cosDelta{ cos(Delta) };

	double x{ r * cosDelta * cosAlpha };
	double y{ r * cosDelta * sinAlpha };
	double z{ r * sin(Delta) };

	const double DeltaX{ (x / rDeltaR) - (z * PMDelta * cosAlpha) - (y * PMAlpha) };
	const double DeltaY{ (y / rDeltaR) - (z * PMDelta * sinAlpha) + (x * PMAlpha) };
	const double DeltaZ{ (z / rDeltaR) + (r * PMDelta * cosDelta) };

	x += t * DeltaX;
	y += t * DeltaY;
	z += t * DeltaZ;

	LLD value;
	value.lon = ACoord::rangezero2tau(atan2(y, x));
	value.lat = atan2(z, sqrt((x * x) + (y * y)));
	return value; // ra = radians, dec = radians
}
LLD AProperMotion::applyMotionInSpace(LLD decra, LLD propermotion, double radius, double radialvelocity , double jd_tt, double jd_epoch) {

	// km/s to parsecs/yr
	radialvelocity /= 977'792;

	//Convert from mas/yr to radians/yr
	propermotion.lat /= 206'265'000;
	propermotion.lon /= 206'265'000;

	const double rDeltaR{ radius * radialvelocity };

	const double cosAlpha{ cos(decra.lon) };
	const double sinAlpha{ sin(decra.lon) };
	const double cosDelta{ cos(decra.lat) };

	double x{ radius * cosDelta * cosAlpha };
	double y{ radius * cosDelta * sinAlpha };
	double z{ radius * sin(decra.lat) };

	const double t{ jd_tt - jd_epoch };
	const double DeltaX{ (x / rDeltaR) - (z * propermotion.lat * cosAlpha) - (y * propermotion.lon) };
	const double DeltaY{ (y / rDeltaR) - (z * propermotion.lat * sinAlpha) + (x * propermotion.lon) };
	const double DeltaZ{ (z / rDeltaR) + (radius * propermotion.lat * cosDelta) };

	x += t * DeltaX;
	y += t * DeltaY;
	z += t * DeltaZ;

	LLD value;
	value.lon = ACoord::rangezero2tau(atan2(y, x));
	value.lat = atan2(z, sqrt((x * x) + (y * y)));
	return value; // ra = radians, dec = radians
}
LLD AProperMotion::EquatorialPMToEcliptic(double Alpha /*ra*/, double Delta /*dec*/, double Beta /*star ec_lat*/, double PMAlpha, double PMDelta, double Epsilon /* obliquity */) noexcept {
	// Alpha, Delta, Beta, Epsilon given in radians, PMAlpha, PMDelta given in mas/yr
	// (if PM is supplied in as/yr instead, the return value is simply in as/yr too)
	// PMAlpha is assumed to be premultiplied by cos(Delta), as is the case in modern catalogues
	const double sinEpsilon{ sin(Epsilon) };
	const double cosEpsilon{ cos(Epsilon) };
	const double cosAlpha{ cos(Alpha) };
	const double sinAlpha{ sin(Alpha) };
	const double cosDelta{ cos(Delta) };
	const double sinDelta{ sin(Delta) };
	const double cosBeta{ cos(Beta) };

	LLD value;
	value.lon = ((PMDelta * sinEpsilon * cosAlpha) + (PMAlpha * cosDelta * ((cosEpsilon * cosDelta) + (sinEpsilon * sinDelta * sinAlpha)))) / (cosBeta * cosBeta);
	value.lat = (PMDelta * ((cosEpsilon * cosDelta) + (sinEpsilon * sinDelta * sinAlpha)) - (PMAlpha * sinEpsilon * cosAlpha * cosDelta)) / cosBeta;
	return value;  //radians
}

// Precession

LLD APrecession::PrecessEquatorial(double Alpha, double Delta, double JD0, double JD) noexcept {
	const double T{ (JD0 - JD_2000) / JD_CENTURY };
	const double Tsquared{ T * T };
	const double t{ (JD - JD0) / JD_CENTURY };
	const double tsquared{ t * t };
	const double tcubed{ tsquared * t };

	const double cosDelta{ cos(Delta) };
	const double sinDelta{ sin(Delta) };
	
	const double sigma{ deg2rad * (ACoord::DMS2degrees(0, 0, ((2306.2181 + (1.39656 * T) - (0.000139 * Tsquared)) * t) + ((0.30188 - (0.000344 * T)) * tsquared) + (0.017998 * tcubed))) };
	const double zeta{ deg2rad * (ACoord::DMS2degrees(0, 0, ((2306.2181 + (1.39656 * T) - (0.000139 * Tsquared)) * t) + ((1.09468 + (0.000066 * T)) * tsquared) + (0.018203 * tcubed))) };
	const double phi{ deg2rad * (ACoord::DMS2degrees(0, 0, ((2004.3109 - (0.8533 * T) - (0.000217 * Tsquared)) * t) - ((0.42665 + (0.000217 * T)) * tsquared) - (0.041833 * tcubed))) };
	const double cosphi{ cos(phi) };
	const double sinphi{ sin(phi) };
	const double cosAlphaplussigma{ cos(Alpha + sigma) };
	const double A{ cosDelta * sin(Alpha + sigma) };
	const double B{ (cosphi * cosDelta * cosAlphaplussigma) - (sinphi * sinDelta) };
	const double C{ (sinphi * cosDelta * cosAlphaplussigma) + (cosphi * sinDelta) };

	LLD value;
	value.lon = ACoord::rangezero2tau(atan2(A, B) + zeta);
	value.lat = asin(C);
	return value;  // radians
}
LLD APrecession::PrecessEquatorialFK4(double Alpha, double Delta, double JD0, double JD) noexcept {
	// From AA+ CAAPrecession, modified to accept and return radians
	const double T{ (JD0 - JD_B1950) / JD_TROPICAL_CENTURY }; // 2415020.3135 = B1950.0, 36524.2199 = tropical century (Fk4 is defined in those terms)
	const double t{ (JD - JD0) / JD_TROPICAL_CENTURY };
	const double tsquared{ t * t };
	const double tcubed{ tsquared * t };

	const double cosDelta{ cos(Delta) };
	const double sinDelta{ sin(Delta) };

	const double sigma{ deg2rad * (ACoord::DMS2degrees(0, 0, ((2304.250 + (1.396 * T)) * t) + (0.302 * tsquared) + (0.018 * tcubed))) };
	const double zeta{ deg2rad * (ACoord::DMS2degrees(0, 0, (0.791 * tsquared) + (0.001 * tcubed))) + sigma };
	const double phi{ deg2rad * (ACoord::DMS2degrees(0, 0, ((2004.682 - (0.853 * T)) * t) - (0.426 * tsquared) - (0.042 * tcubed))) };
	const double cosphi{ cos(phi) };
	const double sinphi{ sin(phi) };
	const double cosAlphaplussigma{ cos(Alpha + sigma) };
	const double A{ cosDelta * sin(Alpha + sigma) };
	const double B{ (cosphi * cosDelta * cosAlphaplussigma) - (sinphi * sinDelta) };
	const double C{ (sinphi * cosDelta * cosAlphaplussigma) + (cosphi * sinDelta) };

	const double DeltaAlpha{ hrs2rad * ACoord::DMS2degrees(0, 0, 0.0775 + (0.0850 * T)) };  // actually in hours, HMS2Hours() would be identical to DMS2degrees()
	LLD value;
	value.lon = ACoord::rangezero2tau((atan2(A, B) + zeta) + DeltaAlpha);
	value.lat = asin(C);
	return value;  // radians
}
LLD APrecession::PrecessEcliptic(double Lambda, double Beta, double JD0, double JD) noexcept {
	const double T{ (JD0 - JD_2000) / JD_CENTURY };
	const double Tsquared{ T * T };
	const double t{ (JD - JD0) / JD_CENTURY };
	const double tsquared{ t * t };
	const double tcubed{ tsquared * t };

	const double cosBeta{ cos(Beta) };
	const double sinBeta{ sin(Beta) };

	const double eta{ deg2rad * (ACoord::DMS2degrees(0, 0, ((47.0029 - (0.06603 * T) + (0.000598 * Tsquared)) * t) + ((-0.03302 + (0.000598 * T)) * tsquared) + (0.00006 * tcubed))) };
	const double coseta{ cos(eta) };
	const double sineta{ sin(eta) };
	const double pi{ deg2rad * (ACoord::DMS2degrees(0, 0, (174.876384 * 3600) + (3289.4789 * T) + (0.60622 * Tsquared) - ((869.8089 + (0.50491 * T)) * t) + (0.03536 * tsquared))) };
	const double sinpiminusLambda{ sin(pi - Lambda) };
	const double p{ deg2rad * (ACoord::DMS2degrees(0, 0, ((5029.0966 + (2.22226 * T) - (0.000042 * Tsquared)) * t) + ((1.11113 - (0.000042 * T)) * tsquared) - (0.000006 * tcubed))) };
	const double A{ (coseta * cosBeta * sinpiminusLambda) - (sineta * sinBeta) };
	const double B{ cosBeta * cos(pi - Lambda) };
	const double C{ (coseta * sinBeta) + (sineta * cosBeta * sinpiminusLambda) };

	LLD value;
	value.lon = ACoord::rangezero2tau(p + pi - atan2(A, B));
	value.lat = asin(C);
	return value;  // radians
}

// Magnitudes

double AMagnitudes::CombinedMagnitude(double m1, double m2) noexcept {
	const double x{ 0.4 * (m2 - m1) };
	return m2 - (2.5 * log10(pow(10.0, x) + 1));
}
double AMagnitudes::CombinedMagnitude(int Magnitudes, const double* pMagnitudes) noexcept {
	// !!! FIX: Use a vector instead with a for each loop
	assert(pMagnitudes != nullptr);

	double value{ 0 };
	for (int i{ 0 }; i < Magnitudes; i++) {
		value += pow(10.0, -0.4 * pMagnitudes[i]);
	}
	return -2.5 * log10(value);
}
double AMagnitudes::BrightnessRatio(double m1, double m2) noexcept {
	const double x{ 0.4 * (m2 - m1) };
	return pow(10.0, x);
}
double AMagnitudes::MagnitudeDifference(double brightnessRatio) noexcept {
	return 2.5 * log10(brightnessRatio);
}



