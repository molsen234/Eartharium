

#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "config.h"
#include "Astronomy.h"


// Coordinate systems:
//  + Heliocentric ecliptic = Ecliptic Latitude, Ecliptic Longitude, Radius Vector
//  + Geocentric equatorial = Declination, Right Ascension, Distance (delta, alpha)
//  - Topocentic horizontal = Elevation, Azimuth, Distance
//  - Heliocentric cartesian  = X, Y, Z
//  - Geocentric cartesian  = X, Y, Z


// How to deal with getting calculations for paths vs reusing calculations for current moment?
// Maybe calling without param jd for current time, and only updating stored values if calling a time function


// ---------------
//  CelestialPath
// ---------------
CelestialPath::CelestialPath(Astronomy* celmec, Planet planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, bool fixed)
    : m_astro(celmec), jdStart(startoffset), jdEnd(endoffset), jdSteps(steps), cpType(type), fixedpath(fixed) { // Uses JD in TT
    this->planet = planet;
    //std::cout << "CelestialPath() constructor, Astronomy* is: " << celmec << "\n";
    m_stepsize = (endoffset - startoffset) / (double)steps;
    entries.reserve(1000);
    update(/* force */ true); // Force update even if fixed is true, to get initial data
}
CelestialPath::~CelestialPath() {
    //std::cout << "~CelestialPath{" << this << "} destroyed: planet = " << planet << "\n";
    entries.clear();
}
bool CelestialPath::operator==(const CelestialPath& other) {
    // Add fuzz factor to double comparisons !!!
    return (planet == other.planet && jdEnd == other.jdEnd && jdStart == other.jdStart && jdSteps == other.jdSteps && fixedpath == other.fixedpath);
}
bool CelestialPath::operator!=(const CelestialPath& other) {
    // Add fuzz factor to double comparisons !!!
    return (planet != other.planet || jdEnd != other.jdEnd || jdStart != other.jdStart || jdSteps != other.jdSteps || fixedpath != other.fixedpath);
}
void CelestialPath::update(bool force) {
    Planetary_Ephemeris eph{ EPH_VSOP87_SHORT };
    if (fixedpath && !force) return;
    double currentjd = m_astro->getJD_TT();
    if (m_jdCurrent == currentjd) return;
    entries.clear();
    for (double jd = currentjd+jdStart; jd < currentjd+jdEnd; jd += m_stepsize) {
        CelestialDetail detail = m_astro->getDetails(jd, planet, eph, cpType);
        entries.emplace_back(detail);
    }
    m_jdCurrent = currentjd;
}
void CelestialPath::incref() {
    m_refcnt++;
}
void CelestialPath::decref() {
    m_refcnt--;
    if (m_refcnt == 0) {
        //std::cout << "CelestialPath: Suicide! " << planet << "\n";
        delete this;
    }
}


// -----------
//  Astronomy
// -----------
bool Astronomy::stellarobjects_loaded = false;
double Astronomy::stellarobjects_epoch = 2000.0;
// These two lines and the loadStellarObjects() function below, with help from:
// https://stackoverflow.com/questions/7531981/how-to-instantiate-a-static-vector-of-object
// (Objective was to have all stellar object lookups handled by Astronomy without loading a duplicate DB for each Astronomy instantiated)
std::vector<Astronomy::stellarobject> Astronomy::stellarobjects;
std::vector<Astronomy::stellarobject_xref> Astronomy::stellarobject_xrefs;
// As above, for constellation boundaries
bool Astronomy::constellations_loaded = false;
std::vector<Astronomy::ConstellationBoundary> Astronomy::constellations;
void Astronomy::loadStellarObjects() {
    if (stellarobjects.empty()) { // First object is an empty that can be returned when no stellarobject matches a search
        stellarobjects.push_back(stellarobject());
        stellarobjects.back().ra = NO_DOUBLE;
        stellarobjects.back().dec = NO_DOUBLE;
        stellarobjects.back().pm_ra = NO_DOUBLE;
        stellarobjects.back().pm_dec = NO_DOUBLE;
        stellarobjects.back().vmag = NO_DOUBLE;
        stellarobjects.back().red = NO_DOUBLE;
        stellarobjects.back().green = NO_DOUBLE;
        stellarobjects.back().blue = NO_DOUBLE;
        stellarobjects.back().identifier = "None";
    }
    size_t res_items = 10000;
    size_t res_namexrefs = 500;
    stellarobjects.reserve(res_items);
    stellarobject_xrefs.reserve(res_namexrefs);
    std::ifstream stream("C:\\Coding\\Eartharium\\visible stars color - v2.csv");
    if (!stream.is_open()) {
        std::cout << "Astronomy::loadStellarObjects() Did not manage to open Star file!\n";
    }
    std::istringstream parse;
    std::string line, item;
    std::getline(stream, line); // Skip headers - UPD: No headers in current version of the file - UPD2: Headers are back!
    unsigned int i = 0;
    while (getline(stream, line)) {
        i++;
        stellarobjects.push_back(stellarobject());
        //std::cout << line << "\n";
        parse.clear();
        parse.str(line);
        //for (std::string item; std::getline(parse, item, ','); ) {
        //    std::cout << item << '\n';
        //}
        std::getline(parse, item, ','); // RA
        //std::cout << "Parsing RA: " << item << '\n';
        stellarobjects.back().ra = std::stod(item);
        std::getline(parse, item, ','); // Dec
        stellarobjects.back().dec = std::stod(item);
        // Proper Motion RA, Dec added 2022-02-16
        std::getline(parse, item, ','); // P.motion RA
        if (item[0] == '~') stellarobjects.back().pm_ra = 0.0;
        else stellarobjects.back().pm_ra = std::stod(item);
        //std::cout << "Proper motion: " << stellarobjects.back().pm_ra << " (" << item << ")";
        std::getline(parse, item, ','); // P.motion Dec
        if (item[0] == '~') stellarobjects.back().pm_dec = 0.0;
        else stellarobjects.back().pm_dec = std::stod(item);
        //std::cout << ", " << stellarobjects.back().pm_dec << " (" << item << ")" << '\n';
        std::getline(parse, item, ','); // Vmag
        stellarobjects.back().vmag = std::stod(item);
        std::getline(parse, item, ','); // Red
        stellarobjects.back().red = std::stod(item);
        std::getline(parse, item, ','); // Green
        stellarobjects.back().green = std::stod(item);
        std::getline(parse, item, ','); // Blue
        stellarobjects.back().blue = std::stod(item);
        std::getline(parse, item); // Identifier
        stellarobjects.back().identifier = item;
    }
    std::ifstream stream2("C:\\Coding\\Eartharium\\CommonStarNamesIAU.csv");
    if (!stream.is_open()) {
        std::cout << "Did not manage to open Star name crossreference file!\n";
    }
    getline(stream2, line); // consume header line
    i = 0;
    while (getline(stream2, line)) {
        i++;
        stellarobject_xrefs.push_back(stellarobject_xref());
        //std::cout << line << "\n";
        parse.clear();
        parse.str(line);
        std::getline(parse, item, ','); // Identifier
        stellarobject_xrefs.back().popular_name = item;
        std::getline(parse, item); // Identifier
        stellarobject_xrefs.back().identifier = item;
    }
    if (i >= res_namexrefs) std::cout << "WARNING! Astronomy::loadStellarObjects(): Name xrefs loaded from file: " << i << ", reserved space: " << res_namexrefs << ". Adding exceeding entries is SLOW!\n";
    stellarobjects_loaded = true;
}
void Astronomy::convertSIMBAD(std::string filename) {
    // Clean up raw SIMBAD exports to clean CSV format
    // NOTE: Add SIMBAD config file so exports match the below "parser"
    char outsep = ','; // Output separator
    std::ifstream streami("C:\\Coding\\Eartharium\\simbad-raw.csv");
    std::ofstream streamo("C:\\Coding\\Eartharium\\simbad-export.csv");
    if (!streami.is_open()) {
        std::cout << "ERROR: Astronomy::convertSIMBAD() could not open SIMBAD Raw File!\n";
        return;
    }
    if (!streamo.is_open()) {
        std::cout << "ERROR: Astronomy::convertSIMBAD() could not open SIMBAD Export File!\n";
        return;
    }
    std::istringstream parse, parse2;
    std::string line, item, item2;
    for (int j = 0; j < 9; j++) std::getline(streami, line); // Skip documentation header
    streamo << "#" << outsep << "id" << outsep << "typ" << outsep << "icrs_ra_h" << outsep << "icrs_ra_m" << outsep << "icrs_ra_s"
        << outsep << "icrs_dec_d" << outsep << "icrs_dec_m" << outsep << "icrs_dec_s" << outsep << "fk5_ra_h" << outsep << "fk5_ra_m"
        << outsep << "fk5_ra_s" << outsep << "fk5_dec_d" << outsep << "fk5_dec_m" << outsep << "fk5_dec_s" << outsep << "fk4_ra_h"
        << outsep << "fk4_ra_m" << outsep << "fk4_ra_s" << outsep << "fk4_dec_d" << outsep << "fk4_dec_m" << outsep << "fk4_dec_s"
        << outsep << "gal_ra" << outsep << "gal_dec" << outsep << "pm_ra" << outsep << "pm_dec" << outsep << "plx"
        << outsep << "Umag" << outsep << "Bmag" << outsep << "Vmag" << outsep << "Rmag" << outsep << "Gmag" << outsep << "Imag"
        << outsep << "spec.type\n";  // write output header line
    unsigned int i = 0;
    while (getline(streami, line)) {  // Process input data lines
        i++;
        parse.clear();
        parse.str(line);
        std::getline(parse, item, ';'); // Record number
        if (item[0] == '=') break;      // last line is all "===========..."
        streamo << rtrim(item) << outsep;
        std::getline(parse, item, ';'); // Identifier
        streamo << rtrim(item) << outsep;
        std::getline(parse, item, ';'); // Type
        streamo << rtrim(item) << outsep;
        std::getline(parse, item, ';'); // ICRS(J2000) RA_h RA_m RA_s Dec_d Dec_m Dec_s
        parse2.clear();
        parse2.str(rtrim(item));
        std::getline(parse2, item2, ' '); // RA_h
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // RA_m
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // RA_s
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // Dec_d
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // Dec_m
        streamo << item2 << outsep;
        std::getline(parse2, item2);      // Dec_s
        streamo << item2 << outsep;
        std::getline(parse, item, ';'); // FK5(J2000) RA_h RA_m RA_s Dec_d Dec_m Dec_s
        parse2.clear();
        parse2.str(rtrim(item));
        std::getline(parse2, item2, ' '); // RA_h
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // RA_m
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // RA_s
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // Dec_d
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // Dec_m
        streamo << item2 << outsep;
        std::getline(parse2, item2);      // Dec_s
        streamo << item2 << outsep;
        std::getline(parse, item, ';');   // FK4(B1950) RA_h RA_m RA_s Dec_d Dec_m Dec_s
        parse2.clear();
        parse2.str(rtrim(item));
        std::getline(parse2, item2, ' '); // RA_h
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // RA_m
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // RA_s
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // Dec_d
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // Dec_m
        streamo << item2 << outsep;
        std::getline(parse2, item2);      // Dec_s
        streamo << item2 << outsep;
        std::getline(parse, item, ';');   // Galactic J2000 RA Dec decimal
        parse2.clear();
        parse2.str(rtrim(item));
        std::getline(parse2, item2, ' '); // gal_RA in decimal
        streamo << item2 << outsep;
        std::getline(parse2, item2, ' '); // gal_Dec in decimal
        streamo << item2 << outsep;
        std::getline(parse, item, ';');   // Proper Motion
        parse2.clear();
        parse2.str(trim(item));           // trim both ends
        getline(parse2, item2, ' ');      // Proper Motion RA
        streamo << item2 << outsep;
        getline(parse2, item2);           // Proper Motion Dec
        streamo << item2 << outsep;
        std::getline(parse, item, ';');   // Parallax
        streamo << trim(item) << outsep;
        std::getline(parse, item, ';');   // Umag
        streamo << trim(item) << outsep;
        std::getline(parse, item, ';');   // Bmag
        streamo << trim(item) << outsep;
        std::getline(parse, item, ';');   // Vmag
        streamo << trim(item) << outsep;
        std::getline(parse, item, ';');   // Rmag
        streamo << trim(item) << outsep;
        std::getline(parse, item, ';');   // Gmag
        streamo << trim(item) << outsep;
        std::getline(parse, item, ';');   // Imag
        streamo << trim(item) << outsep;
        std::getline(parse, item, ';');   // Spectral Type
        streamo << rtrim(item);
        streamo << '\n';
    }
}
Astronomy::stellarobject& Astronomy::getSObyName(const std::string starname) {
    std::string sname = starname;
    for (auto& n : Astronomy::stellarobject_xrefs) {
        //std::cout << n.popular_name << "|" << n.identifier << '\n';
        if (n.popular_name == sname) {
            sname = n.identifier;
            break;
        }
    }
    for (auto& s : Astronomy::stellarobjects) {
        if (s.identifier == sname) {
            return s; // File was loaded in degrees
        }
    }
    std::cout << "ERROR! Astronomy::getSObyName() - Unknown name supplied: " << starname << '\n';
    return stellarobjects[0];  // Dummy entry with NO_DOUBLE ... "None"
}
LLD Astronomy::getDecRAbyName(const std::string starname, bool rad) {
    LLD retval{};
    std::string sname = starname;
    for (auto& n : Astronomy::stellarobject_xrefs) {
        //std::cout << n.popular_name << "|" << n.identifier << '\n';
        if (n.popular_name == sname) {
            sname = n.identifier;
            break;
        }
    }
    for (auto& s : Astronomy::stellarobjects) {
        if (s.identifier == sname) {
            //std::cout << "J2000 Ra Dec of " << starname << ": " << s.ra << ", " << s.dec << '\n';
            retval.lat = rad ? deg2rad * s.dec : s.dec;
            retval.lon = rad ? deg2rad * s.ra : s.ra;
            retval.dst = 0.0;
            return retval;
        }; // File was loaded in degrees
    }
    std::cout << "ERROR! Astronomy::getDecRAbyName() - Unknown name supplied: " << starname << '\n';
    retval.lat = 0.0;
    retval.lon = 0.0;
    retval.dst = NO_DOUBLE;
    return retval;
}
LLD Astronomy::getDecRAwithPMbyName(const std::string starname, double jd_tt, bool rad) {
    LLD retval{};
    std::string sname = starname;
    for (auto& n : Astronomy::stellarobject_xrefs) {
        //std::cout << n.popular_name << "|" << n.identifier << '\n';
        if (n.popular_name == sname) {
            sname = n.identifier;
            break;
        }
    }
    for (auto& s : Astronomy::stellarobjects) {  // !!! FIX: Maybe use AStars::applyProperMotion()
        if (s.identifier == sname) {
            // apply Proper Motion
            //std::cout << "Catalogue position (dec,ra): " << angle2DMSstring(s.dec,false) << "," << angle2uHMSstring(s.ra, false) << std::endl;
            //std::cout << "Applying PM (dec,ra): " << s.pm_dec << "," << s.pm_ra << " (in mas)" << std::endl;
            double elapsedyears = (jd_tt - JD_2000) / JD_YEAR;
            // Calculate in degrees, as that is what the catalogue provides
            //std::cout << "Proper Motion in " << elapsedyears << '\n';
            double dec = s.dec + s.pm_dec * elapsedyears / 3'600'000.0;
            // NOTE: SIMBAD proper motions are from the Hipparcos mission, the right ascension value is already multiplied by cos(s.dec)
            //double ra = s.ra + s.pm_ra / cos(deg2rad * s.dec) * elapsedyears / 3600000.0;
            double ra = s.ra + s.pm_ra * elapsedyears / 3'600'000.0;
            retval.lat = rad ? deg2rad * dec : dec; // Catalogue file was loaded in degrees
            retval.lon = rad ? deg2rad * ra : ra;
            retval.dst = ra, 0.0;
            return retval;
        }
    }
    std::cout << "ERROR! Astronomy::getDecRAwithPMbyName() - Unknown name supplied: " << starname << '\n';
    retval.lat = 0.0;
    retval.lon = 0.0;
    retval.dst = NO_DOUBLE;
    return retval;
};
LLD Astronomy::applyProperMotionJ2000(double jd_tt, LLD decra, LLD propermotion) {  // !!! FIX: Maybe use AStar:: 
    // apply Proper Motion - decra is in radians, propermotion is in milliarcseconds per year, from stellarobject db.
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    double elapsedyears = (jd_tt - JD_2000) / JD_YEAR;
    decra.lat += propermotion.lat * elapsedyears * deg2rad / 3'600'000.0;
    decra.lon += propermotion.lon * elapsedyears * deg2rad / 3'600'000.0;
    return decra;
}
glm::vec4 Astronomy::getColorbyName(const std::string starname) {
    std::string sname = starname;
    for (auto& n : Astronomy::stellarobject_xrefs) {
        //std::cout << n.popular_name << "|" << n.identifier << '\n';
        if (n.popular_name == sname) {
            sname = n.identifier;
            break;
        }
    }
    for (auto& s : Astronomy::stellarobjects) {
        if (s.identifier == sname) {
            return glm::vec4((float)s.red, (float)s.green, (float)s.blue, 1.0f);
        }
    }
    std::cout << "ERROR! Astronomy::getColorbyName() - Unknown name supplied: " << starname << '\n';
    return NO_COLOR;
}

void Astronomy::loadConstellations() {
    if (constellations_loaded) return;
    bool MY_DEBUG = false;
    // !!! FIX: Loads 4 different files. Perhaps optimize to one file format, with a function or python script to compile and sanity check !!!
    char separator = ' ';
    size_t res_items = 100;  // 88 IAU constellations, 2 more for SER1 & SER2, and a few for Zodiac etc
    if (constellations.empty()) { // First object is an empty that can be returned when no stellarobject matches a search
        constellations.reserve(res_items);
        constellations.push_back(ConstellationBoundary());
        constellations.back().collection = false;
        constellations.back().abbr = "NULL";
        constellations.back().name = "No Constellation Boundary";
        constellations.back().area = 0.0;
        constellations.back().rank = 0;
        constellations.back().center = { };
    }
    std::ifstream stream("C:\\Coding\\Eartharium\\constellations from PBarbier\\centers_20.txt");
    if (!stream.is_open()) {
        std::cout << "Astronomy::loadConstellations() Did not manage to open centers_20.txt!\n";
    }
    std::istringstream parse;
    std::string line, item;
    //std::getline(stream, line); // Skip header line
    while (getline(stream, line)) {
        parse.clear();
        parse.str(trim(line));
        if (line.length() < 5) continue; // if line too short to contain valid data, skip
        constellations.push_back(ConstellationBoundary());
        // since line is fixed width data, could slice and trim instead of using istringstream
        if (MY_DEBUG) std::cout << line << '\n';
        std::getline(parse, item, separator); // RA of center in hours
        if (MY_DEBUG) std::cout << " Parsing center RA: " << item << '\n';
        constellations.back().center.lon = hrs2rad * std::stod(item);
        std::getline(parse, item, separator); // Dec of center in degrees
        if (MY_DEBUG) std::cout << " Parsing center Dec: " << item << '\n';
        constellations.back().center.lat = deg2rad * std::stod(item);
        std::getline(parse, item, separator); // Area in degrees squared, separated by multiple spaces for small areas (alignment)
        while (item.length() < 2) std::getline(parse, item, separator); // while item is blank, keep fetching
        if (MY_DEBUG) std::cout << " Parsing area: " << item << '\n';
        constellations.back().area = std::stod(item);   // Keep in degrees squared, will probably never be used by Eartharium directly
        std::getline(parse, item, separator); // rank, separated by multiple spaces for small ranks
        while (item.length() < 1) std::getline(parse, item, separator); // while item is blank, keep fetching
        if (MY_DEBUG) std::cout << " Parsing rank: " << item << '\n';
        constellations.back().rank = std::stoi(item);
        std::getline(parse, item, separator); // IAU TLA, last entry in row only one space separation
        if (MY_DEBUG) std::cout << " Parsing IAU TLA: " << item << '\n';
        constellations.back().abbr = item;
    }
    if (MY_DEBUG) {
        std::cout << "Loaded " << constellations.size() << " constellations:\n";
        for (auto& c : constellations) {
            std::cout << " (" << c.center.lon << "," << c.center.lat << ") " << c.area << " " << c.rank << " " << c.abbr << '\n';
        }
    }
    std::ifstream stream2("C:\\Coding\\Eartharium\\constellations from PBarbier\\bound_in_20.txt");
    if (!stream.is_open()) {
    	std::cout << "Astronomy::loadConstellations() Did not manage to open bound_in_20.txt!\n";
    }
    //getline(stream2, line); // consume header line
    //i = 0;
    double ra, dec;
    bool abbr_match = false;
    while (getline(stream2, line)) {
    //	i++;
        parse.clear();
        parse.str(trim(line));
        if (line.length() < 5) continue;      // if line too short to contain valid data, skip
        abbr_match = false;
        if (MY_DEBUG) std::cout << line << '\n';
        std::getline(parse, item, separator); // RA
        if (MY_DEBUG) std::cout << " Parsing point RA: " << item << '\n';
        ra = hrs2rad * std::stod(item);
        std::getline(parse, item, separator); // Dec
        if (MY_DEBUG) std::cout << " Parsing point Dec: " << item << '\n';
        dec = deg2rad * std::stod(item);
        std::getline(parse, item, separator); // Abbreviation
        if (MY_DEBUG) std::cout << " Parsing point abbr: " << item << '\n';
        for (auto& c : constellations) {
            if (c.abbr == item) {
                abbr_match = true;
                LLD lld{};
                lld.lat = dec;
                lld.lon = ra;
                lld.dst = 0.0;
                c.outline.emplace_back(lld);
                break;
            }
        }
        if (!abbr_match) {
            std::cout << "ERROR! Astronomy::loadConstellations(): no match found for this line: \n";
            std::cout << "  " << line << '\n';
        }
    }
    // Read the name file
    separator = ',';
    std::string abbrev;
    std::ifstream stream3("C:\\Coding\\Eartharium\\IAU Constellation Names Abbr.csv");
    if (!stream.is_open()) {
        std::cout << "Astronomy::loadConstellations() Did not manage to open \'IAU Constellation Names Abbr.csv\'!\n";
    }
    //getline(stream2, line); // consume header line, enable if the file has a header
    while (getline(stream3, line)) {
        parse.clear();
        parse.str(trim(line));
        if (line.length() < 5) continue;      // if line too short to contain valid data, skip
        abbr_match = false;
        std::getline(parse, item, separator); // Abbreviation
        abbrev = item;
        to_upper(abbrev);
        if (MY_DEBUG) std::cout << " Parsing constellation abbr: " << item << " -> " << abbrev << '\n';
        std::getline(parse, item, separator); // Full Latin Name
        if (MY_DEBUG) std::cout << " Parsing constellation name: " << item << '\n';

        for (auto& c : constellations) {
            if (c.abbr == abbrev) {
                abbr_match = true;
                c.name = item;
                break;
            }
        }
        if (!abbr_match) {
            std::cout << "ERROR! Astronomy::loadConstellations(): no match found for this line: \n";
            std::cout << "  " << line << '\n';
        }
    }
    // Read the pairing file
    //MY_DEBUG = true;
    separator = ' ';
    //std::string abbrev;
    std::ifstream stream4("C:\\Coding\\Eartharium\\MDO Constellation Pairing File.txt");
    if (!stream.is_open()) {
        std::cout << "Astronomy::loadConstellations() Did not manage to open \'MDO Constellation Pairing File.txt\'!\n";
    }
    //getline(stream2, line); // consume header line, enable if the file has a header
    size_t idex, num_parts;
    while (getline(stream4, line)) { // process line by line
        parse.clear();
        parse.str(trim(line));
        if (line[0] == '#') continue;
        if (line.length() < 5) continue;      // if line too short to contain valid data, skip
        abbr_match = false;
        std::getline(parse, item, separator); // Abbreviation
        abbrev = item;
        to_upper(abbrev);
        if (MY_DEBUG) std::cout << " Parsing constellation abbr: " << item << " -> " << abbrev << '\n';
        std::getline(parse, item, separator); // Number of parts
        num_parts = stoi(item);
        idex = 0;
        // find existing entry matching abbr
        for (size_t i = 0; i < constellations.size(); i++) {
            if (constellations[i].abbr == abbrev) {
                constellations[i].collection = true;
                idex = i;
                break;
            }
        }
        // Might want to log is idex is still 0
        bool placed_part;
        for (size_t i = 0; i < num_parts; i++) {
            std::getline(parse, item, separator);  // Read part abbr one by one
            abbrev = item;
            to_upper(abbrev);
            placed_part = false;
            for (size_t j = 0; j < constellations.size(); j++) {  // Find matching part in db
                if (constellations[j].abbr == abbrev) {
                    constellations[idex].parts.push_back(j);
                    placed_part = true;
                    break;
                }
            }
            // if !placed_part log that part abbrev was not in db
        }

    }
    // Dump entire db to validate
    MY_DEBUG = false;
    if (MY_DEBUG) {
        for (size_t i = 0; i < constellations.size(); i++) {
            std::cout << "Entry " << i << " :\n";
            std::cout << "  Abbreviation:     " << constellations[i].abbr << '\n';
            std::cout << "  Full Latin Name:  " << constellations[i].name << '\n';
            std::cout << "  Area (deg^2):     " << constellations[i].area << '\n';
            std::cout << "  Rank (by area):   " << constellations[i].rank << '\n';
            std::cout << "  Center (RA,Dec):  " << "(" << constellations[i].center.lon << "," << constellations[i].center.lat << ")" << '\n';
            std::cout << "  Collection:       " << constellations[i].collection << '\n';
            std::cout << "  Number of Parts:  " << constellations[i].parts.size() << '\n';
            for (size_t j = 0; j < constellations[i].parts.size(); j++) {
                std::cout << "    Part " << j << ": " << constellations[constellations[i].parts[j]].abbr << '\n';
            }
            std::cout << "  Number of Points: " << constellations[i].outline.size() << '\n';
            for (size_t j = 0; j < constellations[i].outline.size(); j++) {
                std::cout << "    Point " << j << ": " << "(" << /*rad2hrs * */ constellations[i].outline[j].lon << "," << /* rad2deg * */ constellations[i].outline[j].lat << ")" << '\n';
            }
        }
    }
    // Sanity check: Prune parts without outline points, which were not paired with other outlines, while logging prunded entries.
    constellations_loaded = true;
}


Astronomy::Astronomy() {
    //std::cout << "Astronomy{" << this << "} created!\n";
    //setTimeNow();  // Ensure a default time is set
    m_datetime.setTimeNow();
    if (stellarobjects_loaded) return;
    loadStellarObjects();
}
Astronomy::~Astronomy() {
    //std::cout << "Astronomy{" << this << "} destroyed!\n";
}

// Time functions
void Astronomy::setTime(long yr, long mo, double da, double hr, double mi, double se) {
    eot = 0.0;
    m_datetime.setTime(yr, mo, da, hr, mi, se);
    update();
}
void Astronomy::setTimeNow() {
    m_datetime.setTimeNow();
    update();
}
void Astronomy::setJD_UTC(double jd_utc) { // Expects JD in UTC, use setJD_TT() if you have a JD in TT (Dynamical Time)
    eot = 0.0;
    m_datetime.setJD_UTC(jd_utc);
    update();
}
void Astronomy::setJD_TT(double jd_tt) {
    eot = 0.0;
    m_datetime.setJD_TT(jd_tt);
    update();
}
double Astronomy::getJD_UTC() {
    return m_datetime.jd_utc();
}
double Astronomy::getJD_TT() {
    return m_datetime.jd_tt();
}
void Astronomy::setUnixTime(double utime) { // Unix time is in UTC
    // Unix time stamps are number of seconds since 1970-01-01 00:00:00
    eot = 0.0;
    m_datetime.setUnixTime((long)utime);
    update();
}
void Astronomy::addTime(double d, double h, double min, double sec, bool do_eot) {
    //std::cout << "Current: " << getTimeString() << " JD_TT: " << getJD_TT() << " \t";
    // Is EoT being enabled now
    if (do_eot && eot == 0.0) eot = AEarth::EquationOfTime(m_datetime.jd_tt(), EPH_VSOP87_FULL);
    if (eot != 0.0 && do_eot) min += eot; // If EoT was applied last time update, remove it.
    //std::cout << "Astronomy::addTime() - Remove EoT: " << eot << '\n';
    if (!do_eot) eot = 0.0;
    m_datetime.addTime(0, 0, d, h, min, sec);
    if (do_eot) {
        eot = AEarth::EquationOfTime(m_datetime.jd_tt(), EPH_VSOP87_FULL);
        //std::cout << "Astronomy::addTime() -  Apply EoT: " << eot << '\n';
        m_datetime.addTime(0, 0, 0.0, 0.0, -eot, 0.0);
    }
    update();
    //std::cout << "Updated: " << getTimeString() << " JD_TT: " << getJD_TT() << '\n';
}
std::string Astronomy::getTimeString() { // Date time in UTC
    // Builds and returns a date time string from current JD
    return m_datetime.string();
}
void Astronomy::updateTimeString() {
    // Triggers an update of the internally stored std::string Astronomy::timestr based on current JD
    //m_datetime.string(timestr);
    timestr = m_datetime.string();
}
void Astronomy::dumpCurrentTime(unsigned int frame) {
    if (frame == NO_UINT) std::cout << "Current astronomical time (in UTC) at frame none:\n";
    else std::cout << "Current astronomical time (in UTC) at frame " << frame << ":\n";
    std::cout << " - Year:   " << m_datetime.year() << '\n';
    std::cout << " - Month:  " << m_datetime.month() << '\n';
    std::cout << " - Day:    " << m_datetime.day() << '\n';
    std::cout << " - Hour:   " << m_datetime.hour() << '\n';
    std::cout << " - Minute: " << m_datetime.minute() << '\n';
    std::cout << " - Second: " << m_datetime.second() << '\n';
    std::cout << " - Julian Day (UTC): " << m_datetime.jd_utc() << '\n';
    std::cout << " - Julian Day (TT):  " << m_datetime.jd_tt() << '\n';
}
double Astronomy::ApparentGreenwichSiderealTime(double jd_utc, bool rad) noexcept {
    if (jd_utc == NO_DOUBLE) jd_utc = getJD_UTC();
    if (jd_utc == m_datetime.jd_utc()) return rad ? m_gsidtime : rad2deg * m_gsidtime; // Use cached data
    double gsidtime = AEarth::ApparentGreenwichSiderealTime(jd_utc);
    return rad ? gsidtime : rad2deg * gsidtime;
}
double Astronomy::MeanGreenwichSiderealTime(double jd_utc, bool rad) noexcept {
    if (jd_utc == NO_DOUBLE) jd_utc = getJD_UTC();
    if (jd_utc == m_datetime.jd_utc()) return rad ? m_meangsid : rad2deg * m_meangsid; // Use cached data
    const double mgst = AEarth::MeanGreenwichSiderealTime(jd_utc);
    return rad ? mgst : rad2deg * mgst;
}
double Astronomy::getEoT(double jd_tt) {  // NOTE:  EoT is used while updating time, so be careful if caching
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    return AEarth::EquationOfTime(jd_tt, EPH_VSOP87_FULL);
}

// Coordinate Transformations
LLD Astronomy::calcEc2Geo(double Beta, double Lambda, double Epsilon) noexcept {
    // Spherical coordinates Ecliptic to Equatorial
    return Spherical::Ecliptic2Equatorial2(Lambda, Beta, Epsilon);
}
LLD Astronomy::calcGeo2Ec(double Delta, double Alpha, double Epsilon) noexcept {
    // Spherical coordinates Equatorial to Ecliptic
    return Spherical::Equatorial2Ecliptic2(Alpha, Delta, Epsilon);
}
LLD Astronomy::calcGeo2Topo(LLD pos, LLD loc) {
    // !!! FIX: This simply rotates from equatorial to horizontal coordinates, it does NOT account for the shape of Earth
    // In detail, topocentric is not equal to horizontal. The end coordinates here are horizontal, not topocentric. !!!
    // AA+: CAACoordinateTransformation::Equatorial2Horizontal()
    // TODO: Does NOT account for altitude of observer !!! (maybe doesn't matter, up is still up? Mountains are not high enough for parallax)
    //       UPD: height parallax is significant, you can watch the sun set twice by taking an elevator up a few floors.
    // pos is DecGHA, loc is LatLon
    // NOTE: No caching, not likely to be called with same position twice for same JD
    double LocalHourAngle = pos.lon + loc.lon;
    // from CAACoordinateTransformation::Equatorial2Horizontal(rad2hrs * LocalHourAngle, rad2deg * pos.lat, rad2deg * loc.lat);
    LLD topo;
    topo.lon = atan2(sin(LocalHourAngle), cos(LocalHourAngle) * sin(loc.lat) - tan(pos.lat) * cos(loc.lat));
    topo.lat = asin(sin(loc.lat) * sin(pos.lat) + cos(loc.lat) * cos(pos.lat) * cos(LocalHourAngle));
    return topo;
}
LLD Astronomy::calcDecHA2GP(LLD decra) {
    // Map HA [0 ; pi) -> [0 ; -pi) and [pi ; tau) -> [pi ; 0)
    // !!! FIX: This assumes planetocentric coordinates (longitude is east-from-north) Make option for west-from-south !!!
    LLD res = decra;
    if (res.lon < pi) res.lon = -res.lon;
    else res.lon = tau - res.lon;  // res.lon is now -pi to pi east of south (or east of north?)
    res.lon = ACoord::rangempi2pi(res.lon);
    return res;
}
LLD Astronomy::calcDecRA2GP(LLD decra, double jd_utc) { // Needs JD as UTC
    // Gives GP in geocentric coordinates, what about geographic coordinates?
    LLD retval{};
    if (jd_utc == NO_DOUBLE) {  // jd was not specified, so go with current time
        retval.lat = decra.lat;
        retval.lon = m_gsidtime - decra.lon;
    }
    else {
        retval.lat = decra.lat;
        retval.lon = AEarth::ApparentGreenwichSiderealTime(jd_utc) - decra.lon;
    }
    retval.dst = 0.0;
    return calcDecHA2GP(retval);
}
LLD Astronomy::getDecRA(Planet planet, double jd_tt) {  // JD in TT
    LLD retval{};
    Planetary_Ephemeris eph{ EPH_VSOP87_SHORT };
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    if (jd_tt == planet_jd[planet]) {
        retval.lat = planet_dec[planet];
        retval.lon = planet_ra[planet];
        retval.dst = 0.0;
        return retval;
    }
    CelestialDetail details = getDetails(jd_tt, planet, eph, ECGEO);
    retval.lat = details.geq.lat;
    retval.lon = details.geq.lon;
    retval.dst = 0.0;
    return retval;
}
LLD Astronomy::getDecGHA(Planet planet, double jd_tt) { // JD in TT
    LLD retval{};
    Planetary_Ephemeris eph{ EPH_VSOP87_SHORT };
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    if (jd_tt == planet_jd[planet]) {
        retval.lat = planet_dec[planet];
        retval.lon = planet_gha[planet];
        retval.dst = 0.0;
        return retval;
    }
    CelestialDetail details = getDetails(jd_tt, planet, eph, ECGEO);
    retval.lat = details.geq.lat;
    retval.lon = details.geogha;
    retval.dst = 0.0;
    return retval;
}

// General astronomical adjustments
LLD Astronomy::EclipticAberration(double Beta, double Lambda, double jd_tt) {
    // From CAAAberration::EclipticAberration() converted to accept radians
    // See MEEUS98 Chapter 23 Ron-Vondrák abberation
    LLD aberration{};
    aberration = AEarth::EclipticAberration(Lambda, Beta, jd_tt, EPH_VSOP87_FULL);
    return aberration;
}
LLD Astronomy::EquatorialAberration(double dec, double ra, double jd_tt) {
    // See MEEUS98 Chapter 23 for Ron-Vondrák aberration
    // This aberration is given in FK5 J2000.0, so MUST be applied before precession, and NOT together with nutation
    LLD aberration{};
    aberration = AEarth::EquatorialAberration(ra, dec, jd_tt, EPH_VSOP87_FULL);
    return aberration;
}
LLD Astronomy::PrecessDecRA(const LLD decra, const double jd_tt, const double JD0) {  // ALWAYS in RADIANS
    // See https://climate.nasa.gov/news/2948/milankovitch-orbital-cycles-and-their-role-in-earths-climate/
    // apsidial precession causes the axial precession period to lower from 25771.5 years to around 23000 years.
    // Does the below take into account apsidial precession? A full cirle of Polaris does seem to be around 23000yr
    // Precess the Equinox for a geocentric coordinate (catalogue with proper motion applied)
    // AA+: CAAPrecession::PrecessEquatorial()
    // MEEUS98: Chapter 21
    // (Does NOT abberate)
    // JD0 is Epoch, defaults to J2000 (via JD_2000 macro)
    // jd is desired Julian Day, default to current JD in relevant Astronomy object
    if (JD0 == JD_2000) return PrecessJ2000DecRA(decra, jd_tt);  // Use shorter calculation when epoch is J2000.0 (T = 0)
    LLD retval{};
    const double JD = jd_tt == NO_DOUBLE ? getJD_TT() : jd_tt;
    const double T = (JD0 - JD_2000) / 36525;
    const double Tsquared = T * T;
    const double t = (JD - JD0) / 36525;
    const double tsquared = t * t;
    const double tcubed = tsquared * t;
    const double sigma  = ACoord::dms2rad(0, 0, (2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (0.30188 - 0.000344 * T) * tsquared + 0.017998 * tcubed);
    const double zeta   = ACoord::dms2rad(0, 0, (2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (1.09468 + 0.000066 * T) * tsquared + 0.018203 * tcubed);
    const double phi    = ACoord::dms2rad(0, 0, (2004.3109 - 0.8533 * T - 0.000217 * Tsquared) * t - (0.42665 + 0.000217 * T) * tsquared - 0.041833 * tcubed);
    const double A = cos(decra.lat) * sin(decra.lon + sigma);
    const double B = cos(phi) * cos(decra.lat) * cos(decra.lon + sigma) - sin(phi) * sin(decra.lat);
    const double C = sin(phi) * cos(decra.lat) * cos(decra.lon + sigma) + cos(phi) * sin(decra.lat);
    // if debugging, create printable value
    retval.lat = asin(C);
    retval.lon = atan2(A, B) + zeta;
    retval.dst = 0.0;
    //std::cout << "JD: " << JD << "Ra Dec : " << decra.lon << ", " << decra.lat << "->" << value.lon << ", " << value.lat << '\n';
    return retval;
}
LLD Astronomy::PrecessJ2000DecRA(const LLD decra, const double jd_tt) {  // ALWAYS in RADIANS
    // Precess the Equinox for a geocentric coordinate (catalogue with proper motion applied)
    // jd_tt is desired Julian Day, default to current JD in relevant Astronomy object
    LLD retval{};
    double JD = jd_tt == NO_DOUBLE ? getJD_TT() : jd_tt;
    if (JD == getJD_TT()) {  // If current time, use cached values
        const double A = cos(decra.lat) * sin(decra.lon + prec_j2000_sigma);
        const double B = prec_j2000_phi_c * cos(decra.lat) * cos(decra.lon + prec_j2000_sigma) - prec_j2000_phi_s * sin(decra.lat);
        const double C = prec_j2000_phi_s * cos(decra.lat) * cos(decra.lon + prec_j2000_sigma) + prec_j2000_phi_c * sin(decra.lat);
        retval.lat = asin(C);
        retval.lon = atan2(A, B) + prec_j2000_zeta;
        retval.dst = 0.0;
    }
    else {
        const double t = (JD - JD_2000) / 36525;
        const double tsquared = t * t;
        const double tcubed = tsquared * t;
        const double sigma = ACoord::dms2rad(0, 0, (2306.2181) * t + (0.30188) * tsquared + 0.017998 * tcubed);
        const double zeta  = ACoord::dms2rad(0, 0, (2306.2181) * t + (1.09468) * tsquared + 0.018203 * tcubed);
        const double phi   = ACoord::dms2rad(0, 0, (2004.3109) * t - (0.42665) * tsquared - 0.041833 * tcubed);
        // The above can be calculated once per time update and cached - done.
        const double A = cos(decra.lat) * sin(decra.lon + sigma);
        const double B = cos(phi) * cos(decra.lat) * cos(decra.lon + sigma) - sin(phi) * sin(decra.lat);
        const double C = sin(phi) * cos(decra.lat) * cos(decra.lon + sigma) + cos(phi) * sin(decra.lat);
        retval.lat = asin(C);
        retval.lon = atan2(A, B) + zeta;
        retval.dst = 0.0;
    }
    return retval;
}
double Astronomy::MeanObliquityOfEcliptic(double jd_tt, bool rad) {
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    // !!! FIX: Check cached value against jd_tt
    double retval = AEarth::MeanObliquityOfEcliptic(jd_tt);
    return rad ? retval : rad2deg * retval;
}
double Astronomy::TrueObliquityOfEcliptic(double jd_tt, bool rad) {
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    // !!! FIX: Check cached value against jd_tt
    return MeanObliquityOfEcliptic(jd_tt, rad) + NutationInObliquity(jd_tt, rad);
}
double Astronomy::NutationInObliquity(double jd_tt, bool rad) {
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    // !!! FIX: Check cached value against jd_tt
    double value = AEarth::NutationInObliquity(jd_tt);
    return rad ? value : rad2deg * value;
}
double Astronomy::NutationInLongitude(double jd_tt, bool rad) {
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    // !!! FIX: Check cached value against jd_tt
    double value = AEarth::NutationInLongitude(jd_tt);
    return rad ? value : rad2deg * value;
    // retval is in degrees by default
}
LLD Astronomy::EclipticNutation(double jd_tt) {
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    // !!! FIX: Check cached value against jd_tt
    return AEarth::EclipticNutation(jd_tt);
}
double Astronomy::NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl, bool rad) {
    if (rad) return AEarth::NutationInDeclination(ra, obliq, nut_lon, nut_obl);
    return rad2deg * AEarth::NutationInDeclination(deg2rad * ra, deg2rad * obliq, deg2rad * nut_lon, deg2rad * nut_obl);
}
double Astronomy::NutationInRightAscension(double ra, double dec, double obliq, double nut_lon, double nut_obl, bool rad) {
    if (rad) return AEarth::NutationInRightAscension(ra, dec, obliq, nut_lon, nut_obl);
    return rad2deg * AEarth::NutationInRightAscension(deg2rad*ra, deg2rad * dec, deg2rad * obliq, deg2rad * nut_lon, deg2rad * nut_obl);
}
// Stellar Earth Centered Equatorial
LLD Astronomy::getTrueDecRAbyName(const std::string starname, double jd_tt, bool rad) {
    // Apply Proper Motion
    // (Note: see https://stargazerslounge.com/topic/289480-calculating-ra-and-decl-of-a-star/ which I read after implementing below)
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    LLD decra = getDecRAwithPMbyName(starname, m_datetime.jd_tt(), true); // Always calculate from radians, convert at end to what bool rad indicates
    //std::cout << "Astronomy::getTrueDecRAbyName(): Catalogue RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    // Apply Precession
    double JD0 = EDateTime::getJDUTC2TT(JD_2000); // Epoch 2000.0 - Same as epoch of the catalogue
    const double T = (JD0 - JD_2000) / 36525;
    const double Tsquared = T * T;
    const double t = (jd_tt - JD0) / 36525;
    const double tsquared = t * t;
    const double tcubed = tsquared * t;
    // NOTE: Since the Epoch of the star database is J2000, T and Tsquared are zero, so this could be simplified.
    //       The -O3 compiler flag probably optimizes this? No.
    const double sigma = deg2rad * ((2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (0.30188 - 0.000344 * T) * tsquared + 0.017998 * tcubed) / 3600.0;
    const double zeta  = deg2rad * ((2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (1.09468 + 0.000066 * T) * tsquared + 0.018203 * tcubed) / 3600.0;
    const double phi   = deg2rad * ((2004.3109 - 0.8533 * T - 0.000217 * Tsquared) * t - (0.42665 + 0.000217 * T) * tsquared - 0.041833 * tcubed) / 3600.0;
    const double A = cos(decra.lat) * sin(decra.lon + sigma);
    const double B = cos(phi) * cos(decra.lat) * cos(decra.lon + sigma) - sin(phi) * sin(decra.lat);
    const double C = sin(phi) * cos(decra.lat) * cos(decra.lon + sigma) + cos(phi) * sin(decra.lat);
    // Precessed decra in radians - FIX: !!! WRONG RA VALUE, Dec is OK !!!
    decra.lon = atan2(A, B) + zeta;
    decra.lat = asin(C); // If star is close to pole, this should ideally use acos(sqrt(A*A+B*B));

    // For True position, only precession is applied
    //std::cout << "Astronomy::getTrueDecRAbyName(): Precessed RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    //// Apply nutation
    //double obliq = MeanObliquityOfEcliptic(jd_tt, true);
    //double nut_lon = NutationInLongitude(jd_tt, true);
    //double nut_obl = NutationInObliquity(jd_tt, true);
    //double nut_dec = NutationInDeclination(decra.lon, obliq, nut_lon, nut_obl, true);
    //double nut_ra = NutationInRightAscension(decra.lat, decra.lon, obliq, nut_lon, nut_obl, true);
    ////std::cout << " Calculated obliquity:       " << angle2DMSstring(obliq, true) << '\n';
    ////std::cout << " Calculated nutation in lon: " << angle2DMSstring(nut_lon, true) << '\n';
    ////std::cout << " Calculated nutation in obl: " << angle2DMSstring(nut_obl, true) << '\n';
    ////std::cout << " Calculated nutation in RA:  " << angle2DMSstring(nut_ra, true) << '\n';
    ////std::cout << " Calculated nutation in Dec: " << angle2DMSstring(nut_dec, true) << '\n';
    //decra.lat += nut_dec;
    //decra.lon += nut_ra;
    //// Apply aberration
    ////CAA2DCoordinate aberration = EquatorialAberration(decra.lon, decra.lat, m_jd, true);
    //LLD aberration = EquatorialAberration(decra.lat, decra.lon, m_datetime.jd_tt(), true);
    //decra.lat += aberration.lat;
    //decra.lon += aberration.lon;
    ////std::cout << "Astronomy::getTrueDecRAbyNameJD(): Aberrated RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    //// Precession parameters sigma, zeta & phi in arc seconds
    ////std::cout << "Astronomy::getTrueDecRAbyNameJD(): Nutated RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    //// Ignore annual parallax
    if (!rad) {
        decra.lat *= rad2deg;
        decra.lon *= rad2deg;
    }
    return decra;
}

LLD Astronomy::ApparentStellarPositionbyName(const std::string starname, double jd_tt) {
    // (Note: see https://stargazerslounge.com/topic/289480-calculating-ra-and-decl-of-a-star/ which I read after implementing below)
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    // Apply Proper Motion
    LLD decra = getDecRAwithPMbyName(starname, jd_tt, true); // Always calculate from radians, convert at end to what bool rad indicates
    // Apply aberration (note if simpler aberration formula is used, it should be applied with nutation, see MEEUS98 chapter 23)
    decra += AEarth::EquatorialAberration(decra.lat, decra.lon, jd_tt, EPH_VSOP87_FULL); // Vondrak-Ron aberration is in J2000.0, so apply before precession
    // Apply Precession to get True position
    decra = PrecessJ2000DecRA(decra, jd_tt);
    // Calculate nutation in equatorial geocentric
    double obliq = MeanObliquityOfEcliptic(jd_tt, true);
    LLD ecnut = AEarth::EclipticNutation(jd_tt);
    // Apply the nutation
    decra += AEarth::EquatorialNutation(decra, obliq, ecnut); // Calculate based on True position
    // Annual stellar parallax is ignored here
    return decra;
}

void Astronomy::explainApparentStellarPositionbyName(const std::string starname, double jd_tt) {
    // (See above ApparentStellarPositionbyName() and MEEUS98 chapter 23)
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    stellarobject so = getSObyName(starname);
    if (so.identifier == "None") return;  // bail if no object was found, getSObyName() already printed error.
    std::cout << "Apparent position calculation for " << starname << " (" << so.identifier << ") at JD " << jd_tt << "\n";
    LLD decra{};
    decra.lat = so.dec;
    decra.lon = so.ra;
    decra.dst = 0.0;
    decra *= deg2rad;  // Both Dec and RA are given in degrees in the catalogue
    LLD pm{};
    pm.lat = so.pm_dec;
    pm.lon = so.pm_ra;
    pm.dst = 0.0;
    // decra is now position of J2000 in equinox of J2000
    std::cout << " Catalogue position (FK5 J2000):   " << ACoord::formatDecRA(decra) << "\n";
    // Apply Proper Motion
    std::cout << " Catalogue proper motion (mas/yr): " << so.pm_dec << " / " << so.pm_ra << "\n";
    decra = applyProperMotionJ2000(jd_tt, decra, pm);
    // decra is now position of date in equinox of J2000 = Mean position
    std::cout << " Mean position (w/proper motion):  " << ACoord::formatDecRA(decra) << "\n";
    // Calculate aberration
    LLD aberration = EquatorialAberration(decra.lat, decra.lon, jd_tt); // Calculate based on True position
    decra += aberration;
    std::cout << " Position w/Vondrak-Ron aberration:   " << ACoord::formatDecRA(decra) << "\n";
    // Apply Precession
    decra = PrecessJ2000DecRA(decra, jd_tt);
    // decra is now position of date in equinox of date = True position  (but with aberration included, so maybe astrometric?)
    std::cout << " Position (w/precession):     " << ACoord::formatDecRA(decra) << "\n";
    // Calculate nutation in equatorial geocentric
    double obliq = MeanObliquityOfEcliptic(jd_tt, true);
    LLD ecnut = AEarth::EclipticNutation(jd_tt);
    LLD eqnut = AEarth::EquatorialNutation(decra, obliq, ecnut); // Calculate based on True position
    // Apply nutation and aberration
    decra += eqnut;
    std::cout << " Position with nutation:           " << ACoord::formatDecRA(decra) << "\n";
    // decra is now Apparent position of date in equinox of date (except for parallax which is currently ignored)
    std::cout << " Annual (stellar) parallax has been ignored.\n" << std::endl;
}


// Lunar calculations - Geocentric
LLD Astronomy::MoonTrueEcliptic(double jd_tt, Lunar_Ephemeris eph) {
    // !!! FIX: Maybe make JD optional, defaulting to current, and cache result
    return AMoon::EclipticCoordinates(jd_tt, eph);
}
LLD Astronomy::MoonApparentEcliptic(double jd_tt, Lunar_Ephemeris eph) {
    // !!! FIX: Maybe make JD optional, defaulting to current, and cache result
    LLD moonpos = AMoon::EclipticCoordinates(jd_tt, eph);
    if (eph != MEEUS_SHORT) { // According to MEEUS98, nutation is all that is required for MEEUS_SHORT, and that is already included in AMoon::Ecliptic_Meeus_Short()
        moonpos += AEarth::EclipticAberration(moonpos.lon, moonpos.lat, jd_tt, EPH_VSOP87_FULL);
        moonpos += FK5::CorrectionInLonLat(moonpos, jd_tt);  // Convert to the FK5 system
        moonpos.lon += AEarth::NutationInLongitude(jd_tt);
    }
    return moonpos;
}
LLD Astronomy::MoonTrueEquatorial(double jd_tt, Lunar_Ephemeris eph) {
    // Ecliptic Moon
    LLD emoon = AMoon::EclipticCoordinates(jd_tt, eph);
    //LLD emoon = MoonApparentEcliptic(jd_tt, eph);
    //std::cout << emoon.str_EC() << std::endl;

    // Equatorial Moon
    double Epsilon = TrueObliquityOfEcliptic(jd_tt, true);
    LLD qmoon = Spherical::Ecliptic2Equatorial(emoon, Epsilon);
    return qmoon;
}

// Planetary calculations
unsigned int Astronomy::enablePlanet(Planet planet) {
    return ++planet_refcnt[planet];
}
unsigned int Astronomy::disablePlanet(Planet planet) {
    return --planet_refcnt[planet];
}

LLD Astronomy::EclipticCoordinates(double jd_tt, Planet planet, Planetary_Ephemeris eph) {
    // !!! FIX: Use cache
    LLD retval{};
    switch (planet) {
        case A_MERCURY: return AMercury::EclipticCoordinates(jd_tt, eph);
        case A_VENUS:   return AVenus::EclipticCoordinates(jd_tt, eph);
        case A_MARS:    return AMars::EclipticCoordinates(jd_tt, eph);
        case A_JUPITER: return AJupiter::EclipticCoordinates(jd_tt, eph);
        case A_SATURN:  return ASaturn::EclipticCoordinates(jd_tt, eph);
        case A_URANUS:  return AUranus::EclipticCoordinates(jd_tt, eph);
        case A_NEPTUNE: return ANeptune::EclipticCoordinates(jd_tt, eph);
     }
    std::cout << "Astronomy::EclipticalCoordinates(): ERROR - Unknown planet id passed" << std::endl;
    retval.lat = NO_DOUBLE;
    retval.lon = NO_DOUBLE;
    retval.dst = NO_DOUBLE;
    return retval;
}

CelestialDetailFull Astronomy::planetaryDetails(double jd_tt, Planet planet, Planetary_Ephemeris eph) {
    // return value
    CelestialDetailFull details;
    // Precision parameters for Light Time compensation
    const double prec_angle = 0.00001 * deg2rad;  // 0.00001 degrees = 36 mas (milli arc seconds)
    const double prec_distance = 0.000001;        // 0.000001 AU = 150 km (kilometers)
    // Iterate to find the positions adjusting for light-time correction if required
    double JD0{ jd_tt };            // jd modified during iteration
    LLD pla{};       // target planet/sun coordinates
    LLD pla_prev{};  // previous planet/sun coordinates, during iteration
    glm::dvec3 tgecr_lt{ 0.0 };     // light time adjusted true geocentric ecliptical rectangular coordinates, output of light time iteration
    if (planet != A_SUN) {
        // Calculate the position of the earth first
        const glm::dvec3 earth0_rect{ Spherical::Spherical2Rectangular(AEarth::EclipticCoordinates(JD0, eph)) };
        // True Heliocentric ECliptical Spherical coordinates of the planet
        pla = EclipticCoordinates(JD0, planet, eph);
        details.thecs = pla;
        // True Geocentric ECliptical Rectangular coordinates of the planet
        details.tgecr = Spherical::Spherical2Rectangular(pla) - earth0_rect;  // OK!
        // Iterate to get Light Time corrected planet position
        bool bRecalc{ true };
        bool bFirst{ true };
        while (bRecalc) {
            //std::cout << "Calculating Planet Light Time compensation." << std::endl;
            if (!bFirst) pla = EclipticCoordinates(JD0, planet, eph); // For first run, use values calculated just above
            bFirst = false;
            bRecalc = ((fabs(pla.lon - pla_prev.lon) > prec_angle)
                    || (fabs(pla.lat - pla_prev.lat) > prec_angle)
                    || (fabs(pla.dst - pla_prev.dst) > prec_distance));
            pla_prev = pla;
            //Prepare for the next loop around
            if (bRecalc) JD0 = jd_tt - ACoord::DistanceToLightTime(glm::length(Spherical::Spherical2Rectangular(pla) - earth0_rect));
        }
        tgecr_lt = Spherical::Spherical2Rectangular(pla) - earth0_rect;
    }
    else { // planet == A_SUN
        // Note: True Heliocentric ECliptical Spherical coordinates are by definition 0.0,0.0,0.0
        // True Geocentric ECliptical Spherical coordinates
        pla = AEarth::EclipticCoordinates(JD0, eph);
        details.tgecr = -Spherical::Spherical2Rectangular(pla);  // Sun's Geocentric coordinates are the inverse of Earth's Heliocentric coords
        // Light Time compensated True Geocentric ECliptical Spherical coordinates
        bool bRecalc{ true };  // Prime the while() loop
        bool bFirst{ true };
        while (bRecalc) {
            if (!bFirst) pla = AEarth::EclipticCoordinates(JD0, eph);
            bFirst = false;
            bRecalc = ((fabs(pla.lon - pla_prev.lon) > prec_angle)
                    || (fabs(pla.lat - pla_prev.lat) > prec_angle)
                    || (fabs(pla.dst - pla_prev.dst) > prec_distance));
            pla_prev = pla;
            //Prepare for the next loop around
            if (bRecalc) JD0 = jd_tt - ACoord::DistanceToLightTime(pla.dst);
        }
        // Light Time compensated True Geocentric ECliptical Rectangular coordinates
        tgecr_lt = -Spherical::Spherical2Rectangular(pla);
    }
    // True Geocentric ECliptical Rectangular to True Geocentric ECliptical Spherical
    details.tgecs = Spherical::Rectangular2Spherical(details.tgecr);
    // True Light Time
    details.tlt = ACoord::DistanceToLightTime(details.tgecs.dst);
    // True -> Apparent Geocentric ECliptical Spherical (corrections below are all spherical)
    // Use True Geocentric ECliptic Rectangular to get True Geocentric ECliptic Spherical
    details.agecs = Spherical::Rectangular2Spherical(tgecr_lt);
    LLD aberration = AEarth::EclipticAberration(details.agecs.lon, details.agecs.lat, jd_tt, eph);
    details.agecs += FK5::CorrectionInLonLat(details.agecs, jd_tt);  // Convert to the FK5 system
    details.agecs += aberration; // Both aperration and FK5 correction are calculated from True Geocentric
    details.agecs.lon += AEarth::NutationInLongitude(jd_tt);
    details.alt = ACoord::DistanceToLightTime(details.agecs.dst);  // Apparent Light Time
    // Convert to RA and Dec
    const double epsilon = AEarth::TrueObliquityOfEcliptic(jd_tt);
    // True Geocentric Equatorial Spherical
    details.tgeqs = Spherical::Ecliptic2Equatorial(details.tgecs, epsilon);
    details.tgeqs.dst = details.tgecs.dst;  // True Geocentric Distance
    // Apparent Geocentric Equatorial Spherical
    details.ageqs = Spherical::Ecliptic2Equatorial(details.agecs, epsilon);
    details.ageqs.dst = details.agecs.dst;  // Apparent Geocentric Distance
    details.jd_tt = jd_tt;
    return details;
}
CelestialDetail Astronomy::getDetails(double jd_tt, Planet planet, Planetary_Ephemeris eph, unsigned int type) {
    CelestialDetail details;
    // Precision parameters for Light Time compensation
    const double prec_angle = 0.00001 * deg2rad;  // 0.00001 degrees = 36 mas (milli arc seconds)
    const double prec_distance = 0.000001;        // 0.000001 AU = 150 km (kilometers)
    // Iterate to find the positions adjusting for light-time correction if required
    double JD0{ jd_tt };            // jd modified during iteration
    LLD pla{};       // target planet/sun coordinates
    LLD pla_prev{};  // previous planet/sun coordinates, during iteration
    glm::dvec3 tgecr_lt{ 0.0 };     // light time adjusted true geocentric ecliptical rectangular coordinates, output of light time iteration
    if (planet != A_SUN) {
        // Calculate the position of the earth first
        const glm::dvec3 earth0_rect{ Spherical::Spherical2Rectangular(AEarth::EclipticCoordinates(JD0, eph)) };
        // True Heliocentric ECliptical Spherical coordinates of the planet
        pla = EclipticCoordinates(JD0, planet, eph);
        details.hec = pla;
        if (type == EC) return details;
        // Iterate to get Light Time corrected planet position
        bool bRecalc{ true };
        bool bFirst{ true };
        while (bRecalc) {
            //std::cout << "Calculating Planet Light Time compensation." << std::endl;
            if (!bFirst) pla = EclipticCoordinates(JD0, planet, eph); // For first run, use values calculated just above
            bFirst = false;
            bRecalc = ((fabs(pla.lon - pla_prev.lon) > prec_angle)
                || (fabs(pla.lat - pla_prev.lat) > prec_angle)
                || (fabs(pla.dst - pla_prev.dst) > prec_distance));
            pla_prev = pla;
            //Prepare for the next loop around
            if (bRecalc) JD0 = jd_tt - ACoord::DistanceToLightTime(glm::length(Spherical::Spherical2Rectangular(pla) - earth0_rect));
        }
        tgecr_lt = Spherical::Spherical2Rectangular(pla) - earth0_rect;
    }
    else { // planet == A_SUN
        // Note: True Heliocentric ECliptical Spherical coordinates are by definition 0.0,0.0,0.0
        if (type == EC) return details;  // Simply return pre-initialized zero coordinates of Sun
        // True Geocentric ECliptical Spherical coordinates
        pla = AEarth::EclipticCoordinates(JD0, eph);
        // Light Time compensated True Geocentric ECliptical Spherical coordinates
        bool bRecalc{ true };  // Prime the while() loop
        bool bFirst{ true };
        while (bRecalc) {
            if (!bFirst) pla = AEarth::EclipticCoordinates(JD0, eph);
            bFirst = false;
            bRecalc = ((fabs(pla.lon - pla_prev.lon) > prec_angle)
                || (fabs(pla.lat - pla_prev.lat) > prec_angle)
                || (fabs(pla.dst - pla_prev.dst) > prec_distance));
            pla_prev = pla;
            //Prepare for the next loop around
            if (bRecalc) JD0 = jd_tt - ACoord::DistanceToLightTime(pla.dst);
        }
        // Light Time compensated True Geocentric ECliptical Rectangular coordinates
        tgecr_lt = -Spherical::Spherical2Rectangular(pla);
    }
    // True -> Apparent Geocentric ECliptical Spherical (corrections below are all spherical)
    // Use True Geocentric ECliptic Rectangular to get True Geocentric ECliptic Spherical
    LLD agecs = Spherical::Rectangular2Spherical(tgecr_lt);
    LLD aberration = AEarth::EclipticAberration(agecs.lon, agecs.lat, jd_tt, eph);
    agecs += FK5::CorrectionInLonLat(agecs, jd_tt);  // Convert to the FK5 system
    agecs += aberration; // Both aperration and FK5 correction are calculated from True Geocentric
    agecs.lon += AEarth::NutationInLongitude(jd_tt);
    // Convert to RA and Dec
    const double epsilon = AEarth::TrueObliquityOfEcliptic(jd_tt);
    // Apparent Geocentric Equatorial Spherical
    details.geq = Spherical::Ecliptic2Equatorial(agecs, epsilon);
    details.geogha = ApparentGreenwichSiderealTime(EDateTime::getJDTT2UTC(jd_tt), true) - details.geq.lon;
    details.jd_tt = jd_tt;
    return details;
}
CelestialPath* Astronomy::getCelestialPath(Planet planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, bool fixed) {
    // Optional param fixed determines whether path will evolve with time or remain with initial values
    // Check if there is a matching CelestialPath already
    for (auto cp : cacheCP.m_Elements) {
        // Maybe upgrade type from ec to ecgeo if other params match
        if (cp->planet == planet && cp->jdStart == startoffset && cp->jdEnd == endoffset && cp->jdSteps == steps && cp->cpType == type && fixed == cp->fixedpath) {
            cp->m_refcnt++;
            //std::cout << "CelestialMech::getCelestialPath(): found matching CP for planet: " << cp->planet << "\n";
            return cp;
        }
    }
    // Otherwise create a new CelestialPath
    CelestialPath* path = new CelestialPath(this, planet, startoffset, endoffset, steps, type, fixed);
    path->index = cacheCP.store(path);
    //std::cout << "CelestialMech::getCelestialPath(): created new CP {" << path << "} for planet: " << path->planet << "\n";
    return path;
}
void Astronomy::removeCelestialPath(CelestialPath* path) {
    //std::cout << "CelestialMech::removeCelestialPath() ref count for planet " << path->planet << " is " << path->m_refcnt << "\n";
    path->m_refcnt--;
    if (path->m_refcnt > 0) return;
    //std::cout << "CelestialMech::removeCelestialPath(" << path << ") deleting CP for planet: " << path->planet << "\n";
    cacheCP.remove(path->index);
    delete path;
}
void Astronomy::updateCelestialPaths() {
    //if (cacheCP.empty()) return;
    for (auto cp : cacheCP.m_Elements) {
        if (!cp->fixedpath) cp->update();
    }
}
double Astronomy::getEcLat(Planet planet, double jd_tt, Planetary_Ephemeris eph) {
    // Heliocentric Ecliptic Latitude (ref. Equinox of Epoch) in radians
    if (planet == MERCURY) planet_ecLat[planet] = AMercury::EclipticLatitude(jd_tt, eph);
    else if (planet == A_VENUS) planet_ecLat[planet] = AVenus::EclipticLatitude(jd_tt, eph);
    else if (planet == A_EARTH) planet_ecLat[planet] = AEarth::EclipticLatitude(jd_tt, eph);
    else if (planet == A_MARS) planet_ecLat[planet] = AMars::EclipticLatitude(jd_tt, eph);
    else if (planet == A_JUPITER) planet_ecLat[planet] = AJupiter::EclipticLatitude(jd_tt, eph);
    else if (planet == A_SATURN) planet_ecLat[planet] = ASaturn::EclipticLatitude(jd_tt, eph);
    else if (planet == A_URANUS) planet_ecLat[planet] = AUranus::EclipticLatitude(jd_tt, eph);
    else if (planet == A_NEPTUNE) planet_ecLat[planet] = ANeptune::EclipticLatitude(jd_tt, eph);
    else std::cout << "APlanet::getEcLat(): planet unknown: " << planet << "\n";
    return planet_ecLat[planet];
}
double Astronomy::getEcLon(Planet planet, double jd_tt, Planetary_Ephemeris eph) {
    // Heliocentric Ecliptic Longitude (ref. Equinox of Epoch) in radians
    if (planet == MERCURY) planet_ecLon[planet] = AMercury::EclipticLongitude(jd_tt, eph);
    else if (planet == A_VENUS) planet_ecLon[planet] = AVenus::EclipticLongitude(jd_tt, eph);
    else if (planet == A_EARTH) planet_ecLon[planet] = AEarth::EclipticLongitude(jd_tt, eph);
    else if (planet == A_MARS) planet_ecLon[planet] = AMars::EclipticLongitude(jd_tt, eph);
    else if (planet == A_JUPITER) planet_ecLon[planet] = AJupiter::EclipticLongitude(jd_tt, eph);
    else if (planet == A_SATURN) planet_ecLon[planet] = ASaturn::EclipticLongitude(jd_tt, eph);
    else if (planet == A_URANUS) planet_ecLon[planet] = AUranus::EclipticLongitude(jd_tt, eph);
    else if (planet == A_NEPTUNE) planet_ecLon[planet] = ANeptune::EclipticLongitude(jd_tt, eph);
    else std::cout << "APlanet::getEcLon(): planet unknown: " << planet << "\n";
    return planet_ecLon[planet];
}
double Astronomy::getEcDst(Planet planet, double jd_tt, Planetary_Ephemeris eph) {
    // Heliocentric Ecliptic Radius (ref. Equinox of Epoch) in kilometers
    if (planet == MERCURY) planet_ecRadius[planet] = AMercury::EclipticDistance(jd_tt, eph);
    else if (planet == A_VENUS) planet_ecRadius[planet] = AVenus::EclipticDistance(jd_tt, eph);
    else if (planet == A_EARTH) planet_ecRadius[planet] = AEarth::EclipticDistance(jd_tt, eph);
    else if (planet == A_MARS) planet_ecRadius[planet] = AMars::EclipticDistance(jd_tt, eph);
    else if (planet == A_JUPITER) planet_ecRadius[planet] = AJupiter::EclipticDistance(jd_tt, eph);
    else if (planet == A_SATURN) planet_ecRadius[planet] = ASaturn::EclipticDistance(jd_tt, eph);
    else if (planet == A_URANUS) planet_ecRadius[planet] = AUranus::EclipticDistance(jd_tt, eph);
    else if (planet == A_NEPTUNE) planet_ecRadius[planet] = ANeptune::EclipticDistance(jd_tt, eph);
    else std::cout << "APlanet::getRadius(): planet unknown: " << planet << "\n";
    return planet_ecRadius[planet];
}

int Astronomy::getString2UnixTime(std::string& string) {
    // TODO: Implement like in Eartharium.cpp:TestArea5(), but resilient to missing seconds, and accepting either '/' or '-' date separator
    std::cout << "ERROR: Astronomy::getString2UnixTime() is not yet implemented.";
    return 0;
}

//  Privates
void Astronomy::updateGeocentric(Planet planet) {
    Planetary_Ephemeris eph{ EPH_VSOP87_SHORT };
    if (planet_jd[planet] == m_datetime.jd_tt()) return;  // Skip if time has not updated. Ideally we should never be called at all in that case.
    CelestialDetail details = getDetails(m_datetime.jd_tt(), planet, eph, ECGEO);
    planet_dec[planet] = details.geq.lat; // Geocentric Dec
    planet_ra[planet] = details.geq.lon;  // Geocentric RA
    planet_gha[planet] = details.geogha;  // Geocentric Greenwich HA
    planet_gha[planet] = ACoord::rangezero2tau(planet_gha[planet]);
    planet_jd[planet] = m_datetime.jd_tt();
    //planet_dst[planet] = au2km * details.ApparentGeocentricDistance;
    //std::cout << "Planet, RA, Dec, GSid: " << planet << ", " << planet_ra[planet] << ", " << planet_dec[planet] << ", " << m_gsidtime << "\n";
}
void Astronomy::updatePrecession() {
    // Precession parameters for J2000 - applied to all stars, so well worth caching
    // used in PrecessJ2000DecRA() and update()
    const double t = (m_datetime.jd_tt() - JD_2000) / 36525;
    const double tsquared = t * t;
    const double tcubed = tsquared * t;
    prec_j2000_sigma = ACoord::dms2rad(0, 0, (2306.2181) * t + (0.30188) * tsquared + 0.017998 * tcubed);
    prec_j2000_zeta  = ACoord::dms2rad(0, 0, (2306.2181) * t + (1.09468) * tsquared + 0.018203 * tcubed);
    prec_j2000_phi   = ACoord::dms2rad(0, 0, (2004.3109) * t - (0.42665) * tsquared - 0.041833 * tcubed);
    prec_j2000_phi_s = sin(prec_j2000_phi);
    prec_j2000_phi_c = cos(prec_j2000_phi);
}
void Astronomy::update() {  // Updates cache values for common parameters
    m_meanobliquity = AEarth::MeanObliquityOfEcliptic(m_datetime.jd_tt());
    m_trueobliquity = m_meanobliquity + AEarth::NutationInObliquity(m_datetime.jd_tt());
    m_nutationinlongitude = AEarth::NutationInLongitude(m_datetime.jd_tt());
    m_meangsid = AEarth::MeanGreenwichSiderealTime(m_datetime.jd_utc());
    m_gsidtime = m_meangsid + (m_nutationinlongitude * cos(m_trueobliquity));
    updatePrecession();
    updateTimeString();

    // Update cached planetary positions to current time. Later, include Sun, Moon & Earth?
    for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
        if (planet_refcnt[p] > 0) updateGeocentric((Planet)p);
    }
    // Update cached planetary paths
    updateCelestialPaths();
}

