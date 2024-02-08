#pragma once

class AEarth {
public:
    //Static methods
    static double EclipticLongitude(double JD, bool bHighPrecision) noexcept;
    static double EclipticLatitude(double JD, bool bHighPrecision) noexcept;
    static double RadiusVector(double JD, bool bHighPrecision) noexcept;

    static double SunMeanAnomaly(double JD) noexcept;
    static double Eccentricity(double JD) noexcept {
        const double T{ (JD - 2451545) / 36525 };
        return 1 - (0.002516 * T) - (0.0000074 * T * T);
    }

    static double EclipticLongitudeJ2000(double JD, bool bHighPrecision) noexcept;
    static double EclipticLatitudeJ2000(double JD, bool bHighPrecision) noexcept;
};
