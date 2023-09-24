
#include <array>
#include <string>
#include <chrono>
//#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

#include "AAplus/AAAberration.h"     // For CAAAberration::EarthVelocity
#include "AAplus/AADynamicalTime.h"
#include "AAplus/AAEquationOfTime.h"

#include "config.h"
#include "Astronomy.h"

#include "astronomy/datetime.h"

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
CelestialPath::CelestialPath(Astronomy* celmec, size_t planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, bool fixed)
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
    if (fixedpath && !force) return;
    double currentjd = m_astro->getJD_TT();
    if (m_jdCurrent == currentjd) return;
    entries.clear();
    for (double jd = currentjd+jdStart; jd < currentjd+jdEnd; jd += m_stepsize) {
        CelestialDetail detail = m_astro->getDetails(jd, planet, cpType);
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
    //stellarobject* so{ NO_DOUBLE, NO_DOUBLE, NO_DOUBLE, NO_DOUBLE, NO_DOUBLE, NO_DOUBLE, NO_DOUBLE, NO_DOUBLE, "none" }; // retval
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
LLH Astronomy::getDecRAbyName(const std::string starname, bool rad) {
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
            return { rad ? deg2rad * s.dec : s.dec, rad ? deg2rad * s.ra : s.ra, 0.0 }; // File was loaded in degrees
        }
    }
    std::cout << "ERROR! Astronomy::getDecRAbyName() - Unknown name supplied: " << starname << '\n';
    return { 0.0, 0.0, NO_DOUBLE };
}
LLH Astronomy::getDecRAwithPMbyName(const std::string starname, double jd_tt, bool rad) {
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
            // apply Proper Motion
            double elapsedyears = (jd_tt - JD_2000) / 365.25;
            //std::cout << "Proper Motion in " << elapsedyears << '\n';
            double dec = s.dec + s.pm_dec * elapsedyears / 3600000.0;
            // NOTE: SIMBAD proper motions are from the Hipparcos mission, the right ascension value is already multiplied by cos(s.dec)
            //double ra = s.ra + s.pm_ra / cos(deg2rad * s.dec) * elapsedyears / 3600000.0;
            double ra = s.ra + s.pm_ra * elapsedyears / 3600000.0;
            //std::cout << "On date Ra Dec of " << starname << ": " << ra << ", " << dec << '\n';
            return { rad ? deg2rad * dec : dec, rad ? deg2rad * ra : ra, 0.0 }; // File was loaded in degrees
        }
    }
    std::cout << "ERROR! Astronomy::getDecRAwithPMbyName() - Unknown name supplied: " << starname << '\n';
    return { 0.0, 0.0, NO_DOUBLE };
}
LLH Astronomy::applyProperMotionJ2000(double jd_tt, LLH decra, LLH propermotion) {
    // apply Proper Motion - decra is in radians, propermotion is in milliarcseconds, from stellarobject db.
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    double elapsedyears = (jd_tt - JD_2000) / 365.25;
    decra.lat += propermotion.lat * elapsedyears / 3600000.0;
    decra.lon += propermotion.lon * elapsedyears / 3600000.0;
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
        constellations.back().center = { 0.0, 0.0, 0.0 };
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
                c.outline.emplace_back(LLH{ ra, dec, 0.0 });
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
    if (do_eot && eot == 0.0) eot = CAAEquationOfTime::Calculate(m_datetime.jd_tt(), true);
    if (eot != 0.0 && do_eot) min += eot; // If EoT was applied last time update, remove it.
    //std::cout << "Astronomy::addTime() - Remove EoT: " << eot << '\n';
    if (!do_eot) eot = 0.0;
    m_datetime.addTime(0, 0, d, h, min, sec);
    if (do_eot) {
        eot = CAAEquationOfTime::Calculate(m_datetime.jd_tt(), true);
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
bool Astronomy::isLeapYear(double year) {
    // Can be removed when unix time functions have been moved to EDateTime
    return m_datetime.isLeapYear((long)year);
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
    std::cout << " - Julian Date (TT):  " << m_datetime.jd_tt() << '\n';
    std::cout << " - Julian Date (UTC): " << m_datetime.jd_utc() << '\n';
}

double Astronomy::ApparentGreenwichSiderealTime(double jd_utc, bool rad) noexcept {
    // MEEUS98: Chapter 12
    // AA+: CAASidereal::ApparentGreenwichSiderealTime(JD)
    if (jd_utc == NO_DOUBLE) jd_utc = getJD_UTC();     // cache friendly time
    double meanobliquity = MeanObliquityOfEcliptic(jd_utc, true);
    double trueobliquity = meanobliquity + (NutationInObliquity(jd_utc, true));
    double nutationinlongitude = NutationInLongitude(jd_utc, true);
    double meangsid = MeanGreenwichSiderealTime(jd_utc, true);
    double gsidtime = meangsid + (nutationinlongitude * cos(trueobliquity));
    return rad ? gsidtime : rad2deg * gsidtime;
}
double Astronomy::MeanGreenwichSiderealTime(double jd_utc, bool rad) noexcept {
    // MEEUS98: Chapter 12
    // AA+: CAASidereal::MeanGreenwichSiderealTime(JD)
    //Get the Julian day for the same day at midnight
    //EDateTime date(jd_utc);
    //EDateTime midnight(date.year(), date.month(), date.day(), 0.0, 0.0, 0.0);
    // NOTE: The above is exactly equivalent to:
    double jd_fraction = jd_utc - std::floor(jd_utc) + 0.5; //also good for negative jd
    double jd_midnight = jd_utc - jd_fraction;
    //Calculate the sidereal time at midnight
    // Equation 12.1
    //double T{ (midnight.jd_utc() - JD_2000) / 36525.0 };
    double T{ (jd_midnight - JD_2000) / 36525.0 };
    double TSquared{ T * T };
    double TCubed{ TSquared * T };
    // Equation 12.3 - Value is in degrees, only valid for JDs at midnight
    double Value{ 100.46061837 + (36000.770053608 * T) + (0.000387933 * TSquared) - (TCubed / 38710000) };

    //Adjust by the time of day - h, m, s converted to degrees times the constant suggested by Meeus
    //Value += (((date.hour() * 15.0) + (date.minute() * 0.25) + (date.second() * 0.0041666666666666666666666666666667)) * 1.00273790935);
    Value += jd_fraction * 24.0 * 15.0 * 1.00273790935; // Perfect

    // Alternative that can accept fractional UT JD, but apparently requires a machine with a "sufficient number of digits"
    // A quick test reveals differences from 12.3 above in 9th decimal place which is 1/10000 of a second.
    // If above turns out to be slow (due to the use of EDateTime), this is a reasonable alternative.
    // Meeus Equation 12.1 and 12.4 -> gives Value in degrees
    //const double T = (jd_utc - JD_2000) / 36525.0;
    //const double TSquared = T * T;
    //const double TCubed = TSquared * T;
    //const double Value = 280.46061837 + 360.98564736629 * (jd_utc - JD_2000) + 0.000387933 * TSquared - TCubed / 38710000;

    return rad ? rangezero2tau(deg2rad * Value) : rangezero2threesixty(Value);
    // Tested, and 12.3 yields exact same result as AA+ (not odd, as that is where I got the code):
    // Test Mean Greenwich Sidereal Time against AA+
    //for (long i = 2023; i > -6000; i -= 100) {
    //    astro->setTime(i, 9, 24.0, 23.0, 27.0, 56.0);
    //    std::cout << "Year: " << i << "\n";
    //    std::cout << CAASidereal::MeanGreenwichSiderealTime(astro->getJD_UTC()) << '\n';
    //    std::cout << rad2hrs * astro->MeanGreenwichSiderealTime(astro->getJD_UTC(), true) << '\n';
    //    std::cout << deg2hrs * astro->MeanGreenwichSiderealTime(astro->getJD_UTC()) << "\n\n";
    //}

}
double Astronomy::getEoT(double jd_tt) {  // Expects JD in TT
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    return CAAEquationOfTime::Calculate(jd_tt, true);
}

// Coordinate Transformations
LLH Astronomy::calcEc2Geo(double Beta, double Lambda, double Epsilon, bool rad) noexcept {
    if (!rad) {
        Beta *= deg2rad;
        Lambda *= deg2rad;
        Epsilon *= deg2rad;
    }
    LLH Equatorial;
    Equatorial.lon = rangezero2tau(atan2(sin(Lambda) * cos(Epsilon) - tan(Beta) * sin(Epsilon), cos(Lambda)));
    Equatorial.lat = asin(sin(Beta) * cos(Epsilon) + cos(Beta) * sin(Epsilon) * sin(Lambda));
    if (!rad) {
        Equatorial.lon *= rad2deg;
        Equatorial.lat *= rad2deg;
    }
    return Equatorial;
}
LLH Astronomy::calcGeo2Ec(double Delta, double Alpha, double Epsilon, bool rad) noexcept {
    // AA+: CAACoordinateTransformation::Equatorial2Ecliptic()
    // MEEUS98: This refers to algorithm 13.1 and 13.2 on page 93
    if (!rad) {
        Delta *= deg2rad;
        Alpha *= deg2rad;
        Epsilon *= deg2rad;
    }
    LLH Ecliptic;
    Ecliptic.lon = rangezero2tau(atan2(sin(Alpha) * cos(Epsilon) + tan(Delta) * sin(Epsilon), cos(Alpha)));
    Ecliptic.lat = asin(sin(Delta) * cos(Epsilon) - cos(Delta) * sin(Epsilon) * sin(Alpha));
    if (!rad) {
        Ecliptic.lon *= rad2deg;
        Ecliptic.lat *= rad2deg;
    }
    return Ecliptic;
}
LLH Astronomy::calcGeo2Topo(LLH pos, LLH loc) {
    // AA+: CAACoordinateTransformation::Equatorial2Horizontal()
    // TODO: Does NOT account for altitude of observer !!! (maybe doesn't matter, up is still up? Mountains are not high enough for parallax)
    // pos is DecGHA, loc is LatLon
    // NOTE: No caching, not likely to be called with same position twice for same JD
    double LocalHourAngle = pos.lon + loc.lon;
    // from CAACoordinateTransformation::Equatorial2Horizontal(rad2hrs * LocalHourAngle, rad2deg * pos.lat, rad2deg * loc.lat);
    LLH topo;
    topo.lon = atan2(sin(LocalHourAngle), cos(LocalHourAngle) * sin(loc.lat) - tan(pos.lat) * cos(loc.lat));
    topo.lat = asin(sin(loc.lat) * sin(pos.lat) + cos(loc.lat) * cos(pos.lat) * cos(LocalHourAngle));
    return topo;
}
LLH Astronomy::calcDecHA2GP(LLH decra, bool rad) {
    // Map HA [0 ; pi) -> [0 ; -pi) and [pi ; tau) -> [pi ; 0)
    // !!! FIX: This assumes planetocentric coordinates (longitude is east-from-north) Make option for west-from-south !!!
    LLH res = decra;
    if (!rad) {
        res.lat *= deg2rad;
        res.lon *= deg2rad;
    }
    if (res.lon < pi) res.lon = -res.lon;
    else res.lon = tau - res.lon;  // res.lon is now -pi to pi east of south (or east of north?)
    res.lon = clampmPitoPi(res.lon);
    if (!rad) {
        res.lat *= rad2deg;
        res.lon *= rad2deg;
    }
    return res;
}
LLH Astronomy::calcDecRA2GP(LLH decra, double jd_utc, bool rad) { // Needs JD as UTC
    if (jd_utc == NO_DOUBLE) // jd was not specified, so go with current time
        return calcDecHA2GP({ decra.lat, rad ? (m_gsidtime - decra.lon) : (rad2deg * m_gsidtime - decra.lon), 0.0 }, rad);
    return calcDecHA2GP({ decra.lat, rad ? (ApparentGreenwichSiderealTime(jd_utc) - decra.lon) : (rad2deg * ApparentGreenwichSiderealTime(jd_utc) - decra.lon), 0.0 }, rad);
}
LLH Astronomy::getDecRA(size_t planet, double jd_tt, bool rad) {  // JD in TT
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    if (jd_tt == planet_jd[planet]) return { planet_dec[planet], planet_ra[planet], 0.0 };
    CelestialDetail details = getDetails(jd_tt, planet, ECGEO);
    if (!rad) return { rad2deg * details.geq.lat, rad2deg * details.geq.lon, 0.0 }; // Dec, GHA
    else return { details.geq.lat, details.geq.lon, 0.0 };
}
LLH Astronomy::getDecGHA(size_t planet, double jd_tt, bool rad) { // JD in TT
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    if (jd_tt == planet_jd[planet]) return { planet_dec[planet], planet_gha[planet], 0.0 };
    CelestialDetail details = getDetails(jd_tt, planet, ECGEO);
    if (!rad) return { rad2deg * details.geq.lat, rad2deg * details.geogha, 0.0 }; // Dec, GHA
    else return { details.geq.lat, details.geogha, 0.0 };
}

// General astronomical adjustments
LLH Astronomy::EclipticAberration(double Beta, double Lambda, double jd_tt, bool rad) {
    // From CAAAberration::EclipticAberration() converted to accept radians
    // See MEEUS98 Chapter 23 for Ron-Vondrák abberation
    double lambda = Lambda;
    double beta = Beta;
    if (!rad) {
        lambda *= deg2rad;
        beta *= deg2rad;
    }
    const double T = (jd_tt - JD_2000) / 36525;
    const double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);
    double pi = deg2rad * (102.93735 + T * (1.71946 + 0.00046 * T));
    constexpr double k = 20.49552;
    double SunLongitude = rangezero2tau(EcLonEarth(jd_tt) + (tau * 0.5));

    LLH aberration;
    aberration.lon = (-k * cos(SunLongitude - lambda) + e * k * cos(pi - lambda)) / cos(beta) / 3600;
    aberration.lat = -k * sin(Beta) * (sin(SunLongitude - lambda) - e * sin(pi - lambda)) / 3600;
    if (rad) {
        aberration.lon *= deg2rad;
        aberration.lat *= deg2rad;
    }
    return aberration;
}
LLH Astronomy::EquatorialAberration(double dec, double ra, double jd_tt, bool rad) {
    // See MEEUS98 Chapter 23 for Ron-Vondrák abberation
    double Dec = dec;
    double Ra = ra;
    if (rad) {
        Dec *= rad2deg;
        Ra *= rad2deg;
    }
    double cosDelta = cos(Dec);
    double sinDelta = sin(Dec);
    double cosAlpha = cos(Ra);
    double sinAlpha = sin(Ra);
    const CAA3DCoordinate velocity = CAAAberration::EarthVelocity(jd_tt, true); // high precision
    LLH aberration;
    aberration.lat = -(((velocity.X * cosAlpha + velocity.Y * sinAlpha) * sinDelta - velocity.Z * cosDelta) / 17314463350.0);
    aberration.lon = (velocity.Y * cosAlpha - velocity.X * sinAlpha) / (17314463350.0 * cosDelta);
    if (rad) {
        aberration.lat *= deg2rad;
        aberration.lon *= deg2rad;
    }
    return aberration;
    // retval in radians
}
LLH Astronomy::FK5Correction(double Latitude, double Longitude, double jd_tt, bool rad) {
    double lat = Latitude;
    double lon = Longitude;
    if (rad) {
        lat *= rad2deg;
        lon *= rad2deg;
    }
    const double T = (jd_tt - JD_2000) / 36525;
    double Ldash = lon - deg2rad * T * (1.397 + 0.00031 * T);
    LLH fk5corr;
    const double latcor = 0.03916 * (std::cos(Ldash) - std::sin(Ldash));
    fk5corr.lat = dms2deg(0, 0, latcor);
    const double loncor = -0.09033 + 0.03916 * (std::cos(Ldash) + std::sin(Ldash)) * tan(lat);
    fk5corr.lon = dms2deg(0, 0, loncor);
    if (rad) {
        fk5corr.lon *= deg2rad;
        fk5corr.lat *= deg2rad;
    }
    return fk5corr;
}
LLH Astronomy::PrecessDecRA(const LLH decra, const double jd_tt, const double JD0) {  // ALWAYS in RADIANS
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
    double JD = jd_tt == NO_DOUBLE ? getJD_TT() : jd_tt;
    const double T = (JD0 - JD_2000) / 36525;
    const double Tsquared = T * T;
    const double t = (JD - JD0) / 36525;
    const double tsquared = t * t;
    const double tcubed = tsquared * t;
    const double sigma = deg2rad * dms2deg(0, 0, (2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (0.30188 - 0.000344 * T) * tsquared + 0.017998 * tcubed);
    const double zeta = deg2rad * dms2deg(0, 0, (2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (1.09468 + 0.000066 * T) * tsquared + 0.018203 * tcubed);
    const double phi = deg2rad * dms2deg(0, 0, (2004.3109 - 0.8533 * T - 0.000217 * Tsquared) * t - (0.42665 + 0.000217 * T) * tsquared - 0.041833 * tcubed);
    const double A = cos(decra.lat) * sin(decra.lon + sigma);
    const double B = cos(phi) * cos(decra.lat) * cos(decra.lon + sigma) - sin(phi) * sin(decra.lat);
    const double C = sin(phi) * cos(decra.lat) * cos(decra.lon + sigma) + cos(phi) * sin(decra.lat);
    // if debugging, create printable value
    //LLH value;
    //value.lon = atan2(A, B) + zeta;
    //std::cout << "JD: " << JD << "Ra Dec : " << decra.lon << ", " << decra.lat << "->" << value.lon << ", " << value.lat << '\n';
    //value.lat = asin(C);
    //return value;
    return { asin(C), atan2(A, B) + zeta, 0.0 };
}
LLH Astronomy::PrecessJ2000DecRA(const LLH decra, const double jd_tt) {  // ALWAYS in RADIANS
    // Precess the Equinox for a geocentric coordinate (catalogue with proper motion applied)
    // AA+: CAAPrecession::PrecessEquatorial()
    // MEEUS92: Chapter 21
    // (Does NOT abberate)
    // jd is desired Julian Day, default to current JD in relevant Astronomy object
    double JD = jd_tt == NO_DOUBLE ? getJD_TT() : jd_tt;
    if (JD == getJD_TT()) {  // If current time, use cached values
        const double A = cos(decra.lat) * sin(decra.lon + prec_j2000_sigma);
        const double B = prec_j2000_phi_c * cos(decra.lat) * cos(decra.lon + prec_j2000_sigma) - prec_j2000_phi_s * sin(decra.lat);
        const double C = prec_j2000_phi_s * cos(decra.lat) * cos(decra.lon + prec_j2000_sigma) + prec_j2000_phi_c * sin(decra.lat);
        return { asin(C), atan2(A, B) + prec_j2000_zeta, 0.0 };
    }
    else {
        const double t = (JD - JD_2000) / 36525;
        const double tsquared = t * t;
        const double tcubed = tsquared * t;
        const double sigma = deg2rad * dms2deg(0, 0, (2306.2181) * t + (0.30188) * tsquared + 0.017998 * tcubed);
        const double zeta = deg2rad * dms2deg(0, 0, (2306.2181) * t + (1.09468) * tsquared + 0.018203 * tcubed);
        const double phi = deg2rad * dms2deg(0, 0, (2004.3109) * t - (0.42665) * tsquared - 0.041833 * tcubed);
        // The above can be calculated once per time update and cached - done.
        const double A = cos(decra.lat) * sin(decra.lon + sigma);
        const double B = cos(phi) * cos(decra.lat) * cos(decra.lon + sigma) - sin(phi) * sin(decra.lat);
        const double C = sin(phi) * cos(decra.lat) * cos(decra.lon + sigma) + cos(phi) * sin(decra.lat);
        return { asin(C), atan2(A, B) + zeta, 0.0 };
    }
}
double Astronomy::MeanObliquityOfEcliptic(double jd_tt, bool rad) {
    // AA+: CAANutation::MeanObliquityOfEcliptic(JD)
    // MEEUS98: Chapter 22
    // Calculates mean obliquity using the following:
    // This formula is given in https://en.wikipedia.org/wiki/Axial_tilt credited to J.Laskar 1986
    // Original Source:  http://articles.adsabs.harvard.edu/pdf/1986A%26A...157...59L
    // Original Erratum: http://articles.adsabs.harvard.edu/pdf/1986A%26A...164..437L
    // Good to 0.02" over 1000 years, several arc seconds over 10000 years (around J2000.0)
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    const double U = (jd_tt - JD_2000) / 3652500;  // U is JD in deca millenia, from J2000.0
    const double Usquared = U * U;
    const double Ucubed = Usquared * U;
    const double U4 = Ucubed * U;
    const double U5 = U4 * U;
    const double U6 = U5 * U;
    const double U7 = U6 * U;
    const double U8 = U7 * U;
    const double U9 = U8 * U;
    const double U10 = U9 * U;
    // Could roll up these constants for speed
    double retval = dms2deg(23, 26, 21.448)
        - (secs2deg(4680.93) * U)
        - (secs2deg(1.55) * Usquared)
        + (secs2deg(1999.25) * Ucubed)
        - (secs2deg(51.38) * U4)
        - (secs2deg(249.67) * U5)
        - (secs2deg(39.05) * U6)
        + (secs2deg(7.12) * U7)
        + (secs2deg(27.87) * U8)
        + (secs2deg(5.79) * U9)
        + (secs2deg(2.45) * U10);
    return rad ? deg2rad * retval : retval;
}
double Astronomy::TrueObliquityOfEcliptic(double jd_tt, bool rad) {
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    return MeanObliquityOfEcliptic(jd_tt, rad) + NutationInObliquity(jd_tt, rad);
}
double Astronomy::NutationInObliquity(double jd_tt, bool rad) {
    // Nutation in Obliquity from AA+ v2.30
    // NOTE: Different from NutationInLongitude(), here we use the COSINES of the nutation table!
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    const double T = (jd_tt - JD_2000) / 36525;
    const double Tsquared = T * T;
    const double Tcubed = Tsquared * T;
    double D = 297.85036 + (445267.111480 * T) - (0.0019142 * Tsquared) + (Tcubed / 189474);
    D = rangezero2threesixty(D);
    double M = 357.52772 + (35999.050340 * T) - (0.0001603 * Tsquared) - (Tcubed / 300000);
    M = rangezero2threesixty(M);
    double Mprime = 134.96298 + (477198.867398 * T) + (0.0086972 * Tsquared) + (Tcubed / 56250);
    Mprime = rangezero2threesixty(Mprime);
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * Tsquared) + (Tcubed / 327270);
    F = rangezero2threesixty(F);
    double omega = 125.04452 - (1934.136261 * T) + (0.0020708 * Tsquared) + (Tcubed / 450000);
    omega = rangezero2threesixty(omega);
    double value = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        argument *= deg2rad;
        value += (coeff.coscoeff1 + (coeff.coscoeff2 * T)) * cos(argument) * 0.0001;
    }
    return rad ? deg2rad * value / 3600.0 : value / 3600.0;
}
double Astronomy::NutationInLongitude(double jd_tt, bool rad) {
    // AA+: CAANutation::NutationInLongitude(JD)
    // MEEUS98: Chapter 22
    // NOTE: Different from NutationInObliquity(), here we use the SINES of the nutation table!
    //       So only the for loop differs. Perhaps refactor/cache?
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    const double T = (jd_tt - JD_2000) / 36525;
    const double Tsquared = T * T;
    const double Tcubed = Tsquared * T;
    // Mean Elongation of the Moon from the Sun
    double D = rangezero2threesixty(297.85036 + (445267.111480 * T) - (0.0019142 * Tsquared) + (Tcubed / 189474));
    // Mean Anomaly of the Sun (Earth)
    double M = rangezero2threesixty(357.52772 + (35999.050340 * T) - (0.0001603 * Tsquared) - (Tcubed / 300000));
    // Mean Anomaly of the Moon
    double Mprime = rangezero2threesixty(134.96298 + (477198.867398 * T) + (0.0086972 * Tsquared) + (Tcubed / 56250));
    // Moon's Argument of Latitude
    double F = rangezero2threesixty(93.27191 + (483202.017538 * T) - (0.0036825 * Tsquared) + (Tcubed / 327270));
    // Longitude of the Ascending Node of the Moon's Mean Orbit on the Ecliptic, measured from the Mean Equinox of the Date
    double omega = rangezero2threesixty(125.04452 - (1934.136261 * T) + (0.0020708 * Tsquared) + (Tcubed / 450000));
   double value = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        const double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        //const double radargument = CAACoordinateTransformation::DegreesToRadians(argument);
        value += (coeff.sincoeff1 + (coeff.sincoeff2 * T)) * sin(deg2rad * argument) * 0.0001;
    }
    return rad ? deg2rad * value / 3600.0 : value / 3600.0;
    // retval is in degrees by default
}

double Astronomy::NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl, bool rad) {
    if (!rad) {
        ra *= deg2rad;
        obliq *= deg2rad;
        nut_lon *= deg2rad;
        nut_obl *= deg2rad;
    }
    return rad ? (sin(obliq) * cos(ra) * nut_lon + sin(ra) * nut_obl)
        : rad2deg * (sin(obliq) * cos(ra) * nut_lon + sin(ra) * nut_obl);
}
double Astronomy::NutationInRightAscension(double dec, double ra, double obliq, double nut_lon, double nut_obl, bool rad) {
    if (!rad) {
        dec *= deg2rad;
        ra *= deg2rad;
        obliq *= deg2rad;
        nut_lon *= deg2rad;
        nut_obl *= deg2rad;
    }
    return rad ? (((cos(obliq) + (sin(obliq) * sin(ra) * tan(dec))) * nut_lon) - (cos(ra) * tan(dec) * nut_obl))
        : rad2deg * (((cos(obliq) + (sin(obliq) * sin(ra) * tan(dec))) * nut_lon) - (cos(ra) * tan(dec) * nut_obl));
}
// Stellar Earth Centered Equatorial
LLH Astronomy::getTrueDecRAbyName(const std::string starname, double jd_tt, bool rad) {
    // Apply Proper Motion
    // (Note: see https://stargazerslounge.com/topic/289480-calculating-ra-and-decl-of-a-star/ which I read after implementing below)
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    LLH decra = getDecRAwithPMbyName(starname, m_datetime.jd_tt(), true); // Always calculate from radians, convert at end to what bool rad indicates
    //std::cout << "Astronomy::getTrueDecRAbyName(): Catalogue RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    // Apply Precession
    double JD0 = EDateTime::getJDUTC2TT(JD_2000); // Epoch 2000.0 - Same as epoch of the catalogue
    const double T = (JD0 - JD_2000) / 36525;
    const double Tsquared = T * T;
    const double t = (jd_tt - JD0) / 36525;
    const double tsquared = t * t;
    const double tcubed = tsquared * t;
    // Precession parameters sigma, zeta & phi in arc seconds
    // NOTE: Since the Epoch of the star database is J2000, T and Tsquared are zero, so this could be simplified.
    //       The -O3 compiler flag probably optimizes this?
    const double sigma = deg2rad * ((2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (0.30188 - 0.000344 * T) * tsquared + 0.017998 * tcubed) / 3600.0;
    const double zeta = deg2rad * ((2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (1.09468 + 0.000066 * T) * tsquared + 0.018203 * tcubed) / 3600.0;
    const double phi = deg2rad * ((2004.3109 - 0.8533 * T - 0.000217 * Tsquared) * t - (0.42665 + 0.000217 * T) * tsquared - 0.041833 * tcubed) / 3600.0;
    const double A = cos(decra.lat) * sin(decra.lon + sigma);
    const double B = cos(phi) * cos(decra.lat) * cos(decra.lon + sigma) - sin(phi) * sin(decra.lat);
    const double C = sin(phi) * cos(decra.lat) * cos(decra.lon + sigma) + cos(phi) * sin(decra.lat);
    // Precessed decra in radians - FIX: !!! WRONG RA VALUE, Dec is OK !!!
    decra.lon = atan2(A, B) + zeta;
    decra.lat = asin(C); // If star is close to pole, this should ideally use acos(sqrt(A*A+B*B));
    //std::cout << "Astronomy::getTrueDecRAbyName(): Precessed RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    // Apply nutation
    double obliq = MeanObliquityOfEcliptic(jd_tt, true);
    double nut_lon = NutationInLongitude(jd_tt, true);
    double nut_obl = NutationInObliquity(jd_tt, true);
    double nut_dec = NutationInDeclination(decra.lon, obliq, nut_lon, nut_obl, true);
    double nut_ra = NutationInRightAscension(decra.lat, decra.lon, obliq, nut_lon, nut_obl, true);
    //std::cout << " Calculated obliquity:       " << angle2DMSstring(obliq, true) << '\n';
    //std::cout << " Calculated nutation in lon: " << angle2DMSstring(nut_lon, true) << '\n';
    //std::cout << " Calculated nutation in obl: " << angle2DMSstring(nut_obl, true) << '\n';
    //std::cout << " Calculated nutation in RA:  " << angle2DMSstring(nut_ra, true) << '\n';
    //std::cout << " Calculated nutation in Dec: " << angle2DMSstring(nut_dec, true) << '\n';
    decra.lat += nut_dec;
    decra.lon += nut_ra;
    //std::cout << "Astronomy::getTrueDecRAbyNameJD(): Nutated RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    // Apply aberration
    //CAA2DCoordinate aberration = EquatorialAberration(decra.lon, decra.lat, m_jd, true);
    LLH aberration = EquatorialAberration(decra.lat, decra.lon, m_datetime.jd_tt(), true);
    decra.lat += aberration.lat;
    decra.lon += aberration.lon;
    //std::cout << "Astronomy::getTrueDecRAbyNameJD(): Aberrated RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    // Ignore annual parallax
    if (!rad) {
        decra.lat *= rad2deg;
        decra.lon *= rad2deg;
    }
    return decra;
}


// Planetary calculations
unsigned int Astronomy::enablePlanet(size_t planet) {
    return ++planet_refcnt[planet];
}
unsigned int Astronomy::disablePlanet(size_t planet) {
    return --planet_refcnt[planet];
}
CelestialDetail Astronomy::getDetails(double jd_tt, size_t planet, unsigned int type) {
    CelestialDetail details;
    details.jd_tt = jd_tt;
    //Calculate the position of the earth first
    double JD0 = jd_tt;
    const double L0 = EcLonEarth(JD0);  // Radians
    const double B0 = EcLatEarth(JD0);  // Radians
    const double R0 = EcDstEarth(JD0);  // Astronomical Units
    const double cosB0 = cos(B0);
    //Iterate to find the positions adjusting for light-time correction if required
    double L = 0;
    double B = 0;
    double R = 0;
    if (planet != SUN) {
        bool bRecalc = true;
        bool bFirstRecalc = true;
        double LPrevious = 0;
        double BPrevious = 0;
        double RPrevious = 0;
        while (bRecalc) {
            L = getEcLon(planet, JD0);                   // Radians
            B = getEcLat(planet, JD0);                   // Radians
            R = getRadius(planet, JD0, /* km */ false);  // Astronomical Units
            if (!bFirstRecalc) {
                bRecalc = ((fabs(L - LPrevious) > deg2rad * 0.00001) || (fabs(B - BPrevious) > deg2rad * 0.00001) || (fabs(R - RPrevious) > 0.000001));
                LPrevious = L;
                BPrevious = B;
                RPrevious = R;
            }
            else {
                details.hec.lon = L;
                details.hec.lat = B;
                details.hec.dst = R;
                bFirstRecalc = false;
                if (type == EC) return details;  // Only Heliocentric Ecliptic coordinates
            }
            //Calculate the new value
            if (bRecalc) {
                const double cosB = cos(B);
                const double cosL = cos(L);
                const double x = R * cosB * cosL - R0 * cosB0 * cos(L0);
                const double y = R * cosB * sin(L) - R0 * cosB0 * sin(L0);
                const double z = R * sin(B) - R0 * sin(B0);
                const double distance = sqrt(x * x + y * y + z * z);

                //Prepare for the next loop around
                JD0 = jd_tt - Distance2LightTime(distance);
            }
        }
    }
    double x = 0;
    double y = 0;
    double z = 0;
    if (planet != SUN) {
        const double cosB = cos(B);
        const double cosL = cos(L);
        x = R * cosB * cosL - R0 * cosB0 * cos(L0);
        y = R * cosB * sin(L) - R0 * cosB0 * sin(L0);
        z = R * sin(B) - R0 * sin(B0);
    }
    else {
        x = -R0 * cosB0 * cos(L0);
        y = -R0 * cosB0 * sin(L0);
        z = -R0 * sin(B0);
    }
    const double x2 = x * x;
    const double y2 = y * y;
    double appGeoLat = atan2(z, sqrt(x2 + y2));
    double appGeoLon = rangezero2tau(atan2(y, x));  // atan2() returns [-pi;+pi]
    double appGeoDst = sqrt(x2 + y2 + z * z);
    //details.ApparentLightTime = CAAElliptical::DistanceToLightTime(appGeoDst);
    //Adjust for Aberration
    const double T = (jd_tt - 2451545) / 36525;
    const double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);
    double pi = deg2rad * (102.93735 + T * (1.71946 + 0.00046 * T));
    constexpr double k = 20.49552;
    double SunLongitude = rangezero2tau(EcLonEarth(jd_tt) + (tau * 0.5));
    const double aberrationLon = (-k * cos(SunLongitude - appGeoLon) + e * k * cos(pi - appGeoLon)) / cos(appGeoLat) / 3600;
    const double aberrationLat = -k * sin(appGeoLat) * (sin(SunLongitude - appGeoLon) - e * sin(pi - appGeoLon)) / 3600;
    appGeoLon += deg2rad * aberrationLon;
    appGeoLat += deg2rad * aberrationLat;
    //convert to the FK5 system - Now takes radians
    double Ldash = appGeoLon - deg2rad * T * (1.397 + 0.00031 * T);
    const double fk5corrLon = secs2deg(-0.09033 + 0.03916 * (std::cos(Ldash) + std::sin(Ldash)) * tan(appGeoLat));
    const double fk5corrLat = secs2deg(0.03916 * (std::cos(Ldash) - std::sin(Ldash)));
    appGeoLon += deg2rad * fk5corrLon;
    appGeoLat += deg2rad * fk5corrLat;
    //Correct for Nutation in Longitude
    const double Tsquared = T * T;
    const double Tcubed = Tsquared * T;
    double D = 297.85036 + (445267.111480 * T) - (0.0019142 * Tsquared) + (Tcubed / 189474);
    D = rangezero2threesixty(D);
    double M = 357.52772 + (35999.050340 * T) - (0.0001603 * Tsquared) - (Tcubed / 300000);
    M = rangezero2threesixty(M);
    double Mprime = 134.96298 + (477198.867398 * T) + (0.0086972 * Tsquared) + (Tcubed / 56250);
    Mprime = rangezero2threesixty(Mprime);
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * Tsquared) + (Tcubed / 327270);
    F = rangezero2threesixty(F);
    double omega = 125.04452 - (1934.136261 * T) + (0.0020708 * Tsquared) + (Tcubed / 450000);
    omega = rangezero2threesixty(omega);
    double nulon = 0;
    double nutobec = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        const double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        const double radargument = deg2rad * argument;
        nulon += (coeff.sincoeff1 + (coeff.sincoeff2 * T)) * sin(radargument) * 0.0001;
        nutobec += (coeff.coscoeff1 + (coeff.coscoeff2 * T)) * cos(radargument) * 0.0001;
    }
    appGeoLon += deg2rad * secs2deg(nulon);
    //Obtain True Obliquity of Ecliptic
    // Mean Obliquity of Ecliptic
    const double U = (jd_tt - 2451545) / 3652500;
    const double Usquared = U * U;
    const double Ucubed = Usquared * U;
    const double U4 = Ucubed * U;
    const double U5 = U4 * U;
    const double U6 = U5 * U;
    const double U7 = U6 * U;
    const double U8 = U7 * U;
    const double U9 = U8 * U;
    const double U10 = U9 * U;
    double obliqEc = dms2deg(23, 26, 21.448)
        - (secs2deg(4680.93) * U)
        - (secs2deg(1.55) * Usquared)
        + (secs2deg(1999.25) * Ucubed)
        - (secs2deg(51.38) * U4)
        - (secs2deg(249.67) * U5)
        - (secs2deg(39.05) * U6)
        + (secs2deg(7.12) * U7)
        + (secs2deg(27.87) * U8)
        + (secs2deg(5.79) * U9)
        + (secs2deg(2.45) * U10)
        // Nutation in Obliquity of Ecliptic already calculated in Nutation of Longitude above
        + secs2deg(nutobec);
    //Convert to RA and Dec
    LLH ApparentEqu = calcEc2Geo(appGeoLat, appGeoLon, deg2rad * obliqEc, true);
    details.geq.lon = ApparentEqu.lon; // RA
    details.geogha = ApparentGreenwichSiderealTime(EDateTime::getJDTT2UTC(jd_tt), true) - details.geq.lon;
    details.geq.lat = ApparentEqu.lat; // Dec
    return details;
}
CelestialPath* Astronomy::getCelestialPath(size_t planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, bool fixed) {
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
double Astronomy::getEcLat(size_t planet, double jd_tt) {
    // Heliocentric Ecliptic Latitude (ref. Equinox of Epoch) in radians
    // Unconditionally caching may or may not be desired. Consider a flag, or rename the function so it is clear that getEcLat{planet}() may be preferable. !!!
    if (planet == MERCURY) planet_ecLat[planet] = EcLatMercury(jd_tt);
    else if (planet == VENUS) planet_ecLat[planet] = EcLatVenus(jd_tt);
    else if (planet == EARTH) planet_ecLat[planet] = EcLatEarth(jd_tt);
    else if (planet == MARS) planet_ecLat[planet] = EcLatMars(jd_tt);
    else if (planet == JUPITER) planet_ecLat[planet] = EcLatJupiter(jd_tt);
    else if (planet == SATURN) planet_ecLat[planet] = EcLatSaturn(jd_tt);
    else if (planet == URANUS) planet_ecLat[planet] = EcLatUranus(jd_tt);
    else if (planet == NEPTUNE) planet_ecLat[planet] = EcLatNeptune(jd_tt);
    else std::cout << "APlanet::getEcLat(): planet unknown: " << planet << "\n";
    return planet_ecLat[planet];
}
double Astronomy::getEcLon(size_t planet, double jd_tt) {
    // Heliocentric Ecliptic Longitude (ref. Equinox of Epoch) in radians
    if (planet == MERCURY) planet_ecLon[planet] = EcLonMercury(jd_tt);
    else if (planet == VENUS) planet_ecLon[planet] = EcLonVenus(jd_tt);
    else if (planet == EARTH) planet_ecLon[planet] = EcLonEarth(jd_tt);
    else if (planet == MARS) planet_ecLon[planet] = EcLonMars(jd_tt);
    else if (planet == JUPITER) planet_ecLon[planet] = EcLonJupiter(jd_tt);
    else if (planet == SATURN) planet_ecLon[planet] = EcLonSaturn(jd_tt);
    else if (planet == URANUS) planet_ecLon[planet] = EcLonUranus(jd_tt);
    else if (planet == NEPTUNE) planet_ecLon[planet] = EcLonNeptune(jd_tt);
    else std::cout << "APlanet::getEcLon(): planet unknown: " << planet << "\n";
    return planet_ecLon[planet];
}
double Astronomy::getRadius(size_t planet, double jd_tt, bool km) {
    // Heliocentric Ecliptic Radius (ref. Equinox of Epoch) in kilometers
    if (planet == MERCURY) planet_ecRadius[planet] = EcDstMercury(jd_tt);
    else if (planet == VENUS) planet_ecRadius[planet] = EcDstVenus(jd_tt);
    else if (planet == EARTH) planet_ecRadius[planet] = EcDstEarth(jd_tt);
    else if (planet == MARS) planet_ecRadius[planet] = EcDstMars(jd_tt);
    else if (planet == JUPITER) planet_ecRadius[planet] = EcDstJupiter(jd_tt);
    else if (planet == SATURN) planet_ecRadius[planet] = EcDstSaturn(jd_tt);
    else if (planet == URANUS) planet_ecRadius[planet] = EcDstUranus(jd_tt);
    else if (planet == NEPTUNE) planet_ecRadius[planet] = EcDstNeptune(jd_tt);
    else std::cout << "APlanet::getRadius(): planet unknown: " << planet << "\n";
    if (km) planet_ecRadius[planet] = au2km * planet_ecRadius[planet];
    return planet_ecRadius[planet];
}
// Below is imported from CAA[planet] | planet = {Mercury, Venus, Mars, Jupiter, Saturn, Uranus, Neptune}
// Implements EcLat[planet](), EcLon[planet](), EcDst[planet](), e.g. EcLonMars() etc
// This avoids a few function call context switches, as well as incessant conversions between degrees and radians

//  Heliocentric Ecliptic Calculations for Latitude, Longitude, Distance in radians and AU
double Astronomy::EcLonMercury(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double L0 = 0;
    for (const auto& L0Coefficient : g_L0MercuryCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1 = 0;
    for (const auto& L1Coefficient : g_L1MercuryCoefficients)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2 = 0;
    for (const auto& L2Coefficient : g_L2MercuryCoefficients)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3 = 0;
    for (const auto& L3Coefficient : g_L3MercuryCoefficients)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4 = 0;
    for (const auto& L4Coefficient : g_L4MercuryCoefficients)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5 = 0;
    for (const auto& L5Coefficient : g_L5MercuryCoefficients)
        L5 += (L5Coefficient.A * cos(L5Coefficient.B + (L5Coefficient.C * rho)));
    double value = (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * (L4 + rho * L5))))) / 100000000;
    return rangezero2tau(value);
}
double Astronomy::EcLatMercury(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0MercuryCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1MercuryCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2 = 0;
    for (const auto& B2Coefficient : g_B2MercuryCoefficients)
        B2 += (B2Coefficient.A * cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3 = 0;
    for (const auto& B3Coefficient : g_B3MercuryCoefficients)
        B3 += (B3Coefficient.A * cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4 = 0;
    for (const auto& B4Coefficient : g_B4MercuryCoefficients)
        B4 += (B4Coefficient.A * cos(B4Coefficient.B + (B4Coefficient.C * rho)));
    double value = (B0 + rho * (B1 + rho * (B2 + rho * (B3 + rho * B4)))) / 100000000;
    return rangemhalfpi2halfpi(value);
}
double Astronomy::EcDstMercury(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double R0 = 0;
    for (const auto& R0Coefficient : g_R0MercuryCoefficients)
        R0 += (R0Coefficient.A * cos(R0Coefficient.B + (R0Coefficient.C * rho)));
    double R1 = 0;
    for (const auto& R1Coefficient : g_R1MercuryCoefficients)
        R1 += (R1Coefficient.A * cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2 = 0;
    for (const auto& R2Coefficient : g_R2MercuryCoefficients)
        R2 += (R2Coefficient.A * cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3 = 0;
    for (const auto& R3Coefficient : g_R3MercuryCoefficients)
        R3 += (R3Coefficient.A * cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    return (R0 + rho * (R1 + rho * (R2 + rho * R3))) / 100000000;
}
double Astronomy::EcLonVenus(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double L0 = 0;
    for (const auto& L0Coefficient : g_L0VenusCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1 = 0;
    for (const auto& L1Coefficient : g_L1VenusCoefficients)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2 = 0;
    for (const auto& L2Coefficient : g_L2VenusCoefficients)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3 = 0;
    for (const auto& L3Coefficient : g_L3VenusCoefficients)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4 = 0;
    for (const auto& L4Coefficient : g_L4VenusCoefficients)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5 = 0;
    for (const auto& L5Coefficient : g_L5VenusCoefficients)
        L5 += (L5Coefficient.A * cos(L5Coefficient.B + (L5Coefficient.C * rho)));
    double value = (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * (L4 + rho * L5))))) / 100000000;
    return rangezero2tau(value);
}
double Astronomy::EcLatVenus(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0VenusCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1VenusCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2 = 0;
    for (const auto& B2Coefficient : g_B2VenusCoefficients)
        B2 += (B2Coefficient.A * cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3 = 0;
    for (const auto& B3Coefficient : g_B3VenusCoefficients)
        B3 += (B3Coefficient.A * cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4 = 0;
    for (const auto& B4Coefficient : g_B4VenusCoefficients)
        B4 += (B4Coefficient.A * cos(B4Coefficient.B + (B4Coefficient.C * rho)));
    double value = (B0 + rho * (B1 + rho * (B2 + rho * (B3 + rho * B4)))) / 100000000;
    return rangemhalfpi2halfpi(value);
}
double Astronomy::EcDstVenus(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double R0 = 0;
    for (const auto& R0Coefficient : g_R0VenusCoefficients)
        R0 += (R0Coefficient.A * cos(R0Coefficient.B + (R0Coefficient.C * rho)));
    double R1 = 0;
    for (const auto& R1Coefficient : g_R1VenusCoefficients)
        R1 += (R1Coefficient.A * cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2 = 0;
    for (const auto& R2Coefficient : g_R2VenusCoefficients)
        R2 += (R2Coefficient.A * cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3 = 0;
    for (const auto& R3Coefficient : g_R3VenusCoefficients)
        R3 += (R3Coefficient.A * cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4 = 0;
    for (const auto& R4Coefficient : g_R4VenusCoefficients)
        R4 += (R4Coefficient.A * cos(R4Coefficient.B + (R4Coefficient.C * rho)));
    return (R0 + rho * (R1 + rho * (R2 + rho * (R3 + rho * R4)))) / 100000000;
}
double Astronomy::EcLonEarth(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double L0 = 0;
    for (const auto& L0Coefficient : g_L0EarthCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1 = 0;
    for (const auto& L1Coefficient : g_L1EarthCoefficients)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2 = 0;
    for (const auto& L2Coefficient : g_L2EarthCoefficients)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3 = 0;
    for (const auto& L3Coefficient : g_L3EarthCoefficients)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4 = 0;
    for (const auto& L4Coefficient : g_L4EarthCoefficients)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5 = 0;
    for (const auto& L5Coefficient : g_L5EarthCoefficients)
        L5 += (L5Coefficient.A * cos(L5Coefficient.B + (L5Coefficient.C * rho)));
    double value = (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * (L4 + rho * L5))))) / 100000000;
    return rangezero2tau(value);
}
double Astronomy::EcLatEarth(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0EarthCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1EarthCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double value = (B0 + B1 * rho) / 100000000;
    return rangemhalfpi2halfpi(value);
}
double Astronomy::EcDstEarth(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double R0 = 0;
    for (const auto& R0Coefficient : g_R0EarthCoefficients)
        R0 += (R0Coefficient.A * cos(R0Coefficient.B + (R0Coefficient.C * rho)));
    double R1 = 0;
    for (const auto& R1Coefficient : g_R1EarthCoefficients)
        R1 += (R1Coefficient.A * cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2 = 0;
    for (const auto& R2Coefficient : g_R2EarthCoefficients)
        R2 += (R2Coefficient.A * cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3 = 0;
    for (const auto& R3Coefficient : g_R3EarthCoefficients)
        R3 += (R3Coefficient.A * cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4 = 0;
    for (const auto& R4Coefficient : g_R4EarthCoefficients)
        R4 += (R4Coefficient.A * cos(R4Coefficient.B + (R4Coefficient.C * rho)));
    return (R0 + rho * (R1 + rho * (R2 + rho * (R3 + rho * R4)))) / 100000000;
}
double Astronomy::EcLonMars(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double L0 = 0;
    for (const auto& L0Coefficient : g_L0MarsCoefficients)
        L0 += (L0Coefficient.A * std::cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1 = 0;
    for (const auto& L1Coefficient : g_L1MarsCoefficients)
        L1 += (L1Coefficient.A * std::cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2 = 0;
    for (const auto& L2Coefficient : g_L2MarsCoefficients)
        L2 += (L2Coefficient.A * std::cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3 = 0;
    for (const auto& L3Coefficient : g_L3MarsCoefficients)
        L3 += (L3Coefficient.A * std::cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4 = 0;
    for (const auto& L4Coefficient : g_L4MarsCoefficients)
        L4 += (L4Coefficient.A * std::cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5 = 0;
    for (const auto& L5Coefficient : g_L5MarsCoefficients)
        L5 += (L5Coefficient.A * std::cos(L5Coefficient.B + (L5Coefficient.C * rho)));
    double value = (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * (L4 + rho * L5))))) / 100000000;
    return rangezero2tau(value);
}
double Astronomy::EcLatMars(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0MarsCoefficients)
        B0 += (B0Coefficient.A * std::cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1MarsCoefficients)
        B1 += (B1Coefficient.A * std::cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2 = 0;
    for (const auto& B2Coefficient : g_B2MarsCoefficients)
        B2 += (B2Coefficient.A * std::cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3 = 0;
    for (const auto& B3Coefficient : g_B3MarsCoefficients)
        B3 += (B3Coefficient.A * std::cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4 = 0;
    for (const auto& B4Coefficient : g_B4MarsCoefficients)
        B4 += (B4Coefficient.A * std::cos(B4Coefficient.B + (B4Coefficient.C * rho)));
    double value = (B0 + rho * (B1 + rho * (B2 + rho * (B3 + rho * B4)))) / 100000000;
    return rangemhalfpi2halfpi(value);
}
double Astronomy::EcDstMars(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double R0 = 0;
    for (const auto& R0Coefficients : g_R0MarsCoefficients)
        R0 += (R0Coefficients.A * std::cos(R0Coefficients.B + (R0Coefficients.C * rho)));
    double R1 = 0;
    for (const auto& R1Coefficient : g_R1MarsCoefficients)
        R1 += (R1Coefficient.A * std::cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2 = 0;
    for (const auto& R2Coefficient : g_R2MarsCoefficients)
        R2 += (R2Coefficient.A * std::cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3 = 0;
    for (const auto& R3Coefficient : g_R3MarsCoefficients)
        R3 += (R3Coefficient.A * std::cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4 = 0;
    for (const auto& R4Coefficient : g_R4MarsCoefficients)
        R4 += (R4Coefficient.A * std::cos(R4Coefficient.B + (R4Coefficient.C * rho)));
    return (R0 + rho * (R1 + rho * (R2 + rho * (R3 + rho * R4)))) / 100000000;
}
double Astronomy::EcLonJupiter(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double L0 = 0;
    for (const auto& L0Coefficient : g_L0JupiterCoefficients)
        L0 += (L0Coefficient.A * cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1 = 0;
    for (const auto& L1Coefficient : g_L1JupiterCoefficients)
        L1 += (L1Coefficient.A * cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2 = 0;
    for (const auto& L2Coefficient : g_L2JupiterCoefficients)
        L2 += (L2Coefficient.A * cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3 = 0;
    for (const auto& L3Coefficient : g_L3JupiterCoefficients)
        L3 += (L3Coefficient.A * cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4 = 0;
    for (const auto& L4Coefficient : g_L4JupiterCoefficients)
        L4 += (L4Coefficient.A * cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5 = 0;
    for (const auto& L5Coefficient : g_L5JupiterCoefficients)
        L5 += (L5Coefficient.A * cos(L5Coefficient.B + (L5Coefficient.C * rho)));
    double value = (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * (L4 + rho * L5))))) / 100000000;
    return rangezero2tau(value);
}
double Astronomy::EcLatJupiter(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0JupiterCoefficients)
        B0 += (B0Coefficient.A * std::cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1JupiterCoefficients)
        B1 += (B1Coefficient.A * std::cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2 = 0;
    for (const auto& B2Coefficient : g_B2JupiterCoefficients)
        B2 += (B2Coefficient.A * std::cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3 = 0;
    for (const auto& B3Coefficient : g_B3JupiterCoefficients)
        B3 += (B3Coefficient.A * std::cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4 = 0;
    for (const auto& B4Coefficient : g_B4JupiterCoefficients)
        B4 += (B4Coefficient.A * std::cos(B4Coefficient.B + (B4Coefficient.C * rho)));
    double B5 = 0;
    for (const auto& B5Coefficient : g_B5JupiterCoefficients)
        B5 += (B5Coefficient.A * std::cos(B5Coefficient.B + (B5Coefficient.C * rho)));
    double value = (B0 + rho * (B1 + rho * (B2 + rho * (B3 + rho * (B4 + rho * B5))))) / 100000000;
    return rangemhalfpi2halfpi(value);
}
double Astronomy::EcDstJupiter(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double R0 = 0;
    for (const auto& R0Coefficients : g_R0JupiterCoefficients)
        R0 += (R0Coefficients.A * std::cos(R0Coefficients.B + (R0Coefficients.C * rho)));
    double R1 = 0;
    for (const auto& R1Coefficient : g_R1JupiterCoefficients)
        R1 += (R1Coefficient.A * std::cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2 = 0;
    for (const auto& R2Coefficient : g_R2JupiterCoefficients)
        R2 += (R2Coefficient.A * std::cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3 = 0;
    for (const auto& R3Coefficient : g_R3JupiterCoefficients)
        R3 += (R3Coefficient.A * std::cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4 = 0;
    for (const auto& R4Coefficient : g_R4JupiterCoefficients)
        R4 += (R4Coefficient.A * std::cos(R4Coefficient.B + (R4Coefficient.C * rho)));
    double R5 = 0;
    for (const auto& R5Coefficient : g_R5JupiterCoefficients)
        R5 += (R5Coefficient.A * cos(R5Coefficient.B + (R5Coefficient.C * rho)));

    return (R0 + rho * (R1 + rho * (R2 + rho * (R3 + rho * (R4 + rho * R5))))) / 100000000;
}
double Astronomy::EcLonSaturn(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double L0 = 0;
    for (const auto& L0Coefficient : g_L0SaturnCoefficients)
        L0 += (L0Coefficient.A * std::cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1 = 0;
    for (const auto& L1Coefficient : g_L1SaturnCoefficients)
        L1 += (L1Coefficient.A * std::cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2 = 0;
    for (const auto& L2Coefficient : g_L2SaturnCoefficients)
        L2 += (L2Coefficient.A * std::cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3 = 0;
    for (const auto& L3Coefficient : g_L3SaturnCoefficients)
        L3 += (L3Coefficient.A * std::cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4 = 0;
    for (const auto& L4Coefficient : g_L4SaturnCoefficients)
        L4 += (L4Coefficient.A * std::cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double L5 = 0;
    for (const auto& L5Coefficient : g_L5SaturnCoefficients)
        L5 += (L5Coefficient.A * std::cos(L5Coefficient.B + (L5Coefficient.C * rho)));
    double value = (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * (L4 + rho * L5))))) / 100000000;
    return rangezero2tau(value);
}
double Astronomy::EcLatSaturn(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0SaturnCoefficients)
        B0 += (B0Coefficient.A * std::cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1SaturnCoefficients)
        B1 += (B1Coefficient.A * std::cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2 = 0;
    for (const auto& B2Coefficient : g_B2SaturnCoefficients)
        B2 += (B2Coefficient.A * std::cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3 = 0;
    for (const auto& B3Coefficient : g_B3SaturnCoefficients)
        B3 += (B3Coefficient.A * std::cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4 = 0;
    for (const auto& B4Coefficient : g_B4SaturnCoefficients)
        B4 += (B4Coefficient.A * std::cos(B4Coefficient.B + (B4Coefficient.C * rho)));
    double B5 = 0;
    for (const auto& B5Coefficient : g_B5SaturnCoefficients)
        B5 += (B5Coefficient.A * std::cos(B5Coefficient.B + (B5Coefficient.C * rho)));
    double value = (B0 + rho * (B1 + rho * (B2 + rho * (B3 + rho * (B4 + rho * B5))))) / 100000000;
    return rangemhalfpi2halfpi(value);
}
double Astronomy::EcDstSaturn(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double R0 = 0;
    for (const auto& R0Coefficients : g_R0SaturnCoefficients)
        R0 += (R0Coefficients.A * std::cos(R0Coefficients.B + (R0Coefficients.C * rho)));
    double R1 = 0;
    for (const auto& R1Coefficient : g_R1SaturnCoefficients)
        R1 += (R1Coefficient.A * std::cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2 = 0;
    for (const auto& R2Coefficient : g_R2SaturnCoefficients)
        R2 += (R2Coefficient.A * std::cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3 = 0;
    for (const auto& R3Coefficient : g_R3SaturnCoefficients)
        R3 += (R3Coefficient.A * std::cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4 = 0;
    for (const auto& R4Coefficient : g_R4SaturnCoefficients)
        R4 += (R4Coefficient.A * std::cos(R4Coefficient.B + (R4Coefficient.C * rho)));
    double R5 = 0;
    for (const auto& R5Coefficient : g_R5SaturnCoefficients)
        R5 += (R5Coefficient.A * std::cos(R5Coefficient.B + (R5Coefficient.C * rho)));
    return (R0 + rho * (R1 + rho * (R2 + rho * (R3 + rho * (R4 + rho * R5))))) / 100000000;
}
double Astronomy::EcLonUranus(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double L0 = 0;
    for (const auto& L0Coefficient : g_L0UranusCoefficients)
        L0 += (L0Coefficient.A * std::cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1 = 0;
    for (const auto& L1Coefficient : g_L1UranusCoefficients)
        L1 += (L1Coefficient.A * std::cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2 = 0;
    for (const auto& L2Coefficient : g_L2UranusCoefficients)
        L2 += (L2Coefficient.A * std::cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3 = 0;
    for (const auto& L3Coefficient : g_L3UranusCoefficients)
        L3 += (L3Coefficient.A * std::cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4 = 0;
    for (const auto& L4Coefficient : g_L4UranusCoefficients)
        L4 += (L4Coefficient.A * std::cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double value = (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * L4)))) / 100000000;
    return rangezero2tau(value);
}
double Astronomy::EcLatUranus(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0UranusCoefficients)
        B0 += (B0Coefficient.A * std::cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1UranusCoefficients)
        B1 += (B1Coefficient.A * std::cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2 = 0;
    for (const auto& B2Coefficient : g_B2UranusCoefficients)
        B2 += (B2Coefficient.A * std::cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3 = 0;
    for (const auto& B3Coefficient : g_B3UranusCoefficients)
        B3 += (B3Coefficient.A * std::cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4 = 0;
    for (const auto& B4Coefficient : g_B4UranusCoefficients)
        B4 += (B4Coefficient.A * std::cos(B4Coefficient.B + (B4Coefficient.C * rho)));
    double value = (B0 + rho * (B1 + rho * (B2 + rho * (B3 + rho * B4)))) / 100000000;
    return rangemhalfpi2halfpi(value);
}
double Astronomy::EcDstUranus(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double R0 = 0;
    for (const auto& R0Coefficients : g_R0UranusCoefficients)
        R0 += (R0Coefficients.A * std::cos(R0Coefficients.B + (R0Coefficients.C * rho)));
    double R1 = 0;
    for (const auto& R1Coefficient : g_R1UranusCoefficients)
        R1 += (R1Coefficient.A * std::cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2 = 0;
    for (const auto& R2Coefficient : g_R2UranusCoefficients)
        R2 += (R2Coefficient.A * std::cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3 = 0;
    for (const auto& R3Coefficient : g_R3UranusCoefficients)
        R3 += (R3Coefficient.A * std::cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    double R4 = 0;
    for (const auto& R4Coefficient : g_R4UranusCoefficients)
        R4 += (R4Coefficient.A * std::cos(R4Coefficient.B + (R4Coefficient.C * rho)));
    return (R0 + rho * (R1 + rho * (R2 + rho * (R3 + rho * R4)))) / 100000000;
}
double Astronomy::EcLonNeptune(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double L0 = 0;
    for (const auto& L0Coefficient : g_L0NeptuneCoefficients)
        L0 += (L0Coefficient.A * std::cos(L0Coefficient.B + (L0Coefficient.C * rho)));
    double L1 = 0;
    for (const auto& L1Coefficient : g_L1NeptuneCoefficients)
        L1 += (L1Coefficient.A * std::cos(L1Coefficient.B + (L1Coefficient.C * rho)));
    double L2 = 0;
    for (const auto& L2Coefficient : g_L2NeptuneCoefficients)
        L2 += (L2Coefficient.A * std::cos(L2Coefficient.B + (L2Coefficient.C * rho)));
    double L3 = 0;
    for (const auto& L3Coefficient : g_L3NeptuneCoefficients)
        L3 += (L3Coefficient.A * std::cos(L3Coefficient.B + (L3Coefficient.C * rho)));
    double L4 = 0;
    for (const auto& L4Coefficient : g_L4NeptuneCoefficients)
        L4 += (L4Coefficient.A * std::cos(L4Coefficient.B + (L4Coefficient.C * rho)));
    double value = (L0 + rho * (L1 + rho * (L2 + rho * (L3 + rho * L4)))) / 100000000;
    return rangezero2tau(value);
}
double Astronomy::EcLatNeptune(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0NeptuneCoefficients)
        B0 += (B0Coefficient.A * std::cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1NeptuneCoefficients)
        B1 += (B1Coefficient.A * std::cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double B2 = 0;
    for (const auto& B2Coefficient : g_B2NeptuneCoefficients)
        B2 += (B2Coefficient.A * std::cos(B2Coefficient.B + (B2Coefficient.C * rho)));
    double B3 = 0;
    for (const auto& B3Coefficient : g_B3NeptuneCoefficients)
        B3 += (B3Coefficient.A * std::cos(B3Coefficient.B + (B3Coefficient.C * rho)));
    double B4 = 0;
    for (const auto& B4Coefficient : g_B4NeptuneCoefficients)
        B4 += (B4Coefficient.A * std::cos(B4Coefficient.B + (B4Coefficient.C * rho)));
    double value = (B0 + rho * (B1 + rho * (B2 + rho * (B3 + rho * B4)))) / 100000000;
    return rangemhalfpi2halfpi(value);
}
double Astronomy::EcDstNeptune(double jd_tt) {
    const double rho = (jd_tt - 2451545) / 365250;
    double R0 = 0;
    for (const auto& R0Coefficients : g_R0NeptuneCoefficients)
        R0 += (R0Coefficients.A * std::cos(R0Coefficients.B + (R0Coefficients.C * rho)));
    double R1 = 0;
    for (const auto& R1Coefficient : g_R1NeptuneCoefficients)
        R1 += (R1Coefficient.A * std::cos(R1Coefficient.B + (R1Coefficient.C * rho)));
    double R2 = 0;
    for (const auto& R2Coefficient : g_R2NeptuneCoefficients)
        R2 += (R2Coefficient.A * std::cos(R2Coefficient.B + (R2Coefficient.C * rho)));
    double R3 = 0;
    for (const auto& R3Coefficient : g_R3NeptuneCoefficients)
        R3 += (R3Coefficient.A * std::cos(R3Coefficient.B + (R3Coefficient.C * rho)));
    return (R0 + rho * (R1 + rho * (R2 + rho * R3))) / 100000000;
}
int Astronomy::getString2UnixTime(std::string& string) {
    // TODO: Implement like in Eartharium.cpp:TestArea5(), but resilient to missing seconds, and accepting either '/' or '-' date separator
    std::cout << "ERROR: Astronomy::getString2UnixTime() is not yet implemented.";
    return 0;
}
int Astronomy::getDateTime2UnixTime(double year, double month, double day, double hour, double minute, double second) { // Unix time and date time in UTC
    // Verified to work with negative Unix timestamps too, i.e. dates before epoch (1970-01-01 00:00:00)
    int y = (int)year - 1900;
    int ydays = (int)calcUnixTimeYearDay(year, month, day);
    //std::cout << "Day of Year: " << ydays << "\n";
    int utime = (int)second + (int)minute * 60 + (int)hour * 3600; // Day fraction
    //std::cout << "Day fraction: " << utime << "\n";
    utime += ydays * 86400; // Month and day as calculated above, including current leap year
    utime += (y - 70) * 31536000 + ((y - 69) / 4) * 86400 - ((y - 1) / 100) * 86400 + ((y + 299) / 400) * 86400; // Previous leap years adjustment
    return utime;
}
double Astronomy::getJD2UnixTime(double jd_utc) { // Unix time and JD in UTC
    // FIX: !!! Should use EDateTime conversion function
    if (jd_utc == NO_DOUBLE) jd_utc = m_datetime.jd_utc();
    return (jd_utc - 2440587.5) * 86400;
}
double Astronomy::getUnixTime2JD(double ut) { // Unix time and JD in UTC
    // FIX: !!! Should use EDateTime conversion function
    return ut / 86400 + 2440587.5;
}
int Astronomy::calcUnixTimeYearDay(double year, double month, double day) {
    // Cannot handle most un-normalized dates correctly, so provide sane input dates
    static int months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int mo = (int)(month - 1.0);
    int days = 0;
    for (int m = 0; m < mo; m++) {
        days += months[m];
    }
    if (isLeapYear(year) && month > 2.0) days += 1;
    if (year < 1970) days -= 1;  // 
    return days + (int)day - 1;  // -1 because UnixTime starts on day zero, not day one.
}
double Astronomy::secs2deg(double seconds) {
    return seconds / 3600.0;
}
double Astronomy::rangezero2tau(double rad) { // snap radian value to 0 to 2pi range
    double fResult = fmod(rad, tau);
    if (fResult < 0)
        fResult += tau;
    return fResult;
}
double Astronomy::rangemhalfpi2halfpi(double rad) { // snap radian value to -pi/2 to pi/2 range
    double fResult = rangezero2tau(rad);
    if (fResult > 1.5 * pi)
        fResult = fResult - tau;
    else if (fResult > pi)
        fResult = pi - fResult;
    else if (fResult > pi2)
        fResult = pi - fResult;
    return fResult;
}
double Astronomy::rangezero2threesixty(double deg) { // snap degree value to 0 to 360 range
    double fResult = fmod(deg, 360.0);
    if (fResult < 0)
        fResult += 360.0;
    return fResult;
}
double Astronomy::rangemoneeighty2oneeighty(double deg) { // snap degree value to 0 to 360 range
    double fResult = fmod(deg + 180.0, 360.0);
    if (fResult < 0)
        fResult += 360.0;
    return fResult - 180.0;
}

double Astronomy::rangezero2twentyfour(double deg) { // snap hours value to 0 to 24 range
    double fResult = fmod(deg, 24.0);
    if (fResult < 0)
        fResult += 24.0;
    return fResult;
}
std::string Astronomy::angle2DMSstring(double angle, bool rad) {
    if (rad) angle *= rad2deg;
    std::string dstring;
    double deg = abs(angle);
    double dg = floor(deg) * std::copysign(1.0, angle);
    double min = 60 * (deg - abs(dg));
    double mi = floor(min);
    double sec = 60 * (min - mi);
    int d = (int)dg;
    int m = (int)mi;
    char buff[100];
    snprintf(buff, sizeof(buff), "%03d\xF8%02d\'%06.3f\"", d, m, sec);
    dstring = buff;
    return dstring;
}
std::string Astronomy::angle2uDMSstring(double angle, bool rad) { // Unsigned version, used by latlonFormat() which will insert N/S or E/W instead of sign
    if (rad) angle *= rad2deg;
    std::string dstring;
    double deg = abs(angle);
    double dg = floor(deg);
    double min = 60 * (deg - abs(dg));
    double mi = floor(min);
    double sec = 60 * (min - mi);
    int d = (int)dg;
    int m = (int)mi;
    char buff[100];
    snprintf(buff, sizeof(buff), "%03d\xF8%02d\'%06.3f\"", d, m, sec);
    dstring = buff;
    return dstring;
}
std::string Astronomy::angle2uHMSstring(double angle, bool rad) { // Hour angles are always given in [0;24], thus unsigned
    if (rad) angle *= rad2hrs;
    else angle /= 15.0;
    angle = rangezero2twentyfour(angle);
    std::string dstring;
    double hrs = abs(angle);
    double hr = floor(hrs);  // Does not give correct result when angle is negative
    double min = 60 * (hrs - abs(hr));
    double mi = floor(min);
    double sec = 60 * (min - mi);
    int h = (int)hr;
    int m = (int)mi;
    char buff[100];
    snprintf(buff, sizeof(buff), "%03dh%02dm%06.3fs", h, m, sec);
    dstring = buff;
    return dstring;
}
std::string Astronomy::angle2DMstring(double angle, bool rad) {
    // In celestial navigation it is tradition to use XXX°xx.xxx' format
    // Since these are often deltas, use signed notation.
    if (rad) angle *= rad2deg;
    std::string dstring;
    double deg = abs(angle);
    double dg = floor(deg) * std::copysign(1.0, angle);
    double min = 60 * (deg - abs(dg));
    //double mi = floor(min);
    //double sec = 60 * (min - mi);
    int d = (int)dg;
    //int m = (int)min;
    char buff[100];
    snprintf(buff, sizeof(buff), "%03d\xF8%06.3f\'", d, min);
    dstring = buff;
    return dstring;
}
std::string Astronomy::formatLatLon(double lat, double lon, bool rad) {
    std::string dstring;
    std::string slat = angle2uDMSstring(lat, rad);  // Unsigned version of angle to DMS string
    std::string slon = angle2uDMSstring(lon, rad);  // ditto
    char buff[100];
    snprintf(buff, sizeof(buff), "%s%s , %s%s", lat >= 0.0 ? "N" : "S", slat.c_str(), lon >= 0.0 ? "E" : "W", slon.c_str());
    dstring = buff;
    return dstring;
}
std::string Astronomy::formatDecRA(double dec, double ra, bool rad) {
    std::string dstring;
    std::string sra = angle2uHMSstring(ra, rad);
    std::string sdec = angle2DMSstring(dec, rad);
    char buff[100];
    snprintf(buff, sizeof(buff), "%s%s / %s", dec > 0.0 ? "+" : "-", sdec.c_str(), sra.c_str());
    dstring = buff;
    return dstring;
}
std::string Astronomy::formatEleAz(double ele, double az, bool rad) {
    std::string dstring;
    std::string sele = angle2uDMSstring(ele, rad);  // Unsigned vwersion of angle to DMS string
    std::string saz = angle2uDMSstring(az, rad);  // ditto
    char buff[100];
    snprintf(buff, sizeof(buff), "%s%s / %s%s", ele >= 0.0 ? "+" : "-", sele.c_str(), az >= 0.0 ? "+" : "-", saz.c_str());
    dstring = buff;
    return dstring;
}

//  Privates
void Astronomy::updateGeocentric(size_t planet) {
    if (planet_jd[planet] == m_datetime.jd_tt()) return;  // Skip if time has not updated. Ideally we should never be called at all in that case.
    CelestialDetail details = getDetails(m_datetime.jd_tt(), planet, ECGEO);
    planet_dec[planet] = details.geq.lat; // Geocentric Dec
    planet_ra[planet] = details.geq.lon;  // Geocentric RA
    planet_gha[planet] = details.geogha;  // Geocentric Greenwich HA
    planet_gha[planet] = rangezero2tau(planet_gha[planet]);
    planet_jd[planet] = m_datetime.jd_tt();
    //planet_dst[planet] = au2km * details.ApparentGeocentricDistance;
    //std::cout << "Planet, RA, Dec, GSid: " << planet << ", " << planet_ra[planet] << ", " << planet_dec[planet] << ", " << m_gsidtime << "\n";
}
void Astronomy::updateGsid() { // Updates GSid, and sets nutation in the process
    // If these functions cache results, it is a catch 22. So have separate private functions for these updates:
    m_meanobliquity = MeanObliquityOfEcliptic(m_datetime.jd_utc(), true);
    m_trueobliquity = m_meanobliquity + (NutationInObliquity(m_datetime.jd_utc(), true));
    m_nutationinlongitude = NutationInLongitude(m_datetime.jd_utc(), true);
    m_meangsid = MeanGreenwichSiderealTime(m_datetime.jd_utc(), true);
    m_gsidtime = m_meangsid + (m_nutationinlongitude * cos(m_trueobliquity));
    // All values are in radians use rad2hrs for gsids and rad2deg for the others
}
void Astronomy::updatePrecession() {
    // Precession parameters for J2000 - applied to all stars, so well worth caching
    // used in PrecessJ2000DecRA() and update()
    const double t = (m_datetime.jd_tt() - JD_2000) / 36525;
    const double tsquared = t * t;
    const double tcubed = tsquared * t;
    prec_j2000_sigma = deg2rad * dms2deg(0, 0, (2306.2181) * t + (0.30188) * tsquared + 0.017998 * tcubed);
    prec_j2000_zeta = deg2rad * dms2deg(0, 0, (2306.2181) * t + (1.09468) * tsquared + 0.018203 * tcubed);
    prec_j2000_phi = deg2rad * dms2deg(0, 0, (2004.3109) * t - (0.42665) * tsquared - 0.041833 * tcubed);
    prec_j2000_phi_s = sin(prec_j2000_phi);
    prec_j2000_phi_c = cos(prec_j2000_phi);
}
void Astronomy::update() {
    updateGsid();          // Also updates nutation and obliquity caches
    updatePrecession();
    updateTimeString();

    // Update cached planetary positions to current time. Later, include Sun, Moon & Earth?
    for (unsigned int p = SUN; p <= NEPTUNE; p++) {
        if (planet_refcnt[p] > 0) updateGeocentric(p);
    }
    // Update cached planetary paths
    updateCelestialPaths();
}

