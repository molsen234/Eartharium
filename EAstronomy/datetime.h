#pragma once

#include <string>

// -----------
//  EDateTime
// -----------
class EDateTime {
	// TODO: !!! Merge with DateTime class in Utilities - Done, except Utilities/TimeZones still use DateTime in some places
	// TODO: !!! Integrate with the TimeZones class (in Utilities), to accept setTime() with a timezone and convert automatically
public:
	// Constructors
	// - Default constructor sets current system time in UTC
	EDateTime();
	// - From date and time in UTC
	EDateTime(long year, long month, double day, double hour, double minute, double second);
	// - From Julian Date
	EDateTime(double jd_utc, bool is_tt = false);
	// - From a Unix Time Stamp
	EDateTime(long unixtime);
	// Getters
	long year() const;
	long month() const;
	double day() const;
	double hour() const;
	double minute() const;
	double second() const;
	double jd_tt() const;
	double jd_utc() const;
	bool isLeap() const;
	long weekday() const;
	long dayofyear() const;
	std::string string() const;
	std::string stringms() const;
	long unixTime() const;
	// Setters
	// - Set with date and time in UTC
	void setTime(long year, long month, double day, double hour, double minute, double second);
	// - Set to current system time in UTC
	void setTimeNow();
	// - Set to specific astronomical Julian Date given in UTC
	void setJD_UTC(double jd_utc);
	// - Set to specific astronomical Julian Date given in TT (Dynamical Time)
	void setJD_TT(double jd_tt);
	// - Set from Unix timestamp (seconds since 1970-01-01 00:00:00)
	void setUnixTime(long unixtime);
	void addTime(long year, long month, double day, double hour, double minute, double second);
	// Operators
	bool operator==(EDateTime& other) { return (m_JD_UTC == other.m_JD_UTC); }
	bool operator!=(EDateTime& other) { return (m_JD_UTC != other.m_JD_UTC); }
	bool operator<(EDateTime& other) { return (m_JD_UTC < other.m_JD_UTC); }
	bool operator>(EDateTime& other) { return (m_JD_UTC > other.m_JD_UTC); }
	bool operator<=(EDateTime& other) { return (m_JD_UTC <= other.m_JD_UTC); }
	bool operator>=(EDateTime& other) { return (m_JD_UTC >= other.m_JD_UTC); }
	EDateTime operator+=(EDateTime& other);
	EDateTime operator+(EDateTime& other);
	// Static utility functions - can be used without instantiating
	static void normalizeDateTime(long& yr, long& mo, double& da, double& hr, double& mi, double& se);
	static int myDivQuotient(const int a, const int b);
	static int myDivRemainder(const int a, const int b);
	static double getDateTime2JD_UTC(const long year, const long month, const double day, const double hour, const double minute, const double second);
	static double getDateTime2JD_TT(const long year, const long month, const double day, const double hour, const double minute, const double second);
	static void getJD2DateTime(double jd, long& year, long& month, double& day, double& hour, double& minute, double& second);
	static double getUnixTime2JD_UTC(const long unixtime);
	static double getUnixTime2JD_TT(const long unixtime);
	// !!! ADD: getJD_UTC2xxx() and getJD_TT2xxx() functions for symmetry and ease of use.
	//          With get JD_x2DateTime() above setters can be refactored a bit. !!!
	static long getDateTime2UnixTime(const long year, const long month, const double day, const double hour, const double minute, const double second);
	static long getJD_UTC2UnixTime(double jd_utc);
	static double getJD2MJD(const double jd);
	static double getMJD2JD(const double mjd);
	static double getJDUTC2TT(const double jd);
	static double getJDTT2UTC(const double jd);
	static long calcUnixTimeYearDay(const long year, const long month, const double day);
	static bool isLeapYear(const long year);

	static double FractionalYear(double jd_utc);

	// From AA+ v2.49 CAADynamicalTime
	static double DeltaT(double jd_utc);
	static double DeltaT_Polynomial(double jd_utc);
	static double CumulativeLeapSeconds(double JD);
	static double TT2UTC(double JD);
	static double UTC2TT(double JD);
	constexpr static double TT2TAI(double JD) { return JD - (32.184 / 86400.0); }
	constexpr static double TAI2TT(double JD) { return JD + (32.184 / 86400.0); }
	static double TT2UT1(double JD);
	static double UT12TT(double JD);
	static double UT1MinusUTC(double JD);
private:
	void normalize();
	void calcJDs();

private:
	// Dynamical Time
	double m_JD_TT = 0;  // astronomical jd in tt
	// UT (Civil Time)
	double m_JD_UTC = 0; // astronomical jd in utc
	// Proleptic Gregorian date in UTC
	long m_year = 0;
	long m_month = 0;
	double m_day = 0.0;
	// Time of Day always in UTC
	double m_hour = 0.0;
	double m_minute = 0.0;
	double m_second = 0.0;
	//std::string timezone;  // TODO
};
