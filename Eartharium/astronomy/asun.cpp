
#include <cmath>

#include "../AAPlus/AAEarth.h"

#include "acoordinates.h"
#include "anutation.h"
#include "asun.h"


// Add rad to all of these, and remember to do the same in aearth.h/.cpp

double ASun::GeometricEclipticLongitude(double jd_tt, bool hi) noexcept {
    return ACoord::rangezero2threesixty(CAAEarth::EclipticLongitude(jd_tt, hi) + 180);
}

double ASun::GeometricEclipticLatitude(double jd_tt, bool hi) noexcept {
    return -CAAEarth::EclipticLatitude(jd_tt, hi);
}

double ASun::GeometricEclipticLongitudeJ2000(double jd_tt) noexcept {
    return ACoord::rangezero2threesixty(CAAEarth::EclipticLongitudeJ2000(jd_tt, true) + 180);
}

double ASun::GeometricEclipticLatitudeJ2000(double jd_tt) noexcept {
    return -CAAEarth::EclipticLatitudeJ2000(jd_tt, true);
}

double ASun::GeometricFK5EclipticLongitude(double jd_tt) noexcept {
    //Convert to the FK5 system
    double Longitude{ GeometricEclipticLongitude(jd_tt, false) };
    const double Latitude{ GeometricEclipticLatitude(jd_tt, false) };
    Longitude += FK5::CorrectionInLongitude(Longitude, Latitude, jd_tt);

    return Longitude;
}

double ASun::GeometricFK5EclipticLatitude(double jd_tt) noexcept {
    //Convert to the FK5 system
    const double Longitude{ GeometricEclipticLongitude(jd_tt, true) };
    double Latitude{ GeometricEclipticLatitude(jd_tt, true) };
    const double SunLatCorrection{ FK5::CorrectionInLatitude(Longitude, jd_tt) };
    Latitude += SunLatCorrection;

    return Latitude;
}

double ASun::ApparentEclipticLongitude(double jd_tt) noexcept {
    double Longitude{ GeometricFK5EclipticLongitude(jd_tt) };

    //Apply the correction in longitude due to nutation
    Longitude += ACoord::secs2deg(ANutation::NutationInLongitude(jd_tt, false)); // degrees

    //Apply the correction in longitude due to aberration
    const double R{ CAAEarth::RadiusVector(jd_tt, true) };
    Longitude -= (0.005775518 * R * ACoord::secs2deg(VariationGeometricEclipticLongitude(jd_tt)));
    return Longitude;
}

double ASun::ApparentEclipticLatitude(double jd_tt) noexcept {
    return GeometricFK5EclipticLatitude(jd_tt);
}


glm::dvec3 ASun::EquatorialRectangularCoordinatesMeanEquinox(double jd_tt) noexcept {
    const double Longitude{ deg2rad * GeometricFK5EclipticLongitude(jd_tt) };
    const double sinLongitude{ sin(Longitude) };
    const double Latitude{ deg2rad * GeometricFK5EclipticLatitude(jd_tt) };
    const double cosLatitude{ cos(Latitude) };
    const double sinLatitude{ sin(Latitude) };
    const double R{ CAAEarth::RadiusVector(jd_tt, true) };
    const double epsilon{ AObliquity::MeanObliquityOfEcliptic(jd_tt, true) };  //radians
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
    double Longitude{ deg2rad * GeometricEclipticLongitudeJ2000(jd_tt) };
    double Latitude{ deg2rad * GeometricEclipticLatitudeJ2000(jd_tt) };
    const double coslatitude{ cos(Latitude) };
    const double R{ CAAEarth::RadiusVector(jd_tt, true) };

    return { R * coslatitude * cos(Longitude), R * coslatitude * sin(Longitude), R * sin(Latitude) };
}
glm::dvec3 ASun::EquatorialRectangularCoordinatesJ2000(double jd_tt) noexcept {
    glm::dvec3 value{ EclipticRectangularCoordinatesJ2000(jd_tt) };
    value = FK5::getVSOP2FK5_J2000(value);
    return value;
}
glm::dvec3 ASun::EquatorialRectangularCoordinatesB1950(double jd_tt) noexcept {
    glm::dvec3 value{ EclipticRectangularCoordinatesJ2000(jd_tt) };
    value = FK5::getVSOP2FK5_B1950(value);
    return value;
}
glm::dvec3 ASun::EquatorialRectangularCoordinatesAnyEquinox(double jd_tt, double JDEquinox) noexcept {
    glm::dvec3 value{ EquatorialRectangularCoordinatesJ2000(jd_tt) };
    value = FK5::getVSOP2FK5_AnyEquinox(value, JDEquinox);
    return value;
}
double ASun::VariationGeometricEclipticLongitude(double jd_tt, bool rad) noexcept {
    const double tau{ (jd_tt - JD_2000) / 365250 };
    const double tau2{ tau * tau };
    const double tau3{ tau2 * tau };

    const double deltaLambda{ 3548.193 +
                             (118.568 * sin(deg2rad * (87.5287 + (359993.7286 * tau)))) +
                             (2.476 * sin(deg2rad * (85.0561 + (719987.4571 * tau)))) +
                             (1.376 * sin(deg2rad * (27.8502 + (4452671.1152 * tau)))) +
                             (0.119 * sin(deg2rad * (73.1375 + (450368.8564 * tau)))) +
                             (0.114 * sin(deg2rad * (337.2264 + (329644.6718 * tau)))) +
                             (0.086 * sin(deg2rad * (222.5400 + (659289.3436 * tau)))) +
                             (0.078 * sin(deg2rad * (162.8136 + (9224659.7915 * tau)))) +
                             (0.054 * sin(deg2rad * (82.5823 + (1079981.1857 * tau)))) +
                             (0.052 * sin(deg2rad * (171.5189 + (225184.4282 * tau)))) +
                             (0.034 * sin(deg2rad * (30.3214 + (4092677.3866 * tau)))) +
                             (0.033 * sin(deg2rad * (119.8105 + (337181.4711 * tau)))) +
                             (0.023 * sin(deg2rad * (247.5418 + (299295.6151 * tau)))) +
                             (0.023 * sin(deg2rad * (325.1526 + (315559.5560 * tau)))) +
                             (0.021 * sin(deg2rad * (155.1241 + (675553.2846 * tau)))) +
                             (7.311 * tau * sin(deg2rad * (333.4515 + (359993.7286 * tau)))) +
                             (0.305 * tau * sin(deg2rad * (330.9814 + (719987.4571 * tau)))) +
                             (0.010 * tau * sin(deg2rad * (328.5170 + (1079981.1857 * tau)))) +
                             (0.309 * tau2 * sin(deg2rad * (241.4518 + (359993.7286 * tau)))) +
                             (0.021 * tau2 * sin(deg2rad * (205.0482 + (719987.4571 * tau)))) +
                             (0.004 * tau2 * sin(deg2rad * (297.8610 + (4452671.1152 * tau)))) +
                             (0.010 * tau3 * sin(deg2rad * (154.7066 + (359993.7286 * tau)))) };
    return rad ? deg2rad * deltaLambda : deltaLambda;
}