#pragma once
#include "config.h"
#include "aconfig.h"
#include "acoordinates.h"

class ASaturn {
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


    // Find algorithms for the physical orientation of Saturn

    // Illumination
    static double MagnitudeAA(double r, double Delta, double DeltaU, double B) noexcept;
    static double MagnitudeMuller(double r, double Delta, double DeltaU, double B) noexcept;
    
    // From AA+ v2.55 CAADiameters
    constexpr static double SaturnEquatorialSemidiameterA(double Delta) { return 3600.0 * deg2rad * 83.33 / Delta; }
    constexpr static double SaturnPolarSemidiameterA(double Delta) { return 3600.0 * deg2rad * 74.57 / Delta; }
    constexpr static double SaturnEquatorialSemidiameterB(double Delta) { return 3600.0 * deg2rad * 82.73 / Delta; }
    constexpr static double SaturnPolarSemidiameterB(double Delta) { return 3600.0 * deg2rad * 73.82 / Delta; }
    static double ApparentSaturnPolarSemidiameterA(double Delta, double B) noexcept;
    static double ApparentSaturnPolarSemidiameterB(double Delta, double B) noexcept;

};
