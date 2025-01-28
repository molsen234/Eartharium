
#include "aearth.h"

// These are separate so users of aearth.h don't include them
#include "vsop87/aearth_vsop87_full.h"
#include "vsop87/aearth_vsop87_short.h"
#include "aearth_nutation_table.h"
#include "aearth_aberration_table.h"
#include "aearth_precession_tables.h"

#include "asun.h"
#include "datetime.h"

double AEarth::EclipticLongitude(double jd_tt, Planetary_Ephemeris eph) noexcept {
    if (eph == EPH_VSOP87_FULL) return VSOP87D_Longitude(jd_tt);

    // Calculate Short VSOP87 D from Meeus98
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
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
    double value{ (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * (L4 + rho * L5))))) / 100'000'000 };
    value = ACoord::rangezero2tau(value);
    return value;
}
double AEarth::EclipticLatitude(double jd_tt, Planetary_Ephemeris eph) noexcept {
    if (eph == EPH_VSOP87_FULL) return VSOP87D_Latitude(jd_tt);

    // Currently default is to use VSOP87_SHORT, might swap that with default VSOP87_FULL
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    double B0{ 0 };
    for (const auto& B0Coefficient : g_B0EarthCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1{ 0 };
    for (const auto& B1Coefficient : g_B1EarthCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    //Note for Earth there are no B2, B3 or B4 coefficients to calculate
    double value{ (B0 + (B1 * rho)) / 100'000'000 };
    value = ACoord::rangemhalfpi2halfpi(value);
    return value;
}
double AEarth::EclipticDistance(double jd_tt, Planetary_Ephemeris eph) noexcept {
    if (eph == EPH_VSOP87_FULL) return VSOP87D_Distance(jd_tt);

    // Currently default is to use VSOP87_SHORT, might swap that with default VSOP87_FULL
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
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
    return (R0 + rho * (R1 + rho * (R2 + rho * (R3 + rho * R4)))) / 100'000'000;  // in VSOP87 AU
}
LLD AEarth::EclipticCoordinates(double jd_tt, Planetary_Ephemeris eph) noexcept {
    LLD retval{};
    retval.lat = EclipticLatitude(jd_tt, eph);
    retval.lon = EclipticLongitude(jd_tt, eph);
    retval.dst = EclipticDistance(jd_tt, eph);
    return retval;
}

double AEarth::EclipticLongitudeJ2000(double jd_tt, Planetary_Ephemeris eph) noexcept {
    if (eph == EPH_VSOP87_FULL) return VSOP87B_Longitude(jd_tt);
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
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
    return ACoord::rangezero2tau((L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * L4)))) / 100'000'000);
}
double AEarth::EclipticLatitudeJ2000(double jd_tt, Planetary_Ephemeris eph) noexcept {
    if (eph == EPH_VSOP87_FULL) return VSOP87B_Latitude(jd_tt);
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
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
    return ACoord::rangemhalfpi2halfpi((B0 + rho * (B1 + rho * (B2 +rho * (B3 + rho * B4)))) / 100000000);
}
LLD AEarth::EclipticCoordinatesJ2000(double jd_tt, Planetary_Ephemeris eph) noexcept {
    LLD retval{};
    retval.lat = EclipticLatitudeJ2000(jd_tt, eph);
    retval.lon = EclipticLongitudeJ2000(jd_tt, eph);
    retval.dst = EclipticDistance(jd_tt, eph);  // Distance doesn't change with precession
    return retval;
}


// Precession - !!! FIX: add IAU2006 and TEST Vondrak precession

LLD AEarth::PrecessEquatorial(LLD decra, double JD0, double jd_tt) noexcept {
    // From AA+ CAAPrecession, modified to accept and return radians
    // See https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1977A%26A....58....1L&defaultprint=YES&filetype=.pdf
    LLD value{};
    const double T{ (JD0 - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double t{ (jd_tt - JD0) / JD_CENTURY };
    const double tsquared{ t * t };
    const double tcubed{ tsquared * t };
    const double cosDelta{ cos(decra.lat) };
    const double sinDelta{ sin(decra.lat) };
    const double sigma{ deg2rad * (ACoord::dms2deg(0, 0, ((2306.2181 + (1.39656 * T) - (0.000139 * Tsquared)) * t) + ((0.30188 - (0.000344 * T)) * tsquared) + (0.017998 * tcubed))) };
    const double zeta{ deg2rad * (ACoord::dms2deg(0, 0, ((2306.2181 + (1.39656 * T) - (0.000139 * Tsquared)) * t) + ((1.09468 + (0.000066 * T)) * tsquared) + (0.018203 * tcubed))) };
    const double phi{ deg2rad * (ACoord::dms2deg(0, 0, ((2004.3109 - (0.8533 * T) - (0.000217 * Tsquared)) * t) - ((0.42665 + (0.000217 * T)) * tsquared) - (0.041833 * tcubed))) };
    const double cosphi{ cos(phi) };
    const double sinphi{ sin(phi) };
    const double cosAlphaplussigma{ cos(decra.lon + sigma) };
    const double A{ cosDelta * sin(decra.lon + sigma) };
    const double B{ (cosphi * cosDelta * cosAlphaplussigma) - (sinphi * sinDelta) };
    const double C{ (sinphi * cosDelta * cosAlphaplussigma) + (cosphi * sinDelta) };
    value.lon = ACoord::rangezero2tau(atan2(A, B) + zeta);
    value.lat = asin(C);
    value.dst = decra.dst;
    return value;  // radians
}
LLD AEarth::PrecessEquatorialJ2000(LLD decra, double jd_tt) {
    // From PrecessEquatorial() by taking JD0 = JD_2000
    LLD retval{};
    const double t = (jd_tt - JD_2000) / 36525;
    const double tsquared = t * t;
    const double tcubed = tsquared * t;
    const double sigma = deg2rad * ACoord::dms2deg(0, 0, (2306.2181 * t) + (0.30188 * tsquared) + (0.017998 * tcubed));
    const double zeta = deg2rad * ACoord::dms2deg(0, 0, (2306.2181 * t) + (1.09468 * tsquared) + (0.018203 * tcubed));
    const double phi = deg2rad * ACoord::dms2deg(0, 0, (2004.3109 * t) - (0.42665 * tsquared) - (0.041833 * tcubed));
    const double A = cos(decra.lat) * sin(decra.lon + sigma);
    const double B = cos(phi) * cos(decra.lat) * cos(decra.lon + sigma) - sin(phi) * sin(decra.lat);
    const double C = sin(phi) * cos(decra.lat) * cos(decra.lon + sigma) + cos(phi) * sin(decra.lat);
    retval.lat = asin(C);
    retval.lon = atan2(A, B) + zeta;
    retval.dst = decra.dst;
    return retval;
}
LLD AEarth::PrecessEquatorialFK5(LLD decra, double JD0, double jd_tt) {
    return PrecessEquatorial2(decra.lon, decra.lat, JD0, jd_tt);
}
LLD AEarth::PrecessEquatorial2(double Alpha, double Delta, double JD0, double jd_tt) noexcept {
    // From AA+ CAAPrecession, modified to accept and return radians
    // See https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1977A%26A....58....1L&defaultprint=YES&filetype=.pdf
    LLD value{};
    const double T{ (JD0 - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double t{ (jd_tt - JD0) / JD_CENTURY };
    const double tsquared{ t * t };
    const double tcubed{ tsquared * t };
    const double cosDelta{ cos(Delta) };
    const double sinDelta{ sin(Delta) };
    const double sigma{ deg2rad * (ACoord::dms2deg(0, 0, ((2306.2181 + (1.39656 * T) - (0.000139 * Tsquared)) * t) + ((0.30188 - (0.000344 * T)) * tsquared) + (0.017998 * tcubed))) };
    const double zeta{ deg2rad * (ACoord::dms2deg(0, 0, ((2306.2181 + (1.39656 * T) - (0.000139 * Tsquared)) * t) + ((1.09468 + (0.000066 * T)) * tsquared) + (0.018203 * tcubed))) };
    const double phi{ deg2rad * (ACoord::dms2deg(0, 0, ((2004.3109 - (0.8533 * T) - (0.000217 * Tsquared)) * t) - ((0.42665 + (0.000217 * T)) * tsquared) - (0.041833 * tcubed))) };
    const double cosphi{ cos(phi) };
    const double sinphi{ sin(phi) };
    const double cosAlphaplussigma{ cos(Alpha + sigma) };
    const double A{ cosDelta * sin(Alpha + sigma) };
    const double B{ (cosphi * cosDelta * cosAlphaplussigma) - (sinphi * sinDelta) };
    const double C{ (sinphi * cosDelta * cosAlphaplussigma) + (cosphi * sinDelta) };
    value.lon = ACoord::rangezero2tau(atan2(A, B) + zeta);
    value.lat = asin(C);
    return value;  // radians
}
LLD AEarth::PrecessEquatorialFK4(double Alpha, double Delta, double JD0, double JD) noexcept {
    // From AA+ CAAPrecession, modified to accept and return radians
    LLD value{};
    const double T{ (JD0 - JD_B1950) / JD_TROPICAL_CENTURY }; // 2415020.3135 = B1950.0, 36524.2199 = tropical century (Fk4 is defined in those terms)
    const double t{ (JD - JD0) / JD_TROPICAL_CENTURY };
    const double tsquared{ t * t };
    const double tcubed{ tsquared * t };
    const double cosDelta{ cos(Delta) };
    const double sinDelta{ sin(Delta) };
    const double sigma{ (ACoord::dms2rad(0, 0, ((2304.250 + (1.396 * T)) * t) + (0.302 * tsquared) + (0.018 * tcubed))) };
    const double zeta{ (ACoord::dms2rad(0, 0, (0.791 * tsquared) + (0.001 * tcubed))) + sigma };
    const double phi{ (ACoord::dms2rad(0, 0, ((2004.682 - (0.853 * T)) * t) - (0.426 * tsquared) - (0.042 * tcubed))) };
    const double cosphi{ cos(phi) };
    const double sinphi{ sin(phi) };
    const double cosAlphaplussigma{ cos(Alpha + sigma) };
    const double A{ cosDelta * sin(Alpha + sigma) };
    const double B{ (cosphi * cosDelta * cosAlphaplussigma) - (sinphi * sinDelta) };
    const double C{ (sinphi * cosDelta * cosAlphaplussigma) + (cosphi * sinDelta) };
    const double DeltaAlpha{ hrs2rad * ACoord::dms2deg(0, 0, 0.0775 + (0.0850 * T)) };  // actually in hours, HMS2Hours() would be identical to DMS2degrees()
    value.lon = ACoord::rangezero2tau((atan2(A, B) + zeta) + DeltaAlpha);
    value.lat = asin(C);
    return value;  // radians
}
LLD AEarth::PrecessEcliptic(double Lambda, double Beta, double JD0, double JD) noexcept {
    // See https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1977A%26A....58....1L&defaultprint=YES&filetype=.pdf
    LLD value{};
    const double T{ (JD0 - JD_2000) / JD_CENTURY };
    const double T2{ T * T };
    const double t{ (JD - JD0) / JD_CENTURY };
    const double t2{ t * t };
    const double tcubed{ t2 * t };
    const double cosBeta{ cos(Beta) };
    const double sinBeta{ sin(Beta) };
    const double eta{ (ACoord::dms2rad(0, 0, ((47.0029 - (0.06603 * T) + (0.000598 * T2)) * t) + ((-0.03302 + (0.000598 * T)) * t2) + (0.00006 * tcubed))) };
    const double coseta{ cos(eta) };
    const double sineta{ sin(eta) };
    const double Pi{ (ACoord::dms2rad(0, 0, (174.876384 * 3600) + (3289.4789 * T) + (0.60622 * T2) - ((869.8089 + (0.50491 * T)) * t) + (0.03536 * t2))) };
    const double sinpiminusLambda{ sin(Pi - Lambda) };
    const double p{ (ACoord::dms2rad(0, 0, ((5029.0966 + (2.22226 * T) - (0.000042 * T2)) * t) + ((1.11113 - (0.000042 * T)) * t2) - (0.000006 * tcubed))) };
    const double A{ (coseta * cosBeta * sinpiminusLambda) - (sineta * sinBeta) };
    const double B{ cosBeta * cos(Pi - Lambda) };
    const double C{ (coseta * sinBeta) + (sineta * cosBeta * sinpiminusLambda) };
    value.lon = ACoord::rangezero2tau(p + Pi - atan2(A, B));
    value.lat = asin(C);
    return value;  // radians
}

// ----------------------------
//  APrecession_Model::VONDRAK
// ----------------------------
// Precession from: https://www.aanda.org/articles/aa/full_html/2011/10/aa17274-11/aa17274-11.html
// (also implement the use of the precession model selection in generic functions)
APrecesionAngles AEarth::PrecessionAnglesVondrak(const double jd_tt) {
    // Source: Stellarium 24.1, modified.
    APrecesionAngles retval{};
    double T = (jd_tt - JD_2000) * (1.0 / JD_CENTURY);
    double T2pi = T * tau;  // Julian centuries from J2000.0, premultiplied by 2Pi
    for (const auto& p : Vodrak_prec_table) {
        double invP = p.inv_Pn;
        double sin2piT_P, cos2piT_P;
        double phase = T2pi * invP;
        sin2piT_P = sin(phase);
        cos2piT_P = cos(phase);
        retval.psi_A   += p.psi_Cn   * cos2piT_P + p.psi_Sn   * sin2piT_P;
        retval.omega_A += p.omega_Cn * cos2piT_P + p.omega_Sn * sin2piT_P;
        retval.chi_A   += p.chi_Cn   * cos2piT_P + p.chi_Sn   * sin2piT_P;
    }
    retval.psi_A   += (((289.e-9 * T - 0.00740913) * T + 5042.7980307) * T + 8473.343527) * (deg2rad / 3600.0);
    retval.omega_A += (((151.e-9 * T + 0.00000146) * T - 0.4436568) * T + 84283.175915) * (deg2rad / 3600.0);
    retval.chi_A   += (((-61.e-9 * T + 0.00001472) * T + 0.0790159) * T - 19.657270) * (deg2rad / 3600.0);
    //retval.epsilon_A = PrecessionVondrak_epsilon(jd_tt);
    return retval;  // angles in radians
}
LLD AEarth::PrecessionVondrak_QP(const double jd_tt) {
    // Source: Stellarium 24.1 - modified to match the rest of Eartharium
    // PA,QA are the precession part of the ecliptic pole at jde, in J2000.0 ecliptic coordinates (x, -y), see https://www.aanda.org/articles/aa/pdf/2003/48/aa4068.pdf page 7
    // valid +/- 200000 years from J2000.0
    LLD PAQA{};
    double T = (jd_tt - JD_2000) * (1.0 / JD_CENTURY);
    double T2pi = T * tau;   // Julian centuries from J2000.0, premultiplied by 2Pi
    double sin2piT_P, cos2piT_P, phase;
    for (auto& p : Vondrak_PQ_table) {
        //double invP = p.inv_Pn;  // Table value is 1/Pn, so can be multiplied instead of dividing
        phase = T2pi * p.inv_Pn;
        sin2piT_P = sin(phase);
        cos2piT_P = cos(phase);
        PAQA.lon += p.A1_Cn * cos2piT_P + p.A1_Sn * sin2piT_P;
        PAQA.lat += p.A2_Cn * cos2piT_P + p.A2_Sn * sin2piT_P;
    }
    // Now the polynomial terms in T. Horner's scheme is best again.
    PAQA.lon += (( 0.000000101 * T - 0.00028913) * T - 0.1189000) * T + 5851.607687;
    PAQA.lat += ((-0.000000437 * T - 0.00000020) * T + 1.1689818) * T - 1600.886300;
    PAQA *= deg2rad / 3600.0;  // arcsec to rad
    //PAQA *= 4.848136811095359935899141e-6;  // arcsec to rad
    return PAQA;  // In radians
}
LLD AEarth::PrecessionVondrak_XY(const double jd_tt) {
    // XA,YA are the precession part of the equatorial pole (actually CIP = celestial intermediate pole) at jde
    // valid +/- 200000 years from J2000.0
    LLD XAYA{};
    double T = (jd_tt - JD_2000) * (1.0 / JD_CENTURY);
    double T2pi = T * tau;   // Julian centuries from J2000.0, premultiplied by 2Pi
    for (const auto& p : Vondrak_XY_table) {
        double invP = p.inv_Pn;
        double sin2piT_P, cos2piT_P;
        double phase = T2pi * invP;
        sin2piT_P = sin(phase);
        cos2piT_P = cos(phase);
        XAYA.lon += p.A1_Cn * cos2piT_P + p.A1_Sn * sin2piT_P;
        XAYA.lat += p.A2_Cn * cos2piT_P + p.A2_Sn * sin2piT_P;
    }
    // Now the polynomial terms in T. Horner's scheme is best again.
    XAYA.lon += ((-152.e-9 * T - 0.00037173) * T + 0.4252841) * T + 5453.282155;
    XAYA.lat += ((+231.e-9 * T - 0.00018725) * T - 0.7675452) * T - 73750.930350;
    XAYA *= deg2rad / 3600.0;  // arcsec to rad
    return XAYA;  // In radians
}
double AEarth::PrecessionVondrak_epsilon(double jd_tt) {
    double epsilon{ 0.0 };
    double T = (jd_tt - JD_2000) * (1.0 / JD_CENTURY);
    double T2pi = T * tau;  // Julian centuries from J2000.0, premultiplied by 2Pi
    for (const auto& p : Vondrak_epsilon_table) {
        double invP = p.inv_Pn;
        double sin2piT_P, cos2piT_P;
        double phase = T2pi * invP;
        sin2piT_P = sin(phase);
        cos2piT_P = cos(phase);
        //p_A     += p.A1_Cn * cos2piT_P + p.A1_Sn * sin2piT_P;   // pA not needed at the moment
        epsilon += p.A2_Cn * cos2piT_P + p.A2_Sn * sin2piT_P;
    }
    epsilon += ((110.e-9 * T - 0.00004039) * T + 0.3624445) * T + 84028.206305;
    return epsilon * deg2rad / 3600.0;
}
glm::dvec3 AEarth::EclipticPoleVondrak(double jd_tt) {
    glm::dvec3 retval{ 0.0 };
    const double epsilon0{ deg2rad * 23.4392803055555555555556 };  // obliquity at J2000.0
    // returns the ecliptic pole vector referenced to the mean equator and equinox of J2000.0
    // Source: https://www.aanda.org/articles/aa/pdf/2011/10/aa17274-11.pdf A.1
    LLD PAQA = PrecessionVondrak_QP(jd_tt);
    const double z = sqrt(std::max(1.0 - PAQA.lon * PAQA.lon - PAQA.lat * PAQA.lat, 0.0));
    const double s = sin(epsilon0);
    const double c = cos(epsilon0);
    retval.x = PAQA.lon;
    retval.y = -PAQA.lat * c - z * s;
    retval.z = -PAQA.lat * s + z * c;
    return retval;
}
glm::dvec3 AEarth::EquatorialPoleVondrak(double jd_tt) {
    // returns the equatorial pole vector referenced to the mean equator and equinox of J2000.0
    // See https://www.aanda.org/articles/aa/pdf/2011/10/aa17274-11.pdf A.2
    glm::dvec3 retval;
    LLD XAYA = PrecessionVondrak_XY(jd_tt);
    retval.x = XAYA.lon;
    retval.y = XAYA.lat;
    const double w = retval.x * retval.x + retval.y * retval.y;
    retval.z = (w < 1) ? sqrt(1.0 - w) : 0.0;
    return retval;
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
    double retval = ACoord::dms2deg(23, 26, 21.448)  // Equation 22.3
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
LLD AEarth::EclipticNutation(double jd_tt) {
    // MEEUS98: Chapter 22
    // Returns nutation in longitude and obliquity
    LLD ecnut{ 0.0, 0.0, 0.0 };
    const double T = (jd_tt - JD_2000) / JD_CENTURY;
    const double T2 = T * T;
    const double T3 = T2 * T;
    // Moon's Argument of Latitude
    double F = ACoord::rangezero2threesixty(93.27191 + (483202.017538 * T) - (0.0036825 * T2) + (T3 / 327270));
    // Mean elongation of the Moon from the Sun
    double D = ACoord::rangezero2threesixty(297.85036 + (445267.111480 * T) - (0.0019142 * T2) + (T3 / 189474));
    // Mean anomaly of the Sun (relative to Earth)
    double M = ACoord::rangezero2threesixty(357.52772 + (35999.050340 * T) - (0.0001603 * T2) - (T3 / 300000));
    // Mean anomaly of the Moon (relative to Earth)
    double Mprime = ACoord::rangezero2threesixty(134.96298 + (477198.867398 * T) + (0.0086972 * T2) + (T3 / 56250));
    // Longitude of the ascending node of the Moon's mean orbit on the ecliptic, measured from the mean equinox of the date
    double omega = ACoord::rangezero2threesixty(125.04452 - (1934.136261 * T) + (0.0020708 * T2) + (T3 / 450000));
    // Calculate values in arc seconds
    for (const auto& coeff : g_NutationCoefficients) {
        const double argument = deg2rad * ((coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega));
        ecnut.lon += (coeff.sincoeff1 + (coeff.sincoeff2 * T)) * sin(argument) * 0.0001;  // !!! FIX: Why not divide by 36'000'000 below instead?
        ecnut.lat += (coeff.coscoeff1 + (coeff.coscoeff2 * T)) * cos(argument) * 0.0001;
    } 
    ecnut.lat *= deg2rad;
    ecnut.lon *= deg2rad;
    ecnut.lat /= 3600.0; // Nutation in Obliquity
    ecnut.lon /= 3600.0; // Nutation in Longitude
    return ecnut;
}

double AEarth::NutationInRightAscension(double ra, double dec, double obliq, double nut_lon, double nut_obl) {
    return ((cos(obliq) + (sin(obliq) * sin(ra) * tan(dec))) * nut_lon) - (cos(ra) * tan(dec) * nut_obl);
    // All the deg2rad and rad2deg and multiply or divide by 3600.0 seem to cancel out.
}
double AEarth::NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl) {
    return (sin(obliq) * cos(ra) * nut_lon) + (sin(ra) * nut_obl); // 
    // All the deg2rad and rad2deg and multiply or divide by 3600.0 seem to cancel out.
}
LLD AEarth::EquatorialNutation(LLD decra, double obliq, LLD ecnut) {
    // Converts nutation in longitude and obliquity to nutaion in rightascension and declination
    LLD eqnut{ 0.0, 0.0, 0.0 };
    eqnut.lat = (sin(obliq) * cos(decra.lon) * ecnut.lon) + (sin(decra.lon) * ecnut.lat);
    eqnut.lon = ((cos(obliq) + (sin(obliq) * sin(decra.lon) * tan(decra.lat))) * ecnut.lon) - (cos(decra.lon) * tan(decra.lat) * ecnut.lat);
    return eqnut;
}


// time
double AEarth::EquationOfTime(double jd_tt, Planetary_Ephemeris eph) noexcept {
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

    const LLD Equatorial{ Spherical::Ecliptic2Equatorial2(SunLong, SunLat, epsilon) }; // radians

    double E{ L0 - 0.0057183 - (rad2deg * Equatorial.lon) + (rad2deg * AEarth::NutationInLongitude(jd_tt)) * cos(epsilon) }; // degrees Eqn 28.1

    if (E > 180.0) E = -(360.0 - E);
    E *= 4.0; //Convert to minutes of time
    return E;
}

//Sidereal
//double AEarth::MeanGreenwichSiderealTime(double jd_utc) noexcept {
//    // MEEUS98: Chapter 12
//    // AA+: CAASidereal::MeanGreenwichSiderealTime(JD)
//    //Get the Julian day for the same day at midnight
//    double jd_fraction = jd_utc - std::floor(jd_utc) + 0.5; //also good for negative jd
//    double jd_midnight = jd_utc - jd_fraction;
//    //Calculate the sidereal time at midnight
//    double T{ (jd_midnight - JD_2000) / JD_CENTURY }; // Equation 12.1
//    double TSquared{ T * T };
//    double TCubed{ TSquared * T };
//    double Value{ 100.46061837 + (36000.770053608 * T) + (0.000387933 * TSquared) - (TCubed / 38710000) }; // Equation 12.3
//    // Account for the fraction of day. 1.00273790935 = mean_solar_day / mean_sidereal_day = 24h0m0s / 23h56m4.09..s
//    Value += 24.0 * 15.0 * jd_fraction * 1.00273790935; // Sidereal time in degrees
//    return ACoord::rangezero2tau(deg2rad * Value); // mean sidereal time in radians
//}
double AEarth::MeanGreenwichSiderealTime(double jd_utc) noexcept {
    // MEEUS98: Chapter 12
    // MDO - Alternative eqn 12.4 from MEEUS98, rather than eqn 12.3
    //       Seems to give the same results, so keeping it for now as it is slightly faster
    double T{ (jd_utc - JD_2000) / JD_CENTURY }; // Equation 12.1
    // MEEUS98 Equation 12.4
    double Value{ 280.460'618'37 + (360.985'647'366'29 * (jd_utc - JD_2000)) + (0.000'387'933 * T * T) - ((T * T * T) / 38'710'000) };
    return deg2rad * ACoord::rangezero2threesixty(Value); // mean sidereal time in radians
}
double AEarth::ApparentGreenwichSiderealTime(double jd_utc) noexcept {
    // MEEUS98: Chapter 12
    // AA+: CAASidereal::ApparentGreenwichSiderealTime(JD)
    double meangsid = MeanGreenwichSiderealTime(jd_utc);
    // NOTE: The Obliquity and Nutation should be calculated using jd_tt, but Meeus mentions on page 88 (2nd ed)
    //       that this small difference can be ignored.
    // double jd_tt = jd_utc;
    // Otherwise, use EDateTime::JDUTC2TT() to convert:
    double jd_tt = EDateTime::getJDUTC2TT(jd_utc);  // expensive due to DeltaT
    // Or better, pass both but with jd_tt as optional, but that is difficult with caching in Astronomy
    double trueobliquity = MeanObliquityOfEcliptic(jd_tt) + NutationInObliquity(jd_tt);  // radians
    double nutationinlongitude = NutationInLongitude(jd_tt);                             // radians
    // NOTE: If this is ever changed, also make changes to Astronomy::update()
    double gsidtime = meangsid + (nutationinlongitude * cos(trueobliquity));
    // Validated with https://aa.usno.navy.mil/data/siderealtime
    return gsidtime;  // apparent sidereal time in radians
}

glm::dvec3 AEarth::EarthVelocity(double jd_tt, Planetary_Ephemeris eph) noexcept
{
    glm::dvec3 velocity{ 0.0, 0.0, 0.0 };

    if (eph == EPH_VSOP87_FULL)
    {
        velocity.x = VSOP87A_dX(jd_tt);
        velocity.y = VSOP87A_dY(jd_tt);
        velocity.z = VSOP87A_dZ(jd_tt);
        velocity = FK5::VSOP2FK5_J2000(velocity);
        velocity *= 100'000'000; // scale by 10e8 to match low precision calculation below
        return velocity;
    }

    // Vondrak-Ron angle argument components - Much shorter than VSOP87 but pretty accurate
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double L2{    3.1761467 + (1021.3285546 * T) };  // mean longitude of Venus referred to mean equinox of J2000.0
    const double L3{    1.7534703 + ( 628.3075849 * T) };  // mean longitude of Earth
    const double L4{    6.2034809 + ( 334.0612431 * T) };  // mean longitude of Mars
    const double L5{    0.5995465 + (  52.9690965 * T) };  // mean longitude of Jupiter
    const double L6{    0.8740168 + (  21.3299095 * T) };  // mean longitude of Saturn
    const double L7{    5.4812939 + (   7.4781599 * T) };  // mean longitude of Uranus
    const double L8{    5.3118863 + (   3.8133036 * T) };  // mean longitude of Neptune
    const double Ldash{ 3.8103444 + (8399.6847337 * T) };  // mean longitude of Moon
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

LLD AEarth::EquatorialAberration(double Alpha, double Delta, double jd_tt, Planetary_Ephemeris eph) noexcept {
    // Vondrak-Ron aberration - MEEUS98 chapter 23
    // See https://articles.adsabs.harvard.edu/cgi-bin/nph-iarticle_query?1986BAICz..37...96R&defaultprint=YES&filetype=.pdf
    // or https://www.researchgate.net/publication/234449943_Expansion_of_annual_aberration_into_trigonometric_series
    LLD aberration;
    const double cosAlpha{ cos(Alpha) };
    const double sinAlpha{ sin(Alpha) };
    const double cosDelta{ cos(Delta) };
    const double sinDelta{ sin(Delta) };
    const glm::dvec3 velocity{ EarthVelocity(jd_tt, eph) };
    aberration.lon = ((velocity.y * cosAlpha) - (velocity.x * sinAlpha)) / (17'314'463'350.0 * cosDelta);
    aberration.lat = -(((((velocity.x * cosAlpha) + (velocity.y * sinAlpha)) * sinDelta) - (velocity.z * cosDelta)) / 17'314'463'350.0);
    return aberration; // (dRA, dDec, 0.0) in radians
}

LLD AEarth::EclipticAberration(const double Lambda, const double Beta, const double jd_tt, const Planetary_Ephemeris eph) noexcept {
    LLD aberration;
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double Tsquared{ T * T };
    const double e{ 0.016708634 - (0.000042037 * T) - (0.0000001267 * Tsquared) };
    const double Pi{ deg2rad * (102.93735 + (1.71946 * T) + (0.00046 * Tsquared)) };
    const double k{ 20.49552 };
    const double SunLongitude{ ASun::GeometricEclipticLongitude(jd_tt, eph) }; // radians
    aberration.lon = deg2rad * ((-k * cos(SunLongitude - Lambda) + e * k * cos(Pi - Lambda)) / cos(Beta)) / 3600;
    aberration.lat = deg2rad * (-k * sin(Beta) * (sin(SunLongitude - Lambda) - e * sin(Pi - Lambda))) / 3600;
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

double AEarth::RefractionFromApparent(double Altitude, double Pressure, double Temperature) noexcept {
    // Bennett refraction formula
    // Takes elevation in radians, temperature in Celcius (centigrade), pressure in hPa (mbar)
    // Returns refraction in radians
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

// NOTE: Below can easily be generalized to any 2-axis ellipsoid - also for use with other planets
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
double AEarth::RhoSinPhiPrime(double GeographicalLatitude, double Height) noexcept {
    // used for diurnal parallaxes, eclipses and occultations where a precise observer location is essential
    // height in meters, latitude in radians
    const double U{ atan(0.99664719 * tan(GeographicalLatitude)) };
    return (0.99664719 * sin(U)) + (Height / 6378140.0 * sin(GeographicalLatitude));
}
double AEarth::RhoCosPhiPrime(double GeographicalLatitude, double Height) noexcept {
    // used for diurnal parallaxes, eclipses and occultations where a precise observer location is essential
    // height in meters, latitude in radians
    const double U{ atan(0.99664719 * tan(GeographicalLatitude)) };
    return cos(U) + (Height / 6378140.0 * cos(GeographicalLatitude));
}
double AEarth::RadiusOfParallelOfLatitude(double GeographicalLatitude) noexcept {
    // returns radius in km
    const double sinGeo{ sin(GeographicalLatitude) };
    return (6378.140 * cos(GeographicalLatitude)) / (sqrt(1 - (0.0066943847614084 * sinGeo * sinGeo))); // 0.0066943847614084 = e^2 = 2f-f^2
}
double AEarth::RadiusOfCurvature(double GeographicalLatitude) noexcept {
    // returns radius in km
    const double sinGeo{ sin(GeographicalLatitude) };
    return (6378.140 * (1 - 0.0066943847614084)) / pow((1 - (0.0066943847614084 * sinGeo * sinGeo)), 1.5);
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
    constexpr double f{ 0.0033528131778969144060323814696721 };  // 1/298.257
    return D * (1 + (f * Hprime * sinF * sinF * cosG * cosG) - (f * Hprime2 * cosF * cosF * sinG * sinG));
}
double AEarth::GeographicLatitude2rho(double geolat) {
    // get rho (distance as fraction of semi_major axis, from center of Earth to sea level at observer geographic latitude)
    // MEEUS98 - Chapter 11 p.83
    // To get km, multiply by 6378.140 (IAU76 semimajor axis)
    return 0.998'3271 + 0.001'6764 * cos(2 * geolat) - 0.000'0035 * cos(4 * geolat);
}
LLD AEarth::Geocentric2Geographic(LLD geocentric) {
    LLD geographic{};
    // IAU76 ellipsoid parameters
    double major_axis = 6378.140;
    double flattening = 1.0 / 298.257;
    double minor_axis = major_axis * (1 - flattening);
    // MEEUS98 - Chapter 11
    // phi is geographic latitude
    // phiprime is geocentric latitude
    // At sea level:
    // tan phiprime = (b ^ 2 / a ^ 2) * tan phi
    // tan phi = (a ^ 2 / b ^ 2) * tan phiprime
    geographic.lat = atan2(major_axis * major_axis * tan(geocentric.lat), minor_axis * minor_axis);
    geographic.lon = geocentric.lon;
    // !!! FIX: Include distance from center of Earth?
    return geographic;
}
LLD AEarth::Geographic2Geocentric(LLD geographic) {
    LLD geocentric{};
    // IAU76 ellipsoid parameters
    double major_axis = 6378.140;
    double flattening = 1.0 / 298.257;
    double minor_axis = major_axis * (1 - flattening);
    // MEEUS98 - Chapter 11
    // phi is geographic latitude
    // phiprime is geocentric latitude
    // At sea level:
    // tan phiprime = (b ^ 2 / a ^ 2) * tan phi
    geocentric.lat = atan2(minor_axis * minor_axis * tan(geographic.lat), major_axis * major_axis);
    geocentric.lon = geographic.lon;
    // !!! FIX: Include distance from center of Earth?
    return geocentric;
}



// Parallax - Meeus98 chapter 40

// !!! FIX: Decide where to put this, and how to define it
// MEEUS98 value
//constexpr double g_AAParallax_C1 = 4.2634515103856459e-05;  //sin(ACoord::dms2rad(0, 0, 8.794));
// IAU77 value (changed in AA+ v2.52)
constexpr double g_AAParallax_C1 = 4.2635232628103847e-05;  //sin(ACoord::dms2rad(0, 0, 8.794148));

double AEarth::DistanceToParallax(double Distance) noexcept {
    // Distance in Astronomical Units (AU) -> Parallax angle in radians
    return asin(g_AAParallax_C1 / Distance);
}
double AEarth::ParallaxToDistance(double Parallax) noexcept {
    // Parallax angle in radians -> Distance in Astronomical Units (AU)
    return g_AAParallax_C1 / sin(Parallax);
}

LLD AEarth::Equatorial2TopocentricDelta(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double jd_utc) noexcept {
    const double rhoSinPhiPrime{ RhoSinPhiPrime(Latitude, Height) };
    const double rhoCosPhiPrime{ RhoCosPhiPrime(Latitude, Height) };

    //Calculate the Sidereal time
    const double theta{ ApparentGreenwichSiderealTime(jd_utc) };

    const double cosDelta{ cos(Delta) };

    //Calculate the Parallax
    const double Pi{ asin(g_AAParallax_C1 / Distance) };

    //Calculate the hour angle
    // Use this if west longitudes are positive:
    //const double H{ theta - Longitude - Alpha };  // already in radians
    // East longitudes are positive:
    const double H{ theta + Longitude - Alpha };  // already in radians
    const double cosH{ cos(H) };
    const double sinH{ sin(H) };

    LLD DeltaTopocentric;
    DeltaTopocentric.lon = -Pi * rhoCosPhiPrime * sinH / cosDelta;
    DeltaTopocentric.lat = -Pi * ((rhoSinPhiPrime * cosDelta) - (rhoCosPhiPrime * cosH * sin(Delta)));
    return DeltaTopocentric;  // difference between Spherical Geocentric Equatorial and Spherical Topocentric Equatorial in radians
}

LLD AEarth::Equatorial2Topocentric2(double Alpha, double Delta, double Distance, double Longitude, double Latitude, double Height, double jd_utc) noexcept {
    // Alpha,Delta is geocentric equatorial coordinates
    // Longitude,Latitude, height is observer location
    // jd_utc is UTC date time in JD (only used for AGST calculation, perhaps pass AGST directly instead)
    LLD Topocentric;
    const double rhoSinPhiPrime{ RhoSinPhiPrime(Latitude, Height) };
    const double rhoCosPhiPrime{ RhoCosPhiPrime(Latitude, Height) };
    //Calculate the Sidereal time
    // !!! FIX: Better to pass AGST in instead of jd_utc, it is likely to be available already,
    // or can be calculated by caller at no additional cost
    const double theta{ ApparentGreenwichSiderealTime(jd_utc) };
    const double cosDelta{ cos(Delta) };
    const double sinpi{ g_AAParallax_C1 / Distance };  // Parallax
    // If west longitudes are considered positive, use this:
    // const double H{ theta - Longitude - Alpha };  // Hour angle
    // East longitudes are considered positive:
    const double H{ theta + Longitude - Alpha };  // Hour angle
    const double cosH{ cos(H) };
    const double sinH{ sin(H) };
    const double DeltaAlpha{ atan2(-rhoCosPhiPrime * sinpi * sinH, cosDelta - (rhoCosPhiPrime * sinpi * cosH)) };
    Topocentric.lon = ACoord::rangezero2tau(Alpha + DeltaAlpha);
    Topocentric.lat = (atan2((sin(Delta) - (rhoSinPhiPrime * sinpi)) * cos(DeltaAlpha), cosDelta - (rhoCosPhiPrime * sinpi * cosH)));
    return Topocentric;  // Spherical Topocentric Equatorial coordinates in radians
}
LLD AEarth::Ecliptic2Topocentric2(double Lambda, double Beta, double Semidiameter, double Distance, double Epsilon, double Latitude, double Height, double jd_utc) noexcept {
    // Lambda,Beta is geocentric ecliptic coordinates
    // Semidiameter, Distance is objects semidiameter in radians and distance in AU
    // Epsilon is (True?) Obliquity of Ecliptic
    // Latitude, Height of observer location
    // note the .dst value in the returned LLD holds the semidiameter of the object in radians
    LLD Topocentric;
    const double S{ RhoSinPhiPrime(Latitude, Height) };
    const double C{ RhoCosPhiPrime(Latitude, Height) };
    const double sine{ sin(Epsilon) };
    const double cose{ cos(Epsilon) };
    const double cosBeta{ cos(Beta) };
    const double sinBeta{ sin(Beta) };
    double theta{ ApparentGreenwichSiderealTime(jd_utc) };  // Just pass in AGST instead of jd_utc
    const double sintheta{ sin(theta) };
    const double sinpi{ g_AAParallax_C1 / Distance };  // Parallax
    const double N{ (cos(Lambda) * cosBeta) - (C * sinpi * cos(theta)) };
    Topocentric.lon = atan2((sin(Lambda) * cosBeta) - (sinpi * ((S * sine) + (C * cose * sintheta))), N);
    const double cosTopocentricLambda{ cos(Topocentric.lon) };
    Topocentric.lat = atan(cosTopocentricLambda * (sinBeta - (sinpi * ((S * cose) - (C * sine * sintheta)))) / N);
    Topocentric.dst = asin(cosTopocentricLambda * cos(Topocentric.lat) * sin(Semidiameter) / N);
    Topocentric.lon = ACoord::rangezero2tau(Topocentric.lon);
    return Topocentric;  // Spherical Topocentric Ecliptic coordinates in radians
}
// MDO
LLD AEarth::Equatorial2Topocentric(LLD decradst, LLD latlonhgt, double agst) noexcept {
    // NOTE: Now takes AGST instead of JD_UTC !
    // decradst is geocentric equatorial coordinates incl. distance in AU
    // latlonhgt is observer location, incl height above MSL
    // agst is Apparent Greenwich Sidereal Time
    LLD Topocentric;
    const double rhoSinPhiPrime{ RhoSinPhiPrime(latlonhgt.lat, latlonhgt.dst) };
    const double rhoCosPhiPrime{ RhoCosPhiPrime(latlonhgt.lat, latlonhgt.dst) };
    const double cosDelta{ cos(decradst.lat) };
    const double sinpi{ g_AAParallax_C1 / decradst.dst };   // Parallax from distance in AU
    // If west longitudes are considered positive, use this:
    // const double H{ agst - latlonhgt.lon - decradst.lon };  // Hour angle
    // East longitudes are considered positive:
    const double H{ agst + latlonhgt.lon - decradst.lon };  // Hour angle
    //std::cout << "agst=" << agst << " lon=" << latlonhgt.lon << " RA=" << decradst.lon << " H=" << H << "\n";
    const double cosH{ cos(H) };
    const double sinH{ sin(H) };
    const double DeltaAlpha{ atan2(-(rhoCosPhiPrime * sinpi * sinH), cosDelta - (rhoCosPhiPrime * sinpi * cosH)) };
    Topocentric.lon = ACoord::rangezero2tau(decradst.lon + DeltaAlpha);
    Topocentric.lat = (atan2((sin(decradst.lat) - (rhoSinPhiPrime * sinpi)) * cos(DeltaAlpha), cosDelta - (rhoCosPhiPrime * sinpi * cosH)));
    Topocentric.dst = decradst.dst;
    return Topocentric;  // Spherical Topocentric Equatorial coordinates in radians
}
LLD AEarth::Ecliptic2Topocentric(LLD latlondst, double Semidiameter, double Latitude, double Height, double Epsilon, double agst) noexcept {
    // Object: latlondst is geocentric ecliptic coordinates, Semidiameter is object's semidiameter in radians and distance in AU
    // Observer: Latitude, Height of observer location in radians & meters
    // Epsilon is (True?) Obliquity of Ecliptic, agst is Apparent Greemwich Sidereal Time
    // NOTE: the .dst value in the returned LLD holds the semidiameter of the object in radians
    LLD Topocentric;
    const double S{ RhoSinPhiPrime(Latitude, Height) };
    const double C{ RhoCosPhiPrime(Latitude, Height) };
    const double sine{ sin(Epsilon) };
    const double cose{ cos(Epsilon) };
    const double cosBeta{ cos(latlondst.lat) };
    const double sinBeta{ sin(latlondst.lat) };
    const double sintheta{ sin(agst) };
    const double sinpi{ g_AAParallax_C1 / latlondst.dst };  // Parallax
    const double N{ (cos(latlondst.lon) * cosBeta) - (C * sinpi * cos(agst)) };
    Topocentric.lon = atan2((sin(latlondst.lon) * cosBeta) - (sinpi * ((S * sine) + (C * cose * sintheta))), N);
    const double cosTopocentricLambda{ cos(Topocentric.lon) };
    Topocentric.lat = atan(cosTopocentricLambda * (sinBeta - (sinpi * ((S * cose) - (C * sine * sintheta)))) / N);
    Topocentric.dst = asin(cosTopocentricLambda * cos(Topocentric.lat) * sin(Semidiameter) / N);
    Topocentric.lon = ACoord::rangezero2tau(Topocentric.lon);
    return Topocentric;  // Spherical Topocentric Ecliptic coordinates in radians
}

// Equinoxes and Solstices

double AEarth::NorthwardEquinox(long year, Planetary_Ephemeris eph) noexcept {
    double jde{ 0 };
    if (year <= 1000) {
        const double Y{ year / 1000.0 };
        jde = 1721139.29189 + Y * (365242.13740 + Y * (0.06134 + Y * (0.00111 - (0.00071 * Y))));
    } else {
        const double Y{ (year - 2000.0) / 1000.0 };
        jde = 2'451'623.809'84 + Y * (365'242.374'04 + Y * (0.051'69 - Y * (0.004'11 + (0.000'57 * Y))));
    }
    double Correction{ 0 };
    do {
        const double SunLongitude{ ASun::ApparentEclipticLongitude(jde, eph) };
        Correction = 58.0 * sin(-SunLongitude);
        jde += Correction;
    } while (fabs(Correction) > 0.00001);  // While Correction > 0.864 seconds
    return jde;
}
double AEarth::NorthernSolstice(long year, Planetary_Ephemeris eph) noexcept {
    double jde{ 0 };
    if (year <= 1000) {
        const double Y{ year / 1000.0 };
        jde = 1721233.25401 + Y * (365241.72562 - Y * (0.05323 - Y * (0.00907 + (0.00025 * Y))));
    } else {
        const double Y{ (year - 2000.0) / 1000.0 };
        jde = 2451716.56767 + Y * (365241.62603 + Y * (0.00325 + Y * (0.00888 - (0.00030 * Y))));
    }
    double Correction{ 0 };
    do {
        const double SunLongitude{ ASun::ApparentEclipticLongitude(jde, eph) };
        Correction = 58 * sin(pi2 - SunLongitude);
        jde += Correction;
    } while (fabs(Correction) > 0.00001);  // While Correction > 0.864 seconds
    return jde;
}
double AEarth::SouthwardEquinox(long Year, Planetary_Ephemeris eph) noexcept {
    double jde{ 0 };
    if (Year <= 1000) {
        const double Y{ Year / 1000.0 };
        jde = 1721325.70455 + Y * (365242.49558 - Y * (0.11677 + Y * (0.00297 - (0.00074 * Y))));
    } else {
        const double Y{ (Year - 2000.0) / 1000.0 };
        jde = 2451810.21715 + Y * (365242.01767 - Y * (0.11575 - Y * (0.00337 + (0.00078 * Y))));
    }
    double Correction{ 0 };
    do {
        const double SunLongitude{ ASun::ApparentEclipticLongitude(jde, eph) };
        Correction = 58 * sin(pi - SunLongitude);
        jde += Correction;
    } while (fabs(Correction) > 0.00001);  // While Correction > 0.864 seconds
    return jde;
}
double AEarth::SouthernSolstice(long Year, Planetary_Ephemeris eph) noexcept {
    double jde{ 0 };
    if (Year <= 1000) {
        const double Y{ Year / 1000.0 };
        jde = 1721414.39987 + Y * (365242.88257 - Y * (0.00769 + Y * (0.00933 + (0.00006 * Y))));
    } else {
        const double Y{ (Year - 2000.0) / 1000.0 };
        jde = 2451900.05952 + Y * (365242.74049 - Y * (0.06223 + Y * (0.00823 - (0.00032 * Y))));
    }
    double Correction{ 0 };
    do {
        const double SunLongitude{ ASun::ApparentEclipticLongitude(jde, eph) };
        Correction = 58 * sin(3.0 * pi2 - SunLongitude);
        jde += Correction;
    } while (fabs(Correction) > 0.00001);  // While Correction > 0.864 seconds
    return jde;
}
double AEarth::LengthOfSpring(long Year, Hemisphere hemi, Planetary_Ephemeris eph) noexcept {
    if (hemi == NORTHERN) return NorthernSolstice(Year, eph) - NorthwardEquinox(Year, eph);
    else return SouthernSolstice(Year, eph) - SouthwardEquinox(Year, eph);
}
double AEarth::LengthOfSummer(long Year, Hemisphere hemi, Planetary_Ephemeris eph) noexcept {
    //The Summer season wraps around into the following year for the southern hemisphere
    if (hemi == NORTHERN) return SouthwardEquinox(Year, eph) - NorthernSolstice(Year, eph);
    else return NorthwardEquinox(Year + 1, eph) - SouthernSolstice(Year, eph);
}
double AEarth::LengthOfAutumn(long Year, Hemisphere hemi, Planetary_Ephemeris eph) noexcept {
    if (hemi == NORTHERN) return SouthernSolstice(Year, eph) - SouthwardEquinox(Year, eph);
    else return NorthernSolstice(Year, eph) - NorthwardEquinox(Year, eph);
}
double AEarth::LengthOfWinter(long Year, Hemisphere hemi, Planetary_Ephemeris eph) noexcept {
    //The Winter season wraps around into the following year for the Northern hemisphere
    if (hemi == NORTHERN) return NorthwardEquinox(Year + 1, eph) - SouthernSolstice(Year, eph);
    else return SouthwardEquinox(Year, eph) - NorthernSolstice(Year, eph);
}
double AEarth::TropicalYearLength(double jd_tt) {
    // returns approximate length of the mean tropical year in ephemeris days
    // Source: https://en.wikipedia.org/wiki/Tropical_year
    // See also: https://web.archive.org/web/20190430134555/http://aa.usno.navy.mil/publications/docs/c15_usb_online.pdf
    // According to Wikipedia, the formula is valid 8000BCE to 12000CE
    double retval;
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    retval = 365.242'189'6698 - 6.15359e-6 * T - 7.29e-10 * T * T + 2.64e-10 * T * T * T;
    return retval;
}


// VSOP87
// ======

// VSOP87 Orbital Parameters - Heliocentric Ecliptic at Equinox of J2000.0
double AEarth::VSOP87_A(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_A_EMB.data(), g_VSOP87_A_EMB.size(), false);
}
double AEarth::VSOP87_L(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_L_EMB.data(), g_VSOP87_L_EMB.size(), true);
}
double AEarth::VSOP87_K(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_K_EMB.data(), g_VSOP87_K_EMB.size(), false);
}
double AEarth::VSOP87_H(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_H_EMB.data(), g_VSOP87_H_EMB.size(), false);
}
double AEarth::VSOP87_Q(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_Q_EMB.data(), g_VSOP87_Q_EMB.size(), false);
}
double AEarth::VSOP87_P(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_P_EMB.data(), g_VSOP87_P_EMB.size(), false);
}

// Ephemeris A - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
double AEarth::VSOP87A_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_X_EARTH.data(), g_VSOP87A_X_EARTH.size(), false);
}
double AEarth::VSOP87A_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Y_EARTH.data(), g_VSOP87A_Y_EARTH.size(), false);
}
double AEarth::VSOP87A_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Z_EARTH.data(), g_VSOP87A_Z_EARTH.size(), false);
}
double AEarth::VSOP87A_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_X_EARTH.data(), g_VSOP87A_X_EARTH.size());
}
double AEarth::VSOP87A_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Y_EARTH.data(), g_VSOP87A_Y_EARTH.size());
}
double AEarth::VSOP87A_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Z_EARTH.data(), g_VSOP87A_Z_EARTH.size());
}

// Ephemeris B - Spherical Heliocentric Ecliptic at J2000.0
double AEarth::VSOP87B_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_L_EARTH.data(), g_VSOP87B_L_EARTH.size(), true);
}
double AEarth::VSOP87B_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87B_B_EARTH.data(), g_VSOP87B_B_EARTH.size(), true));
}
double AEarth::VSOP87B_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_R_EARTH.data(), g_VSOP87B_R_EARTH.size(), false);
}
double AEarth::VSOP87B_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_L_EARTH.data(), g_VSOP87B_L_EARTH.size());
}
double AEarth::VSOP87B_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_B_EARTH.data(), g_VSOP87B_B_EARTH.size());
}
double AEarth::VSOP87B_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_R_EARTH.data(), g_VSOP87B_R_EARTH.size());
}

// Ephemeris C - Rectangular Heliocentric Ecliptic at Equinox of Date
double AEarth::VSOP87C_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_X_EARTH.data(), g_VSOP87C_X_EARTH.size(), false);
}
double AEarth::VSOP87C_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Y_EARTH.data(), g_VSOP87C_Y_EARTH.size(), false);
}
double AEarth::VSOP87C_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Z_EARTH.data(), g_VSOP87C_Z_EARTH.size(), false);
}
double AEarth::VSOP87C_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_X_EARTH.data(), g_VSOP87C_X_EARTH.size());
}
double AEarth::VSOP87C_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Y_EARTH.data(), g_VSOP87C_Y_EARTH.size());
}
double AEarth::VSOP87C_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Z_EARTH.data(), g_VSOP87C_Z_EARTH.size());
}

// Ephemeris D - Spherical Heliocentric Ecliptic at Equinox of Date
double AEarth::VSOP87D_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_L_EARTH.data(), g_VSOP87D_L_EARTH.size(), true);
}
double AEarth::VSOP87D_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87D_B_EARTH.data(), g_VSOP87D_B_EARTH.size(), true));
}
double AEarth::VSOP87D_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_R_EARTH.data(), g_VSOP87D_R_EARTH.size(), false);
}
double AEarth::VSOP87D_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_L_EARTH.data(), g_VSOP87D_L_EARTH.size());
}
double AEarth::VSOP87D_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_B_EARTH.data(), g_VSOP87D_B_EARTH.size());
}
double AEarth::VSOP87D_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_R_EARTH.data(), g_VSOP87D_R_EARTH.size());
}

// VSOP87 Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
double AEarth::VSOP87E_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_X_EARTH.data(), g_VSOP87E_X_EARTH.size(), false);
}
double AEarth::VSOP87E_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Y_EARTH.data(), g_VSOP87E_Y_EARTH.size(), false);
}
double AEarth::VSOP87E_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Z_EARTH.data(), g_VSOP87E_Z_EARTH.size(), false);
}
double AEarth::VSOP87E_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_X_EARTH.data(), g_VSOP87E_X_EARTH.size());
}
double AEarth::VSOP87E_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Y_EARTH.data(), g_VSOP87E_Y_EARTH.size());
}
double AEarth::VSOP87E_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Z_EARTH.data(), g_VSOP87E_Z_EARTH.size());
}

