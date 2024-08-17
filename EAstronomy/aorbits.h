#pragma once

#include <cmath>

// --------------------------
//  Kepler Elliptical Orbits
// --------------------------
// The VSOP87 orbital parameters {A,L,K,H,Q,P} are "cooked" orbital parameters. To obtain the traditional keplarian values, see (Rust) code here:
// https://docs.rs/crate/vsop87/latest/source/src/lib.rs
// Summary:
// --------
// Semimajor axis               a = A
// Eccentricity:                e = sqrt(H*H + K*K)
// Inclination:                 i = acos(1 - 2 * (P * P + Q * Q))
// Longitude of Ascending Node: Omega = atan(P / Q) = atan2(P,Q)
// Mean anomaly:                M = L
// Longitude of Periapsis:      omega = asin(H / e)
//
// NOTE: the page also has some SIMD optimization of coordinate calculations, which might be interesting to look into for the below functions.
struct Keplerian {
    double a{ 0.0 }; // semimajor_axis (a)
    double e{ 0.0 }; // eccentricity   (e)
    double i{ 0.0 }; // inclination    (i)
    double O{ 0.0 }; // Longitude of Ascending Node (Capital Omega)
    double M{ 0.0 }; // Mean Anomaly   (M0)
    double o{ 0.0 }; // Longitude of Periapsis      (small omega)
};
static Keplerian VSOP87_to_Kepler(double A, double L, double K, double H, double Q, double P) {
    Keplerian kep{};
    kep.a = A;
    kep.e = sqrt(H * H + K * K);
    kep.i = acos(1 - 2 * (P * P + Q * Q));
    kep.O = atan2(P, Q);
    kep.M = L;
    kep.o = asin(H / kep.e);
    return kep;
}
