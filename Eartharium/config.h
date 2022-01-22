#pragma once

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
#define LIGHT_RED glm::vec4(1.0f, 0.2f, 0.2f, 1.0f)
#define LIGHT_ORANGE glm::vec4(1.0f, 0.5f, 0.0f, 1.0f)
#define LIGHT_YELLOW glm::vec4(1.0f, 1.0f, 0.3f, 1.0f)
#define LIGHT_GREEN glm::vec4(0.3f, 1.0f, 0.3f, 1.0f)
#define LIGHT_BLUE glm::vec4(0.3f, 0.3f, 1.0f, 1.0f)
#define LIGHT_GREY glm::vec4(0.8f, 0.8f, 0.8f, 1.0f)
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

// No care values - Used to indicate that no value is provided. Used to keep existing values whatever they are in functions that receive multiple values
#define NO_UINT maxuint
#define NO_FLOAT maxfloat
#define NO_DOUBLE maxdouble
#define NO_VEC2 glm::vec2(maxfloat)
#define NO_VEC3 glm::vec3(maxfloat)
#define NO_VEC4 glm::vec4(maxfloat)
#define NO_COLOR glm::vec4(maxfloat)  // Used to mark nocare values in method calls

// Earth config
constexpr auto NE_SMEAR = 0.001f;        // Stepsize for estimating North and East vectors in Earth class (numerical differentiation)
const float latlonwidth = 0.002f;        // Latitude / longitude path width
const float llprimewidth = 0.003f;       // Prime meridian and equator path width

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
const double ninety = deg2rad * 89.99999;  // Used to avoid singularity at poles
const double tiny = 0.00001;               // Used to determine practically zero
const unsigned int maxuint = 4294967295;   // pow(2,32)-1 used to represent 'none' for various indices
const float maxfloat = FLT_MAX;
const double maxdouble = DBL_MAX;

// Astronomical constants
const double earthradius = 6378.1370;  // kilometers
const double earthaxialtilt = 23.439281; // degrees (2007 wikipedia). Value changes over time, as Earth axis wobbles
const double earthtropics = 23.4365;   // degrees (DMS: 23°26'11.4"). Value fixed by convention - No !!! Is defined as obliquity of ecliptic
const double eartharctics = 66.5635;   // NOTE: Actually changes over time, but set here to align with tropics
const double astronomicalunit = 149597870.7; // 1 AU in km, from https://en.wikipedia.org/wiki/Astronomical_unit
const double sunradius = 696340.0;     // Some uncertainty around this number: https://academic.oup.com/mnras/article/276/2/476/998827
const double moonradius = 1737.3;
const double sidereals = 86164.09053083288; // Seconds in 1 sidereal day
const double sidereald = sidereals / 86400; // Sidereal day as fraction of calendar day
const double km2au = astronomicalunit;
const double au2km = 1 / astronomicalunit;

// Math helper functions
inline glm::vec3 projectVector2Plane(glm::vec3 vector, glm::vec3 normal) {
	// Projects vector onto the plane defined by the normal
	return vector - normal * glm::dot(vector, normal) / (float)pow(glm::length(normal), 2);
}
inline double clamp0to360(double degrees) {
	while (degrees > 360.0) degrees -= 360.0;
	while (degrees < 0.0) degrees += 360.0;
	return degrees;
}
inline double clamp0toTau(double radians) {
	// See Astronomy::rangezero2tau(double rad) !!!
	while (radians > tau) radians -= tau;
	while (radians < 0.0) radians += tau;
	return radians;
}