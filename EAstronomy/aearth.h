#pragma once

//#include <cmath>
#include "aconfig.h"
#include "acoordinates.h"

// For returning Vondrak et al precession angles
struct APrecesionAngles {
    double psi_A{ 0.0 };
    double omega_A{ 0.0 };
    double chi_A{ 0.0 };
    //double epsilon_A{ 0.0 };
};

class AEarth {
public:
    // Position (see notes below)
    static double EclipticLongitude(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double EclipticLatitude(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double EclipticDistance(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static LLD EclipticCoordinates(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;

    static double EclipticLongitudeJ2000(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double EclipticLatitudeJ2000(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static LLD EclipticCoordinatesJ2000(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;

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

    // Precession - generic functions?
    // getObliquity()
    // getEclipticPole()     // in RA_Dec of date?
    // getEquatorialPole()   // 

    // APrecession_Model::MEEUS
    // MDO
    static LLD PrecessEquatorial(LLD decra, double JD0, double JD) noexcept;
    static LLD PrecessEquatorialJ2000(LLD decra, double jd_tt);
    static LLD PrecessEquatorialFK5(LLD decra, double JD0, double jd_tt);

    // Based directly on AA+ v2.49
    static LLD PrecessEquatorial2(double Alpha, double Delta, double JD0, double JD) noexcept;
    static LLD PrecessEquatorialFK4(double Alpha, double Delta, double JD0, double JD) noexcept;
    static LLD PrecessEcliptic(double Lambda, double Beta, double JD0, double JD) noexcept;

    // APrecession_Model::IAU2006
    // Find and implement algorithm if it is significantly faster than VONDRAK, as validity range is much shorter

    // APrecession_Model::VONDRAK - !!! FIX: Not yet added to Python interface
    // Precession from: https://www.aanda.org/articles/aa/full_html/2011/10/aa17274-11/aa17274-11.html
    static APrecesionAngles PrecessionAnglesVondrak(const double jd_tt);
    static LLD PrecessionVondrak_QP(const double jd_tt);
    static LLD PrecessionVondrak_XY(const double jd_tt);
    static double PrecessionVondrak_epsilon(double jd_tt);
    static glm::dvec3 EclipticPoleVondrak(double jd_tt);
    static glm::dvec3 EquatorialPoleVondrak(double jd_tt);

    // Obliquity
    static double MeanObliquityOfEcliptic(double jd_tt);
    static double TrueObliquityOfEcliptic(double jd_tt);

    // Nutation
    static double NutationInLongitude(double jd_tt);
    static double NutationInObliquity(double jd_tt);
    static LLD EclipticNutation(double jd_tt);
    static double NutationInRightAscension(double dec, double ra, double obliq, double nut_lon, double nut_obl);
    static double NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl);
    static LLD EquatorialNutation(LLD decra, double obliq, LLD ecnutation);

    // Time
    static double EquationOfTime(double jd_tt, Planetary_Ephemeris eph) noexcept;  // returns decimal minutes of time
    static double MeanGreenwichSiderealTime(double jd_utc) noexcept;
    static double ApparentGreenwichSiderealTime(double jd_utc) noexcept;

    // Aberration
    static glm::dvec3 EarthVelocity(double jd_tt, Planetary_Ephemeris eph) noexcept;
    static LLD EquatorialAberration(double Alpha, double Delta, double jd_tt, Planetary_Ephemeris eph) noexcept;
    static LLD EclipticAberration(double Lambda, double Beta, double jd_tt, Planetary_Ephemeris eph) noexcept;
\
    // Refraction
    static double RefractionFromApparent(double Altitude, double Pressure = 1010, double Temperature = 10) noexcept;
    static double RefractionFromTrue(double Altitude, double Pressure = 1010, double Temperature = 10) noexcept;

    // Figure of Earth (CAAGlobe) - Based on IAU76 ellipsoid
    static double RhoSinPhiPrime(double GeographicalLatitude, double Height) noexcept;
    static double RhoCosPhiPrime(double GeographicalLatitude, double Height) noexcept;
    static double RadiusOfParallelOfLatitude(double GeographicalLatitude) noexcept;
    static double RadiusOfCurvature(double GeographicalLatitude) noexcept;
    static double DistanceBetweenPoints(double GeographicalLatitude1, double GeographicalLongitude1, double GeographicalLatitude2, double GeographicalLongitude2) noexcept;

    static double GeographicLatitude2rho(double geolat);
    static LLD Geocentric2Geographic(LLD geocentric);
    static LLD Geographic2Geocentric(LLD geographic);

    // Parallax
    static double DistanceToParallax(double Distance) noexcept;
    static double ParallaxToDistance(double Parallax) noexcept;
    static LLD Equatorial2Topocentric(LLD decradst, LLD latlonhgt, double agst) noexcept;
    static LLD Equatorial2Topocentric2(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD) noexcept;
    static LLD Equatorial2TopocentricDelta(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double JD) noexcept;
    static LLD Ecliptic2Topocentric(LLD latlondst, double Semidiameter, double Latitude, double Height, double Epsilon, double agst) noexcept;
    static LLD Ecliptic2Topocentric2(double Lambda, double Beta, double Semidiameter, double Distance, double Epsilon, double Latitude, double Height, double JD) noexcept;

    // Equinoxes and Solstices
    static double NorthwardEquinox(long year, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double NorthernSolstice(long year, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double SouthwardEquinox(long year, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double SouthernSolstice(long year, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double LengthOfSpring(long year, Hemisphere hemi, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double LengthOfSummer(long year, Hemisphere hemi, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double LengthOfAutumn(long year, Hemisphere hemi, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;
    static double LengthOfWinter(long year, Hemisphere hemi, Planetary_Ephemeris eph = EPH_VSOP87_FULL) noexcept;

    static double TropicalYearLength(double jd_tt);

    // VSOP87
    // ======

    // Orbit Parameters - For Earth Moon Barycentre
    static double VSOP87_A(double jd_tt) noexcept;
    static double VSOP87_L(double jd_tt) noexcept;
    static double VSOP87_K(double jd_tt) noexcept;
    static double VSOP87_H(double jd_tt) noexcept;
    static double VSOP87_Q(double jd_tt) noexcept;
    static double VSOP87_P(double jd_tt) noexcept;

    // Ephemeris A - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
    static double VSOP87A_X(double jd_tt);
    static double VSOP87A_Y(double jd_tt);
    static double VSOP87A_Z(double jd_tt);
    static double VSOP87A_dX(double jd_tt);
    static double VSOP87A_dY(double jd_tt);
    static double VSOP87A_dZ(double jd_tt);

    // Ephemeris B - Spherical Heliocentric Ecliptic at J2000.0
    static double VSOP87B_Longitude(double jd_tt);
    static double VSOP87B_Latitude(double jd_tt);
    static double VSOP87B_Distance(double jd_tt);
    static double VSOP87B_dLongitude(double jd_tt);
    static double VSOP87B_dLatitude(double jd_tt);
    static double VSOP87B_dDistance(double jd_tt);
    
    // Ephemeris C - Rectangular Heliocentric Ecliptic at Equinox of Date
    static double VSOP87C_X(double jd_tt);
    static double VSOP87C_Y(double jd_tt);
    static double VSOP87C_Z(double jd_tt);
    static double VSOP87C_dX(double jd_tt);
    static double VSOP87C_dY(double jd_tt);
    static double VSOP87C_dZ(double jd_tt);

    // Ephemeris D - Spherical Heliocentric Ecliptic at Equinox of Date
    static double VSOP87D_Longitude(double jd_tt);
    static double VSOP87D_Latitude(double jd_tt);
    static double VSOP87D_Distance(double jd_tt);
    static double VSOP87D_dLongitude(double jd_tt);
    static double VSOP87D_dLatitude(double jd_tt);
    static double VSOP87D_dDistance(double jd_tt);

    // Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
    static double VSOP87E_X(double jd_tt);
    static double VSOP87E_Y(double jd_tt);
    static double VSOP87E_Z(double jd_tt);
    static double VSOP87E_dX(double jd_tt);
    static double VSOP87E_dY(double jd_tt);
    static double VSOP87E_dZ(double jd_tt);

    // Position
    //LLD HECS_True(double jd_tt, bool rad = false);   // Spherical True Heliocentric Ecliptic Coordinates at jd_tt
    //XYZ HECR_True(double jd_tt, bool rad = false);   // Rectangular True Heliocentric Ecliptic Coordinates at jd_tt
    //LLD GECS_True(double jd_tt, bool rad = false);   // Spherical True Geocentric Ecliptic Coordinates at jd_tt
    //XYZ GECR_True(double jd_tt, bool rad = false);   // Rectangular True Geocentric Ecliptic Coordinates at jd_tt
    // ...
};
