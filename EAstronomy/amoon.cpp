
#include <cstddef>
#include "acoordinates.h"
#include "amoon.h"
#include "amoon_eph_short.h"
#include "amoon_eph_elp2000.h"
#include "amoon_eph_elpmpp02.h"
#include "aearth.h"
#include "asun.h"

// IMPORTANT NOTE:
// ===============
// When converting NutationInLongitude and NutationInObliquity from AA+ to aearth.cpp, be aware that
// CAANutation delivered the result in arc seconds, whereas AEarth delivers them in radians
// The two are used in some CAAPhysicalMoon and CAAMoon functions, so care must be take when converting those!!
// UPD: Care was taken with CAAMoon below

LLD AMoon::EclipticCoordinates(double jd_tt, Lunar_Ephemeris eph) {
    LLD coords{ };
    if (eph == MEEUS_SHORT) {
        return Ecliptic_Meeus_Short(jd_tt);
        //coords.lat = EclipticLatitude(jd_tt);
        //coords.lon = EclipticLongitude(jd_tt);
        //coords.dst = RadiusVector(jd_tt);
    }
    else if (eph == ELP2000_82) {
        //return AELP2000::EclipticCoordinates(jd_tt);
        coords.lon = AELP2000::EclipticLongitude(jd_tt);
        coords.lat = AELP2000::EclipticLatitude(jd_tt);
        coords.dst = AELP2000::RadiusVector(jd_tt);
    }
    else if (eph == ELP_MPP02) {
        coords.lat = AELPMPP02::EclipticLatitude(jd_tt);
        coords.lon = AELPMPP02::EclipticLongitude(jd_tt);
        coords.dst = AELPMPP02::RadiusVector(jd_tt);
    }
    coords.lon = ACoord::rangezero2tau(coords.lon);
    return coords;  // radians and kilometers
}
LLD AMoon::Ecliptic_Meeus_Short(double jd_tt) {
    LLD coord{ };
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double T2{ T * T };
    const double T3{ T2 * T };
    const double T4{ T3 * T };

    // Mean Longiude
    const double Ldash{ ACoord::rangezero2tau(deg2rad * (218.316'4477 + (481'267.881'234'21 * T) - (0.0015786 * T2) + (T3 / 538'841) - (T4 / 65'194'000))) };
    // Mean Elongation
    const double D{ ACoord::rangezero2tau(deg2rad * (297.850'1921 + (445'267.111'4034 * T) - (0.001'8819 * T2) + (T3 / 545'868) - (T4 / 113'065'000))) };
    // Sun Mean Anomaly
    const double M{ ACoord::rangezero2tau(deg2rad * (357.529'1092 + (35'999.050'2909 * T) - (0.000'1536 * T2) + (T3 / 24'490'000))) };
    // Mean Anomaly
    const double Mdash{ ACoord::rangezero2tau(deg2rad * (134.963'3964 + (477'198.867'5055 * T) + (0.008'7414 * T2) + (T3 / 69'699) - (T4 / 14'712'000))) };
    // Argument of Latitude
    const double F{ ACoord::rangezero2tau(deg2rad * (93.272'0950 + (483'202.017'5233 * T) - (0.003'6539 * T2) - (T3 / 3'526'000) + (T4 / 863'310'000))) };
    // Earth Eccentricity
    const double E{ 1 - (0.002'516 * T) - (0.000'0074 * T2) };
    const double E2{ E * E };

    const double A1{ ACoord::rangezero2tau(deg2rad * (119.75 + (131.849 * T))) };
    const double A2{ ACoord::rangezero2tau(deg2rad * (53.09 + (479264.290 * T))) };
    const double A3{ ACoord::rangezero2tau(deg2rad * (313.45 + (481'266.484 * T))) };
    constexpr size_t nLRCoefficients{ g_MoonCoefficients1.size() };
    double SigmaL{ 0.0 };
    double SigmaB{ 0.0 };
    double SigmaR{ 0.0 };
    // Longitude and RadiusVector
    for (size_t i{ 0 }; i < nLRCoefficients; i++) {
        double trig_argument = (g_MoonCoefficients1[i].D * D) + (g_MoonCoefficients1[i].M * M) +  (g_MoonCoefficients1[i].Mdash * Mdash) + (g_MoonCoefficients1[i].F * F);
        double ThisSigmaL{ g_MoonCoefficients2[i].A * sin(trig_argument) };
        double ThisSigmaR{ g_MoonCoefficients2[i].B * cos(trig_argument) };

        if ((g_MoonCoefficients1[i].M == 1) || (g_MoonCoefficients1[i].M == -1)) {
            ThisSigmaL *= E;
            ThisSigmaR *= E;
        }
        else if ((g_MoonCoefficients1[i].M == 2) || (g_MoonCoefficients1[i].M == -2)) {
            ThisSigmaL *= E2;
            ThisSigmaR *= E2;
        }
        SigmaL += ThisSigmaL;
        SigmaR += ThisSigmaR;
    }
    SigmaL += 3958 * sin(A1);
    SigmaL += 1962 * sin(Ldash - F);
    SigmaL +=  318 * sin(A2);
    SigmaL *= deg2rad;
    SigmaL /= 1'000'000;
    coord.lon = ACoord::rangezero2tau(Ldash + SigmaL + AEarth::NutationInLongitude(jd_tt));  // radians
    coord.dst = 385'000.56 + (SigmaR / 1'000);  // kilometers

    // Latitude
    constexpr size_t nBCoefficients{ g_MoonCoefficients3.size() };
    for (size_t i{ 0 }; i < nBCoefficients; i++) {
        double ThisSigma{ g_MoonCoefficients4[i] * sin((g_MoonCoefficients3[i].D * D) + (g_MoonCoefficients3[i].M * M) +
                                                    (g_MoonCoefficients3[i].Mdash * Mdash) + (g_MoonCoefficients3[i].F * F)) };
        if ((g_MoonCoefficients3[i].M == 1) || (g_MoonCoefficients3[i].M == -1)) ThisSigma *= E;
        else if ((g_MoonCoefficients3[i].M == 2) || (g_MoonCoefficients3[i].M == -2)) ThisSigma *= E2;
        SigmaB += ThisSigma;
    }
    SigmaB -= 2235 * sin(Ldash);
    SigmaB +=  382 * sin(A3);
    SigmaB +=  175 * sin(A1 - F);
    SigmaB +=  175 * sin(A1 + F);
    SigmaB +=  127 * sin(Ldash - Mdash);
    SigmaB -=  115 * sin(Ldash + Mdash);
    SigmaB /= 1'000'000;
    coord.lat = deg2rad * SigmaB;  // radians

    return coord;
}
double AMoon::MeanLongitude(double jd_tt) noexcept {
    // Moons mean longitude (L') referred to mean equinox of date , including -0.70" light time compensation (eqn 47.1)
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double Tcubed{ Tsquared * T };
    const double T4{ Tcubed * T };
    return ACoord::rangezero2tau(deg2rad * (218.316'4477 + (481'267.881'234'21 * T) - (0.0015786 * Tsquared) + (Tcubed / 538'841) - (T4 / 65'194'000)));
}
double AMoon::MeanElongation(double jd_tt) noexcept {
    // Mean Elongation of the Moon (D)
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double Tcubed{ Tsquared * T };
    const double T4{ Tcubed * T };
    return ACoord::rangezero2tau(deg2rad * (297.850'1921 + (445'267.111'4034 * T) - (0.001'8819 * Tsquared) + (Tcubed / 545'868) - (T4 / 113'065'000)));
}
double AMoon::MeanAnomaly(double jd_tt) noexcept {
    // Moon's Mean Anomaly (M')
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double Tcubed{ Tsquared * T };
    const double T4{ Tcubed * T };
    return ACoord::rangezero2tau(deg2rad * (134.963'3964 + (477'198.867'5055 * T) + (0.008'7414 * Tsquared) + (Tcubed / 69'699) - (T4 / 14'712'000)));
}
double AMoon::ArgumentOfLatitude(double jd_tt) noexcept {
    // Moon's Argument of Latitude (mean distance of the Moon from its ascending node) (F)
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double Tcubed{ Tsquared * T };
    const double T4{ Tcubed * T };
    return ACoord::rangezero2tau(deg2rad * (93.272'0950 + (483'202.017'5233 * T) - (0.003'6539 * Tsquared) - (Tcubed / 3'526'000) + (T4 / 863'310'000)));
}

double AMoon::EclipticLongitude(double jd_tt) noexcept {
    double Ldash{ MeanLongitude(jd_tt) };
    double D{ MeanElongation(jd_tt) };
    double M{ AEarth::SunMeanAnomaly(jd_tt) };
    double Mdash{ MeanAnomaly(jd_tt) };
    double F{ ArgumentOfLatitude(jd_tt) };

    const double E{ AEarth::Eccentricity(jd_tt) };
    const double Esquared{ E * E };
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    double A1{ ACoord::rangezero2tau(deg2rad * (119.75 + (131.849 * T))) };
    double A2{ ACoord::rangezero2tau(deg2rad * (53.09 + (479264.290 * T))) };

    constexpr size_t nLCoefficients{ g_MoonCoefficients1.size() };
    assert(nLCoefficients == g_MoonCoefficients2.size());

    double SigmaL{ 0 };
    for (size_t i{ 0 }; i < nLCoefficients; i++) {
        double ThisSigma{ g_MoonCoefficients2[i].A * sin((g_MoonCoefficients1[i].D * D) + (g_MoonCoefficients1[i].M * M) +
                                                      (g_MoonCoefficients1[i].Mdash * Mdash) + (g_MoonCoefficients1[i].F * F)) };

        if ((g_MoonCoefficients1[i].M == 1) || (g_MoonCoefficients1[i].M == -1)) ThisSigma *= E;
        else if ((g_MoonCoefficients1[i].M == 2) || (g_MoonCoefficients1[i].M == -2)) ThisSigma *= Esquared;
        SigmaL += ThisSigma;
    }

    //Finally the additive terms
    SigmaL += 3958 * sin(A1);
    SigmaL += 1962 * sin(Ldash - F);
    SigmaL += 318 * sin(A2);
    SigmaL *= deg2rad;
    //And finally apply the nutation in longitude
    const double NutationInLong{ AEarth::NutationInLongitude(jd_tt) }; // radians

    return ACoord::rangezero2tau(Ldash + (SigmaL / 1'000'000) + (NutationInLong));
}
double AMoon::RadiusVector(double jd_tt) noexcept
{
    double D{ MeanElongation(jd_tt) };
    double M{ AEarth::SunMeanAnomaly(jd_tt) };
    double Mdash{ MeanAnomaly(jd_tt) };
    double F{ ArgumentOfLatitude(jd_tt) };
    const double E{ AEarth::Eccentricity(jd_tt) };
    const double Esquared{ E * E };

    constexpr size_t nRCoefficients{ g_MoonCoefficients1.size() };
    assert(nRCoefficients == g_MoonCoefficients2.size());
    double SigmaR{ 0 };
    for (size_t i{ 0 }; i < nRCoefficients; i++) {
        double ThisSigma{ g_MoonCoefficients2[i].B * cos((g_MoonCoefficients1[i].D * D) + (g_MoonCoefficients1[i].M * M) +
                                                      (g_MoonCoefficients1[i].Mdash * Mdash) + (g_MoonCoefficients1[i].F * F)) };

        if ((g_MoonCoefficients1[i].M == 1) || (g_MoonCoefficients1[i].M == -1)) ThisSigma *= E;
        else if ((g_MoonCoefficients1[i].M == 2) || (g_MoonCoefficients1[i].M == -2)) ThisSigma *= Esquared;
        SigmaR += ThisSigma;
    }

    return 385000.56 + (SigmaR / 1000);  // in km
}
double AMoon::EclipticLatitude(double jd_tt) noexcept {
    double Ldash{ MeanLongitude(jd_tt) };
    double D{ MeanElongation(jd_tt) };
    double M{ AEarth::SunMeanAnomaly(jd_tt) };
    double Mdash{ MeanAnomaly(jd_tt) };
    double F{ ArgumentOfLatitude(jd_tt) };

    const double E{ AEarth::Eccentricity(jd_tt) };
    const double Esquared{ E * E };
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };

    double A1{ ACoord::rangezero2tau(deg2rad * (119.75 + (131.849 * T))) };
    double A3{ ACoord::rangezero2tau(deg2rad * (313.45 + (481266.484 * T))) };

    constexpr size_t nBCoefficients{ g_MoonCoefficients3.size() };
    assert(nBCoefficients == g_MoonCoefficients4.size());
    double SigmaB{ 0 };
    for (size_t i{ 0 }; i < nBCoefficients; i++) {
        double ThisSigma{ g_MoonCoefficients4[i] * sin((g_MoonCoefficients3[i].D * D) + (g_MoonCoefficients3[i].M * M) +
                                                    (g_MoonCoefficients3[i].Mdash * Mdash) + (g_MoonCoefficients3[i].F * F)) };

        if ((g_MoonCoefficients3[i].M == 1) || (g_MoonCoefficients3[i].M == -1)) ThisSigma *= E;
        else if ((g_MoonCoefficients3[i].M == 2) || (g_MoonCoefficients3[i].M == -2)) ThisSigma *= Esquared;
        SigmaB += ThisSigma;
    }

    //Finally the additive terms
    SigmaB -= 2235 * sin(Ldash);
    SigmaB += 382 * sin(A3);
    SigmaB += 175 * sin(A1 - F);
    SigmaB += 175 * sin(A1 + F);
    SigmaB += 127 * sin(Ldash - Mdash);
    SigmaB -= 115 * sin(Ldash + Mdash);

    return deg2rad * SigmaB / 1'000'000;  // radians
}
double AMoon::RadiusVectorToHorizontalParallax(double RadiusVector) noexcept {
    return asin(6378.14 / RadiusVector);  // parallax in radians
}
double AMoon::HorizontalParallaxToRadiusVector(double Parallax) noexcept {
    return 6378.14 / sin(Parallax);  // distance in km
}
double AMoon::MeanLongitudeAscendingNode(double jd_tt) noexcept {
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double Tcubed{ Tsquared * T };
    const double T4{ Tcubed * T };
    return ACoord::rangezero2tau(deg2rad * (125.0445479 - (1934.1362891 * T) + (0.0020754 * Tsquared) + (Tcubed / 467441) - (T4 / 60616000)));
}
double AMoon::MeanLongitudePerigee(double jd_tt) noexcept {
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double Tcubed{ Tsquared * T };
    const double T4{ Tcubed * T };
    return ACoord::rangezero2tau(deg2rad * (83.3532465 + (4069.0137287 * T) - (0.0103200 * Tsquared) - (Tcubed / 80053) + (T4 / 18999000)));
}
double AMoon::TrueLongitudeAscendingNode(double jd_tt) noexcept {
    double TrueAscendingNode{ MeanLongitudeAscendingNode(jd_tt) };

    double D{ MeanElongation(jd_tt) };
    double M{ AEarth::SunMeanAnomaly(jd_tt) };
    double Mdash{ MeanAnomaly(jd_tt) };
    double F{ ArgumentOfLatitude(jd_tt) };

    //Add the principal additive terms
    TrueAscendingNode -= 1.4979 * sin(2 * (D - F));
    TrueAscendingNode -= 0.1500 * sin(M);
    TrueAscendingNode -= 0.1226 * sin(2 * D);
    TrueAscendingNode += 0.1176 * sin(2 * F);
    TrueAscendingNode -= 0.0801 * sin(2 * (Mdash - F));

    return ACoord::rangezero2tau(deg2rad * TrueAscendingNode);
}


// Moon Illuminated Fraction - MEEUS98 Chapter 48
// !!! FIX: Clean up parameter names, and move to acoordinates, as this is not Moon specific
double AMoon::GeocentricElongation(double ObjectAlpha, double ObjectDelta, double SunAlpha, double SunDelta) noexcept {
    // Object = Moon
    // Generic formula for two objects, should be moved to acoordinates
    return acos((sin(SunDelta) * sin(ObjectDelta)) + (cos(SunDelta) * cos(ObjectDelta) * cos(SunAlpha - ObjectAlpha)));
}
double AMoon::PhaseAngle(double GeocentricElongation, double EarthObjectDistance, double EarthSunDistance) noexcept {
    // Elongation and Distance of Moon, and distance of Sun - Think this will work for other bodies, so move to acoordinates
    return ACoord::rangezero2tau(atan2(EarthSunDistance * sin(GeocentricElongation), EarthObjectDistance - (EarthSunDistance * cos(GeocentricElongation))));
}
double AMoon::IlluminatedFraction(double PhaseAngle) noexcept {
    return (1 + cos(PhaseAngle)) / 2; // Retval is a fraction, so no units
}
double AMoon::PositionAngle(double Alpha0, double Delta0, double Alpha, double Delta) noexcept {
    // 0 is the Sun, other arg is the object (Moon). Should also be moved to acoordinates or aearth
    return ACoord::rangezero2tau(atan2(cos(Delta0) * sin(Alpha0 - Alpha), (sin(Delta0) * cos(Delta)) - (cos(Delta0) * sin(Delta) * cos(Alpha0 - Alpha))));
}


// Moon Phases
// MEEUS98 Chapter 49
double AMoon::TruePhase(double jd_tt, Lunar_Phase phase) {
    double fracyear = (jd_tt - JD_0000) / JD_YEAR;
    double K = (double)int(Phase_K(fracyear));
    if (K < 0.0) K -= 1.0;               // Ensure we get a K earlier than the desired time
    K += phase * 0.25;
    double jde = TruePhaseK(K);   // If this is outside the desired lunation, recalculate K and try again
    if (jde < jd_tt) {
        return TruePhaseK(K + 1.0);
    }
    return jde;
}
// From AA+ v2.49 - CAAMoonPhases
double AMoon::TruePhaseK(double k) noexcept {
    //What will be the return value
    double JD{ MeanPhase(k) };
    //convert from K to T
    const double T{ k / 1236.85 };
    const double T2{ T * T };
    const double T3{ T2 * T };
    const double T4{ T3 * T };
    const double E{ 1 - (0.002516 * T) - (0.0000074 * T2) };
    const double E2{ E * E };
    double M{ ACoord::rangezero2tau(deg2rad * (2.5534 + (29.10535670 * k) - (0.0000014 * T2) - (0.00000011 * T3))) };
    double Mdash{ ACoord::rangezero2tau(deg2rad * (201.5643 + (385.81693528 * k) + (0.0107582 * T2) + (0.00001238 * T3) - (0.000000058 * T4))) };
    double F{ ACoord::rangezero2tau(deg2rad * (160.7108 + (390.67050284 * k) - (0.0016118 * T2) - (0.00000227 * T3) + (0.000000011 * T4))) };
    double omega{ ACoord::rangezero2tau(deg2rad * (124.7746 - (1.56375588 * k) + (0.0020672 * T2) + (0.00000215 * T3))) };
    double A1{ ACoord::rangezero2tau(deg2rad * (299.77 + (0.107408 * k) - (0.009173 * T2))) };
    double A2{ ACoord::rangezero2tau(deg2rad * (251.88 + (0.016321 * k))) };
    double A3{ ACoord::rangezero2tau(deg2rad * (251.83 + (26.651886 * k))) };
    double A4{ ACoord::rangezero2tau(deg2rad * (349.42 + (36.412478 * k))) };
    double A5{ ACoord::rangezero2tau(deg2rad * (84.66 + (18.206239 * k))) };
    double A6{ ACoord::rangezero2tau(deg2rad * (141.74 + (53.303771 * k))) };
    double A7{ ACoord::rangezero2tau(deg2rad * (207.14 + (2.453732 * k))) };
    double A8{ ACoord::rangezero2tau(deg2rad * (154.84 + (7.306860 * k))) };
    double A9{ ACoord::rangezero2tau(deg2rad * (34.52 + (27.261239 * k))) };
    double A10{ ACoord::rangezero2tau(deg2rad * (207.19 + (0.121824 * k))) };
    double A11{ ACoord::rangezero2tau(deg2rad * (291.34 + (1.844379 * k))) };
    double A12{ ACoord::rangezero2tau(deg2rad * (161.72 + (24.198154 * k))) };
    double A13{ ACoord::rangezero2tau(deg2rad * (239.56 + (25.513099 * k))) };
    double A14{ ACoord::rangezero2tau(deg2rad * (331.55 + (3.592518 * k))) };
    double kint{ 0 };
    double kfrac{ modf(k, &kint) };
    if (kfrac < 0) kfrac = 1 + kfrac;
    const double twoMdash{ 2 * Mdash };
    const double twoF{ 2 * F };
    const double twoM{ 2 * M };
    const double threeMdash{ 3 * Mdash };
    if (kfrac == 0) //New Moon
    {
        const double DeltaJD{ (-0.40720 * sin(Mdash)) +
                             (0.17241 * E * sin(M)) +
                             (0.01608 * sin(twoMdash)) +
                             (0.01039 * sin(twoF)) +
                             (0.00739 * E * sin(Mdash - M)) +
                             (-0.00514 * E * sin(Mdash + M)) +
                             (0.00208 * E2 * sin(twoM)) +
                             (-0.00111 * sin(Mdash - twoF)) +
                             (-0.00057 * sin(Mdash + twoF)) +
                             (0.00056 * E * sin(twoMdash + M)) +
                             (-0.00042 * sin(threeMdash)) +
                             (0.00042 * E * sin(M + twoF)) +
                             (0.00038 * E * sin(M - twoF)) +
                             (-0.00024 * E * sin(twoMdash - M)) +
                             (-0.00017 * sin(omega)) +
                             (-0.00007 * sin(Mdash + twoM)) +
                             (0.00004 * sin(twoMdash - twoF)) +
                             (0.00004 * sin(3 * M)) +
                             (0.00003 * sin(Mdash + M - twoF)) +
                             (0.00003 * sin(twoMdash + twoF)) +
                             (-0.00003 * sin(Mdash + M + twoF)) +
                             (0.00003 * sin(Mdash - M + twoF)) +
                             (-0.00002 * sin(Mdash - M - twoF)) +
                             (-0.00002 * sin(threeMdash + M)) +
                             (0.00002 * sin(4 * Mdash)) };
        JD += DeltaJD;
    }
    else if ((kfrac == 0.25) || (kfrac == 0.75)) //First Quarter or Last Quarter
    {
        const double DeltaJD{ (-0.62801 * sin(Mdash)) +
                             (0.17172 * E * sin(M)) +
                             (-0.01183 * E * sin(Mdash + M)) +
                             (0.00862 * sin(twoMdash)) +
                             (0.00804 * sin(twoF)) +
                             (0.00454 * E * sin(Mdash - M)) +
                             (0.00204 * E2 * sin(twoM)) +
                             (-0.00180 * sin(Mdash - twoF)) +
                             (-0.00070 * sin(Mdash + twoF)) +
                             (-0.00040 * sin(threeMdash)) +
                             (-0.00034 * E * sin(twoMdash - M)) +
                             (0.00032 * E * sin(M + twoF)) +
                             (0.00032 * E * sin(M - twoF)) +
                             (-0.00028 * E2 * sin(Mdash + twoM)) +
                             (0.00027 * E * sin(twoMdash + M)) +
                             (-0.00017 * sin(omega)) +
                             (-0.00005 * sin(Mdash - M - twoF)) +
                             (0.00004 * sin(twoMdash + twoF)) +
                             (-0.00004 * sin(Mdash + M + twoF)) +
                             (0.00004 * sin(Mdash - twoM)) +
                             (0.00003 * sin(Mdash + M - twoF)) +
                             (0.00003 * sin(3 * M)) +
                             (0.00002 * sin(twoMdash - twoF)) +
                             (0.00002 * sin(Mdash - M + twoF)) +
                             (-0.00002 * sin(threeMdash + M)) };
        JD += DeltaJD;
        const double W{ 0.00306 - (0.00038 * E * cos(M)) + (0.00026 * cos(Mdash)) - (0.00002 * cos(Mdash - M)) + (0.00002 * cos(Mdash + M)) + (0.00002 * cos(twoF)) };
        if (kfrac == 0.25) //First quarter
            JD += W;
        else
            JD -= W;
    }
    else if (kfrac == 0.5) //Full Moon
    {
        const double DeltaJD{ (-0.40614 * sin(Mdash)) +
                             (0.17302 * E * sin(M)) +
                             (0.01614 * sin(twoMdash)) +
                             (0.01043 * sin(twoF)) +
                             (0.00734 * E * sin(Mdash - M)) +
                             (-0.00514 * E * sin(Mdash + M)) +
                             (0.00209 * E2 * sin(twoM)) +
                             (-0.00111 * sin(Mdash - twoF)) +
                             (-0.00057 * sin(Mdash + twoF)) +
                             (0.00056 * E * sin(twoMdash + M)) +
                             (-0.00042 * sin(threeMdash)) +
                             (0.00042 * E * sin(M + twoF)) +
                             (0.00038 * E * sin(M - twoF)) +
                             (-0.00024 * E * sin(twoMdash - M)) +
                             (-0.00017 * sin(omega)) +
                             (-0.00007 * sin(Mdash + twoM)) +
                             (0.00004 * sin(twoMdash - twoF)) +
                             (0.00004 * sin(3 * M)) +
                             (0.00003 * sin(Mdash + M - twoF)) +
                             (0.00003 * sin(twoMdash + twoF)) +
                             (-0.00003 * sin(Mdash + M + twoF)) +
                             (0.00003 * sin(Mdash - M + twoF)) +
                             (-0.00002 * sin(Mdash - M - twoF)) +
                             (-0.00002 * sin(threeMdash + M)) +
                             (0.00002 * sin(4 * Mdash)) };
        JD += DeltaJD;
    }
    else {
        assert(false);
    }
    //Additional corrections for all phases
    const double DeltaJD2{ (0.000325 * sin(A1)) +
                          (0.000165 * sin(A2)) +
                          (0.000164 * sin(A3)) +
                          (0.000126 * sin(A4)) +
                          (0.000110 * sin(A5)) +
                          (0.000062 * sin(A6)) +
                          (0.000060 * sin(A7)) +
                          (0.000056 * sin(A8)) +
                          (0.000047 * sin(A9)) +
                          (0.000042 * sin(A10)) +
                          (0.000040 * sin(A11)) +
                          (0.000037 * sin(A12)) +
                          (0.000035 * sin(A13)) +
                          (0.000023 * sin(A14)) };
    JD += DeltaJD2;
    return JD;
}

// Lunar Nodes
double AMoon::PassageThroNode(double jd_tt, Node node) {
    double fracyear = (jd_tt - JD_0000) / JD_YEAR;
    //EDateTime moonjd{ jd_tt, true };
    //fracyear = moonjd.year() + (moonjd.dayofyear() - 1.0) / JD_YEAR; // leave out hrs, min, sec to ensure we get early K
    //std::cout << "fracyear: " << fracyear << " ";
    double K = (double)int(Node_K(fracyear));
    if (K < 0.0) K -= 1.0;               // Ensure we get a K earlier than the desired time
    K += node * 0.5;
    double jde = PassageThroNodeK(K);   // If this is outside the desired lunation, recalculate K and try again
    if (jde < jd_tt) {
        std::cout << " *> ";
        return PassageThroNodeK(K + 1.0);
    }
    return jde;
}
double AMoon::PassageThroNodeK(double k) noexcept {
    // MEEUS98 chapter 51
    // AA+ v2.49 CAAMoonNodes
    //convert from K to T
    const double T{ k / 1342.23 };
    const double T2{ T * T };
    const double T3{ T2 * T };
    const double T4{ T3 * T };
    double D{ deg2rad * ACoord::rangezero2threesixty(183.6380 + (331.73735682 * k) + (0.0014852 * T2) + (0.00000209 * T3) - (0.000000010 * T4)) };
    double M{ deg2rad * ACoord::rangezero2threesixty(17.4006 + (26.82037250 * k) + (0.0001186 * T2) + (0.00000006 * T3)) };
    double Mdash{ deg2rad * ACoord::rangezero2threesixty(38.3776 + (355.52747313 * k) + (0.0123499 * T2) + (0.000014627 * T3) - (0.000000069 * T4)) };
    double omega{ deg2rad * ACoord::rangezero2threesixty(123.9767 - (1.44098956 * k) + (0.0020608 * T2) + (0.00000214 * T3) - (0.000000016 * T4)) };
    double V{ deg2rad * ACoord::rangezero2threesixty(299.75 + (132.85 * T) - (0.009173 * T2)) };
    double P{ deg2rad * ACoord::rangezero2threesixty(omega + 272.75 - (2.3 * T)) };
    const double E{ 1 - (0.002516 * T) - (0.0000074 * T2) };
    const double twoD{ 2 * D };
    const double fourD{ twoD * 2 };
    const double twoMdash{ 2 * Mdash };
    const double JD{ 2451565.1619 +
                    (27.212220817 * k) +
                    (0.0002762 * T2) +
                    (0.000000021 * T3) -
                    (0.000000000088 * T4) -
                    (0.4721 * sin(Mdash)) -
                    (0.1649 * sin(twoD)) -
                    (0.0868 * sin(twoD - Mdash)) +
                    (0.0084 * sin(twoD + Mdash)) -
                    (E * 0.0083 * sin(twoD - M)) -
                    (E * 0.0039 * sin(twoD - M - Mdash)) +
                    (0.0034 * sin(twoMdash)) -
                    (0.0031 * sin(twoD - twoMdash)) +
                    (E * 0.0030 * sin(twoD + M)) +
                    (E * 0.0028 * sin(M - Mdash)) +
                    (E * 0.0026 * sin(M)) +
                    (0.0025 * sin(fourD)) +
                    (0.0024 * sin(D)) +
                    (E * 0.0022 * sin(M + Mdash)) +
                    (0.0017 * sin(omega)) +
                    (0.0014 * sin(fourD - Mdash)) +
                    (E * 0.0005 * sin(twoD + M - Mdash)) +
                    (E * 0.0004 * sin(twoD - M + Mdash)) -
                    (E * 0.0003 * sin(twoD - 2 * M)) +
                    (E * 0.0003 * sin(fourD - M)) +
                    (0.0003 * sin(V)) +
                    (0.0003 * sin(P)) };
    return JD;
}


void AMoon::CalculateOpticalLibration(double JD, double Lambda, double Beta, double& ldash, double& bdash, double& ldash2, double& bdash2, double& epsilon, double& omega, double& DeltaU, double& sigma, double& I, double& rho) noexcept
{
    //Calculate the initial quantities
    const double cosBetarad{ cos(Beta) };
    const double sinBetarad{ sin(Beta) };
    I = deg2rad * 1.54242;
    const double cosI{ cos(I) };
    const double sinI{ sin(I) };
    DeltaU = AEarth::NutationInLongitude(JD);
    const double F{ AMoon::ArgumentOfLatitude(JD) };
    const double twoF{ 2 * F };
    omega = AMoon::MeanLongitudeAscendingNode(JD);
    epsilon = AEarth::MeanObliquityOfEcliptic(JD) + AEarth::NutationInObliquity(JD);

    //Calculate the optical librations
    const double W{ Lambda - DeltaU - omega };
    const double sinW{ sin(W) };
    const double A{ atan2((sinW * cosBetarad * cosI) - (sinBetarad * sinI), cos(W) * cosBetarad) };
    const double cosA{ cos(A) };
    const double sinA{ sin(A) };
    ldash = ACoord::rangezero2tau(A - F);
    if (ldash > pi)
        ldash -= tau;
    bdash = asin(-(sinW * cosBetarad * sinI) - (sinBetarad * cosI));

    //Calculate the physical librations
    const double T{ (JD - 2451545) / 36525 };
    double K1{ deg2rad * (119.75 + (131.849 * T)) };
    //K1 = CAACoordinateTransformation::DegreesToRadians(K1);
    double K2{ deg2rad * (72.56 + (20.186 * T)) };
    //K2 = CAACoordinateTransformation::DegreesToRadians(K2);

    double M{ AEarth::SunMeanAnomaly(JD) };
    //M = CAACoordinateTransformation::DegreesToRadians(M);
    double Mdash{ AMoon::MeanAnomaly(JD) };
    //Mdash = CAACoordinateTransformation::DegreesToRadians(Mdash);
    const double twoMdash{ 2 * Mdash };
    double D{ AMoon::MeanElongation(JD) };
    //D = CAACoordinateTransformation::DegreesToRadians(D);
    const double twoD{ 2 * D };
    const double E{ AEarth::Eccentricity(JD) };

    rho = (-0.02752 * cos(Mdash)) +
        (-0.02245 * sin(F)) +
        (0.00684 * cos(Mdash - twoF)) +
        (-0.00293 * cos(twoF)) +
        (-0.00085 * cos(twoF - twoD)) +
        (-0.00054 * cos(Mdash - twoD)) +
        (-0.00020 * sin(Mdash + F)) +
        (-0.00020 * cos(Mdash + twoF)) +
        (-0.00020 * cos(Mdash - F)) +
        (0.00014 * cos(Mdash + twoF - twoD));

    sigma = (-0.02816 * sin(Mdash)) +
        (0.02244 * cos(F)) +
        (-0.00682 * sin(Mdash - twoF)) +
        (-0.00279 * sin(twoF)) +
        (-0.00083 * sin(twoF - twoD)) +
        (0.00069 * sin(Mdash - twoD)) +
        (0.00040 * cos(Mdash + F)) +
        (-0.00025 * sin(twoMdash)) +
        (-0.00023 * sin(Mdash + twoF)) +
        (0.00020 * cos(Mdash - F)) +
        (0.00019 * sin(Mdash - F)) +
        (0.00013 * sin(Mdash + twoF - twoD)) +
        (-0.00010 * cos(Mdash - 3 * F));

    const double tau_m{ (0.02520 * E * sin(M)) +
                     (0.00473 * sin(twoMdash - twoF)) +
                     (-0.00467 * sin(Mdash)) +
                     (0.00396 * sin(K1)) +
                     (0.00276 * sin(twoMdash - twoD)) +
                     (0.00196 * sin(omega)) +
                     (-0.00183 * cos(Mdash - F)) +
                     (0.00115 * sin(Mdash - twoD)) +
                     (-0.00096 * sin(Mdash - D)) +
                     (0.00046 * sin(twoF - twoD)) +
                     (-0.00039 * sin(Mdash - F)) +
                     (-0.00032 * sin(Mdash - M - D)) +
                     (0.00027 * sin(twoMdash - M - twoD)) +
                     (0.00023 * sin(K2)) +
                     (-0.00014 * sin(twoD)) +
                     (0.00014 * cos(twoMdash - twoF)) +
                     (-0.00012 * sin(Mdash - twoF)) +
                     (-0.00012 * sin(twoMdash)) +
                     (0.00011 * sin(twoMdash - 2 * M - twoD)) };

    ldash2 = deg2rad * ( -tau_m + (rho * cosA) + (sigma * sinA * tan(bdash)));
    //bdash = CAACoordinateTransformation::RadiansToDegrees(bdash);
    bdash2 = (sigma * cosA) - (rho * sinA);
}

APhysicalMoonDetails AMoon::CalculateHelper(double JD, double& Lambda, double& Beta, double& epsilon, LLD& Equatorial) noexcept
{
    //What will be the return value
    APhysicalMoonDetails details;

    //Calculate the initial quantities
    Lambda = AMoon::EclipticLongitude(JD);
    Beta = AMoon::EclipticLatitude(JD);

    //Calculate the optical libration
    double omega{ 0 };
    double DeltaU{ 0 };
    double sigma{ 0 };
    double I{ 0 };
    double rho{ 0 };
    CalculateOpticalLibration(JD, Lambda, Beta, details.ldash, details.bdash, details.ldash2, details.bdash2, epsilon, omega, DeltaU, sigma, I, rho);
    //const double epsilonrad{ CAACoordinateTransformation::DegreesToRadians(epsilon) };

    //Calculate the total libration
    details.l = details.ldash + details.ldash2;
    details.b = details.bdash + details.bdash2;
    //const double b{ CAACoordinateTransformation::DegreesToRadians(details.b) };

    //Calculate the position angle
    const double V{ omega + DeltaU + sigma / sin(I) };
    const double I_rho{ I + rho };
    const double sinI_rho{ sin(I_rho) };
    const double X{ sinI_rho * sin(V) };
    const double Y{ sinI_rho * cos(V) * cos(epsilon) - cos(I_rho) * sin(epsilon) };
    const double w{ atan2(X, Y) };

    Equatorial = Spherical::Ecliptic2Equatorial2(Lambda, Beta, epsilon);
    details.P = asin(sqrt((X * X) + (Y * Y)) * cos(Equatorial.lon - w) / (cos(details.b)));
    return details;
}

APhysicalMoonDetails AMoon::CalculateGeocentric(double JD) noexcept {
    double Lambda{ 0 };
    double Beta{ 0 };
    double epsilon{ 0 };
    LLD Equatorial;
    return CalculateHelper(JD, Lambda, Beta, epsilon, Equatorial);
}

APhysicalMoonDetails AMoon::CalculateTopocentric(double JD, double Longitude, double Latitude) noexcept
{
    //First convert to radians
    //Longitude = CAACoordinateTransformation::DegreesToRadians(Longitude);
    //Latitude = CAACoordinateTransformation::DegreesToRadians(Latitude);
    const double cosLatitude{ cos(Latitude) };
    const double sinLatitude{ sin(Latitude) };

    double Lambda{ 0 };
    double Beta{ 0 };
    double epsilon{ 0 };
    LLD Equatorial;
    APhysicalMoonDetails details{ CalculateHelper(JD, Lambda, Beta, epsilon, Equatorial) };

    const double R{ AMoon::RadiusVector(JD) };
    const double pi_m{ AMoon::RadiusVectorToHorizontalParallax(R) };
    const double Alpha{ Equatorial.lon };
    const double Delta{ Equatorial.lat };
    const double cosDelta{ cos(Delta) };
    const double sinDelta{ sin(Delta) };

    const double AST{ AEarth::ApparentGreenwichSiderealTime(JD) };
    const double H{ AST - Longitude - Alpha };
    const double cosH{ cos(H) };

    const double Q{ atan2(cosLatitude * sin(H), (cosDelta * sinLatitude) - (sinDelta * cosLatitude * cosH)) };
    const double Z{ acos((sinDelta * sinLatitude) + (cosDelta * cosLatitude * cosH)) };
    const double pidash{ pi_m * (sin(Z) + (0.0084 * sin(2 * Z))) };

    const double DeltaL{ -pidash * sin(Q - details.P) / cos(details.b) };
    details.l += DeltaL;
    const double DeltaB{ pidash * cos(Q - details.P) };
    details.b += DeltaB;
    const double DeltaP{ DeltaL * sin(details.b) - (pidash * sin(Q) * tan(Delta)) };
    details.P += DeltaP;
    return details;
}

ASelenographicMoonDetails AMoon::CalculateSelenographicPositionOfSun(double JD, Lunar_Ephemeris eph) noexcept
{
    //What will be the return value
    ASelenographicMoonDetails details;

    Planetary_Ephemeris eph2{ EPH_VSOP87_FULL };
    if (eph == MEEUS_SHORT) eph2 = EPH_VSOP87_SHORT;

    const double R{ AEarth::EclipticDistance(JD, eph2) * 149597970 };
    const double Delta{ AMoon::RadiusVector(JD) };
    const double lambda0{ ASun::ApparentEclipticLongitude(JD, eph2) };
    const double lambda{ AMoon::EclipticLongitude(JD) };
    const double beta{ AMoon::EclipticLatitude(JD) };

    const double lambdah{ ACoord::rangezero2threesixty(lambda0 + 180 + (Delta / R * 57.296 * cos(beta) * sin(lambda0 - lambda))) };
    const double betah{ Delta / R * beta };

    //Calculate the optical libration
    double omega{ 0 };
    double DeltaU{ 0 };
    double sigma{ 0 };
    double I{ 0 };
    double rho{ 0 };
    double ldash0{ 0 };
    double bdash0{ 0 };
    double ldash20{ 0 };
    double bdash20{ 0 };
    double epsilon{ 0 };
    CalculateOpticalLibration(JD, lambdah, betah, ldash0, bdash0, ldash20, bdash20, epsilon, omega, DeltaU, sigma, I, rho);

    details.l0 = ldash0 + ldash20;
    details.b0 = bdash0 + bdash20;
    details.c0 = ACoord::rangezero2tau((deg2rad * 450) - details.l0);
    return details;
}

double AMoon::AltitudeOfSun(double JD, double Longitude, double Latitude, Lunar_Ephemeris eph) noexcept
{
    //Calculate the selenographic details
    ASelenographicMoonDetails selenographicDetails{ CalculateSelenographicPositionOfSun(JD, eph) };
    //selenographicDetails.b0 = CAACoordinateTransformation::DegreesToRadians(selenographicDetails.b0);
    //selenographicDetails.c0 = CAACoordinateTransformation::DegreesToRadians(selenographicDetails.c0);

    // Buggy! Fixed to below in AA+ v2.52
    //return asin(sin((selenographicDetails.b0) * sin(Latitude)) + (cos(selenographicDetails.b0) * cos(Latitude) * sin(selenographicDetails.c0 + Longitude)));
    return asin((sin(selenographicDetails.b0) * sin(Latitude)) + (cos(selenographicDetails.b0) * cos(Latitude) * sin(selenographicDetails.c0 + Longitude)));
}

double AMoon::SunriseSunsetHelper(double JD, double Longitude, double Latitude, bool bSunrise, Lunar_Ephemeris eph) noexcept {
    double JDResult{ JD };
    //const double Latituderad{ CAACoordinateTransformation::DegreesToRadians(Latitude) };
    double h{ 0 };
    do {
        h = rad2deg * AltitudeOfSun(JDResult, Longitude, Latitude, eph);
        const double DeltaJD{ h / (12.19075 * cos(Latitude)) };
        if (bSunrise)
            JDResult -= DeltaJD;
        else
            JDResult += DeltaJD;
    } while (fabs(h) > 0.001);
    return JDResult;
}

double AMoon::TimeOfSunrise(double JD, double Longitude, double Latitude, Lunar_Ephemeris eph) noexcept {
    return SunriseSunsetHelper(JD, Longitude, Latitude, true, eph);
}

double AMoon::TimeOfSunset(double JD, double Longitude, double Latitude, Lunar_Ephemeris eph) noexcept{
    return SunriseSunsetHelper(JD, Longitude, Latitude, false, eph);
}

// Diameter - From AA+ v2.55 CAADiameters
// updated the "k" constant used in this method from 0.272481 (MEEUS98) to the more modern value of k = 0.2725076.
// See section "1.9 Mean Lunar Radius" of https://umbra.nascom.nasa.gov/eclipse/20060329/text/chapter_1.html
// for the reason why the new value was adopted in 1982
double AMoon::GeocentricMoonSemidiameter(double Delta) noexcept {
    return asin(0.2725076 * 6378.14 / Delta);
}
double AMoon::TopocentricMoonSemidiameter(double DistanceDelta, double Delta, double H, double Latitude, double Height) noexcept {
    const double pi_{ asin(6378.14 / DistanceDelta) };
    const double cosDelta{ cos(Delta) };
    const double A{ cosDelta * sin(H) };
    const double sinPi{ sin(pi_) };
    const double B{ (cosDelta * cos(H)) - (AEarth::RhoCosPhiPrime(Latitude, Height) * sinPi) };
    const double C{ sin(Delta) - (AEarth::RhoSinPhiPrime(Latitude, Height) * sinPi) };
    const double q{ sqrt((A * A) + (B * B) + (C * C)) };

    const double s{ GeocentricMoonSemidiameter(DistanceDelta) };
    return asin(sin(s) / q);
}





// ELP2000

// Return values in the few public facing functions have been converted to return angles in radians
// Distances and rectangular coordinates are in kilometers (km)
// All other internally used functions receive and return whichever units are convenient for them


// MDO simply calculate lat,lon,dst together to cut down on redundancy
LLD AELP2000::EclipticCoordinates(double jd_tt) {
    LLD retval{};
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (jd_tt - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    double* pT{ t.data() };
    //size_t nTSize{ t.size() };
    int nTSize{ (int)t.size() };   // Should ideally be a size_t, but is never > 5. Leave as int for Accumulate functions.

    // Moon Mean Mean Longitude (W1)
    const double fW1_2{ g_W[0] + (g_W[3] * pT[1]) };
    const double fW1{ fW1_2 + ((g_W[6] * pT[2]) + (g_W[9] * pT[3]) + (g_W[12] * pT[4])) };
    // Mean Longitude Lunar Perigee (W2)
    const double fW2_2{ g_W[1] + (g_W[4] * pT[1]) };
    const double fW2{ fW2_2 + ((g_W[7] * pT[2]) + (g_W[10] * pT[3]) + (g_W[13] * pT[4])) };
    // Mean Longitude Lunar Ascending Node (W3)
    const double fW3_2{ g_W[2] + (g_W[5] * pT[1]) };
    const double fW3{ fW3_2 + ((g_W[8] * pT[2]) + (g_W[11] * pT[3]) + (g_W[14] * pT[4])) };
    // Mean Heliocentric Mean Longitude Earth Moon Barycentre (T)
    const double fT_2{ g_EARTH[0] + (g_EARTH[1] * pT[1]) };
    const double fT{ fT_2 + ((g_EARTH[2] * pT[2]) + (g_EARTH[3] * pT[3]) + (g_EARTH[4] * pT[4])) };
    // Mean Longitude Of Perilhelion Of Earth Moon Barycentre (Omega')
    const double fOmegadash_2{ g_PERI[0] + (g_PERI[1] * pT[1]) };
    const double fOmegadash{fOmegadash_2 + ((g_PERI[2] * pT[2]) + (g_PERI[3] * pT[3])) };

    //Compute the Delaunay arguments for the specified time
    // Moon Mean Solar Elongation (D = W1 - T + pi)
    const double fD2{ fW1_2 - fT_2 + pi};
    const double fD{ fW1 - fT + pi};
    // Sun Mean Anomaly (l' = T - Omega')
    const double fldash2{ fT_2 - fOmegadash_2 };
    const double fldash{ fT - fOmegadash };
    // Moon Mean Anomaly (l = W1 - W2)
    const double fl2{ fW1_2 - fW2_2 };
    const double fl{ fW1 - fW2 };
    // Moon Mean Argument Of Latitude (F = W1 - W3)
    const double fF2{ fW1_2 - fW3_2 };
    const double fF{ fW1 - fW3 };

    // Planet Mean Longitude for specified time
    const double fMe{ g_Pelp2k[0][0] + (g_Pelp2k[0][1] * pT[1]) };  // Mercury
    const double fV{ g_Pelp2k[1][0] + (g_Pelp2k[1][1] * pT[1]) };   // Venus
    const double fMa{ g_Pelp2k[3][0] + (g_Pelp2k[3][1] * pT[1]) };  // Mars
    const double fJ{ g_Pelp2k[4][0] + (g_Pelp2k[4][1] * pT[1]) };   // Jupiter
    const double fS{ g_Pelp2k[5][0] + (g_Pelp2k[5][1] * pT[1]) };   // Saturn
    const double fU{ g_Pelp2k[6][0] + (g_Pelp2k[6][1] * pT[1]) };   // Uranus
    const double fN{ g_Pelp2k[7][0] + (g_Pelp2k[7][1] * pT[1]) };   // Neptune


    // Calculate the Latitude
    const double B{ Accumulate(g_ELP2.data(), g_ELP2.size(), fD, fldash, fl, fF) +
                   Accumulate(pT, nTSize, g_ELP5.data(), g_ELP5.size(), fD2, fldash2, fl2, fF2, false) +
                   Accumulate_2(pT, nTSize, g_ELP8.data(), g_ELP8.size(), fD2, fldash2, fl2, fF2, false) +
                   AccumulateTable1(g_ELP11.data(), g_ELP11.size(), fD2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU, fN) +
                   AccumulateTable1_2(pT, nTSize, g_ELP14.data(), g_ELP14.size(), fD2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU, fN) +
                   AccumulateTable2(g_ELP17.data(), g_ELP17.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU) +
                   AccumulateTable2_2(pT, nTSize, g_ELP20.data(), g_ELP20.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU) +
                   Accumulate(pT, nTSize, g_ELP23.data(), g_ELP23.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate_2(pT, nTSize, g_ELP26.data(), g_ELP26.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate(pT, nTSize, g_ELP29.data(), g_ELP29.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate(pT, nTSize, g_ELP32.data(), g_ELP32.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate_3(pT, nTSize, g_ELP35.data(), g_ELP35.size(), fD2, fldash2, fl2, fF2) };

    // Calculate the Longitude
    const double A{ Accumulate(g_ELP1.data(), g_ELP1.size(), fD, fldash, fl, fF) +
                   Accumulate(pT, nTSize, g_ELP4.data(), g_ELP4.size(), fD2, fldash2, fl2, fF2, false) +
                   Accumulate_2(pT, nTSize, g_ELP7.data(), g_ELP7.size(), fD2, fldash2, fl2, fF2, false) +
                   AccumulateTable1(g_ELP10.data(), g_ELP10.size(), fD2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU, fN) +
                   AccumulateTable1_2(pT, nTSize, g_ELP13.data(), g_ELP13.size(), fD2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU, fN) +
                   AccumulateTable2(g_ELP16.data(), g_ELP16.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU) +
                   AccumulateTable2_2(pT, nTSize, g_ELP19.data(), g_ELP19.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU) +
                   Accumulate(pT, nTSize, g_ELP22.data(), g_ELP22.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate_2(pT, nTSize, g_ELP25.data(), g_ELP25.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate(pT, nTSize, g_ELP28.data(), g_ELP28.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate(pT, nTSize, g_ELP31.data(), g_ELP31.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate_3(pT, nTSize, g_ELP34.data(), g_ELP34.size(), fD2, fldash2, fl2, fF2) };

    // Calculate the Distance
    const double fValue{ Accumulate_2(g_ELP3.data(), g_ELP3.size(), fD, fldash, fl, fF) +
                        Accumulate(pT, nTSize, g_ELP6.data(), g_ELP6.size(), fD2, fldash2, fl2, fF2, false) +
                        Accumulate_2(pT, nTSize, g_ELP9.data(), g_ELP9.size(), fD2, fldash2, fl2, fF2, false) +
                        AccumulateTable1(g_ELP12.data(), g_ELP12.size(), fD2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU, fN) +
                        AccumulateTable1_2(pT, nTSize, g_ELP15.data(), g_ELP15.size(), fD2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU, fN) +
                        AccumulateTable2(g_ELP18.data(), g_ELP18.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU) +
                        AccumulateTable2_2(pT, nTSize, g_ELP21.data(), g_ELP21.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT_2, fMa, fJ, fS, fU) +
                        Accumulate(pT, nTSize, g_ELP24.data(), g_ELP24.size(), fD2, fldash2, fl2, fF2, true) +
                        Accumulate_2(pT, nTSize, g_ELP27.data(), g_ELP27.size(), fD2, fldash2, fl2, fF2, true) +
                        Accumulate(pT, nTSize, g_ELP30.data(), g_ELP30.size(), fD2, fldash2, fl2, fF2, true) +
                        Accumulate(pT, nTSize, g_ELP33.data(), g_ELP33.size(), fD2, fldash2, fl2, fF2, true) +
                        Accumulate_3(pT, nTSize, g_ELP36.data(), g_ELP36.size(), fD2, fldash2, fl2, fF2) };

    retval.lat = (deg2rad * ACoord::rangemninety2ninety(B / 3600.0));  // radians
    retval.lon = ACoord::rangezero2tau(deg2rad * (A / 3600.0) + fW1);  // radians
    retval.dst = fValue * 384747.9806448954 / 384747.9806743165;       // kilometers (km)
    return retval;
}


// Public / documented (in AA+) functions, let them return radians for angles, km for distances
double AELP2000::EclipticLongitude(double JD) noexcept {
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return ACoord::rangezero2tau(deg2rad * EclipticLongitude(t.data(), 5));  // radians
}
double AELP2000::EclipticLatitude(double jd_tt) noexcept {
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (jd_tt - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return (deg2rad * EclipticLatitude(t.data(), 5));  // radians
}
double AELP2000::RadiusVector(double jd_tt) noexcept {
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (jd_tt - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return RadiusVector(t.data(), 5);  // kilometers (km)
}
glm::dvec3 AELP2000::EclipticRectangularCoordinates(double jd_tt) noexcept {
    return Spherical::Spherical2Rectangular(EclipticCoordinates(jd_tt));  // kilometers (km)
}
glm::dvec3 AELP2000::EclipticRectangularCoordinatesJ2000(double jd_tt) noexcept {
    std::array<double, 5> t{};
    t[0] = 1;
    t[1] = (jd_tt - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    const double P{ (1.0180391e-5 + (4.7020439e-7 * t[1]) + (-5.417367e-10 * t[2]) + (-2.507948e-12 * t[3]) + (4.63486e-15 * t[4])) * t[1] };
    const double Q{ (-1.13469002e-4 + (1.2372674e-7 * t[1]) + (1.265417e-9 * t[2]) + (-1.371808e-12 * t[3]) + (-3.20334e-15 * t[4])) * t[1] };
    const double TwoP{ 2 * P };
    const double P2{ P * P };
    const double Q2{ Q * Q };
    const double OneMinus2P2{ 1 - (2 * P2) };
    const double TwoPQ{ TwoP * Q };
    const double Twosqrt1MinusPart{ 2 * sqrt(1 - P2 - Q2) };
    const glm::dvec3 Ecliptic{ EclipticRectangularCoordinates(jd_tt) };
    glm::dvec3 J2000{ 0.0 };
    J2000.x = (OneMinus2P2 * Ecliptic.x) + (TwoPQ * Ecliptic.y) + (P * Twosqrt1MinusPart * Ecliptic.z);
    J2000.y = (TwoPQ * Ecliptic.x) + ((1 - 2 * Q2) * Ecliptic.y) - (Q * Twosqrt1MinusPart * Ecliptic.z);
    J2000.z = (-P * Twosqrt1MinusPart * Ecliptic.x) + (Q * Twosqrt1MinusPart * Ecliptic.y) + ((1 - 2 * P2 - 2 * Q2) * Ecliptic.z);
    return J2000;  // kilometers (km)
}
glm::dvec3 AELP2000::EquatorialRectangularCoordinatesFK5(double jd_tt) noexcept {
    const glm::dvec3 J2000{ EclipticRectangularCoordinatesJ2000(jd_tt) };
    glm::dvec3 FK5{ 0.0 };
    FK5.x = J2000.x + (0.000000437913 * J2000.y) - (0.000000189859 * J2000.z);
    FK5.y = (-0.000000477299 * J2000.x) + (0.917482137607 * J2000.y) - (0.397776981701 * J2000.z);
    FK5.z = (0.397776981701 * J2000.y) + (0.917482137607 * J2000.z);
    return FK5;  // kilometers (km)
}

// The below appear to be internal functions. Let them pass/return whichever units are convenient
double AELP2000::MoonMeanMeanLongitude(const double* pT, int nTSize) noexcept //Aka W1
{
    //Validate our parameters
    assert(pT);
    assert((nTSize == 5) || (nTSize == 2));
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pT);
#endif
    double fValue{ g_W[0] + (g_W[3] * pT[1]) };
    if (nTSize == 5)
        fValue += ((g_W[6] * pT[2]) + (g_W[9] * pT[3]) + (g_W[12] * pT[4]));
    return fValue;  // radians
}
double AELP2000::MoonMeanMeanLongitude(double JD) noexcept //Aka W1
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return MoonMeanMeanLongitude(t.data(), 5);
}
double AELP2000::MeanLongitudeLunarPerigee(const double* pT, int nTSize) noexcept //Aka W2
{
    //Validate our parameters
    assert(pT);
    assert((nTSize == 5) || (nTSize == 2));
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pT);
#endif
    double fValue{ g_W[1] + (g_W[4] * pT[1]) };
    if (nTSize == 5)
        fValue += ((g_W[7] * pT[2]) + (g_W[10] * pT[3]) + (g_W[13] * pT[4]));
    return fValue;
}
double AELP2000::MeanLongitudeLunarPerigee(double JD) noexcept //Aka W2
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return MeanLongitudeLunarPerigee(t.data(), 5);
}
double AELP2000::MeanLongitudeLunarAscendingNode(const double* pT, int nTSize) noexcept //Aka W3
{
    //Validate our parameters
    assert(pT);
    assert((nTSize == 5) || (nTSize == 2));
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pT);
#endif
    double fValue{ g_W[2] + (g_W[5] * pT[1]) };
    if (nTSize == 5)
        fValue += ((g_W[8] * pT[2]) + (g_W[11] * pT[3]) + (g_W[14] * pT[4]));
    return fValue;
}
double AELP2000::MeanLongitudeLunarAscendingNode(double JD) noexcept //Aka W3
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return MeanLongitudeLunarAscendingNode(t.data(), 5);
}
double AELP2000::MeanHeliocentricMeanLongitudeEarthMoonBarycentre(const double* pT, int nTSize) noexcept //Aka T
{
    //Validate our parameters
    assert(pT);
    assert((nTSize == 5) || (nTSize == 2));
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pT);
#endif
    double fValue{ g_EARTH[0] + (g_EARTH[1] * pT[1]) };
    if (nTSize == 5)
        fValue += ((g_EARTH[2] * pT[2]) + (g_EARTH[3] * pT[3]) + (g_EARTH[4] * pT[4]));
    return fValue;
}
double AELP2000::MeanHeliocentricMeanLongitudeEarthMoonBarycentre(double JD) noexcept //Aka T
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return MeanHeliocentricMeanLongitudeEarthMoonBarycentre(t.data(), 5);
}
double AELP2000::MeanLongitudeOfPerihelionOfEarthMoonBarycentre(const double* pT, int nTSize) noexcept //Aka Omega'
{
    //Validate our parameters
    assert(pT);
    assert((nTSize == 5) || (nTSize == 2));
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pT);
#endif
    double fValue{ g_PERI[0] + (g_PERI[1] * pT[1]) };
    if (nTSize == 5)
        fValue += ((g_PERI[2] * pT[2]) + (g_PERI[3] * pT[3]));
    return fValue;
}
double AELP2000::MeanLongitudeOfPerihelionOfEarthMoonBarycentre(double JD) noexcept //Aka Omega'
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return MeanLongitudeOfPerihelionOfEarthMoonBarycentre(t.data(), 5);
}
double AELP2000::MoonMeanSolarElongation(const double* pT, int nTSize) noexcept //Aka D
{
    //Implement D in terms of W1 and T
    return MoonMeanMeanLongitude(pT, nTSize) - MeanHeliocentricMeanLongitudeEarthMoonBarycentre(pT, nTSize) + pi;
}
double AELP2000::MoonMeanSolarElongation(double JD) noexcept //Aka D
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return MoonMeanSolarElongation(t.data(), 5);
}
double AELP2000::SunMeanAnomaly(const double* pT, int nTSize) noexcept //Aka l'
{
    //Implement l' in terms of T and Omega'
    return MeanHeliocentricMeanLongitudeEarthMoonBarycentre(pT, nTSize) - MeanLongitudeOfPerihelionOfEarthMoonBarycentre(pT, nTSize);
}
double AELP2000::SunMeanAnomaly(double JD) noexcept //Aka l'
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];

    return SunMeanAnomaly(t.data(), 5);
}
double AELP2000::MoonMeanAnomaly(const double* pT, int nTSize) noexcept //Aka l
{
    //Implement L in terms of W1 and W2
    return MoonMeanMeanLongitude(pT, nTSize) - MeanLongitudeLunarPerigee(pT, nTSize);
}
double AELP2000::MoonMeanAnomaly(double JD) noexcept //Aka l
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];

    return MoonMeanAnomaly(t.data(), 5);
}
double AELP2000::MoonMeanArgumentOfLatitude(const double* pT, int nTSize) noexcept //Aka F
{
    //Implement F in terms of W1 and W3
    return MoonMeanMeanLongitude(pT, nTSize) - MeanLongitudeLunarAscendingNode(pT, nTSize);
}
double AELP2000::MoonMeanArgumentOfLatitude(double JD) noexcept //Aka F
{
    //Calculate Julian centuries
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];

    return MoonMeanArgumentOfLatitude(t.data(), 5);
}
double AELP2000::MercuryMeanLongitude(double T) noexcept {
    return g_Pelp2k[0][0] + (g_Pelp2k[0][1] * T);
}
double AELP2000::VenusMeanLongitude(double T) noexcept {
    return g_Pelp2k[1][0] + (g_Pelp2k[1][1] * T);
}
double AELP2000::MarsMeanLongitude(double T) noexcept {
    return g_Pelp2k[3][0] + (g_Pelp2k[3][1] * T);
}
double AELP2000::JupiterMeanLongitude(double T) noexcept {
    return g_Pelp2k[4][0] + (g_Pelp2k[4][1] * T);
}
double AELP2000::SaturnMeanLongitude(double T) noexcept {
    return g_Pelp2k[5][0] + (g_Pelp2k[5][1] * T);
}
double AELP2000::UranusMeanLongitude(double T) noexcept {
    return g_Pelp2k[6][0] + (g_Pelp2k[6][1] * T);
}
double AELP2000::NeptuneMeanLongitude(double T) noexcept {
    return g_Pelp2k[7][0] + (g_Pelp2k[7][1] * T);
}

//Handle the main problem calculation (Longitude & Latitude)
double AELP2000::Accumulate(const A_ELP2000MainProblemCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF) noexcept
{
    //Validate our parameters
    assert(pCoefficients);
    assert(nCoefficients);
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pCoefficients);
#endif
    //What will be the return value from this function
    double fResult{ 0 };
    //Accumulate the result
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        const double tgv{ pCoefficients[j].m_B[0] + (g_DTASM * pCoefficients[j].m_B[4]) };
        const double x{ pCoefficients[j].m_A + (tgv * (g_DELNP - (g_AM * g_DELNU))) + (pCoefficients[j].m_B[1] * g_DELG) + (pCoefficients[j].m_B[2] * g_DELE) + (pCoefficients[j].m_B[3] * g_DELEP) };
        const double y{ (fD * pCoefficients[j].m_I[0]) + (fldash * pCoefficients[j].m_I[1]) + (fl * pCoefficients[j].m_I[2]) + (fF * pCoefficients[j].m_I[3]) };
        fResult += (x * sin(y));
    }
    return fResult;
}
double AELP2000::Accumulate_2(const A_ELP2000MainProblemCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF) noexcept
{
    //Validate our parameters
    assert(pCoefficients);
    assert(nCoefficients);
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pCoefficients);
#endif
    //What will be the return value from this function
    double fResult{ 0 };
    //Accumulate the result
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        const double tgv{ pCoefficients[j].m_B[0] + (g_DTASM * pCoefficients[j].m_B[4]) };
        double A{ pCoefficients[j].m_A };
        A -= (A * 2.0 * g_DELNU / 3.0);
        const double x{ A + (tgv * (g_DELNP - (g_AM * g_DELNU))) + (pCoefficients[j].m_B[1] * g_DELG) + (pCoefficients[j].m_B[2] * g_DELE) + (pCoefficients[j].m_B[3] * g_DELEP) };
        const double y{ (fD * pCoefficients[j].m_I[0]) + (fldash * pCoefficients[j].m_I[1]) + (fl * pCoefficients[j].m_I[2]) + (fF * pCoefficients[j].m_I[3]) };
        fResult += (x * cos(y));
    }
    return fResult;
}
//Handle the Earth figure perturbations, Tidal Effects, Moon figure & Relativistic perturbations calculation
double AELP2000::Accumulate(const double* pT, int nTSize, const A_ELP2000EarthTidalMoonRelativisticSolarEccentricityCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF, bool bI1isZero) noexcept
{
    //What will be the return value from this function
    double fResult{ 0 };
    //Accumulate the result
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        if (bI1isZero)
            assert(pCoefficients[j].m_IZ == 0);

        double y{ (fD * pCoefficients[j].m_I[0]) + (fldash * pCoefficients[j].m_I[1]) +
                 (fl * pCoefficients[j].m_I[2]) + (fF * pCoefficients[j].m_I[3]) +
                 (deg2rad * pCoefficients[j].m_O) };
        if (!bI1isZero)
            y += (pCoefficients[j].m_IZ * g_ZETA[0] * pT[0]) + (pCoefficients[j].m_IZ * g_ZETA[1] * pT[1]);
        fResult += (pCoefficients[j].m_A * sin(y));
    }
    return fResult;
}
//Handle the Earth figure perturbations & Tidal Effects /t calculation
double AELP2000::Accumulate_2(const double* pT, int nTSize, const A_ELP2000EarthTidalMoonRelativisticSolarEccentricityCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF, bool bI1isZero) noexcept
{
    //What will be the return value from this function
    double fResult{ 0 };
    //Accumulate the result
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        if (bI1isZero)
            assert(pCoefficients[j].m_IZ == 0);

        double y{ (fD * pCoefficients[j].m_I[0]) + (fldash * pCoefficients[j].m_I[1]) +
                 (fl * pCoefficients[j].m_I[2]) + (fF * pCoefficients[j].m_I[3]) +
                 (deg2rad * pCoefficients[j].m_O) };
        if (!bI1isZero)
            y += (pCoefficients[j].m_IZ * g_ZETA[0] * pT[0]) + (pCoefficients[j].m_IZ * g_ZETA[1] * pT[1]);
        fResult += (pCoefficients[j].m_A * pT[1] * sin(y));
    }
    return fResult;
}
//Handle the Planetary perturbations Table 1 calculation
double AELP2000::AccumulateTable1(const A_ELP2000PlanetPertCoefficient* pCoefficients, size_t nCoefficients, double fD, double fl, double fF, double fMe, double fV, double fT, double fMa, double fJ, double fS, double fU, double fN) noexcept
{
    //Validate our parameters
    assert(pCoefficients);
    assert(nCoefficients);
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pCoefficients);
#endif
    //What will be the return value from this function
    double fResult{ 0 };
    //Accumulate the result
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        const double y{ (fMe * pCoefficients[j].m_ip[0]) +
                       (fV * pCoefficients[j].m_ip[1]) +
                       (fT * pCoefficients[j].m_ip[2]) +
                       (fMa * pCoefficients[j].m_ip[3]) +
                       (fJ * pCoefficients[j].m_ip[4]) +
                       (fS * pCoefficients[j].m_ip[5]) +
                       (fU * pCoefficients[j].m_ip[6]) +
                       (fN * pCoefficients[j].m_ip[7]) +
                       (fD * pCoefficients[j].m_ip[8]) +
                       (fl * pCoefficients[j].m_ip[9]) +
                       (fF * pCoefficients[j].m_ip[10]) +
                       (deg2rad * pCoefficients[j].m_theta) };
        fResult += (pCoefficients[j].m_O * sin(y));
    }
    return fResult;
}
//Handle the Planetary perturbations Table 1 /t calculation
double AELP2000::AccumulateTable1_2(const double* pT, int nTSize, const A_ELP2000PlanetPertCoefficient* pCoefficients, size_t nCoefficients, double fD, double fl, double fF, double fMe, double fV, double fT, double fMa, double fJ, double fS, double fU, double fN) noexcept
{
    //What will be the return value from this function
    double fResult{ 0 };
    //Accumulate the result
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        const double y{ (fMe * pCoefficients[j].m_ip[0]) +
                       (fV * pCoefficients[j].m_ip[1]) +
                       (fT * pCoefficients[j].m_ip[2]) +
                       (fMa * pCoefficients[j].m_ip[3]) +
                       (fJ * pCoefficients[j].m_ip[4]) +
                       (fS * pCoefficients[j].m_ip[5]) +
                       (fU * pCoefficients[j].m_ip[6]) +
                       (fN * pCoefficients[j].m_ip[7]) +
                       (fD * pCoefficients[j].m_ip[8]) +
                       (fl * pCoefficients[j].m_ip[9]) +
                       (fF * pCoefficients[j].m_ip[10]) +
                       (deg2rad * pCoefficients[j].m_theta) };
        fResult += (pCoefficients[j].m_O * pT[1] * sin(y));
    }
    return fResult;
}
//Handle the Planetary perturbations Table 2 calculation
double AELP2000::AccumulateTable2(const A_ELP2000PlanetPertCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF, double fMe, double fV, double fT, double fMa, double fJ, double fS, double fU) noexcept
{
    //Validate our parameters
    assert(pCoefficients);
    assert(nCoefficients);
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pCoefficients);
#endif

    //What will be the return value from this function
    double fResult{ 0 };
    //Accumulate the result
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        const double y{ (fMe * pCoefficients[j].m_ip[0]) +
                       (fV * pCoefficients[j].m_ip[1]) +
                       (fT * pCoefficients[j].m_ip[2]) +
                       (fMa * pCoefficients[j].m_ip[3]) +
                       (fJ * pCoefficients[j].m_ip[4]) +
                       (fS * pCoefficients[j].m_ip[5]) +
                       (fU * pCoefficients[j].m_ip[6]) +
                       (fD * pCoefficients[j].m_ip[7]) +
                       (fldash * pCoefficients[j].m_ip[8]) +
                       (fl * pCoefficients[j].m_ip[9]) +
                       (fF * pCoefficients[j].m_ip[10]) +
                       (deg2rad * pCoefficients[j].m_theta) };
        fResult += (pCoefficients[j].m_O * sin(y));
    }
    return fResult;
}
//Handle the Planetary perturbations Table 2 /t calculation
double AELP2000::AccumulateTable2_2(const double* pT, int nTSize, const A_ELP2000PlanetPertCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF, double fMe, double fV, double fT, double fMa, double fJ, double fS, double fU) noexcept
{
    //What will be the return value from this function
    double fResult{ 0 };
    //Accumulate the result
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        const double y{ (fMe * pCoefficients[j].m_ip[0]) +
                       (fV * pCoefficients[j].m_ip[1]) +
                       (fT * pCoefficients[j].m_ip[2]) +
                       (fMa * pCoefficients[j].m_ip[3]) +
                       (fJ * pCoefficients[j].m_ip[4]) +
                       (fS * pCoefficients[j].m_ip[5]) +
                       (fU * pCoefficients[j].m_ip[6]) +
                       (fD * pCoefficients[j].m_ip[7]) +
                       (fldash * pCoefficients[j].m_ip[8]) +
                       (fl * pCoefficients[j].m_ip[9]) +
                       (fF * pCoefficients[j].m_ip[10]) +
                       (deg2rad * pCoefficients[j].m_theta) };
        fResult += (pCoefficients[j].m_O * pT[1] * sin(y));
    }
    return fResult;
}
//Handle the Planetary perturbations (solar eccentricity) /t squared calculation
double AELP2000::Accumulate_3(const double* pT, int nTSize, const A_ELP2000EarthTidalMoonRelativisticSolarEccentricityCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF) noexcept
{
    //What will be the return value from this function
    double fResult{ 0 };
    for (size_t j{ 0 }; j < nCoefficients; j++)
    {
        assert(pCoefficients[j].m_IZ == 0);
        const double y{ (fD * pCoefficients[j].m_I[0]) +
                       (fldash * pCoefficients[j].m_I[1]) +
                       (fl * pCoefficients[j].m_I[2]) +
                       (fF * pCoefficients[j].m_I[3]) +
                       (deg2rad * pCoefficients[j].m_O) };
        fResult += (pCoefficients[j].m_A * pT[2] * sin(y));
    }
    return fResult;
}
#ifdef _MSC_VER
#pragma warning(suppress : 26429)
#endif //#ifdef _MSC_VER
double AELP2000::EclipticLongitude(const double* pT, int nTSize) noexcept
{
    //Validate our parameters
    assert(pT);
    assert(nTSize >= 2);
    //Compute the delaney arguments for the specified time
    const double fD{ MoonMeanSolarElongation(pT, nTSize) };
    const double fldash{ SunMeanAnomaly(pT, nTSize) };
    const double fl{ MoonMeanAnomaly(pT, nTSize) };
    const double fF{ MoonMeanArgumentOfLatitude(pT, nTSize) };
    const double fD2{ MoonMeanSolarElongation(pT, 2) };
    const double fldash2{ SunMeanAnomaly(pT, 2) };
    const double fl2{ MoonMeanAnomaly(pT, 2) };
    const double fF2{ MoonMeanArgumentOfLatitude(pT, 2) };
    //Compute the planet mean longitudes for the specified time
    const double fMe{ MercuryMeanLongitude(pT[1]) };
    const double fV{ VenusMeanLongitude(pT[1]) };
    const double fT{ MeanHeliocentricMeanLongitudeEarthMoonBarycentre(pT, 2) };
    const double fMa{ MarsMeanLongitude(pT[1]) };
    const double fJ{ JupiterMeanLongitude(pT[1]) };
    const double fS{ SaturnMeanLongitude(pT[1]) };
    const double fU{ UranusMeanLongitude(pT[1]) };
    const double fN{ NeptuneMeanLongitude(pT[1]) };
    //Calculate the Longitude
    const double A{ Accumulate(g_ELP1.data(), g_ELP1.size(), fD, fldash, fl, fF) +
                   Accumulate(pT, nTSize, g_ELP4.data(), g_ELP4.size(), fD2, fldash2, fl2, fF2, false) +
                   Accumulate_2(pT, nTSize, g_ELP7.data(), g_ELP7.size(), fD2, fldash2, fl2, fF2, false) +
                   AccumulateTable1(g_ELP10.data(), g_ELP10.size(), fD2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU, fN) +
                   AccumulateTable1_2(pT, nTSize, g_ELP13.data(), g_ELP13.size(), fD2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU, fN) +
                   AccumulateTable2(g_ELP16.data(), g_ELP16.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU) +
                   AccumulateTable2_2(pT, nTSize, g_ELP19.data(), g_ELP19.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU) +
                   Accumulate(pT, nTSize, g_ELP22.data(), g_ELP22.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate_2(pT, nTSize, g_ELP25.data(), g_ELP25.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate(pT, nTSize, g_ELP28.data(), g_ELP28.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate(pT, nTSize, g_ELP31.data(), g_ELP31.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate_3(pT, nTSize, g_ELP34.data(), g_ELP34.size(), fD2, fldash2, fl2, fF2) };
    return ACoord::rangezero2threesixty((A / 3600.0) + ( rad2deg * MoonMeanMeanLongitude(pT, nTSize)));
}
#ifdef _MSC_VER
#pragma warning(suppress : 26429)
#endif //#ifdef _MSC_VER
double AELP2000::EclipticLatitude(const double* pT, int nTSize) noexcept
{
    //Validate our parameters
    assert(pT);
    assert(nTSize >= 2);
    //Compute the delaney arguments for the specified time
    const double fD{ MoonMeanSolarElongation(pT, nTSize) };
    const double fldash{ SunMeanAnomaly(pT, nTSize) };
    const double fl{ MoonMeanAnomaly(pT, nTSize) };
    const double fF{ MoonMeanArgumentOfLatitude(pT, nTSize) };
    const double fD2{ MoonMeanSolarElongation(pT, 2) };
    const double fldash2{ SunMeanAnomaly(pT, 2) };
    const double fl2{ MoonMeanAnomaly(pT, 2) };
    const double fF2{ MoonMeanArgumentOfLatitude(pT, 2) };
    //Compute the planet mean longitudes for the specified time
    const double fMe{ MercuryMeanLongitude(pT[1]) };
    const double fV{ VenusMeanLongitude(pT[1]) };
    const double fT{ MeanHeliocentricMeanLongitudeEarthMoonBarycentre(pT, 2) };
    const double fMa{ MarsMeanLongitude(pT[1]) };
    const double fJ{ JupiterMeanLongitude(pT[1]) };
    const double fS{ SaturnMeanLongitude(pT[1]) };
    const double fU{ UranusMeanLongitude(pT[1]) };
    const double fN{ NeptuneMeanLongitude(pT[1]) };
    //Calculate the Longitude
    const double B{ Accumulate(g_ELP2.data(), g_ELP2.size(), fD, fldash, fl, fF) +
                   Accumulate(pT, nTSize, g_ELP5.data(), g_ELP5.size(), fD2, fldash2, fl2, fF2, false) +
                   Accumulate_2(pT, nTSize, g_ELP8.data(), g_ELP8.size(), fD2, fldash2, fl2, fF2, false) +
                   AccumulateTable1(g_ELP11.data(), g_ELP11.size(), fD2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU, fN) +
                   AccumulateTable1_2(pT, nTSize, g_ELP14.data(), g_ELP14.size(), fD2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU, fN) +
                   AccumulateTable2(g_ELP17.data(), g_ELP17.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU) +
                   AccumulateTable2_2(pT, nTSize, g_ELP20.data(), g_ELP20.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU) +
                   Accumulate(pT, nTSize, g_ELP23.data(), g_ELP23.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate_2(pT, nTSize, g_ELP26.data(), g_ELP26.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate(pT, nTSize, g_ELP29.data(), g_ELP29.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate(pT, nTSize, g_ELP32.data(), g_ELP32.size(), fD2, fldash2, fl2, fF2, true) +
                   Accumulate_3(pT, nTSize, g_ELP35.data(), g_ELP35.size(), fD2, fldash2, fl2, fF2) };
    return ACoord::rangemninety2ninety(B / 3600.0);
}
#ifdef _MSC_VER
#pragma warning(suppress : 26429)
#endif //#ifdef _MSC_VER
double AELP2000::RadiusVector(const double* pT, int nTSize) noexcept
{
    //Validate our parameters
    assert(pT);
    assert(nTSize >= 2);
    //Compute the delaney arguments for the specified time
    const double fD{ MoonMeanSolarElongation(pT, nTSize) };
    const double fldash{ SunMeanAnomaly(pT, nTSize) };
    const double fl{ MoonMeanAnomaly(pT, nTSize) };
    const double fF{ MoonMeanArgumentOfLatitude(pT, nTSize) };
    const double fD2{ MoonMeanSolarElongation(pT, 2) };
    const double fldash2{ SunMeanAnomaly(pT, 2) };
    const double fl2{ MoonMeanAnomaly(pT, 2) };
    const double fF2{ MoonMeanArgumentOfLatitude(pT, 2) };
    //Compute the planet mean longitudes for the specified time
    const double fMe{ MercuryMeanLongitude(pT[1]) };
    const double fV{ VenusMeanLongitude(pT[1]) };
    const double fT{ MeanHeliocentricMeanLongitudeEarthMoonBarycentre(pT, 2) };
    const double fMa{ MarsMeanLongitude(pT[1]) };
    const double fJ{ JupiterMeanLongitude(pT[1]) };
    const double fS{ SaturnMeanLongitude(pT[1]) };
    const double fU{ UranusMeanLongitude(pT[1]) };
    const double fN{ NeptuneMeanLongitude(pT[1]) };
    //Calculate the Longitude
    const double fValue{ Accumulate_2(g_ELP3.data(), g_ELP3.size(), fD, fldash, fl, fF) +
                        Accumulate(pT, nTSize, g_ELP6.data(), g_ELP6.size(), fD2, fldash2, fl2, fF2, false) +
                        Accumulate_2(pT, nTSize, g_ELP9.data(), g_ELP9.size(), fD2, fldash2, fl2, fF2, false) +
                        AccumulateTable1(g_ELP12.data(), g_ELP12.size(), fD2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU, fN) +
                        AccumulateTable1_2(pT, nTSize, g_ELP15.data(), g_ELP15.size(), fD2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU, fN) +
                        AccumulateTable2(g_ELP18.data(), g_ELP18.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU) +
                        AccumulateTable2_2(pT, nTSize, g_ELP21.data(), g_ELP21.size(), fD2, fldash2, fl2, fF2, fMe, fV, fT, fMa, fJ, fS, fU) +
                        Accumulate(pT, nTSize, g_ELP24.data(), g_ELP24.size(), fD2, fldash2, fl2, fF2, true) +
                        Accumulate_2(pT, nTSize, g_ELP27.data(), g_ELP27.size(), fD2, fldash2, fl2, fF2, true) +
                        Accumulate(pT, nTSize, g_ELP30.data(), g_ELP30.size(), fD2, fldash2, fl2, fF2, true) +
                        Accumulate(pT, nTSize, g_ELP33.data(), g_ELP33.size(), fD2, fldash2, fl2, fF2, true) +
                        Accumulate_3(pT, nTSize, g_ELP36.data(), g_ELP36.size(), fD2, fldash2, fl2, fF2) };
    return fValue * 384747.9806448954 / 384747.9806743165;
}


// ELP MPP02 - From AA+ v2.49 (P.J.Naughter)

LLD AELPMPP02::EclipticCoordinates(double jd_tt, ELPMPP02_Correction correction, double* pDerivative) noexcept {
    LLD moonpos{};
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (jd_tt - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];

    moonpos.lon = deg2rad * EclipticLongitude(t.data(), 5, correction, pDerivative);
    moonpos.lat = deg2rad * EclipticLatitude(t.data(), 5, correction, pDerivative);
    moonpos.dst = RadiusVector(t.data(), 5, correction, pDerivative);  // km
    return moonpos;
}
double AELPMPP02::EclipticLongitude(double JD, ELPMPP02_Correction correction, double* pDerivative) noexcept {
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return deg2rad * EclipticLongitude(t.data(), 5, correction, pDerivative);  // radians
}
double AELPMPP02::EclipticLatitude(double JD, ELPMPP02_Correction correction, double* pDerivative) noexcept {
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return deg2rad * (EclipticLatitude(t.data(), 5, correction, pDerivative));  // radians
}
double AELPMPP02::RadiusVector(double JD, ELPMPP02_Correction correction, double* pDerivative) noexcept {
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - JD_2000) / JD_CENTURY;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return RadiusVector(t.data(), 5, correction, pDerivative);  // kilometers
}

glm::dvec3 AELPMPP02::EclipticRectangularCoordinates(double JD, ELPMPP02_Correction correction, glm::dvec3* pDerivative) noexcept {
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return EclipticRectangularCoordinates(t.data(), 5, correction, pDerivative);  // kilometers
}

glm::dvec3 AELPMPP02::EclipticRectangularCoordinatesJ2000(double JD, ELPMPP02_Correction correction, glm::dvec3* pDerivative) noexcept {
    std::array<double, 5> t{ 0.0 };
    t[0] = 1;
    t[1] = (JD - 2451545) / 36525;
    t[2] = t[1] * t[1];
    t[3] = t[2] * t[1];
    t[4] = t[3] * t[1];
    return EclipticRectangularCoordinatesJ2000(t.data(), 5, correction, pDerivative);  // kilometers
}

// private methods
double AELPMPP02::EclipticLongitude(const double* pT, int nTSize, ELPMPP02_Correction correction, double* pDerivative) noexcept {
    //Validate our parameters
    assert(pT);
    assert(nTSize == 5);

    //Work out the right data arrays to use
    const std::array<std::array<double, 5>, 3>* g_pW{ nullptr };
    const std::array<double, 5>* g_pEARTH{ nullptr };
    const std::array<double, 5>* g_pPERI{ nullptr };
    const std::array<double, 5>* g_pZETA{ nullptr };
    const std::array<double, 1023>* g_pMAIN_S1_Coeff{ nullptr };
    switch (correction)
    {
    case ELPMPP02_Nominal: {
        g_pW = &g_W_Nominal;
        g_pEARTH = &g_EARTH_Nominal;
        g_pPERI = &g_PERI_Nominal;
        g_pZETA = &g_ZETA_Nominal;
        g_pMAIN_S1_Coeff = &g_MAIN_S1_Nominal;
        break;
    }
    case ELPMPP02_LLR: {
        g_pW = &g_W_LLR;
        g_pEARTH = &g_EARTH_LLR;
        g_pPERI = &g_PERI_LLR;
        g_pZETA = &g_ZETA_LLR;
        g_pMAIN_S1_Coeff = &g_MAIN_S1_LLR;
        break;
    }
    case ELPMPP02_DE405: {
        g_pW = &g_W_DE405;
        g_pEARTH = &g_EARTH_DE405;
        g_pPERI = &g_PERI_DE405;
        g_pZETA = &g_ZETA_DE405;
        g_pMAIN_S1_Coeff = &g_MAIN_S1_DE405;
        break;
    }
    case ELPMPP02_DE406: {
        g_pW = &g_W_DE406;
        g_pEARTH = &g_EARTH_DE406;
        g_pPERI = &g_PERI_DE406;
        g_pZETA = &g_ZETA_DE406;
        g_pMAIN_S1_Coeff = &g_MAIN_S1_DE406;
        break;
    }
    default: {
        assert(false);
        break;
    }
    }
    assert(g_pW);
    assert(g_pPERI);
    assert(g_pEARTH);
    assert(g_pZETA);
    const std::array<std::array<double, 5>, 3>& g_W{ *g_pW };
    const std::array<double, 5>& g_EARTH{ *g_pEARTH };
    const std::array<double, 5>& g_PERI{ *g_pPERI };
    const std::array<double, 5>& g_ZETA{ *g_pZETA };
    const std::array<double, 1023>& g_MAIN_S1_Coeff{ *g_pMAIN_S1_Coeff };

    //Setup the Delaunay array
    std::array<std::array<double, 5>, 4> fDelaunay{};
    for (int i{ 0 }; i < 5; i++)
    {
        fDelaunay[0][i] = g_W[0][i] - g_EARTH[i]; //D
        fDelaunay[1][i] = g_W[0][i] - g_W[2][i]; //F
        fDelaunay[2][i] = g_W[0][i] - g_W[1][i]; //l
        fDelaunay[3][i] = g_EARTH[i] - g_PERI[i]; //l'
    }
    fDelaunay[0][0] += pi;

    //What will be the return value from this method
    double fResult{ 0 };

    //Set the output parameter to a default value if required
    if (pDerivative) *pDerivative = 0;

    //First the main problem
    constexpr size_t nEndI{ g_MAIN_S1.size() };
    for (size_t i{ 0 }; i < nEndI; i++) {
        std::array<double, 5> fFi{};
        for (int a{ 0 }; a < 5; a++) {
            for (int j{ 0 }; j < 4; j++) fFi[a] += (g_MAIN_S1[i].m_I[j] * fDelaunay[j][a]);
        }
        double y{ fFi[0] };
        double yp{ 0 };
        for (int k{ 1 }; k <= 4; k++) {
            y += (fFi[k] * pT[k]);
            yp += (k * fFi[k] * pT[k - 1]);
        }
#ifdef _MSC_VER
#pragma warning(suppress : 26488)
#endif //#ifdef _MSC_VER
        fResult += (g_MAIN_S1_Coeff[i] * sin(y));
        if (pDerivative)
#ifdef _MSC_VER
#pragma warning(suppress : 26488)
#endif //#ifdef _MSC_VER
            * pDerivative += (g_MAIN_S1_Coeff[i] * cos(y) * yp);
    }

    //Then the perturbations
    constexpr size_t nEndk{ g_PERT_S1.size() };  // Why does this give an error when set to constexpr? It compiles fine.
    for (size_t k = 0; k < nEndk; k++) {
        const size_t nEndP{ g_PERT_S1[k].m_nTableSize };
        for (size_t p{ 0 }; p < nEndP; p++) {
            std::array<double, 5> fFi{};
            fFi[0] = atan2(g_PERT_S1[k].m_pTable[p].m_C, g_PERT_S1[k].m_pTable[p].m_S);
            if (fFi[0] < 0) fFi[0] += tau;
            for (int a{ 0 }; a < 5; a++) {
                for (int i{ 0 }; i < 4; i++)
                    fFi[a] += (g_PERT_S1[k].m_pTable[p].m_I[i] * fDelaunay[i][a]);
                if (a < 2) {
                    for (size_t i{ 4 }; i < 12; i++) fFi[a] += (g_Pmpp02[i - 4][a] * g_PERT_S1[k].m_pTable[p].m_I[i]);
                }
                fFi[a] += (g_PERT_S1[k].m_pTable[p].m_I[12] * g_ZETA[a]);
            }

            double y{ fFi[0] };
            double yp{ 0 };
            for (int i{ 1 }; i <= 4; i++) {
                y += (fFi[i] * pT[i]);
                yp += (i * fFi[i] * pT[i - 1]);
            }
            double x{ sqrt((g_PERT_S1[k].m_pTable[p].m_S * g_PERT_S1[k].m_pTable[p].m_S) + (g_PERT_S1[k].m_pTable[p].m_C * g_PERT_S1[k].m_pTable[p].m_C)) };
            fResult += (x * pT[k] * sin(y));
            if (pDerivative) {
                if (k == 0) *pDerivative += (x * pT[k] * yp * cos(y));
                else *pDerivative += (((k * x * pT[k - 1]) * sin(y)) + (x * pT[k] * yp * cos(y)));
            }
        }
    }

    fResult = ACoord::rangezero2threesixty((fResult / 3600.0) + rad2deg * (g_W[0][0] + (g_W[0][1] * pT[1]) + (g_W[0][2] * pT[2]) + (g_W[0][3] * pT[3]) + (g_W[0][4] * pT[4])));
    if (pDerivative) {
        *pDerivative = (*pDerivative / 3600.0) + rad2deg * (g_W[0][1] + (2 * g_W[0][2] * pT[1]) + (3 * g_W[0][3] * pT[2]) + (4 * g_W[0][4] * pT[3]));
        *pDerivative /= JD_CENTURY;
    }
    return fResult;
}

double AELPMPP02::EclipticLatitude(const double* pT, int nTSize, ELPMPP02_Correction correction, double* pDerivative) noexcept
{
    //Validate our parameters
    assert(pT);
    assert(nTSize == 5);
    //Work out the right data arrays to use
    const std::array<std::array<double, 5>, 3>* g_pW{ nullptr };
    const std::array<double, 5>* g_pEARTH{ nullptr };
    const std::array<double, 5>* g_pPERI{ nullptr };
    const std::array<double, 5>* g_pZETA{ nullptr };
    const std::array<double, 918>* g_pMAIN_S2_Coeff{ nullptr };
    switch (correction) {
    case ELPMPP02_Nominal: {
        g_pW = &g_W_Nominal;
        g_pEARTH = &g_EARTH_Nominal;
        g_pPERI = &g_PERI_Nominal;
        g_pZETA = &g_ZETA_Nominal;
        g_pMAIN_S2_Coeff = &g_MAIN_S2_Nominal;
        break;
    }
    case ELPMPP02_LLR: {
        g_pW = &g_W_LLR;
        g_pEARTH = &g_EARTH_LLR;
        g_pPERI = &g_PERI_LLR;
        g_pZETA = &g_ZETA_LLR;
        g_pMAIN_S2_Coeff = &g_MAIN_S2_LLR;
        break;
    }
    case ELPMPP02_DE405: {
        g_pW = &g_W_DE405;
        g_pEARTH = &g_EARTH_DE405;
        g_pPERI = &g_PERI_DE405;
        g_pZETA = &g_ZETA_DE405;
        g_pMAIN_S2_Coeff = &g_MAIN_S2_DE405;
        break;
    }
    case ELPMPP02_DE406: {
        g_pW = &g_W_DE406;
        g_pEARTH = &g_EARTH_DE406;
        g_pPERI = &g_PERI_DE406;
        g_pZETA = &g_ZETA_DE406;
        g_pMAIN_S2_Coeff = &g_MAIN_S2_DE406;
        break;
    }
    default: {
        assert(false);
        break;
    }
    }
    assert(g_pW);
    assert(g_pPERI);
    assert(g_pEARTH);
    assert(g_pZETA);

    const std::array<std::array<double, 5>, 3>& g_W{ *g_pW };
    const std::array<double, 5>& g_EARTH{ *g_pEARTH };
    const std::array<double, 5>& g_PERI{ *g_pPERI };
    const std::array<double, 5>& g_ZETA{ *g_pZETA };
    const std::array<double, 918>& g_MAIN_S2_Coeff{ *g_pMAIN_S2_Coeff };

    //Setup the Delaunay array
    std::array<std::array<double, 5>, 4> fDelaunay{};
    for (int i{ 0 }; i < 5; i++)
    {
        fDelaunay[0][i] = g_W[0][i] - g_EARTH[i]; //D
        fDelaunay[1][i] = g_W[0][i] - g_W[2][i]; //F
        fDelaunay[2][i] = g_W[0][i] - g_W[1][i]; //l
        fDelaunay[3][i] = g_EARTH[i] - g_PERI[i]; //l'
    }
    fDelaunay[0][0] += pi;

    //What will be the return value from this method
    double fResult{ 0 };

    //Set the output parameter to a default value if required
    if (pDerivative) *pDerivative = 0;

    //First the main problem
    constexpr size_t nEndI{ g_MAIN_S2.size() };
    for (size_t i{ 0 }; i < nEndI; i++) {
        std::array<double, 5> fFi{};
        for (int a{ 0 }; a < 5; a++) {
            for (int j{ 0 }; j < 4; j++) fFi[a] += (g_MAIN_S2[i].m_I[j] * fDelaunay[j][a]);
        }
        double y{ fFi[0] };
        double yp{ 0 };
        for (int k{ 1 }; k <= 4; k++) {
            y += (fFi[k] * pT[k]);
            yp += (k * fFi[k] * pT[k - 1]);
        }
#ifdef _MSC_VER
#pragma warning(suppress : 26488)
#endif //#ifdef _MSC_VER
        fResult += (g_MAIN_S2_Coeff[i] * sin(y));
        if (pDerivative)
#ifdef _MSC_VER
#pragma warning(suppress : 26488)
#endif //#ifdef _MSC_VER
            * pDerivative += (g_MAIN_S2_Coeff[i] * cos(y) * yp);
    }

    //Then the perturbations
    constexpr size_t nEndk{ g_PERT_S2.size() };
    for (size_t k{ 0 }; k < nEndk; k++) {
        const size_t nEndP{ g_PERT_S2[k].m_nTableSize };
        for (size_t p{ 0 }; p < nEndP; p++) {
            std::array<double, 5> fFi{};
            fFi[0] = atan2(g_PERT_S2[k].m_pTable[p].m_C, g_PERT_S2[k].m_pTable[p].m_S);
            if (fFi[0] < 0)
                fFi[0] += tau;
            for (int a{ 0 }; a < 5; a++) {
                for (int i{ 0 }; i < 4; i++) fFi[a] += (g_PERT_S2[k].m_pTable[p].m_I[i] * fDelaunay[i][a]);
                if (a < 2) {
                    for (size_t i{ 4 }; i < 12; i++) fFi[a] += (g_Pmpp02[i - 4][a] * g_PERT_S2[k].m_pTable[p].m_I[i]);
                }
                fFi[a] += (g_PERT_S2[k].m_pTable[p].m_I[12] * g_ZETA[a]);
            }

            double y{ fFi[0] };
            double yp{ 0 };
            for (int i{ 1 }; i <= 4; i++) {
                y += (fFi[i] * pT[i]);
                yp += (i * fFi[i] * pT[i - 1]);
            }
            double x{ sqrt((g_PERT_S2[k].m_pTable[p].m_S * g_PERT_S2[k].m_pTable[p].m_S) + (g_PERT_S2[k].m_pTable[p].m_C * g_PERT_S2[k].m_pTable[p].m_C)) };
            fResult += (x * pT[k] * sin(y));
            if (pDerivative) {
                if (k == 0)  *pDerivative += (x * pT[k] * yp * cos(y));
                else *pDerivative += (((k * x * pT[k - 1]) * sin(y)) + (x * pT[k] * yp * cos(y)));
            }
        }
    }
    fResult = ACoord::rangemninety2ninety(fResult / 3600.0);
    if (pDerivative) *pDerivative = *pDerivative / (3600.0 * 36525.0);
    return fResult;
}

double AELPMPP02::RadiusVector(const double* pT, int nTSize, ELPMPP02_Correction correction, double* pDerivative) noexcept
{
    //Validate our parameters
    assert(pT);
    assert(nTSize == 5);

    //Work out the right data arrays to use
    const std::array<std::array<double, 5>, 3>* g_pW{ nullptr };
    const std::array<double, 5>* g_pEARTH{ nullptr };
    const std::array<double, 5>* g_pPERI{ nullptr };
    const std::array<double, 5>* g_pZETA{ nullptr };
    const std::array<double, 704>* g_pMAIN_S3_Coeff{ nullptr };
    switch (correction)
    {
    case ELPMPP02_Nominal: {
        g_pW = &g_W_Nominal;
        g_pEARTH = &g_EARTH_Nominal;
        g_pPERI = &g_PERI_Nominal;
        g_pZETA = &g_ZETA_Nominal;
        g_pMAIN_S3_Coeff = &g_MAIN_S3_Nominal;
        break;
    }
    case ELPMPP02_LLR:
    {
        g_pW = &g_W_LLR;
        g_pEARTH = &g_EARTH_LLR;
        g_pPERI = &g_PERI_LLR;
        g_pZETA = &g_ZETA_LLR;
        g_pMAIN_S3_Coeff = &g_MAIN_S3_LLR;
        break;
    }
    case ELPMPP02_DE405: {
        g_pW = &g_W_DE405;
        g_pEARTH = &g_EARTH_DE405;
        g_pPERI = &g_PERI_DE405;
        g_pZETA = &g_ZETA_DE405;
        g_pMAIN_S3_Coeff = &g_MAIN_S3_DE405;
        break;
    }
    case ELPMPP02_DE406: {
        g_pW = &g_W_DE406;
        g_pEARTH = &g_EARTH_DE406;
        g_pPERI = &g_PERI_DE406;
        g_pZETA = &g_ZETA_DE406;
        g_pMAIN_S3_Coeff = &g_MAIN_S3_DE406;
        break;
    }
    default: {
        assert(false);
        break;
    }
    }
    assert(g_pW);
    assert(g_pPERI);
    assert(g_pEARTH);
    assert(g_pZETA);
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(g_pW);
#pragma warning(suppress: 26477)
    __analysis_assume(g_pEARTH);
#pragma warning(suppress: 26477)
    __analysis_assume(g_pPERI);
#pragma warning(suppress: 26477)
    __analysis_assume(g_pZETA);
#pragma warning(suppress: 26477)
    __analysis_assume(pT);
#pragma warning(suppress: 26477)
    __analysis_assume(g_pMAIN_S3_Coeff);
#endif
    const std::array<std::array<double, 5>, 3>& g_W{ *g_pW };
    const std::array<double, 5>& g_EARTH{ *g_pEARTH };
    const std::array<double, 5>& g_PERI{ *g_pPERI };
    const std::array<double, 5>& g_ZETA{ *g_pZETA };
    const std::array<double, 704>& g_MAIN_S3_Coeff{ *g_pMAIN_S3_Coeff };

    //Setup the Delaunay array
    std::array<std::array<double, 5>, 4> fDelaunay{};
    for (int i{ 0 }; i < 5; i++)
    {
        fDelaunay[0][i] = g_W[0][i] - g_EARTH[i]; //D
        fDelaunay[1][i] = g_W[0][i] - g_W[2][i];  //F
        fDelaunay[2][i] = g_W[0][i] - g_W[1][i];  //l
        fDelaunay[3][i] = g_EARTH[i] - g_PERI[i]; //l'
    }
    fDelaunay[0][0] += pi;

    //What will be the return value from this method
    double fResult{ 0 };

    //Set the output parameter to a default value if required
    if (pDerivative)
        *pDerivative = 0;

    //First the main problem
    constexpr size_t nEndI{ g_MAIN_S3.size() };
    for (size_t i{ 0 }; i < nEndI; i++)
    {
        std::array<double, 5> fFi{};
        for (int a{ 0 }; a < 5; a++)
        {
            for (int j{ 0 }; j < 4; j++)
                fFi[a] += (g_MAIN_S3[i].m_I[j] * fDelaunay[j][a]);
        }
        double y{ fFi[0] };
        double yp{ 0 };
        for (int k{ 1 }; k <= 4; k++)
        {
            y += (fFi[k] * pT[k]);
            yp += (k * fFi[k] * pT[k - 1]);
        }
#ifdef _MSC_VER
#pragma warning(suppress : 26488)
#endif //#ifdef _MSC_VER
        fResult += (g_MAIN_S3_Coeff[i] * cos(y));
        if (pDerivative)
#ifdef _MSC_VER
#pragma warning(suppress : 26488)
#endif //#ifdef _MSC_VER
            * pDerivative -= (g_MAIN_S3_Coeff[i] * sin(y) * yp);
    }

    //Then the perturbations
    constexpr size_t nEndk{ g_PERT_S3.size() };
    for (size_t k{ 0 }; k < nEndk; k++) {
        const size_t nEndP{ g_PERT_S3[k].m_nTableSize };
        for (size_t p{ 0 }; p < nEndP; p++) {
            std::array<double, 5> fFi{};
            fFi[0] = atan2(g_PERT_S3[k].m_pTable[p].m_C, g_PERT_S3[k].m_pTable[p].m_S);
            if (fFi[0] < 0) fFi[0] += tau;
            for (int a{ 0 }; a < 5; a++) {
                for (int i{ 0 }; i < 4; i++)
                    fFi[a] += (g_PERT_S3[k].m_pTable[p].m_I[i] * fDelaunay[i][a]);
                if (a < 2) {
                    for (size_t i{ 4 }; i < 12; i++)
                        fFi[a] += (g_Pmpp02[i - 4][a] * g_PERT_S3[k].m_pTable[p].m_I[i]);
                }
                fFi[a] += (g_PERT_S3[k].m_pTable[p].m_I[12] * g_ZETA[a]);
            }

            double y{ fFi[0] };
            double yp{ 0 };
            for (int i{ 1 }; i <= 4; i++)
            {
                y += (fFi[i] * pT[i]);
                yp += (i * fFi[i] * pT[i - 1]);
            }
            double x{ sqrt((g_PERT_S3[k].m_pTable[p].m_S * g_PERT_S3[k].m_pTable[p].m_S) + (g_PERT_S3[k].m_pTable[p].m_C * g_PERT_S3[k].m_pTable[p].m_C)) };
            fResult += (x * pT[k] * sin(y));
            if (pDerivative)
            {
                if (k == 0)
                    *pDerivative += (x * pT[k] * yp * cos(y));
                else
                    *pDerivative += (((k * x * pT[k - 1]) * sin(y)) + (x * pT[k] * yp * cos(y)));
            }
        }
    }

    fResult *= 0.99999994982652029474691585875733;
    if (pDerivative)
        *pDerivative /= 36525.0;

    return fResult;
}


glm::dvec3 AELPMPP02::EclipticRectangularCoordinates(const double* pT, int nTSize, ELPMPP02_Correction correction, glm::dvec3* pDerivative) noexcept {
    double fLongitudeDerivative{ 0 };
    double fLongitude{ EclipticLongitude(pT, nTSize, correction, &fLongitudeDerivative) };
    fLongitudeDerivative *= 36525.0;
    double fLatitudeDerivative{ 0 };
    double fLatitude{ EclipticLatitude(pT, nTSize, correction, &fLatitudeDerivative) };
    fLatitudeDerivative *= 36525.0;
    double fRadiusDerivative{ 0 };
    const double fRadius{ RadiusVector(pT, nTSize, correction, &fRadiusDerivative) };
    fRadiusDerivative *= 36525.0;
    fLongitude *= deg2rad;
    fLongitudeDerivative *= deg2rad;
    fLatitude *= deg2rad;
    fLatitudeDerivative *= deg2rad;
    const double fCosLong{ cos(fLongitude) };
    const double fSinLong{ sin(fLongitude) };
    const double fCosLat{ cos(fLatitude) };
    const double fSinLat{ sin(fLatitude) };
    const double fRCosLat{ fRadius * fCosLat };
    const double fRSinLat{ fRadius * fSinLat };
    glm::dvec3 value{ 0.0 };
    value.x = fRCosLat * fCosLong;
    value.y = fRCosLat * fSinLong;
    value.z = fRSinLat;

    if (pDerivative)
    {
        pDerivative->x = ((((fRadiusDerivative * fCosLat) - (fLatitudeDerivative * fRSinLat)) * fCosLong) - (fLongitudeDerivative * value.y)) / 36525.0;
        pDerivative->y = ((((fRadiusDerivative * fCosLat) - (fLatitudeDerivative * fRSinLat)) * fSinLong) + (fLongitudeDerivative * value.x)) / 36525.0;
        pDerivative->z = ((fRadiusDerivative * fSinLat) + (fLatitudeDerivative * fRCosLat)) / 36525.0;
    }

    return value;
}


glm::dvec3 AELPMPP02::EclipticRectangularCoordinatesJ2000(const double* pT, int nTSize, ELPMPP02_Correction correction, glm::dvec3* pDerivative) noexcept
{
    //Validate our parameters
    assert(pT);
    assert(nTSize == 5);
#ifdef __analysis_assume
#pragma warning(suppress: 26477)
    __analysis_assume(pT);
#endif

    static constexpr std::array<double, 5> LaskarsP
    { {
      1.0180391e-5,
      4.7020439e-7,
     -5.417367e-10,
     -2.507948e-12,
      4.63486e-15
    } };

    static constexpr std::array<double, 5> LaskarsQ
    { {
     -1.13469002e-4,
      1.2372674e-7,
      1.265417e-9,
     -1.371808e-12,
     -3.20334e-15
    } };

    glm::dvec3 EclipticDerivative;
    const glm::dvec3 Ecliptic{ EclipticRectangularCoordinates(pT, nTSize, correction, &EclipticDerivative) };
    EclipticDerivative.x *= 36525.0;
    EclipticDerivative.y *= 36525.0;
    EclipticDerivative.z *= 36525.0;
    const double fP{ (LaskarsP[0] + (LaskarsP[1] * pT[1]) + (LaskarsP[2] * pT[2]) + (LaskarsP[3] * pT[3]) + (LaskarsP[4] * pT[4])) * pT[1] };
    const double fQ{ (LaskarsQ[0] + (LaskarsQ[1] * pT[1]) + (LaskarsQ[2] * pT[2]) + (LaskarsQ[3] * pT[3]) + (LaskarsQ[4] * pT[4])) * pT[1] };
    const double fTwoP{ 2 * fP };
    const double fP2{ fP * fP };
    const double fQ2{ fQ * fQ };
    const double fOneMinus2P2{ 1 - (2 * fP2) };
    const double fOneMinus2Q2{ 1 - (2 * fQ2) };
    const double fTwoPQ{ fTwoP * fQ };
    const double fTwosqrt1MinusPart{ 2 * sqrt(1 - fP2 - fQ2) };
    const double fPTwosqrt1MinusPart{ fP * fTwosqrt1MinusPart };
    const double fQTwosqrt1MinusPart{ fQ * fTwosqrt1MinusPart };
    glm::dvec3 J2000{ 0.0 };
    J2000.x = (fOneMinus2P2 * Ecliptic.x) + (fTwoPQ * Ecliptic.y) + (fPTwosqrt1MinusPart * Ecliptic.z);
    J2000.y = (fTwoPQ * Ecliptic.x) + (fOneMinus2Q2 * Ecliptic.y) - (fQTwosqrt1MinusPart * Ecliptic.z);
    J2000.z = (-fPTwosqrt1MinusPart * Ecliptic.x) + (fQTwosqrt1MinusPart * Ecliptic.y) + ((fOneMinus2P2 + fOneMinus2Q2 - 1) * Ecliptic.z);
    if (pDerivative)
    {
        const double fPp{ LaskarsP[0] + ((2 * LaskarsP[1]) + (3 * LaskarsP[2] * pT[1]) + (4 * LaskarsP[3] * pT[2]) + (5 * LaskarsP[3] * pT[4])) * pT[1] };
        const double fQp{ LaskarsQ[0] + ((2 * LaskarsQ[1]) + (3 * LaskarsQ[2] * pT[1]) + (4 * LaskarsQ[3] * pT[2]) + (5 * LaskarsQ[3] * pT[4])) * pT[1] };
        const double fMinus4PPp{ -4 * fP * fPp };
        const double fMinus4QQp{ -4 * fQ * fQp };
        const double fTwoPpQpQp{ 2 * ((fPp * fQ) + (fP * fQp)) };
        const double fK1{ (fMinus4PPp + fMinus4QQp) / fTwosqrt1MinusPart };
        const double fK2{ (fPp * fTwosqrt1MinusPart) + (fP * fK1) };
        const double fK3{ (fQp * fTwosqrt1MinusPart) + (fQ * fK1) };
        pDerivative->x = ((fOneMinus2P2 * EclipticDerivative.x) + (fTwoPQ * EclipticDerivative.y) + (fPTwosqrt1MinusPart * EclipticDerivative.z) + (fMinus4PPp * Ecliptic.x) + (fTwoPpQpQp * Ecliptic.y) + (fK2 * Ecliptic.z)) / 36525.0;
        pDerivative->y = ((fTwoPQ * EclipticDerivative.x) + (fOneMinus2Q2 * EclipticDerivative.y) - (fQTwosqrt1MinusPart * EclipticDerivative.z) + (fTwoPpQpQp * Ecliptic.x) + (fMinus4QQp * Ecliptic.y) - (fK3 * Ecliptic.z)) / 36525.0;
        pDerivative->z = ((-fPTwosqrt1MinusPart * EclipticDerivative.x) + (fQTwosqrt1MinusPart * EclipticDerivative.y) + ((fOneMinus2P2 + fOneMinus2Q2 - 1) * EclipticDerivative.z) - (fK2 * Ecliptic.x) + (fK3 * Ecliptic.y) + ((fMinus4PPp + fMinus4QQp) * Ecliptic.z)) / 36525.0;
    }
    return J2000;
}



