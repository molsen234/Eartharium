
#include "amars.h"
#include "acoordinates.h"
#include "vsop87/amars_vsop87_short.h"
#include "vsop87/amars_vsop87_full.h"
#include "aearth.h"
#include "asun.h"
#include "amoon.h"

double AMars::EclipticLongitude(double jd_tt, Planetary_Ephemeris eph) noexcept {

    if (eph == EPH_VSOP87_FULL) return VSOP87_D_Longitude(jd_tt);

    // VSOP87 D Short
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };
    const double rho5{ rho4 * rho };

    double L0{ 0 };
    for (const auto& L0Coefficient : g_L0MarsCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1{ 0 };
    for (const auto& L1Coefficient : g_L1MarsCoefficients)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2{ 0 };
    for (const auto& L2Coefficient : g_L2MarsCoefficients)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3{ 0 };
    for (const auto& L3Coefficient : g_L3MarsCoefficients)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4{ 0 };
    for (const auto& L4Coefficient : g_L4MarsCoefficients)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5{ 0 };
    for (const auto& L5Coefficient : g_L5MarsCoefficients)
        L5 += (L5Coefficient.A * cos(L5Coefficient.B + (L5Coefficient.C * rho)));

    double value{ (L0 + (L1 * rho) + (L2 * rhosquared) + (L3 * rhocubed) + (L4 * rho4) + (L5 * rho5)) / 100000000 };

    return ACoord::rangezero2tau(value);
}
double AMars::EclipticLatitude(double jd_tt, Planetary_Ephemeris eph) noexcept {

    if (eph == EPH_VSOP87_FULL) return VSOP87_D_Latitude(jd_tt);

    // VSOP87 D Short
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };

    double B0{ 0 };
    for (const auto& B0Coefficient : g_B0MarsCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1{ 0 };
    for (const auto& B1Coefficient : g_B1MarsCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2{ 0 };
    for (const auto& B2Coefficient : g_B2MarsCoefficients)
        B2 += (B2Coefficient.A * cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3{ 0 };
    for (const auto& B3Coefficient : g_B3MarsCoefficients)
        B3 += (B3Coefficient.A * cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4{ 0 };
    for (const auto& B4Coefficient : g_B4MarsCoefficients)
        B4 += (B4Coefficient.A * cos(B4Coefficient.B + (B4Coefficient.C * rho)));

    double value{ (B0 + (B1 * rho) + (B2 * rhosquared) + (B3 * rhocubed) + (B4 * rho4)) / 100000000 };

    return ACoord::rangemhalfpi2halfpi(value);
}
double AMars::EclipticDistance(double jd_tt, Planetary_Ephemeris eph) noexcept {

    if (eph == EPH_VSOP87_FULL) return VSOP87_D_Distance(jd_tt);

    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };

    double R0{ 0 };
    for (const auto& R0Coefficients : g_R0MarsCoefficients)
        R0 += (R0Coefficients.A * std::cos(R0Coefficients.B + (R0Coefficients.C * rho)));
    double R1{ 0 };
    for (const auto& R1Coefficient : g_R1MarsCoefficients)
        R1 += (R1Coefficient.A * std::cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2{ 0 };
    for (const auto& R2Coefficient : g_R2MarsCoefficients)
        R2 += (R2Coefficient.A * std::cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3{ 0 };
    for (const auto& R3Coefficient : g_R3MarsCoefficients)
        R3 += (R3Coefficient.A * std::cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4{ 0 };
    for (const auto& R4Coefficient : g_R4MarsCoefficients)
        R4 += (R4Coefficient.A * std::cos(R4Coefficient.B + (R4Coefficient.C * rho)));

    return (R0 + (R1 * rho) + (R2 * rhosquared) + (R3 * rhocubed) + (R4 * rho4)) / 100000000;
}
LLD AMars::EclipticCoordinates(double jd_tt, Planetary_Ephemeris eph) {
    LLD retval{};
    retval.lat = EclipticLatitude(jd_tt, eph);
    retval.lon = EclipticLongitude(jd_tt, eph);
    retval.dst = EclipticDistance(jd_tt, eph);
    return retval;
}

// VSOP87 Orbital Parameters - Heliocentric Ecliptic at Equinox of J2000.0
double AMars::A(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_A_MARS.data(), g_VSOP87_A_MARS.size(), false);
}
double AMars::L(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_L_MARS.data(), g_VSOP87_L_MARS.size(), true);
}
double AMars::K(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_K_MARS.data(), g_VSOP87_K_MARS.size(), false);
}
double AMars::H(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_H_MARS.data(), g_VSOP87_H_MARS.size(), false);
}
double AMars::Q(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_Q_MARS.data(), g_VSOP87_Q_MARS.size(), false);
}
double AMars::P(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_P_MARS.data(), g_VSOP87_P_MARS.size(), false);
}

// VSOP87 Ephemeris A - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
double AMars::VSOP87_A_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_X_MARS.data(), g_VSOP87A_X_MARS.size(), false);
}
double AMars::VSOP87_A_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Y_MARS.data(), g_VSOP87A_Y_MARS.size(), false);
}
double AMars::VSOP87_A_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Z_MARS.data(), g_VSOP87A_Z_MARS.size(), false);
}
double AMars::VSOP87_A_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_X_MARS.data(), g_VSOP87A_X_MARS.size());
}
double AMars::VSOP87_A_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Y_MARS.data(), g_VSOP87A_Y_MARS.size());
}
double AMars::VSOP87_A_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Z_MARS.data(), g_VSOP87A_Z_MARS.size());
}

// VSOP87 Ephemeris B - Spherical Heliocentric Ecliptic at J2000.0
double AMars::VSOP87_B_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_L_MARS.data(), g_VSOP87B_L_MARS.size(), true);
}
double AMars::VSOP87_B_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87B_B_MARS.data(), g_VSOP87B_B_MARS.size(), true));
}
double AMars::VSOP87_B_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_R_MARS.data(), g_VSOP87B_R_MARS.size(), false);
}
double AMars::VSOP87_B_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_L_MARS.data(), g_VSOP87B_L_MARS.size());
}
double AMars::VSOP87_B_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_B_MARS.data(), g_VSOP87B_B_MARS.size());
}
double AMars::VSOP87_B_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_R_MARS.data(), g_VSOP87B_R_MARS.size());
}

// VSOP87 Ephemeris C - Rectangular Heliocentric Ecliptic at Equinox of Date
double AMars::VSOP87_C_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_X_MARS.data(), g_VSOP87C_X_MARS.size(), false);
}
double AMars::VSOP87_C_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Y_MARS.data(), g_VSOP87C_Y_MARS.size(), false);
}
double AMars::VSOP87_C_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Z_MARS.data(), g_VSOP87C_Z_MARS.size(), false);
}
double AMars::VSOP87_C_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_X_MARS.data(), g_VSOP87C_X_MARS.size());
}
double AMars::VSOP87_C_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Y_MARS.data(), g_VSOP87C_Y_MARS.size());
}
double AMars::VSOP87_C_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Z_MARS.data(), g_VSOP87C_Z_MARS.size());
}

// VSOP87 Ephemeris D - Spherical Heliocentric Ecliptic at Equinox of Date
double AMars::VSOP87_D_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_L_MARS.data(), g_VSOP87D_L_MARS.size(), true);
}
double AMars::VSOP87_D_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87D_B_MARS.data(), g_VSOP87D_B_MARS.size(), true));
}
double AMars::VSOP87_D_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_R_MARS.data(), g_VSOP87D_R_MARS.size(), false);
}
double AMars::VSOP87_D_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_L_MARS.data(), g_VSOP87D_L_MARS.size());
}
double AMars::VSOP87_D_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_B_MARS.data(), g_VSOP87D_B_MARS.size());
}
double AMars::VSOP87_D_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_R_MARS.data(), g_VSOP87D_R_MARS.size());
}

// VSOP87 Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
double AMars::VSOP87_E_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_X_MARS.data(), g_VSOP87E_X_MARS.size(), false);
}
double AMars::VSOP87_E_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Y_MARS.data(), g_VSOP87E_Y_MARS.size(), false);
}
double AMars::VSOP87_E_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Z_MARS.data(), g_VSOP87E_Z_MARS.size(), false);
}
double AMars::VSOP87_E_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_X_MARS.data(), g_VSOP87E_X_MARS.size());
}
double AMars::VSOP87_E_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Y_MARS.data(), g_VSOP87E_Y_MARS.size());
}
double AMars::VSOP87_E_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Z_MARS.data(), g_VSOP87E_Z_MARS.size());
}


// Physical Mars
APhysicalMarsDetails AMars::calcPhysicalMarsDetails(double JD, Planetary_Ephemeris eph) noexcept
{
    APhysicalMarsDetails details;

    //Step 1
    const double T{ (JD - 2451545) / 36525 };
    double Lambda0rad{ deg2rad * (352.9065 + (1.17330 * T))};
    double Beta0rad{ deg2rad * (63.2818 - (0.00394 * T)) };
    const double cosBeta0rad{ cos(Beta0rad) };
    const double sinBeta0rad{ sin(Beta0rad) };

    //Step 2
    const double l0rad{ AEarth::EclipticLongitude(JD, eph) };
    const double b0rad{ AEarth::EclipticLatitude(JD, eph) };
    const double R{ AEarth::EclipticDistance(JD, eph) };

    double PreviousLightTravelTime{ 0 };
    double LightTravelTime{ 0 };
    double x{ 0 };
    double y{ 0 };
    double z{ 0 };
    bool bIterate{ true };
    double DELTA{ 0 };
    double lrad{ 0 };
    double brad{ 0 };
    double r{ 0 };
    while (bIterate)
    {
        const double JD2{ JD - LightTravelTime };

        //Step 3
        lrad = AMars::EclipticLongitude(JD2, eph);
        brad = AMars::EclipticLatitude(JD2, eph);
        const double cosbrad{ cos(brad) };
        r = AMars::EclipticDistance(JD2, eph);

        //Step 4
        x = (r * cosbrad * cos(lrad)) - (R * cos(l0rad));
        y = (r * cosbrad * sin(lrad)) - (R * sin(l0rad));
        z = (r * sin(brad)) - (R * sin(b0rad));
        DELTA = sqrt((x * x) + (y * y) + (z * z));
        LightTravelTime = ACoord::DistanceToLightTime(DELTA);

        //Prepare for the next loop around
        bIterate = (fabs(LightTravelTime - PreviousLightTravelTime) > 2e-6); //2e-6 corresponds to 0.17 of a second
        if (bIterate)
            PreviousLightTravelTime = LightTravelTime;
    }

    //Step 5
    double lambdarad{ atan2(y, x) };
    double betarad{ atan2(z, sqrt((x * x) + (y * y))) };

    //Step 6
    details.DE = asin((-sinBeta0rad * sin(betarad)) - (cosBeta0rad * cos(betarad) * cos(Lambda0rad - lambdarad)));

    //Step 7
    const double Nrad{deg2rad * (49.5581 + (0.7721 * T)) };
    const double ldashrad{ (lrad - deg2rad * (0.00697 / r))};
    const double bdashrad{ (brad - deg2rad * (0.000225 * (cos(lrad - Nrad) / r))) };

    //Step 8
    details.DS = asin((-sinBeta0rad * sin(bdashrad)) - (cosBeta0rad * cos(bdashrad) * cos(Lambda0rad - ldashrad)));

    //Step 9
    const double W{ ACoord::rangezero2tau( deg2rad * (11.504 + (350.89200025 * (JD - LightTravelTime - 2433282.5)))) };

    //Step 10
    double e0rad{ AEarth::MeanObliquityOfEcliptic(JD) };
    const double e0{ rad2deg * e0rad };
    const double cose0rad{ cos(e0rad) };
    const double sine0rad{ sin(e0rad) };
    const LLD PoleEquatorial{ Spherical::Ecliptic2Equatorial2(Lambda0rad, Beta0rad, e0rad) };
    
    //Step 11
    const double u{ (y * cose0rad) - (z * sine0rad) };
    const double v{ (y * sine0rad) + (z * cose0rad) };
    const double alpharad{ atan2(u, x) };
    const double deltarad{ atan2(v, sqrt((x * x) + (u * u))) };
    const double cosdeltarad{ cos(deltarad) };
    const double alpha0radminusalpharad{ PoleEquatorial.lon - alpharad };
    const double xi{ atan2((sin(PoleEquatorial.lat) * cosdeltarad * cos(alpha0radminusalpharad)) - (sin(deltarad) * cos(PoleEquatorial.lat)), cosdeltarad * sin(alpha0radminusalpharad)) };

    //Step 12
    details.w = ACoord::rangezero2tau(W - xi);  // degrees

    //Step 13
    const double NutationInObliquity{ AEarth::NutationInObliquity(JD) };
    const double NutationInLongitude{ AEarth::NutationInLongitude(JD) };

    //Step 14
    const double l0radminuslambdarad{ l0rad - lambdarad };
    lambdarad += deg2rad * (0.005693 * cos(l0radminuslambdarad) / cos(betarad));
    betarad += deg2rad * (0.005693 * sin(l0radminuslambdarad) * sin(betarad));

    //Step 15
    Lambda0rad += NutationInLongitude;
    lambdarad += NutationInLongitude;
    e0rad += NutationInObliquity;

    //Step 16
    const LLD ApparentPoleEquatorial{ Spherical::Ecliptic2Equatorial2(Lambda0rad, Beta0rad, e0rad) };
    const double cosdelta0dash{ cos(ApparentPoleEquatorial.lat) };
    const LLD ApparentMars{ Spherical::Ecliptic2Equatorial2(lambdarad, betarad, e0rad) };
    const double alpha0dashminusalphadash{ ApparentPoleEquatorial.lon - ApparentMars.lon };

    //Step 17
    details.P = ACoord::rangezero2tau(atan2(cosdelta0dash * sin(alpha0dashminusalphadash), sin(ApparentPoleEquatorial.lat) * cos(ApparentMars.lat) - cosdelta0dash * sin(ApparentMars.lat) * cos(alpha0dashminusalphadash)));

    //Step 18
    const double SunLambda{ ASun::GeometricEclipticLongitude(JD, eph) };
    const double SunBeta{ ASun::GeometricEclipticLatitude(JD, eph) };
    const LLD SunEquatorial{ Spherical::Ecliptic2Equatorial2(SunLambda, SunBeta, e0rad) };
    details.X = AMoon::PositionAngle(SunEquatorial.lon, SunEquatorial.lat, alpharad, deltarad);

    //Step 19
    details.d = 9.36 / DELTA;
    details.k = AIllumination::IlluminatedFraction2(r, R, DELTA);  // distances to fraction
    details.q = (1 - details.k) * details.d;
    details.d *= deg2rad / 3600;
    details.q *= deg2rad / 3600;

    return details;
}

// Illumination
double AMars::MagnitudeAA(double r, double Delta, double i) noexcept {
    // r = Mars to Sun distance in AU
    // Delta = Mars to Earth distance in AU
    // i = Mars phase angle in radians
    return -1.52 + (5 * log10(r * Delta)) + (0.016 * i);
}
double AMars::MagnitudeMuller(double r, double Delta, double i) noexcept {
    // r = Mars to Sun distance in AU
    // Delta = Mars to Earth distance in AU
    // i = Mars phase angle in radians
    i *= rad2deg;
    return -1.3 + (5 * log10(r * Delta)) + (0.01486 * i);
}
