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

// For debug convenience
#define VPRINT(v) std::cout << "(" << v.x << "," << v.y << "," << v.z << ")\n"