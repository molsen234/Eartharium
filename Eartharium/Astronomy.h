#pragma once

#include <vector>

#include "config.h"

#include "../EAstronomy/all.h"

// Protos
class Astronomy;

struct CelestialDetail {  // Used for CelestialPath entries
	// keeps a planetary position in Heliocentric Ecliptic and optionally Geocentric Equatorial coordinates
	double jd_tt{ 0.0 };   // JD in Terrestrial Time when the position was calculated
	LLD hec{};             // Heliocentric ECliptic coordinates
	LLD geq{};             // Geocentric EQuatorial coordinates
	double geogha{ 0.0 };  // Greenwich Hour Angle
	void print() const {
		hec.print();
		geq.print();
	}
};

struct CelestialDetailFull {
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

	// Mostly for troubleshooting, and also serves as a key to the abbreviated member variable names
	void print() const {
		std::cout << " True Heliocentric Ecliptic Spherical:     " << thecs.str_EC() << std::endl;
		std::cout << " True Geocentric Ecliptic Rectangular:     " << tgecr.x << ", " << tgecr.y << ", " << tgecr.z << std::endl;
		std::cout << " True Geocentric Ecliptic Spherical:       " << tgecs.str_EC() << std::endl;
		std::cout << " True Geocentric Equatorial Spherical:     " << tgeqs.str_EQ() << std::endl;
		std::cout << " Apparent Geocentric Ecliptic Spherical:   " << agecs.str_EC() << std::endl;
		std::cout << " Apparent Geocentric Equatorial Spherical: " << ageqs.str_EQ() << std::endl;
		std::cout << " True Light Time:                          " << tlt << std::endl;
		std::cout << " Apparent Light Time:                      " << alt << std::endl;
		std::cout << " Calculated at JD TT:                      " << jd_tt << std::endl;
	}
};

// ---------------
//  CelestialPath
// ---------------
class CelestialPath {
// Stores a period of CelestialDetail, used for planetary paths in Earth
public:
	Planet planet{ A_NONE };
	double jdStart{ 0.0 };   // Start and End are offsets from current JD, Start is negative, End is positive.
	double jdEnd{ 0.0 };
	unsigned int jdSteps{ 0 };
	unsigned int cpType{ EC };
	bool fixedpath{ false }; // When this flag is set to true, the updater will no longer recalculate the path
	unsigned int m_refcnt{ 1 };
	size_t index{ NO_UINT };
	std::vector<CelestialDetail> entries;
private:
	Astronomy* m_astro{ nullptr };
	double m_jdCurrent{ 0.0 }; // The JD of the current data.
	double m_stepsize{ 0.0 };
	bool m_updating{ true };   // Whether this path is updating to current JD (true) or it is frozen in time (false)
public:
	CelestialPath(Astronomy* celmec, Planet planet, double start, double end, unsigned int steps, unsigned int type, bool fixed = false);
	~CelestialPath();
	bool operator==(const CelestialPath& other);
	bool operator!=(const CelestialPath& other);
	void update(bool force = true);
	void incref();
	void decref();
private:
};


// -----------
//  Astronomy
// -----------
class Astronomy {
public:
	// For ImGUI
	std::string timestr;
	// == Stellar Objects ==
	// Stellar Object Database Entry - Source: SIMBAD
	struct stellarobject {
		double ra{ 0.0 };
		double dec{ 0.0 };
		double pm_ra{ 0.0 };
		double pm_dec{ 0.0 };
		// !!! ADD: rad_vel and plax
		double vmag{ 0.0 };
		double red{ 0.0 };
		double green{ 0.0 };
		double blue{ 0.0 };
		std::string identifier;
	};
	// Stellar Object Common Name cross reference - Source: IAU Popular Names
	struct stellarobject_xref {
		std::string popular_name;
		std::string identifier;
	};
	static bool stellarobjects_loaded;
	static double stellarobjects_epoch; // Not used anywhere in Astronomy object, just here so users can query it.
	static std::vector<stellarobject> stellarobjects; // Common for all Astronomy instances
	static std::vector<stellarobject_xref> stellarobject_xrefs; // ditto
	static void loadStellarObjects();
	static void convertSIMBAD(std::string filename);
	static stellarobject& getSObyName(const std::string starname);
	static LLD getDecRAbyName(const std::string starname, bool rad = false);
	static LLD getDecRAwithPMbyName(const std::string starname, double jd, bool rad = false);
	LLD applyProperMotionJ2000(double jd_tt, LLD decra, LLD propermotion);
	static glm::vec4 getColorbyName(const std::string starname);

	// == Constellation Boundaries ==
	// Source: https://pbarbier.com/constellations/boundaries.html bound_in_20.txt and centers_20.txt
	//         (precessed from original Delponte B1875.0 boundaries to J2000.0, sorted consistent (CCW) orientation and interpolated to 1 degree.
	// NOTE: Search should capitalize abbreviations so user can ask for CMa or And and still get CMA or AND matches
	
	//struct constellationpart {
	//	// A Constellation Boundary is made up of one or more constellation parts, see below.
	//	// This is due to Serpens (SER) which inconveniently is cut into two separate outlines by Ophiucus (OPH)
	//	std::string abbr = "XYZ1";            // IAU TLA (all caps) plus running number, to accomodate Serpens
	//	std::string name = "Full Latin Name"; // Full Latin name of constellation part, e.g. SER1 = Serpens Caput, SER2 = Serpens Cauda
	//	// bounding box?
	//	std::vector<LLD> outline;             // Outline coordinates CCW when viewed from Earth's surface, in Dec/RA of J2000.0
	//};
	//struct ConstellationBoundary1 {
	//	// Collects 1 or more constellationpart structs, to accomodate Serpens being split in two by Ophiucus
	//	// While inconvenient, this also allows to create groups of constellations such as Zodiac (ZOD) and NH/SH for hemispheres
	//	// 3 types of zodiac: tropical, sidereal, constellational
	//	std::string abbr = "XYZ";             // Official TLA, but in all capitals, note Serpens (SER) is in two parts SER1 & SER2
	//	std::string name = "Full Latin Name"; // Full Latin name of constellation
	//	double area = 0.0;                    // Constellation area in deg^2
	//	size_t rank = 0;                      // Rank on sorted list of areas (1 = biggest, 88 = smallest)
	//	LLD center = { 0.0, 0.0, 0.0 };       // Barycenter of constellation polygon, useful for placing labels etc
	//	std::vector<constellationpart> boundary;
	//};

	// == Asterisms ==
	// For asterisms, it is probably best to have a data file with names/ids of stars rather than coordinates. That
	// would allow easy illustration of stellar proper motion across millennia, and there would be no need to precess
	// the asterism data.

	struct ConstellationBoundary {
		bool collection{ false };                 // Is this a collection of outlines or an actual outline
		std::string abbr{ "XYZ1" };
		std::string name{ "Full Latin Name" };
		double area{ 0.0 };
		size_t rank{ 0 };
		LLD center{};
		std::vector<LLD> outline;
		std::vector<size_t> parts;               // Stores indices of parts. There might be a better way.
	};
	static bool constellations_loaded;
	static std::vector<ConstellationBoundary> constellations;
	static void loadConstellations();
private:
	EDateTime m_datetime;    // Default constructor initializes to current system time in UTC
	double eot{ 0.0 };

	// -= Time dependent calc cache =-
	double m_gsidtime{ 0.0 }; // Apparent Greenwich Sidereal time in radians
	double m_meangsid{ 0.0 }; // Mean Greenwich Sidereal time in radians
	// Precession parameters from epoch J2000.0 to current time - applied to all stars from catalogue
	double prec_j2000_sigma{ 0.0 };
	double prec_j2000_zeta{ 0.0 };
	double prec_j2000_phi{ 0.0 };
	double prec_j2000_phi_s{ 0.0 }; // sin
	double prec_j2000_phi_c{ 0.0 }; // cos
    // Nutations
	double m_meanobliquity{ 0.0 };
	double m_trueobliquity{ 0.0 };
	double m_nutationinlongitude{ 0.0 };

	tightvec<CelestialPath*> cacheCP;
	// Planet current time cached parameters (Tip: planet 0 is the Sun)
	// This could be done using a vector<CelestialDetail> instead, if refcnt is added to CelestialDetail. Check where changes are needed !!!
	unsigned int planet_refcnt[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	double planet_ecJD[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }; // JD of Ecliptic data
	double planet_ecLat[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double planet_ecLon[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double planet_ecRadius[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double planet_jd[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };   // JD of Geocentric data
	double planet_ra[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double planet_gha[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	double planet_dec[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	//double planet_dst[10] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
public:
	Astronomy();
	~Astronomy();
	// Time functions
	void setTime(long yr, long mo, double da, double hr, double mi, double se);
	void setTimeNow();
	void setJD_UTC(double jd_utc);
	void setJD_TT(double jd_tt);
	double getJD_UTC();
	double getJD_TT();
	void setUnixTime(double utime);
	void addTime(double d, double h, double min, double sec, bool do_eot = false);
	std::string getTimeString();
	void updateTimeString();
	void dumpCurrentTime(unsigned int frame = NO_UINT);
	double ApparentGreenwichSiderealTime(double jd_utc = NO_DOUBLE, bool rad = false) noexcept;
	double MeanGreenwichSiderealTime(double jd_utc, bool rad = false) noexcept;
	double getEoT(double jd_tt = NO_DOUBLE);

	// Coordinate transformations
	LLD calcEc2Geo(double Beta, double Lambda, double Epsilon) noexcept;
	LLD calcGeo2Ec(double Delta, double Alpha, double Epsilon) noexcept;
	LLD calcGeo2Topo(LLD pos, LLD loc);
	LLD calcDecHA2GP(LLD decra);
	LLD calcDecRA2GP(LLD decra, double jd_utc);
	LLD getDecRA(Planet planet, double jd_tt = NO_DOUBLE);
	LLD getDecGHA(Planet planet, double jd_tt = NO_DOUBLE);

	// General astronomical adjustments
	double Distance2LightTime(double distance) { return distance * 0.0057755183; };
	LLD EclipticAberration(double Beta, double Lambda, double jd_tt);
	LLD EquatorialAberration(double dec, double ra, double jd_tt);
	LLD PrecessDecRA(const LLD decra, const double jd_tt = NO_DOUBLE, const double JD0 = JD_2000); // If no Epoch, assume J2000
	LLD PrecessJ2000DecRA(const LLD decra, const double jd_tt = NO_DOUBLE); // Optimized for J2000.0 epoch

	double MeanObliquityOfEcliptic(double jd_tt = NO_DOUBLE, bool rad = false); // = false);
	double TrueObliquityOfEcliptic(double jd_tt = NO_DOUBLE, bool rad = false);
	double NutationInObliquity(double jd_tt = NO_DOUBLE, bool rad = false);
	double NutationInLongitude(double jd_tt = NO_DOUBLE, bool rad = false);
	LLD EclipticNutation(double jd_tt = NO_DOUBLE);
	double NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl, bool rad = false);
	double NutationInRightAscension(double dec, double ra, double obliq, double nut_lon, double nut_obl, bool rad = false);

	// Stellar Earth Centered Equatorial
	LLD getTrueDecRAbyName(const std::string starname, double jd_tt = NO_DOUBLE, bool rad = false);
	LLD ApparentStellarPositionbyName(const std::string starname, double jd_tt = NO_DOUBLE);
	void explainApparentStellarPositionbyName(const std::string starname, double jd_tt = NO_DOUBLE);
	//LLD calcTrueDecRa(const LLD decra, const double jd_tt = NO_DOUBLE, const double JD0 = JD_2000); // If no Epoch, assume J2000

	// Lunar calculations
	LLD MoonTrueEcliptic(double jd_tt, Lunar_Ephemeris eph = MEEUS_SHORT);
	LLD MoonApparentEcliptic(double jd_tt, Lunar_Ephemeris eph = MEEUS_SHORT);
	LLD MoonTrueEquatorial(double jd_tt, Lunar_Ephemeris eph = MEEUS_SHORT);

	// Planetary calculations
	unsigned int enablePlanet(Planet planet);
	unsigned int disablePlanet(Planet planet);
	LLD EclipticCoordinates(double jd_tt, Planet planet, Planetary_Ephemeris eph);
	CelestialDetailFull planetaryDetails(double jd_tt, Planet planet, Planetary_Ephemeris eph);
	CelestialDetail getDetails(double jd_tt, Planet planet, Planetary_Ephemeris eph, unsigned int type);
	CelestialPath* getCelestialPath(Planet planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, bool fixed = false);
	void updateCelestialPaths();
	void removeCelestialPath(CelestialPath* path);
	// Planetary Heliocentric Ecliptic Coordinates (VSOP87 D - Short)
	double getEcLat(Planet planet, double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_SHORT);
	double getEcLon(Planet planet, double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_SHORT);
	double getEcDst(Planet planet, double jd_tt, Planetary_Ephemeris eph = EPH_VSOP87_SHORT);

	// UNIX time helpers - !!! FIX: Move to EDateTime
	int getString2UnixTime(std::string& string);

	// Static functions, available without object instantiation
	// currently none

private:
	void updateGeocentric(Planet planet);
	void updatePrecession();
	void update();
};
