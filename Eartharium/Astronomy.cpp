
#include <array>
#include <string>
#include <chrono>
//#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

#include "config.h"
#include "Astronomy.h"

#include "astronomy/aconfig.h"
#include "astronomy/acoordinates.h"
#include "astronomy/datetime.h"
#include "astronomy/aearth.h"
#include "astronomy/asun.h"
#include "astronomy/amercury.h"
#include "astronomy/avenus.h"
#include "astronomy/amars.h"
#include "astronomy/ajupiter.h"
#include "astronomy/asaturn.h"
#include "astronomy/auranus.h"
#include "astronomy/aneptune.h"
#include "astronomy/amoon.h"

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
LLD Astronomy::getDecRAbyName(const std::string starname, bool rad) {
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
LLD Astronomy::getDecRAwithPMbyName(const std::string starname, double jd_tt, bool rad) {
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
            double dec = s.dec + s.pm_dec * elapsedyears / 3'600'000.0;
            // NOTE: SIMBAD proper motions are from the Hipparcos mission, the right ascension value is already multiplied by cos(s.dec)
            //double ra = s.ra + s.pm_ra / cos(deg2rad * s.dec) * elapsedyears / 3600000.0;
            double ra = s.ra + s.pm_ra * elapsedyears / 3'600'000.0;
            //std::cout << "On date Ra Dec of " << starname << ": " << ra << ", " << dec << '\n';
            return { rad ? deg2rad * dec : dec, rad ? deg2rad * ra : ra, 0.0 }; // File was loaded in degrees
        }
    }
    std::cout << "ERROR! Astronomy::getDecRAwithPMbyName() - Unknown name supplied: " << starname << '\n';
    return { 0.0, 0.0, NO_DOUBLE };
}
LLD Astronomy::applyProperMotionJ2000(double jd_tt, LLD decra, LLD propermotion) {
    // apply Proper Motion - decra is in radians, propermotion is in milliarcseconds per year, from stellarobject db.
    if (jd_tt == NO_DOUBLE) jd_tt = getJD_TT();
    double elapsedyears = (jd_tt - JD_2000) / 365.25;
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
                c.outline.emplace_back(LLD{ ra, dec, 0.0 });
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
    //if (do_eot && eot == 0.0) eot = CAAEquationOfTime::Calculate(m_datetime.jd_tt(), true);
    if (do_eot && eot == 0.0) eot = AEarth::EquationOfTime(m_datetime.jd_tt(), VSOP87_FULL);
    if (eot != 0.0 && do_eot) min += eot; // If EoT was applied last time update, remove it.
    //std::cout << "Astronomy::addTime() - Remove EoT: " << eot << '\n';
    if (!do_eot) eot = 0.0;
    m_datetime.addTime(0, 0, d, h, min, sec);
    if (do_eot) {
        //eot = CAAEquationOfTime::Calculate(m_datetime.jd_tt(), true);
        eot = AEarth::EquationOfTime(m_datetime.jd_tt(), VSOP87_FULL);
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
    std::cout << " - Julian Date (TT):  " << m_datetime.jd_tt() << '\n';
    std::cout << " - Julian Date (UTC): " << m_datetime.jd_utc() << '\n';
}

double Astronomy::ApparentGreenwichSiderealTime(double jd_utc, bool rad) noexcept {
    if (jd_utc == NO_DOUBLE) jd_utc = getJD_UTC();
    double gsidtime = AEarth::ApparentGreenwichSiderealTime(jd_utc);
    //std::cout << "Astronomy::ApparentGreenwichSiderealTime(" << jd_utc << "," << (rad ? "true" : "false") << ") gsidtime = " << gsidtime << std::endl;
    return rad ? gsidtime : rad2deg * gsidtime;
}
double Astronomy::MeanGreenwichSiderealTime(double jd_utc, bool rad) noexcept {
    const double mgst = AEarth::MeanGreenwichSiderealTime(jd_utc);
    return rad ? mgst : rad2deg * mgst;
}
double Astronomy::getEoT(double jd_tt) {
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    return AEarth::EquationOfTime(jd_tt, VSOP87_FULL);
}

// Coordinate Transformations
LLD Astronomy::calcEc2Geo(double Beta, double Lambda, double Epsilon, bool rad) noexcept {
    if (!rad) {
        Beta *= deg2rad;
        Lambda *= deg2rad;
        Epsilon *= deg2rad;
    }
    LLD Equatorial;
    Equatorial.lat = asin(sin(Beta) * cos(Epsilon) + cos(Beta) * sin(Epsilon) * sin(Lambda));
    Equatorial.lon = ACoord::rangezero2tau(atan2(sin(Lambda) * cos(Epsilon) - tan(Beta) * sin(Epsilon), cos(Lambda)));
    if (!rad) {
        Equatorial.lat *= rad2deg;
        Equatorial.lon *= rad2deg;
    }
    return Equatorial;
}
LLD Astronomy::calcGeo2Ec(double Delta, double Alpha, double Epsilon, bool rad) noexcept {
    // AA+: CAACoordinateTransformation::Equatorial2Ecliptic()
    // MEEUS98: This refers to algorithm 13.1 and 13.2 on page 93
    if (!rad) {
        Delta *= deg2rad;
        Alpha *= deg2rad;
        Epsilon *= deg2rad;
    }
    LLD Ecliptic;
    Ecliptic.lon = ACoord::rangezero2tau(atan2(sin(Alpha) * cos(Epsilon) + tan(Delta) * sin(Epsilon), cos(Alpha)));
    Ecliptic.lat = asin(sin(Delta) * cos(Epsilon) - cos(Delta) * sin(Epsilon) * sin(Alpha));
    if (!rad) {
        Ecliptic.lon *= rad2deg;
        Ecliptic.lat *= rad2deg;
    }
    return Ecliptic;
}
LLD Astronomy::calcGeo2Topo(LLD pos, LLD loc) {
    // AA+: CAACoordinateTransformation::Equatorial2Horizontal()
    // TODO: Does NOT account for altitude of observer !!! (maybe doesn't matter, up is still up? Mountains are not high enough for parallax)
    // pos is DecGHA, loc is LatLon
    // NOTE: No caching, not likely to be called with same position twice for same JD
    double LocalHourAngle = pos.lon + loc.lon;
    // from CAACoordinateTransformation::Equatorial2Horizontal(rad2hrs * LocalHourAngle, rad2deg * pos.lat, rad2deg * loc.lat);
    LLD topo;
    topo.lon = atan2(sin(LocalHourAngle), cos(LocalHourAngle) * sin(loc.lat) - tan(pos.lat) * cos(loc.lat));
    topo.lat = asin(sin(loc.lat) * sin(pos.lat) + cos(loc.lat) * cos(pos.lat) * cos(LocalHourAngle));
    return topo;
}
LLD Astronomy::calcDecHA2GP(LLD decra, bool rad) {
    // Map HA [0 ; pi) -> [0 ; -pi) and [pi ; tau) -> [pi ; 0)
    // !!! FIX: This assumes planetocentric coordinates (longitude is east-from-north) Make option for west-from-south !!!
    LLD res = decra;
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
LLD Astronomy::calcDecRA2GP(LLD decra, double jd_utc, bool rad) { // Needs JD as UTC
    if (jd_utc == NO_DOUBLE) // jd was not specified, so go with current time
        return calcDecHA2GP({ decra.lat, rad ? (m_gsidtime - decra.lon) : (rad2deg * m_gsidtime - decra.lon), 0.0 }, rad);
    return calcDecHA2GP({ decra.lat, rad ? (ApparentGreenwichSiderealTime(jd_utc) - decra.lon) : (rad2deg * ApparentGreenwichSiderealTime(jd_utc) - decra.lon), 0.0 }, rad);
}
LLD Astronomy::getDecRA(size_t planet, double jd_tt, bool rad) {  // JD in TT
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    if (jd_tt == planet_jd[planet]) return { planet_dec[planet], planet_ra[planet], 0.0 };
    CelestialDetail details = getDetails(jd_tt, planet, ECGEO);
    if (!rad) return { rad2deg * details.geq.lat, rad2deg * details.geq.lon, 0.0 }; // Dec, GHA
    else return { details.geq.lat, details.geq.lon, 0.0 };
}
LLD Astronomy::getDecGHA(size_t planet, double jd_tt, bool rad) { // JD in TT
    if (jd_tt == NO_DOUBLE) jd_tt = m_datetime.jd_tt();
    if (jd_tt == planet_jd[planet]) return { planet_dec[planet], planet_gha[planet], 0.0 };
    CelestialDetail details = getDetails(jd_tt, planet, ECGEO);
    if (!rad) return { rad2deg * details.geq.lat, rad2deg * details.geogha, 0.0 }; // Dec, GHA
    else return { details.geq.lat, details.geogha, 0.0 };
}

// General astronomical adjustments
LLD Astronomy::EclipticAberration(double Beta, double Lambda, double jd_tt, bool rad) {
    // From CAAAberration::EclipticAberration() converted to accept radians
    // See MEEUS98 Chapter 23 for Ron-Vondrák abberation
    double lambda = Lambda;
    double beta = Beta;
    if (!rad) {
        lambda *= deg2rad;
        beta *= deg2rad;
    }
    const double T = (jd_tt - JD_2000) / 36525.0;
    const double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);
    double pi = deg2rad * (102.93735 + T * (1.71946 + 0.00046 * T));
    constexpr double k = 20.49552;
    double SunLongitude = ACoord::rangezero2tau(getEcLon(EARTH, jd_tt) + (tau * 0.5));

    LLD aberration;
    aberration.lon = (-k * cos(SunLongitude - lambda) + e * k * cos(pi - lambda)) / cos(beta) / 3600.0;
    aberration.lat = -k * sin(Beta) * (sin(SunLongitude - lambda) - e * sin(pi - lambda)) / 3600.0;
    if (rad) {
        aberration.lon *= deg2rad;
        aberration.lat *= deg2rad;
    }
    return aberration;
}
LLD Astronomy::EquatorialAberration(double dec, double ra, double jd_tt, bool rad) {
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
    //const CAA3DCoordinate velocity = CAAAberration::EarthVelocity(jd_tt, true); // high precision
    const glm::dvec3 velocity = AEarth::EarthVelocity(jd_tt, VSOP87_FULL);
    LLD aberration;
    aberration.lat = -(((velocity.x * cosAlpha + velocity.y * sinAlpha) * sinDelta - velocity.z * cosDelta) / 17314463350.0);
    aberration.lon = (velocity.y * cosAlpha - velocity.x * sinAlpha) / (17314463350.0 * cosDelta);
    if (rad) {
        aberration.lat *= deg2rad;
        aberration.lon *= deg2rad;
    }
    return aberration;
    // retval in radians
}
LLD Astronomy::FK5Correction(double Latitude, double Longitude, double jd_tt, bool rad) {
    double lat = Latitude;
    double lon = Longitude;
    if (rad) {
        lat *= rad2deg;
        lon *= rad2deg;
    }
    const double T = (jd_tt - JD_2000) / 36525;
    double Ldash = lon - deg2rad * T * (1.397 + 0.00031 * T);
    LLD fk5corr;
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
    //LLD value;
    //value.lon = atan2(A, B) + zeta;
    //value.lat = asin(C);
    //std::cout << "JD: " << JD << "Ra Dec : " << decra.lon << ", " << decra.lat << "->" << value.lon << ", " << value.lat << '\n';
    //return value;
    return { asin(C), atan2(A, B) + zeta, 0.0 };
}
LLD Astronomy::PrecessJ2000DecRA(const LLD decra, const double jd_tt) {  // ALWAYS in RADIANS
    // Precess the Equinox for a geocentric coordinate (catalogue with proper motion applied)
    // AA+: CAAPrecession::PrecessEquatorial()
    // MEEUS92: Chapter 21
    // (Does NOT aberrate)
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
    // Apply aberration
    //CAA2DCoordinate aberration = EquatorialAberration(decra.lon, decra.lat, m_jd, true);
    LLD aberration = EquatorialAberration(decra.lat, decra.lon, m_datetime.jd_tt(), true);
    decra.lat += aberration.lat;
    decra.lon += aberration.lon;
    //std::cout << "Astronomy::getTrueDecRAbyNameJD(): Aberrated RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    // Precession parameters sigma, zeta & phi in arc seconds
    //std::cout << "Astronomy::getTrueDecRAbyNameJD(): Nutated RA(hrs), Dec(deg): " << radecFormat(decra.lon, decra.lat, true) << '\n';
    // Ignore annual parallax
    if (!rad) {
        decra.lat *= rad2deg;
        decra.lon *= rad2deg;
    }
    return decra;
}

// Lunar calculations
LLD Astronomy::MoonTrueEcliptic(double jd_tt, Lunar_Ephemeris eph) {
    // !!! FIX: Maybe make JD optional, defaulting to current, and cache result
    return AMoon::EclipticCoordinates(jd_tt, eph);
}
LLD Astronomy::MoonApparentEcliptic(double jd_tt, Lunar_Ephemeris eph) {
    // !!! FIX: Maybe make JD optional, defaulting to current, and cache result
    LLD moonpos{ 0.0, 0.0, 0.0 };
    moonpos = AMoon::EclipticCoordinates(jd_tt, eph);

    //moonpos += AEarth::EclipticAberration(moonpos.lon, moonpos.lat, jd_tt, VSOP87_FULL);
    //moonpos += FK5::CorrectionInLonLat(moonpos, jd_tt);  // Convert to the FK5 system
    moonpos.lon += AEarth::NutationInLongitude(jd_tt);
    // According to Meeus, nutation is all that is required for MEEUS_SHORT, which is simply a short ELP2000, so same should apply to all?
    return moonpos;
}
LLD Astronomy::MoonTrueEquatorial(double jd_tt, Lunar_Ephemeris eph) {
    // Ecliptic Moon
    LLD emoon = AMoon::EclipticCoordinates(jd_tt, eph);
    //LLD emoon = MoonApparentEcliptic(jd_tt, eph);
    //std::cout << emoon.str_EC() << std::endl;

    // Equatorial Moon
    double Epsilon = TrueObliquityOfEcliptic(jd_tt, true);
    LLD qmoon = Spherical::Ecliptic2Equatorial(emoon, Epsilon, true);
    return qmoon;
}

// Planetary calculations
unsigned int Astronomy::enablePlanet(size_t planet) {
    return ++planet_refcnt[planet];
}
unsigned int Astronomy::disablePlanet(size_t planet) {
    return --planet_refcnt[planet];
}

LLD Astronomy::EclipticalCoordinates(double jd_tt, Planet planet, Ephemeris eph) {
    switch (planet) {
        case A_MERCURY: return AMercury::EclipticCoordinates(jd_tt, eph);
        case A_VENUS:   return AVenus::EclipticCoordinates(jd_tt, eph);
        case A_MARS:    return AMars::EclipticCoordinates(jd_tt, eph);
        case A_JUPITER: return AJupiter::EclipticCoordinates(jd_tt, eph);
        case A_SATURN:  return ASaturn::EclipticCoordinates(jd_tt, eph);
        case A_URANUS:  return AUranus::EclipticCoordinates(jd_tt, eph);
        case A_NEPTUNE: return ANeptune::EclipticCoordinates(jd_tt, eph);
        default: {
            std::cout << "Astronomy::EclipticalCoordinates(): ERROR - Unknown planet id passed" << std::endl;
            assert(false);
        }
    }
}
CelestialDetailFull Astronomy::planetaryDetails(double jd_tt, Planet planet, Ephemeris eph) {
    // return value
    CelestialDetailFull details;

    // Precision parameters for Light Time compensation
    const double prec_angle = 0.00001 * deg2rad;  // 0.00001 degrees = 36 mas (milli arc seconds)
    const double prec_distance = 0.000001;        // AU

    // Iterate to find the positions adjusting for light-time correction if required
    double JD0{ jd_tt };            // jd modified during iteration
    LLD pla{ 0.0, 0.0, 0.0 };       // target planet/sun coordinates
    LLD pla_prev{ 0.0, 0.0, 0.0 };  // previous planet/sun coordinates, during iteration
    glm::dvec3 tgecr_lt{ 0.0 };     // light time adjusted true geocentric ecliptical rectangular coordinates, output of light time iteration

    if (planet != A_SUN) {
        // Calculate the position of the earth first
        const glm::dvec3 earth0_rect{ Spherical::Spherical2Rectangular(AEarth::EclipticCoordinates(JD0, eph)) };
        // True Heliocentric ECliptical Spherical coordinates of the planet
        pla = EclipticalCoordinates(JD0, planet, eph);
        details.thecs = pla;
        // True Geocentric ECliptical Rectangular coordinates of the planet
        details.tgecr = Spherical::Spherical2Rectangular(pla) - earth0_rect;  // OK!
        // Iterate to get Light Time corrected planet position
        bool bRecalc{ true };
        bool bFirst{ true };
        while (bRecalc) {
            //std::cout << "Calculating Planet Light Time compensation." << std::endl;
            if (!bFirst) pla = EclipticalCoordinates(JD0, planet, eph); // For first run, use values calculated just above
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
    details.agecs += AEarth::EclipticAberration(details.agecs.lon, details.agecs.lat, jd_tt, eph);
    details.agecs += FK5::CorrectionInLonLat(details.agecs, jd_tt);  // Convert to the FK5 system
    details.agecs.lon += AEarth::NutationInLongitude(jd_tt);
    details.alt = ACoord::DistanceToLightTime(details.agecs.dst);  // Apparent Light Time
    // Convert to RA and Dec
    const double epsilon = AEarth::TrueObliquityOfEcliptic(jd_tt);
    // True Geocentric Equatorial Spherical
    details.tgeqs = Spherical::Ecliptic2Equatorial(details.tgecs, epsilon, true);
    details.tgeqs.dst = details.tgecs.dst;  // True Geocentric Distance
    // Apparent Geocentric Equatorial Spherical
    details.ageqs = Spherical::Ecliptic2Equatorial(details.agecs, epsilon, true);
    details.ageqs.dst = details.agecs.dst;  // Apparent Geocentric Distance
    details.jd_tt = jd_tt;
    return details;
}

CelestialDetail Astronomy::getDetails2(double jd_tt, size_t planet, unsigned int type, bool hi) {
    // Use ASun and compare to below original
    if (planet != SUN) {
        std::cout << "Astronomy::getDetails2(): ERROR = This is a test function for the Sun, but a different object was supplied.";
        return CelestialDetail{ NO_DOUBLE, { NO_DOUBLE, NO_DOUBLE, NO_DOUBLE}, { NO_DOUBLE, NO_DOUBLE, NO_DOUBLE}, NO_DOUBLE };
    }
    double lon = ASun::GeometricEclipticLongitude(getJD_TT(), VSOP87_FULL);
    double lat = ASun::GeometricEclipticLatitude(getJD_TT(), VSOP87_FULL);
    CelestialDetail retval{ jd_tt, { lat, lon, 0.0 }, { 0.0, 0.0, 0.0 }, 0.0 };
    return retval;
}

CelestialDetail Astronomy::getDetails(double jd_tt, size_t planet, unsigned int type) {
    CelestialDetail details;
    details.jd_tt = jd_tt;
    //Calculate the position of the earth first
    double JD0 = jd_tt;
    const double B0 = getEcLat(EARTH, JD0);  // Radians
    const double L0 = getEcLon(EARTH, JD0);  // Radians
    const double R0 = getEcDst(EARTH, JD0);  // Astronomical Units
    const double cosB0 = cos(B0);
    //Iterate to find the positions adjusting for light-time correction if required
    double B = 0;
    double L = 0;
    double R = 0;
    if (planet != SUN) {
        bool bRecalc = true;
        bool bFirstRecalc = true;
        double BPrevious = 0;
        double LPrevious = 0;
        double RPrevious = 0;
        while (bRecalc) {
            B = getEcLat(planet, JD0); // Radians
            L = getEcLon(planet, JD0); // Radians
            R = getEcDst(planet, JD0); // Astronomical Units
            if (!bFirstRecalc) {
                bRecalc = ((fabs(L - LPrevious) > deg2rad * 0.00001) || (fabs(B - BPrevious) > deg2rad * 0.00001) || (fabs(R - RPrevious) > 0.000001));
                BPrevious = B;
                LPrevious = L;
                RPrevious = R;
            }
            else {
                details.hec.lat = B;
                details.hec.lon = L;
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
        //std::cout << "getDetails(): Earth = L0, B0, R0 " << L0 << ", " << B0 << ", " << R0 << '\n';
        //std::cout << "getDetails(): Earth = x, y, z " << x << ", " << y << ", " << z << '\n';
        
    }
    const double x2 = x * x;
    const double y2 = y * y;
    double appGeoLat = atan2(z, sqrt(x2 + y2));
    double appGeoLon = ACoord::rangezero2tau(atan2(y, x));  // atan2() returns [-pi;+pi]
    double appGeoDst = sqrt(x2 + y2 + z * z);
    //details.ApparentLightTime = CAAElliptical::DistanceToLightTime(appGeoDst);
    //Adjust for Aberration
    const double T = (jd_tt - 2451545) / 36525;
    const double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);
    double pi = deg2rad * (102.93735 + T * (1.71946 + 0.00046 * T));
    constexpr double k = 20.49552;
    double SunLongitude = ACoord::rangezero2tau(getEcLon(EARTH, jd_tt) + (tau * 0.5));
    const double aberrationLon = (-k * cos(SunLongitude - appGeoLon) + e * k * cos(pi - appGeoLon)) / cos(appGeoLat) / 3600;
    const double aberrationLat = -k * sin(appGeoLat) * (sin(SunLongitude - appGeoLon) - e * sin(pi - appGeoLon)) / 3600;
    appGeoLon += deg2rad * aberrationLon;
    appGeoLat += deg2rad * aberrationLat;
    //convert to the FK5 system - Now takes radians
    double Ldash = appGeoLon - deg2rad * T * (1.397 + 0.00031 * T);
    const double fk5corrLon = ACoord::secs2deg(-0.09033 + 0.03916 * (std::cos(Ldash) + std::sin(Ldash)) * tan(appGeoLat));
    const double fk5corrLat = ACoord::secs2deg(0.03916 * (std::cos(Ldash) - std::sin(Ldash)));
    appGeoLon += deg2rad * fk5corrLon;
    appGeoLat += deg2rad * fk5corrLat;
    //Correct for Nutation in Longitude
    const double Tsquared = T * T;
    const double Tcubed = Tsquared * T;
    double D = 297.85036 + (445267.111480 * T) - (0.0019142 * Tsquared) + (Tcubed / 189474);
    D = ACoord::rangezero2threesixty(D);
    double M = 357.52772 + (35999.050340 * T) - (0.0001603 * Tsquared) - (Tcubed / 300000);
    M = ACoord::rangezero2threesixty(M);
    double Mprime = 134.96298 + (477198.867398 * T) + (0.0086972 * Tsquared) + (Tcubed / 56250);
    Mprime = ACoord::rangezero2threesixty(Mprime);
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * Tsquared) + (Tcubed / 327270);
    F = ACoord::rangezero2threesixty(F);
    double omega = 125.04452 - (1934.136261 * T) + (0.0020708 * Tsquared) + (Tcubed / 450000);
    omega = ACoord::rangezero2threesixty(omega);

    double nulon = 0;
    double nutobec = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        const double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        const double radargument = deg2rad * argument;
        nulon += (coeff.sincoeff1 + (coeff.sincoeff2 * T)) * sin(radargument) * 0.0001;
        nutobec += (coeff.coscoeff1 + (coeff.coscoeff2 * T)) * cos(radargument) * 0.0001;
    }
    appGeoLon += deg2rad * ACoord::secs2deg(nulon);
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
        - (ACoord::secs2deg(4680.93) * U)
        - (ACoord::secs2deg(1.55) * Usquared)
        + (ACoord::secs2deg(1999.25) * Ucubed)
        - (ACoord::secs2deg(51.38) * U4)
        - (ACoord::secs2deg(249.67) * U5)
        - (ACoord::secs2deg(39.05) * U6)
        + (ACoord::secs2deg(7.12) * U7)
        + (ACoord::secs2deg(27.87) * U8)
        + (ACoord::secs2deg(5.79) * U9)
        + (ACoord::secs2deg(2.45) * U10)
        // Nutation in Obliquity of Ecliptic already calculated in Nutation of Longitude above
        + ACoord::secs2deg(nutobec);
    //Convert to RA and Dec
    LLD ApparentEqu = calcEc2Geo(appGeoLat, appGeoLon, deg2rad * obliqEc, true);
    details.geq.lon = ApparentEqu.lon; // RA
    details.geogha = ApparentGreenwichSiderealTime(EDateTime::getJDTT2UTC(jd_tt), true) - details.geq.lon;
    details.geq.lat = ApparentEqu.lat; // Dec
    return details;
}

CelestialDetail Astronomy::getDetailsNew(double jd_tt, size_t planet, unsigned int type) {
    //bool debug = true;
    bool debug = false;
    Ephemeris eph = VSOP87_FULL;
    //Ephemeris eph = VSOP87_SHORT;
    CelestialDetail details;
    details.jd_tt = jd_tt;
    //Calculate the position of the earth first
    double JD = jd_tt;  // JD will later be varied by light speed distance to find the true position of the planet

    const LLD earth0{ 0.0, 0.0, 0.0 };

    const double B0 = getEcLat(EARTH, JD, eph);  // Radians
    const double L0 = getEcLon(EARTH, JD, eph);  // Radians
    const double R0 = getEcDst(EARTH, JD, eph);  // Astronomical Units
    const double cosB0 = cos(B0);
    const double cosL0 = cos(L0);
    const double sinB0 = sin(B0);
    const double sinL0 = sin(L0);
    //Iterate to find the positions adjusting for light-time correction if required
    double B = 0;
    double L = 0;
    double R = 0;

    if (planet != SUN) {
        bool bRecalc = true;
        bool bFirstRecalc = true;
        double BPrevious = 0;
        double LPrevious = 0;
        double RPrevious = 0;

        while (bRecalc) {
            B = getEcLat(planet, JD, eph);                   // Radians
            L = getEcLon(planet, JD, eph);                   // Radians
            R = getEcDst(planet, JD, eph);  // Astronomical Units
            if (debug) {
                std::cout << " JD,B,L,R: " << JD << "," << B << "," << L << "," << R << "\n";
            }
            if (!bFirstRecalc) {
                bRecalc = ((fabs(L - LPrevious) > deg2rad * 0.00001) || (fabs(B - BPrevious) > deg2rad * 0.00001) || (fabs(R - RPrevious) > 0.000001));
                BPrevious = B;
                LPrevious = L;
                RPrevious = R;
            }
            else {
                details.hec.lat = B;
                details.hec.lon = L;
                details.hec.dst = R;
                bFirstRecalc = false;
                if (type == EC) return details;  // Only Heliocentric Ecliptic coordinates
            }
            //Calculate the new value
            if (bRecalc) {
                const double cosB = cos(B);
                //const double cosL = cos(L);
                const double x = R * cosB * cos(L) - R0 * cosB0 * cosL0;
                const double y = R * cosB * sin(L) - R0 * cosB0 * sinL0;
                const double z = R * sin(B) - R0 * sinB0;
                const double distance = sqrt(x * x + y * y + z * z);

                //Prepare for the next loop around
                JD = jd_tt - Distance2LightTime(distance);
            }
        }
    }

    // Here we have apparent heliocentric ecliptic spherical coordinates

    // Convert to geocentric via rectangular coordinates
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
        //std::cout << "getDetails(): Earth = L0, B0, R0 " << L0 << ", " << B0 << ", " << R0 << '\n';
        //std::cout << "getDetails(): Earth = x, y, z " << x << ", " << y << ", " << z << '\n';

    }
    const double x2 = x * x;
    const double y2 = y * y;
    //LLD appGeo{ 0.0, 0.0, 0.0 };
    double appGeoLat = atan2(z, sqrt(x2 + y2));
    double appGeoLon = ACoord::rangezero2tau(atan2(y, x));  // atan2() returns [-pi;+pi]
    double appGeoDst = sqrt(x2 + y2 + z * z);
    //details.ApparentLightTime = CAAElliptical::DistanceToLightTime(appGeoDst);

    //Adjust for Ecliptic Aberration
    const double T = (jd_tt - JD_2000) / JD_CENTURY;
    const double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);
    double pi = deg2rad * (102.93735 + T * (1.71946 + 0.00046 * T));
    constexpr double k = 20.49552;
    double SunLongitude = ACoord::rangezero2tau(L0 + (tau * 0.5));
    // double SunLongitude2 = ASun::GeometricEclipticLongitude(jd_tt, VSOP87_SHORT);
    // std::cout << "Sun Lon: " << SunLongitude << " " << SunLongitude2 << std::endl;
    // Sun Lon: 2.91424565486794 2.91424565486794

    const double aberrationLon = (-k * cos(SunLongitude - appGeoLon) + e * k * cos(pi - appGeoLon)) / cos(appGeoLat) / 3600;
    const double aberrationLat = -k * sin(appGeoLat) * (sin(SunLongitude - appGeoLon) - e * sin(pi - appGeoLon)) / 3600;
    appGeoLon += deg2rad * aberrationLon;
    appGeoLat += deg2rad * aberrationLat;

    //convert to the FK5 system - Now takes radians
    double Ldash = appGeoLon - deg2rad * T * (1.397 + 0.00031 * T);
    const double fk5corrLon = ACoord::secs2deg(-0.09033 + 0.03916 * (std::cos(Ldash) + std::sin(Ldash)) * tan(appGeoLat));
    const double fk5corrLat = ACoord::secs2deg(0.03916 * (std::cos(Ldash) - std::sin(Ldash)));
    appGeoLon += deg2rad * fk5corrLon;
    appGeoLat += deg2rad * fk5corrLat;

    //Correct for Nutation in Longitude
    const double Tsquared = T * T;
    const double Tcubed = Tsquared * T;
    double D = 297.85036 + (445267.111480 * T) - (0.0019142 * Tsquared) + (Tcubed / 189474);
    D = ACoord::rangezero2threesixty(D);
    double M = 357.52772 + (35999.050340 * T) - (0.0001603 * Tsquared) - (Tcubed / 300000);
    M = ACoord::rangezero2threesixty(M);
    double Mprime = 134.96298 + (477198.867398 * T) + (0.0086972 * Tsquared) + (Tcubed / 56250);
    Mprime = ACoord::rangezero2threesixty(Mprime);
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * Tsquared) + (Tcubed / 327270);
    F = ACoord::rangezero2threesixty(F);
    double omega = 125.04452 - (1934.136261 * T) + (0.0020708 * Tsquared) + (Tcubed / 450000);
    omega = ACoord::rangezero2threesixty(omega);
    double nulon = 0;
    double nutobec = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        const double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        const double radargument = deg2rad * argument;
        nulon += (coeff.sincoeff1 + (coeff.sincoeff2 * T)) * sin(radargument) * 0.0001;
        nutobec += (coeff.coscoeff1 + (coeff.coscoeff2 * T)) * cos(radargument) * 0.0001;
    }
    appGeoLon += deg2rad * ACoord::secs2deg(nulon);

    //Obtain True Obliquity of Ecliptic
    // Mean Obliquity of Ecliptic
    const double U = (jd_tt - JD_2000) / 3652500;
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
        - (ACoord::secs2deg(4680.93) * U)
        - (ACoord::secs2deg(1.55) * Usquared)
        + (ACoord::secs2deg(1999.25) * Ucubed)
        - (ACoord::secs2deg(51.38) * U4)
        - (ACoord::secs2deg(249.67) * U5)
        - (ACoord::secs2deg(39.05) * U6)
        + (ACoord::secs2deg(7.12) * U7)
        + (ACoord::secs2deg(27.87) * U8)
        + (ACoord::secs2deg(5.79) * U9)
        + (ACoord::secs2deg(2.45) * U10)
        // Nutation in Obliquity of Ecliptic already calculated in Nutation of Longitude above
        + ACoord::secs2deg(nutobec);

    //Convert to RA and Dec
    //LLD ApparentEqu = calcEc2Geo(appGeoLat, appGeoLon, deg2rad * obliqEc, true);
    //details.geq.lat = ApparentEqu.lat; // Dec
    //details.geq.lon = ApparentEqu.lon; // RA
    details.geq = calcEc2Geo(appGeoLat, appGeoLon, deg2rad * obliqEc, true);
    details.geogha = ApparentGreenwichSiderealTime(EDateTime::getJDTT2UTC(jd_tt), true) - details.geq.lon;
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
double Astronomy::getEcLat(size_t planet, double jd_tt, Ephemeris eph) {
    // Heliocentric Ecliptic Latitude (ref. Equinox of Epoch) in radians
    // Unconditionally caching may or may not be desired. Consider a flag, or rename the function so it is clear that getEcLat{planet}() may be preferable. !!!
    if (planet == MERCURY) planet_ecLat[planet] = AMercury::EclipticLatitude(jd_tt, eph);
    else if (planet == VENUS) planet_ecLat[planet] = AVenus::EclipticLatitude(jd_tt, eph);
    else if (planet == EARTH) planet_ecLat[planet] = AEarth::EclipticLatitude(jd_tt, eph);
    else if (planet == MARS) planet_ecLat[planet] = AMars::EclipticLatitude(jd_tt, eph);
    else if (planet == JUPITER) planet_ecLat[planet] = AJupiter::EclipticLatitude(jd_tt, eph);
    else if (planet == SATURN) planet_ecLat[planet] = ASaturn::EclipticLatitude(jd_tt, eph);
    else if (planet == URANUS) planet_ecLat[planet] = AUranus::EclipticLatitude(jd_tt, eph);
    else if (planet == NEPTUNE) planet_ecLat[planet] = ANeptune::EclipticLatitude(jd_tt, eph);
    else std::cout << "APlanet::getEcLat(): planet unknown: " << planet << "\n";
    return planet_ecLat[planet];
}
double Astronomy::getEcLon(size_t planet, double jd_tt, Ephemeris eph) {
    // Heliocentric Ecliptic Longitude (ref. Equinox of Epoch) in radians
    if (planet == MERCURY) planet_ecLon[planet] = AMercury::EclipticLongitude(jd_tt, eph);
    else if (planet == VENUS) planet_ecLon[planet] = AVenus::EclipticLongitude(jd_tt, eph);
    else if (planet == EARTH) planet_ecLon[planet] = AEarth::EclipticLongitude(jd_tt, eph);
    else if (planet == MARS) planet_ecLon[planet] = AMars::EclipticLongitude(jd_tt, eph);
    else if (planet == JUPITER) planet_ecLon[planet] = AJupiter::EclipticLongitude(jd_tt, eph);
    else if (planet == SATURN) planet_ecLon[planet] = ASaturn::EclipticLongitude(jd_tt, eph);
    else if (planet == URANUS) planet_ecLon[planet] = AUranus::EclipticLongitude(jd_tt, eph);
    else if (planet == NEPTUNE) planet_ecLon[planet] = ANeptune::EclipticLongitude(jd_tt, eph);
    else std::cout << "APlanet::getEcLon(): planet unknown: " << planet << "\n";
    return planet_ecLon[planet];
}
double Astronomy::getEcDst(size_t planet, double jd_tt, Ephemeris eph) {
    // Heliocentric Ecliptic Radius (ref. Equinox of Epoch) in kilometers
    if (planet == MERCURY) planet_ecRadius[planet] = AMercury::EclipticDistance(jd_tt, eph);
    else if (planet == VENUS) planet_ecRadius[planet] = AVenus::EclipticDistance(jd_tt, eph);
    else if (planet == EARTH) planet_ecRadius[planet] = AEarth::EclipticDistance(jd_tt, eph);
    else if (planet == MARS) planet_ecRadius[planet] = AMars::EclipticDistance(jd_tt, eph);
    else if (planet == JUPITER) planet_ecRadius[planet] = AJupiter::EclipticDistance(jd_tt, eph);
    else if (planet == SATURN) planet_ecRadius[planet] = ASaturn::EclipticDistance(jd_tt, eph);
    else if (planet == URANUS) planet_ecRadius[planet] = AUranus::EclipticDistance(jd_tt, eph);
    else if (planet == NEPTUNE) planet_ecRadius[planet] = ANeptune::EclipticDistance(jd_tt, eph);
    else std::cout << "APlanet::getRadius(): planet unknown: " << planet << "\n";
    return planet_ecRadius[planet];
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
    if (EDateTime::isLeapYear((long)year) && month > 2.0) days += 1;
    if (year < 1970) days -= 1;  // 
    return days + (int)day - 1;  // -1 because UnixTime starts on day zero, not day one.
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
    angle = ACoord::rangezero2twentyfour(angle);
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
    std::string slon = angle2uDMSstring(lon, rad);  // ditto, because we do N/S and E/W below
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
    planet_gha[planet] = ACoord::rangezero2tau(planet_gha[planet]);
    planet_jd[planet] = m_datetime.jd_tt();
    //planet_dst[planet] = au2km * details.ApparentGeocentricDistance;
    //std::cout << "Planet, RA, Dec, GSid: " << planet << ", " << planet_ra[planet] << ", " << planet_dec[planet] << ", " << m_gsidtime << "\n";
}
void Astronomy::updateAGsid() { // Updates GSid, and sets nutation in the process
    // If these functions cache results, it is a catch 22. So have separate private functions for these updates:
    //m_meanobliquity = MeanObliquityOfEcliptic(m_datetime.jd_tt(), true);
    m_meanobliquity = AEarth::MeanObliquityOfEcliptic(m_datetime.jd_tt());
    m_trueobliquity = m_meanobliquity + AEarth::NutationInObliquity(m_datetime.jd_tt());
    m_nutationinlongitude = NutationInLongitude(m_datetime.jd_tt(), true);
    m_meangsid = updateMGsid(m_datetime.jd_utc());
    m_gsidtime = m_meangsid + (m_nutationinlongitude * cos(m_trueobliquity));
}
double Astronomy::updateMGsid(double jd_utc) {
    double jd_fraction = jd_utc - std::floor(jd_utc) + 0.5; //also good for negative jd
    double T{ (jd_utc - jd_fraction - JD_2000) / JD_CENTURY };
    double TSquared{ T * T };
    double TCubed{ TSquared * T };
    double Value{ 100.46061837 + (36000.770053608 * T) + (0.000387933 * TSquared) - (TCubed / 38710000) };
    Value += jd_fraction * 24.0 * 15.0 * 1.00273790935;
    return ACoord::rangezero2tau(deg2rad * Value);
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
    updateAGsid();          // Also updates nutation and obliquity caches
    updatePrecession();
    updateTimeString();

    // Update cached planetary positions to current time. Later, include Sun, Moon & Earth?
    for (unsigned int p = SUN; p <= NEPTUNE; p++) {
        if (planet_refcnt[p] > 0) updateGeocentric(p);
    }
    // Update cached planetary paths
    updateCelestialPaths();
}

