
#include <chrono>


#include "datetime.h"
#include "datetime_tables.h"  // Generated with python script build_datetime_tables.py

#include "config.h"

// -----------
//  EDateTime
// -----------
    // TODO: !!! Integrate with the TimeZones class, to accept setTime() with a timezone and convert automatically !!!
// Constructors
// - Default constructor sets current system time in UTC
EDateTime::EDateTime() {
    setTimeNow();
}
// - From date and time in UTC
EDateTime::EDateTime(long year, long month, double day, double hour, double minute, double second)
    : m_year(year), m_month(month), m_day(day), m_hour(hour), m_minute(minute), m_second(second) {
    normalize();
    calcJDs();
}
// - From Julian Date
EDateTime::EDateTime(double jd, bool is_tt) { // Expects JD in UTC
    // Optional bool tt is false by default, indicating a JD_UTC is expected.
    // If instead a JD_TT is supplied, set the bool to true for correct conversion.
    // This is needed as two different ctors both just taking a double is ambiguous and thus results in a compiler error.
    if (!is_tt) {
        m_JD_UTC = jd;
        m_JD_TT = getJDUTC2TT(jd);
    }
    else {
        m_JD_TT = jd;
        m_JD_UTC = getJDTT2UTC(jd);
    }
    getJD2DateTime(m_JD_UTC, m_year, m_month, m_day, m_hour, m_minute, m_second);
}
// - From a Unix Time Stamp
EDateTime::EDateTime(long unixtime) {
    // Unix time is in UTC
    setJD_UTC(getUnixTime2JD_UTC(unixtime));
}
// Getters
long EDateTime::year() const { return m_year; }
long EDateTime::month() const { return m_month; }
double EDateTime::day() const { return m_day; }
double EDateTime::hour() const { return m_hour; }
double EDateTime::minute() const { return m_minute; }
double EDateTime::second() const { return m_second; }
double EDateTime::jd_tt() /* Astronomical Julian date */ const { return m_JD_TT; }
double EDateTime::jd_utc() /* Astronomical Julian date */ const { return m_JD_UTC; }
bool EDateTime::isLeap() /* returns true if leap year, false otherwise */ const { return isLeapYear(m_year); }
long EDateTime::weekday() /* Sun=0 Mon=1 Tue=2 Wed=3 Thu=4 Fri=5 Sat=6 */ const { return ((int)(m_JD_TT + 0.5) + 1) % 7; }
long EDateTime::dayofyear() const {
    static long months[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    long days = 0;
    for(long m=1; m<m_month; m++) { // Don't count current month
        days += months[m];
    }
    if ((m_month > 2) && isLeap()) days++; // Add leap day if it has actually occurred since the start of the year.
    days += (long)m_day;
    return days;
}
std::string EDateTime::string() const /* Neatly formatted date time string */ {
    //std::cout << dstring << '\n';
    char buff[100];
    snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02.0f UTC",
        m_year, m_month, (long)m_day, (long)m_hour, (long)m_minute, m_second);
    std::string dstring = buff;
    return dstring;
}
std::string EDateTime::stringms() const /* Neatly formatted date time string with milliseconds */ {
    //std::cout << dstring << '\n';
    char buff[100];
    snprintf(buff, sizeof(buff), "%04d-%02d-%02d %02d:%02d:%02.3f UTC",
        m_year, m_month, (long)m_day, (long)m_hour, (long)m_minute, m_second);
    std::string dstring = buff;
    return dstring;
}

long EDateTime::unixTime() const { /* Returns Unix timestamp (in UTC?) of current time */
    return getDateTime2UnixTime(m_year, m_month, m_day, m_hour, m_minute, m_second);
}
// Setters
// - Set with date and time in UTC
void EDateTime::setTime(long year, long month, double day, double hour, double minute, double second) {
    m_year = year;
    m_month = month;
    m_day = day;
    m_hour = hour;
    m_minute = minute;
    m_second = second;
    normalize();
    calcJDs();
}
// - Set to current system time in UTC
void EDateTime::setTimeNow() {
    time_t tt = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    tm utc_tm;   // tm has a maximum resolution of 1 second, but includes leap seconds.
    gmtime_s(&utc_tm, &tt);
    setTime((long)utc_tm.tm_year + 1900, (long)utc_tm.tm_mon + 1, (double)utc_tm.tm_mday,
        (double)utc_tm.tm_hour, (double)utc_tm.tm_min, (double)utc_tm.tm_sec);
}
// - Set to specific astronomical Julian Date given in UTC
void EDateTime::setJD_UTC(double jd_utc) {
    m_JD_UTC = jd_utc;
    m_JD_TT = getJDUTC2TT(m_JD_UTC);
    getJD2DateTime(m_JD_UTC, m_year, m_month, m_day, m_hour, m_minute, m_second);
}
// - Set to specific astronomical Julian Date given in TT (Dynamical Time)
void EDateTime::setJD_TT(double jd_tt) {
    m_JD_TT = jd_tt;
    m_JD_UTC = getJDTT2UTC(m_JD_TT);
    //std::cout << "EDateTime::setJD_TT(): m_JD_TT=" << m_JD_TT << ", m_JD_UTC=";
    getJD2DateTime(m_JD_UTC, m_year, m_month, m_day, m_hour, m_minute, m_second);
    //std::cout << m_JD_UTC << "\n";
}
// - Set from Unix timestamp (seconds since 1970-01-01 00:00:00)
void EDateTime::setUnixTime(long unixtime) { setJD_UTC(getUnixTime2JD_UTC(unixtime)); }
void EDateTime::addTime(long year, long month, double day, double hour, double minute, double second) {
    m_year += year;
    m_month += month;
    m_day += day;
    m_hour += hour;
    m_minute += minute;
    m_second += second;
    normalize();
    // !!! FIX: Store old JD, check if leap year was applied between the old and new JDs and adjust accordingly.
    //          Likewise for deltaT outside UTC validity, and when crossing UTC leap year boundary. !!!
    calcJDs();
}
EDateTime EDateTime::operator+=(EDateTime& other) {
    // A bit weird, uses other as a duration rather than a point in time
    // the JDs of other are thus without meaning
    addTime(other.m_year, other.m_month, other.m_day, other.m_hour, other.m_minute, other.m_second);
    return *this;
}
EDateTime EDateTime::operator+(EDateTime& other) {
    // A bit weird, uses other as a duration rather than a point in time
    // the JDs of other are thus without meaning
    EDateTime retval{ this->m_JD_UTC, false }; // copy myself using jd_utc ctor
    retval.addTime(other.m_year, other.m_month, other.m_day, other.m_hour, other.m_minute, other.m_second);
    return retval;
}
void EDateTime::normalize() {
    // FIX: !!! Should probably be a private function? !!!
    static long months[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // Year (is allowed to be any integer)
    // Month (must be in [1;12] range)
    while (m_month <= 0) {
        m_month += 12;
        m_year -= 1;
    }
    while (m_month > 12) {
        m_month -= 12;
        m_year += 1;
    }
    // Fractional day
    double accDay = m_second / 86400.0 + m_minute / 1440.0 + m_hour / 24.0 + m_day;
    // Make positive
    while ((int)accDay <= 0) {
        accDay += 365.0;
        if (isLeapYear(m_year) && m_month > 2) accDay += 1.0;
        m_year--;
    }
    // Normalize to length of month
    while ((int)accDay > (isLeapYear(m_year) && m_month == 2) * 1 + months[m_month]) {
        accDay -= months[m_month];
        if (m_month == 2 && isLeapYear(m_year)) accDay -= 1.0;
        m_month++;
        if (m_month > 12) {
            m_year += 1;
            m_month = 1;
        }
    }
    m_day = floor(accDay);
    // Bias slightly to avoid getting 0h59m59.99999999s instead of 1h0m0s (due to precision)
    // Note: Meeus uses a bias of 0.0001 seconds in the same manner.
    // Possibly a better solution is to use accSeconds instead of accDays, since double floats can represent all integers below 2^53 exactly.
    // Or rather, store the fractional part of m_seconds, then calculate everything in integer seconds, and then add the stored fractional part back.
    // For now, the bias does what it needs to do, and the seconds remain accurate to just below 2/1000000 of a second.
    // When rounding for display it is unnoticed, and the astronomical calculations have inaccuracies greater than any movement in that timespan.
    // Using addTime() normalizes every time, but even with thousands of calls the accumulation is in the miliseconds range.
    accDay -= m_day - 0.00000000001;
    accDay *= 24.0;
    m_hour = floor(accDay);
    accDay -= m_hour;
    accDay *= 60.0;
    m_minute = floor(accDay);
    accDay -= m_minute;
    m_second = accDay * 60.0;
}
void EDateTime::calcJDs() {
    // FIX: !!! Should probably be a private function? !!!
    // The AA library does not support proleptic gregorian dates, hence the below function which does.
    // Calculates Julian Date (decimal Julian Day) using method given here: https://www.aa.quae.nl/en/reken/juliaansedag.html
    // Verified using online calculator here: https://keisan.casio.com/exec/system/1227779487
    //  and here: https://neoprogrammics.com/jd-number-table-for-any-month/
    // NOTE: Those two online calculators skip year 0 for civil calendaring reasons.
    //       The below calculation has a full year 0 to facilitate simple subtraction of proleptic years across BCE/CE boundary.
    //       Thus, year -1 in the above calculators equals year 0 in below calculation, which equals year 1BCE.
    int a1 = myDivQuotient(m_month - 3, 12);
    int m1 = myDivRemainder(m_month - 3, 12);
    int c1 = myDivQuotient(m_year + a1, 100);
    int a2 = myDivRemainder(m_year + a1, 100);
    int J1 = myDivQuotient(146097 * c1, 4);
    int J2 = myDivQuotient(36525 * a2, 100);
    int J3 = myDivQuotient(153 * m1 + 2, 5);
    int J = J1 + J2 + J3 + (int)m_day + 1721119;
    double frac = (m_second + m_minute * 60.0 + m_hour * 3600.0) / 86400.0;
    // Compare to table in calculation source, note year -1000 entry is mixed between 15 Dec and 15 Aug for some reason.
    //std::cout << "Partials: " << a1 << "," << m1 << "," << c1 << "," << a2 << "," << J1 << "," << J2 << "," << J3 << "," << J << '\n';
    // Above J + frac is civil JD for midnight. Subtract 0.5 for astronomical JD.
    m_JD_UTC = (double)J - 0.5 + frac;
    m_JD_TT = getJDUTC2TT(m_JD_UTC);
}

// Static utility functions - can be used without instantiating
void EDateTime::normalizeDateTime(long& yr, long& mo, double& da, double& hr, double& mi, double& se) {
    // FIX: !!! Should probably return a new EDateTime instead of modifying in place, otherwise it won't work with pybind11 !!!
    static long months[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // Year (is allowed to be any integer)
    // Month
    while (mo <= 0) {
        mo += 12;
        yr -= 1;
    }
    while (mo > 12) {
        mo -= 12;
        yr += 1;
    }
    // Fractional day
    double accDay = se / 86400.0 + mi / 1440.0 + hr / 24.0 + da;
    // Make positive
    while ((int)accDay <= 0.0) {
        accDay += 365.0;
        if (isLeapYear(yr) && mo > 2) accDay += 1.0;
        yr--;
    }
    // Normalize to length of month
    while ((int)accDay > (isLeapYear(yr) && mo == 2) * 1 + months[mo]) {
        accDay -= months[mo];
        if (mo == 2 && isLeapYear(yr)) accDay -= 1.0;
        mo++;
        if (mo > 12) {
            yr += 1;
            mo = 1;
        }
    }
    da = floor(accDay);
    accDay -= da - 0.00000000001;
    accDay *= 24.0; // Fractional hours
    hr = floor(accDay);
    accDay -= hr;
    accDay *= 60.0; // Fractional minutes
    mi = floor(accDay);
    accDay -= mi;
    se = accDay * 60.0;  // Fractional seconds
}
int EDateTime::myDivQuotient(const int a, const int b) {
    // Quotient of div as defined in https://www.aa.quae.nl/en/reken/juliaansedag.html section 2.3
    return (int)floor((double)a / b);
}
int EDateTime::myDivRemainder(const int a, const int b) {
    // Remainder of div as defined in https://www.aa.quae.nl/en/reken/juliaansedag.html section 2.3
    int retval = a % b;
    if (retval < 0) retval += b;
    return retval;
}
double EDateTime::getDateTime2JD_UTC(const long year, const long month, const double day, const double hour, const double minute, const double second) {
    // normalizeDateTime() takes arguments by reference, thus would not work with literals
    long yr = year;
    long mo = month;
    double da = day;
    double hr = hour;
    double mi = minute;
    double se = second;
    normalizeDateTime(yr, mo, da, hr, mi, se);
    int a1 = myDivQuotient(mo - 3, 12);
    int m1 = myDivRemainder(mo - 3, 12);
    int c1 = myDivQuotient(yr + a1, 100);
    int a2 = myDivRemainder(yr + a1, 100);
    int J1 = myDivQuotient(146097 * c1, 4);
    int J2 = myDivQuotient(36525 * a2, 100);
    int J3 = myDivQuotient(153 * m1 + 2, 5);
    int J = J1 + J2 + J3 + (int)da + 1721119;
    double frac = (se + mi * 60.0 + hr * 3600.0) / 86400.0;
    //std::cout << "Date time (YYYY-MM-DD hh:mm:ss) to Julian Date: " << yr << "/" << mo << "/" << da << " " << hr << ":" << mi << ":" << se << " : " << JD << '\n';
    return (double)J - 0.5 + frac;
}
double EDateTime::getDateTime2JD_TT(const long year, const long month, const double day, const double hour, const double minute, const double second) {
    return getJDUTC2TT(getDateTime2JD_UTC(year, month, day, hour, minute, second));
}
void EDateTime::getJD2DateTime(double jd, long& year, long& month, double& day, double& hour, double& minute, double& second) {
    // if jd is in UTC, the date time will be in UTC. If in TT, then the date time will also be in TT.
    //
    // // TT / UTC correctness confirmed as per: https://aa.usno.navy.mil/faq/TT using code:
    //EDateTime J2k{ JD_2000, /* bool tt = */ true };
    //std::cout << J2k.string() << " detailed seconds: " << J2k.second() << '\n';
    // Output: 2000-01-01 11:58:56 UTC detailed seconds: 55.8159989118576
    //According to link above:
    // The epoch designated "J2000.0" is specified as Julian date 2451545.0 TT, or 2000 January 1, 12h TT.
    // This epoch can also be expressed as 2000 January 1, 11:59:27.816 TAI or 2000 January 1, 11:58:55.816 UTC.
    //
    // source for JD to date time calculation: https://www.aa.quae.nl/en/reken/juliaansedag.html#4_2
    jd += 0.5; // From astronomical jd to civil which is assumed below
    int c1 = myDivQuotient((int)jd * 4 - 6884477, 146097);
    int e1 = myDivRemainder((int)jd * 4 - 6884477, 146097) / 4;
    int a1 = myDivQuotient(100 * e1 + 99, 36525);
    int e2 = myDivRemainder(100 * e1 + 99, 36525) / 100;
    int m1 = myDivQuotient(5 * e2 + 2, 153);
    int e3 = myDivRemainder(5 * e2 + 2, 153) / 5;
    int a2 = myDivQuotient(m1 + 2, 12);
    int m2 = myDivRemainder(m1 + 2, 12);
    year = 100 * c1 + a1 + a2;
    month = m2 + 1;
    day = e3 + 1;
    double frac = jd - (int)jd + 0.00000000001;
    frac *= 24.0;
    hour = floor(frac);
    frac -= hour;
    frac *= 60.0;
    minute = floor(frac);
    frac -= minute;
    second = frac * 60.0;
}
double EDateTime::getUnixTime2JD_UTC(const long unixtime) { // this returns a JD in UTC
    // !!! FIX: IMPORTANT! Unix Time is leap second agnostic, UTC is NOT!
    //          See: https://en.wikipedia.org/wiki/Unix_time#Leap_seconds
    //          Figure out how to handle this and whether these calculations are OK or not.
    return ((double)unixtime / 86400.0) + JD_UNIX;
}
double EDateTime::getUnixTime2JD_TT(const long unixtime) { // this returns a JD in TT
    return getJDUTC2TT(getUnixTime2JD_UTC(unixtime));
}
// !!! ADD: getJD_UTC2xxx() and getJD_TT2xxx() functions for symmetry and ease of use.
//          With get JD_x2DateTime() above setters can be refactored a bit. !!!
long EDateTime::getDateTime2UnixTime(const long year, const long month, const double day, const double hour, const double minute, const double second) {
    // Both Unix time and the date time returned are in UTC
    // Verified to work with negative Unix timestamps too, i.e. dates before epoch (1970-01-01 00:00:00)
    // NOTE: The minimum representable date is Friday 1901-12-13, and the maximum representable date is Tuesday 2038-01-19
    //       The conversion does not check bounds!
    long y = (long)year - 1900;
    long ydays = (long)calcUnixTimeYearDay(year, month, day);
    //std::cout << "Day of Year: " << ydays << "\n";
    long utime = (long)second + (long)minute * 60 + (long)hour * 3600; // Day fraction
    //std::cout << "Day fraction: " << utime << "\n";
    utime += ydays * 86400; // Month and day as calculated above, including current leap year
    utime += (y - 70) * 31536000 + ((y - 69) / 4) * 86400 - ((y - 1) / 100) * 86400 + ((y + 299) / 400) * 86400; // Previous leap years adjustment
    return utime;
}
long EDateTime::getJD_UTC2UnixTime(double jd_utc) { // Unix time and JD in UTC
    return long((jd_utc - JD_UNIX) * 86400.0);
}
double EDateTime::getJD2MJD(const double jd) { /* JD to Modified JD */ return jd - MJD_BASE; }
double EDateTime::getMJD2JD(const double mjd) { /* Modified JD to JD */ return mjd + MJD_BASE; }
double EDateTime::getJDUTC2TT(const double jd_utc) { /* JD in UTC to JD in TT */
    if ((jd_utc < FIRST_LEAP_SECOND_JD || (jd_utc > LAST_LEAP_SECOND_JD + 500)))
        return jd_utc + (EDateTime::DeltaT(jd_utc) / 86400.0); // Outside leap second table, use deltaT
    return jd_utc + (EDateTime::CumulativeLeapSeconds(jd_utc) + 32.184) / 86400.0;
} // Inside leap second table, use value from there
double EDateTime::getJDTT2UTC(const double jd_tt) { /* JD in TT to JD in UTC */
    // Note: If provided TT is less than 1.4228180 seconds past Jan 1 1961, the resulting UTC will be off by 1.4228180 seconds
    if ((jd_tt < FIRST_LEAP_SECOND_JD || (jd_tt > LAST_LEAP_SECOND_JD + 500)))
        return jd_tt - (EDateTime::DeltaT(jd_tt) / 86400.0); // Outside leap second table, use deltaT
    return jd_tt - (EDateTime::CumulativeLeapSeconds(jd_tt) + 32.184) / 86400.0;
}
double EDateTime::TT2UT1(double JD) {
    return JD - (DeltaT(JD) / 86400.0);
}
double EDateTime::UT12TT(double JD) {
    return JD + (DeltaT(JD) / 86400.0);
}
double EDateTime::UT1MinusUTC(double JD) {
    return -(DeltaT(JD) - CumulativeLeapSeconds(JD) - 32.184);
}
long EDateTime::calcUnixTimeYearDay(const long year, const long month, const double day) {
    // Cannot handle most un-normalized dates correctly, so provide sane input dates
    static long months[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    long mo = month - 1;
    long days = 0;
    for (long m = 0; m < mo; m++) {
        days += months[m];
    }
    if (isLeapYear(year) && month > 2) days += 1;
    if (year < 1970) days -= 1;  // 
    return days + (long)day - 1;  // -1 because UnixTime starts on day zero, not day one.
}
bool EDateTime::isLeapYear(const long year) {
    if (year % 400 == 0) return true;
    if (year % 100 == 0) return false;
    if (year % 4 == 0) return true;
    return false;
}

double EDateTime::FractionalYear(double jd_utc) {
    //// !!! FIX: INFINITE LOOP !!! DeltaT_Polynomial -> FractionalYear -> EDateTime ctor -> getJDUTC2JDTT -> DeltaT -> DeltaT_Polynomial
    //EDateTime date{ jd_utc, false };
    //double DaysInYear{ date.isLeap() ? 366.0 : 365.0 };  // is date.isLeap() * 1.0 + 365.0  faster? Not currently since isLeap has 4 if's
    //return date.year() + ((jd_utc - getDateTime2JD_UTC(date.year(), 1, 1, 0, 0, 0)) / DaysInYear);
    long yr = 0, mo = 0;
    double da = 0.0, hr = 0.0, mi = 0.0, se = 0.0;
    getJD2DateTime(jd_utc, yr, mo, da, hr, mi, se);
    double DaysInYear{ isLeapYear(yr) ? 366.0 : 365.0 };
    return yr + ((jd_utc - getDateTime2JD_UTC(yr, 1, 1, 0, 0, 0)) / DaysInYear);
}
double EDateTime::DeltaT(double jd_utc) {
    double Delta{ 0 };
    //Determine if we can use the lookup table
    constexpr auto nLookupElements{ g_DeltaTValues.size() };
    //assert(nLookupElements >= 2);
    if ((jd_utc >= g_DeltaTValues[0].JD) && (jd_utc < g_DeltaTValues[nLookupElements - 1].JD)) {
        //Find the index in the lookup table which contains the JD value closest to the JD input parameter
        const DeltaTValue toFind{ jd_utc, 0 };
        const auto iter{ std::upper_bound(g_DeltaTValues.cbegin(), g_DeltaTValues.cend(), toFind, [](const DeltaTValue& element1, const DeltaTValue& element2) noexcept -> bool
          {
            return element1.JD < element2.JD;
          }) };
        //assert(iter != g_DeltaTValues.cend());
        const auto nFoundIndex{ iter - g_DeltaTValues.cbegin() };

        //Now do a simple linear interpolation of the DeltaT values from the lookup table
        Delta = (jd_utc - g_DeltaTValues[nFoundIndex - 1].JD) / (g_DeltaTValues[nFoundIndex].JD - g_DeltaTValues[nFoundIndex - 1].JD) * (g_DeltaTValues[nFoundIndex].DeltaT - g_DeltaTValues[nFoundIndex - 1].DeltaT) + g_DeltaTValues[nFoundIndex - 1].DeltaT;
    }
    else Delta = DeltaT_Polynomial(jd_utc);
    return Delta;
}
double EDateTime::DeltaT_Polynomial(double jd_utc) {
    const double y{ FractionalYear(jd_utc) };
    double Delta{ 0.0 };
    //Use the polynomial expressions from Espenak & Meeus 2006.
    // References: http://eclipse.gsfc.nasa.gov/SEcat5/deltatpoly.html
    // and http://www.staff.science.uu.nl/~gent0113/deltat/deltat_old.htm (Espenak & Meeus 2006 section)
    if (y < -500) {
        const double u{ (y - 1820) / 100.0 };
        const double u2{ u * u };
        Delta = -20 + (32 * u2);
    }
    else if (y < 500) {
        const double u{ y / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        const double u4{ u3 * u };
        const double u5{ u4 * u };
        const double u6{ u5 * u };
        Delta = 10583.6 + (-1014.41 * u) + (33.78311 * u2) + (-5.952053 * u3) + (-0.1798452 * u4) + (0.022174192 * u5) + (0.0090316521 * u6);
    }
    else if (y < 1600) {
        const double u{ (y - 1000) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        const double u4{ u3 * u };
        const double u5{ u4 * u };
        const double u6{ u5 * u };
        Delta = 1574.2 + (-556.01 * u) + (71.23472 * u2) + (0.319781 * u3) + (-0.8503463 * u4) + (-0.005050998 * u5) + (0.0083572073 * u6);
    }
    else if (y < 1700) {
        const double u{ (y - 1600) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        Delta = 120 + (-98.08 * u) + (-153.2 * u2) + (u3 / 0.007129);
    }
    else if (y < 1800) {
        const double u{ (y - 1700) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        const double u4{ u3 * u };
        Delta = 8.83 + (16.03 * u) + (-59.285 * u2) + (133.36 * u3) + (-u4 / 0.01174);
    }
    else if (y < 1860) {
        const double u{ (y - 1800) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        const double u4{ u3 * u };
        const double u5{ u4 * u };
        const double u6{ u5 * u };
        const double u7{ u6 * u };
        Delta = 13.72 + (-33.2447 * u) + (68.612 * u2) + (4111.6 * u3) + (-37436 * u4) + (121272 * u5) + (-169900 * u6) + (87500 * u7);
    }
    else if (y < 1900) {
        const double u{ (y - 1860) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        const double u4{ u3 * u };
        const double u5{ u4 * u };
        Delta = 7.62 + (57.37 * u) + (-2517.54 * u2) + (16806.68 * u3) + (-44736.24 * u4) + (u5 / 0.0000233174);
    }
    else if (y < 1920) {
        const double u{ (y - 1900) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        const double u4{ u3 * u };
        Delta = -2.79 + (149.4119 * u) + (-598.939 * u2) + (6196.6 * u3) + (-19700 * u4);
    }
    else if (y < 1941) {
        const double u{ (y - 1920) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        Delta = 21.20 + (84.493 * u) + (-761.00 * u2) + (2093.6 * u3);
    }
    else if (y < 1961) {
        const double u{ (y - 1950) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        Delta = 29.07 + (40.7 * u) + (-u2 / 0.0233) + (u3 / 0.002547);
    }
    else if (y < 1986) {
        const double u{ (y - 1975) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        Delta = 45.45 + 106.7 * u - u2 / 0.026 - u3 / 0.000718;
    }
    else if (y < 2005) {
        const double u{ (y - 2000) / 100.0 };
        const double u2{ u * u };
        const double u3{ u2 * u };
        const double u4{ u3 * u };
        const double u5{ u4 * u };
        Delta = 63.86 + (33.45 * u) + (-603.74 * u2) + (1727.5 * u3) + (65181.4 * u4) + (237359.9 * u5);
    }
    else if (y < 2050) {
        const double u{ (y - 2000) / 100.0 };
        const double u2{ u * u };
        Delta = 62.92 + (32.217 * u) + (55.89 * u2);
    }
    else if (y < 2150) {
        const double u{ (y - 1820) / 100.0 };
        const double u2{ u * u };
        Delta = -205.72 + (56.28 * u) + (32 * u2);
    }
    else {
        const double u{ (y - 1820) / 100.0 };
        const double u2 = u * u;
        Delta = -20 + (32 * u2);
    }
    return Delta;
}
double EDateTime::CumulativeLeapSeconds(double jd_utc) {
    double LeapSeconds{ 0 };
    if (jd_utc >= g_LeapSecondCoefficients[0].JD) {
        constexpr auto nLookupElements{ g_LeapSecondCoefficients.size() };
        //assert(nLookupElements >= 2);
        if (jd_utc < g_LeapSecondCoefficients[nLookupElements - 1].JD) {
            //Find the index in the lookup table which contains the JD value closest to the JD input parameter
            const LeapSecondCoefficient toFind{ jd_utc, 0, 0, 0 };
            const auto iter{ std::upper_bound(g_LeapSecondCoefficients.cbegin(), g_LeapSecondCoefficients.cend(), toFind, [](const LeapSecondCoefficient& element1, const LeapSecondCoefficient& element2) noexcept -> bool
              {
                return element1.JD < element2.JD;
              }) };

            //assert(iter != g_LeapSecondCoefficients.cend());
            const auto nFoundIndex{ iter - g_LeapSecondCoefficients.cbegin() };
            LeapSeconds = g_LeapSecondCoefficients[nFoundIndex - 1].LeapSeconds + (jd_utc - MJD_BASE - g_LeapSecondCoefficients[nFoundIndex - 1].BaseMJD) * g_LeapSecondCoefficients[nFoundIndex - 1].Coefficient;
        }
        else LeapSeconds = g_LeapSecondCoefficients[nLookupElements - 1].LeapSeconds + (jd_utc - MJD_BASE - g_LeapSecondCoefficients[nLookupElements - 1].BaseMJD) * g_LeapSecondCoefficients[nLookupElements - 1].Coefficient;
    }
    return LeapSeconds;
}

