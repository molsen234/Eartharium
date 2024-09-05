#pragma once
#include <array>
#include "aconfig.h"
#include "acoordinates.h"

class APhysicalMoonDetails
{
public:
	//Member variables
	double ldash{ 0 };
	double bdash{ 0 };
	double ldash2{ 0 };
	double bdash2{ 0 };
	double l{ 0 };
	double b{ 0 };
	double P{ 0 };
};

class ASelenographicMoonDetails
{
public:
	//Member variables
	double l0{ 0 };
	double b0{ 0 };
	double c0{ 0 };
};


class AMoon {
public:
	static LLD EclipticCoordinates(double jd_tt, Lunar_Ephemeris eph = ELP2000_82);
	static LLD Ecliptic_Meeus_Short(double jd_tt);
	// From AA+ CAAMoon
	static double MeanLongitude(double jd_tt) noexcept;
	static double MeanElongation(double jd_tt) noexcept;
	static double MeanAnomaly(double jd_tt) noexcept;
	static double ArgumentOfLatitude(double jd_tt) noexcept;
	static double MeanLongitudeAscendingNode(double jd_tt) noexcept;
	static double MeanLongitudePerigee(double jd_tt) noexcept;
	static double TrueLongitudeAscendingNode(double jd_tt) noexcept;
	// Following three should take Moon_Ephemeris argument as ELP2000 & ELPMPP02 are added
	static double EclipticLongitude(double jd_tt) noexcept;
	static double EclipticLatitude(double jd_tt) noexcept;
	static double RadiusVector(double jd_tt) noexcept;
	static LLD EclipticCoordinates(double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_FULL);

	static double RadiusVectorToHorizontalParallax(double RadiusVector) noexcept;
	static double HorizontalParallaxToRadiusVector(double Parallax) noexcept;

	// Moon Illuminated Fraction
	static double GeocentricElongation(double ObjectAlpha, double ObjectDelta, double SunAlpha, double SunDelta) noexcept;
	static double PhaseAngle(double GeocentricElongation, double EarthObjectDistance, double EarthSunDistance) noexcept;
	static double IlluminatedFraction(double PhaseAngle) noexcept;
	static double PositionAngle(double Alpha0, double Delta0, double Alpha, double Delta) noexcept;

	// Moon Phases
	constexpr static double Phase_K(double Year) noexcept { return 12.3685 * (Year - 2000); }
	static double MeanPhase(double k) noexcept {
		//convert from K to T
		const double T{ k / 1236.85 };
		const double T2{ T * T };
		const double T3{ T2 * T };
		const double T4{ T3 * T };
		return 2451550.09766 + (29.530588861 * k) + (0.00015437 * T2) - (0.000000150 * T3) + (0.00000000073 * T4);
	}
	static double TruePhase(double jd_tt, Lunar_Phase phase);
	static double TruePhaseK(double K) noexcept;

	// Lunar Nodes
	constexpr static double Node_K(double Year) noexcept {
		// MEEUS98 chapter 51
		return 13.4223 * (Year - 2000.05);
	}
	static double PassageThroNode(double jd_tt, Node node);
	static double PassageThroNodeK(double k) noexcept;

	// Physical Moon
	static APhysicalMoonDetails CalculateGeocentric(double JD) noexcept;
	static APhysicalMoonDetails CalculateTopocentric(double JD, double Longitude, double Latitude) noexcept;
	static ASelenographicMoonDetails CalculateSelenographicPositionOfSun(double JD, Lunar_Ephemeris eph) noexcept;
	static double AltitudeOfSun(double JD, double Longitude, double Latitude, Lunar_Ephemeris eph) noexcept;
	static double TimeOfSunrise(double JD, double Longitude, double Latitude, Lunar_Ephemeris eph) noexcept;
	static double TimeOfSunset(double JD, double Longitude, double Latitude, Lunar_Ephemeris eph) noexcept;

	// Diameter
	static double GeocentricMoonSemidiameter(double Delta) noexcept;
	static double TopocentricMoonSemidiameter(double DistanceDelta, double Delta, double H, double Latitude, double Height) noexcept;


protected:
	// Physical Moon
	static double SunriseSunsetHelper(double JD, double Longitude, double Latitude, bool bSunrise, Lunar_Ephemeris eph) noexcept;
	static APhysicalMoonDetails CalculateHelper(double JD, double& Lambda, double& Beta, double& epsilon, LLD& Equatorial) noexcept;
	static void CalculateOpticalLibration(double JD, double Lambda, double Beta, double& ldash, double& bdash, double& ldash2, double& bdash2, double& epsilon, double& omega, double& DeltaU, double& sigma, double& I, double& rho) noexcept;


};



// ELP2000
struct A_ELP2000MainProblemCoefficient {
	std::array<int, 4> m_I;
	double m_A;
	std::array<double, 6> m_B;
};
struct A_ELP2000EarthTidalMoonRelativisticSolarEccentricityCoefficient {
	int m_IZ;
	std::array<int, 4> m_I;
	double m_O;
	double m_A;
	double m_P;
};
struct A_ELP2000PlanetPertCoefficient {
	std::array<int, 11> m_ip;
	double m_theta;
	double m_O;
	double m_P;
};
class AELP2000 {
public:
	// MDO
	static LLD EclipticCoordinates(double jd_tt);
	// From AA+ v2.49
	static double EclipticLongitude(double jd_tt) noexcept;
	static double EclipticLatitude(double jd_tt) noexcept;
	static double RadiusVector(double jd_tt) noexcept;
	static glm::dvec3 EclipticRectangularCoordinates(double jd_tt) noexcept;
	static glm::dvec3 EclipticRectangularCoordinatesJ2000(double jd_tt) noexcept;
	static glm::dvec3 EquatorialRectangularCoordinatesFK5(double jd_tt) noexcept;
	// Apparently the below functions are not intended for public consumption
	// (at least P.J.Naughter does not document them in the user guide)
	// So setting them private.
private:
	static double EclipticLongitude(const double* pT, int nTSize) noexcept;
	static double EclipticLatitude(const double* pT, int nTSize) noexcept;
	static double RadiusVector(const double* pT, int nTSize) noexcept;
	static double MoonMeanMeanLongitude(const double* pT, int nTSize) noexcept;
	static double MoonMeanMeanLongitude(double jd_tt) noexcept;
	static double MeanLongitudeLunarPerigee(const double* pT, int nTSize) noexcept;
	static double MeanLongitudeLunarPerigee(double jd_tt) noexcept;
	static double MeanLongitudeLunarAscendingNode(const double* pT, int nTSize) noexcept;
	static double MeanLongitudeLunarAscendingNode(double jd_tt) noexcept;
	static double MeanHeliocentricMeanLongitudeEarthMoonBarycentre(const double* pT, int nTSize) noexcept;
	static double MeanHeliocentricMeanLongitudeEarthMoonBarycentre(double jd_tt) noexcept;
	static double MeanLongitudeOfPerihelionOfEarthMoonBarycentre(const double* pT, int nTSize) noexcept;
	static double MeanLongitudeOfPerihelionOfEarthMoonBarycentre(double jd_tt) noexcept;
	static double MoonMeanSolarElongation(const double* pT, int nTSize) noexcept;
	static double MoonMeanSolarElongation(double jd_tt) noexcept;
	static double SunMeanAnomaly(const double* pT, int nTSize) noexcept;
	static double SunMeanAnomaly(double jd_tt) noexcept;
	static double MoonMeanAnomaly(const double* pT, int nTSize) noexcept;
	static double MoonMeanAnomaly(double jd_tt) noexcept;
	static double MoonMeanArgumentOfLatitude(const double* pT, int nTSize) noexcept;
	static double MoonMeanArgumentOfLatitude(double jd_tt) noexcept;
	static double MercuryMeanLongitude(double T) noexcept;
	static double VenusMeanLongitude(double T) noexcept;
	static double MarsMeanLongitude(double T) noexcept;
	static double JupiterMeanLongitude(double T) noexcept;
	static double SaturnMeanLongitude(double T) noexcept;
	static double UranusMeanLongitude(double T) noexcept;
	static double NeptuneMeanLongitude(double T) noexcept;
protected:
	static double Accumulate(const A_ELP2000MainProblemCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF) noexcept;
	static double Accumulate_2(const A_ELP2000MainProblemCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF) noexcept;
	static double Accumulate(const double* pT, int nTSize, const A_ELP2000EarthTidalMoonRelativisticSolarEccentricityCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF, bool bI1isZero) noexcept;
	static double Accumulate_2(const double* pT, int nTSize, const A_ELP2000EarthTidalMoonRelativisticSolarEccentricityCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF, bool bI1isZero) noexcept;
	static double AccumulateTable1(const A_ELP2000PlanetPertCoefficient* pCoefficients, size_t nCoefficients, double fD, double fl, double fF, double fMe, double fV, double fT, double fMa, double fJ, double fS, double fU, double fN) noexcept;
	static double AccumulateTable1_2(const double* pT, int nTSize, const A_ELP2000PlanetPertCoefficient* pCoefficients, size_t nCoefficients, double fD, double fl, double fF, double fMe, double fV, double fT, double fMa, double fJ, double fS, double fU, double fN) noexcept;
	static double AccumulateTable2(const A_ELP2000PlanetPertCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF, double fMe, double fV, double fT, double fMa, double fJ, double fS, double fU) noexcept;
	static double AccumulateTable2_2(const double* pT, int nTSize, const A_ELP2000PlanetPertCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF, double fMe, double fV, double fT, double fMa, double fJ, double fS, double fU) noexcept;
	static double Accumulate_3(const double* pT, int nTSize, const A_ELP2000EarthTidalMoonRelativisticSolarEccentricityCoefficient* pCoefficients, size_t nCoefficients, double fD, double fldash, double fl, double fF) noexcept;
};

// ELPMPP02

class AELPMPP02 {
public:
	static LLD EclipticCoordinates(double jd_tt, ELPMPP02_Correction correction = ELPMPP02_LLR, double* pDerivative = nullptr) noexcept;
	static double EclipticLongitude(double JD, ELPMPP02_Correction correction = ELPMPP02_LLR, double* pDerivative = nullptr) noexcept;
	static double EclipticLatitude(double JD, ELPMPP02_Correction correction = ELPMPP02_LLR, double* pDerivative = nullptr) noexcept;
	static double RadiusVector(double JD, ELPMPP02_Correction correction = ELPMPP02_LLR, double* pDerivative = nullptr) noexcept;
	static glm::dvec3 EclipticRectangularCoordinates(double JD, ELPMPP02_Correction correction = ELPMPP02_LLR, glm::dvec3* pDerivative = nullptr) noexcept;
	static glm::dvec3 EclipticRectangularCoordinatesJ2000(double JD, ELPMPP02_Correction correction = ELPMPP02_LLR, glm::dvec3* pDerivative = nullptr) noexcept;
private:
	static double EclipticLongitude(const double* pT, int nTSize, ELPMPP02_Correction correction = ELPMPP02_LLR, double* pDerivative = nullptr) noexcept;
	static double EclipticLatitude(const double* pT, int nTSize, ELPMPP02_Correction correction = ELPMPP02_LLR, double* pDerivative = nullptr) noexcept;
	static double RadiusVector(const double* pT, int nTSize, ELPMPP02_Correction correction = ELPMPP02_LLR, double* pDerivative = nullptr) noexcept;
	static glm::dvec3 EclipticRectangularCoordinates(const double* pT, int nTSize, ELPMPP02_Correction correction = ELPMPP02_LLR, glm::dvec3* pDerivative = nullptr) noexcept;
	static glm::dvec3 EclipticRectangularCoordinatesJ2000(const double* pT, int nTSize, ELPMPP02_Correction correction = ELPMPP02_LLR, glm::dvec3* pDerivative = nullptr) noexcept;
};

