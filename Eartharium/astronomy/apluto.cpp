
#include "../config.h"
#include "acoordinates.h"
#include "apluto.h"
#include "apluto_eph_short.h"

double APluto::EclipticLongitude(double jd_tt) noexcept {
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double J{ 34.35 + (3034.9057 * T) };
    const double S{ 50.08 + (1222.1138 * T) };
    const double P{ 238.96 + (144.9600 * T) };

    //Calculate Longitude
    double L{ 0 };
    constexpr size_t nPlutoCoefficients{ g_PlutoArgumentCoefficients.size() };
    for (size_t i{ 0 }; i < nPlutoCoefficients; i++)
    {
        double Alpha{ (g_PlutoArgumentCoefficients[i].J * J) + (g_PlutoArgumentCoefficients[i].S * S) + (g_PlutoArgumentCoefficients[i].P * P) };
        Alpha *= deg2rad;
        L += ((g_PlutoLongitudeCoefficients[i].A * sin(Alpha)) + (g_PlutoLongitudeCoefficients[i].B * cos(Alpha)));
    }
    L = L / 1000000;
    L += (238.958116 + (144.96 * T));

    return ACoord::rangezero2tau(deg2rad * L);
}
double APluto::EclipticLatitude(double jd_tt) noexcept {
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double J{ 34.35 + (3034.9057 * T) };
    const double S{ 50.08 + (1222.1138 * T) };
    const double P{ 238.96 + (144.9600 * T) };

    //Calculate Latitude
    double L{ 0 };
    constexpr size_t nPlutoCoefficients{ g_PlutoArgumentCoefficients.size() };
    for (size_t i{ 0 }; i < nPlutoCoefficients; i++)
    {
        double Alpha{ (g_PlutoArgumentCoefficients[i].J * J) + (g_PlutoArgumentCoefficients[i].S * S) + (g_PlutoArgumentCoefficients[i].P * P) };
        Alpha *= deg2rad;
        L += ((g_PlutoLatitudeCoefficients[i].A * sin(Alpha)) + (g_PlutoLatitudeCoefficients[i].B * cos(Alpha)));
    }
    L = L / 1000000;
    L += -3.908239;

    return ACoord::rangemhalfpi2halfpi(deg2rad * L);
}

double APluto::RadiusVector(double jd_tt) noexcept {
    const double T{ (jd_tt - JD_2000) / JD_CENTURY };
    const double J{ 34.35 + (3034.9057 * T) };
    const double S{ 50.08 + (1222.1138 * T) };
    const double P{ 238.96 + (144.9600 * T) };

    //Calculate Radius
    double R{ 0 };
    constexpr size_t nPlutoCoefficients{ g_PlutoArgumentCoefficients.size() };
    for (size_t i{ 0 }; i < nPlutoCoefficients; i++)
    {
        double Alpha{ (g_PlutoArgumentCoefficients[i].J * J) + (g_PlutoArgumentCoefficients[i].S * S) + (g_PlutoArgumentCoefficients[i].P * P) };
        Alpha *= deg2rad;
        R += ((g_PlutoRadiusCoefficients[i].A * sin(Alpha)) + (g_PlutoRadiusCoefficients[i].B * cos(Alpha)));
    }
    R = R / 10000000;
    R += 40.7241346;

    return R;  // in AU (astronomical units)
}

LLD APluto::EclipticCoordinates(double jd_tt) {
    return { EclipticLatitude(jd_tt), EclipticLongitude(jd_tt), RadiusVector(jd_tt) };
}