#pragma once

#include "glm\glm.hpp"

#include "aconfig.h"
#include "acoordinates.h"


struct AEllipticalObjectElements {
    // 
    double a{ 0 };           // Semimajor axis in AU
    double e{ 0 };           // eccentricity
    double i{ 0 };           // orbital plane inclination in radians
    double w{ 0 };           // argument of perihelion in radians
    double omega{ 0 };       // longitude of ascending node in radians
    double JDEquinox{ 0 };   // desired julian day to calculate position for
    double T{ 0 };           // julian day of passage of perihelion
<<<<<<< HEAD
    void print();
=======
    void print() {
        std::cout << " a: " << a << " e: " << e << " i: " << rad2deg * i
            << " w: " << rad2deg * w << " omega: " << rad2deg * omega
            << " JDEquinox: " << JDEquinox << " T: " << T << "\n";
    }
>>>>>>> 28303ac (Added asteroids and comets)
};

//struct AEllipticalPlanetaryDetails {
//    double ApparentGeocentricEclipticalLongitude{ 0 };
//    double ApparentGeocentricEclipticalLatitude{ 0 };
//    double ApparentGeocentricDistance{ 0 };
//    double ApparentLightTime{ 0 };
//    double ApparentGeocentricRA{ 0 };
//    double ApparentGeocentricDeclination{ 0 };
//    glm::dvec3 TrueGeocentricRectangularEcliptical;
//    double TrueHeliocentricEclipticalLongitude{ 0 };
//    double TrueHeliocentricEclipticalLatitude{ 0 };
//    double TrueHeliocentricDistance{ 0 };
//    double TrueGeocentricEclipticalLongitude{ 0 };
//    double TrueGeocentricEclipticalLatitude{ 0 };
//    double TrueGeocentricDistance{ 0 };
//    double TrueLightTime{ 0 };
//    double TrueGeocentricRA{ 0 };
//    double TrueGeocentricDeclination{ 0 };
//};
struct APlanetaryDetails {
    // Full set of the details from Astronomy::PlanetaryDetails() which is a copy of CAAElliptical::Calculate()
    LLD thecs{ };             // True Heliocentric ECliptical Spherical coordinates
    LLD tgecs{ };             // True Geocentric ECliptical Spherical coordinates
    glm::dvec3 tgecr{ 0.0 };  // True Geocentric ECliptical Rectangular coordinates
    LLD tgeqs{ };             // True Geocentric EQuatorial Spherical coordinates
    LLD agecs{ };             // Apparent Geocentric EClipticalSpherical coordinates
    LLD ageqs{ };             // Apparent Geocentric EQuatorial Spherical coordinates
    double tlt{ 0.0 };        // True Light Time
    double alt{ 0.0 };        // Apparent Light Time
    double jd_tt{ 0.0 };      // JD in Terrestrial Time when the position was calculated
};


struct AEllipticalObjectDetails {
    glm::dvec3 HeliocentricRectangularEquatorial{ 0.0 };
    glm::dvec3 HeliocentricRectangularEcliptical{ 0.0 };
    double HeliocentricEclipticLongitude{ 0 };
    double HeliocentricEclipticLatitude{ 0 };
    double TrueGeocentricRA{ 0 };
    double TrueGeocentricDeclination{ 0 };
    double TrueGeocentricDistance{ 0 };
    double TrueGeocentricLightTime{ 0 };
    double AstrometricGeocentricRA{ 0 };
    double AstrometricGeocentricDeclination{ 0 };
    double AstrometricGeocentricDistance{ 0 };
    double AstrometricGeocentricLightTime{ 0 };
    double Elongation{ 0 };
    double PhaseAngle{ 0 };
};

class AElliptical {
public:
    // MDO
    static glm::dvec3 HeliocentricRectangular(double JD, const AEllipticalObjectElements& elements) noexcept;
    static double PerihelionDistance2SemimajorAxis(double e, double q) noexcept;
    static double SemimajorAxis2PerihelionDistance(double e, double a) noexcept;

	// from CAAKepler
	static double calcKepler(double M, double e, int nIterations = 53) noexcept;
	// from CAAElliptical
    static APlanetaryDetails getPlanetaryDetails(double jd_tt, Planet planet, Planetary_Ephemeris eph) noexcept;
    static LLD EclipticCoordinates(double jd_tt, Planet planet, Planetary_Ephemeris eph);
    constexpr static double SemiMajorAxisFromPerihelionDistance(double q, double e) { return q / (1 - e); }
    static double MeanMotionFromSemiMajorAxis(double a) noexcept;
    static AEllipticalObjectDetails Calculate(double JD, const AEllipticalObjectElements& elements) noexcept;
    static double InstantaneousVelocity(double r, double a) noexcept;
    static double VelocityAtPerihelion(double e, double a) noexcept;
    static double VelocityAtAphelion(double e, double a) noexcept;
    static double LengthOfEllipse(double e, double a) noexcept;
    static double CometMagnitude(double g, double delta, double k, double r) noexcept;
    static double MinorPlanetMagnitude(double H, double delta, double G, double r, double PhaseAngle) noexcept;
};

class AEllipticalOrbit {
    // MDO - MEEUS98 Chapter 33
public:
    AEllipticalOrbit(AEllipticalObjectElements& elements);
    void updateElements(AEllipticalObjectElements& elements);
    glm::dvec3 EquatorialHeliocentricRectangular(double jd_tt);
    glm::dvec3 EclipticHeliocentricRectangular(double jd_tt);
<<<<<<< HEAD
    glm::dvec3 UniformHeliocentricRectangular(double jd_tt);
    LLD EclipticHeliocentricSpherical(double jd_tt);  // Use for plotting orbits
private:
    void calcParams();
public:
    double n{ 0 };
    AEllipticalObjectElements m_elements;
private:
=======
    LLD EclipticHeliocentricSpherical(double jd_tt);
private:
    void calcParams();
private:
    AEllipticalObjectElements m_elements;
>>>>>>> 28303ac (Added asteroids and comets)
    double sinOmega{ 0 };
    double cosOmega{ 0 };
    double cosi{ 0 };
    double sini{ 0 };
    double A{ 0 };
    double B{ 0 };
    double C{ 0 };
    double a{ 0 };
    double b{ 0 };
    double c{ 0 };
<<<<<<< HEAD
=======
    double n{ 0 };
>>>>>>> 28303ac (Added asteroids and comets)
};
