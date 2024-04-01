#pragma once
#include "../config.h"

class AProperMotion {
	// Apply proper motion in mas/yr to equatorial position in radians
	static LLD AdjustPositionUsingUniformProperMotion(double t, double Alpha, double Delta, double PMAlpha, double PMDelta) noexcept;
	static LLD applyProperMotion(LLD decra, LLD propermotion, double jd_tt, double jd_epoch = JD_2000);
	// Apply motion in space (proper motion, radial velocity, distance) to equatorial position in radians
	static LLD AdjustPositionUsingMotionInSpace(double r, double deltar, double t, double Alpha, double Delta, double PMAlpha, double PMDelta) noexcept;
	static LLD applyMotionInSpace(LLD decra, LLD propermotion, double radius, double radialvelocity, double jd_tt, double jd_epoch = JD_2000);

	static LLD EquatorialPMToEcliptic(double Alpha, double Delta, double Beta, double PMAlpha, double PMDelta, double Epsilon) noexcept;
};
class APrecession {
public:
	static LLD PrecessEquatorial(double Alpha, double Delta, double JD0, double JD) noexcept;
	static LLD PrecessEquatorialFK4(double Alpha, double Delta, double JD0, double JD) noexcept;
	static LLD PrecessEcliptic(double Lambda, double Beta, double JD0, double JD) noexcept;
};

class AMagnitudes {
	static double CombinedMagnitude(double m1, double m2) noexcept;
	static double CombinedMagnitude(int Magnitudes, const double* pMagnitudes) noexcept;
	static double BrightnessRatio(double m1, double m2) noexcept;
	static double MagnitudeDifference(double brightnessRatio) noexcept;
};

