
#include <set>

#include "Utilities.h"

#include "AAplus/AADate.h"  // Merge DateTime with EDateTime in Astronomy



// -----------
//  Date Time
// -----------
void DateTime::add(DateTime& other) {
    // Don't supply weird negative parameters, we are not prepared for that!
    second += other.second;
    while (second > 60.0) { second -= 60.0; minute += 1.0; } // Take into account leap seconds? No...
    while (second < 0.0) { second += 60.0; minute -= 1.0; }
    minute += other.minute;
    while (minute > 60.0) { minute -= 60.0; hour += 1.0; }
    while (minute < 0.0) { minute += 60.0; hour -= 1.0; }
    hour += other.hour;
    while (hour > 24.0) { hour -= 24.0; day += 1.0; }
    while (hour < 0.0) { hour += 24.0; day -= 1.0; }
    day += other.day;
    while (day > (CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal)) {
        day -= CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal;
        month += 1;
    }
    while (day < 0.0) {
        day += CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal;
        month -= 1;
    }
    month += other.month;
    while (month > 12) { month -= 12; year += 1; }
    while (month < 0) { month += 12; year -= 1; }
    // Not the most efficient way, but it should get the job done.
    year += other.year;
}
void DateTime::add(long y, long mo, double d, double h, double mi, double s) {
    // Don't supply weird negative parameters, we are not prepared for that!
    second += s;
    while (second > 60.0) { second -= 60.0; minute += 1.0; }
    while (second < 0.0) { second += 60.0; minute -= 1.0; }
    minute += mi;
    while (minute > 60.0) { minute -= 60.0; hour += 1.0; }
    while (minute < 0.0) { minute += 60.0; hour -= 1.0; }
    hour += h;
    while (hour > 24.0) { hour -= 24.0; day += 1.0; }
    while (hour < 0.0) { hour += 24.0; day -= 1.0; }
    day += d;
    while (day > (CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal)) {
        day -= CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal;
        month += 1;
    }
    while (day < 0.0) {
        day += CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal;
        month -= 1;
    }
    month += mo;
    while (month > 12) { month -= 12; year += 1; }
    while (month < 0) { month += 12; year -= 1; }
    year += y;
    // Not the most efficient way, but it should get the job done.
}
void DateTime::print() {
    std::cout << year << "-" << month << "-" << day << " " << hour << ":" << minute << ":" << second << suffix;
}
DateTime DateTime::operator+(DateTime& other) {
    // Don't supply weird negative parameters, we are not prepared for that!
    double rsec = second + other.second;
    double rmin = minute + other.minute;
    double rhour = hour + other.hour;
    double rday = day + other.day;

    // Normalize into valid date
    while (rsec > 60.0) { rsec -= 60.0; rmin += 1.0; }
    while (rsec < 0.0) { rsec += 60.0; rmin -= 1.0; }
    while (rmin > 60.0) { rmin -= 60.0; rhour += 1.0; }
    while (rmin < 0.0) { rmin += 60.0; rhour -= 1.0; }
    while (rhour > 24.0) { rhour -= 24.0; rday += 1.0; }
    while (rhour < 0.0) { rhour += 24.0; rday -= 1.0; }
    // Accumulate as if only time and day were added, in order to advance leap years correctly
    while (rday > (CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal)) {
        rday -= CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal;
        month += 1;
    }
    while (rday < 0.0) {
        rday += CAADate::IsLeap(year, true) ? tzFile::months[(size_t)month - 1].days_leap : tzFile::months[(size_t)month - 1].days_normal;
        month -= 1;
    }
    // Now add the months, and roll year forward if needed
    long rmonth = month + other.month;
    while (rmonth > 12) { rmonth -= 12; year += 1; }
    while (rmonth < 0) { rmonth += 12; year -= 1; }
    // Finally increase years
    long ryear = year + other.year;
    // Not the most efficient way, but it should get the job done.
    return { ryear, rmonth, rday, rhour, rmin, rsec,'\0' }; // adding suffixes does not make sense, so blank it
// A minus operator is less obvious to implement. The add functions currently do not anticipate negative parameters.
}
bool DateTime::operator<(DateTime& other) {
    if (year < other.year) return true;
    if (year > other.year) return false;
    // Here the years must be equal so check next smaller unit
    if (month < other.month) return true;
    if (month > other.month) return false;
    if (day < other.day) return true;
    if (day > other.day) return false;
    if (hour < other.hour) return true;
    if (hour > other.hour) return false;
    if (minute < other.minute) return true;
    if (minute > other.minute) return false;
    if (second < other.second) return true;
    if (second > other.second) return false;
    // They are the same
    return false;
}
bool DateTime::operator>(DateTime& other) {
    if (year > other.year) return true;
    if (year < other.year) return false;
    // Here the years must be equal so check next smaller unit
    if (month > other.month) return true;
    if (month < other.month) return false;
    if (day > other.day) return true;
    if (day < other.day) return false;
    if (hour > other.hour) return true;
    if (hour < other.hour) return false;
    if (minute > other.minute) return true;
    if (minute < other.minute) return false;
    if (second > other.second) return true;
    if (second < other.second) return false;
    // They are the same
    return false;
}
bool DateTime::operator<=(DateTime& other) {
    if (year < other.year) return true;
    if (year > other.year) return false;
    // Here the years must be equal so check next smaller unit
    if (month < other.month) return true;
    if (month > other.month) return false;
    if (day < other.day) return true;
    if (day > other.day) return false;
    if (hour < other.hour) return true;
    if (hour > other.hour) return false;
    if (minute < other.minute) return true;
    if (minute > other.minute) return false;
    if (second < other.second) return true;
    if (second > other.second) return false;
    // They are the same
    return true;
}
bool DateTime::operator>=(DateTime& other) {
    if (year > other.year) return true;
    if (year < other.year) return false;
    // Here the years must be equal so check next smaller unit
    if (month > other.month) return true;
    if (month < other.month) return false;
    if (day > other.day) return true;
    if (day < other.day) return false;
    if (hour > other.hour) return true;
    if (hour < other.hour) return false;
    if (minute > other.minute) return true;
    if (minute < other.minute) return false;
    if (second > other.second) return true;
    if (second < other.second) return false;
    // They are the same
    return true;
}
bool DateTime::operator==(DateTime& other) {
    if (year != other.year) return false;
    // Here the years must be equal so check next smaller unit
    if (month != other.month) return false;
    if (day != other.day) return false;
    if (hour != other.hour) return false;
    if (minute != other.minute) return false;
    if (second != other.second) return false;
    // They are the same
    return true;
}
bool DateTime::operator!=(DateTime& other) {
    if (year != other.year) return true;
    // Here the years must be equal so check next smaller unit
    if (month != other.month) return true;
    if (day != other.day) return true;
    if (hour != other.hour) return true;
    if (minute != other.minute) return true;
    if (second != other.second) return true;
    // They are the same
    return false;
}
// stream output DateTime object, alas it must be a friend function outside DateTime according to the internet: https://stackoverflow.com/questions/49581430/how-to-overload-ofstream-operator-in-c
std::ostream& operator<<(std::ostream& os, const DateTime& dt) {
    os << dt.year << '-' << dt.month << '-' << dt.day << " " << dt.hour << ":" << dt.minute << ":" << dt.second << dt.suffix;
    return os;
}


// ---------
//  tz File
// ---------
void tzFile::loadFiles(tzFile::iana_tz& timezonedb, const std::string filepath) {
    // Should probably scan provided directory for files, instead of hardcoding them here !!!
    // Also check the backward, backzone, zone and zone1970 files, find the combo with most historical data
    loadFile(timezonedb, filepath + "africa");
    loadFile(timezonedb, filepath + "antarctica");
    loadFile(timezonedb, filepath + "asia");
    loadFile(timezonedb, filepath + "australasia");
    loadFile(timezonedb, filepath + "etcetera");
    loadFile(timezonedb, filepath + "europe");
    loadFile(timezonedb, filepath + "northamerica");
    loadFile(timezonedb, filepath + "southamerica");
    std::cout << "tzFile::loadFiles() Totals = Links: " << timezonedb.tzLinks.size()
        << " Zones: " << timezonedb.tzZones.size()
        << " Rule Lines: " << timezonedb.tzRules.size() << '\n';
}
void tzFile::loadFile(tzFile::iana_tz& timezonedb, const std::string filename) {
    //std::cout << "tzFile::loadFile() - Processing file: " << filename << '\n';
    std::istringstream parse;
    std::string line;
    std::ifstream stream(filename);
    //getline(stream, line); // Skip headers
    unsigned int i = 0;
    std::string token;
    //std::string token2;
    bool zonecont = false;
    std::string r_from;
    std::string r_to;
    std::string r_in;
    std::string r_at;
    std::string r_save;
    std::string z_until;
    while (getline(stream, line)) { // Skip whitespace when reading line
        i++;
        //if (i == 1077) {
        //    std::cout << "BREAK!\n";
        //}
        std::size_t pos = line.find_first_of("#", 0);
        if (pos != line.npos) {
            line = line.erase(pos);
            //std::cout << i << ": - Truncated: " << line << '\n';
        }
        if (line == "") continue;
        token.clear();
        parse.str(line);
        parse.clear();
        parse >> token;
        if (token.empty()) continue; // line was empty after stripping comment
        //std::cout << i << ": -- Token: " << token2 << '\n';
        
        parse.str(line);
        parse.clear();  // Clears flags, not content.
        if (zonecont) { // Zone continuation mode
            std::cout << line << '\n';
            timezonedb.tzZones.back()->zoneconts.push_back(new tzZoneCont());
            parse >> timezonedb.tzZones.back()->zoneconts.back()->stdoff;
            parse >> timezonedb.tzZones.back()->zoneconts.back()->rules;
            parse >> timezonedb.tzZones.back()->zoneconts.back()->format;
            z_until.clear();
            parse >> z_until;
            if (z_until.empty()) {
                zonecont = false;
                std::cout << '\n';
                continue;
            }
            if (!z_until.empty()) timezonedb.tzZones.back()->zoneconts.back()->until.year = std::stol(z_until);
            z_until.clear();
            parse >> z_until;
            if (z_until != "") timezonedb.tzZones.back()->zoneconts.back()->until.month = parseMonth(z_until);
            else timezonedb.tzZones.back()->zoneconts.back()->until.month = 1;
            parse >> timezonedb.tzZones.back()->zoneconts.back()->until_day;
            z_until.clear();
            parse >> z_until;
            if (!z_until.empty()) parseTime(z_until,
                timezonedb.tzZones.back()->zoneconts.back()->until.hour, 
                timezonedb.tzZones.back()->zoneconts.back()->until.minute,
                timezonedb.tzZones.back()->zoneconts.back()->until.second,
                timezonedb.tzZones.back()->zoneconts.back()->until.suffix);
            else {
                timezonedb.tzZones.back()->zoneconts.back()->until.hour = 0.0;
                timezonedb.tzZones.back()->zoneconts.back()->until.minute = 0.0;
                timezonedb.tzZones.back()->zoneconts.back()->until.second = 0.0;
                timezonedb.tzZones.back()->zoneconts.back()->until.suffix = 'w';
            }
        }
        parse >> token;
        if (token.size() == 0) continue;
        if (token == "Link") { // Links are always 1 line records
            timezonedb.tzLinks.push_back(new tzLink());
            parse >> timezonedb.tzLinks.back()->target;
            parse >> timezonedb.tzLinks.back()->source;
            token.clear();
            continue;
        }
        if (token == "Rule") { // Rules are always 1 line records
            timezonedb.tzRules.push_back(new tzRule());
            parse >> timezonedb.tzRules.back()->name;    // Fine as string
            r_from.clear();
            parse >> r_from;    // Is just the year (or min) - store as long
            if (r_from.substr(0, 2) == "mi") timezonedb.tzRules.back()->from = LONG_MIN;
            else timezonedb.tzRules.back()->from = std::stol(r_from);
            r_to.clear();
            parse >> r_to;      // Is just the year (or only, max) - store as long
            if (r_to.substr(0, 2) == "ma") timezonedb.tzRules.back()->to = LONG_MAX;
            else if (r_to[0] == 'o') timezonedb.tzRules.back()->to = timezonedb.tzRules.back()->from;
            else timezonedb.tzRules.back()->to = std::stol(r_to);
            token.clear();
            parse >> token;     // Skip reserved field
            r_in.clear();
            parse >> r_in;
            timezonedb.tzRules.back()->at.month = parseMonth(r_in);
            parse >> timezonedb.tzRules.back()->on; // day - calculated every lookup, could pre-parse but it adds a lot of clutter
            r_at.clear();
            parse >> r_at;      // time and suffix, store as partial DateTime
            parseTime(r_at, timezonedb.tzRules.back()->at.hour, timezonedb.tzRules.back()->at.minute,
                timezonedb.tzRules.back()->at.second, timezonedb.tzRules.back()->at.suffix);
            r_save.clear();
            parse >> r_save;
            parseTime(r_save, timezonedb.tzRules.back()->save.hour, timezonedb.tzRules.back()->save.minute,
                timezonedb.tzRules.back()->save.second, timezonedb.tzRules.back()->save.suffix);
            parse >> timezonedb.tzRules.back()->letters; // Fine as string
            token.clear();
            continue;
        }
        if (token == "Zone") {
            timezonedb.tzZones.push_back(new tzZone());
            std::cout << line << '\n';
            parse >> timezonedb.tzZones.back()->name;
            timezonedb.tzZones.back()->zoneconts.push_back(new tzZoneCont());

            parse >> timezonedb.tzZones.back()->zoneconts.back()->stdoff;     // Could be partial DateTime
            parse >> timezonedb.tzZones.back()->zoneconts.back()->rules;      // Fine as string
            parse >> timezonedb.tzZones.back()->zoneconts.back()->format;     // Fine as string
            z_until.clear();
            parse >> z_until;
            if (!z_until.empty()) {
                zonecont = true;
            }
            if (z_until != "") timezonedb.tzZones.back()->zoneconts.back()->until.year = std::stol(z_until);
            z_until.clear();
            parse >> z_until;
            if (z_until != "") timezonedb.tzZones.back()->zoneconts.back()->until.month = parseMonth(z_until);
            else timezonedb.tzZones.back()->zoneconts.back()->until.month = 1;
            parse >> timezonedb.tzZones.back()->zoneconts.back()->until_day;
            z_until.clear();
            parse >> z_until;
            if (!z_until.empty()) parseTime(z_until,
                timezonedb.tzZones.back()->zoneconts.back()->until.hour,
                timezonedb.tzZones.back()->zoneconts.back()->until.minute,
                timezonedb.tzZones.back()->zoneconts.back()->until.second,
                timezonedb.tzZones.back()->zoneconts.back()->until.suffix);
            else {
                timezonedb.tzZones.back()->zoneconts.back()->until.hour = 0.0;
                timezonedb.tzZones.back()->zoneconts.back()->until.minute = 0.0;
                timezonedb.tzZones.back()->zoneconts.back()->until.second = 0.0;
                timezonedb.tzZones.back()->zoneconts.back()->until.suffix = 'w';
            }
            token.clear();
            continue;
        }
        //std::cout << "tzFile::loadFile() - Martian on line " << i << ": " << line << '\n';
        //std::cout << " (Martian is a file line that causes the code to take a path it should not be able to reach)"
    }
    std::cout << "tzFile::loadFile() - Processed " << i << " lines in file: " << filename << '\n';
    //std::cout << "Loaded Links:\n";
    //for (auto& link : timezonedb.tzLinks) {
    //    std::cout << "- source: " << link->source << ", target: " << link->target << '\n';
    //}
    //std::cout << "Loaded Rules:\n";
    //for (auto& rule : timezonedb.tzRules) {
    //    std::cout << "- name: " << rule->name << ", from: " << rule->from
    //        << ", to: " << rule->to << ", in: " << rule->in
    //        << ", on: " << rule->on << ", at: " << rule->at
    //        << ", save: " << rule->save << ", letters: " << rule->letters
    //        << '\n';
    //}
    //std::cout << "Loaded Zones:\n";
    //for (auto& zone : timezonedb.tzZones) {
    //    std::cout << "- name: " << zone->name << ", stdoff: " << zone->stdoff
    //        << ", rules: " << zone->rules << ", format: " << zone->format
    //        << ", until: " << zone->until_year << zone->until_month << zone->until_day << zone->until_time << '\n';
    //    for (auto& czone : zone->zoneconts) {
    //        std::cout << "-- stdoff: " << czone->stdoff << ", rules: " << czone->rules << ", format: " << czone->format
    //            << ", until: " << czone->until_year << czone->until_month << czone->until_day << czone->until_time << '\n';
    //    }
    //}
}
long tzFile::parseYear(const std::string& year) {
    // Explicit empty year is at end of zone continuation, Rule lines use 'maximum', abbreviated to 'max' in current files.
    if (year.empty() || year == "") return LONG_MAX;
    if (year.substr(0, 1) == "o") {
        std::cout << "tzFile::parseYear(" << year << "): Cannot process \'only\', as FROM year of Rule is unknown. Returning LONG_MIN.\n";
        return LONG_MIN;
    }
    if (year.substr(0, 2) == "mi") return LONG_MIN;
    if (year.substr(0, 2) == "ma") return LONG_MAX;
    return std::stol(year);
}
long tzFile::parseMonth(const std::string& month) {
    if (month.empty()  || month == "") return 1;
    for (auto& m : tzFile::months) {
        if (month.substr(0, m.match_name.size()) == m.match_name) return m.number;
    }
    std::cout << "tzFile::parseMonth(" << month << "): Unidentified month string received. Passing back default value of 1\n";
    return 1;
}
double tzFile::parseDay(const std::string& day, long& month, long& year) {
    // 1 - 31 (y), lastX, X>=y (first X on or after y), X<=y (last X on or before y) ; X is weekday, y is daynumber (in the month)
    // month and year are passed by reference, as caller will need them updated
    //std::cout << "tzFile::parseDay(\"" << day << "\", " << month << ", " << year << ") called.\n";

    if (day.empty() || day == "") return 1; // Default is lowest valid value

    if (day[0] > 47 && day[0] < 58) { // Starts with a digit, so much be a number
        return std::stod(day); // No month/year shift possible
    }

    int retval = 0;
    std::string wday;
    unsigned int wdaynum = 0; // Desired week day number
    if (day.substr(0, 4) == "last") { // Last wday in month
        wday = day.substr(4, std::string::npos);
        for (auto& wd : weekdays) {
            if (wday.substr(0, wd.match_name.size()) == wd.match_name) wdaynum = wd.number;
        }
        unsigned int fwday = (unsigned int)CAADate(year, month, 1.0, true).DayOfWeek(); // First of month weekday
        retval = 22 + (wdaynum - fwday); // Can never exceed 28, but may fall too low, so week forward afterwards, until less than 7 from end.
        if (CAADate::IsLeap(year, true)) while (retval <= months[(size_t)month - 1].days_leap - 7) retval += 7;
        else while (retval <= months[(size_t)month - 1].days_normal - 7) retval += 7;
        //std::cout << " -> last " << weekdays[wdaynum].full_name << " in " << months[(size_t)month - 1].full_name << " is the " << retval << '\n';
        return retval; // No month/year shift possible
    }

    // Parse Day>=date and Day<=date
    bool forward = false;
    // Find equal sign to split string into components
    for (unsigned int i = 0; i < day.size(); i++) {
        if (day[i] == '=') {
            wday = day.substr(0, (size_t)i - 1);
            retval = std::stoi(day.substr((size_t)i + 1, std::string::npos));
            forward = (day[(size_t)i - 1] == '>');
            break;
        }
    }
    // Find weekday number of sought after day
    for (auto& wd : weekdays) {
        if (wday.substr(0, wd.match_name.size()) == wd.match_name) wdaynum = wd.number;
    }
    // Input has been parsed: forward is true with >=, false with <=; wdaynum is weekday number of desired day; retval is limit daynumber
    std::cout << " - " << weekdays[wdaynum].full_name << (forward ? " greater" : " less") << " than or equal to " << retval << '\n';
    if (!forward) { // backwards is the same as forwards from 6 days earlier
        retval -= 6;
        if (retval < 1) { // Went to previous month
            month--;
            if (month == 0) { // Went to previous year
                month = 12;
                year--;
            }
            retval += (CAADate::IsLeap(year, true) ? months[(size_t)month - 1].days_leap : months[(size_t)month - 1].days_normal);
        }
    }
    unsigned int cwday = (unsigned int)CAADate(year, month, retval, true).DayOfWeek();
    if (cwday > wdaynum) retval += 7;
    retval += wdaynum - cwday;
    if (retval <= (CAADate::IsLeap(year, true) ? months[(size_t)month - 1].days_leap : months[(size_t)month - 1].days_normal)) return retval;
    else { // Went to following month
        month++;
        if (month == 13) { month = 1; year++; } // Went to following year
        return (double)retval - (CAADate::IsLeap(year, true) ? months[(size_t)month - 2].days_leap : months[(size_t)month - 2].days_normal);
    }
}
void tzFile::parseTime(const std::string& timestr, double& hours, double& minutes, double& seconds, char& suffix) {
    bool negative = false;
    if (timestr[0] == '-' && timestr.size() == 1) { // Just to make sure a single dash ('-') gets recognized as zero and not a minus.
        hours = 0.0;
        minutes = 0.0;
        seconds = 0.0;
        suffix = 0;
        return;
    }
    std::string hrs;
    std::string mins;
    std::string secs;
    size_t i = 0;
    if (timestr[0] == '-') negative = true;
    for (i = 0; i < timestr.size(); i++) {
        if (timestr[i] == '-' || (timestr[i] > 47 && timestr[i] < 58) || timestr[i] == '.') hrs += timestr[i];
        if (timestr[i] == ':') break;
        suffix = timestr[i];
    }
    for (++i; i < timestr.size(); i++) {
        if (timestr[i] == '-' || (timestr[i] > 47 && timestr[i] < 58 || timestr[i] == '.')) mins += timestr[i];
        if (timestr[i] == ':') break;
        suffix = timestr[i];
    }
    for (++i; i < timestr.size(); i++) {
        if (timestr[i] == '-' || (timestr[i] > 47 && timestr[i] < 58 || timestr[i] == '.')) secs += timestr[i];
        if (timestr[i] == ':') break;
        suffix = timestr[i];
    }
    //std::cout << "Hrs: " << hrs << ", Mins: " << mins << ", Secs: " << secs << ", Suffix: " << suffix << '\n';
    if (!hrs.empty()) hours = std::stod(hrs);
    if (!mins.empty()) minutes = negative ? -std::stod(mins) : std::stod(mins);
    if (!secs.empty()) seconds = negative ? -std::stod(secs) : std::stod(secs);
    if (suffix != 'w' && suffix != 's'&& suffix != 'u'&& suffix != 'z'&& suffix != 'g') suffix = 'w';
}
void tzFile::printLink(tzFile::tzLink& link) {
    std::cout << "Link = Source: " << link.source << ", Target: " << link.target << '\n';
}
void tzFile::printRule(tzFile::tzRule& rule) {
    std::cout << "Rule " << rule.name << '\t' << rule.from << '\t' << rule.to << '\t' << rule.at.month << '\t' << rule.on << '\t'
        << rule.at.hour << ":" << rule.at.minute << ":" << rule.at.second << rule.at.suffix << '\t'
        << rule.save << '\t' << rule.letters << '\n';
}
void tzFile::printZone(tzFile::tzZone& zone) {
    std::cout << "Zone " << zone.name << '\n';
    for (auto& z : zone.zoneconts) printZoneLine(*z);
}
void tzFile::printZoneLine(tzFile::tzZoneCont& zc) {
    std::cout << '\t' << zc.stdoff << '\t' << zc.rules << '\t' << zc.format << '\t'
        << zc.until.year << "-" << zc.until.month << "-" << zc.until_day << " "
        << zc.until.hour << ":" << zc.until.minute << ":" << zc.until.second << zc.until.suffix << '\n';
    // INCOMPLETE !!!

}
// Initialize here due to: https://stackoverflow.com/questions/14495536/how-to-initialize-const-member-variable-in-a-class
const std::vector<tzFile::monthInfo> tzFile::months = {
       {  1, 31, 31, "January", "Ja" },
       {  2, 28, 29, "February", "F" },
       {  3, 31, 31, "March", "Mar" },
       {  4, 30, 30, "April", "Ap"},
       {  5, 31, 31, "May", "May" },
       {  6, 30, 30, "June", "Jun" },
       {  7, 31, 31, "July", "Jul" },
       {  8, 31, 31, "August", "Au" },
       {  9, 30, 30, "September", "S" },
       { 10, 31, 31, "October", "O" },
       { 11, 30, 30, "November", "N" },
       { 12, 31, 31, "December", "D"}
};
const std::vector<tzFile::weekdayInfo> tzFile::weekdays = {
       {  0, "Sunday", "Su" },
       {  1, "Monday", "M" },
       {  2, "Tuesday", "Tu" },
       {  3, "Wednesday", "W" },
       {  4, "Thursday", "Th"},
       {  5, "Friday", "F" },
       {  6, "Saturday", "Sa" }
};
std::string tzFile::getLocalTime(const tzFile::iana_tz& tzdb, const std::string& timezone, const DateTime& moment) {
    bool debug = false;
    if (debug) std::cout << "tzFile::getLocalTime(tzdb, \"" << timezone << "\", "<< moment << "):\n";

    // Resolve any links
    std::string tz = timezone; // timezone is const, so it is easy to pass in.
    for (auto& l : tzdb.tzLinks) {
        if (l->source == tz) tz = l->target;
        break;
    }
    
    // Find appropriate Zone
    tzFile::tzZone* myzone = nullptr;
    for (auto& z : tzdb.tzZones) { // Alas, the tzdb contains few zone lines with non-numerical days in UNTIL which need to be parsed.
        if (z->name == tz) {
            myzone = z;
            break;
        }
    }
    if (myzone == nullptr) {
        std::cout << "ERROR! tzFile::getLocalTime(): No zone found matching zone name \"" << timezone << "\"\n";
        return "tzFile::getLocalTime(): Bad Zone Name!";
    }
    //else if (debug) std::cout << "Found zone: " << myzone;

    //for (auto& z : tzdb.tzZones) {
    //    for (auto& zc : z->zoneconts) {
    //        std::cout << zc->until << '\n';
    //    }
    //}

    // Determine applicable zone line
    DateTime now_utc = moment;
    DateTime zc_begin, zc_until;
    for (auto& zc : myzone->zoneconts) {
        zc_begin = zc->begin;
        zc_until = zc->until;
        // Interpret day according to current time - IMPORTANT: Should take suffix into account, it does not currently !!!
        // NOTE: Can also check first if years are out of range, and save the parse effort for close matches
        zc_begin.day = parseDay(zc->begin_day, now_utc.year, zc->begin.month); // Note year and month cannot be const, as they might be adjusted
        zc_until.day = parseDay(zc->until_day, now_utc.year, zc->until.month);

    }

    // + Find Zone matching the supplied name, applying Links if necessary.
    // - Find Zone line matching the provided universal time in UT/UTC - ACTUALLY: They are probably in wall-clock time unless they have suffix
    // - Apply the stdoff offset to obtain local standard time
    // - Find applicable Rule (Rules?)
    // - Apply the appropriate Rule, if any, to obtain local wall clock time via Rule.save
    // - Compose string with the Rule.letters to Zone.format
    // - Refactor, such that Rules are in a struct Rule, with the name, and has RuleLine entries in a vector, a la Zone.
    //   This will cut down on search length from 2157 Rule lines to #rulenames + #rulesetlen, which is substantially less.
    // MISSING:
    // Account for the change time suffix in the rule. w = wallclock time, s = (local) standard time, u(or g or z) universal time, none = w.
    // The Zone may list a time value rather than a Rule name, then that is the amount of saving directly.
    return "Oh look! It is many o'clock !!";
}
std::string tzFile::getLocalTime(tzFile::iana_tz& timezonedb, const std::string& timezone, long year, long month, double day, double hour, double minute, double second) {
    // + Find Zone matching the supplied name, applying Links if necessary.
    // + Find Zone line matching the provided universal time in UT/UTC
    // + Apply the stdoff offset to obtain local standard time
    // + Find applicable Rule (Rules?)
    // + Apply the appropriate Rule, if any, to obtain local wall clock time via Rule.save
    // + Compose string with the Rule.letters to Zone.format
    // - Refactor, such that Rules are in a struct Rule, with the name, and has RuleLine entries in a vector, a la Zone.
    //   This will cut down on search length from 2157 Rule lines to #rulenames + #rulesetlen, which is substantially less.
    // MISSING:
    // Account for the change time suffix in the rule. w = wallclock time, s = (local) standard time, u(or g or z) universal time, none = w.
    // The Zone may list a time value rather than a Rule name, then that is the amount of saving directly.

    //bool debug = false;
    //if (timezone == "America/Menominee") debug = true;

    DateTime utime;
    DateTime stime;
    utime.year = year;
    utime.month = month;
    utime.day = day;
    utime.hour = hour;
    utime.minute = minute;
    utime.second = second;
    stime = utime;
    std::cout << utime << '\n';
    DateTime b_zone;
    DateTime e_zone;
    DateTime b_rule;
    DateTime e_rule;
    DateTime rule_at;
    DateTime rule_save;

    std::set<std::string> rulenames; // Use Set so we don't get duplicates
    std::string tzname = timezone;
    tzFile::tzZone* myzone = nullptr;
    std::string tzformat;
    // Resolve tz Links
    for (auto& link : timezonedb.tzLinks) {
        if (link->source == timezone) {
            tzname = link->target;
            printLink(*link);
        }
    }
    // Look up Time Zone by name, and collect associated Rule Names
    for (auto& zone : timezonedb.tzZones) {
        if (zone->name == tzname) {
            myzone = zone;
            for (auto& czone : zone->zoneconts) {
                // Can we decide on the zone line to apply here? 
                rulenames.insert(czone->rules);
            }
            break;
        }
    }
    if (myzone == nullptr) {
        std::cout << "ERROR! TimeZones::getLocalTime(): No zone definition found matching zone name: " << tzname << '\n';
        return "ERROR! TimeZones::getLocalTime()";
    }
    else printZone(*myzone);

    b_zone.year = LONG_MIN;
    std::string abbr;
    // Which Zone Line are we in?
    for (auto& czone : myzone->zoneconts) {
        e_zone.year = czone->until.year;
        e_zone.month = czone->until.month;
        e_zone.day = tzFile::parseDay(czone->until_day, e_zone.month, e_zone.year);
        if (b_zone <= utime && utime <= e_zone) {
            printZoneLine(*czone);
            // We are in this Zone Line - calculate local standard time
            double hr = 0.0, min = 0.0, sec = 0.0;
            char suf = 0;
            tzFile::parseTime(czone->stdoff, hr, min, sec, suf);
            stime.add(0, 0, 0.0, hr, min, sec); // There is no suffix on Zone.STDOFF, but there IS in Zone.UNTIL
            tzformat = czone->format;
        }
        // Parse applicable rules here
        // Loop over all Rules and examine the ones with names collected above
        // - inefficient, if Rules are stored in timezonedb grouped by name, they can be skipped over more quickly.
        DateTime best_at;
        for (auto rule : timezonedb.tzRules) {
            if (rule->name == czone->rules) {
                //std::cout << "-- Rule: " << rule->name << '\t' << rule->from << '\t' << rule->to << '\t'
                //    << rule->in << '\t' << rule->on << '\t' << rule->at << '\t' << rule->save << '\t' << rule->letters << '\n';
                // Find start and end years for Rule
                b_rule.year = rule->from;
                e_rule.year = rule->to;
                e_rule.month = 12;
                e_rule.day = 31;
                if (e_zone.year < b_rule.year) continue; // Zone ends before this rule starts
                if (b_zone.year > e_rule.year) continue; // Zone starts after this rule ends
                // Zone is a year match, so a candidate.
                // BUG: Should check suffix of transition time, if it is standard or wall clock, don't compare to *utime!!!
                if (b_rule < utime && utime < e_rule) { // universal time is within the rule period
                    std::cout << rule << '\n'; // printRule(*rule);
                    
                    //tzFile::parseTime(rule->at, rule_at->hour, rule_at->minute, rule_at->second, rule_at->suffix);
                    // IN/ON/AT Rule is within year, so use current time year, NOT Rule or Zone begin time years.
                    rule_at = { 0, 0, 0.0, rule->at.hour, rule->at.minute, rule->at.second, rule->at.suffix };
                    rule_at.year = year;
                    rule_at.month = rule->at.month; // Rule.AT
                    rule_at.day = tzFile::parseDay(rule->on, rule_at.month, rule_at.year);
                    if (rule_at < stime && rule_at > best_at) { // If rule is better (in past, but closer)
                        best_at = rule_at;
                        rule_save = rule->save;
                        abbr = rule->letters;
                        std::cout << abbr; // "BOING!\n";
                    }
                }
            }
        }
        b_zone = e_zone;
    }
    // Apply found save time string
    //tzFile::parseTime(save, rule_save.hour, rule_save.minute, rule_save.second, rule_save.suffix);
    stime.add(rule_save);
    std::string tzabbr;
    if (abbr == "-") abbr = ""; // Normalize Rule.LETTER/S, it now contains the correct string for %s replacement
    char tza[] = "1234567890";
    for (size_t i = 0; i < tzformat.size(); i++) { // Insert Rule.LETTER/S
        if (tzformat[i] == '%') {
            snprintf(tza, sizeof(tza), tzformat.c_str(), abbr.c_str());
            tzabbr = tza;
            break;
        }
        if (tzformat[i] == '/') { // Take AAA/BBB to AAA if standard time, BBB if daylight saving 
            if (rule_save.hour == 0.0 && rule_save.minute == 0.0 && rule_save.second == 0.0)
                tzabbr = tzformat.substr(0, i).c_str(); // i, not i-1, because parameter is a length, not an index.
            else tzabbr = tzformat.substr(i + 1, std::string::npos);
            break;
        }
        if (tzformat[i] == '-' || tzformat[i] == '+') {
            tzabbr = tzformat;
            break;
        }
    }
    // Build date time string
    char ltime[] = "12345678901234567890123456789012345678901234567890";
    std::string localtime;
    snprintf(ltime, sizeof(ltime), "%04d-%02d-%02d %02d:%02d:%02.0f %s\n",
        (int)stime.year, (int)stime.month, (int)stime.day, (int)stime.hour, (int)stime.minute, stime.second, tzabbr.c_str());
    localtime = ltime;
    return localtime; // "It is many o'clock!";
}
void tzFile::dumpTimeZoneDetails(tzFile::iana_tz& timezonedb, const std::string& timezone) {
    // Resolve tz Links
    std::set<std::string> rulenames; // Use Set so we don't get duplicates
    std::string tzname = timezone;
    tzFile::tzZone* myzone = nullptr;
    for (auto& link : timezonedb.tzLinks) {
        if (link->source == timezone) {
            tzname = link->target;
            break;
        }
    }
    if (tzname != timezone) std::cout << "Time zone " << timezone << " redirects to time zone : " << tzname << '\n';
    for (auto& zone : timezonedb.tzZones) {
        if (zone->name == tzname) {
            std::cout << "\nFull Zone Details for " << tzname << " : \n";
            myzone = zone;
            for (auto& czone : zone->zoneconts) {
                std::cout << czone->stdoff << '\t' << czone->rules << '\t' << czone->format << '\t'
                    << czone->until.year << ' ' << czone->until.month << ' ' << czone->until_day << ' '
                    << czone->until.hour << ":" << czone->until.minute << ":" << czone->until.second << czone->until.suffix << '\n';
                rulenames.insert(czone->rules);
            }
            break;
        }
    }
    if (myzone == nullptr) {
        std::cout << "ERROR! TimeZones::dumpTimeZoneDetails(): No zone definition found matching zone name: " << tzname << '\n';
        return;
    }
    std::cout << "\nFull Ruleset referenced by " << tzname << " : \n";
    for (auto& rulename : rulenames) {
        //std::cout << rulename << '\n';
        for (auto& rule : timezonedb.tzRules) {
            if (rule->name == rulename) std::cout << "Rule\t" << rule->name << '\t' << rule->from << '\t' << rule->to << '\t'
                << rule->at.month << '\t' << rule->on << '\t'
                << rule->at.hour << ':' << rule->at.minute << ':' << rule->at.second << rule->at.suffix
                << '\t' << rule->save << '\t' << rule->letters << '\n';
        }
    }
    std::cout << "\n\n\nSummary of Zone intervals for " << tzname << " : \n";
    long b_year = LONG_MIN;
    long b_month = 0;
    double b_day = 0.0;
    long e_year = myzone->zoneconts[0]->until.year;
    long e_month = myzone->zoneconts[0]->until.month;
    double e_day = tzFile::parseDay(myzone->zoneconts[0]->until_day, b_month, b_year);  // Should those not be e_month and e_year ? !!!
    long b_ryear = 0;
    long b_rmonth = 0;
    double b_rday = 0.0;
    long e_ryear = 0;
    long e_rmonth = 0;
    double e_rday = 0.0;
    std::cout << "- From: " << b_year << "-" << b_month << "-" << b_day << " To: " << e_year << "-" << e_month << "-" << e_day << '\n';
    b_year = e_year;
    b_month = e_month;
    b_day = e_day;
    for (auto& czone : myzone->zoneconts) {
        e_year = czone->until.year;
        e_month = czone->until.month;
        e_day = tzFile::parseDay(czone->until_day, e_month, e_year);
        std::cout << "- From: " << b_year << "-" << b_month << "-" << b_day << " To: " << e_year << "-" << e_month << "-" << e_day << '\n';
        // Parsed zone line
        std::cout << "-- StdOff: " << czone->stdoff << ", Rule: " << czone->rules << ", TZ abbreviation: " << czone->format << " \n";
        // Parse applicable rules here
        for (auto rule : timezonedb.tzRules) {
            if (rule->name == czone->rules) {
                b_ryear = rule->from;
                e_ryear = rule->to;
                // Should build datetime objects and compare them in full, accounting for months and days - but first gain intuition
                // - Wait, zones are matched to years, but should be compared to end month day & time for zone line. Hmm, intuition indeed.
                // - See implementation of getLocalTime() and follow suit
                if (e_year < b_ryear) continue; // Zone ends before this rule starts
                if (b_year > e_ryear) continue; // Zone starts after this rule ends
                // Zone is a year match, so a candidate.
                std::cout << "-- Rule: " << rule->name << '\t' << rule->from << '\t' << rule->to << '\t'
                    << rule->at.month << '\t' << rule->on << '\t'
                    << rule->at.hour << ':' << rule->at.minute << ':' << rule->at.second << rule->at.suffix
                    << '\t' << rule->save << '\t' << rule->letters << '\n';
            }
        }
        b_year = e_year;
        b_month = e_month;
        b_day = e_day;
    }
}


// ------------
//  Shape File
// ------------
void ShapeFile::printRecord(const std::vector<ShapeRecord*>& records, const unsigned int rindex) {
    //rindex--; // IMPORTANT! Record numbers start at 1, the vector starts at 0. Since we are const, we cannot decrement rindex.
    std::cout << "Record number: " << records[(size_t)rindex - 1]->recordnum << '\n';
    std::cout << " Type: " << records[(size_t)rindex - 1]->type << '\n';
    std::cout << " Number of parts: " << records[(size_t)rindex - 1]->numparts << '\n';
    for (auto& part : records[(size_t)rindex - 1]->parts) {
        std::cout << "  Part number: " << part.partnum;
        std::cout << "  Start Index: " << part.startindex;
        std::cout << "  Length: " << part.length;
        std::cout << "  End Index: " << part.startindex + part.length - 1 << '\n'; // so iterate to > parts.length, not >=
    }
    std::cout << " Number of points: " << records[(size_t)rindex - 1]->points.size() << '\n';
    for (unsigned int i = 0; i < records[(size_t)rindex - 1]->points.size(); i++) {
        std::cout << "  Point number " << i << " (" << records[(size_t)rindex-1]->points[i].latitude << "," << records[(size_t)rindex-1]->points[(size_t)i].longitude << ")\n";
    }
}
int ShapeFile::parseFile(std::vector<ShapeRecord*>& records, const std::string& shapefile) {
    bool MY_DEBUG = false; // enable/disable lots of output while parsing the file
    // Generalize a bit, and re-use for TimeZones !!!
    uint32_t bytecnt = 0;
    std::ifstream infile(shapefile, std::ifstream::in | std::ifstream::binary);
    if (!infile.is_open()) {
        std::cout << "ShapeFile::parseFile() - Did not manage to open Shapefile: " << shapefile << "\n";
        return -1;
    }
    uint32_t valint = 0;
    double valdbl = 0.0;
    unsigned int record = 1;
    valint = BinaryFile::readIntBig(infile, bytecnt); // magic number, is always 0x0000270A
    if (MY_DEBUG) std::cout << "Magic number: " << valint << '\n';
    if (valint != 9994) {
        std::cout << "Was expecting Magic number 9994 (0x0000270A), got: " << valint << " !Not sure how to proceed, so bailing.\n";
        infile.close();
        return -1;
    }
    valint = BinaryFile::readIntBig(infile, bytecnt); // 5 Unused ints
    valint = BinaryFile::readIntBig(infile, bytecnt);
    valint = BinaryFile::readIntBig(infile, bytecnt);
    valint = BinaryFile::readIntBig(infile, bytecnt);
    valint = BinaryFile::readIntBig(infile, bytecnt);
    uint32_t filesize = 2 * BinaryFile::readIntBig(infile, bytecnt); // File length in 16 bit words, including header, int32 big endian
    if (MY_DEBUG) std::cout << "File Size: " << valint << '\n';
    unsigned int fileversion = BinaryFile::readIntLittle(infile, bytecnt); // Version, int32 little endian
    if (MY_DEBUG) std::cout << "Version: " << fileversion << '\n';
    valint = BinaryFile::readIntLittle(infile, bytecnt); // Shape Type, int32 little endian
    if (MY_DEBUG) std::cout << "Shape Type: " << valint << '\n';
    if (valint != 5) {
        std::cout << "Was expecting Shape Type 5! Not sure how to proceed, so bailing.\n";
        infile.close();
        return -1;
    }
    double minX = BinaryFile::readDoubleLittle(infile, bytecnt);
    double minY = BinaryFile::readDoubleLittle(infile, bytecnt);
    double maxX = BinaryFile::readDoubleLittle(infile, bytecnt);
    double maxY = BinaryFile::readDoubleLittle(infile, bytecnt);
    double minZ = BinaryFile::readDoubleLittle(infile, bytecnt);
    double maxZ = BinaryFile::readDoubleLittle(infile, bytecnt);
    double minM = BinaryFile::readDoubleLittle(infile, bytecnt);
    double maxM = BinaryFile::readDoubleLittle(infile, bytecnt);
    if (MY_DEBUG) std::cout << "MBB - Minimum Bounding Box:\n";
    if (MY_DEBUG) std::cout << " minX: " << minX << ", minY: " << minY << '\n';
    if (MY_DEBUG) std::cout << " maxX: " << maxX << ", maxY: " << maxY << '\n';
    if (MY_DEBUG) std::cout << "Z range: " << minZ << ", " << maxZ << '\n';
    if (MY_DEBUG) std::cout << "M range: " << minM << ", " << maxM << '\n';
    uint32_t rnum = 0;
    uint32_t temp = 0;
    uint32_t recordlen = 0;
    unsigned int pcnt = 0;
    while (bytecnt < filesize) { // Run until the anticipated end of file
        records.push_back(new ShapeRecord());
        records.back()->recordnum = BinaryFile::readIntBig(infile, bytecnt);
        if (MY_DEBUG) std::cout << " Record Number: " << records.back()->recordnum << '\n';
        recordlen = 2 * BinaryFile::readIntBig(infile, bytecnt);
        if (MY_DEBUG) std::cout << " Record Length: " << recordlen << '\n'; // length is counted in 16 bit words for some reason
        unsigned int recordend = bytecnt + recordlen;

        records.back()->type = BinaryFile::readIntLittle(infile, bytecnt);
        if (MY_DEBUG) std::cout << "Shape Type: " << valint << '\n';
        if (records.back()->type != 5) {
            std::cout << "Was expecting Shape Type 5! Not sure how to proceed, so bailing.\n";
            infile.close();
            return -1;
        }
        double minX = BinaryFile::readDoubleLittle(infile, bytecnt);
        double minY = BinaryFile::readDoubleLittle(infile, bytecnt);
        double maxX = BinaryFile::readDoubleLittle(infile, bytecnt);
        double maxY = BinaryFile::readDoubleLittle(infile, bytecnt);
        if (MY_DEBUG) std::cout << "  MBR - Minimum Bounding Box:\n";
        if (MY_DEBUG) std::cout << "   minX: " << minX << ", minY: " << minY << '\n';
        if (MY_DEBUG) std::cout << "   maxX: " << maxX << ", maxY: " << maxY << '\n';
        records.back()->numparts = BinaryFile::readIntLittle(infile, bytecnt);
        uint32_t numpoints = BinaryFile::readIntLittle(infile, bytecnt); // Needed later when capping last part index
        if (MY_DEBUG) std::cout << "  Number of parts: " << records.back()->numparts << '\n';
        if (MY_DEBUG) std::cout << "  Number of points: " << numpoints << '\n';
        unsigned int index;
        for (unsigned int i = 1; i <= records.back()->numparts; i++) {
            // Get parts array indices
            index = BinaryFile::readIntLittle(infile, bytecnt); // index into point array where this part starts
            if (i > 1) records.back()->parts.back().length = index - records.back()->parts.back().startindex; // finish previous part
            records.back()->parts.push_back(ShapePart()); // start new part
            records.back()->parts.back().startindex = index;
            records.back()->parts.back().partnum = i;
            if (MY_DEBUG) std::cout << "  Part " << i << " point data start index: " << records.back()->parts.back().startindex << '\n';
        }
        records.back()->parts.back().length = numpoints - records.back()->parts.back().startindex;
        pcnt = 0;
        while (bytecnt < recordend) {
            // Stuff points for all parts into array
            records.back()->points.emplace_back(LatLon());
            records.back()->points.back().longitude = BinaryFile::readDoubleLittle(infile, bytecnt);
            records.back()->points.back().latitude = BinaryFile::readDoubleLittle(infile, bytecnt);
            if (MY_DEBUG) std::cout << "    Point " << pcnt << ": (" << records.back()->points.back().latitude << ", " << records.back()->points.back().longitude << ")\n";
            pcnt++;
        }
        // Now save the count of points for this part
        records.back()->parts.back().length = pcnt - records.back()->parts.back().startindex;
        if (MY_DEBUG) std::cout << "Last part length: " << records.back()->parts.back().length << '\n';
        //temp++;
        //if (temp > 4) break; // It goes on and on without this...
        // This successfully parses the whole current file, including multiple parts in records. Now consider how to organize the data in memory!
    }
    return 0;
}


// -------------
//  Binary File
// -------------
double BinaryFile::readDoubleBig(std::istream& infile, uint32_t& bytecnt) {
    uint8_t b[8] = {0,0,0,0,0,0,0,0};
    infile.read((char*)b, 8);
    uint64_t val = (uint64_t)b[7] | ((uint64_t)b[6] << 8) | ((uint64_t)b[5] << 16) | ((uint64_t)b[4] << 24) |
        ((uint64_t)b[3] << 32) | ((uint64_t)b[2] << 40) | ((uint64_t)b[1] << 48) | ((uint64_t)b[0] << 56);

    double dbl = *(double*)&val;
    bytecnt += 8;
    return dbl;
}
double BinaryFile::readDoubleLittle(std::istream& infile, uint32_t& bytecnt) {
    uint8_t b[8] = { 0,0,0,0,0,0,0,0 };
    infile.read((char*)b, 8);
    uint64_t val = (uint64_t)b[0] | ((uint64_t)b[1] << 8) | ((uint64_t)b[2] << 16) | ((uint64_t)b[3] << 24) |
        ((uint64_t)b[4] << 32) | ((uint64_t)b[5] << 40) | ((uint64_t)b[6] << 48) | ((uint64_t)b[7] << 56);

    double dbl = *(double*)&val;
    bytecnt += 8;
    return dbl;
}
uint32_t BinaryFile::readIntBig(std::istream& infile, uint32_t& bytecnt) {
    uint8_t bytes[4] = { 0,0,0,0 };
    infile.read((char*)bytes, 4);
    uint32_t val = bytes[3] | (bytes[2] << 8) | (bytes[1] << 16) | (bytes[0] << 24);
    bytecnt += 4;
    return val;
}
uint32_t BinaryFile::readIntLittle(std::istream& infile, uint32_t& bytecnt) {
    uint8_t bytes[4] = { 0,0,0,0 };
    infile.read((char*)bytes, 4);
    uint32_t val = bytes[0] | (bytes[1] << 8) | (bytes[2] << 16) | (bytes[3] << 24);
    bytecnt += 4;
    return val;
}