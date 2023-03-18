#include "Astronomy.h"
#include "Earth.h"
#include <array>

// Coordinate systems:
//  + Heliocentric ecliptic = Ecliptic Latitude, Ecliptic Longitude, Radius Vector
//  + Geocentric equatorial = Declination, Right Ascension, Distance
//  - Topocentic horizontal = Elevation, Azimuth, Distance
//  - Heliocentric cartesian  = X, Y, Z
//  - Geocentric cartesian  = X, Y, Z

// Time functions:
//  Create a time delta unit
//  Increase / decrease by delta unit
//  Set to specific JD
//  Set to specific UTC
//  Set to specific UNIX TimeStamp

// How to deal with getting calculations for paths vs reusing calculations for current moment?
// Maybe calling without param jd for current time, and only updating stored values if calling a time function


//  Protos
class Earth;


// ---------------
//  CelestialPath
// ---------------
CelestialPath::CelestialPath(Astronomy* celmec, size_t planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, bool fixed)
    : m_astro(celmec), jdStart(startoffset), jdEnd(endoffset), jdSteps(steps), cpType(type), fixedpath(fixed) {
    // Also have the option of a fixed CelestialPath that doesn't follow/update with current JD. !!!
    this->planet = planet;
    //std::cout << "CelestialPath() constructor, CelestialMech* is: " << celmec << "\n";
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
    double currentjd = m_astro->getJD();
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
Astronomy::Astronomy() {
    //std::cout << "Astronomy{" << this << "} created!\n";
    setTimeNow();  // Ensure a default time is set
    if (stellarobjects_loaded) return;
    loadStellarObjects();
}
Astronomy::~Astronomy() {
    //std::cout << "Astronomy{" << this << "} destroyed!\n";
}
bool Astronomy::stellarobjects_loaded = false;
double Astronomy::epoch = 2000.0;
// These two lines and the loadStellarObjects() function below, with help from:
// https://stackoverflow.com/questions/7531981/how-to-instantiate-a-static-vector-of-object
// (Objective was to have all stellar object lookups handled by Astronomy without loading a duplicate DB for each Astronomy instantiated)
std::vector<Astronomy::stellarobject> Astronomy::stellarobjects;
std::vector<Astronomy::stellarobject_xref> Astronomy::stellarobject_xrefs;
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

    char outsep = ','; // Output separator
    std::ifstream streami("C:\\Coding\\Eartharium\\simbad-raw.csv");
    std::ofstream streamo("C:\\Coding\\Eartharium\\simbad-export.csv");
    if (!streami.is_open()) {
        std::cout << "Did not manage to open SIMBAD Raw File!\n";
        return;
    }
    if (!streamo.is_open()) {
        std::cout << "Did not manage to open SIMBAD Export File!\n";
        return;
    }
    std::istringstream parse, parse2;
    std::string line, item, item2;
    for (int j = 0; j < 9; j++) std::getline(streami, line);

    streamo << "#" << outsep << "id" << outsep << "typ" << outsep << "icrs_ra_h" << outsep << "icrs_ra_m" << outsep << "icrs_ra_s"
        << outsep << "icrs_dec_d" << outsep << "icrs_dec_m" << outsep << "icrs_dec_s" << outsep << "fk5_ra_h" << outsep << "fk5_ra_m"
        << outsep << "fk5_ra_s" << outsep << "fk5_dec_d" << outsep << "fk5_dec_m" << outsep << "fk5_dec_s" << outsep << "fk4_ra_h"
        << outsep << "fk4_ra_m" << outsep << "fk4_ra_s" << outsep << "fk4_dec_d" << outsep << "fk4_dec_m" << outsep << "fk4_dec_s"
        << outsep << "gal_ra" << outsep << "gal_dec" << outsep << "pm_ra" << outsep << "pm_dec" << outsep << "plx"
        << outsep << "Umag" << outsep << "Bmag" << outsep << "Vmag" << outsep << "Rmag" << outsep << "Gmag" << outsep << "Imag"
        << outsep << "spec.type\n";
    unsigned int i = 0;
    while (getline(streami, line)) {
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
            return { rad ? deg2rad * s.dec : s.dec, rad ? deg2rad * s.ra : s.ra, 0.0 }; // File was loaded in degrees
        }
    }
    std::cout << "ERROR! Astronomy::getDecRAbyName() - Unknown name supplied: " << starname << '\n';
    return { 0.0, 0.0, NO_DOUBLE };
}
LLH Astronomy::getDecRAwithPMbyName(const std::string starname, double jd, bool rad) {
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
            double elapsedyears = (jd - 2451545.0) / 365.25;
            double dec = s.dec + s.pm_dec * elapsedyears / 3600000.0;
            // NOTE: SIMBAD proper motions are from the Hipparcos mission, the right ascension value is already multiplied by cos(s.dec)
            //double ra = s.ra + s.pm_ra / cos(deg2rad * s.dec) * elapsedyears / 3600000.0;
            double ra = s.ra + s.pm_ra * elapsedyears / 3600000.0;
            return { rad ? deg2rad * dec : dec, rad ? deg2rad * ra : ra, 0.0 }; // File was loaded in degrees
        }
    }
    std::cout << "ERROR! Astronomy::getDecRAwithPMbyName() - Unknown name supplied: " << starname << '\n';
    return { 0.0, 0.0, NO_DOUBLE };
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
            return glm::vec4((float)s.red, (float)s.green, (float)s.blue, 1.0f); // File was loaded in degrees
        }
    }
    std::cout << "ERROR! Astronomy::getColorbyName() - Unknown name supplied: " << starname << '\n';
    return NO_COLOR;
}

void Astronomy::setTime(long yr, long mo, double da, double hr, double mi, double se, bool gre) {
    eot = 0.0;
    // TODO: Normalize entries !!!
    //while (hr >= 24.0) { hr -= 24.0; da += 1.0; }  // !!! Untested...
    //while (hr < 0.0) { hr += 24.0; da -= 1.0; }    // !!! Untested...
    m_date = CAADate(yr, mo, da, hr, mi, se + 0.01, gre);
    m_jd = m_date.Julian();
    m_year = yr;
    m_month = mo;
    m_day = da;
    m_hour = hr;
    m_minute = mi;
    m_second = se;
    m_gregorian = gre;
    update();
}
void Astronomy::setTimeNow() {
    time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    //#pragma warning(disable : 4996)
    tm utc_tm = *gmtime(&tt);   // Using tm has a maximum resolution of 1 second.
    setTime((long)utc_tm.tm_year + 1900, (long)utc_tm.tm_mon + 1, (double)utc_tm.tm_mday,
        (double)utc_tm.tm_hour, (double)utc_tm.tm_min, (double)utc_tm.tm_sec, true);
}
void Astronomy::setJD(double jd, bool gregorian) {
    eot = 0.0;
    m_jd = jd;
    m_gregorian = gregorian;
    m_date = CAADate(m_jd, m_gregorian);  // When we have m_date, do we really need to keep all the below around too?
    m_year = m_date.Year();  // NOTE: CAADate calculates all of these with .Get() at every one of these calls, so maybe copy that into a local method
    m_month = m_date.Month();
    m_day = m_date.Day();
    m_hour = m_date.Hour();
    m_minute = m_date.Minute();
    m_second = m_date.Second();
    update();
}
double Astronomy::getJD() {
    //std::cout << "Astronomy{" << this << "}::getJD()\n";
    return m_jd;
}
void Astronomy::addTime(double d, double h, double min, double sec, bool do_eot) {
    /// <summary>
    /// Public method to increase (or decrease if negative) the current time. It will automatically update internal state.
    /// </summary>
    // No sense in adding years and months, those are not consistent durations. Well, they still advance time, and CAADate() is duration aware.
    if (do_eot && eot == 0.0) eot = CAAEquationOfTime::Calculate(m_jd, true);
    if (eot != 0.0 && do_eot) m_minute += eot; // If EoT was applied last time update, remove it.
    //std::cout << "Astronomy::addTime() - Remove EoT: " << eot << '\n';
    if (!do_eot) eot = 0.0;
    m_day += d;
    m_hour += h;
    m_minute += min;
    m_second += sec;
    if (do_eot) {
        m_date = CAADate(m_year, m_month, m_day, m_hour, m_minute, m_second, m_gregorian);
        m_jd = m_date.Julian();
        eot = CAAEquationOfTime::Calculate(m_jd, true);
        //std::cout << "Astronomy::addTime() -  Apply EoT: " << eot << '\n';
        m_minute -= eot;
    }
    // Time variables are most likely not within bounds anymore, so recalculate them all via CAADate()
    // Note: This introduces some small inaccuracies around 0.000006 seconds.
    m_date = CAADate(m_year, m_month, m_day, m_hour, m_minute, m_second, m_gregorian);
    m_jd = m_date.Julian();
    m_year = m_date.Year();
    m_month = m_date.Month();
    m_day = m_date.Day();
    m_hour = m_date.Hour();
    m_minute = m_date.Minute();
    m_second = m_date.Second();
    update();
}
void Astronomy::dumpCurrentTime(unsigned int frame) {
    if (frame == NO_UINT) std::cout << "Current astronomical time at frame none:\n";
    else std::cout << "Current astronomical time at frame " << frame << ":\n";
    std::cout << " - Gregorian: " << (m_gregorian ? "true" : "false") << '\n';
    std::cout << " - Year: " << m_year << '\n';
    std::cout << " - Month: " << m_month << '\n';
    std::cout << " - Day: " << m_day << '\n';
    std::cout << " - Hour: " << m_hour << '\n';
    std::cout << " - Minute: " << m_minute << '\n';
    std::cout << " - Second: " << m_second << '\n';
    // std::cout << "Astronomy::setTime("<<m_year<<", "<<m_month
}
void Astronomy::setUnixTime(double utime) {
    setJD(getUnixTime2JD(utime), true); // NOTE: Assuming gregorian due to the valid range of Unix timestamps
}
double Astronomy::calculateGsid(double jd) {
    /// <summary>
    /// Public method to obtain Greenwich Sidereal Time from Julian Date.
    /// </summary>
    return hrs2rad * CAASidereal::ApparentGreenwichSiderealTime(jd);
}
double Astronomy::getGsid(double jd) {
    /// <summary>
    /// Public method to obtain Greenwich Sidereal Time from Julian Date. If no Julian Date is supplied, current time is used.
    /// </summary>
    if (jd == 0.0 || jd == m_jd) return m_gsidtime;
    else return calculateGsid(jd);
}
double Astronomy::getEoT(double jd) {
    if (jd == 0.0) jd = m_jd;
    return CAAEquationOfTime::Calculate(jd, true);
}
void Astronomy::getTimeString(char* dstring) {
    // Builds and returns a date time string from current JD
    long y, mo, d, h, mi;
    double s;
    m_date.Get(y, mo, d, h, mi, s);
    sprintf(dstring, "%04d-%02d-%02d %02d:%02d:%02.0f UTC\n", y, mo, d, h, mi, s);
}
void Astronomy::updateTimeString() {
    // Triggers an update of the internally stored std::string Astronomy::timestr based on current JD
    long y, mo, d, h, mi;
    double s;
    m_date.Get(y, mo, d, h, mi, s);
    sprintf((char*)timestr.c_str(), "%04d-%02d-%02d %02d:%02d:%02.0f UTC\n", y, mo, d, h, mi, s);
}
void Astronomy::DateTimeString(CAADate* date, char* dstring) {
    // dstring should be at least 21 chars long
    sprintf(dstring, "%04d-%02d-%02d %02d:%02d:%02.0f UTC", date->Year(), date->Month(), date->Day(), date->Hour(), date->Minute(), date->Second());
}
void Astronomy::JulianDateTimeString(double jd, char* dstring) {
    // dstring should be at least 21 chars long
    CAADate date = CAADate(jd, true);
    sprintf(dstring, "%04d-%02d-%02d %02d:%02d:%02.0f UTC", date.Year(), date.Month(), date.Day(), date.Hour(), date.Minute(), date.Second());
}
int Astronomy::getString2UnixTime(std::string& string) {
    // TODO: Implement like in Eartharium.cpp:TestArea5(), but resilient to missing seconds, and accepting either '/' or '-' date separator
    return 0;
}
int Astronomy::getDateTime2UnixTime(double year, double month, double day, double hour, double minute, double second) {
    // Verified to work with negative Unix timestamps too, i.e. dates before epoch (1970-01-01 00:00:00)
    int y = (int)year - 1900;
    int ydays = (int)calcYearDay(year, month, day);
    //std::cout << "Day of Year: " << ydays << "\n";
    int utime = (int)second + (int)minute * 60 + (int)hour * 3600; // Day fraction
    //std::cout << "Day fraction: " << utime << "\n";
    utime += ydays * 86400; // Month and day as calculated above, including current leap year
    utime += (y - 70) * 31536000 + ((y- 69) / 4) * 86400 - ((y - 1) / 100) * 86400 + ((y + 299) / 400) * 86400; // Previous leap years adjustment
    return utime;
}
double Astronomy::getJD2UnixTime(double jd) {
    if (jd == 0.0) jd = m_jd;
    return (jd - 2440587.5) * 86400;
}
double Astronomy::getUnixTime2JD(double ut) {
    return ut / 86400 + 2440587.5;
}
int Astronomy::calcYearDay(double year, double month, double day) {
    // Cannot handle most un-normalized dates correctly, so provide sane input dates
    static int months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int mo = (int)(month - 1.0);
    int days = 0;
    for (int m = 0; m < mo; m++) {
        days += months[m];
    }
    if (isLeapYear(year) && month > 2.0) days += 1;
    if (year < 1970) days -= 1;
    return days + (int)day - 1;
}
bool Astronomy::isLeapYear(double year) {
    int y = (int)year;
    //if (((y % 4 == 0) && (y % 100 != 0)) || (y % 400 == 0)) return true;
    if (y % 400 == 0) return true;
    if (y % 100 == 0) return false;
    if (y % 4 == 0) return true;
    return false;
}
unsigned int Astronomy::enablePlanet(size_t planet) {
    return ++planet_refcnt[planet];
}
unsigned int Astronomy::disablePlanet(size_t planet) {
    return --planet_refcnt[planet];
}
LLH Astronomy::getDecRA(size_t planet, double jd) {
    if (jd == 0.0) jd = m_jd;
    if (jd == planet_jd[planet]) return { planet_dec[planet], planet_ra[planet], 0.0 };
    CelestialDetail details = getDetails(jd, planet, ECGEO);
    return { details.geodec, details.geora, 0.0 };
}
LLH Astronomy::getDecGHA(size_t planet, double jd, bool rad) {
    if (jd == 0.0) jd = m_jd;
    if (jd == planet_jd[planet]) return { planet_dec[planet], planet_gha[planet], 0.0 };
    CelestialDetail details = getDetails(jd, planet, ECGEO);
    if (!rad) return { rad2deg * details.geodec, rad2deg * details.geogha, 0.0 };
    else return { details.geodec, details.geogha, 0.0 };
}
void Astronomy::updateGeocentric(size_t planet) {
    if (planet_jd[planet] == m_jd) return;  // Skip if time has not updated. Ideally we should never be called at all in that case.
    CelestialDetail details = getDetails(m_jd, planet, ECGEO);
    planet_dec[planet] = details.geodec;
    planet_ra[planet] = details.geora;
    planet_gha[planet] = details.geogha;
    planet_gha[planet] = rangezero2tau(planet_gha[planet]);
    planet_jd[planet] = m_jd;
    //planet_dst[planet] = au2km * details.ApparentGeocentricDistance;
    //std::cout << "Planet, RA, Dec, GSid: " << planet << ", " << planet_ra[planet] << ", " << planet_dec[planet] << ", " << m_gsidtime << "\n";
}
LLH Astronomy::calcEc2Geo(double Lambda, double Beta, double Epsilon) {
    LLH Equatorial;
    Equatorial.lon = atan2(sin(Lambda) * cos(Epsilon) - tan(Beta) * sin(Epsilon), cos(Lambda));
    if (Equatorial.lon < 0)
        Equatorial.lon += tau;
    Equatorial.lat = asin(sin(Beta) * cos(Epsilon) + cos(Beta) * sin(Epsilon) * sin(Lambda));
    return Equatorial;
}
LLH Astronomy::calcGeo2Topo(LLH pos, LLH loc) {
    // TODO: Does NOT account for altitude of observer !!!
    // pos is DecGHA, loc is LatLon
    // NOTE: No caching, not likely to be called with same position twice for same JD
    double LocalHourAngle = pos.lon + loc.lon;
    // from CAACoordinateTransformation::Equatorial2Horizontal(rad2hrs * LocalHourAngle, rad2deg * pos.lat, rad2deg * loc.lat);
    LLH topo;
    topo.lon = atan2(sin(LocalHourAngle), cos(LocalHourAngle) * sin(loc.lat) - tan(pos.lat) * cos(loc.lat));
    topo.lat = asin(sin(loc.lat) * sin(pos.lat) + cos(loc.lat) * cos(pos.lat) * cos(LocalHourAngle));
    return topo;
}
double inline Astronomy::secs2deg(double seconds) {
    return seconds / 3600;
}
double Astronomy::rangezero2tau(double rad) { // snap radian value to 0 to 2pi range
    double fResult = fmod(rad, tau);
    if (fResult < 0)
        fResult += tau;
    return fResult;
}
double Astronomy::rangepi2pi(double rad) { // snap radian value to -pi to pi range
    double fResult = rangezero2tau(rad);
    if (fResult > 1.5 * pi)
        fResult = fResult - tau;
    else if (fResult > pi)
        fResult = pi - fResult;
    else if (fResult > pi2)
        fResult = pi - fResult;
    return fResult;
}
void Astronomy::stringRad2DMS(double rad, char* dstring) {
    double deg = abs(rad2deg * rad);
    double dg = floor(deg) * std::copysign(1.0, rad);
    double min = 60 * (deg - abs(dg));
    double mi = floor(min);
    double sec = 60 * (min - mi);
    int d = (int)dg;
    int m = (int)mi;
    sprintf(dstring, "%03d\xF8%02d\'%02.2f\"", d, m, sec);
}
void Astronomy::stringRad2HMS(double rad, char* dstring) {
    double hrs = rad2hrs * rad;
    double hr = floor(hrs);
    double min = 60 * (abs(hrs) - hr);
    double mi = floor(min);
    double sec = 60 * (min - mi);
    int h = (int)hr;
    int m = (int)mi;
    sprintf(dstring, "%02dh%02dm%02.2fs", h, m, sec);
}
void Astronomy::stringDeg2DMS(double deg, char* dstring) {
    double deg2 = abs(deg);
    double dg = floor(deg2) * std::copysign(1.0, deg);
    double min = 60 * (deg2 - abs(dg));
    double mi = floor(min);
    double sec = 60 * (min - mi);
    int d = (int)dg;
    int m = (int)mi;
    sprintf(dstring, "%03d\xF8%02d\'%02.2f\"", d, m, sec);
}
double Astronomy::getEclipticObliquity(double jd, bool rad) {
    // jd defaults to current time, rad defaults to false
    if (jd == NO_DOUBLE) jd = m_jd;
    return rad ? CAANutation::TrueObliquityOfEcliptic(jd) * deg2rad : CAANutation::TrueObliquityOfEcliptic(jd);
}
CelestialDetail Astronomy::getDetails(double JD, size_t planet, unsigned int type) {
    CelestialDetail details;
    details.jd = JD;
    //Calculate the position of the earth first
    double JD0 = JD;
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
                details.eclon = L;
                details.eclat = B;
                details.ecdst = R;
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
                JD0 = JD - CAAElliptical::DistanceToLightTime(distance);
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
    double appGeoLon = rangezero2tau(atan2(y, x));
    double appGeoDst = sqrt(x2 + y2 + z * z);
    //details.ApparentLightTime = CAAElliptical::DistanceToLightTime(appGeoDst);
    //Adjust for Aberration
    const double T = (JD - 2451545) / 36525;
    const double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);
    double pi = deg2rad * (102.93735 + T * (1.71946 + 0.00046 * T));
    constexpr double k = 20.49552;
    double SunLongitude = rangezero2tau(EcLonEarth(JD) + (tau * 0.5));
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
    D = CAACoordinateTransformation::MapTo0To360Range(D);
    double M = 357.52772 + (35999.050340 * T) - (0.0001603 * Tsquared) - (Tcubed / 300000);
    M = CAACoordinateTransformation::MapTo0To360Range(M);
    double Mprime = 134.96298 + (477198.867398 * T) + (0.0086972 * Tsquared) + (Tcubed / 56250);
    Mprime = CAACoordinateTransformation::MapTo0To360Range(Mprime);
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * Tsquared) + (Tcubed / 327270);
    F = CAACoordinateTransformation::MapTo0To360Range(F);
    double omega = 125.04452 - (1934.136261 * T) + (0.0020708 * Tsquared) + (Tcubed / 450000);
    omega = CAACoordinateTransformation::MapTo0To360Range(omega);
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
    const double U = (JD - 2451545) / 3652500;
    const double Usquared = U * U;
    const double Ucubed = Usquared * U;
    const double U4 = Ucubed * U;
    const double U5 = U4 * U;
    const double U6 = U5 * U;
    const double U7 = U6 * U;
    const double U8 = U7 * U;
    const double U9 = U8 * U;
    const double U10 = U9 * U;
    double obliqEc = CAACoordinateTransformation::DMSToDegrees(23, 26, 21.448)
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
    LLH ApparentEqu = calcEc2Geo(appGeoLon, appGeoLat, deg2rad * obliqEc);
    details.geora = ApparentEqu.lon;
    details.geogha = getGsid(JD) - details.geora;
    details.geodec = ApparentEqu.lat;
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
CAA2DCoordinate Astronomy::EclipticAberration(double Lambda, double Beta, double JD) {
    // From CAAAberration::EclipticAberration() converted to accept radians
    const double T = (JD - JD2000) / 36525;
    const double e = 0.016708634 - T * (0.000042037 + 0.0000001267 * T);
    double pi = deg2rad * (102.93735 + T * (1.71946 + 0.00046 * T));
    constexpr double k = 20.49552;
    double SunLongitude = rangezero2tau(EcLonEarth(JD) + (tau * 0.5));

    CAA2DCoordinate aberration;
    aberration.X = (-k * cos(SunLongitude - Lambda) + e * k * cos(pi - Lambda)) / cos(Beta) / 3600;
    aberration.Y = -k * sin(Beta) * (sin(SunLongitude - Lambda) - e * sin(pi - Lambda)) / 3600;
    return aberration;
}
CAA2DCoordinate Astronomy::EquatorialAberration(double ra, double dec, double JD, bool bHighPrecision) {
    const double cosAlpha = cos(ra);
    const double sinAlpha = sin(ra);
    const double cosDelta = cos(dec);
    const double sinDelta = sin(dec);
 
    const CAA3DCoordinate velocity = CAAAberration::EarthVelocity(JD, bHighPrecision);
    CAA2DCoordinate aberration;
    aberration.X = (velocity.Y * cosAlpha - velocity.X * sinAlpha) / (17314463350.0 * cosDelta);
    aberration.Y = -(((velocity.X * cosAlpha + velocity.Y * sinAlpha) * sinDelta - velocity.Z * cosDelta) / 17314463350.0);
    return aberration;
    // retval in radians
}
CAA2DCoordinate Astronomy::FK5Correction(double Longitude, double Latitude, double JD) {
    const double T = (JD - JD2000) / 36525;
    double Ldash = Longitude - deg2rad * T * (1.397 + 0.00031 * T);
    CAA2DCoordinate fk5corr;
    const double loncor = -0.09033 + 0.03916 * (std::cos(Ldash) + std::sin(Ldash)) * tan(Latitude);
    fk5corr.X = CAACoordinateTransformation::DMSToDegrees(0, 0, loncor);
    const double latcor = 0.03916 * (std::cos(Ldash) - std::sin(Ldash));
    fk5corr.Y = CAACoordinateTransformation::DMSToDegrees(0, 0, latcor);
    return fk5corr;
}
double Astronomy::NutationInLongitude(double JD) {
    const double T = (JD - JD2000) / 36525;
    const double Tsquared = T * T;
    const double Tcubed = Tsquared * T;
    double D = 297.85036 + (445267.111480 * T) - (0.0019142 * Tsquared) + (Tcubed / 189474);
    D = CAACoordinateTransformation::MapTo0To360Range(D);
    double M = 357.52772 + (35999.050340 * T) - (0.0001603 * Tsquared) - (Tcubed / 300000);
    M = CAACoordinateTransformation::MapTo0To360Range(M);
    double Mprime = 134.96298 + (477198.867398 * T) + (0.0086972 * Tsquared) + (Tcubed / 56250);
    Mprime = CAACoordinateTransformation::MapTo0To360Range(Mprime);
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * Tsquared) + (Tcubed / 327270);
    F = CAACoordinateTransformation::MapTo0To360Range(F);
    double omega = 125.04452 - (1934.136261 * T) + (0.0020708 * Tsquared) + (Tcubed / 450000);
    omega = CAACoordinateTransformation::MapTo0To360Range(omega);
    double value = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        const double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        //const double radargument = CAACoordinateTransformation::DegreesToRadians(argument);
        value += (coeff.sincoeff1 + (coeff.sincoeff2 * T)) * sin(deg2rad * argument) * 0.0001;
    }
    return value;
    // retval is in arc seconds of degrees - could convert to radians or make bool rad flag
}
double Astronomy::TrueObliquityOfEcliptic(double JD) {
    // Despite the method name, this is typically called mean obliquity, and is without lunar nutation (18.6yr) and other shortterm variations
    // This formula is given in https://en.wikipedia.org/wiki/Axial_tilt credited to J.Laskar 1986
    // Original Source:  http://articles.adsabs.harvard.edu/pdf/1986A%26A...157...59L
    // Original Erratum: http://articles.adsabs.harvard.edu/pdf/1986A%26A...164..437L
    // Good to 0.02" over 1000 years, several arc seconds over 10000 years (around J2000.0)
    const double U = (JD - JD2000) / 3652500;  // U is JD in deca millenia, from J2000.0
    const double Usquared = U * U;
    const double Ucubed = Usquared * U;
    const double U4 = Ucubed * U;
    const double U5 = U4 * U;
    const double U6 = U5 * U;
    const double U7 = U6 * U;
    const double U8 = U7 * U;
    const double U9 = U8 * U;
    const double U10 = U9 * U;
    return CAACoordinateTransformation::DMSToDegrees(23, 26, 21.448)
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
        + secs2deg(NutationInObliquity(JD));
}
double Astronomy::NutationInObliquity(double JD) {
    const double T = (JD - JD2000) / 36525;
    const double Tsquared = T * T;
    const double Tcubed = Tsquared * T;
    double D = 297.85036 + (445267.111480 * T) - (0.0019142 * Tsquared) + (Tcubed / 189474);
    D = CAACoordinateTransformation::MapTo0To360Range(D);
    double M = 357.52772 + (35999.050340 * T) - (0.0001603 * Tsquared) - (Tcubed / 300000);
    M = CAACoordinateTransformation::MapTo0To360Range(M);
    double Mprime = 134.96298 + (477198.867398 * T) + (0.0086972 * Tsquared) + (Tcubed / 56250);
    Mprime = CAACoordinateTransformation::MapTo0To360Range(Mprime);
    double F = 93.27191 + (483202.017538 * T) - (0.0036825 * Tsquared) + (Tcubed / 327270);
    F = CAACoordinateTransformation::MapTo0To360Range(F);
    double omega = 125.04452 - (1934.136261 * T) + (0.0020708 * Tsquared) + (Tcubed / 450000);
    omega = CAACoordinateTransformation::MapTo0To360Range(omega);
    double value = 0;
    for (const auto& coeff : g_NutationCoefficients) {
        const double argument = (coeff.D * D) + (coeff.M * M) + (coeff.Mprime * Mprime) + (coeff.F * F) + (coeff.omega * omega);
        //const double radargument = CAACoordinateTransformation::DegreesToRadians(argument);
        value += (coeff.coscoeff1 + (coeff.coscoeff2 * T)) * cos(deg2rad * argument) * 0.0001;
    }
    // value is in arc seconds of degree - convert? No because TrueObliquityOfEcliptic() uses degrees - Or use bool rad flag !!!
    return value;
}
double Astronomy::NutationInRightAscension(double ra, double dec, double obliq, double nut_lon, double nut_obl) {
    // Already in radians
    return deg2rad * ((cos(obliq) + (sin(obliq) * sin(ra) * tan(dec))) * nut_lon) - (cos(ra) * tan(dec) * nut_obl) / 3600.0;
    // retval is in radians
}
double Astronomy::NutationInDeclination(double ra, double obliq, double nut_lon, double nut_obl) {
    // Already in radians
    return deg2rad * (sin(obliq) * cos(ra) * nut_lon + sin(ra) * nut_obl) / 3600.0;
    // retval is in radians
}

LLH Astronomy::getTrueDecRAbyName(const std::string starname, bool rad) {
    // Apply Proper Motion
    LLH decra = getDecRAwithPMbyName(starname, m_jd, true); // Always calculate from radians, convert at end to what bool rad indicates
    std::cout << "Astronomy::getTrueDecRAbyName(): Catalogue RA(hrs), Dec(deg): " << rad2hrs * decra.lon << ", " << rad2deg * decra.lat << '\n';

    // Apply Precession
    double JD0 = JD2000; // Epoch 2000.0 - The epoch of the catalogue
    const double T = (JD0 - JD2000) / 36525;
    const double Tsquared = T * T;
    const double t = (m_jd - JD0) / 36525;
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
    // Precessed decra in radians
    decra.lon = atan2(A, B) + zeta;
    decra.lat = asin(C); // If star is close to pole, this should ideally use acos(sqrt(A*A+B*B);
    std::cout << "Astronomy::getTrueDecRAbyName(): Precessed RA(hrs), Dec(deg): " << rad2hrs * decra.lon << ", " << rad2deg * decra.lat << '\n';
    // Apply nutation
    double obliq = getEclipticObliquity(m_jd, true);
    double nut_lon = deg2rad * NutationInLongitude(m_jd) / 3600.0; // Add bool rad flag !!!
    double nut_obl = deg2rad * NutationInObliquity(m_jd) / 3600.0;
    double nut_ra = NutationInRightAscension(decra.lon, decra.lat, obliq, nut_lon, nut_obl);
    double nut_dec =NutationInDeclination(decra.lon, obliq, nut_lon, nut_obl);
    decra.lon += nut_ra;
    decra.lat += nut_dec;
    std::cout << "Astronomy::getTrueDecRAbyName(): Nutated RA(hrs), Dec(deg): " << rad2hrs * decra.lon << ", " << rad2deg * decra.lat << '\n';
    // Apply aberration
    CAA2DCoordinate aberration = EquatorialAberration(decra.lon, decra.lat, m_jd, true);
    decra.lon += aberration.X;
    decra.lat += aberration.Y;
    std::cout << "Astronomy::getTrueDecRAbyName(): Aberrated RA(hrs), Dec(deg): " << rad2hrs * decra.lon << ", " << rad2deg * decra.lat << '\n';
    // Ignore annual parallax
    if (!rad) {
        decra.lat *= rad2deg;
        decra.lon *= rad2deg;
    }
    return decra;
}
LLH Astronomy::calcTrueDecRa(const LLH decra, const double jd, const double JD0) {
    // JD0 is Epoch, defaults to J2000 (via JD2000 macro)
    // jd is desired Julian Day, default to current JD in relevant Astronomy object
    double JD = jd == NO_DOUBLE ? getJD() : jd;
    const double T = (JD0 - JD2000) / 36525;
    const double Tsquared = T * T;
    const double t = (JD - JD0) / 36525;
    const double tsquared = t * t;
    const double tcubed = tsquared * t;

    const double sigma = CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, (2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (0.30188 - 0.000344 * T) * tsquared + 0.017998 * tcubed));
    const double zeta = CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, (2306.2181 + 1.39656 * T - 0.000139 * Tsquared) * t + (1.09468 + 0.000066 * T) * tsquared + 0.018203 * tcubed));
    const double phi = CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, (2004.3109 - 0.8533 * T - 0.000217 * Tsquared) * t - (0.42665 + 0.000217 * T) * tsquared - 0.041833 * tcubed));
    const double A = cos(decra.lat) * sin(decra.lon + sigma);
    const double B = cos(phi) * cos(decra.lat) * cos(decra.lon + sigma) - sin(phi) * sin(decra.lat);
    const double C = sin(phi) * cos(decra.lat) * cos(decra.lon + sigma) + cos(phi) * sin(decra.lat);

    LLH value;
    value.lon = atan2(A, B) + zeta;
    //std::cout << "JD: " << JD << "Ra Dec : " << decra.lon << ", " << decra.lat << "->" << value.lon << ", " << value.lat << '\n';
    value.lat = asin(C);

    return value;
}


double Astronomy::getEcLat(size_t planet, double jd) {
    // Heliocentric Ecliptic Latitude (ref. Equinox of Epoch) in radians
    // Unconditionally caching may or may not be desired. Consider a flag, or rename the function so it is clear that getEcLat{planet}() may be preferable. !!!
    if (planet == MERCURY) planet_ecLat[planet] = EcLatMercury(jd);
    else if (planet == VENUS) planet_ecLat[planet] = EcLatVenus(jd);
    else if (planet == EARTH) planet_ecLat[planet] = EcLatEarth(jd);
    else if (planet == MARS) planet_ecLat[planet] = EcLatMars(jd);
    else if (planet == JUPITER) planet_ecLat[planet] = EcLatJupiter(jd);
    else if (planet == SATURN) planet_ecLat[planet] = EcLatSaturn(jd);
    else if (planet == URANUS) planet_ecLat[planet] = EcLatUranus(jd);
    else if (planet == NEPTUNE) planet_ecLat[planet] = EcLatNeptune(jd);
    else std::cout << "APlanet::getEcLat(): planet unknown: " << planet << "\n";
    return planet_ecLat[planet];
}
double Astronomy::getEcLon(size_t planet, double jd) {
    // Heliocentric Ecliptic Longitude (ref. Equinox of Epoch) in radians
    if (planet == MERCURY) planet_ecLon[planet] = EcLonMercury(jd);
    else if (planet == VENUS) planet_ecLon[planet] = EcLonVenus(jd);
    else if (planet == EARTH) planet_ecLon[planet] = EcLonEarth(jd);
    else if (planet == MARS) planet_ecLon[planet] = EcLonMars(jd);
    else if (planet == JUPITER) planet_ecLon[planet] = EcLonJupiter(jd);
    else if (planet == SATURN) planet_ecLon[planet] = EcLonSaturn(jd);
    else if (planet == URANUS) planet_ecLon[planet] = EcLonUranus(jd);
    else if (planet == NEPTUNE) planet_ecLon[planet] = EcLonNeptune(jd);
    else std::cout << "APlanet::getEcLon(): planet unknown: " << planet << "\n";
    return planet_ecLon[planet];
}
double Astronomy::getRadius(size_t planet, double jd, bool km) {
    // Heliocentric Ecliptic Radius (ref. Equinox of Epoch) in kilometers
    if (planet == MERCURY) planet_ecRadius[planet] = EcDstMercury(jd);
    else if (planet == VENUS) planet_ecRadius[planet] = EcDstVenus(jd);
    else if (planet == EARTH) planet_ecRadius[planet] = EcDstEarth(jd);
    else if (planet == MARS) planet_ecRadius[planet] = EcDstMars(jd);
    else if (planet == JUPITER) planet_ecRadius[planet] = EcDstJupiter(jd);
    else if (planet == SATURN) planet_ecRadius[planet] = EcDstSaturn(jd);
    else if (planet == URANUS) planet_ecRadius[planet] = EcDstUranus(jd);
    else if (planet == NEPTUNE) planet_ecRadius[planet] = EcDstNeptune(jd);
    else std::cout << "APlanet::getRadius(): planet unknown: " << planet << "\n";
    if (km) planet_ecRadius[planet] = au2km * planet_ecRadius[planet];
    return planet_ecRadius[planet];
}
// Below is imported from CAA[planet] | planet = {Mercury, Venus, Mars, Jupiter, Saturn, Uranus, Neptune}
// Implements EcLat[planet](), EcLon[planet](), EcDst[planet](), e.g. EcLonMars() etc
// This avoids a few function call context switches, as well as incessant conversions between degrees and radians

//  Heliocentric Ecliptic Calculations for Latitude, Longitude, Distance in radians and AU
double Astronomy::EcLonMercury(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLatMercury(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
    return rangepi2pi(value);
}
double Astronomy::EcDstMercury(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLonVenus(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLatVenus(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
    return rangepi2pi(value);
}
double Astronomy::EcDstVenus(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLonEarth(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLatEarth(double jd) {
    const double rho = (jd - 2451545) / 365250;
    double B0 = 0;
    for (const auto& B0Coefficient : g_B0EarthCoefficients)
        B0 += (B0Coefficient.A * cos(B0Coefficient.B + (B0Coefficient.C * rho)));
    double B1 = 0;
    for (const auto& B1Coefficient : g_B1EarthCoefficients)
        B1 += (B1Coefficient.A * cos(B1Coefficient.B + (B1Coefficient.C * rho)));
    double value = (B0 + B1 * rho) / 100000000;
    return rangepi2pi(value);
}
double Astronomy::EcDstEarth(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLonMars(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLatMars(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
    return rangepi2pi(value);
}
double Astronomy::EcDstMars(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
// Jupiter
double Astronomy::EcLonJupiter(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLatJupiter(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
    return rangepi2pi(value);
}
double Astronomy::EcDstJupiter(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLonSaturn(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLatSaturn(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
    return rangepi2pi(value);
}
double Astronomy::EcDstSaturn(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLonUranus(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLatUranus(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
    return rangepi2pi(value);
}
double Astronomy::EcDstUranus(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLonNeptune(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
double Astronomy::EcLatNeptune(double jd) {
    const double rho = (jd - 2451545) / 365250;
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
    return rangepi2pi(value);
}
double Astronomy::EcDstNeptune(double jd) {
    const double rho = (jd - 2451545) / 365250;
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

//  Privates
void Astronomy::updateGsid() {
    m_gsidtime = calculateGsid(m_jd);
}
void Astronomy::update() {
    updateGsid();
    updateTimeString();

    // Update cached planetary positions to current time. Later, include Sun, Moon & Earth?
    for (unsigned int p = SUN; p <= NEPTUNE; p++) {
        if (planet_refcnt[p] > 0) updateGeocentric(p);
    }
    // Update cached planetary paths
    updateCelestialPaths();
}

