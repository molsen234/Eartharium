#pragma once

#include "config.h"
#include "aconfig.h"
#include "acoordinates.h"

struct APhysicalMarsDetails {
    double DE{ 0 };    // planetocentric declination in degrees of the Earth
    double DS{ 0 };    // planetocentric declination in degrees of the Sun
    double w{ 0 };     // aerographic longitude in degrees of the central meridian as seen from Earth
    double P{ 0 };     // geocentric position angle of Mars' northern rotational pole in degrees
    double X{ 0 };     // position angle in degrees of the mid-point of the illuminated limb
    double k{ 0 };     // illuminated fraction of the planet's disk
    double q{ 0 };     // defect of illumination
    double d{ 0 };     // apparent diameter of Mars in arc seconds
    void print() const {
        std::cout << "Planetocentric declination of the Earth (DE):        " << DE * rad2deg << "\n";            // radians
        std::cout << "Planetocentric declination of the Sun (DS):          " << DS * rad2deg << "\n";            // radians
        std::cout << "Aerographic longitude of central meridian (w):       " << w * rad2deg << "\n";             // radians
        std::cout << "Geocentric position angle of Mars' N pole (P):       " << P * rad2deg << "\n";             // radians
        std::cout << "Position angle of mid-point of illuminated limb (X): " << X * rad2deg << "\n";             // radians
        std::cout << "Illuminated fraction of the planet's disk (k):       " << k << "\n";                       // fraction (0.0->1.0)
        std::cout << "Defect of illumination (q):                          " << q * rad2deg * 3600 << "\n";      // radians
        std::cout << "Apparent diameter of Mars (d):                       " << d * rad2deg * 3600 << std::endl; // radians
    }
};

class AMars {
public:
    static double EclipticLongitude(double jd_tt, Planetary_Ephemeris eph) noexcept;
    static double EclipticLatitude(double jd_tt, Planetary_Ephemeris eph) noexcept;
    static double EclipticDistance(double jd_tt, Planetary_Ephemeris eph) noexcept;
    static LLD EclipticCoordinates(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL);

    // VSOP87
    // ======
    // Fundamental Orbit Parameters
    static double A(double jd_tt) noexcept;
    static double L(double jd_tt) noexcept;
    static double K(double jd_tt) noexcept;
    static double H(double jd_tt) noexcept;
    static double Q(double jd_tt) noexcept;
    static double P(double jd_tt) noexcept;

    // Ephemeris A - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
    static double VSOP87_A_X(double jd_tt);
    static double VSOP87_A_Y(double jd_tt);
    static double VSOP87_A_Z(double jd_tt);
    static double VSOP87_A_dX(double jd_tt);
    static double VSOP87_A_dY(double jd_tt);
    static double VSOP87_A_dZ(double jd_tt);

    // Ephemeris B - Spherical Heliocentric Ecliptic at J2000.0
    static double VSOP87_B_Longitude(double jd_tt);
    static double VSOP87_B_Latitude(double jd_tt);
    static double VSOP87_B_Distance(double jd_tt);
    static double VSOP87_B_dLongitude(double jd_tt);
    static double VSOP87_B_dLatitude(double jd_tt);
    static double VSOP87_B_dDistance(double jd_tt);

    // Ephemeris C - Rectangular Heliocentric Ecliptic at Equinox of Date
    static double VSOP87_C_X(double jd_tt);
    static double VSOP87_C_Y(double jd_tt);
    static double VSOP87_C_Z(double jd_tt);
    static double VSOP87_C_dX(double jd_tt);
    static double VSOP87_C_dY(double jd_tt);
    static double VSOP87_C_dZ(double jd_tt);

    // Ephemeris D - Spherical Heliocentric Ecliptic at Equinox of Date
    static double VSOP87_D_Longitude(double jd_tt);
    static double VSOP87_D_Latitude(double jd_tt);
    static double VSOP87_D_Distance(double jd_tt);
    static double VSOP87_D_dLongitude(double jd_tt);
    static double VSOP87_D_dLatitude(double jd_tt);
    static double VSOP87_D_dDistance(double jd_tt);

    // Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
    static double VSOP87_E_X(double jd_tt);
    static double VSOP87_E_Y(double jd_tt);
    static double VSOP87_E_Z(double jd_tt);
    static double VSOP87_E_dX(double jd_tt);
    static double VSOP87_E_dY(double jd_tt);
    static double VSOP87_E_dZ(double jd_tt);

    // From CAAPhysicalMars
    static APhysicalMarsDetails calcPhysicalMarsDetails(double JD, Planetary_Ephemeris eph) noexcept;

    // Magnitude
    static double MagnitudeAA(double r, double Delta, double i) noexcept;
    static double MagnitudeMuller(double r, double Delta, double i) noexcept;

    // Diameters from AA+ v2.55
    constexpr static double MarsSemidiameterA(double Delta) { return 3600.0 * deg2rad * 4.68 / Delta; }
    constexpr static double MarsSemidiameterB(double Delta) { return 3600.0 * deg2rad * 4.68 / Delta; }

    // Find algorithms for the physical orientation of Mars if above are not sufficient

};
