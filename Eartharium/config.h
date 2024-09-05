#pragma once

// C++ std::
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <algorithm>

// GLM
#include <glm/glm.hpp>

// AA+
//#include "AAplus/AAElliptical.h" // For the Enums - UPD: still using same enums, but not using AA+ anymore.

// EAstronomy
#include "../EAstronomy/config.h"

const unsigned int maxuint = 4294967295;     // pow(2,32)-1 used to represent 'none' for various indices
enum itemtype {
    SUN = 0, // CAAElliptical::Object::SUN, // Don't move these celestial objects, they need to match with AA+ enums - UPD: moved AA+ to Astronomy
    MERCURY,
    VENUS,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE,
    EARTH,
    MOON,
    LOC,    // From here onwards, you can rearrange as you please.
    ZENITH,
    NORTH,
    EAST,
    NORMAL,  // Calculated differently than Zenith
    FLATSKY_SP,
    FLATSKY_GP, // GP mode for placing celestial bodies over Earth
    FLATSKY_LC_DOME, // LC places everything at the correct AziEle for one particular location. If used on more than one location, objects are duplicated.
    FLATSKY_LC_PLANE,// DOME projects to a hemisphere, PLANE to a flat ceiling.
    FLATSKY_HD, // HALFDOME - Not a local setting
    LOCSKY,
    TRUESUN3D,
    TRUEANALEMMA3D,
    FLATSUN3D,
    FLATANALEMMA3D,
    SUNSECTOR,  // Corey Kell's 45 degree Sectors
    TRUEMOON3D,
    TRUELUNALEMMA3D,
    TRUEPLANET3D,
    SIDPLANET3D,
    AZIELE3D,
    RADEC3D,
    ARROWEXTENT,
    TANGENT,
    MERIDIAN,
    PARALLEL,
    GRIDLAT,    // Used to identify which were added by addGrid();
    GRIDLON,
    EQUATOR,
    PRIME_MERIDIAN,
    ARCTIC,
    TROPIC,
    LATITUDE,
    LONGITUDE,
    HORIZON,
    CIRCUMPOLAR,
    DOT,
    CURVE,
    GREATARC,
    LERPARC,
    FLATARC,    // Aka Derp Arc - shortest distance on AE map
    SUNTERMINATOR,
    MOONTERMINATOR,
    TISSOT,
    SEMITERMINATOR,
    FB_PLAIN,
    FB_CUBEMAP,
    SHADOW_MAP,
    SHADOW_BOX,
    EC,
    ECGEO,
    LAYER3D,
    LAYERTEXT,
    LAYERGUI,
    LAYERPLOT,
    HALT,        // These three are PathTracker modes
    LOOP,
    BOUNCE,
    REFR_BENNETT, // J.Meeus
    REFR_ALMANAC, // https://www.madinstro.net/sundry/navsext.html
    XYZ,          // Rotation orders, used in SceneObject::inherit()
    ZYX,
    ALL,
    NONE = maxuint,
};

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
// #define NO_DOUBLE maxdouble
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
#define VPRINT(v) std::cout << "(" << v.x << "," << v.y << "," << v.z << ")"
#define VPRINTNL(v) std::cout << "(" << v.x << "," << v.y << "," << v.z << ")\n"


// TimeZones and CountryBorders config
const float surface_offset = 0.0001f;

// Mathematical constants
// const double pi = 3.1415926535897932384626433832795;
const float pif = 3.1415926535897932384626433832795f;
// const double tau = 2.0 * pi;
const float tauf = 2.0f * pif;
// const double pi2 = pi / 2.0;
const float pi2f = pif / 2.0f;
// const double pi4 = pi / 4.0;
const float pi4f = pif / 4.0f;
const double twopi = 2.0 / pi;
// const double deg2rad = tau / 360.0;          // 0.017453292519943295769236907684886
const float deg2radf = tauf / 360.0f;
// const double rad2deg = 360.0 / tau;          // 57.295779513082320876798154814105
const float rad2degf = 360.0f / tauf;
// const double hrs2deg = 15.0;
// const double deg2hrs = 1.0 / 15.0;
// const double rad2hrs = 24.0 / tau;           // 3.8197186342054880584532103209403
// const double hrs2rad = tau / 24.0;           // 0.26179938779914943653855361527329
const double ninety = deg2rad * 89.99999;    // Used to avoid singularity at poles
const double tiny = 0.00001;                 // Used to determine practically zero
const double verytiny = 0.0000001;           // even closer to zero
const float maxfloat = FLT_MAX;
// const double maxdouble = DBL_MAX;

// Astronomical constants
//// For FK5
//const double JD_2000 = 2'451'545.0;               // Standard Epoch J2000 in Julian Day
//// For FK4
//const double JD_B1950 = 2'415'020.3135;           // Besselian Epoch in Julian Day
//const double JD_TROPICAL_CENTURY = 36524.2199;    // Duration of a Besselian (Tropical) Century in Julian Days
//// For Moon Phase K calculations
//const double JD_0000 = 1'721'045.0;               // JD of epoch 0000-01-01 0:00:00 = JD_2000 - (JD_YEAR * 2000.0)
//// For period calculations
//const double JD_YEAR = 365.25;                    // Julian year
//const double JD_CENTURY = 36'525.0;               // Julian century
//const double JD_MILLENNIUM = 365'250.0;           // Julian millennium
//// For deltaT lookups
//const double FIRST_LEAP_SECOND_JD = 2'437'300.5;  // 1961 Jan 1, first entry in Leap Second Table
//const double LAST_LEAP_SECOND_JD = 2'457'754.5;   // 2017, currently last entry in Leap Second Table
//// For Modified Julian Day calculations
//const double MJD_BASE = 2'400'000.5;
//// UNIX epoch, used when converting timestamps
//const double JD_UNIX = 2'440'587.5;               // Unix epoch 1970-01-01 00:00:00
// Earth parameters
const double earthradius = 6371.008;              // Earth average radius in kilometers
const double earthradiusm = 6'371'008;            // Earth average radius in meters
//const double earthaxialtilt = 23.439281;        // degrees (2007 wikipedia). Value changes over time, as Earth axis wobbles
//const double earthtropics = 23.4365;            // degrees (DMS: 23°26'11.4"). Value fixed by convention - No !!! Is defined as obliquity of ecliptic
//const double eartharctics = 66.5635;            // NOTE: Actually changes over time, but set here to align with tropics
//const double astronomicalunit = 149'597'870.7;    // 1 AU in km, from https://en.wikipedia.org/wiki/Astronomical_unit
const double sunradius = 696'340.0;               // Some uncertainty around this number: https://academic.oup.com/mnras/article/276/2/476/998827
const double moonradius = 1737.3;
const double sidereals = 86'164.09053083288;      // Seconds in 1 sidereal day
const double sidereald = sidereals / 86400.0;     // Sidereal day as fraction of calendar day
const double lunalemmad = 1.035028;
//const double km2au = astronomicalunit;
//const double au2km = 1.0 / astronomicalunit;

// WGS84 geometry
// Source: https://www.osti.gov/servlets/purl/231228
const double majorAxisWGS84 = 6378.1370;
const double minorAxisWGS84 = 6356.7523142;
const double flatteningWGS84 = (majorAxisWGS84 - minorAxisWGS84) / majorAxisWGS84;
const double eSquaredWGS84 = ((majorAxisWGS84 * majorAxisWGS84) - (minorAxisWGS84 * minorAxisWGS84)) / (majorAxisWGS84 * majorAxisWGS84);
const double ePrimeSquaredWGS84 = ((majorAxisWGS84 * majorAxisWGS84) - (minorAxisWGS84 * minorAxisWGS84)) / (minorAxisWGS84 * minorAxisWGS84);


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
static void to_upper(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}
static void stringreplace(std::string& subject, const std::string& search, const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

// Math helper functions
static glm::vec3 projectVector2Plane(glm::vec3 vector, glm::vec3 normal) {
	// Projects vector onto the plane defined by the normal
	return vector - normal * glm::dot(vector, normal) / (float)pow(glm::length(normal), 2);
}


// ----------
//  TightVec
// ----------
template<typename T>
class tightvec {
public:
    std::vector<T> m_Elements;
    std::map<size_t, size_t> m_remap;
    std::map<size_t, size_t> m_xmap;
    std::vector<size_t> m_freed;
    //tightvec<T>();
    //~tightvec<T>();
    size_t size();
    size_t capacity();
    void clear();
    bool empty();
    size_t physFirst();
    T& operator[](size_t oid);
    void reserve(size_t reserve);
    size_t store(T element, bool debug = false);
    T& retrieve(size_t oid);
    void update(size_t oid, T element);
    void remove(size_t oid, bool debug = false);
};
template<typename T>
size_t tightvec<T>::size() {
    return m_Elements.size();
}
template<typename T>
size_t tightvec<T>::capacity() {
    return m_Elements.capacity();
}
template<typename T>
bool tightvec<T>::empty() {
    return m_Elements.empty();
}
template<typename T>
void tightvec<T>::clear() {
    m_Elements.clear();
    m_freed.clear();
    m_remap.clear();
    m_xmap.clear();
}
template<typename T>
size_t tightvec<T>::physFirst() {
    return m_xmap[0];
}
template<typename T>
T& tightvec<T>::operator[](size_t oid) {
    //std::cout << "oid: " << oid << "\n";
    return retrieve(oid);
}
template<typename T>
void tightvec<T>::reserve(size_t reserve) {
    m_Elements.reserve(reserve);
}
template<typename T>
size_t tightvec<T>::store(T element, bool debug) {
    if (debug) std::cout << "tightvec{" << this << "}::store() called\n"; // Can't print element, type is not known at compile time
    if (debug) for (auto& o : m_remap) {
        std::cout << "tightvec{" << this << "}::store(): m_remap " << o.first << ", " << o.second << "\n";
    }
    if (debug) for (auto& o : m_xmap) {
        std::cout << "tightvec{" << this << "}::store(): m_xmap " << o.first << ", " << o.second << "\n";
    }
    if (debug) for (auto& o : m_freed) {
        std::cout << "tightvec{" << this << "}::store(): m_freed " << o << "\n";
    }
    size_t oid, iid;
    iid = m_Elements.size(); // vector is compact, so always add to end. No! Well, yes. Because remove() fills gaps.
    m_Elements.emplace_back(element);
    if (m_freed.size() > 0) { // Recycle freed outer indices
        oid = m_freed.back();
        m_freed.pop_back();
    }
    else oid = iid;
    if (debug) std::cout << "Stored item in m_Elements[" << iid << "], using reference (oid): " << oid << "\n";
    //std::cout << "aElem - Added to end entry: " << oid << "\n";
    m_remap[oid] = iid;  // remaps oid to iid
    m_xmap[iid] = oid;   // maps iid to oid, used to "ripple" remap
    if (debug) for (auto& o : m_remap) {
        std::cout << "tightvec{" << this << "}::store(): m_remap " << o.first << ", " << o.second << "\n";
    }
    if (debug) for (auto& o : m_xmap) {
        std::cout << "tightvec{" << this << "}::store(): m_xmap " << o.first << ", " << o.second << "\n";
    }
    if (debug) for (auto& o : m_freed) {
        std::cout << "tightvec{" << this << "}::store(): m_freed " << o << "\n";
    }
    return oid;
}
template<typename T>
T& tightvec<T>::retrieve(size_t oid) {
    if (m_remap.count(oid)) return m_Elements[m_remap[oid]];
    else {
        std::cout << "ERROR: tightvec::retrieve() or tightvec[] called with non-existing index: " << oid << "\n";
        return m_Elements.back();
    }
}
template<typename T>
void tightvec<T>::update(size_t oid, T element) {
    size_t iid = m_remap[oid];
    m_Elements[iid] = element;
}
template<typename T>
void tightvec<T>::remove(size_t oid, bool debug) {
    if (debug) std::cout << "tightvec{" << this << "}::remove(" << oid << ") called\n";
    if (m_Elements.size() == 0) {  // calling pop_back() on empty vector is undefined, so avoid it.
        std::cout << "tightvec<T>:Remove(): Reached m_Elements of ZERO size, yet was asked to delete an entry!! Ignoring!\n\n";
        return;
    }
    if (debug) for (auto& o : m_remap) {
        std::cout << "tightvec{" << this << "}::remove(): m_remap " << o.first << ", " << o.second << "\n";
    }
    if (debug) for (auto& o : m_xmap) {
        std::cout << "tightvec{" << this << "}::remove(): m_xmap " << o.first << ", " << o.second << "\n";
    }
    if (debug) for (auto& o : m_freed) {
        std::cout << "tightvec{" << this << "}::remove(): m_freed " << o << "\n";
    }
    size_t iid = m_remap[oid];
    if (debug) std::cout << "oid, iid: " << oid << ", " << iid << "\n";
    size_t last = m_Elements.size() - 1;
    //std::cout << "oid: " << oid << "\n";
    //std::cout << "iid: " << iid << "\n";
    //std::cout << "last: " << last << "\n";
    if (iid != last) { // Removing element in middle of vector
        m_Elements[iid] = m_Elements[last]; //.back();
        //std::cout << "Replace element: " << iid << " with " << last << "\n";
        size_t loid = m_xmap[last]; // If last was previously relocated, get oid of last. If not relocated, it still points last.
        m_remap[loid] = iid;
        if (debug) std::cout << "setting m_remap[" << loid << "] = " << iid << "\n";
        m_xmap[iid] = loid;
        if (debug) std::cout << "setting m_xmap[" << iid << "] = " << loid << "\n";
        //m_freed.push_back(oid);
    }
    // moved here:
    m_freed.push_back(oid);
    m_Elements.pop_back();
    m_remap.erase(oid);
    m_xmap.erase(last);
    if (debug) for (auto& o : m_remap) {
        std::cout << "tightvec{" << this << "}::remove(): m_remap " << o.first << ", " << o.second << "\n";
    }
    if (debug) for (auto& o : m_xmap) {
        std::cout << "tightvec{" << this << "}::remove(): m_xmap " << o.first << ", " << o.second << "\n";
    }
    if (debug) for (auto& o : m_freed) {
        std::cout << "tightvec{" << this << "}::remove(): m_freed " << o << "\n";
    }
};
