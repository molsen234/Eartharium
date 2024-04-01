
#include "aearth.h"
// These are separate so users of aearth.h don't include them
#include "vsop87/aearth_vsop87_full.h"
#include "vsop87/aearth_vsop87_short.h"
#include "aearth_nutation_table.h"
#include "aearth_aberration_table.h"

#include "asun.h"

double AEarth::EclipticLongitude(double jd_tt, Ephemeris eph) noexcept {
    if (eph == VSOP87_FULL) return VSOP87_D_Longitude(jd_tt);

    // Calculate Short VSOP87 D from Meeus98
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };
    const double rho5{ rho4 * rho };

    double L0{ 0 };
    for (const auto& L0Coefficient : g_L0EarthCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1{ 0 };
    for (const auto& L1Coefficient : g_L1EarthCoefficients)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2{ 0 };
    for (const auto& L2Coefficient : g_L2EarthCoefficients)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3{ 0 };
    for (const auto& L3Coefficient : g_L3EarthCoefficients)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4{ 0 };
    for (const auto& L4Coefficient : g_L4EarthCoefficients)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5{ 0 };
    for (const auto& L5Coefficient : g_L5EarthCoefficients)
        L5 += (L5Coefficient.A * cos(L5Coefficient.B + (L5Coefficient.C * rho)));

    double value{ (L0 + (L1 * rho) + (L2 * rhosquared) + (L3 * rhocubed) + (L4 * rho4) + (L5 * rho5)) / 100000000 };
    value = ACoord::rangezero2tau(value);
    //return rad ? value : rad2deg * value;
    return value;
}
double AEarth::EclipticLatitude(double jd_tt, Ephemeris eph) noexcept {
    if (eph == VSOP87_FULL) return VSOP87_D_Latitude(jd_tt);

    // Currently default is to use VSOP87_SHORT, might swap that with default VSOP87_FULL
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };

    double B0{ 0 };
    for (const auto& B0Coefficient : g_B0EarthCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1{ 0 };
    for (const auto& B1Coefficient : g_B1EarthCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    //Note for Earth there are no B2, B3 or B4 coefficients to calculate

    double value{ (B0 + (B1 * rho)) / 100000000 };

    value = ACoord::rangemhalfpi2halfpi(value);
    //return rad ? value : rad2deg * value;
    return value;
}
double AEarth::EclipticDistance(double jd_tt, Ephemeris eph) noexcept {
    if (eph == VSOP87_FULL) return VSOP87_D_Distance(jd_tt);

    // Currently default is to use VSOP87_SHORT, might swap that with default VSOP87_FULL
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };

    double R0{ 0 };
    for (const auto& R0Coefficient : g_R0EarthCoefficients)
        R0 += (R0Coefficient.A * cos(R0Coefficient.B + (R0Coefficient.C * rho)));
    double R1{ 0 };
    for (const auto& R1Coefficient : g_R1EarthCoefficients)
        R1 += (R1Coefficient.A * cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2{ 0 };
    for (const auto& R2Coefficient : g_R2EarthCoefficients)
        R2 += (R2Coefficient.A * cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3{ 0 };
    for (const auto& R3Coefficient : g_R3EarthCoefficients)
        R3 += (R3Coefficient.A * cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4{ 0 };
    for (const auto& R4Coefficient : g_R4EarthCoefficients)
        R4 += (R4Coefficient.A * cos(R4Coefficient.B + (R4Coefficient.C * rho)));

    return (R0 + (R1 * rho) + (R2 * rhosquared) + (R3 * rhocubed) + (R4 * rho4)) / 100000000;  // in VSOP87 AU
}
LLD AEarth::EclipticCoordinates(double jd_tt, Ephemeris eph) {
    // Apart from calculating powers of time parameter, there is no gain if consolidating the component functions
    return { EclipticLatitude(jd_tt, eph), EclipticLongitude(jd_tt, eph), EclipticDistance(jd_tt, eph) };
}

double AEarth::EclipticLongitudeJ2000(double jd_tt, Ephemeris eph) noexcept {
    if (eph == VSOP87_FULL) return VSOP87_B_Longitude(jd_tt);

    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };

    double L0{ 0 };
    for (const auto& L0Coefficient : g_L0EarthCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1{ 0 };
    for (const auto& L1Coefficient : g_L1EarthCoefficientsJ2000)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2{ 0 };
    for (const auto& L2Coefficient : g_L2EarthCoefficientsJ2000)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3{ 0 };
    for (const auto& L3Coefficient : g_L3EarthCoefficientsJ2000)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4{ 0 };
    for (const auto& L4Coefficient : g_L4EarthCoefficientsJ2000)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));

    double value{ (L0 + (L1 * rho) + (L2 * rhosquared) + (L3 * rhocubed) + (L4 * rho4)) / 100000000 };

    //value = ACoord::rangezero2tau(value);
    //return rad ? value : rad2deg * value;
    return ACoord::rangezero2tau(value);
}
double AEarth::EclipticLatitudeJ2000(double jd_tt, Ephemeris eph) noexcept {
    if (eph == VSOP87_FULL) return VSOP87_B_Latitude(jd_tt);

    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };

    double B0{ 0 };
    for (const auto& B0Coefficient : g_B0EarthCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1{ 0 };
    for (const auto& B1Coefficient : g_B1EarthCoefficientsJ2000)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2{ 0 };
    for (const auto& B2Coefficient : g_B2EarthCoefficientsJ2000)
        B2 += (B2Coefficient.A * cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3{ 0 };
    for (const auto& B3Coefficient : g_B3EarthCoefficientsJ2000)
        B3 += (B3Coefficient.A * cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4{ 0 };
    for (const auto& B4Coefficient : g_B4EarthCoefficientsJ2000)
        B4 += (B4Coefficient.A * cos(B4Coefficient.B + (B4Coefficient.C * rho)));

    double value{ (B0 + (B1 * rho) + (B2 * rhosquared) + (B3 * rhocubed) + (B4 * rho4)) / 100000000 };

    //value = ACoord::rangemhalfpi2halfpi(value);
    //return rad ? value : rad2deg * value;
    return ACoord::rangemhalfpi2halfpi(value);
}

// Obliquity
double AEarth::MeanObliquityOfEcliptic(double jd_tt) {
    // AA+: CAANutation::MeanObliquityOfEcliptic(JD)
    // MEEUS98: Chapter 22
    // Calculates mean obliquity using the following:
    // This formula is given in https://en.wikipedia.org/wiki/Axial_tilt credited to J.Laskar 1986
    // Original Source:  http://articles.adsabs.harvard.edu/pdf/1986A%26A...157...59L
    // Original Erratum: http://articles.adsabs.harvard.edu/pdf/1986A%26A...164..437L
    // Good to 0.02" over 1000 years, several arc seconds over 10000 years (around J2000.0)
    // Validity Range - according to Meeus98 page 147:
    // "The accuracy of this expression is estimated at 0".01 after 1000 years (that is, between A.D. 1000 and 3000),
    // and a few seconds of arc after 10000 years."
    // Also: "It is important to note that formula (22.3) is valid only over a period of 10000 years each side of J2000.0,
    // that is, for |U| < 1.0."
    const double U = (jd_tt - JD_2000) / (JD_MILLENNIUM * 10.0);  // U is JD in deca millenia, from J2000.0
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
    double retval = dms2deg(23, 26, 21.448)  // Equation 22.3
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
    return deg2rad * retval;
}

double AEarth::TrueObliquityOfEcliptic(double jd_tt) {
    return MeanObliquityOfEcliptic(jd_tt) + NutationInObliquity(jd_tt);
}


// Nutation
double AEarth::NutationInLongitude(double jd_tt) {
    // AA+: CAANutation::NutationInLongitude(JD)
    // MEEUS98: Chapter 22
    // NOTE: In AA+ this returns arc seconds, here it is radians or degrees directly
    //       This matters in CAAPhysicalMoon and CAAMoon (and check ASun)
    const double T = (jd_tt - JD_2000) / JD_CENTURY;
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
    //return rad ? deg2rad * value / 3600.0 : value / 3600.0;
    return deg2rad * value / 3600.0;
}

double AEarth::NutationInObliquity(double jd_tt) {
    // Nutation in Obliquity from AA+ v2.30
    // NOTE: Different from NutationInLongitude(), here we use the COSINES of the nutation table!
    const double T = (jd_tt - JD_2000) / JD_CENTURY;
    const double T2 = T * T;
    const double T3 = T2 * T;
    // Mean elongation of the Moon from the Sun
    double D = ACoord::rangezero2threesixty(297.85036 + (445267.111480 * T) - (0.0019142 * T2) + (T3 / 189474));
    // Mean anomaly of the Sun (relative to Earth)
    double M = ACoord::rangezero2threesixty(357.52772 + (35999.050340 * T) - (0.0001603 * T2) - (T3 / 300000));
    // Mean anomaly of the Moon (relative to Earth)
    double Mprime = ACoord::rangezero2threesixty(134.96298 + (477198.867398 * T) + (0.0086972 * T2) + (T3 / 56250));
    // Moon's argument of Latitude
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * T2) + (T3 / 327270);
    F = ACoord::rangezero2threesixty(F);
    // Longitude of the ascending node of the Moon's mean orbit on the ecliptic, measured from the mean equinox of the date
    double omega = ACoord::rangezero2threesixty(125.04452 - (1934.136261 * T) + (0.0020708 * T2) + (T3 / 450000));
    
    double value = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        argument *= deg2rad;
        value += (coeff.coscoeff1 + (coeff.coscoeff2 * T)) * cos(argument) * 0.0001;
    }
    return deg2rad * value / 3600.0;
}

double AEarth::NutationInRightAscension(double ra, double dec, double obliq, double nut_lon, double nut_obl) {
    return ((cos(obliq) + (sin(obliq) * sin(ra) * tan(dec))) * nut_lon) - (cos(ra) * tan(dec) * nut_obl);
    // All the deg2rad and rad2deg and multiply or divide by 3600.0 seem to cancel out.
}

double AEarth::NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl) {
    return (sin(obliq) * cos(ra) * nut_lon) + (sin(ra) * nut_obl); // 
    // All the deg2rad and rad2deg and multiply or divide by 3600.0 seem to cancel out.
}


// time
double AEarth::EquationOfTime(double jd_tt, Ephemeris eph) noexcept {
    // AA+ CAAEquationOfTime
    // Meeus98 chapter 28
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };
    const double rho5{ rho4 * rho };
    //Calculate the Suns mean longitude in degrees
    const double L0{ ACoord::rangezero2threesixty(280.4664567 + (360007.6982779 * rho) + (0.03032028 * rhosquared) +
                                                                  (rhocubed / 49931) - (rho4 / 15300) - (rho5 / 2000000)) }; // Eqn 28.2
    //Calculate the Suns apparent right ascension
    const double SunLong{ ASun::ApparentEclipticLongitude(jd_tt, eph) }; // radians
    const double SunLat{ ASun::ApparentEclipticLatitude(jd_tt, eph) };   // radians
    double epsilon{ TrueObliquityOfEcliptic(jd_tt)};                     // radians

    const LLD Equatorial{ Spherical::Ecliptic2Equatorial(SunLong, SunLat, epsilon, true /*rad*/) }; // radians

    double E{ L0 - 0.0057183 - (rad2deg * Equatorial.lon) + (rad2deg * AEarth::NutationInLongitude(jd_tt)) * cos(epsilon) }; // degrees Eqn 28.1

    if (E > 180) E = -(360 - E);
    E *= 4; //Convert to minutes of time
    return E;
}

//Sidereal
double AEarth::MeanGreenwichSiderealTime(double jd_utc) noexcept { // 
    // MEEUS98: Chapter 12
    // AA+: CAASidereal::MeanGreenwichSiderealTime(JD)
    //Get the Julian day for the same day at midnight
    double jd_fraction = jd_utc - std::floor(jd_utc) + 0.5; //also good for negative jd
    double jd_midnight = jd_utc - jd_fraction;
    //Calculate the sidereal time at midnight
    double T{ (jd_midnight - JD_2000) / JD_CENTURY }; // Equation 12.1
    double TSquared{ T * T };
    double TCubed{ TSquared * T };
    double Value{ 100.46061837 + (36000.770053608 * T) + (0.000387933 * TSquared) - (TCubed / 38710000) }; // Equation 12.3
    // Account for the fraction of day. 1.00273790935 = mean_solar_day / mean_sidereal_day = 24h0m0s / 23h56m4.09..s
    Value += 24.0 * 15.0 * jd_fraction * 1.00273790935; // Sidereal time in degrees
    return ACoord::rangezero2tau(deg2rad * Value); // mean sidereal time in radians
}

double AEarth::ApparentGreenwichSiderealTime(double jd_utc) noexcept {
    // MEEUS98: Chapter 12
    // AA+: CAASidereal::ApparentGreenwichSiderealTime(JD)
    double meangsid = MeanGreenwichSiderealTime(jd_utc);
    // NOTE: The Obliquity and Nutation should be calculated using jd_tt, but Meeus mentions on page 88 (2nd ed)
    //       that this small difference can be ignored.
    double trueobliquity = MeanObliquityOfEcliptic(jd_utc) + NutationInObliquity(jd_utc);    // radians
    double nutationinlongitude = NutationInLongitude(jd_utc);                                // radians
    double gsidtime = meangsid + (nutationinlongitude * cos(trueobliquity));
    //return rad ? gsidtime : rad2deg * gsidtime;
    return gsidtime;  // apparent sidereal time in radians
}

glm::dvec3 AEarth::EarthVelocity(double jd_tt, Ephemeris eph) noexcept
{
    glm::dvec3 velocity{ 0.0, 0.0, 0.0 };

    if (eph == VSOP87_FULL)
    {
        velocity.x = VSOP87_A_dX(jd_tt);
        velocity.y = VSOP87_A_dY(jd_tt);
        velocity.z = VSOP87_A_dZ(jd_tt);
        velocity = FK5::getVSOP2FK5_J2000(velocity);
        velocity *= 100'000'000; // scale by 10e8 to match low precision calculation below
        return velocity;
    }

    // VSOP87_SHORT ephemeris A ? - Check in Meeus98
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double L2{    3.1761467 + (1021.3285546 * T) };
    const double L3{    1.7534703 + ( 628.3075849 * T) };
    const double L4{    6.2034809 + ( 334.0612431 * T) };
    const double L5{    0.5995465 + (  52.9690965 * T) };
    const double L6{    0.8740168 + (  21.3299095 * T) };
    const double L7{    5.4812939 + (   7.4781599 * T) };
    const double L8{    5.3118863 + (   3.8133036 * T) };
    const double Ldash{ 3.8103444 + (8399.6847337 * T) };
    const double D{     5.1984667 + (7771.3771486 * T) };
    const double Mdash{ 2.3555559 + (8328.6914289 * T) };
    const double F{     1.6279052 + (8433.4661601 * T) };

    for (const auto& coeff : g_AberrationCoefficients) {
        const double Argument{ (coeff.L2 * L2) + (coeff.L3 * L3) + (coeff.L4 * L4) + (coeff.L5 * L5)
                                + (coeff.L6 * L6) + (coeff.L7 * L7) + (coeff.L8 * L8) + (coeff.Ldash * Ldash)
                                + (coeff.D * D) + (coeff.Mdash * Mdash) + (coeff.F * F) };

        const double sinArgument{ sin(Argument) };
        const double cosArgument{ cos(Argument) };
        velocity.x += (coeff.xsin + (coeff.xsint * T)) * sinArgument;
        velocity.x += (coeff.xcos + (coeff.xcost * T)) * cosArgument;
        velocity.y += (coeff.ysin + (coeff.ysint * T)) * sinArgument;
        velocity.y += (coeff.ycos + (coeff.ycost * T)) * cosArgument;
        velocity.z += (coeff.zsin + (coeff.zsint * T)) * sinArgument;
        velocity.z += (coeff.zcos + (coeff.zcost * T)) * cosArgument;
    }
    return velocity;
}

LLD AEarth::EquatorialAberration(double Alpha, double Delta, double jd_tt, Ephemeris eph) noexcept {
    //const double alpha = hrs2rad * Alpha;  // already in radians
    //const double delta = deg2rad * Delta;

    const double cosAlpha{ cos(Alpha) };
    const double sinAlpha{ sin(Alpha) };
    const double cosDelta{ cos(Delta) };
    const double sinDelta{ sin(Delta) };

    const glm::dvec3 velocity{ EarthVelocity(jd_tt, eph) };

    LLD aberration;
    aberration.lon = ((velocity.y * cosAlpha) - (velocity.x * sinAlpha)) / (17'314'463'350.0 * cosDelta);
    aberration.lat = -(((((velocity.x * cosAlpha) + (velocity.y * sinAlpha)) * sinDelta) - (velocity.z * cosDelta)) / 17'314'463'350.0);

    return aberration; // (dRA, dDec, 0.0) in radians
}

LLD AEarth::EclipticAberration(const double Lambda, const double Beta, const double jd_tt, const Ephemeris eph) noexcept {
    //double lambda = deg2rad * Lambda;  // already in radians
    //double beta = deg2rad * Beta;

    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double e{ 0.016708634 - (0.000042037 * T) - (0.0000001267 * Tsquared) };
    double pi{ deg2rad * (102.93735 + (1.71946 * T) + (0.00046 * Tsquared)) };
    constexpr double k{ 20.49552 };
    double SunLongitude{ ASun::GeometricEclipticLongitude(jd_tt, eph) }; // radians

    LLD aberration;
    aberration.lon = (-k * cos(SunLongitude - Lambda) + e * k * cos(pi - Lambda)) / cos(Beta) / 3600;
    aberration.lat = -k * sin(Beta) * (sin(SunLongitude - Lambda) - e * sin(pi - Lambda)) / 3600;

    return aberration;
}


// Refraction

// ---------------------
//  Refraction function - Was in Eartharium.cpp, might need to be put somewhere
// ---------------------
//double calcRefraction(Application& app, double altitude, double pressure = 1013.25, double temperature = 15.0) {
//    // pressure in mBar, temperature in Celsius, altitude in degrees, returns arc minutes
//    // Source: Almanac method https://www.madinstro.net/sundry/navsext.html
//    // Bennett method described in J.Meeus, implemented in AA+
//    double retval = NO_DOUBLE;
//    if (app.sio_refmethod == REFR_BENNETT) retval = 60.0 * CAARefraction::RefractionFromApparent(altitude, pressure, temperature);
//    else if (app.sio_refmethod == REFR_ALMANAC) retval = (0.267 * pressure / (temperature + 273.15))
//        / tan(deg2rad * (altitude + 0.04848 / (tan(deg2rad * altitude) + 0.028)));
//    else std::cout << "WARNING: calcRefraction() was called while app.sio_refmethod was set to unknown refraction method!\n";
//    return retval;
//}

//double Earth::calcRefractionAlmanac(double elevation, double temperature, double pressure) {
//    // returns refraction correction in arcminutes. Takes elevation in degrees, temperature in Celcius (centigrade), pressure in hPa (mbar)
//    // Source: https://www.madinstro.net/sundry/navsext.html
//    return (0.267 * pressure / (temperature + 273.15)) / tan(deg2rad * (elevation + 0.04848 / (tan(deg2rad * elevation) + 0.028)));
//}
//double Earth::calcRefractionBennett(double elevation, double temperature = 10, double pressure = 1010) {
//    // returns refraction correction in arcminutes. Takes elevation in degrees, temperature in Celcius (centigrade), pressure in hPa (mbar)
//    // Source: J.Meeus AA1998
//    // AA+: CAARefraction::RefractionFromApparent()
//    if (elevation <= -1.6962987799993996)
//        elevation = -1.6962987799993996;
//    return (1 / (tan(deg2rad * (elevation + (7.31 / (elevation + 4.4))))) + 0.0013515) * (pressure / 1010 * 283 / (273 + temperature)) / 60;
//}

double AEarth::RefractionFromApparent(double Altitude, double Pressure, double Temperature) noexcept {
    // Bennett refraction formula
    // From AA+ CAARefraction
    // Meeus98 chapter 16
    Altitude *= rad2deg;
    //return a constant value from this method if the altitude is below a specific value
    if (Altitude <= -1.6962987799993996)
        Altitude = -1.6962987799993996;

    double value{ 1 / (tan(deg2rad * (Altitude + (7.31 / (Altitude + 4.4))))) + 0.0013515 };
    value *= (Pressure / 1010 * 283 / (273 + Temperature));
    return deg2rad * value / 60.0;  // correction in radians
}
double AEarth::RefractionFromTrue(double Altitude, double Pressure, double Temperature) noexcept {
    // Sæmundsson refraction formula
    // From AA+ CAARefraction
    // Meeus98 chapter 16
    Altitude *= rad2deg;
    //return a constant value from this method if the altitude is below a specific value
    if (Altitude <= -1.9006387000003735)
        Altitude = -1.9006387000003735;

    double value{ 1.02 / (tan(deg2rad * (Altitude + (10.3 / (Altitude + 5.11))))) + 0.0019279 };
    value *= (Pressure / 1010 * 283 / (273 + Temperature));
    return deg2rad * value / 60.0;  // correction in radians
}


// CAAGlobe - The figure of Earth
// Meeus98: chapter 11
// Constants assume the following ellipsoid parameters (IAU 1976 - System of Astronomical Constants):
// a = 6378.140 km
// 1/f = 298.257
// thus:
// flattening f = 1/298.257
// semiminor axis b = a(1-f) = 6356.755
// b/a = (1-f) = 0.996'647'19
// eccentricity of meridian e = sqrt(2f-f^2) = 0.081'829'22
double AEarth::RhoSinThetaPrime(double GeographicalLatitude, double Height) noexcept {
    // used for diurnal parallaxes, eclipses and occultations where a precise observer location is essential
    // height in meters, latitude in radians
    const double U{ atan(0.99664719 * tan(GeographicalLatitude)) };
    return (0.99664719 * sin(U)) + (Height / 6378140 * sin(GeographicalLatitude));
}
double AEarth::RhoCosThetaPrime(double GeographicalLatitude, double Height) noexcept {
    // used for diurnal parallaxes, eclipses and occultations where a precise observer location is essential
    const double U{ atan(0.99664719 * tan(GeographicalLatitude)) };
    return cos(U) + (Height / 6378140 * cos(GeographicalLatitude));
}
double AEarth::RadiusOfParallelOfLatitude(double GeographicalLatitude) noexcept {
    // returns radius in km
    const double sinGeo{ sin(GeographicalLatitude) };
    return (6378.14 * cos(GeographicalLatitude)) / (sqrt(1 - (0.0066943847614084 * sinGeo * sinGeo)));
}
double AEarth::RadiusOfCurvature(double GeographicalLatitude) noexcept {
    // returns radius in km
    const double sinGeo{ sin(GeographicalLatitude) };
    return (6378.14 * (1 - 0.0066943847614084)) / pow((1 - (0.0066943847614084 * sinGeo * sinGeo)), 1.5);
}
double AEarth::DistanceBetweenPoints(double GeographicalLatitude1, double GeographicalLongitude1, double GeographicalLatitude2, double GeographicalLongitude2) noexcept {
    // returns distance in km
    const double F{ (GeographicalLatitude1 + GeographicalLatitude2) / 2 };
    const double G{ (GeographicalLatitude1 - GeographicalLatitude2) / 2 };
    const double lambda{ (GeographicalLongitude1 - GeographicalLongitude2) / 2 };
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
    const double D{ 2 * w * 6378.14 };
    const double Hprime{ (3 * R - 1) / (2 * C) };
    const double Hprime2{ (3 * R + 1) / (2 * S) };
    constexpr double f{ 0.0033528131778969144060323814696721 };
    return D * (1 + (f * Hprime * sinF * sinF * cosG * cosG) - (f * Hprime2 * cosF * cosF * sinG * sinG));
}

// Parallax - Meeus98 chapter 40

// !!! FIX: Decide where to put this, and how to define it
constexpr double g_AAParallax_C1 = 4.2634515103856459e-05; //sin(CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, 8.794)));

double AEarth::DistanceToParallax(double Distance) noexcept {
    // Distance in Astronomical Units (AU) -> Parallax angle in radians
    return asin(g_AAParallax_C1 / Distance);
}
double AEarth::ParallaxToDistance(double Parallax) noexcept {
    // Parallax angle in radians -> Distance in Astronomical Units (AU)
    return g_AAParallax_C1 / sin(Parallax);
}

LLD AEarth::Equatorial2TopocentricDelta(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double jd_utc) noexcept {
    const double rhoSinThetaPrime{ RhoSinThetaPrime(Latitude, Height) };
    const double rhoCosThetaPrime{ RhoCosThetaPrime(Latitude, Height) };

    //Calculate the Sidereal time
    const double theta{ ApparentGreenwichSiderealTime(jd_utc) };

    const double cosDelta{ cos(Delta) };

    //Calculate the Parallax
    const double pi{ asin(g_AAParallax_C1 / Distance) };

    //Calculate the hour angle
    const double H{ theta - Longitude - Alpha };  // already in radians
    const double cosH{ cos(H) };
    const double sinH{ sin(H) };

    LLD DeltaTopocentric;
    DeltaTopocentric.lon = -pi * rhoCosThetaPrime * sinH / cosDelta;
    DeltaTopocentric.lat = -pi * ((rhoSinThetaPrime * cosDelta) - (rhoCosThetaPrime * cosH * sin(Delta)));
    return DeltaTopocentric;  // difference between Spherical Geocentric Equatorial and Spherical Topocentric Equatorial in radians
}

LLD AEarth::Equatorial2Topocentric(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double jd_utc) noexcept {
    const double rhoSinThetaPrime{ RhoSinThetaPrime(Latitude, Height) };
    const double rhoCosThetaPrime{ RhoCosThetaPrime(Latitude, Height) };

    //Calculate the Sidereal time
    const double theta{ ApparentGreenwichSiderealTime(jd_utc) };

    const double cosDelta{ cos(Delta) };

    //Calculate the Parallax
    const double pi{ asin(g_AAParallax_C1 / Distance) };
    const double sinpi{ sin(pi) };  // !!! FIX: why take sine of arcsine here?

    //Calculate the hour angle
    const double H{ theta - Longitude - Alpha };
    const double cosH{ cos(H) };
    const double sinH{ sin(H) };

    //Calculate the adjustment in right ascension
    const double DeltaAlpha{ atan2(-rhoCosThetaPrime * sinpi * sinH, cosDelta - (rhoCosThetaPrime * sinpi * cosH)) };

    LLD Topocentric;
    Topocentric.lon = ACoord::rangezero2tau(Alpha + DeltaAlpha);
    Topocentric.lat = (atan2((sin(Delta) - (rhoSinThetaPrime * sinpi)) * cos(DeltaAlpha), cosDelta - (rhoCosThetaPrime * sinpi * cosH)));
    return Topocentric;  // Spherical Topocentric Equatorial coordinates in radians
}

LLD AEarth::Ecliptic2Topocentric(double Lambda, double Beta, double Semidiameter, double Distance, double Epsilon, double Latitude, double Height, double jd_utc) noexcept {
    // note the .dst value in the returned LLD holds the semidiameter of the object in radians
    const double S{ RhoSinThetaPrime(Latitude, Height) };
    const double C{ RhoCosThetaPrime(Latitude, Height) };

    const double sine{ sin(Epsilon) };
    const double cose{ cos(Epsilon) };
    const double cosBeta{ cos(Beta) };
    const double sinBeta{ sin(Beta) };

    //Calculate the Sidereal time
    double theta{ ApparentGreenwichSiderealTime(jd_utc) };
    const double sintheta{ sin(theta) };

    //Calculate the Parallax
    const double pi{ asin(g_AAParallax_C1 / Distance) };
    const double sinpi{ sin(pi) };  // !!! FIX: why take sine of arcsine here?

    const double N{ (cos(Lambda) * cosBeta) - (C * sinpi * cos(theta)) };

    LLD Topocentric;
    Topocentric.lon = atan2((sin(Lambda) * cosBeta) - (sinpi * ((S * sine) + (C * cose * sintheta))), N);
    const double cosTopocentricLambda{ cos(Topocentric.lon) };
    Topocentric.lat = atan(cosTopocentricLambda * (sinBeta - (sinpi * ((S * cose) - (C * sine * sintheta)))) / N);
    Topocentric.dst = asin(cosTopocentricLambda * cos(Topocentric.lat) * sin(Semidiameter) / N);

    Topocentric.lon = ACoord::rangezero2tau(Topocentric.lon);

    return Topocentric;  // Spherical Topocentric Ecliptic coordinates in radians
}

// Equinoxes and Solstices

double AEarth::NorthwardEquinox(long Year, Ephemeris eph) noexcept {
    //calculate the approximate date
    double JDE{ 0 };
    if (Year <= 1000) {
        const double Y{ Year / 1000.0 };
        const double Ysquared{ Y * Y };
        const double Ycubed{ Ysquared * Y };
        const double Y4{ Ycubed * Y };
        JDE = 1721139.29189 + (365242.13740 * Y) + (0.06134 * Ysquared) + (0.00111 * Ycubed) - (0.00071 * Y4);
    } else {
        const double Y{ (Year - 2000.0) / 1000.0 };
        const double Ysquared{ Y * Y };
        const double Ycubed{ Ysquared * Y };
        const double Y4{ Ycubed * Y };
        JDE = 2451623.80984 + (365242.37404 * Y) + (0.05169 * Ysquared) - (0.00411 * Ycubed) - (0.00057 * Y4);
    }
    double Correction{ 0 };
    do
    {
        const double SunLongitude{ ASun::ApparentEclipticLongitude(JDE, eph) };
        Correction = 58 * sin(deg2rad * -SunLongitude);
        JDE += Correction;
    } while (fabs(Correction) > 0.00001); //Corresponds to an error of 0.86 of a second
    return JDE;
}
double AEarth::NorthernSolstice(long Year, Ephemeris eph) noexcept {
    //calculate the approximate date
    double JDE{ 0 };
    if (Year <= 1000) {
        const double Y{ Year / 1000.0 };
        const double Ysquared{ Y * Y };
        const double Ycubed{ Ysquared * Y };
        const double Y4{ Ycubed * Y };
        JDE = 1721233.25401 + (365241.72562 * Y) - (0.05323 * Ysquared) + (0.00907 * Ycubed) + (0.00025 * Y4);
    } else {
        const double Y{ (Year - 2000.0) / 1000.0 };
        const double Ysquared{ Y * Y };
        const double Ycubed{ Ysquared * Y };
        const double Y4{ Ycubed * Y };
        JDE = 2451716.56767 + (365241.62603 * Y) + (0.00325 * Ysquared) + (0.00888 * Ycubed) - (0.00030 * Y4);
    }
    double Correction{ 0 };
    do
    {
        const double SunLongitude{ ASun::ApparentEclipticLongitude(JDE, eph) };
        Correction = 58 * sin(deg2rad * (90 - SunLongitude));
        JDE += Correction;
    } while (fabs(Correction) > 0.00001); //Corresponds to an error of 0.86 of a second
    return JDE;
}

double AEarth::SouthwardEquinox(long Year, Ephemeris eph) noexcept {
    //calculate the approximate date
    double JDE{ 0 };
    if (Year <= 1000) {
        const double Y{ Year / 1000.0 };
        const double Ysquared{ Y * Y };
        const double Ycubed{ Ysquared * Y };
        const double Y4{ Ycubed * Y };
        JDE = 1721325.70455 + (365242.49558 * Y) - (0.11677 * Ysquared) - (0.00297 * Ycubed) + (0.00074 * Y4);
    } else {
        const double Y{ (Year - 2000.0) / 1000.0 };
        const double Ysquared{ Y * Y };
        const double Ycubed{ Ysquared * Y };
        const double Y4{ Ycubed * Y };
        JDE = 2451810.21715 + (365242.01767 * Y) - (0.11575 * Ysquared) + (0.00337 * Ycubed) + (0.00078 * Y4);
    }
    double Correction{ 0 };
    do
    {
        const double SunLongitude{ ASun::ApparentEclipticLongitude(JDE, eph) };
        Correction = 58 * sin(deg2rad * (180 - SunLongitude));
        JDE += Correction;
    } while (fabs(Correction) > 0.00001); //Corresponds to an error of 0.86 of a second
    return JDE;
}
double AEarth::SouthernSolstice(long Year, Ephemeris eph) noexcept {
    //calculate the approximate date
    double JDE{ 0 };
    if (Year <= 1000) {
        const double Y{ Year / 1000.0 };
        const double Ysquared{ Y * Y };
        const double Ycubed{ Ysquared * Y };
        const double Y4{ Ycubed * Y };
        JDE = 1721414.39987 + (365242.88257 * Y) - (0.00769 * Ysquared) - (0.00933 * Ycubed) - (0.00006 * Y4);
    } else {
        const double Y{ (Year - 2000.0) / 1000.0 };
        const double Ysquared{ Y * Y };
        const double Ycubed{ Ysquared * Y };
        const double Y4{ Ycubed * Y };
        JDE = 2451900.05952 + (365242.74049 * Y) - (0.06223 * Ysquared) - (0.00823 * Ycubed) + (0.00032 * Y4);
    }
    double Correction{ 0 };
    do
    {
        const double SunLongitude{ ASun::ApparentEclipticLongitude(JDE, eph) };
        Correction = 58 * sin(deg2rad * (270 - SunLongitude));
        JDE += Correction;
    } while (fabs(Correction) > 0.00001); //Corresponds to an error of 0.86 of a second
    return JDE;
}

double AEarth::LengthOfSpring(long Year, Hemisphere hemi, Ephemeris eph) noexcept
{
    if (hemi == NORTHERN) return NorthernSolstice(Year, eph) - NorthwardEquinox(Year, eph);
    else return SouthernSolstice(Year, eph) - SouthwardEquinox(Year, eph);
}
double AEarth::LengthOfSummer(long Year, Hemisphere hemi, Ephemeris eph) noexcept
{
    if (hemi == NORTHERN) return SouthwardEquinox(Year, eph) - NorthernSolstice(Year, eph);
    else {
        //The Summer season wraps around into the following year for the southern hemisphere
        return NorthwardEquinox(Year + 1, eph) - SouthernSolstice(Year, eph);
    }
}
double AEarth::LengthOfAutumn(long Year, Hemisphere hemi, Ephemeris eph) noexcept
{
    if (hemi == NORTHERN) return SouthernSolstice(Year, eph) - SouthwardEquinox(Year, eph);
    else return NorthernSolstice(Year, eph) - NorthwardEquinox(Year, eph);
}
double AEarth::LengthOfWinter(long Year, Hemisphere hemi, Ephemeris eph) noexcept
{
    if (hemi == NORTHERN) {
        //The Winter season wraps around into the following year for the Northern hemisphere
        return NorthwardEquinox(Year + 1, eph) - SouthernSolstice(Year, eph);
    }
    else return SouthwardEquinox(Year, eph) - NorthernSolstice(Year, eph);
}





// VSOP87
// ======

// VSOP87 Orbital Parameters - Heliocentric Ecliptic at Equinox of J2000.0
double AEarth::A(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_A_EMB.data(), g_VSOP87_A_EMB.size(), false);
}
double AEarth::L(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_L_EMB.data(), g_VSOP87_L_EMB.size(), true);
}
double AEarth::K(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_K_EMB.data(), g_VSOP87_K_EMB.size(), false);
}
double AEarth::H(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_H_EMB.data(), g_VSOP87_H_EMB.size(), false);
}
double AEarth::Q(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_Q_EMB.data(), g_VSOP87_Q_EMB.size(), false);
}
double AEarth::P(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_P_EMB.data(), g_VSOP87_P_EMB.size(), false);
}

// Ephemeris A - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
double AEarth::VSOP87_A_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_X_EARTH.data(), g_VSOP87A_X_EARTH.size(), false);
}
double AEarth::VSOP87_A_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Y_EARTH.data(), g_VSOP87A_Y_EARTH.size(), false);
}
double AEarth::VSOP87_A_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Z_EARTH.data(), g_VSOP87A_Z_EARTH.size(), false);
}
double AEarth::VSOP87_A_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_X_EARTH.data(), g_VSOP87A_X_EARTH.size());
}
double AEarth::VSOP87_A_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Y_EARTH.data(), g_VSOP87A_Y_EARTH.size());
}
double AEarth::VSOP87_A_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Z_EARTH.data(), g_VSOP87A_Z_EARTH.size());
}

// Ephemeris B - Spherical Heliocentric Ecliptic at J2000.0
double AEarth::VSOP87_B_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_L_EARTH.data(), g_VSOP87B_L_EARTH.size(), true);
}
double AEarth::VSOP87_B_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87B_B_EARTH.data(), g_VSOP87B_B_EARTH.size(), true));
}
double AEarth::VSOP87_B_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_R_EARTH.data(), g_VSOP87B_R_EARTH.size(), false);
}
double AEarth::VSOP87_B_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_L_EARTH.data(), g_VSOP87B_L_EARTH.size());
}
double AEarth::VSOP87_B_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_B_EARTH.data(), g_VSOP87B_B_EARTH.size());
}
double AEarth::VSOP87_B_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_R_EARTH.data(), g_VSOP87B_R_EARTH.size());
}

// Ephemeris C - Rectangular Heliocentric Ecliptic at Equinox of Date
double AEarth::VSOP87_C_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_X_EARTH.data(), g_VSOP87C_X_EARTH.size(), false);
}
double AEarth::VSOP87_C_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Y_EARTH.data(), g_VSOP87C_Y_EARTH.size(), false);
}
double AEarth::VSOP87_C_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Z_EARTH.data(), g_VSOP87C_Z_EARTH.size(), false);
}
double AEarth::VSOP87_C_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_X_EARTH.data(), g_VSOP87C_X_EARTH.size());
}
double AEarth::VSOP87_C_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Y_EARTH.data(), g_VSOP87C_Y_EARTH.size());
}
double AEarth::VSOP87_C_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Z_EARTH.data(), g_VSOP87C_Z_EARTH.size());
}

// Ephemeris D - Spherical Heliocentric Ecliptic at Equinox of Date
double AEarth::VSOP87_D_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_L_EARTH.data(), g_VSOP87D_L_EARTH.size(), true);
}
double AEarth::VSOP87_D_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87D_B_EARTH.data(), g_VSOP87D_B_EARTH.size(), true));
}
double AEarth::VSOP87_D_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_R_EARTH.data(), g_VSOP87D_R_EARTH.size(), false);
}
double AEarth::VSOP87_D_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_L_EARTH.data(), g_VSOP87D_L_EARTH.size());
}
double AEarth::VSOP87_D_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_B_EARTH.data(), g_VSOP87D_B_EARTH.size());
}
double AEarth::VSOP87_D_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_R_EARTH.data(), g_VSOP87D_R_EARTH.size());
}

// VSOP87 Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
double AEarth::VSOP87_E_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_X_EARTH.data(), g_VSOP87E_X_EARTH.size(), false);
}
double AEarth::VSOP87_E_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Y_EARTH.data(), g_VSOP87E_Y_EARTH.size(), false);
}
double AEarth::VSOP87_E_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Z_EARTH.data(), g_VSOP87E_Z_EARTH.size(), false);
}
double AEarth::VSOP87_E_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_X_EARTH.data(), g_VSOP87E_X_EARTH.size());
}
double AEarth::VSOP87_E_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Y_EARTH.data(), g_VSOP87E_Y_EARTH.size());
}
double AEarth::VSOP87_E_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Z_EARTH.data(), g_VSOP87E_Z_EARTH.size());
}

