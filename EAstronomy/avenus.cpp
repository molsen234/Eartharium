
#include "avenus.h"
#include "acoordinates.h"
#include "vsop87/avenus_vsop87_short.h"
#include "vsop87/avenus_vsop87_full.h"

double AVenus::EclipticLongitude(double jd_tt, Planetary_Ephemeris eph) noexcept {

    if (eph == EPH_VSOP87_FULL) return VSOP87_D_Longitude(jd_tt);

    // VSOP87 D Short
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };
    const double rho5{ rho4 * rho };

    double L0{ 0 };
    for (const auto& L0Coefficient : g_L0VenusCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1{ 0 };
    for (const auto& L1Coefficient : g_L1VenusCoefficients)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2{ 0 };
    for (const auto& L2Coefficient : g_L2VenusCoefficients)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3{ 0 };
    for (const auto& L3Coefficient : g_L3VenusCoefficients)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4{ 0 };
    for (const auto& L4Coefficient : g_L4VenusCoefficients)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5{ 0 };
    for (const auto& L5Coefficient : g_L5VenusCoefficients)
        L5 += (L5Coefficient.A * cos(L5Coefficient.B + (L5Coefficient.C * rho)));

    double value{ (L0 + (L1 * rho) + (L2 * rhosquared) + (L3 * rhocubed) + (L4 * rho4) + (L5 * rho5)) / 100000000 };

    return ACoord::rangezero2tau(value);
}
double AVenus::EclipticLatitude(double jd_tt, Planetary_Ephemeris eph) noexcept {

    if (eph == EPH_VSOP87_FULL) return VSOP87_D_Latitude(jd_tt);

    // VSOP87 D Short
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };

    double B0{ 0 };
    for (const auto& B0Coefficient : g_B0VenusCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1{ 0 };
    for (const auto& B1Coefficient : g_B1VenusCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2{ 0 };
    for (const auto& B2Coefficient : g_B2VenusCoefficients)
        B2 += (B2Coefficient.A * cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3{ 0 };
    for (const auto& B3Coefficient : g_B3VenusCoefficients)
        B3 += (B3Coefficient.A * cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4{ 0 };
    for (const auto& B4Coefficient : g_B4VenusCoefficients)
        B4 += (B4Coefficient.A * cos(B4Coefficient.B + (B4Coefficient.C * rho)));

    double value{ (B0 + (B1 * rho) + (B2 * rhosquared) + (B3 * rhocubed) + (B4 * rho4)) / 100000000 };

    return ACoord::rangemhalfpi2halfpi(value);
}
double AVenus::EclipticDistance(double jd_tt, Planetary_Ephemeris eph) noexcept {

    if (eph == EPH_VSOP87_FULL) return VSOP87_D_Distance(jd_tt);

    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };

    double R0{ 0 };
    for (const auto& R0Coefficient : g_R0VenusCoefficients)
        R0 += (R0Coefficient.A * cos(R0Coefficient.B + (R0Coefficient.C * rho)));
    double R1{ 0 };
    for (const auto& R1Coefficient : g_R1VenusCoefficients)
        R1 += (R1Coefficient.A * cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2{ 0 };
    for (const auto& R2Coefficient : g_R2VenusCoefficients)
        R2 += (R2Coefficient.A * cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3{ 0 };
    for (const auto& R3Coefficient : g_R3VenusCoefficients)
        R3 += (R3Coefficient.A * cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4{ 0 };
    for (const auto& R4Coefficient : g_R4VenusCoefficients)
        R4 += (R4Coefficient.A * cos(R4Coefficient.B + (R4Coefficient.C * rho)));

    return (R0 + (R1 * rho) + (R2 * rhosquared) + (R3 * rhocubed) + (R4 * rho4)) / 100000000;
}
LLD AVenus::EclipticCoordinates(double jd_tt, Planetary_Ephemeris eph) {
    LLD retval{};
    retval.lat = EclipticLatitude(jd_tt, eph);
    retval.lon = EclipticLongitude(jd_tt, eph);
    retval.dst = EclipticDistance(jd_tt, eph);
    return retval;
}

// VSOP87 Orbital Parameters - Heliocentric Ecliptic at Equinox of J2000.0
double AVenus::A(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_A_VENUS.data(), g_VSOP87_A_VENUS.size(), false);
}
double AVenus::L(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_L_VENUS.data(), g_VSOP87_L_VENUS.size(), true);
}
double AVenus::K(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_K_VENUS.data(), g_VSOP87_K_VENUS.size(), false);
}
double AVenus::H(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_H_VENUS.data(), g_VSOP87_H_VENUS.size(), false);
}
double AVenus::Q(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_Q_VENUS.data(), g_VSOP87_Q_VENUS.size(), false);
}
double AVenus::P(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_P_VENUS.data(), g_VSOP87_P_VENUS.size(), false);
}

// VSOP87 Ephemeris A - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
double AVenus::VSOP87_A_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_X_VENUS.data(), g_VSOP87A_X_VENUS.size(), false);
}
double AVenus::VSOP87_A_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Y_VENUS.data(), g_VSOP87A_Y_VENUS.size(), false);
}
double AVenus::VSOP87_A_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Z_VENUS.data(), g_VSOP87A_Z_VENUS.size(), false);
}
double AVenus::VSOP87_A_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_X_VENUS.data(), g_VSOP87A_X_VENUS.size());
}
double AVenus::VSOP87_A_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Y_VENUS.data(), g_VSOP87A_Y_VENUS.size());
}
double AVenus::VSOP87_A_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Z_VENUS.data(), g_VSOP87A_Z_VENUS.size());
}

// VSOP87 Ephemeris B - Spherical Heliocentric Ecliptic at J2000.0
double AVenus::VSOP87_B_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_L_VENUS.data(), g_VSOP87B_L_VENUS.size(), true);
}
double AVenus::VSOP87_B_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87B_B_VENUS.data(), g_VSOP87B_B_VENUS.size(), true));
}
double AVenus::VSOP87_B_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_R_VENUS.data(), g_VSOP87B_R_VENUS.size(), false);
}
double AVenus::VSOP87_B_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_L_VENUS.data(), g_VSOP87B_L_VENUS.size());
}
double AVenus::VSOP87_B_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_B_VENUS.data(), g_VSOP87B_B_VENUS.size());
}
double AVenus::VSOP87_B_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_R_VENUS.data(), g_VSOP87B_R_VENUS.size());
}

// VSOP87 Ephemeris C - Rectangular Heliocentric Ecliptic at Equinox of Date
double AVenus::VSOP87_C_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_X_VENUS.data(), g_VSOP87C_X_VENUS.size(), false);
}
double AVenus::VSOP87_C_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Y_VENUS.data(), g_VSOP87C_Y_VENUS.size(), false);
}
double AVenus::VSOP87_C_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Z_VENUS.data(), g_VSOP87C_Z_VENUS.size(), false);
}
double AVenus::VSOP87_C_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_X_VENUS.data(), g_VSOP87C_X_VENUS.size());
}
double AVenus::VSOP87_C_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Y_VENUS.data(), g_VSOP87C_Y_VENUS.size());
}
double AVenus::VSOP87_C_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Z_VENUS.data(), g_VSOP87C_Z_VENUS.size());
}

// VSOP87 Ephemeris D - Spherical Heliocentric Ecliptic at Equinox of Date
double AVenus::VSOP87_D_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_L_VENUS.data(), g_VSOP87D_L_VENUS.size(), true);
}
double AVenus::VSOP87_D_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87D_B_VENUS.data(), g_VSOP87D_B_VENUS.size(), true));
}
double AVenus::VSOP87_D_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_R_VENUS.data(), g_VSOP87D_R_VENUS.size(), false);
}
double AVenus::VSOP87_D_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_L_VENUS.data(), g_VSOP87D_L_VENUS.size());
}
double AVenus::VSOP87_D_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_B_VENUS.data(), g_VSOP87D_B_VENUS.size());
}
double AVenus::VSOP87_D_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_R_VENUS.data(), g_VSOP87D_R_VENUS.size());
}

// VSOP87 Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
double AVenus::VSOP87_E_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_X_VENUS.data(), g_VSOP87E_X_VENUS.size(), false);
}
double AVenus::VSOP87_E_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Y_VENUS.data(), g_VSOP87E_Y_VENUS.size(), false);
}
double AVenus::VSOP87_E_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Z_VENUS.data(), g_VSOP87E_Z_VENUS.size(), false);
}
double AVenus::VSOP87_E_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_X_VENUS.data(), g_VSOP87E_X_VENUS.size());
}
double AVenus::VSOP87_E_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Y_VENUS.data(), g_VSOP87E_Y_VENUS.size());
}
double AVenus::VSOP87_E_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Z_VENUS.data(), g_VSOP87E_Z_VENUS.size());
}

// Illumination
double AVenus::MagnitudeAA(double r, double Delta, double i) noexcept {
    // r = Venus to Sun distance in AU
    // Delta = Venus to Earth distance in AU
    // i = Venus phase angle in radians
    i *= rad2deg;
    const double i2{ i * i };
    const double i3{ i2 * i };
    return -4.40 + (5 * log10(r * Delta)) + (0.0009 * i) + (0.000239 * i2) - (0.00000065 * i3);
}
double AVenus::MagnitudeMuller(double r, double Delta, double i) noexcept {
    // r = Venus to Sun distance in AU
    // Delta = Venus to Earth distance in AU
    // i = Venus phase angle in radians
    i *= rad2deg;
    return -4.00 + (5 * log10(r * Delta)) + (0.01322 * i) + (0.0000004247 * i * i * i);
}
