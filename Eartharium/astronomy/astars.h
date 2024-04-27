#pragma once
#include "../config.h"
#include "acoordinates.h"

class AProperMotion {
public:
	// MDO
	static LLD applyProperMotion(LLD decra, LLD propermotion, double jd_tt, double jd_epoch = JD_2000);
	static LLD applyMotionInSpace(LLD decra, LLD propermotion, double radius, double radialvelocity, double jd_tt, double jd_epoch = JD_2000);
	// AA+ v2.49
	// Apply proper motion in mas/yr to equatorial position in radians
	static LLD AdjustPositionUsingUniformProperMotion(double t, double Alpha, double Delta, double PMAlpha, double PMDelta) noexcept;
	// Apply motion in space (proper motion, radial velocity, distance) to equatorial position in radians
	static LLD AdjustPositionUsingMotionInSpace(double r, double deltar, double t, double Alpha, double Delta, double PMAlpha, double PMDelta) noexcept;
	static LLD EquatorialPMToEcliptic(double Alpha, double Delta, double Beta, double PMAlpha, double PMDelta, double Epsilon) noexcept;
};
class APrecession {
public:
	// MDO
	static LLD PrecessEquatorialJ2000(LLD decra, double jd_tt);
	// Insert precession from: https://www.aanda.org/articles/aa/full_html/2011/10/aa17274-11/aa17274-11.html

	// Based directly on AA+ v2.49
	static LLD PrecessEquatorial(double Alpha, double Delta, double JD0, double JD) noexcept;
	static LLD PrecessEquatorialFK4(double Alpha, double Delta, double JD0, double JD) noexcept;
	static LLD PrecessEcliptic(double Lambda, double Beta, double JD0, double JD) noexcept;
};

class AMagnitudes {
public:
	// AA+ v2.49
	static double CombinedMagnitude(double m1, double m2) noexcept;
	static double CombinedMagnitude(int Magnitudes, const double* pMagnitudes) noexcept;
	static double BrightnessRatio(double m1, double m2) noexcept;
	static double MagnitudeDifference(double brightnessRatio) noexcept;
};

