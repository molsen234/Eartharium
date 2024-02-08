
#include <cmath>
//#include <array>

#include "acoordinates.h"
#include "anutation.h"


double ANutation::NutationInLongitude(double jd_tt, bool rad) {
    // AA+: CAANutation::NutationInLongitude(JD)
    // MEEUS98: Chapter 22
    const double T = (jd_tt - JD_2000) / 36525;
    const double T2 = T * T;
    const double T3 = T2 * T;
    // Mean Elongation of the Moon from the Sun
    double D = ACoord::rangezero2threesixty(297.85036 + (445267.111480 * T) - (0.0019142 * T2) + (T3 / 189474));
    // Mean Anomaly of the Sun (Earth)
    double M = ACoord::rangezero2threesixty(357.52772 + (35999.050340 * T) - (0.0001603 * T2) - (T3 / 300000));
    // Mean Anomaly of the Moon
    double Mprime = ACoord::rangezero2threesixty(134.96298 + (477198.867398 * T) + (0.0086972 * T2) + (T3 / 56250));
    // Moon's Argument of Latitude
    double F = ACoord::rangezero2threesixty(93.27191 + (483202.017538 * T) - (0.0036825 * T2) + (T3 / 327270));
    // Longitude of the Ascending Node of the Moon's Mean Orbit on the Ecliptic, measured from the Mean Equinox of the Date
    double omega = ACoord::rangezero2threesixty(125.04452 - (1934.136261 * T) + (0.0020708 * T2) + (T3 / 450000));
    double value = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        const double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        //const double radargument = CAACoordinateTransformation::DegreesToRadians(argument);
        value += (coeff.sincoeff1 + (coeff.sincoeff2 * T)) * sin(deg2rad * argument) * 0.0001;
    }
    return rad ? deg2rad * value / 3600.0 : value / 3600.0;
    // retval is in degrees by default
}

double ANutation::NutationInObliquity(double jd_tt, bool rad) {
    // Nutation in Obliquity from AA+ v2.30
    // NOTE: Different from NutationInLongitude(), here we use the COSINES of the nutation table!
    const double T = (jd_tt - JD_2000) / 36525;
    const double T2 = T * T;
    const double T3 = T2 * T;
    double D = 297.85036 + (445267.111480 * T) - (0.0019142 * T2) + (T3 / 189474);
    D = ACoord::rangezero2threesixty(D);
    double M = 357.52772 + (35999.050340 * T) - (0.0001603 * T2) - (T3 / 300000);
    M = ACoord::rangezero2threesixty(M);
    double Mprime = 134.96298 + (477198.867398 * T) + (0.0086972 * T2) + (T3 / 56250);
    Mprime = ACoord::rangezero2threesixty(Mprime);
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * T2) + (T3 / 327270);
    F = ACoord::rangezero2threesixty(F);
    double omega = 125.04452 - (1934.136261 * T) + (0.0020708 * T2) + (T3 / 450000);
    omega = ACoord::rangezero2threesixty(omega);
    double value = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        argument *= deg2rad;
        value += (coeff.coscoeff1 + (coeff.coscoeff2 * T)) * cos(argument) * 0.0001;
    }
    return rad ? deg2rad * value / 3600.0 : value / 3600.0;
}

double ANutation::NutationInRightAscension(double dec, double ra, double obliq, double nut_lon, double nut_obl, bool rad) {
    if (!rad) {
        dec *= deg2rad;
        ra *= deg2rad;
        obliq *= deg2rad;
        nut_lon *= deg2rad;
        nut_obl *= deg2rad;
    }
    return rad ? (((cos(obliq) + (sin(obliq) * sin(ra) * tan(dec))) * nut_lon) - (cos(ra) * tan(dec) * nut_obl))
        : rad2deg * (((cos(obliq) + (sin(obliq) * sin(ra) * tan(dec))) * nut_lon) - (cos(ra) * tan(dec) * nut_obl));
}
double NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl, bool rad) {
    if (!rad) {
        ra *= deg2rad;
        obliq *= deg2rad;
        nut_lon *= deg2rad;
        nut_obl *= deg2rad;
    }
    return rad ? (sin(obliq) * cos(ra) * nut_lon + sin(ra) * nut_obl)
        : rad2deg * (sin(obliq) * cos(ra) * nut_lon + sin(ra) * nut_obl);
}

double AObliquity::MeanObliquityOfEcliptic(double jd_tt, bool rad) {
    // AA+: CAANutation::MeanObliquityOfEcliptic(JD)
    // MEEUS98: Chapter 22
    // Calculates mean obliquity using the following:
    // This formula is given in https://en.wikipedia.org/wiki/Axial_tilt credited to J.Laskar 1986
    // Original Source:  http://articles.adsabs.harvard.edu/pdf/1986A%26A...157...59L
    // Original Erratum: http://articles.adsabs.harvard.edu/pdf/1986A%26A...164..437L
    // Good to 0.02" over 1000 years, several arc seconds over 10000 years (around J2000.0)
    const double U = (jd_tt - JD_2000) / 3652500;  // U is JD in deca millenia, from J2000.0
    const double U2 = U * U;
    const double U3 = U2 * U;
    const double U4 = U3 * U;
    const double U5 = U4 * U;
    const double U6 = U5 * U;
    const double U7 = U6 * U;
    const double U8 = U7 * U;
    const double U9 = U8 * U;
    const double U10 = U9 * U;
    // Could roll up these constants for speed, but would lose precision
    double retval = dms2deg(23, 26, 21.448)
        - (ACoord::secs2deg(4680.93) * U)
        - (ACoord::secs2deg(1.55) * U2)
        + (ACoord::secs2deg(1999.25) * U3)
        - (ACoord::secs2deg(51.38) * U4)
        - (ACoord::secs2deg(249.67) * U5)
        - (ACoord::secs2deg(39.05) * U6)
        + (ACoord::secs2deg(7.12) * U7)
        + (ACoord::secs2deg(27.87) * U8)
        + (ACoord::secs2deg(5.79) * U9)
        + (ACoord::secs2deg(2.45) * U10);
    return rad ? deg2rad * retval : retval;
}

double AObliquity::TrueObliquityOfEcliptic(double jd_tt, bool rad) {
    return MeanObliquityOfEcliptic(jd_tt, rad) + ANutation::NutationInObliquity(jd_tt, rad);
}

