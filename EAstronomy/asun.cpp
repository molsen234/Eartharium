
#include <cmath>

#include "acoordinates.h"
#include "asun.h"
#include "vsop87/asun_vsop87_full.h"
#include "aearth.h"

double ASun::GeometricEclipticLongitude(double jd_tt, Planetary_Ephemeris eph) noexcept {
    return ACoord::rangezero2tau(AEarth::EclipticLongitude(jd_tt, eph) + pi); // Invert Earth coordinates
}
double ASun::GeometricEclipticLatitude(double jd_tt, Planetary_Ephemeris eph) noexcept {
    return -AEarth::EclipticLatitude(jd_tt, eph);
}

double ASun::GeometricEclipticLongitudeJ2000(double jd_tt, Planetary_Ephemeris eph) noexcept {
    return ACoord::rangezero2tau(AEarth::EclipticLongitudeJ2000(jd_tt, eph) + pi);  // 180 -> pi
}
double ASun::GeometricEclipticLatitudeJ2000(double jd_tt, Planetary_Ephemeris eph) noexcept {
    return -AEarth::EclipticLatitudeJ2000(jd_tt, eph);
}

double ASun::GeometricFK5EclipticLongitude(double jd_tt, Planetary_Ephemeris eph) noexcept {
    //Convert to the FK5 system
    double Longitude{ GeometricEclipticLongitude(jd_tt, eph) };
    const double Latitude{ GeometricEclipticLatitude(jd_tt, eph) };
    Longitude += FK5::CorrectionInLongitude(Longitude, Latitude, jd_tt);
    return Longitude;
}
double ASun::GeometricFK5EclipticLatitude(double jd_tt, Planetary_Ephemeris eph) noexcept {
    //Convert to the FK5 system
    const double Longitude{ GeometricEclipticLongitude(jd_tt, eph) };
    double Latitude{ GeometricEclipticLatitude(jd_tt, eph) };
    Latitude += FK5::CorrectionInLatitude(Longitude, jd_tt);
    return Latitude;
}

double ASun::ApparentEclipticLongitude(double jd_tt, Planetary_Ephemeris eph) noexcept {
    double Longitude{ GeometricFK5EclipticLongitude(jd_tt) };       // radians
    //Apply the correction in longitude due to nutation
    Longitude += AEarth::NutationInLongitude(jd_tt);                // radians
    //Apply the correction in longitude due to aberration
    const double R{ AEarth::EclipticDistance(jd_tt, eph) };
    // AA+ has the following, but the 20.4898/R correction is quite inaccurate so retaining the VariationGeometricEclipticLongitude()
    //if (bHighPrecision)
    //    Longitude -= (0.005775518 * R * CAACoordinateTransformation::DMSToDegrees(0, 0, VariationGeometricEclipticLongitude(JD)));
    //else
    //    Longitude -= CAACoordinateTransformation::DMSToDegrees(0, 0, 20.4898 / R);

    Longitude -= (0.005775518 * R * VariationGeometricEclipticLongitude(jd_tt)); // 0.005775518 = 1 AU light time in days
    return Longitude;
}
double ASun::ApparentEclipticLatitude(double jd_tt, Planetary_Ephemeris eph) noexcept {
    return GeometricFK5EclipticLatitude(jd_tt, eph);
}


glm::dvec3 ASun::EquatorialRectangularCoordinatesMeanEquinox(double jd_tt) noexcept {
    const double Longitude{ deg2rad * GeometricFK5EclipticLongitude(jd_tt) };
    const double sinLongitude{ sin(Longitude) };
    const double Latitude{ deg2rad * GeometricFK5EclipticLatitude(jd_tt) };
    const double cosLatitude{ cos(Latitude) };
    const double sinLatitude{ sin(Latitude) };
    const double R{ AEarth::EclipticDistance(jd_tt, EPH_VSOP87_FULL) };
    const double epsilon{ AEarth::MeanObliquityOfEcliptic(jd_tt) };
    const double cosepsilon{ cos(epsilon) };
    const double sinepsilon{ sin(epsilon) };

    glm::dvec3 value{
        R * cosLatitude * cos(Longitude),
        R * ((cosLatitude * sinLongitude * cosepsilon) - (sinLatitude * sinepsilon)),
        R * ((cosLatitude * sinLongitude * sinepsilon) + (sinLatitude * cosepsilon))
    };
    return value;
}
glm::dvec3 ASun::EclipticRectangularCoordinatesJ2000(double jd_tt) noexcept {
    double Longitude{ GeometricEclipticLongitudeJ2000(jd_tt) };
    double Latitude{ GeometricEclipticLatitudeJ2000(jd_tt) };
    const double coslatitude{ cos(Latitude) };
    const double R{ AEarth::EclipticDistance(jd_tt, EPH_VSOP87_FULL) };

    return { R * coslatitude * cos(Longitude), R * coslatitude * sin(Longitude), R * sin(Latitude) };
}
glm::dvec3 ASun::EquatorialRectangularCoordinatesJ2000(double jd_tt) noexcept {
    glm::dvec3 value{ EclipticRectangularCoordinatesJ2000(jd_tt) };
    value = FK5::VSOP2FK5_J2000(value);
    return value;
}
glm::dvec3 ASun::EquatorialRectangularCoordinatesB1950(double jd_tt) noexcept {
    glm::dvec3 value{ EclipticRectangularCoordinatesJ2000(jd_tt) };
    value = FK5::VSOP2FK5_B1950(value);
    return value;
}
glm::dvec3 ASun::EquatorialRectangularCoordinatesAnyEquinox(double jd_tt, double JDEquinox) noexcept {
    glm::dvec3 value{ EquatorialRectangularCoordinatesJ2000(jd_tt) };
    value = FK5::VSOP2FK5_AnyEquinox(value, JDEquinox);
    return value;
}
double ASun::VariationGeometricEclipticLongitude(double jd_tt) noexcept {
    // AA+ CAASun::VariationGeometricEclipticLongitude()
    // Meeus98 page 168
    const double Tau{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double Tau2{ Tau * Tau };
    const double Tau3{ Tau2 * Tau };

    // Calculate arcseconds per day of the Sun due to aberration
    const double deltaLambda{ 3548.193 +
                             (118.568 * sin(deg2rad * (87.5287 + (359993.7286 * Tau)))) +
                             (2.476 * sin(deg2rad * (85.0561 + (719987.4571 * Tau)))) +
                             (1.376 * sin(deg2rad * (27.8502 + (4452671.1152 * Tau)))) +
                             (0.119 * sin(deg2rad * (73.1375 + (450368.8564 * Tau)))) +
                             (0.114 * sin(deg2rad * (337.2264 + (329644.6718 * Tau)))) +
                             (0.086 * sin(deg2rad * (222.5400 + (659289.3436 * Tau)))) +
                             (0.078 * sin(deg2rad * (162.8136 + (9224659.7915 * Tau)))) +
                             (0.054 * sin(deg2rad * (82.5823 + (1079981.1857 * Tau)))) +
                             (0.052 * sin(deg2rad * (171.5189 + (225184.4282 * Tau)))) +
                             (0.034 * sin(deg2rad * (30.3214 + (4092677.3866 * Tau)))) +
                             (0.033 * sin(deg2rad * (119.8105 + (337181.4711 * Tau)))) +
                             (0.023 * sin(deg2rad * (247.5418 + (299295.6151 * Tau)))) +
                             (0.023 * sin(deg2rad * (325.1526 + (315559.5560 * Tau)))) +
                             (0.021 * sin(deg2rad * (155.1241 + (675553.2846 * Tau)))) +
                             (7.311 * Tau * sin(deg2rad * (333.4515 + (359993.7286 * Tau)))) +
                             (0.305 * Tau * sin(deg2rad * (330.9814 + (719987.4571 * Tau)))) +
                             (0.010 * Tau * sin(deg2rad * (328.5170 + (1079981.1857 * Tau)))) +
                             (0.309 * Tau2 * sin(deg2rad * (241.4518 + (359993.7286 * Tau)))) +
                             (0.021 * Tau2 * sin(deg2rad * (205.0482 + (719987.4571 * Tau)))) +
                             (0.004 * Tau2 * sin(deg2rad * (297.8610 + (4452671.1152 * Tau)))) +
                             (0.010 * Tau3 * sin(deg2rad * (154.7066 + (359993.7286 * Tau)))) };
    return deg2rad * deltaLambda / 3600.0; // radians per day of the Sun due to aberration
}


// VSOP87 Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
double ASun::VSOP87_E_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_X_SUN.data(), g_VSOP87E_X_SUN.size(), false);
}
double ASun::VSOP87_E_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Y_SUN.data(), g_VSOP87E_Y_SUN.size(), false);
}
double ASun::VSOP87_E_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Z_SUN.data(), g_VSOP87E_Z_SUN.size(), false);
}
double ASun::VSOP87_E_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_X_SUN.data(), g_VSOP87E_X_SUN.size());
}
double ASun::VSOP87_E_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Y_SUN.data(), g_VSOP87E_Y_SUN.size());
}
double ASun::VSOP87_E_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Z_SUN.data(), g_VSOP87E_Z_SUN.size());
}


// Physical - directly from AA+, should be cleaned up a bit for efficiency

PhysicalSunDetails ASun::CalculatePhysicalSun(double jd_tt, Planetary_Ephemeris eph) noexcept {
    PhysicalSunDetails details;

    double theta{ ACoord::rangezero2threesixty((jd_tt - 2398220) * 360 / 25.38) * deg2rad };
    double I{ 7.25 * deg2rad };
    double K{ (73.6667 + (1.3958333 * (jd_tt - 2396758) / 36525)) * deg2rad };

    //Calculate the apparent longitude of the sun (excluding the effect of nutation)
    const double L{ AEarth::EclipticLongitude(jd_tt, eph) };
    const double R{ AEarth::EclipticDistance(jd_tt, eph) };
    double SunLong{ L + pi - ACoord::dms2rad(0, 0, 20.4898 / R) };

    double epsilon{ AEarth::TrueObliquityOfEcliptic(jd_tt) };

    const double x{ atan(-cos(SunLong) * tan(epsilon)) };
    const double y{ atan(-cos(SunLong - K) * tan(I)) };

    details.P = x + y;
    details.B0 = asin(sin(SunLong - K) * sin(I));
    const double SunLongMinusK{ SunLong - K };
    const double eta{ atan2(-sin(SunLongMinusK) * cos(I), -cos(SunLongMinusK)) };
    details.L0 = ACoord::rangezero2tau(eta - theta);

    return details;
}

double ASun::TimeOfStartOfRotation(long C) noexcept {
    // C is the carrington count
    double JED{ 2398140.2270 + (27.2752316 * C) };
    double M{ ACoord::rangezero2threesixty(281.96 + (26.882476 * C)) };
    M *= deg2rad;
    const double twoM{ 2 * M };
    JED += ((0.1454 * sin(M)) - (0.0085 * sin(twoM)) - (0.0141 * cos(twoM)));
    return JED;
}