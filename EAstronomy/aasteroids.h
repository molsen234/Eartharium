#pragma once

#include "acoordinates.h"

class AAsteroids {
public:
	// Diameters
	constexpr static double ApparentAsteroidDiameter(double Delta, double d) {
		// Delta = distance in AU
		// d = diameter in km
		// returns apparent diameter in radians
		return deg2rad * (0.0013788 * d) / (Delta * 3600.0);
	}
	static double AsteroidDiameter(double H, double A) noexcept;
};
