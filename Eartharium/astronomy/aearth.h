#pragma once

//#include <cmath>
#include "aconfig.h"
#include "acoordinates.h"

class AEarth {
public:
    // Position (see notes below)
    static double EclipticLongitude(double jd_tt, Ephemeris eph = VSOP87_FULL) noexcept;
    static double EclipticLatitude(double jd_tt, Ephemeris eph = VSOP87_FULL) noexcept;
    static double EclipticDistance(double jd_tt, Ephemeris eph = VSOP87_FULL) noexcept;
    static LLD EclipticCoordinates(double jd_tt, Ephemeris eph = VSOP87_FULL);

    static double EclipticLongitudeJ2000(double jd_tt, Ephemeris eph = VSOP87_FULL) noexcept;
    static double EclipticLatitudeJ2000(double jd_tt, Ephemeris eph = VSOP87_FULL) noexcept;

    static double SunMeanAnomaly(double jd_tt) noexcept {
        const double T{ (jd_tt - JD_2000) / JD_CENTURY };
        const double T2{ T * T };
        const double T3{ T2 * T };
        double value{ ACoord::rangezero2threesixty(357.529'1092 + (35'999.050'2909 * T) - (0.000'1536 * T2) + (T3 / 24'490'000)) };
        return deg2rad * value;
    }
    static double Eccentricity(double jd_tt) noexcept {
        const double T{ (jd_tt - JD_2000) / JD_CENTURY };
        return 1 - (0.002516 * T) - (0.0000074 * T * T);  // not an angle
    }
    // Precession
    // 
    // Obliquity
    static double MeanObliquityOfEcliptic(double jd_tt);
    static double TrueObliquityOfEcliptic(double jd_tt);

    // Nutation
    static double NutationInLongitude(double jd_tt);
    static double NutationInObliquity(double jd_tt);
    static double NutationInRightAscension(double dec, double ra, double obliq, double nut_lon, double nut_obl);
    static double NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl);

    // Time
    static double EquationOfTime(double JD, Ephemeris eph) noexcept;  // returns decimal minutes of time
    static double MeanGreenwichSiderealTime(double jd_utc) noexcept;
    static double ApparentGreenwichSiderealTime(double jd_utc) noexcept;

    // Aberration
    static glm::dvec3 EarthVelocity(double jd_tt, Ephemeris eph) noexcept;
    static LLD EquatorialAberration(double Alpha, double Delta, double jd_tt, Ephemeris eph) noexcept;
    static LLD EclipticAberration(double Lambda, double Beta, double jd_tt, Ephemeris eph) noexcept;

    // Refraction
    static double RefractionFromApparent(double Altitude, double Pressure = 1010, double Temperature = 10) noexcept;
    static double RefractionFromTrue(double Altitude, double Pressure = 1010, double Temperature = 10) noexcept;

    // Figure of Earth (CAAGlobe)
    static double RhoSinThetaPrime(double GeographicalLatitude, double Height) noexcept;
    static double RhoCosThetaPrime(double GeographicalLatitude, double Height) noexcept;
    static double RadiusOfParallelOfLatitude(double GeographicalLatitude) noexcept;
    static double RadiusOfCurvature(double GeographicalLatitude) noexcept;
    static double DistanceBetweenPoints(double GeographicalLatitude1, double GeographicalLongitude1, double GeographicalLatitude2, double GeographicalLongitude2) noexcept;

    // Parallax
    static double DistanceToParallax(double Distance) noexcept;
    static double ParallaxToDistance(double Parallax) noexcept;
    static LLD Equatorial2TopocentricDelta(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD) noexcept;
    static LLD Equatorial2Topocentric(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD) noexcept;
    static LLD Ecliptic2Topocentric(double Lambda, double Beta, double Semidiameter, double Distance, double Epsilon, double Latitude, double Height, double JD) noexcept;

    // Equinoxes and Solstices
    static double NorthwardEquinox(long Year, Ephemeris eph = VSOP87_FULL) noexcept;
    static double NorthernSolstice(long Year, Ephemeris eph = VSOP87_FULL) noexcept;
    static double SouthwardEquinox(long Year, Ephemeris eph = VSOP87_FULL) noexcept;
    static double SouthernSolstice(long Year, Ephemeris eph = VSOP87_FULL) noexcept;
    static double LengthOfSpring(long Year, Hemisphere hemi, Ephemeris eph = VSOP87_FULL) noexcept;
    static double LengthOfSummer(long Year, Hemisphere hemi, Ephemeris eph = VSOP87_FULL) noexcept;
    static double LengthOfAutumn(long Year, Hemisphere hemi, Ephemeris eph = VSOP87_FULL) noexcept;
    static double LengthOfWinter(long Year, Hemisphere hemi, Ephemeris eph = VSOP87_FULL) noexcept;



    // VSOP87
    // ======

    // Orbit Parameters - For Earth Moon Barycentre
    static double A(double jd_tt) noexcept;
    static double L(double jd_tt) noexcept;
    static double K(double jd_tt) noexcept;
    static double H(double jd_tt) noexcept;
    static double Q(double jd_tt) noexcept;
    static double P(double jd_tt) noexcept;
    // These are "cooked" orbital parameters. To obtain the traditional keplarian values, see (Rust) code here:
    // https://docs.rs/crate/vsop87/latest/source/src/lib.rs
    // Summary:
    // --------
    // Semimajor axis               a = A
    // Eccentricity:                e = sqrt(H*H + K*K)
    // Inclination:                 i = acos(1 - 2 * (P * P + Q * Q))
    // Longitude of Ascending Node: Omega = atan(P / Q) = atan2(P,Q)
    // Mean anomaly:                M = L
    // Longitude of Periapsis:      omega = asin(H / e)
    //
    // NOTE: the page also has some SIMD optimization of coordinate calculations, which might be interesting to look into for the below functions.
    struct Keplerian {
        double a{ 0.0 }; // semimajor_axis (a)
        double e{ 0.0 }; // eccentricity   (e)
        double i{ 0.0 }; // inclination    (i)
        double O{ 0.0 }; // Longitude of Ascending Node (Capital Omega)
        double M{ 0.0 }; // Mean Anomaly   (M0)
        double o{ 0.0 }; // Longitude of Periapsis      (small omega)
    };
    static Keplerian VSOP87_to_Kepler(double A, double L, double K, double H, double Q, double P) {
        Keplerian kep{};
        kep.a = A;
        kep.e = sqrt(H * H + K * K);
        kep.i = acos(1 - 2 * (P * P + Q * Q));
        kep.O = atan2(P, Q);
        kep.M = L;
        kep.o = asin(H / kep.e);
        return kep;
    }

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

    // Ephemeris E - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
    static double VSOP87_E_X(double jd_tt);
    static double VSOP87_E_Y(double jd_tt);
    static double VSOP87_E_Z(double jd_tt);
    static double VSOP87_E_dX(double jd_tt);
    static double VSOP87_E_dY(double jd_tt);
    static double VSOP87_E_dZ(double jd_tt);

    // Position
    //LLD HECS_True(double jd_tt, bool rad = false);   // Spherical True Heliocentric Ecliptic Coordinates at jd_tt
    //XYZ HECR_True(double jd_tt, bool rad = false);   // Rectangular True Heliocentric Ecliptic Coordinates at jd_tt
    //LLD GECS_True(double jd_tt, bool rad = false);   // Spherical True Heliocentric Ecliptic Coordinates at jd_tt
    //XYZ GECR_True(double jd_tt, bool rad = false);   // Rectangular True Heliocentric Ecliptic Coordinates at jd_tt

};
