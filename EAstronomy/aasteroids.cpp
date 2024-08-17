
#include "aasteroids.h"
#include<cmath>

double AAsteroids::AsteroidDiameter(double H, double A) noexcept {
	// H = Absolute Magnitude of asteroid
	// A = Albedo of asteroid
	// Returns asteroid in kilometers
	// MEEUS98 chapter 55
	const double x{ 3.12 - (H / 5) - (0.217147 * log(A)) };
	return pow(10.0, x);
}
