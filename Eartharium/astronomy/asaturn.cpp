#include "asaturn.h"
#include "acoordinates.h"
#include "vsop87/asaturn_vsop87_short.h"
#include "vsop87/asaturn_vsop87_full.h"

double ASaturn::EclipticLongitude(double jd_tt, Ephemeris eph) noexcept {

    if (eph == VSOP87_FULL) return VSOP87_D_Longitude(jd_tt);

    // VSOP87 D Short
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };
    const double rho5{ rho4 * rho };

    double L0{ 0 };
    for (const auto& L0Coefficient : g_L0SaturnCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1{ 0 };
    for (const auto& L1Coefficient : g_L1SaturnCoefficients)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2{ 0 };
    for (const auto& L2Coefficient : g_L2SaturnCoefficients)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3{ 0 };
    for (const auto& L3Coefficient : g_L3SaturnCoefficients)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4{ 0 };
    for (const auto& L4Coefficient : g_L4SaturnCoefficients)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5{ 0 };
    for (const auto& L5Coefficient : g_L5SaturnCoefficients)
        L5 += (L5Coefficient.A * cos(L5Coefficient.B + (L5Coefficient.C * rho)));

    double value{ (L0 + (L1 * rho) + (L2 * rhosquared) + (L3 * rhocubed) + (L4 * rho4) + (L5 * rho5)) / 100000000 };

    return ACoord::rangezero2tau(value);
}
double ASaturn::EclipticLatitude(double jd_tt, Ephemeris eph) noexcept {

    if (eph == VSOP87_FULL) return VSOP87_D_Latitude(jd_tt);

    // VSOP87 D Short
    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };
    const double rho5{ rho4 * rho };

    double B0{ 0 };
    for (const auto& B0Coefficient : g_B0SaturnCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1{ 0 };
    for (const auto& B1Coefficient : g_B1SaturnCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2{ 0 };
    for (const auto& B2Coefficient : g_B2SaturnCoefficients)
        B2 += (B2Coefficient.A * cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3{ 0 };
    for (const auto& B3Coefficient : g_B3SaturnCoefficients)
        B3 += (B3Coefficient.A * cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4{ 0 };
    for (const auto& B4Coefficient : g_B4SaturnCoefficients)
        B4 += (B4Coefficient.A * cos(B4Coefficient.B + (B4Coefficient.C * rho)));
    double B5{ 0 };
    for (const auto& B5Coefficient : g_B5SaturnCoefficients)
        B5 += (B5Coefficient.A * cos(B5Coefficient.B + (B5Coefficient.C * rho)));

    double value{ (B0 + (B1 * rho) + (B2 * rhosquared) + (B3 * rhocubed) + (B4 * rho4) + (B5 * rho5)) / 100000000 };

    return ACoord::rangemhalfpi2halfpi(value);
}
double ASaturn::EclipticDistance(double jd_tt, Ephemeris eph) noexcept {

    if (eph == VSOP87_FULL) return VSOP87_D_Distance(jd_tt);

    const double rho{ (jd_tt - JD_2000) / JD_MILLENNIUM };
    const double rhosquared{ rho * rho };
    const double rhocubed{ rhosquared * rho };
    const double rho4{ rhocubed * rho };
    const double rho5{ rho4 * rho };

    double R0{ 0 };
    for (const auto& R0Coefficient : g_R0SaturnCoefficients)
        R0 += (R0Coefficient.A * cos(R0Coefficient.B + (R0Coefficient.C * rho)));
    double R1{ 0 };
    for (const auto& R1Coefficient : g_R1SaturnCoefficients)
        R1 += (R1Coefficient.A * cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2{ 0 };
    for (const auto& R2Coefficient : g_R2SaturnCoefficients)
        R2 += (R2Coefficient.A * cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3{ 0 };
    for (const auto& R3Coefficient : g_R3SaturnCoefficients)
        R3 += (R3Coefficient.A * cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4{ 0 };
    for (const auto& R4Coefficient : g_R4SaturnCoefficients)
        R4 += (R4Coefficient.A * cos(R4Coefficient.B + (R4Coefficient.C * rho)));
    double R5{ 0 };
    for (const auto& R5Coefficient : g_R5SaturnCoefficients)
        R5 += (R5Coefficient.A * cos(R5Coefficient.B + (R5Coefficient.C * rho)));

    return (R0 + (R1 * rho) + (R2 * rhosquared) + (R3 * rhocubed) + (R4 * rho4) + (R5 * rho5)) / 100000000;
}
LLD ASaturn::EclipticCoordinates(double jd_tt, Ephemeris eph) {
    return { EclipticLatitude(jd_tt, eph), EclipticLongitude(jd_tt, eph), EclipticDistance(jd_tt, eph) };
}

// VSOP87 Orbital Parameters - Heliocentric Ecliptic at Equinox of J2000.0
double ASaturn::A(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_A_SATURN.data(), g_VSOP87_A_SATURN.size(), false);
}
double ASaturn::L(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_L_SATURN.data(), g_VSOP87_L_SATURN.size(), true);
}
double ASaturn::K(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_K_SATURN.data(), g_VSOP87_K_SATURN.size(), false);
}
double ASaturn::H(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_H_SATURN.data(), g_VSOP87_H_SATURN.size(), false);
}
double ASaturn::Q(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_Q_SATURN.data(), g_VSOP87_Q_SATURN.size(), false);
}
double ASaturn::P(double JD) noexcept {
    return VSOP87::Calculate(JD, g_VSOP87_P_SATURN.data(), g_VSOP87_P_SATURN.size(), false);
}

// VSOP87 Ephemeris A - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
double ASaturn::VSOP87_A_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_X_SATURN.data(), g_VSOP87A_X_SATURN.size(), false);
}
double ASaturn::VSOP87_A_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Y_SATURN.data(), g_VSOP87A_Y_SATURN.size(), false);
}
double ASaturn::VSOP87_A_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87A_Z_SATURN.data(), g_VSOP87A_Z_SATURN.size(), false);
}
double ASaturn::VSOP87_A_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_X_SATURN.data(), g_VSOP87A_X_SATURN.size());
}
double ASaturn::VSOP87_A_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Y_SATURN.data(), g_VSOP87A_Y_SATURN.size());
}
double ASaturn::VSOP87_A_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87A_Z_SATURN.data(), g_VSOP87A_Z_SATURN.size());
}

// VSOP87 Ephemeris B - Spherical Heliocentric Ecliptic at J2000.0
double ASaturn::VSOP87_B_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_L_SATURN.data(), g_VSOP87B_L_SATURN.size(), true);
}
double ASaturn::VSOP87_B_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87B_B_SATURN.data(), g_VSOP87B_B_SATURN.size(), true));
}
double ASaturn::VSOP87_B_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87B_R_SATURN.data(), g_VSOP87B_R_SATURN.size(), false);
}
double ASaturn::VSOP87_B_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_L_SATURN.data(), g_VSOP87B_L_SATURN.size());
}
double ASaturn::VSOP87_B_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_B_SATURN.data(), g_VSOP87B_B_SATURN.size());
}
double ASaturn::VSOP87_B_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87B_R_SATURN.data(), g_VSOP87B_R_SATURN.size());
}

// VSOP87 Ephemeris C - Rectangular Heliocentric Ecliptic at Equinox of Date
double ASaturn::VSOP87_C_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_X_SATURN.data(), g_VSOP87C_X_SATURN.size(), false);
}
double ASaturn::VSOP87_C_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Y_SATURN.data(), g_VSOP87C_Y_SATURN.size(), false);
}
double ASaturn::VSOP87_C_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87C_Z_SATURN.data(), g_VSOP87C_Z_SATURN.size(), false);
}
double ASaturn::VSOP87_C_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_X_SATURN.data(), g_VSOP87C_X_SATURN.size());
}
double ASaturn::VSOP87_C_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Y_SATURN.data(), g_VSOP87C_Y_SATURN.size());
}
double ASaturn::VSOP87_C_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87C_Z_SATURN.data(), g_VSOP87C_Z_SATURN.size());
}

// VSOP87 Ephemeris D - Spherical Heliocentric Ecliptic at Equinox of Date
double ASaturn::VSOP87_D_Longitude(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_L_SATURN.data(), g_VSOP87D_L_SATURN.size(), true);
}
double ASaturn::VSOP87_D_Latitude(double jd_tt) {
    return ACoord::rangemhalfpi2halfpi(VSOP87::Calculate(jd_tt, g_VSOP87D_B_SATURN.data(), g_VSOP87D_B_SATURN.size(), true));
}
double ASaturn::VSOP87_D_Distance(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87D_R_SATURN.data(), g_VSOP87D_R_SATURN.size(), false);
}
double ASaturn::VSOP87_D_dLongitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_L_SATURN.data(), g_VSOP87D_L_SATURN.size());
}
double ASaturn::VSOP87_D_dLatitude(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_B_SATURN.data(), g_VSOP87D_B_SATURN.size());
}
double ASaturn::VSOP87_D_dDistance(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87D_R_SATURN.data(), g_VSOP87D_R_SATURN.size());
}

// VSOP87 Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
double ASaturn::VSOP87_E_X(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_X_SATURN.data(), g_VSOP87E_X_SATURN.size(), false);
}
double ASaturn::VSOP87_E_Y(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Y_SATURN.data(), g_VSOP87E_Y_SATURN.size(), false);
}
double ASaturn::VSOP87_E_Z(double jd_tt) {
    return VSOP87::Calculate(jd_tt, g_VSOP87E_Z_SATURN.data(), g_VSOP87E_Z_SATURN.size(), false);
}
double ASaturn::VSOP87_E_dX(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_X_SATURN.data(), g_VSOP87E_X_SATURN.size());
}
double ASaturn::VSOP87_E_dY(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Y_SATURN.data(), g_VSOP87E_Y_SATURN.size());
}
double ASaturn::VSOP87_E_dZ(double jd_tt) {
    return VSOP87::Calculate_Dash(jd_tt, g_VSOP87E_Z_SATURN.data(), g_VSOP87E_Z_SATURN.size());
}
