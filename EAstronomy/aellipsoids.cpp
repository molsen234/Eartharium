
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
// Generalized version of functions in AEarth
// !!! FIX: According to https://en.wikipedia.org/wiki/Geodetic_coordinates#Geodetic_vs._geocentric_coordinates:
// "The standard notation for geodetic latitude is [phi]." (i.e. phi, not theta as is used in AA+)
double AEllipsoid::RhoSinPhiPrime(double PlanetographicLatitude, double Height, Ellipsoid_2axis& ellipsoid) noexcept {
    // height in meters, latitude in radians
    const double u{ atan((1.0 - ellipsoid.flattening) * tan(PlanetographicLatitude)) };
    return ((1.0 - ellipsoid.flattening) * sin(u)) + (Height / (ellipsoid.semimajor_axis * 1000) * sin(PlanetographicLatitude));
}
double AEllipsoid::RhoCosPhiPrime(double PlanetographicLatitude, double Height, Ellipsoid_2axis& ellipsoid) noexcept {
    // height in meters, latitude in radians
    const double u{ atan((1.0 - ellipsoid.flattening) * tan(PlanetographicLatitude)) };
    return cos(u) + (Height / (ellipsoid.semimajor_axis * 1000) * cos(PlanetographicLatitude));
}