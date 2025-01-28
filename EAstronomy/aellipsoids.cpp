
#include "aellipsoids.h"
#include "acoordinates.h"

LLD AEllipsoid::Planetocentric2Planetographic(LLD planetocentric, Ellipsoid_2axis& ellipsoid) {
    // NOTE: planetocentric.dst is not used, coordinates are assumed to be at ellipsoid surface
    LLD planetographic{};
    // MEEUS98 - Chapter 11
    // phi is planetographic latitude
    // phiprime is planetocentric latitude
    // At ellipsoid surface:
    // tan phiprime = (b ^ 2 / a ^ 2) * tan phi
    // tan phi = (a ^ 2 / b ^ 2) * tan phiprime
    planetographic.lat = atan2(ellipsoid.semimajor_axis * ellipsoid.semimajor_axis * tan(planetocentric.lat), ellipsoid.semiminor_axis * ellipsoid.semiminor_axis);
    planetographic.lon = planetocentric.lon;
    return planetographic;
}
LLD AEllipsoid::Planetographic2Planetocentric(LLD planetographic, Ellipsoid_2axis& ellipsoid) {
    // NOTE: planetographic.dst is not used, coordinates are assumed to be at ellipsoid surface
    LLD planetocentric{};
    // MEEUS98 - Chapter 11
    // phi is geographic latitude
    // phiprime is geocentric latitude
    // At ellipsoid surface:
    // tan phiprime = (b ^ 2 / a ^ 2) * tan phi
    planetocentric.lat = atan2(ellipsoid.semiminor_axis * ellipsoid.semiminor_axis * tan(planetographic.lat), ellipsoid.semimajor_axis * ellipsoid.semimajor_axis);
    planetocentric.lon = planetographic.lon;
    return planetocentric;
}

// Meeus98: chapter 11
// Generalized versions of functions in AEarth - Original source: MEEUS98 Chapter 11
// According to https://en.wikipedia.org/wiki/Geodetic_coordinates#Geodetic_vs._geocentric_coordinates:
// "The standard notation for geodetic latitude is [phi]." (i.e. phi, not theta as is used in AA+)
double AEllipsoid::RhoSinPhiPrime(double PlanetographicLatitude, double Height, Ellipsoid_2axis& ellipsoid) noexcept {
    // height in meters, latitude in radians
    const double u{ atan((1.0 - ellipsoid.flattening) * tan(PlanetographicLatitude)) };
    return ((1.0 - ellipsoid.flattening) * sin(u)) + (Height / (ellipsoid.semimajor_axis * 1000.0) * sin(PlanetographicLatitude));
}
double AEllipsoid::RhoCosPhiPrime(double PlanetographicLatitude, double Height, Ellipsoid_2axis& ellipsoid) noexcept {
    // height in meters, latitude in radians
    const double u{ atan((1.0 - ellipsoid.flattening) * tan(PlanetographicLatitude)) };
    return cos(u) + (Height / (ellipsoid.semimajor_axis * 1000.0) * cos(PlanetographicLatitude));
}
double AEllipsoid::RadiusOfParallelOfLatitude(double GeographicalLatitude, Ellipsoid_2axis& ellipsoid) noexcept {
    // returns radius in km (really returns same units as semimajor axis is defined in)
    //const double sinGeo{ sin(GeographicalLatitude) };
    //return (ellipsoid.semimajor_axis * cos(GeographicalLatitude)) / (sqrt(1.0 - (ellipsoid.eccentricity * ellipsoid.eccentricity * sinGeo * sinGeo))); // 0.0066943847614084 = e^2 = 2f-f^2
    const double eccSinGeo{ ellipsoid.eccentricity * sin(GeographicalLatitude) };
    return (ellipsoid.semimajor_axis * cos(GeographicalLatitude)) / (sqrt(1.0 - (eccSinGeo * eccSinGeo))); // 0.0066943847614084 = e^2 = 2f-f^2
}
double AEllipsoid::RadiusOfCurvature(double GeographicalLatitude, Ellipsoid_2axis& ellipsoid) noexcept { // Meridional radius of curvature
    // returns radius in km (really returns same units as semimajor axis is defined in)
    //const double sinGeo{ sin(GeographicalLatitude) };
    //return (ellipsoid.semimajor_axis * (1.0 - ellipsoid.eccentricity * ellipsoid.eccentricity)) / pow((1.0 - (ellipsoid.eccentricity * ellipsoid.eccentricity * sinGeo * sinGeo)), 1.5);
    const double eccSinGeo{ ellipsoid.eccentricity * sin(GeographicalLatitude) };
    return (ellipsoid.semimajor_axis * (1.0 - ellipsoid.eccentricity * ellipsoid.eccentricity)) / pow(1.0 - (eccSinGeo * eccSinGeo), 1.5);
}
double AEllipsoid::RadiusOfPrimeVertical(double GeographicalLatitude, Ellipsoid_2axis ellipsoid) {
    // source: https://en.wikipedia.org/wiki/Earth_radius#Prime_vertical
    //double sinPhi{ sin(GeographicalLatitude) };
    //return ellipsoid.semimajor_axis / (sqrt(1.0 - (ellipsoid.eccentricity * ellipsoid.eccentricity * sinPhi * sinPhi)));
    double eccSinPhi{ ellipsoid.eccentricity * sin(GeographicalLatitude) };
    return ellipsoid.semimajor_axis / (sqrt(1.0 - (eccSinPhi * eccSinPhi)));
}
double AEllipsoid::DistanceBetweenPoints(double GeographicalLatitude1, double GeographicalLongitude1, double GeographicalLatitude2, double GeographicalLongitude2, Ellipsoid_2axis& ellipsoid) noexcept {
    // returns distance in km (really returns same units as semimajor axis is defined in)
    const double F{ (GeographicalLatitude1 + GeographicalLatitude2) / 2.0 };
    const double G{ (GeographicalLatitude1 - GeographicalLatitude2) / 2.0 };
    const double lambda{ (GeographicalLongitude1 - GeographicalLongitude2) / 2.0 };
    // !!! FIX: the following 6 consts are always used squared, so can be pre-squared to ssasve 6 multiplications
    const double sinG{ sin(G) };
    const double cosG{ cos(G) };
    const double cosF{ cos(F) };
    const double sinF{ sin(F) };
    const double sinLambda{ sin(lambda) };
    const double cosLambda{ cos(lambda) };
    const double S{ (sinG * sinG * cosLambda * cosLambda) + (cosF * cosF * sinLambda * sinLambda) };
    const double C{ (cosG * cosG * cosLambda * cosLambda) + (sinF * sinF * sinLambda * sinLambda) };
    const double w{ atan(sqrt(S / C)) };
    const double R{ sqrt(S * C) / w };
    const double D{ 2 * w * ellipsoid.semimajor_axis };
    const double Hprime{ (3.0 * R - 1) / (2.0 * C) };
    const double Hprime2{ (3.0 * R + 1) / (2.0 * S) };
    const double f{ ellipsoid.flattening };
    return D * (1.0 + (f * Hprime * sinF * sinF * cosG * cosG) - (f * Hprime2 * cosF * cosF * sinG * sinG));
}

// Initialize the static Ellipsoids
Ellipsoid_2axis AEllipsoid::Earth_IAU76{ Earth_IAU76_f, Earth_IAU76_a , Earth_IAU76_b, Earth_IAU76_e };
Ellipsoid_2axis AEllipsoid::Earth_WGS84{ Earth_WGS84_f, Earth_WGS84_a , Earth_WGS84_b, Earth_WGS84_e };
