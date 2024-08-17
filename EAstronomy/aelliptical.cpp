
#include "aelliptical.h"

#include "aearth.h"
#include "asun.h"
#include "amercury.h"
#include "avenus.h"
#include "amars.h"
#include "ajupiter.h"
#include "asaturn.h"
#include "auranus.h"
#include "aneptune.h"

#include <cmath>

// from CAAKepler
double AElliptical::calcKepler(double M, double e, int nIterations) noexcept {
    // MEEUS98 chapter 30
    // Solve Kepler's equation
    // returns Eccentric Anomaly, from M = Mean anomaly, e = eccentricity
    double F{ 1 };
    if (M < 0) F = -1;
    M = fabs(M) / tau;
    M = (M - static_cast<int>(M)) * tau * F;
    if (M < 0) M += tau;
    F = 1;
    if (M > pi) {
        M = tau - M;
        F = -1;
    }
    double E{ pi / 2 };
    double D{ pi / 4 };
    for (int i{ 0 }; i < nIterations; i++) {
        const double M1{ E - (e * sin(E)) };
        if (M > M1) E += D;
        else E -= D;
        D /= 2;
    }
    return E * F;  // radians
}


// from CAAElliptical - Compare to planetaryDetails() in Astronomy.cpp

APlanetaryDetails AElliptical::getPlanetaryDetails(double jd_tt, Planet planet, Planetary_Ephemeris eph) noexcept
// !!! Planet A_EMB and A_EARTH currently not implemented, they don't make much sense in geocentric calculations, or maybe A_EMB does?
{
    // return value
    APlanetaryDetails details;

    // Precision parameters for Light Time compensation
    const double prec_angle = 0.00001 * deg2rad;  // 0.00001 degrees = 36 mas (milli arc seconds)
    const double prec_distance = 0.000001;        // AU

    // Iterate to find the positions adjusting for light-time correction if required
    double JD0{ jd_tt };            // jd modified during iteration
    LLD pla{};                      // target planet/sun coordinates
    LLD pla_prev{};                 // previous planet/sun coordinates, during iteration
    glm::dvec3 tgecr_lt{ 0.0 };     // light time adjusted true geocentric ecliptical rectangular coordinates, output of light time iteration

    if (planet != A_SUN) {
        // Calculate the position of the earth first
        const glm::dvec3 earth0_rect{ Spherical::Spherical2Rectangular(AEarth::EclipticCoordinates(JD0, eph)) };
        // True Heliocentric ECliptical Spherical coordinates of the planet
        pla = EclipticCoordinates(JD0, planet, eph);
        details.thecs = pla;
        // True Geocentric ECliptical Rectangular coordinates of the planet
        details.tgecr = Spherical::Spherical2Rectangular(pla) - earth0_rect;  // OK!
        // Iterate to get Light Time corrected planet position
        bool bRecalc{ true };
        bool bFirst{ true };
        while (bRecalc) {
            //std::cout << "Calculating Planet Light Time compensation." << std::endl;
            if (!bFirst) pla = EclipticCoordinates(JD0, planet, eph); // For first run, use values calculated just above
            bFirst = false;
            bRecalc = ((fabs(pla.lon - pla_prev.lon) > prec_angle)
                || (fabs(pla.lat - pla_prev.lat) > prec_angle)
                || (fabs(pla.dst - pla_prev.dst) > prec_distance));
            pla_prev = pla;
            //Prepare for the next loop around
            if (bRecalc) JD0 = jd_tt - ACoord::DistanceToLightTime(glm::length(Spherical::Spherical2Rectangular(pla) - earth0_rect));
        }
        tgecr_lt = Spherical::Spherical2Rectangular(pla) - earth0_rect;
    }
    else { // planet == A_SUN
        // Note: True Heliocentric ECliptical Spherical coordinates of the Sun are by definition 0.0,0.0,0.0
        // True Geocentric ECliptical Spherical coordinates
        pla = AEarth::EclipticCoordinates(JD0, eph);
        details.tgecr = -Spherical::Spherical2Rectangular(pla);  // Sun's Geocentric coordinates are the inverse of Earth's Heliocentric coords
        // Light Time compensated True Geocentric ECliptical Spherical coordinates
        bool bRecalc{ true };  // Prime the while() loop
        bool bFirst{ true };
        while (bRecalc) {
            if (!bFirst) pla = AEarth::EclipticCoordinates(JD0, eph);
            bFirst = false;
            bRecalc = ((fabs(pla.lon - pla_prev.lon) > prec_angle)
                || (fabs(pla.lat - pla_prev.lat) > prec_angle)
                || (fabs(pla.dst - pla_prev.dst) > prec_distance));
            pla_prev = pla;
            //Prepare for the next loop around
            if (bRecalc) JD0 = jd_tt - ACoord::DistanceToLightTime(pla.dst);
        }
        // Light Time compensated True Geocentric ECliptical Rectangular coordinates
        tgecr_lt = -Spherical::Spherical2Rectangular(pla);
    }

    // True Geocentric ECliptical Rectangular to True Geocentric ECliptical Spherical
    details.tgecs = Spherical::Rectangular2Spherical(details.tgecr);
    // True Light Time
    details.tlt = ACoord::DistanceToLightTime(details.tgecs.dst);

    // True -> Apparent Geocentric ECliptical Spherical (corrections below are all spherical)
    // Use True Geocentric ECliptic Rectangular to get True Geocentric ECliptic Spherical
    details.agecs = Spherical::Rectangular2Spherical(tgecr_lt);
    LLD aberration = AEarth::EclipticAberration(details.agecs.lon, details.agecs.lat, jd_tt, eph);
    details.agecs += FK5::CorrectionInLonLat(details.agecs, jd_tt);  // Convert to the FK5 system
    details.agecs += aberration; // Both aberration and FK5 correction are calculated from True Geocentric
    details.agecs.lon += AEarth::NutationInLongitude(jd_tt);
    details.alt = ACoord::DistanceToLightTime(details.agecs.dst);  // Apparent Light Time
    // Convert to RA and Dec
    const double epsilon = AEarth::TrueObliquityOfEcliptic(jd_tt);
    // True Geocentric Equatorial Spherical
    details.tgeqs = Spherical::Ecliptic2Equatorial(details.tgecs, epsilon);
    details.tgeqs.dst = details.tgecs.dst;  // True Geocentric Distance
    // Apparent Geocentric Equatorial Spherical
    details.ageqs = Spherical::Ecliptic2Equatorial(details.agecs, epsilon);
    details.ageqs.dst = details.agecs.dst;  // Apparent Geocentric Distance
    details.jd_tt = jd_tt;
    return details;
}
LLD AElliptical::EclipticCoordinates(double jd_tt, Planet planet, Planetary_Ephemeris eph) {
    // helper function to make AElliptical::getPlanetaryDetails() a bit easier to read
    LLD retval{};
    switch (planet) {
        //case A_SUN: is not needed by AElliptical::getPlanetDetails()
        case A_MERCURY: return AMercury::EclipticCoordinates(jd_tt, eph);
        case A_VENUS:   return AVenus::EclipticCoordinates(jd_tt, eph);
        //case A_EARTH:   return AEarth::EclipticCoordinates(jd_tt, eph);  // Needed when observer can be elsewhere
        //case A_EMB: is not yet implemented (AAVSOP87A_EMB -> aearth or amoon or a new aemb.[h/cpp]
        case A_MARS:    return AMars::EclipticCoordinates(jd_tt, eph);
        case A_JUPITER: return AJupiter::EclipticCoordinates(jd_tt, eph);
        case A_SATURN:  return ASaturn::EclipticCoordinates(jd_tt, eph);
        case A_URANUS:  return AUranus::EclipticCoordinates(jd_tt, eph);
        case A_NEPTUNE: return ANeptune::EclipticCoordinates(jd_tt, eph);
        default: {
            std::cout << "AElliptical::EclipticCoordinates(): ERROR - Unknown planet id passed" << std::endl;
            assert(false);
        }
    }
    // Should never get here, but return invalid values, just in case.
    retval.lat = NO_DOUBLE;
    retval.lon = NO_DOUBLE;
    retval.dst = NO_DOUBLE;
    return retval;
}
double AElliptical::MeanMotionFromSemiMajorAxis(double a) noexcept {
    return deg2rad * 0.9856076686 / (a * sqrt(a));  // radians
}

AEllipticalObjectDetails AElliptical::Calculate(double JD, const AEllipticalObjectElements& elements, bool bHighPrecision) noexcept {
    // MEEUS98 chapter 33
    // from AA+ v2.49 CAAElliptical

    AEllipticalObjectDetails details; // Return value

    double Epsilon{ AEarth::MeanObliquityOfEcliptic(elements.JDEquinox) };

    const double sinEpsilon{ sin(Epsilon) };
    const double cosEpsilon{ cos(Epsilon) };
    const double sinOmega{ sin(elements.omega) };
    const double cosOmega{ cos(elements.omega) };
    const double cosi{ cos(elements.i) };
    const double sini{ sin(elements.i) };

    const double F{ cosOmega };
    const double G{ sinOmega * cosEpsilon };
    const double H{ sinOmega * sinEpsilon };
    const double P{ -sinOmega * cosi };
    const double Q{ (cosOmega * cosi * cosEpsilon) - (sini * sinEpsilon) };
    const double R{ (cosOmega * cosi * sinEpsilon) + (sini * cosEpsilon) };
    const double a{ sqrt((F * F) + (P * P)) };
    const double b{ sqrt((G * G) + (Q * Q)) };
    const double c{ sqrt((H * H) + (R * R)) };
    const double A{ atan2(F, P) };
    const double B{ atan2(G, Q) };
    const double C{ atan2(H, R) };
    const double n{ AElliptical::MeanMotionFromSemiMajorAxis(elements.a) };  // radians

    const glm::dvec3 SunCoord{ ASun::EquatorialRectangularCoordinatesAnyEquinox(JD, elements.JDEquinox) };

    double JD0{ JD };
    for (int j = 0; j < 2; j++) {
        const double M{ n * (JD0 - elements.T) };  // radians
        double E{ calcKepler(M, elements.e) };     // radians
        const double v{ 2 * atan(sqrt((1 + elements.e) / (1 - elements.e)) * tan(E / 2)) };
        const double r{ elements.a * (1 - (elements.e * cos(E))) };
        const double x{ r * a * sin(A + elements.w + v) };
        const double y{ r * b * sin(B + elements.w + v) };
        const double z{ r * c * sin(C + elements.w + v) };

        if (j == 0)  {
            details.HeliocentricRectangularEquatorial.x = x;
            details.HeliocentricRectangularEquatorial.y = y;
            details.HeliocentricRectangularEquatorial.z = z;

            //Calculate the heliocentric ecliptic coordinates also
            const double u{ elements.w + v };
            const double cosu{ cos(u) };
            const double sinu{ sin(u) };

            details.HeliocentricRectangularEcliptical.x = r * ((cosOmega * cosu) - (sinOmega * sinu * cosi));
            details.HeliocentricRectangularEcliptical.y = r * ((sinOmega * cosu) + (cosOmega * sinu * cosi));
            details.HeliocentricRectangularEcliptical.z = r * sini * sinu;

            details.HeliocentricEclipticLongitude = ACoord::rangezero2tau(atan2(details.HeliocentricRectangularEcliptical.y, details.HeliocentricRectangularEcliptical.x));
            details.HeliocentricEclipticLatitude = asin(details.HeliocentricRectangularEcliptical.z / r);
        }
        const double psi{ SunCoord.x + x };
        const double nu{ SunCoord.y + y };
        const double nusquared(nu * nu);
        const double sigma{ SunCoord.z + z };
        const double psisquared{ psi * psi };

        double Alpha{ atan2(nu, psi) };
        double Delta{ atan2(sigma, sqrt(psisquared + nusquared)) };
        const double Distance{ sqrt(psisquared + nusquared + (sigma * sigma)) };

        if (j == 0) {
            details.TrueGeocentricRA = ACoord::rangezero2tau(Alpha);
            details.TrueGeocentricDeclination = Delta;
            details.TrueGeocentricDistance = Distance;
            details.TrueGeocentricLightTime = ACoord::DistanceToLightTime(Distance);
        }
        else {
            // Astrometric coordinates include lighttime compensation, but NOT aberration nor nutation
            details.AstrometricGeocentricRA = ACoord::rangezero2tau(Alpha);
            details.AstrometricGeocentricDeclination = Delta;
            details.AstrometricGeocentricDistance = Distance;
            details.AstrometricGeocentricLightTime = ACoord::DistanceToLightTime(Distance);

            const double RES{ sqrt((SunCoord.x * SunCoord.x) + (SunCoord.y * SunCoord.y) + (SunCoord.z * SunCoord.z)) };
            const double rsquared(r * r);
            const double Distancesquared(Distance * Distance);
            details.Elongation = acos(((RES * RES) + Distancesquared - rsquared) / (2 * RES * Distance));
            details.PhaseAngle = acos((rsquared + Distancesquared - RES * RES) / (2 * r * Distance));
        }
        if (j == 0) //Prepare for the next loop around
            JD0 = JD - details.TrueGeocentricLightTime;
    }
    return details;
}
double AElliptical::InstantaneousVelocity(double r, double a) noexcept {
    // MEEUS98 chapter 33 ?
    // r The distance of the object from the Sun in astronomical units.
    // a The semi major axis of the orbit in astronomical units.
    // returns the instantaneous velocity of the object in kilometres per second
    return 42.1219 * sqrt((1 / r) - (1 / (2 * a)));
}
double AElliptical::VelocityAtPerihelion(double e, double a) noexcept {
    // MEEUS98 chapter 33 ?
    // e The eccentricity of the orbit
    // a The semi major axis of the orbit in astronomical units
    // returns velocity of the object in kilometres per second at perihelion
    return 29.7847 / sqrt(a) * sqrt((1 + e) / (1 - e));
}
double AElliptical::VelocityAtAphelion(double e, double a) noexcept {
    // MEEUS98 chapter 33 ?
    // e The eccentricity of the orbit
    // a The semi major axis of the orbit in astronomical units
    // returns velocity of the object in kilometres per second at aphelion
    return 29.7847 / sqrt(a) * sqrt((1 - e) / (1 + e));
}
double AElliptical::LengthOfEllipse(double e, double a) noexcept {
    // MEEUS98 chapter 33 ?
    // e The eccentricity of the orbit
    // a The semi major axis of the orbit in astronomical units
    // returns length of ellipse in AU
    const double b{ a * sqrt(1 - (e * e)) };
    return pi * (3 * (a + b) - sqrt((a + (3 * b)) * ((3 * a) + b)));
}
double AElliptical::CometMagnitude(double g, double delta, double k, double r) noexcept {
    // MEEUS98 eq 33.13
    // g The absolute magnitude of the comet.
    // delta Distance of the comet to the Earth in astronomical units.
    // k A constant which differs from one comet to another.
    // r Distance of the comet from the Sun in astronomical units
    return g + (5 * log10(delta)) + (k * log10(r));
}
double AElliptical::MinorPlanetMagnitude(double H, double delta, double G, double r, double PhaseAngle) noexcept {
    // MEEUS98 eq 33.14
    // H The mean absolute visual magnitude of the minor planet.
    // delta Distance of the minor planet to the Earth in astronomical units.
    // G The so called "slope parameter" which differs from one minor planet to another.
    // r Distance of the minor planet from the Sun in astronomical units
    // PhaseAngle the Sun - body - Earth angle in radians
    const double phi1{ exp(-3.33 * pow(tan(PhaseAngle / 2.0), 0.63)) };
    const double phi2{ exp(-1.87 * pow(tan(PhaseAngle / 2.0), 1.22)) };
    return H + (5.0 * log10(r * delta)) - (2.5 * log10(((1.0 - G) * phi1) + (G * phi2)));
}
