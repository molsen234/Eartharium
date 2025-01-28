#pragma once

#include <string>
#include <vector>

//#include "astronomy/acoordinates.h"  // for LLD
//#include "astronomy/datetime.h"

// Protos
struct ShapePart;
struct LatLon;
struct ShapeRecord;
class tzFile;
class ShapeFile;
class BinaryFile;
class SmartPath;


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
        LLD location{ };             // <gx:coord>8.52743 47.48766 610</gx:coord>
    };
    static int parseFile(SmartPath& path, const std::string& kmlfile);
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
