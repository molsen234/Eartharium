#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


//#include "Earth.h"      // So KML File parser knows SmartPath
#include "Astronomy.h"  // For EDateTime

// Protos
struct ShapePart;
struct LatLon;
struct ShapeRecord;
class tzFile;
class ShapeFile;
class BinaryFile;
struct DateTime;
//class EDateTime;
class SmartPath;

std::ostream& operator<<(std::ostream& os, const DateTime& dt);
// -----------
//  Date Time
// -----------
// For Date and Time, recording an instant in time - types chosen for compatibility with AA+ CAADate
struct DateTime {
    long year = 0;
    long month = 0;
    double day = 0.0;
    double hour = 0.0;
    double minute = 0.0;
    double second = 0.0;
    char suffix = 0;
    void add(DateTime& other);
    void add(long y, long mo, double d, double h, double mi, double s);
    void print();
    DateTime operator+(DateTime& other);
    bool operator<(DateTime& other);
    bool operator>(DateTime& other);
    bool operator<=(DateTime& other);
    bool operator>=(DateTime& other);
    bool operator==(DateTime& other);
    bool operator!=(DateTime& other);
    friend std::ostream& operator<<(std::ostream& os, const DateTime& dt);
};


// ---------
//  tz File
// ---------
class tzFile {
public:
    struct tzLink {
        std::string source;
        std::string target;
    };
    struct tzRule {
        std::string name;
        long from = 0;
        long to = 0;
        std::string on; // On needs parsing every run, as it may give different dates in different years
        DateTime at;    // month=IN,hour=AT,minute=AT,second=AT,suffix=AT 
        DateTime save;
        std::string letters;
    };
    struct tzZoneCont {
        DateTime begin;
        std::string begin_day;
        DateTime until;
        std::string until_day; // Can (a few do) contain non-numerical values which need parsing at each use
        std::string stdoff;
        std::string rules;
        std::string format;
    };
    struct tzZone {
        std::string name;
        std::vector<tzZoneCont*> zoneconts;
    };
    struct iana_tz {
        std::vector<tzFile::tzLink*> tzLinks;
        std::vector<tzFile::tzRule*> tzRules;
        std::vector<tzFile::tzZone*> tzZones;
    };
    // Month details - Used in Rule.IN, Zone.UNTIL, and calculations
    struct monthInfo {
        unsigned int number = 0;
        int days_normal = 0;
        int days_leap = 0;
        std::string full_name;
        std::string match_name; // Should be shortest unique
    };
    struct weekdayInfo {
        unsigned int number = 0;
        std::string full_name;
        std::string match_name; // Should be shortest unique
    };
    static const std::vector<monthInfo> months;
    static const std::vector<weekdayInfo> weekdays;
    static void loadFiles(tzFile::iana_tz& tzdb, const std::string filepath);
    static void loadFile(tzFile::iana_tz& tzdb, const std::string filename);
    static long parseYear(const std::string& year);
    static long parseMonth(const std::string& month);
    static double parseDay(const std::string& day, long& month, long& year);
    static void parseTime(const std::string& timestr, double& hours, double& minutes, double& seconds, char& suffix);
    static void printLink(tzFile::tzLink& link);
    static void printRule(tzFile::tzRule& rule);
    static void printZone(tzFile::tzZone& zone);
    static void printZoneLine(tzFile::tzZoneCont& zc);
    static std::string getLocalTime(const tzFile::iana_tz& tzdb, const std::string& tz, const DateTime& moment);
    static std::string getLocalTime(tzFile::iana_tz& timezonedb, const std::string& timezone, long year, long month, double day, double hour, double minute, double second);
    static void dumpTimeZoneDetails(tzFile::iana_tz& timezonedb, const std::string& timezone);
};


// ------------
//  Shape File
// ------------
class ShapeFile {
    // Handles ESRI ShapeFiles https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf
    // Currently only knows how to load POLYGON outlines, as that is all that is needed (can be tested properly)
public:
    struct ShapePart {
        size_t partnum = 0;
        size_t startindex = 0;
        size_t length = 0; // Number of points in this part
    };
    struct LatLon {
        double latitude = 0.0;
        double longitude = 0.0;
    };
    struct ShapeRecord {
        size_t recordnum = 0;
        size_t type = 0;
        size_t numparts = 0;
        std::vector<LatLon> points;
        std::vector<ShapePart> parts;
    };
    static void printRecord(const std::vector<ShapeRecord*>& records, const unsigned int rindex);
    static int parseFile(std::vector<ShapeRecord*>& records, const std::string& shapefile);
};


// ----------
//  KML File
// ----------
class KMLFile {
// Tested with KML files from FlightAware. The parser is VERY simplistic, so it may not parse KML files from other sources
public:
    struct kmlentry {
        EDateTime timestamp;         // <when>2016-08-07T21:14:39Z</when>
        LLD location{ 0.0,0.0,0.0 }; // <gx:coord>8.52743 47.48766 610</gx:coord>
    };
    static int parseFile(SmartPath& path, const std::string& kmlfile) {
        // !!! FIX: Make a TimePath to use instead of SmartPath !!!
        std::ifstream infile(kmlfile, std::ifstream::in); // | std::ifstream::binary)
        if (!infile.is_open()) return -1;
        std::vector<kmlentry> kmlbuffer;
        std::string line;
        long yr, mo;
        double da, hr, mi, se;
        double lat, lon, dst;
        std::string token;
        size_t index{ 0 };
        while (getline(infile, line)) {
            trim(line);                               // trim() is in config.h
            if (line.length() < 9) continue;
            if (line.substr(1, 4) == "when") {
                //std::cout << line << "\n";
                yr = stol(line.substr(8, 4));
                mo = stol(line.substr(11, 2));
                da = stod(line.substr(14, 2));
                hr = stod(line.substr(17, 2));
                mi = stod(line.substr(20, 2));
                se = stod(line.substr(23, 2));
                //std::cout << yr << ";" << mo << ";" << da << ";" << hr << ";" << mi << ";" << se << "\n";
                kmlbuffer.emplace_back(kmlentry{ EDateTime{yr,mo,da,hr,mi,se}, LLD{0.0,0.0,0.0} });
            }
            if (line.substr(1, 8) == "gx:coord") {
                //std::cout << line << "\n";
                line.erase(0, line.find_first_of('>')+1);
                line.erase(line.find_first_of('<'));
                std::stringstream ss(line);
                //std::cout << "Index: " << index << " : " << line << "\n";
                ss >> token;
                lat = stod(token);
                ss >> token;
                lon = stod(token);
                ss >> token;
                dst = stod(token);
                kmlbuffer[index].location = { lat, lon, dst };
                //std::cout << "Index: " << index << " : ";
                //kmlbuffer[index].location.print();
                index++;
            }
        }
        std::cout << "KMLFile::parseFile(): " << index << " points loaded:\n";
        index = 0;
        for (auto& p : kmlbuffer) {
            std::cout << index << " : " << p.timestamp.string() << " = ";
            p.location.print();
            index++;
        }
        return 0;
    }
};


// -------------
//  Binary File
// -------------
class BinaryFile {
    // Basic functions for reading binary data in big or little endian
public:
    static double readDoubleBig(std::istream& infile, uint32_t& bytecnt);
    static double readDoubleLittle(std::istream& infile, uint32_t& bytecnt);
    static uint32_t readIntBig(std::istream& infile, uint32_t& bytecnt);
    static uint32_t readIntLittle(std::istream& infile, uint32_t& bytecnt);
};
