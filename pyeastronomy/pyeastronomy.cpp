
// -----------------------
//  Pybind11 requirements
// -----------------------
//#include <python310/Python.h>

//#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/operators.h>  // For py::self operator overload
//#include <pybind11/stl.h>
namespace py = pybind11;

//  Self
//#include "pythoninterface.h"

// -------------------------------------
//  Used in definitions of Python calls
// -------------------------------------
// External libs
//#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

// base astronomy functions
#include "acoordinates.h"
#include "datetime.h"
#include "aellipsoids.h"
#include "asun.h"
#include "aearth.h"
#include "amoon.h"
#include "avenus.h"
#include "aelliptical.h"


// -------------------------------------
//  Python scripting module definitions
// -------------------------------------
PYBIND11_MODULE(pyeastronomy, m) {
    // IMPORTANT: Define things in order of dependencies. If not, loading the module will hang with no errors. Very annoying!
    m.doc() = "Eartharium Astronomy Module for python";
    //m.def("getApplication", &PyIface::getApplication, py::return_value_policy::reference)
    //    ;
    py::class_<LLD>(m, "LLD")
        .def(py::init<double, double, double>())
        .def("str", &LLD::str, "Output in raw units (radians and whichever unit distance is in).")
        .def("str_EQ", &LLD::str_EQ, "Output in Equatorial format.")
        .def("str_EC", &LLD::str_EC, "Output in Ecliptic format.")
        // operator overloads from https://pybind11.readthedocs.io/en/stable/advanced/classes.html#operator-overloading
        .def(py::self *= double())
        .def(py::self += py::self)
        .def(py::self + py::self)
        .def(py::self * double())
        .def_readwrite("lat", &LLD::lat)
        .def_readwrite("lon", &LLD::lon)
        .def_readwrite("dst", &LLD::dst)
        ;
    py::class_<glm::vec3>(m, "vec3") // Mostly from: https://github.com/xzfn/vmath/blob/master/src/wrap_vmath.cpp
        .def("__repr__", [](glm::vec3& self) { return glm::to_string(self); })
        .def(py::init([]() { return glm::vec3(0.0f, 0.0f, 0.0f); }))
        .def(py::init<float, float, float>())
        .def_readwrite("x", &glm::vec3::x)
        .def_readwrite("y", &glm::vec3::y)
        .def_readwrite("z", &glm::vec3::z)
        ;
    py::class_<glm::vec4>(m, "vec4") // Must be before classes that use it. No errors are generated, but it crashes at runtime.
        .def("__repr__", [](glm::vec4& self) { return glm::to_string(self); })
        .def(py::init([]() { /* std::cout << "C++ creating vec4!\n"; */ return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); }))
        .def(py::init<float, float, float, float>())
        .def_readwrite("x", &glm::vec4::x)
        .def_readwrite("y", &glm::vec4::y)
        .def_readwrite("z", &glm::vec4::z)
        .def_readwrite("w", &glm::vec4::w)
        ;
    py::class_<glm::dvec3>(m, "dvec3") // Mostly from: https://github.com/xzfn/vmath/blob/master/src/wrap_vmath.cpp
        .def("__repr__", [](glm::dvec3& self) { return glm::to_string(self); })
        .def(py::init([]() { return glm::dvec3(0.0, 0.0, 0.0); }))
        .def(py::init<double, double, double>())
        .def_readwrite("x", &glm::dvec3::x)
        .def_readwrite("y", &glm::dvec3::y)
        .def_readwrite("z", &glm::dvec3::z)
        ;
    py::class_<EDateTime>(m, "EDateTime")
        .def(py::init<>())
        .def(py::init<long, long, double, double, double, double>())
        .def(py::init<double, bool>(),py::arg("jd"), py::arg("is_tt") = false)
        // Above tested with python code:
        //othertime = EDateTime(2460449.5, True) 
        //print(othertime.string())  # 2024-05-18 23:58:51 UTC
        //othertime2 = EDateTime(2460449.5)  # default = False, i.e.UTC
        //print(othertime2.string())  # 2024-05-19 00:00:00 UTC
        .def(py::init<long>())
        .def("year", &EDateTime::year, "Returns the year of current date time.")
        .def("month", &EDateTime::month, "Returns the month of current date time.")
        .def("day", &EDateTime::day, "Returns the day of current date time.")
        .def("hour", &EDateTime::hour, "Returns the hour of current date time.")
        .def("minute", &EDateTime::minute, "Returns the minute of current date time.")
        .def("second", &EDateTime::second, "Returns the second of current date time.")
        .def("jd_tt", &EDateTime::jd_tt, "Returns the JD of current date time in TT (dynamical time).")
        .def("jd_utc", &EDateTime::jd_utc, "Returns the JD of current date time in (astrnomical) UTC.")
        .def("isLeap", &EDateTime::isLeap, "Returns True if current year is leap, false otherwise (year 1 BCE is year 0).")
        .def("weekday", &EDateTime::weekday, "Returns day of week; Sunday = 0, Monday = 1, etc.")
        .def("dayofyear", &EDateTime::dayofyear, "Returns the number of the day within the year")
        .def("string", &EDateTime::string, "Returns date time string in UTC with nearest whole second.")
        .def("stringms", &EDateTime::stringms, "Returns date time string in UTC with seconds given to 3 decimal places.")
        .def("unixTime", &EDateTime::unixTime, "Returns current date time in (integer) Unix timestamp format.")
        .def("setTime", &EDateTime::setTime,  "Sets current date time in integer year, month, float day, hour, minute, second.",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        .def("setTimeNow", &EDateTime::setTimeNow, "Sets EDateTime to current system date time in UTC."
        )
        .def("setJD_UTC", &EDateTime::setJD_UTC, "Sets current date time to JD in UTC.",
            py::arg("jd_utc")
        )
        .def("setJD_TT", &EDateTime::setJD_TT, "Sets current date time to JD in TT (dynamical time).",
            py::arg("jd_tt")
        )
        .def("setUnixTime", &EDateTime::setUnixTime, "Sets current date time to Unix timestamp.",
            py::arg("unixtime")
        )
        .def("addTime", &EDateTime::addTime, "Adds int year, month, float day, hour, minute, second to current date time.",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        // static void normalizeDateTime(long& yr, long& mo, double& da, double& hr, double& mi, double& se)
        // FIX: !!! The above modified parameters will not be passed back to Python I think !!!
        .def_static("myDivQuotient", &EDateTime::myDivQuotient, "This and myDivRemainder are required for some JD conversions.",
            py::arg("int1"), py::arg("int2")
        )
        .def_static("myDivRemiander", &EDateTime::myDivRemainder, "This and myDivQuotient are required for some JD conversions.",
            py::arg("int1"), py::arg("int2")
        )
        .def_static("getDateTime2JD_UTC", &EDateTime::getDateTime2JD_UTC, "Convert int year, month, float day, hour, minute, second to JD in UTC.",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        .def_static("getDateTime2JD_TT", &EDateTime::getDateTime2JD_TT, "Convert int year, month, float day, hour, minute, second to JD in TT (dynamical time).",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        .def_static("getUnixTime2JD_UTC", &EDateTime::getUnixTime2JD_UTC, "Convert Unix timestamp (in UTC) to JD in UTC.",
            py::arg("unixTime")
        )
        .def_static("getUnixTime2JD_TT", &EDateTime::getUnixTime2JD_TT, "Convert Unix timestamp (in UTC) to JD in TT (dynamical time).",
            py::arg("unixTime")
        )
        .def_static("getDateTime2UnixTime", &EDateTime::getDateTime2UnixTime, "Convert int year, month, float day, hour, minute, second to Unix timestamp in UTC.",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        .def_static("getJD_UTC2UnixTime", &EDateTime::getJD_UTC2UnixTime, "Convert a JD in UTC into a 32 bit Unix time stamp.",
            py::arg("jd_utc")
        )
        .def_static("getJD2MJD", &EDateTime::getJD2MJD, "Convert a JD (in TT or UTC) to Modified Julian Date (also in TT or UTC).",
            py::arg("jd")
        )
        .def_static("getMJD2JD", &EDateTime::getMJD2JD, "Convert a Modified Julian Date (in TT or UTC) to JD (also in TT or UTC).",
            py::arg("mjd")
        )
        .def_static("getJDUTC2TT", &EDateTime::getJDUTC2TT, "Convert a JD in UTC to JD in TT (dynamic time).",
            py::arg("jd_utc")
        )
        .def_static("getJDTT2UTC", &EDateTime::getJDTT2UTC, "Convert a JD in TT (dynamic time) to JD in UTC.",
            py::arg("jd_tt")
        )
        .def_static("calcUnixTimeYearDay", &EDateTime::calcUnixTimeYearDay, "Calculate Unix time of a date in UTC. Used internally in EDateTime.",
            py::arg("year"), py::arg("month"), py::arg("day")
        )
        .def_static("isLeapYear", &EDateTime::isLeapYear,
            "Returns true if provided year is a leap year, false otherwise. Note: year 1 BCE = year 0.",
            py::arg("year")
        )
        .def_static("DeltaT", &EDateTime::DeltaT, "Get the deltaT value for the given JD UTC using IERS table lookup.",
            py::arg("jd_utc")
        )
        .def_static("DeltaT_Polynomial", &EDateTime::DeltaT_Polynomial, "Get the deltaT value for the given JD UTC using NASA polynomials.",
            py::arg("jd_utc")
        )
        ;

    // -------------------
    //  astronomy library
    // -------------------
    // aconfig.h
    py::enum_<Planetary_Ephemeris>(m, "Planetary_Ephemeris")
        //.value("EPH_DEFAULT", Planetary_Ephemeris::EPH_DEFAULT)
        .value("EPH_VSOP87_SHORT", Planetary_Ephemeris::EPH_VSOP87_SHORT)
        .value("EPH_VSOP87_FULL", Planetary_Ephemeris::EPH_VSOP87_FULL)
        .export_values()
        ;
    py::enum_<Planet>(m, "Planet")
        .value("A_MERCURY", Planet::A_MERCURY)
        .value("A_VENUS", Planet::A_VENUS)
        .value("A_EARTH", Planet::A_EARTH)
        .value("A_MARS", Planet::A_MARS)
        .value("A_JUPITER", Planet::A_JUPITER)
        .value("A_SATURN", Planet::A_SATURN)
        .value("A_URANUS", Planet::A_URANUS)
        .value("A_NEPTUNE", Planet::A_NEPTUNE)
        .value("A_SUN", Planet::A_SUN)
        .value("A_EMB", Planet::A_EMB)
        .value("A_NONE", Planet::A_NONE)
        .export_values()
        ;
    // acoordinates.[h/cpp]
    py::class_<ACoord>(m, "ACoord")
        .def_static("rangezero2tau", &ACoord::rangezero2tau, "Snaps an angle to the range [0;2pi)",
            py::arg("angle")
        )
        .def_static("rangezero2threesixty", &ACoord::rangezero2threesixty, "Snaps an angle to the range [0;2pi)",
            py::arg("angle")
        )
        .def_static("rangezero2twentyfour", &ACoord::rangezero2twentyfour, "Snaps an angle to the range [0;2pi)",
            py::arg("angle")
        )
        .def_static("rangempi2pi", &ACoord::rangempi2pi, "Snaps an angle to the range [0;2pi)",
            py::arg("angle")
        )
        .def_static("rangemoneeighty2oneeighty", &ACoord::rangemoneeighty2oneeighty, "Snaps an angle to the range [0;2pi)",
            py::arg("angle")
        )
        .def_static("rangemhalfpi2halfpi", &ACoord::rangemhalfpi2halfpi, "Snaps an angle to the range [0;2pi)",
            py::arg("angle")
        )
        .def_static("rangemninety2ninety", &ACoord::rangemninety2ninety, "Snaps an angle to the range [0;2pi)",
            py::arg("angle")
        )
        .def_static("secs2deg", &ACoord::secs2deg, "Convert arcseconds to degrees (i.e. *3600).",
            py::arg("seconds")
        )
        .def_static("dms2rad", &ACoord::dms2rad, "Convert degrees, minutes, seconds to radians.",
            py::arg("d"), py::arg("m"), py::arg("s")
        )
        .def_static("dms2deg", &ACoord::dms2deg, "Convert degrees, minutes, seconds to decimal degrees.",
            py::arg("d"), py::arg("m"), py::arg("s")
        )
        .def_static("hms2rad", &ACoord::hms2rad, "Convert hours, minutes, seconds to radians.",
            py::arg("h"), py::arg("m"), py::arg("s")
        )
        .def_static("hms2deg", &ACoord::hms2deg, "Convert hours, minutes, seconds to decimal degrees.",
            py::arg("h"), py::arg("m"), py::arg("s")
        )
        .def_static("DistanceToLightTime", &ACoord::DistanceToLightTime, "Convert distance in AU to light time in days.",
            py::arg("Distance")
        )
        .def_static("angle2DMSstring", &ACoord::angle2DMSstringPython, "Angle (in radians) to degrees, minutes, seconds string.",
            py::arg("angle")
        )
        .def_static("angle2uDMSstring", &ACoord::angle2uDMSstringPython, "Angle (in radians) to unsigned degrees, minutes, seconds string.",
            py::arg("angle")
        )
        .def_static("angle2uHMSstring", &ACoord::angle2uHMSstring, "Angle (in radians) to unsigned hours, minutes, seconds string.",
            py::arg("angle")
        )
        .def_static("angle2DMstring", &ACoord::angle2DMstringPython, "Angle (in radians) to degrees, minutes string.",
            py::arg("angle")
        )
        .def_static("formatLatLon", &ACoord::formatLatLonPython, "Format coordinate to traditional Latitude Longitude string",
            py::arg("latlon")
        )
        //.def_static("formatLatLon2", &ACoord::formatLatLon2, "Format coordinate to traditional Latitude Longitude string",
        //    py::arg("lat"), py::arg("lon")
        //)
        .def_static("formatDecRA", &ACoord::formatDecRAPython, "Format coordinate to traditional Declination Right Ascension string",
            py::arg("decra")
        )
        //.def_static("formatDecRA2", &ACoord::formatDecRA2, "Format coordinate to traditional Declination Right Ascension string",
        //    py::arg("dec"), py::arg("ra")
        //)
        .def_static("formatEleAz", &ACoord::formatEleAzPython, "Format coordinate to traditional Elevation Azimuth string",
            py::arg("eleaz")
        )
        //.def_static("formatEleAz2", &ACoord::formatEleAz2, "Format coordinate to traditional Elevation Azimuth string",
        //    py::arg("ele"), py::arg("az")
        //)
        ;
    py::class_<AAngularSeparation>(m, "AAngularSeparation")
        .def_static("Separation", &AAngularSeparation::Separation, "Calculate angular separation between the two points.",
            py::arg("Alpha1"), py::arg("Delta1"), py::arg("Alpha2"), py::arg("Delta2")
        )
        .def_static("PositionAngle", &AAngularSeparation::PositionAngle, "Calculate the position angle of point 2 from point 1.",
            py::arg("Alpha1"), py::arg("Delta1"), py::arg("Alpha2"), py::arg("Delta2")
        )
        .def_static("DistanceFromGreatArc", &AAngularSeparation::DistanceFromGreatArc, "Calculate the distance point 3 from the great arc defined by points 1 and 2.",
            py::arg("Alpha1"), py::arg("Delta1"), py::arg("Alpha2"), py::arg("Delta2"), py::arg("Alpha3"), py::arg("Delta3")
        )
        .def_static("SmallestCircle", &AAngularSeparation::SmallestCircle, "Radius of smallest circle containing the 3 points. type1 is boolean indicating if it is type 1 or not.",
            py::arg("Alpha1"), py::arg("Delta1"), py::arg("Alpha2"), py::arg("Delta2"), py::arg("Alpha3"), py::arg("Delta3"), py::arg("type1")
        )
        ;
    py::class_<Spherical>(m, "Spherical")
        .def_static("Equatorial2Ecliptic", &Spherical::Equatorial2Ecliptic, "Convert Equatorial coordinates to Ecliptic, Epsilon is the True Obliquity of the Ecliptic.",
            py::arg("decra"), py::arg("Epsilon")
        )
        .def_static("Equatorial2Ecliptic2", &Spherical::Equatorial2Ecliptic2, "Convert Equatorial coordinates to Ecliptic, Epsilon is the True Obliquity of the Ecliptic.",
            py::arg("Alpha"), py::arg("Delta"), py::arg("Epsilon")
        )
        .def_static("Ecliptic2Equatorial", &Spherical::Ecliptic2Equatorial, "Convert Ecliptic coordinates to Equatorial, trueobliq is the True Obliquity of the Ecliptic.",
            py::arg("latlon"), py::arg("trueobliq")
        )
        .def_static("Ecliptic2Equatorial2", &Spherical::Equatorial2Ecliptic2, "Convert Ecliptic coordinates to Equatorial, Epsilon is the True Obliquity of the Ecliptic.",
            py::arg("Lambda"), py::arg("Beta"), py::arg("Epsilon")
        )
        .def_static("Equatorial2Horizontal", &Spherical::Equatorial2Horizontal, "Convert Equatorial coordinates to Horizontal, Latitude is the is the Observer Latitude.",
            py::arg("LocalHourAngle"), py::arg("Delta"), py::arg("Latitude")
        )
        .def_static("Horizontal2Equatorial", &Spherical::Horizontal2Equatorial, "Convert Horizontal coordinates to Equatorial, Latitude is the is the Observer Latitude.",
            py::arg("Azimuth"), py::arg("Altitude"), py::arg("Latitude")
        )
        .def_static("Equatorial2Galactic", &Spherical::Equatorial2Galactic, "Convert Equatorial coordinates to Galactic.",
            py::arg("Alpha"), py::arg("Delta")
        )
        .def_static("Galactic2Equatorial", &Spherical::Galactic2Equatorial, "Convert Galactic coordinates to Equatorial.",
            py::arg("l"), py::arg("b")
        )
        .def_static("Spherical2Rectangular", &Spherical::Spherical2Rectangular, "Convert Spherical coordinates to Rectangular (Cartesian).",
            py::arg("spherical")
        )
        .def_static("Rectangular2Spherical", &Spherical::Rectangular2Spherical, "Convert Rectangular (Cartesian) coordinates to Spherical.",
            py::arg("rectangular")
        )
        ;
    py::class_<FK5>(m, "FK5")
        .def_static("CorrectionInLongitude", &FK5::CorrectionInLongitude, "Calculate FK5 correction in longitude.",
            py::arg("lon"), py::arg("lat"), py::arg("jd_tt")
        )
        .def_static("CorrectionInLatitude", &FK5::CorrectionInLatitude, "Calculate FK5 correction in latitude.",
            py::arg("lon"), py::arg("jd_tt")
        )
        .def_static("CorrectionInLonLat", &FK5::CorrectionInLonLat, "Calculate FK5 correction.",
            py::arg("latlon"), py::arg("jd_tt")
        )
        .def_static("VSOP2FK5_J2000", &FK5::VSOP2FK5_J2000, "Transform from VSOP87 frame to FK5 frame at J2000.0.",
            py::arg("value")
        )
        .def_static("VSOP2FK5_B1950", &FK5::VSOP2FK5_B1950, "Transform from VSOP87 frame to FK5 frame at B1950.0.",
            py::arg("value")
        )
        .def_static("VSOP2FK5_AnyEquinox", &FK5::VSOP2FK5_AnyEquinox, "Transform from VSOP87 frame to FK5 frame at given equinox.",
            py::arg("value"), py::arg("JDEquinox")
        )
        ;
    py::class_<AIllumination>(m, "AIllumination")
        .def_static("PhaseAngle", &AIllumination::PhaseAngle, "Calculate illumination phase angle.",
            py::arg("r"), py::arg("R"), py::arg("Delta")
        )
        .def_static("PhaseAngle2", &AIllumination::PhaseAngle2, "Calculate illumination phase angle.",
            py::arg("R"), py::arg("R0"), py::arg("B"), py::arg("L"), py::arg("L0"), py::arg("Delta")
        )
        .def_static("PhaseAngleRectangular", &AIllumination::PhaseAngleRectangular, "Calculate illumination phase angle from rectangular coordinates.",
            py::arg("x"), py::arg("y"), py::arg("z"), py::arg("B"), py::arg("L"), py::arg("Delta")
        )
        .def_static("IlluminatedFraction", &AIllumination::IlluminatedFraction, "Calculate illuminated fraction from phase angle.",
            py::arg("PhaseAngle")
        )
        .def_static("IlluminatedFraction2", &AIllumination::IlluminatedFraction2, "Calculate illuminated fraction.",
            py::arg("r"), py::arg("R"), py::arg("Delta")
        )
        ;
    // aellipsoids.[h/cpp]
    py::class_<Ellipsoid_2axis>(m, "Ellipsoid_2axis")
        .def_readwrite("flattening", &Ellipsoid_2axis::flattening)
        .def_readwrite("semimajor_axis", &Ellipsoid_2axis::semimajor_axis)
        .def_readwrite("semiminor_axis", &Ellipsoid_2axis::semiminor_axis)
        .def_readwrite("eccentricity", &Ellipsoid_2axis::eccentricity)
        //.def(py::init<double, double, double, double>())
        .def(py::init<>())
        .def("fromSemimajorSemiminor", &Ellipsoid_2axis::fromSemimajorSemiminor, "Initialize from semimajor and semiminor axes.",
            py::arg("semimajor"), py::arg("semiminor")
        )
        .def("fromSemimajorFlattening", &Ellipsoid_2axis::fromSemimajorFlattening, "Initialize from semimajor axis and flattening.",
            py::arg("semimajor"), py::arg("flattening")
        )
        .def("fromSemimajorEccentricity", &Ellipsoid_2axis::fromSemimajorEccentricity, "Initialize from semimajor axis and  eccentricity.",
            py::arg("semimajor"), py::arg("eccentricity")
        )
        ;
    py::class_<AEllipsoid>(m, "AEllipsoid")
        .def_static("Planetocentric2Planetographic", &AEllipsoid::Planetocentric2Planetographic, "",
            py::arg("planetocentric"), py::arg("ellipsoid")
            )
        .def_static("Planetographic2Planetocentric", &AEllipsoid::Planetographic2Planetocentric, "",
            py::arg("planetographic"), py::arg("ellipsoid")
        )
        .def_static("RhoSinPhiPrime", &AEllipsoid::RhoSinPhiPrime, "",
            py::arg("GeographicalLatitude"), py::arg("Height"), py::arg("ellipsoid")
            )
        .def_static("RhoCosPhiPrime", &AEllipsoid::RhoCosPhiPrime, "",
            py::arg("GeographicalLatitude"), py::arg("Height"), py::arg("ellipsoid")
        )
        .def_static("RadiusOfParallelOfLatitude", &AEllipsoid::RadiusOfParallelOfLatitude, "Returns the radius of the parallel of latitude in same units as ellipsoid semimajor axis (km).",
            py::arg("GeographicalLatitude"), py::arg("ellipsoid")
        )
        .def_static("RadiusOfCurvature", &AEllipsoid::RadiusOfCurvature, "Returns the meridional (north-south) radius of curvature in same units as ellipsoid semimajor axis (km).",
            py::arg("GeographicalLatitude"), py::arg("ellipsoid")
        )
        .def_static("RadiusOfPrimeVertical", &AEllipsoid::RadiusOfPrimeVertical, "Returns the radius of curvature along the prime vertical (east-west) in same units as ellipsoid semimajor axis (km).",
            py::arg("GeographicalLatitude"), py::arg("ellipsoid")
        )
        .def_static("DistanceBetweenPoints", &AEllipsoid::DistanceBetweenPoints, "Returns ellipsoid distance between the given two points, in same units as ellipsoid semimajor axis (km).",
            py::arg("GeographicalLatitude1"),
            py::arg("GeographicalLongitude1"),
            py::arg("GeographicalLatitude2"),
            py::arg("GeographicalLongitude2"),
            py::arg("ellipsoid")
            )
        .def_readonly_static("Earth_IAU76", &AEllipsoid::Earth_IAU76)
        .def_readonly_static("Earth_WGS84", &AEllipsoid::Earth_WGS84)
        ;
    // asun.[h/cpp]
    py::class_<PhysicalSunDetails>(m, "PhysicalSunDetails")
        .def_readwrite("P", &PhysicalSunDetails::P)
        .def_readwrite("B0", &PhysicalSunDetails::B0)
        .def_readwrite("L0", &PhysicalSunDetails::L0)
        ;
    py::class_<ASun>(m, "ASun")
        // Position
        .def_static("GeometricEclipticLongitude", &ASun::GeometricEclipticLongitude, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("GeometricEclipticLatitude", &ASun::GeometricEclipticLatitude, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("GeometricEclipticLongitudeJ2000", &ASun::GeometricEclipticLongitudeJ2000, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("GeometricEclipticLatitudeJ2000", &ASun::GeometricEclipticLatitudeJ2000, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("GeometricFK5EclipticLongitude", &ASun::GeometricFK5EclipticLongitude, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("GeometricFK5EclipticLatitude", &ASun::GeometricFK5EclipticLatitude, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("ApparentEclipticLongitude", &ASun::GeometricEclipticLongitude, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("ApparentEclipticLatitude", &ASun::GeometricEclipticLatitude, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("VariationGeometricEclipticLongitude", &ASun::VariationGeometricEclipticLongitude, "",
            py::arg("jd_tt")
        )
        .def_static("EquatorialRectangularCoordinatesMeanEquinox", &ASun::EquatorialRectangularCoordinatesMeanEquinox, "",
            py::arg("jd_tt")
        )
        .def_static("EclipticRectangularCoordinatesJ2000", &ASun::EclipticRectangularCoordinatesJ2000, "",
            py::arg("jd_tt")
        )
        .def_static("EquatorialRectangularCoordinatesJ2000", &ASun::EquatorialRectangularCoordinatesJ2000, "",
            py::arg("jd_tt")
        )
        .def_static("EquatorialRectangularCoordinatesB1950", &ASun::EquatorialRectangularCoordinatesB1950, "",
            py::arg("jd_tt")
        )
        .def_static("EquatorialRectangularCoordinatesAnyEquinox", &ASun::EquatorialRectangularCoordinatesAnyEquinox, "",
            py::arg("jd_tt"), py::arg("JDEquinox")
        )
        .def_static("VSOP87_E_X", &ASun::VSOP87_E_X, "",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_E_Y", &ASun::VSOP87_E_Y, "",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_E_Z", &ASun::VSOP87_E_Z, "",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_E_dX", &ASun::VSOP87_E_dX, "",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_E_dY", &ASun::VSOP87_E_dY, "",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_E_dZ", &ASun::VSOP87_E_dZ, "",
            py::arg("jd_tt")
        )
        .def_static("SunSemidiameterA", &ASun::SunSemidiameterA, "",
            py::arg("Delta")
        )
        .def_static("CalculatePhysicalSun", &ASun::CalculatePhysicalSun, "",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("TimeOfStartOfRotation", &ASun::TimeOfStartOfRotation, "",
            py::arg("C")
        )
        ;
    // aearth.[h/cpp]
    py::class_<AEarth>(m, "AEarth")
        // Position
        .def_static("EclipticCoordinates", &AEarth::EclipticCoordinates, "Heliocentric Ecliptic Spherical coordinates referred to equinox and ecliptic of date.",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("EclipticCoordinatesJ2000", &AEarth::EclipticCoordinatesJ2000, "Heliocentric Ecliptic Spherical coordinates referred to equinox and ecliptic of J2000.0.",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("EclipticLongitude", &AEarth::EclipticLongitude, "Heliocentric Ecliptic Longitude (radians) referred to equinox and ecliptic of date.",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("EclipticLatitude", &AEarth::EclipticLatitude, "Heliocentric Ecliptic Latitude (radians) referred to equinox and ecliptic of date.",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("EclipticDistance", &AEarth::EclipticDistance, "Heliocentric Ecliptic Spherical Distance (VSOP87 AUs). Equally valid with the J2000 Latitude and Longitude.",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("EclipticLongitudeJ2000", &AEarth::EclipticLongitudeJ2000, "Heliocentric Ecliptic Longitude (radians) referred to equinox and ecliptic of J2000.0.",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("EclipticLatitudeJ2000", &AEarth::EclipticLatitudeJ2000, "Heliocentric Ecliptic Latitude (radians) referred to equinox and ecliptic of J2000.0.",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("SunMeanAnomaly", &AEarth::SunMeanAnomaly, "Sun's Mean Anomaly, used in many other calculations.",
            py::arg("jd_tt")
        )
        .def_static("Eccentricity", &AEarth::Eccentricity, "Eccentricity of Earth's orbit around the Sun.",
            py::arg("jd_tt")
        )
        // Precession
        .def_static("PrecessEquatorialJ2000", &AEarth::PrecessEquatorialJ2000, "Precess Geocentric Equatorial Spherical coordinates from J2000.0 to given moment.",
            py::arg("decra"), py::arg("jd_tt")
        )
        .def_static("PrecessEquatorialFK5", &AEarth::PrecessEquatorialFK5, "Precess Geocentric Equatorial Spherical coordinates from given JD0 to given moment.",
            py::arg("decra"), py::arg("JD0"), py::arg("jd_tt")
        )
        // NOTE: This one is not useful without additional calculations, see MEEUS98 chapter 21 & 23 on FK4 coordinates and aberration
        //       P.J.Naugher is incorrect when stating that this returns FK5 coordinates. E-terms must be dropped in aberration, and equinox correction must be applied
        //.def_static("PrecessEquatorialFK4", &AEarth::PrecessEquatorialFK4, "Precess from JD0 in FK4 to .",
        //    py::arg("decra"), py::arg("JD0"), py::arg("jd_tt")
        //)
        .def_static("PrecessEcliptic", &AEarth::PrecessEcliptic, "Precess Geocentric Ecliptic Spherical coordinates from given JD0 to given moment.",
            py::arg("Lambda"), py::arg("Beta"), py::arg("JD0"), py::arg("jd_tt")
        )
        // Obliquity
        .def_static("MeanObliquityOfEcliptic", &AEarth::MeanObliquityOfEcliptic, "The Mean Obliquity of the Ecliptic, i.e. the tilt angle of Earth's axis.",
            py::arg("jd_tt")
        )
        .def_static("TrueObliquityOfEcliptic", &AEarth::TrueObliquityOfEcliptic, "The True Obliquity of the Ecliptic, i.e. the tilt angle of Earth's axis.",
            py::arg("jd_tt")
        )
        .def_static("PrecessionVondrak_epsilon", &AEarth::PrecessionVondrak_epsilon, "Obliquity long term validity.",
            py::arg("jd_tt")
        )
        // Nutation
        .def_static("EclipticNutation", &AEarth::EclipticNutation, "Nutation in Geocentric Ecliptic Spherical coordinates at given moment.",
            py::arg("jd_tt")
        )
        .def_static("NutationInLongitude", &AEarth::NutationInLongitude, "Nutation in Geocenric Ecliptic Longitude (radians) at given moment.",
            py::arg("jd_tt")
        )
        .def_static("NutationInObliquity", &AEarth::NutationInObliquity, "Nutation in Geocenric Ecliptic Obliquity (radians) at given moment.",
            py::arg("jd_tt")
        )
        .def_static("EquatorialNutation", &AEarth::EquatorialNutation, "Apply the given Geocentric Ecliptic Spherical Nutation to the Geocentric Equatorial Spherical coordinates.",
            py::arg("decra"), py::arg("obliq"), py::arg("ecnutation")
        )
        .def_static("NutationInRightAscension", &AEarth::NutationInRightAscension, "Get the Nutation in RA from the Ecliptic Nutation for the given Equatorial coordinates.",
            py::arg("dec"), py::arg("ra"), py::arg("obliq"), py::arg("nut_lon"), py::arg("nut_obl")
        )
        .def_static("NutationInDeclination", &AEarth::NutationInDeclination, "Get the Nutation in Dec from the Ecliptic Obliquity and Nutation for the given Equatorial RA.",
            py::arg("ra"), py::arg("obliq"), py::arg("nut_lon"), py::arg("nut_obl")
        )
        // Time
        .def_static("EquationOfTime", &AEarth::EquationOfTime, " Calculate Equation of Time in Minutes.",
            py::arg("jd_tt"), py::arg("eph")
        )
        .def_static("MeanGreenwichSiderealTime", &AEarth::MeanGreenwichSiderealTime, "Calculate Mean Greenwich Sidereal Time.",
            py::arg("jd_utc")
        )
        .def_static("ApparentGreenwichSiderealTime", &AEarth::ApparentGreenwichSiderealTime, "Calculate Mean Greenwich Sidereal Time.",
            py::arg("jd_utc")
        )
        // Aberration
        .def_static("EarthVelocity", &AEarth::EarthVelocity, "Calculate the instantaneous velocity of Earth in rectangular coordinates.",
            py::arg("jd_tt"), py::arg("eph")
        )
        .def_static("EquatorialAberration", &AEarth::EquatorialAberration, "Calculate Aberration in Equatorial coordinates.",
            py::arg("Alpha"), py::arg("Delta"), py::arg("jd_tt"), py::arg("eph")
        )
        .def_static("EclipticAberration", &AEarth::EclipticAberration, "Calculate Aberration in Ecliptic coordinates.",
            py::arg("Lambda"), py::arg("Beta"), py::arg("jd_tt"), py::arg("eph")
        )
        //.def_static("Vondrak_Ron_Aberration", &AEarth::Vondrak_Ron_Aberration, "Calculate Vondrak Ron aberration.",
        //    py::arg("jd_tt")
        //)
        // Atmospheric Refraction
        .def_static("RefractionFromApparent", &AEarth::RefractionFromApparent, "Use Bennet's refraction formula to refract a calculated coordinate.",
            py::arg("Altitude"), py::arg("Pressure") = 1010, py::arg("Temperature") = 10
        )
        .def_static("RefractionFromTrue", &AEarth::RefractionFromTrue, "Use Saemundsson's refraction formula to refract an observed coordinate.",
            py::arg("Altitude"), py::arg("Pressure") = 1010, py::arg("Temperature") = 10
        )
        // Figure of Earth
        .def_static("RhoSinPhiPrime", &AEarth::RhoSinPhiPrime, "Calculate parallax parameter.",
            py::arg("GeographicalLatitude"), py::arg("Height")
        )
        .def_static("RhoCosPhiPrime", &AEarth::RhoCosPhiPrime, "Calculate parallax parameter.",
            py::arg("GeographicalLatitude"), py::arg("Height")
        )
        .def_static("RadiusOfParallelOfLatitude", &AEarth::RadiusOfParallelOfLatitude, "Calculate the radius (in kilometers) of a Parallel of Latitude.",
            py::arg("GeographicalLatitude")
        )
        .def_static("RadiusOfCurvature", &AEarth::RadiusOfCurvature, "Calculate the radius of curvatute (in kilometers) at the given Latitude.",
            py::arg("GeographicalLatitude")
        )
        .def_static("DistanceBetweenPoints", &AEarth::DistanceBetweenPoints, "Calculate the distance between two points on Earth.",
            py::arg("GeographicalLatitude1"), py::arg("GeographicalLongitude1"), py::arg("GeographicalLatitude2"), py::arg("GeographicalLongitude2")
        )
        // Parallax
        .def_static("DistanceToParallax", &AEarth::DistanceToParallax, "Convert Distance in AU to Parallax in radians.",
            py::arg("Distance")
        )
        .def_static("ParallaxToDistance", &AEarth::ParallaxToDistance, "Convert Parallax in radians to Distance in AU.",
            py::arg("Parallax")
        )
        .def_static("Equatorial2Topocentric", &AEarth::Equatorial2Topocentric, "Calculate Geocentric Equatorial to Topocentric Equatorial, agst is Apparent Greenwich Sidereal Time.",
            py::arg("decradst"), py::arg("latlonhgt"), py::arg("agst")
        )
        .def_static("Equatorial2Topocentric2", &AEarth::Equatorial2Topocentric2, "Calculate Geocentric Equatorial to Topocentric Equatorial.",
            py::arg("Alpha"), py::arg("Delta"), py::arg("Distance"), py::arg("Longitude"), py::arg("Latitude"), py::arg("Height"), py::arg("JD")
        )
        .def_static("Equatorial2TopocentricDelta", &AEarth::Equatorial2TopocentricDelta, "Calculate Geocentric Equatorial to Topocentric Equatorial Offset/Difference, agst is Apparent Greenwich Sidereal Time.",
            py::arg("Alpha"), py::arg("Delta"), py::arg("Distance"), py::arg("Longitude"), py::arg("Latitude"), py::arg("Height"), py::arg("JD")
        )
        .def_static("Ecliptic2Topocentric", &AEarth::Ecliptic2Topocentric, "Calculate Geocentric Ecliptic coordinates to Topocentric Equatorial,Epsilon is Obliquity of Ecliptic and agst is Apparent Greenwich Sidereal Time.",
            py::arg("latlondst"), py::arg("Semidiameter"), py::arg("Latitude"), py::arg("Height"), py::arg("Epsilon"), py::arg("agst")
        )
        .def_static("Ecliptic2Topocentric2", &AEarth::Ecliptic2Topocentric2, "Calculate Geocentric Ecliptic coordinates to Topocentric Equatorial, Epsilon is the Obliquity of the Ecliptic.",
           py::arg("Lambda"), py::arg("Beta"), py::arg("Semidiameter"), py::arg("Distance"), py::arg("Epsilon"), py::arg("Latitude"), py::arg("Height"), py::arg("JD")
        )
        // Equinoxes and Solstices
        .def_static("NorthwardEquinox", &AEarth::NorthwardEquinox, "Northward (vernal) Equinox of given year.",
            py::arg("year"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("NorthernSolstice", &AEarth::NorthernSolstice, "Northern (summer) Solstice of given year.",
            py::arg("year"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("SouthwardEquinox", &AEarth::SouthwardEquinox, "Southward (autumnal) Equinox of given year.",
            py::arg("year"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("SouthernSolstice", &AEarth::SouthernSolstice, "Southern (winter) Solstice of given year.",
            py::arg("year"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        .def_static("TropicalYearLength", &AEarth::TropicalYearLength, "Tropical year length in ephemeris days.",
            py::arg("jd_tt")
        )
        // VSOP87 - Orbital Parameters (NOTE: these are cooked, to get true Keplerian parameters, see aorbits.[cpp/h]
        .def_static("VSOP87_A", &AEarth::VSOP87_A, "VSOP87 Orbital Parameter A.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_L", &AEarth::VSOP87_L, "VSOP87 Orbital Parameter L.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_K", &AEarth::VSOP87_K, "VSOP87 Orbital Parameter K.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_H", &AEarth::VSOP87_H, "VSOP87 Orbital Parameter H.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_Q", &AEarth::VSOP87_Q, "VSOP87 Orbital Parameter Q.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87_P", &AEarth::VSOP87_P, "VSOP87 Orbital Parameter P.",
            py::arg("jd_tt")
        )
        // Ephemeris A - Rectangular Heliocentric Ecliptic at Equinox of J2000.0
        .def_static("VSOP87A_X", &AEarth::VSOP87A_X, "VSOP87 A Rectangular Heliocentric Ecliptic X coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87A_Y", &AEarth::VSOP87A_Y, "VSOP87 A Rectangular Heliocentric Ecliptic Y coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87A_Z", &AEarth::VSOP87A_Z, "VSOP87 A Rectangular Heliocentric Ecliptic Z coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87A_dX", &AEarth::VSOP87A_dX, "VSOP87 A Rectangular Heliocentric Ecliptic X velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87A_dY", &AEarth::VSOP87A_dY, "VSOP87 A Rectangular Heliocentric Ecliptic Y velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87A_dZ", &AEarth::VSOP87A_dZ, "VSOP87 A Rectangular Heliocentric Ecliptic Z velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        // Ephemeris B - Spherical Heliocentric Ecliptic at J2000.0
        .def_static("VSOP87B_Longitude", &AEarth::VSOP87B_Longitude, "VSOP87 B Spherical Heliocentric Ecliptic Longitude coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87B_Latitude", &AEarth::VSOP87B_Latitude, "VSOP87 B Spherical Heliocentric Ecliptic Latitude coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87B_Distance", &AEarth::VSOP87B_Distance, "VSOP87 B Spherical Heliocentric Ecliptic Distance coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87B_dLongitude", &AEarth::VSOP87B_dLongitude, "VSOP87 B Spherical Heliocentric Ecliptic Longitude velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87B_dLatitude", &AEarth::VSOP87B_dLatitude, "VSOP87 B Spherical Heliocentric Ecliptic Latitude velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87B_dDistance", &AEarth::VSOP87B_dDistance, "VSOP87 B Spherical Heliocentric Ecliptic Distance velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        // Ephemeris C - Rectangular Heliocentric Ecliptic at Equinox of Date
        .def_static("VSOP87C_X", &AEarth::VSOP87C_X, "VSOP87 C Rectangular Heliocentric Ecliptic X coordinate at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87C_Y", &AEarth::VSOP87C_Y, "VSOP87 C Rectangular Heliocentric Ecliptic Y coordinate at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87C_Z", &AEarth::VSOP87C_Z, "VSOP87 C Rectangular Heliocentric Ecliptic Z coordinate at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87C_dX", &AEarth::VSOP87C_dX, "VSOP87 C Rectangular Heliocentric Ecliptic X velocity at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87C_dY", &AEarth::VSOP87C_dY, "VSOP87 C Rectangular Heliocentric Ecliptic Y velocity at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87C_dZ", &AEarth::VSOP87C_dZ, "VSOP87 C Rectangular Heliocentric Ecliptic Z velocity at Equinox of date.",
            py::arg("jd_tt")
        )
        // Ephemeris D - Spherical Heliocentric Ecliptic at Equinox of Date
        .def_static("VSOP87D_Longitude", &AEarth::VSOP87D_Longitude, "VSOP87 D Spherical Heliocentric Ecliptic Longitude coordinate at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87D_Latitude", &AEarth::VSOP87D_Latitude, "VSOP87 D Spherical Heliocentric Ecliptic Latitude coordinate at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87D_Distance", &AEarth::VSOP87D_Distance, "VSOP87 D Spherical Heliocentric Ecliptic Distance coordinate at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87D_dLongitude", &AEarth::VSOP87D_dLongitude, "VSOP87 D Spherical Heliocentric Ecliptic Longitude velocity at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87D_dLatitude", &AEarth::VSOP87D_dLatitude, "VSOP87 D Spherical Heliocentric Ecliptic Latitude velocity at Equinox of date.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87D_dDistance", &AEarth::VSOP87D_dDistance, "VSOP87 D Spherical Heliocentric Ecliptic Distance velocity at Equinox of date.",
            py::arg("jd_tt")
        )
        // Ephemeris E - Rectangular Barycentric Ecliptic at Equinox of J2000.0
        .def_static("VSOP87E_X", &AEarth::VSOP87E_X, "VSOP87 E Rectangular Barycentric Ecliptic X coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87E_Y", &AEarth::VSOP87E_Y, "VSOP87 E Rectangular Barycentric Ecliptic Y coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87E_Z", &AEarth::VSOP87E_Z, "VSOP87 E Rectangular Barycentric Ecliptic Z coordinate at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87E_dX", &AEarth::VSOP87E_dX, "VSOP87 E Rectangular Barycentric Ecliptic X velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87E_dY", &AEarth::VSOP87E_dY, "VSOP87 E Rectangular Barycentric Ecliptic Y velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        .def_static("VSOP87E_dZ", &AEarth::VSOP87E_dZ, "VSOP87 E Rectangular Barycentric Ecliptic Z velocity at Equinox of J2000.0.",
            py::arg("jd_tt")
        )
        ;
    // amoon.h
    py::enum_<Lunar_Ephemeris>(m, "Lunar_Ephemeris")
        .value("MEEUS_SHORT", Lunar_Ephemeris::MEEUS_SHORT)
        .value("ELP2000_82", Lunar_Ephemeris::ELP2000_82)
        .value("ELP_MPP02", Lunar_Ephemeris::ELP_MPP02)
        .export_values()
        ;
    py::enum_<Lunar_Phase>(m, "Lunar_Phase")
        .value("NEW_MOON", Lunar_Phase::NEW_MOON)
        .value("FIRST_QUARTER_MOON", Lunar_Phase::FIRST_QUARTER_MOON)
        .value("FULL_MOON", Lunar_Phase::FULL_MOON)
        .value("THIRD_QUARTER_MOON", Lunar_Phase::THIRD_QUARTER_MOON)
        .export_values()
        ;
    py::enum_<ELPMPP02_Correction>(m, "ELPMPP02_Correction")
        .value("ELPMPP02_Nominal", ELPMPP02_Correction::ELPMPP02_Nominal)
        .value("ELPMPP02_LLR", ELPMPP02_Correction::ELPMPP02_LLR)
        .value("ELPMPP02_DE405", ELPMPP02_Correction::ELPMPP02_DE405)
        .value("ELPMPP02_DE406", ELPMPP02_Correction::ELPMPP02_DE406)
        .export_values()
        ;
    py::class_<APhysicalMoonDetails>(m, "APhysicalMoonDetails")
        .def_readwrite("ldash", &APhysicalMoonDetails::ldash)    // The optical libration in longitude in radians
        .def_readwrite("bdash", &APhysicalMoonDetails::bdash)    // The optical libration in latitude in radians
        .def_readwrite("ldash2", &APhysicalMoonDetails::ldash2)  // The physical libration in longitude in radians
        .def_readwrite("bdash2", &APhysicalMoonDetails::bdash2)  // The physical libration in latitude in radians
        .def_readwrite("l", &APhysicalMoonDetails::l)            // The total libration in longitude in radians
        .def_readwrite("b", &APhysicalMoonDetails::b)            // The total libration in latitude in radians
        .def_readwrite("P", &APhysicalMoonDetails::P)            // The position angle in radians of the Moon's axis of rotation
        ;
    py::class_< ASelenographicMoonDetails>(m, "ASelenographicMoonDetails")
        .def_readwrite("l0", &ASelenographicMoonDetails::l0)
        .def_readwrite("b0", &ASelenographicMoonDetails::b0)
        .def_readwrite("c0", &ASelenographicMoonDetails::c0)
        ;
    py::class_<AMoon>(m, "AMoon")
        .def_static("EclipticCoordinates", &AMoon::EclipticCoordinates, "Geocentric Ecliptic Moon coordinates",
            py::arg("jd_tt"), py::arg("eph") = Lunar_Ephemeris::ELP2000_82
        )
        .def_static("RadiusVectorToHorizontalParallax", &AMoon::RadiusVectorToHorizontalParallax, "Takes distance in km, returns parallax in radians",
            py::arg("distance")
        )
        .def_static("HorizontalParallaxToRadiusVector", &AMoon::HorizontalParallaxToRadiusVector, "Takes parallax in radians, returns distance in km",
            py::arg("parallax")
        )
        // !!! FIX: Moon Illuminated Fraction functions go here, unless they are moved to a more generic location (they work for planets too?)
        .def_static("Phase_K", &AMoon::Phase_K, "Takes decimal year, returns the K value used to calculate the mean Moon phase",
            py::arg("year")
        )
        .def_static("MeanPhase", &AMoon::MeanPhase, "Takes K value from AMoon.Phase_K(), returns the mean phase of the Moon",
            py::arg("K")
        )
        .def_static("TruePhase", &AMoon::TruePhase, "Takes jd_tt and desired quarter phase, returns nearest following jd_tt of the speficied Moon phase",
            py::arg("jd_tt"), py::arg("phase")
        )
        .def_static("TruePhaseK", &AMoon::TruePhaseK, "Takes K from AMoon.Phase_K(), returns nearest following jd_tt of the speficied Moon phase",
            py::arg("K")
        )
        // !!! FIX: Lunar Node calculations should be amended to ensure K value is always half integer before exporting to python, see phase calculations
        //.def_static("Node_K", &AMoon::Node_K, "Takes decimal year, returns K value for node passage calculation",
        //    py::arg("year")
        //)
        .def_static("CalculateGeocentric", &AMoon::CalculateGeocentric, "Geocentric lunar optical, physical, total libration and axis angle",
            py::arg("jd_tt")
        )
        .def_static("CalculateTopocentric", &AMoon::CalculateTopocentric, "Topocentric lunar optical, physical, total libration and axis angle",
            py::arg("jd_tt"), py::arg("Longitude"), py::arg("Latitude")
        )
        .def_static("CalculateSelenographicPositionOfSun", &AMoon::CalculateSelenographicPositionOfSun, "Selenograpic position of Sun",
            py::arg("jd_tt"), py::arg("eph")
        )
        .def_static("AltitudeOfSun", &AMoon::AltitudeOfSun, "Altitude of Sun at given location on Moon",
            py::arg("jd_tt"), py::arg("Longitude"), py::arg("Latitude"), py::arg("eph")
        )
        .def_static("TimeOfSunrise", &AMoon::TimeOfSunrise, "Time of next sunrise at given location on Moon",
            py::arg("jd_tt"), py::arg("Longitude"), py::arg("Latitude"), py::arg("eph")
        )
        .def_static("TimeOfSunset", &AMoon::TimeOfSunset, "Time of next sunset at given location on Moon",
            py::arg("jd_tt"), py::arg("Longitude"), py::arg("Latitude"), py::arg("eph")
        )
        .def_static("GeocentricMoonSemidiameter", &AMoon::GeocentricMoonSemidiameter, "takes geocentric distance in km, returns Moon demidiameter in radians",
            py::arg("distance")
        )
        .def_static("TopocentricMoonSemidiameter", &AMoon::TopocentricMoonSemidiameter, "Returns topocentric Moon semidiameter in radians",
            py::arg("DistanceDelta"), py::arg("Delta"), py::arg("H"), py::arg("Latitude"), py::arg("Height")
        )
        ;
    py::class_<AVenus>(m, "AVenus")
        .def_static("EclipticLongitude", &AVenus::EclipticLongitude, "True Ecliptic Longitude in radians.",
            py::arg("jd_tt"), py::arg("eph")
        )
        .def_static("EclipticLatitude", &AVenus::EclipticLatitude, "True Ecliptic Latitude in radians.",
            py::arg("jd_tt"), py::arg("eph")
        )
        .def_static("EclipticDistance", &AVenus::EclipticDistance, "True Ecliptic Distance in VSOP87 AU.",
            py::arg("jd_tt"), py::arg("eph")
        )
        .def_static("EclipticCoordinates", &AVenus::EclipticCoordinates, "True Ecliptic coordinates (in radians and VSOP87 AU).",
            py::arg("jd_tt"), py::arg("eph") = Planetary_Ephemeris::EPH_VSOP87_FULL
        )
        ;
    // aelliptical.h
    py::class_<APlanetaryDetails>(m, "APlanetaryDetails")
        .def_readwrite("thecs", &APlanetaryDetails::thecs)  // LLD True Heliocentric ECliptical Spherical coordinates
        .def_readwrite("tgecs", &APlanetaryDetails::tgecs)  // LLD True Geocentric ECliptical Spherical coordinates
        .def_readwrite("tgecr", &APlanetaryDetails::tgecr)  // glm::dvec3 True Geocentric ECliptical Rectangular coordinates
        .def_readwrite("tgeqs", &APlanetaryDetails::tgeqs)  // LLD True Geocentric EQuatorial Spherical coordinates
        .def_readwrite("agecs", &APlanetaryDetails::agecs)  // LLD Apparent Geocentric EClipticalSpherical coordinates
        .def_readwrite("ageqs", &APlanetaryDetails::ageqs)  // LLD Apparent Geocentric EQuatorial Spherical coordinates
        .def_readwrite("tlt", &APlanetaryDetails::tlt)      // double True Light Time
        .def_readwrite("alt", &APlanetaryDetails::alt)      // double Apparent Light Time
        .def_readwrite("jd_tt", &APlanetaryDetails::jd_tt)  // double JD in Terrestrial Time when the position was calculated
        ;
    py::class_<AElliptical>(m, "AElliptical")
        .def_static("calcKepler", &AElliptical::calcKepler, "Calculate Eccentric Anomaly, from M = Mean anomaly, e = eccentricity",
            py::arg("M"), py::arg("e"), py::arg("nIterations")
        )
        .def_static("getPlanetaryDetails", &AElliptical::getPlanetaryDetails, "Calculate True and Apparent positions of given planet at given time. Returns an APlanetaryDetails object.",
            py::arg("jd_tt"), py::arg("planet"), py::arg("eph")
        )
        .def_static("EclipticCoordinates", &AElliptical::EclipticCoordinates, "Calculate Geometric Heliocentric Ecliptic Spherical coordinates of given planet at epoch of ephemeris.",
            py::arg("jd_tt"), py::arg("planet"), py::arg("eph")
        )
        .def_static("SemiMajorAxisFromPerihelionDistance", &AElliptical::SemiMajorAxisFromPerihelionDistance, "Calculates the semimajor axis from the perihelion distance (q) and the eccentricity (e).",
            py::arg("q"), py::arg("e")
        )
        .def_static("MeanMotionFromSemiMajorAxis", &AElliptical::MeanMotionFromSemiMajorAxis, "Calculates the Mean motion from the semimajor axis for an object in elliptical orbit around the Sun.",
            py::arg("a")
        )
        .def_static("InstantaneousVelocity", AElliptical::InstantaneousVelocity, "Calculate the instantaeous velocity from the solar distance (AU) and the semimajor axis.",
            py::arg("r"), py::arg("a")
        )
        .def_static("VelocityAtPerihelion", AElliptical::VelocityAtPerihelion, "Calculate the instantaneous velocity at perihelion from eccentricity and semimajor axis.",
            py::arg("e"), py::arg("a")
        )
        .def_static("VelocityAtAphelion", AElliptical::VelocityAtPerihelion, "Calculate the instantaneous velocity at aphelion from eccentricity and semimajor axis.",
            py::arg("e"), py::arg("a")
        )
        .def_static("LengthOfEllipse", AElliptical::LengthOfEllipse, "Calculate the length of the ellipse from eccentricity and semimajor axis.",
            py::arg("e"), py::arg("a")
        )
        .def_static("CometMagnitude", &AElliptical::CometMagnitude, "Calculate apparent magnitude from g=absolute magnitude of the comet, delta=comet to Earth distance (AU), k=comet specific constant, r=comet to Sun distance (AU).",
            py::arg("g"), py::arg("delta"), py::arg("k"), py::arg("r")
        )
        .def_static("MinorPlanetMagnitude", &AElliptical::MinorPlanetMagnitude, "H=mean absolute visual magnitude of minor planet, delta=minor planet to Earth distance (AU), G=slope parameter specific to minor planet, rminor planet to Sun distance (AU), PhaseAngle=Sun-body-Earth angle (radians).",
            py::arg("H"), py::arg("delta"), py::arg("G"), py::arg("r"), py::arg("PhaseAngle")
        )
        ;
        m.attr("__version__") = "dev2";

};

