#pragma once

#include <string>
#include <glm/glm.hpp>

// Camera movement via keyboard - These limits are used in Eartharium.cpp:keyboard_callback() for GLFW
#define CAMERA_MAX_DIST 100.0f
#define CAMERA_MIN_DIST 2.0f
#define CAMERA_STEP_FACTOR 0.1f // Multiplier / divisor for camera distance because 1 to 2 degrees doubles the FoV but 90 to 91 barely changes it 
#define CAMERA_MAX_FOV 180.0f  // In degrees - May need revision if implementing 360 degree virtual videos
#define CAMERA_MIN_FOV 1.0f    // In degrees - extremely low FoV angles cause a hperbolic effect
#define CAMERA_FOV_FACTOR 0.1f // Multiplier / divisor for FoV because 1 to 2 degrees doubles the FoV but 90 to 91 barely changes it
#define CAMERA_MAX_LATITUDE 90.0f // Latitude locked to +/-90 to avoid turning upside down
#define CAMERA_MIN_LATIDUDE -90.0f
//#define CAMERA_MAX_LONGITUDE 180.0f // Unrestricted to circle limitless
//#define CAMERA_MIN_LONGIDUDE -180.0f
#define CAMERA_ANGLE_STEP 2.0f

// Basic colors - The names explains it all
#define WHITE glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define GREY glm::vec4(0.5f,0.5f,0.5f,1.0f)
#define BLACK glm::vec4(0.0f,0.0f,0.0f,1.0f)
#define RED glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)
#define ORANGE glm::vec4(0.7f, 0.3f, 0.0f, 1.0f)
#define YELLOW glm::vec4(1.0f, 1.0f, 0.0f, 1.0f)
#define GREEN glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)
#define BLUE glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)
#define PURPLE glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)
#define LIGHT_GREY glm::vec4(0.8f, 0.8f, 0.8f, 1.0f)
#define LIGHT_RED glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
#define LIGHT_ORANGE glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)
#define LIGHT_YELLOW glm::vec4(1.0f, 1.0f, 0.3f, 1.0f)
#define LIGHT_GREEN glm::vec4(0.3f, 1.0f, 0.3f, 1.0f)
#define LIGHT_BLUE glm::vec4(0.3f, 0.3f, 1.0f, 1.0f)
#define LIGHT_PURPLE glm::vec4(1.0f, 0.5f, 1.0f, 1.0f)
#define AQUA glm::vec4(0.0f, 1.0f, 1.0f, 1.0f)

// Default object colors - For non-textured solar system view, and planet arrows/paths in Earth and Location
#define SUNCOLOR glm::vec4(0.8f, 0.8f, 0.0f, 1.0f)
#define MERCURYCOLOR glm::vec4(0.6f, 0.6f, 0.6f, 1.0f)
#define VENUSCOLOR glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)
#define EARTHCOLOR glm::vec4(0.0f, 0.5f, 1.0f, 1.0f)
#define MARSCOLOR glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
#define JUPITERCOLOR glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)
#define SATURNCOLOR glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)
#define URANUSCOLOR glm::vec4(0.5f, 1.0f, 1.0f, 1.0f)
#define NEPTUNECOLOR glm::vec4(0.5f, 0.5f, 1.0f, 1.0f)
#define MOONCOLOR LIGHT_GREY

#define SUNMERCOLOR BLUE

// No care values - Used to indicate that no value is provided. Used to keep existing values whatever they are in functions that receive multiple values
#define NO_UINT maxuint
#define NO_FLOAT maxfloat
#define NO_DOUBLE maxdouble
#define NO_VEC2 glm::vec2(maxfloat)
#define NO_VEC3 glm::vec3(maxfloat)
#define NO_VEC4 glm::vec4(maxfloat)
#define NO_COLOR glm::vec4(maxfloat)  // Used to mark nocare values in method calls

// Initial reservation values - Not hard limits, simply prevents a lot of vector re-allocations
#define DOTS_RESERVE 50000

// Earth config
constexpr auto NE_SMEAR = 0.001f;        // Stepsize for estimating North and East vectors in Earth class (numerical differentiation)
const float latlonwidth = 0.002f;        // Latitude / longitude path width
const float llprimewidth = 0.003f;       // Prime meridian and equator path width
const float pathwidth = 0.003f;
const float sumnerlinewidth = pathwidth;

// Location config
// - LocDot -
#define DEFAULT_LOCDOT_COLOR glm::vec4(1.0f, 0.3f, 0.3f, 1.0f)
const float locdotsize = 0.01f;
// - LocLine -
const float loclinewidth = 0.001f;
// - LocArrow -
const float loccoordarrowwidth = 0.005f;
const float locazielewidth = 0.003f;
const float locsunarrowwidth = 0.003f;
// - LocPath -
const float locpathwidth = 0.002f;       // Default local sky paths width
const float locanalemmawidth = locpathwidth;
const float locsunpathwidth = locpathwidth;

// Primitives cofig
// - PolyCurve -
const size_t polycurvereserve = 5000; // default number of points in a path to reserve

// For debug convenience
#define VPRINT(v) std::cout << "(" << v.x << "," << v.y << "," << v.z << ")\n"


// TimeZones and CountryBorders config
const float surface_offset = 0.0001f;

// Mathematical constants
const double pi = 3.14159265358979323846;
const double tau = 2.0 * pi;
const double pi2 = pi / 2.0;
const double pi4 = pi / 4.0;
const double twopi = 2 / pi;
const double deg2rad = tau / 360.0;
const double rad2deg = 360.0 / tau;
const float pif = 3.14159265358979323846f;
const float tauf = 2.0f * pif;
const float pi2f = pif / 2.0f;
const float pi4f = pif / 4.0f;
const float deg2radf = tauf / 360.0f;
const float rad2degf = 360.0f / tauf;
const double hrs2dec = 15.0;
const double dec2hrs = 1.0 / 15.0;
const double rad2hrs = 24.0 / tau;
const double hrs2rad = tau / 24.0;
const double ninety = deg2rad * 89.99999;    // Used to avoid singularity at poles
const double tiny = 0.00001;                 // Used to determine practically zero
const double verytiny = 0.0000001;           // even closer to zero
const unsigned int maxuint = 4294967295;     // pow(2,32)-1 used to represent 'none' for various indices
const float maxfloat = FLT_MAX;
const double maxdouble = DBL_MAX;

// Astronomical constants
const double JD2000 = 2451545.0;             // Standard Epoch J2000 in Julian Day
const double FIRST_LEAP_SECOND_JD = 2437300.5; // 1961 Jan 1, first entry in Leap Second Table
const double LAST_LEAP_SECOND_JD = 2457754.5;  // 2017, currently last entry in Leap Second Table
const double earthradius = 6371.0;           // Earth average radius in kilometers
//const double earthaxialtilt = 23.439281;     // degrees (2007 wikipedia). Value changes over time, as Earth axis wobbles
//const double earthtropics = 23.4365;         // degrees (DMS: 23°26'11.4"). Value fixed by convention - No !!! Is defined as obliquity of ecliptic
//const double eartharctics = 66.5635;         // NOTE: Actually changes over time, but set here to align with tropics
const double astronomicalunit = 149597870.7; // 1 AU in km, from https://en.wikipedia.org/wiki/Astronomical_unit
const double sunradius = 696340.0;           // Some uncertainty around this number: https://academic.oup.com/mnras/article/276/2/476/998827
const double moonradius = 1737.3;
const double sidereals = 86164.09053083288;  // Seconds in 1 sidereal day
const double sidereald = sidereals / 86400.0;// Sidereal day as fraction of calendar day
const double lunalemmad = 1.035028;
const double km2au = astronomicalunit;
const double au2km = 1.0 / astronomicalunit;

// WGS84 geometry
// Source: https://www.osti.gov/servlets/purl/231228
const double majorAxisWGS84 = 6378.1370;
const double minorAxisWGS84 = 6356.7523142;
const double flatteningWGS84 = (majorAxisWGS84 - minorAxisWGS84) / majorAxisWGS84;
const double eSquaredWGS84 = ((majorAxisWGS84 * majorAxisWGS84) - (minorAxisWGS84 * minorAxisWGS84)) / (majorAxisWGS84 * majorAxisWGS84);
const double ePrimeSquaredWGS84 = ((majorAxisWGS84 * majorAxisWGS84) - (minorAxisWGS84 * minorAxisWGS84)) / (minorAxisWGS84 * minorAxisWGS84);

static double dms2rad(double d, double m, double s) {
	if (d >= 0.0) return deg2rad * (d + m / 60.0 + s / 3600.0);
	return deg2rad * (d - m / 60.0 - s / 3600.0);
}
static double dms2deg(double d, double m, double s) {
	if (d >= 0.0) return d + m / 60.0 + s / 3600.0;
	return d - m / 60.0 - s / 3600.0;
}
static double hms2rad(double h, double m, double s) {
	if (h >= 0.0) return deg2rad * (h * 15.0 + m / 60.0 + s / 3600.0);
	return deg2rad * (h * 15.0 - m / 60.0 - s / 3600.0);
}
static double hms2deg(double h, double m, double s) {
	if (h >= 0.0) return h * 15.0 + m / 60.0 + s / 3600.0;
	return h * 15.0 - m / 60.0 - s / 3600.0;
}
// String helper functions
// Source: https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
static const char* whitespace = " \t\n\r\f\v";
static std::string& rtrim(std::string& s, const char* t = whitespace) {
	// trim from end of string (right)
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}
static std::string& ltrim(std::string& s, const char* t = whitespace) {
	// trim from beginning of string (left)
	s.erase(0, s.find_first_not_of(t));
	return s;
}
static std::string& trim(std::string& s, const char* t = whitespace) {
	// trim from both ends of string (right then left)
	return ltrim(rtrim(s, t), t);
}
// Math helper functions
static glm::vec3 projectVector2Plane(glm::vec3 vector, glm::vec3 normal) {
	// Projects vector onto the plane defined by the normal
	return vector - normal * glm::dot(vector, normal) / (float)pow(glm::length(normal), 2);
}
static double clampmPitoPi(double radians) {
	while (radians < -pi) radians += tau;
	while (radians > pi) radians -= tau;
	return radians;
}
static double clamp0to360(double degrees) {
	while (degrees > 360.0) degrees -= 360.0;
	while (degrees < 0.0) degrees += 360.0;
	return degrees;
}
static double clamp0toTau(double radians) {
	// See Astronomy::rangezero2tau(double rad) !!!
	while (radians > tau) radians -= tau;
	while (radians < 0.0) radians += tau;
	return radians;
}
static double clamp0to24(double radians) {
	// See Astronomy::rangezero2tau(double rad) !!!
	while (radians > 24.0) radians -= 24.0;
	while (radians < 0.0) radians += 24.0;
	return radians;
}