#pragma once
#include "acoordinates.h"

class AProperMotion {
	// MEEUS98 Chapter 21
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

class AMagnitudes {
public:
	// AA+ v2.49
	static double CombinedMagnitude(double m1, double m2) noexcept;
	static double CombinedMagnitude(int Magnitudes, const double* pMagnitudes) noexcept;
	static double BrightnessRatio(double m1, double m2) noexcept;
	static double MagnitudeDifference(double brightnessRatio) noexcept;
};

// Stellar parallax
// See: https://sceweb.sce.uhcl.edu/helm/WEB-Positional%20Astronomy/Tutorial/Annual%20parallax/Annualparallax.html
// NOTE: Accoring to Meeus98 Chapter 23, stellar parallax never exceeds 0.8 arc seconds, and may be ignored in most cases.
//       Only 13 stars have annual parallax exceeding 0.25 arc seconds, and they are not close to the ecliptic, so they
//       do not participate in occultations or planetary conjunctions (where the effect does matter):
//       Alf Cen, Lalande 21185, Sirius, Eps Eri, 61 Cyg, Procyon, Eps Indi, Sigma2398, Groombridge 34, Tau Ceti,
//       Lacaille 9352, Cordoba 29191 and Kapteyn's star.
//       Thus Meeus does not give a calculation of the stars positional shift due to parallax.