
#include <chrono>  // For comparing execution times of different functions.

#include <glm/gtx/string_cast.hpp>

// These are used by various scenarios, because stuff is missing from Astronomy
//#include "AAplus/AAAberration.h"
//#include "AAplus/AAMoon.h"
//#include "AAplus/AANutation.h"
//#include "AAplus/AAPhysicalMoon.h"
//#include "AAplus/AAPrecession.h"   // !!! FIX: Solver evaluations use this, convert those to use Astronomy !!!
//#include "AAplus/AARefraction.h"   // refraction is calculated in a dynamic function in Earth, refactor that
//
//#include "AAplus/AARiseTransitSet2.h"
//#include "AAplus/AAMoonPhases2.h"

// For the Python interface. Split into separate file if possible.
#include <python310/Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
namespace py = pybind11;

// Protos
void GLClearError();
void GLPrintError();

//#include "OpenGL.h"
#include "Primitives.h"
#include "Earth.h"
#include "Astronomy.h"
//#include "Utilities.h"
#include "astronomy/acoordinates.h"
#include "astronomy/aearth.h"
#include "FigureGeometry.h"

// -----------
//  Locations
// -----------
// Just a few handy locations. Eventually a location manager should load from a file and offer interactive maintenance !!!
LLD l_gnw = { 51.477811, 0.001475, 0.0 };      // Greenwich Royal Observatory, UK
LLD l_qqt = { 69.243574, -53.540529, 0.0 };    // Quequertarsuaq, Greenland
LLD l_cph = { 55.6761, 12.5683, 0.0 };         // Copenhagen, Denmark 55.6761° N, 12.5683° E
LLD l_nyc = { 40.7182, -74.0060, 0.0 };        // New York, USA 40.7128° N, 74.0060° W
LLD l_tok = { 35.668009, 139.773577, 0.0 };    // Tokyo, Japan 35.668009, 139.773577
LLD l_sye = { 24.0889, 32.8998, 0.0 };         // Syene, Egypt 24.0889° N, 32.8998° E
LLD l_alx = { 31.2001, 29.9187, 0.0 };         // Alexandria, Egypt 31.2001° N, 29.9187° E
LLD l_clm = { 6.9271, 79.8612, 0.0 };          // Colombo, Sri Lanka 6.9271° N, 79.8612° E
LLD l_rig = { -53.822025, -67.651686, 0.0 };   // Rio Grande, Argentina - 53.822025, -67.651686
LLD l_ams = { 52.3676, 4.9041, 0.0 };          // Amsterdam, The Netherlands 52.3676° N, 4.9041° E
LLD l_perth = { -31.942045, 115.963123, 0.0 }; // Perth airport(PER) Australia
LLD l_sydney{ -33.8688, 151.2093, 66.0 };      // Sydney Australia according to google maps (incl elevation)
LLD l_cptwn = { -33.974195, 18.602728, 0.0 };  // Cape Town airport South Africa
LLD l_puare = { -53.002324, -70.854556, 0.0 }; // Punta Arenas airport(PUQ) Chile
LLD l_stiag = { -33.3898, -70.7944, 0.0 };     // Santiago airport(SCL) Chile
LLD l_sydny = { -33.9399, 151.1753, 0.0 };     // Sydney airport(SYD) Australia
LLD l_buair = { -34.818463, -58.534176, 0.0 }; // Buenos Aires airport(EZE) Argentina
LLD l_frafu = { 50.037967, 8.561057, 0.0 };    // Frankfurt airport(FRA) Germany
LLD l_seoul = { 37.468028, 126.442011, 0.0 };  // Seoul airport(ICN) South Korea
LLD l_kabul = { 34.543447, 69.177123, 1817.522 };  // Kabul FlatEarthIntel.com measurepoint
LLD l_jamaica = { 18.1096, -77.2975, 0.0 };    // Jamaica according to google. For Columbus Eclipse

// Places to check webcams for sunrise/sunset
LLD l_phuket = { 7.891579, 98.295930, 0.0 };   // Phuket west facing (sunset) webcam: https://www.webcamtaxi.com/en/thailand/phuket/patong-beach-resort.html


// ---------------
//  Solar Eclipse
// ---------------
// Currently unused obviously. Here as a reminder to implement such a class
class SolarEclipse {
    // This is the stuff I'd love to make: https://www.youtube.com/watch?v=IaBt5Zau3ng&ab_channel=NASAScientificVisualizationStudio
    // Along with showing where the Sun and Moon are in relation to Earth, with shadow cones for umbra, penumbra and antumbra
public:
};







void TimeGetDetails(Application& app) {
    Astronomy* astro = app.newAstronomy();
    astro->setTime(-200, 9, 23.0, 0.0, 0.0, 0.0);
    //astro->setTimeNow();

    auto start = std::chrono::high_resolution_clock::now();
    auto stop = std::chrono::high_resolution_clock::now();
    CelestialDetail old;
    double count = 1000; // 100'000;
    double i = 0;
    start = std::chrono::high_resolution_clock::now();
    old = astro->getDetailsNew(astro->getJD_TT(), URANUS, ECGEO);
    //return;
    for (i = 0; i < count; i++) {
        old = astro->getDetailsNew(astro->getJD_TT(), URANUS, ECGEO);
    }
    stop = std::chrono::high_resolution_clock::now();
    std::cout << " Time in us:  " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << '\n';
    std::cout << "RA/Dec: " << rad2hrs * old.geq.lon << ", " << rad2deg * old.geq.lat << "\n";
}

#include "AAplus/AAElliptical.h"
void testPlanetaryDetail(Application& app) {

    // Mars - 2024-04-08 12:00 UTC

    // My library (converted from AA+ to always use radians)
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2024, 4, 8.0, 12.0, 0.0, 0.0);
    std::cout << "\n\nMARS " << astro->timestr << " - MDO Library" << std::endl;
    CelestialDetailFull details = astro->planetaryDetails(astro->getJD_TT(), A_MARS, VSOP87_FULL);
    details.print();
    std::cout << std::endl;

    // AA+ - P.J.Naughter
    std::cout << "MARS - AA+ v2.49 - P.J.Naughter" << std::endl;
    CAAEllipticalPlanetaryDetails aaplus = CAAElliptical::Calculate(astro->getJD_TT(), CAAElliptical::Object::MARS, true);
    std::cout << " True Heliocentric Ecliptic Spherical:    "
        << aaplus.TrueHeliocentricEclipticalLatitude << ","
        << aaplus.TrueHeliocentricEclipticalLongitude << ","
        << aaplus.TrueHeliocentricDistance << std::endl;

    std::cout << " True Geocentric Ecliptic Rectangular:     "
        << aaplus.TrueGeocentricRectangularEcliptical.X << ","
        << aaplus.TrueGeocentricRectangularEcliptical.Y << ","
        << aaplus.TrueGeocentricRectangularEcliptical.Z << std::endl;

    std::cout << " True Geocentric Ecliptic Spherical:       "
        << aaplus.TrueGeocentricEclipticalLatitude << ","
        << aaplus.TrueGeocentricEclipticalLongitude << ","
        << aaplus.TrueGeocentricDistance << std::endl;

    std::cout << " True Geocentric Equatorial Spherical:     "
        << aaplus.TrueGeocentricDeclination << ","
        << aaplus.TrueGeocentricRA << ","
        << aaplus.TrueGeocentricDistance << std::endl;

    std::cout << " Apparent Geocentric Ecliptic Spherical:   "
        << aaplus.ApparentGeocentricEclipticalLatitude << ","
        << aaplus.ApparentGeocentricEclipticalLongitude << ","
        << aaplus.ApparentGeocentricDistance << std::endl;

    std::cout << " Apparent Geocentric Equatorial Spherical: "
        << aaplus.ApparentGeocentricDeclination << ","
        << aaplus.ApparentGeocentricRA << ","
        << aaplus.ApparentGeocentricDistance << std::endl;

    // Sun - 2024-04-08 12:00 UTC

    // Astronomy::planetaryDetails() - M.D.Olsen
    std::cout << "\n\nSUN " << astro->timestr << " - MDO Library" << std::endl;
    details = astro->planetaryDetails(astro->getJD_TT(), A_SUN, VSOP87_FULL);
    details.print();
    std::cout << std::endl;

    // AA+ - P.J.Naughter
    std::cout << "SUN - AA+ v2.49 - P.J.Naughter" << std::endl;
    aaplus = CAAElliptical::Calculate(astro->getJD_TT(), CAAElliptical::Object::SUN, true);
    std::cout << " True Heliocentric Ecliptic Spherical:     "
        << aaplus.TrueHeliocentricEclipticalLatitude << ","
        << aaplus.TrueHeliocentricEclipticalLongitude << ","
        << aaplus.TrueHeliocentricDistance << std::endl;

    std::cout << " True Geocentric Ecliptic Rectangular:     "
        << aaplus.TrueGeocentricRectangularEcliptical.X << ","
        << aaplus.TrueGeocentricRectangularEcliptical.Y << ","
        << aaplus.TrueGeocentricRectangularEcliptical.Z << std::endl;

    std::cout << " True Geocentric Ecliptic Spherical:       "
        << aaplus.TrueGeocentricEclipticalLatitude << ","
        << aaplus.TrueGeocentricEclipticalLongitude << ","
        << aaplus.TrueGeocentricDistance << std::endl;

    std::cout << " True Geocentric Equatorial Spherical:     "
        << aaplus.TrueGeocentricDeclination << ","
        << aaplus.TrueGeocentricRA << ","
        << aaplus.TrueGeocentricDistance << std::endl;

    std::cout << " Apparent Geocentric Ecliptic Spherical:   "
        << aaplus.ApparentGeocentricEclipticalLatitude << ","
        << aaplus.ApparentGeocentricEclipticalLongitude << ","
        << aaplus.ApparentGeocentricDistance << std::endl;

    std::cout << " Apparent Geocentric Equatorial Spherical: "
        << aaplus.ApparentGeocentricDeclination << ","
        << aaplus.ApparentGeocentricRA << ","
        << aaplus.ApparentGeocentricDistance << std::endl;


    //// Time the two functions to verify cost of the refactor
    //auto start = std::chrono::high_resolution_clock::now();
    //auto stop = std::chrono::high_resolution_clock::now();
    //CelestialDetailFull old1;
    //CAAEllipticalPlanetaryDetails old2;
    //double count = 10000; // 100'000;
    //double i = 0;
    //
    //// AA+ v2.49
    //start = std::chrono::high_resolution_clock::now();
    //for (i = 0; i < count; i++) {
    //    old2 = CAAElliptical::Calculate(astro->getJD_TT(), CAAElliptical::Object::MARS, true);
    //}
    //stop = std::chrono::high_resolution_clock::now();
    //std::cout << "CAAElliptical::Calculate() Time in us:  " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << '\n';
    //
    //// Astronomy (MDO)
    //start = std::chrono::high_resolution_clock::now();
    //for (i = 0; i < count; i++) {
    //    old1 = astro->planetaryDetails(astro->getJD_TT(), A_MARS, VSOP87_FULL);
    //}
    //stop = std::chrono::high_resolution_clock::now();
    //std::cout << "Astronomy::planetaryDetail() Time in us:  " << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() << '\n';
}

#include "astronomy/amoon.h"
#include "astronomy/astars.h"  // for precession 
#include "AAplus/AAMoon.h"
#include "AAplus/AAELP2000.h"
#include "AAplus/AAELPMPP02.h"

void testLunarPosition(Application& app) {
    // Moon - 2024-04-08 12:00 UTC

    // My library (converted from AA+ to always use radians)
    Astronomy* astro = app.newAstronomy();
    //astro->setTime(2024, 4, 8.0, 12.0, 0.0, 0.0);
    //astro->setTime(1992, 4, 12.0, 0.0, 0.0, 0.0);
    astro->setJD_UTC(2448724.5);
    double jd_tt = astro->getJD_TT();
    //double jd_tt = 2448724.5;

    // True Geocentric Ecliptic Spherical coordinates
    LLD meeusa1 = AMoon::EclipticCoordinates(jd_tt, MEEUS_SHORT);
    LLD meeusb1 = astro->MoonApparentEcliptic(jd_tt, MEEUS_SHORT);
    LLD elp2ka1 = AMoon::EclipticCoordinates(jd_tt, ELP2000_82);
    LLD elp2kb1 = astro->MoonTrueEcliptic(jd_tt, ELP2000_82);
    LLD elp2kc1 = AELP2000::EclipticCoordinates(jd_tt);
    LLD mpp02a1 = AMoon::EclipticCoordinates(jd_tt, ELP_MPP02);
    LLD mpp02b1 = astro->MoonTrueEcliptic(jd_tt, ELP_MPP02);
    glm::dvec3 mrect = AELP2000::EclipticRectangularCoordinates(jd_tt);
    glm::dvec3 mrfk5 = AELP2000::EquatorialRectangularCoordinatesFK5(jd_tt);
    LLD mfk5 = Spherical::Rectangular2Spherical(mrfk5);
    LLD mdate = APrecession::PrecessEquatorialJ2000(mfk5, jd_tt);

    std::cout << "\n\nMOON " << astro->timestr << " -> jd_tt: " << jd_tt << " - MDO Library" << std::endl;
    std::cout << "Ephemeris coordinates: " << std::endl;
    std::cout << " MDO Meeus Short: " << meeusa1.str_EC() << " - " << astro->angle2DMSstring(meeusa1.lat, true) << "," << astro->angle2uDMSstring(meeusa1.lon, true) << std::endl;
    std::cout << " MDO ELP2000-82:  " << elp2ka1.str_EC() << " - " << astro->angle2DMSstring(elp2ka1.lat, true) << "," << astro->angle2uDMSstring(elp2ka1.lon, true) << std::endl;
    std::cout << " LLD ELP2000-82:  " << elp2kc1.str_EC() << " - " << astro->angle2DMSstring(elp2kc1.lat, true) << "," << astro->angle2uDMSstring(elp2kc1.lon, true) << std::endl;
    std::cout << " MDO ELP2000-82 Rectangular:  " << mrect.x << "," << mrect.y << "," << mrect.z << std::endl;
    std::cout << " MDO ELP2000-82 EQ FK5@J2000: " << mrfk5.x << "," << mrfk5.y << "," << mrfk5.z << std::endl;
    std::cout << " MDO ELP2000-82 FK5@J2000: " << mfk5.str_EC() << " - " << astro->angle2DMSstring(mfk5.lat, true) << "," << astro->angle2uHMSstring(mfk5.lon, true) << std::endl;
    std::cout << " MDO ELP2000-82 FK5@date: " << mdate.str_EC() << " - " << astro->angle2DMSstring(mdate.lat, true) << "," << astro->angle2uHMSstring(mdate.lon, true) << std::endl;
    std::cout << " MDO ELPMPP02:    " << mpp02a1.str_EC() << " - " << astro->angle2DMSstring(mpp02a1.lat, true) << "," << astro->angle2uDMSstring(mpp02a1.lon, true) << std::endl;
    std::cout << std::endl;


    // AA+ - P.J.Naughter
    std::cout << "MOON - AA+ v2.49 - P.J.Naughter" << std::endl;
    std::cout << "Ephemeris coordinates: " << std::endl;
    std::cout << " AA+ Meeus Short: "
        << CAAMoon::EclipticLatitude(jd_tt) << ","
        << CAAMoon::EclipticLongitude(jd_tt) << ","
        << CAAMoon::RadiusVector(jd_tt) << std::endl;
    std::cout << " AA+ ELP2000-82:  "
        << CAAELP2000::EclipticLatitude(jd_tt) << ","
        << CAAELP2000::EclipticLongitude(jd_tt) << ","
        << CAAELP2000::RadiusVector(jd_tt) << std::endl;
    CAA3DCoordinate rect{ CAAELP2000::EclipticRectangularCoordinates(jd_tt) };
    std::cout << " AA+ ELP2000-82 Rectangular: " << rect.X << "," << rect.Y << "," << rect.Z << std::endl;
    CAA3DCoordinate rfk5{ CAAELP2000::EquatorialRectangularCoordinatesFK5(jd_tt) };
    std::cout << " AA+ ELP2000-82 EQ Rect FK5: " << rfk5.X << "," << rfk5.Y << "," << rfk5.Z << std::endl;
    LLD sfk5 = Spherical::Rectangular2Spherical({ rfk5.X, rfk5.Y, rfk5.Z });
    std::cout << " LLD ELP2000-82 FK5: " << sfk5.str_EC() << " - " << astro->angle2DMSstring(sfk5.lat, true) << "," << astro->angle2uHMSstring(sfk5.lon, true) << std::endl;
    std::cout << " AA+ ELPMPP02:    "
        << CAAELPMPP02::EclipticLatitude(jd_tt) << ","
        << CAAELPMPP02::EclipticLongitude(jd_tt) << ","
        << CAAELPMPP02::RadiusVector(jd_tt) << std::endl;

    // NOTE: When comparing the above to Stellarium, the following facts are important:
    //       1) Stellarium displays and takes JD in UTC!
    //       2) Switch off topocentric coordinates!
    //       3) Switch off atmosphere!
    //       4) Switch off aberration.
    //       5) Meeus Short returns Ecliptic of date directly.
    //       6) ELP2000_82 returns FK5 @J2000 when using EquatorialRectangularCoordinatesFK5()
    astro->setJD_UTC(JD_2000);
    std::cout << "\n\n J2000.0 UTC: " << astro->getJD_UTC() << " TT: " << astro->getJD_TT() << std::endl;
}


void testDetailedEarth(Application& app) {
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2024, 4, 8.0, 18.0, 40.0, 0.0);
    //std::cout << astro.getTimeString() << std::endl;
    Scene* scene = app.newScene();
    scene->astro = astro;
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "EarthView");

    DetailedEarth* erf = new DetailedEarth(scene, nullptr, "NSAE", 180, 90, 1.0f);
    app.currentEarth2 = erf;
    erf->w_twilight = false;
    //erf->addEquator();
    //erf->addPrimeMeridian();
    //erf->addArcticCircles();
    //erf->removeArcticCircles();
    //erf->addTropicCircles();
    erf->addSunGP();
    erf->sungp->setRadius(0.1f);
    //erf->position = { 0.0f, 0.5f, 0.0f };
    //erf->addGrid(15.0);
    //erf->addCelestialSphere();
    //erf->celestialgrid->setSpacing(24);
    //erf->gridOb->setColor(LIGHT_GREY);
    //erf->addEcliptic();
    erf->addMoonGP();
    erf->moongp->setRadius(0.1f);

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            //astro->setTimeNow();
            astro->addTime(0.0, 0.0, 4.0, 0.0);
            //astro->addTime(0.0, 0.0, 0.0, 31558149.504); // Sidereal year in seconds
        }

        //app.anim = false; // Nice for single step action. <space> will set app.anim in app.render, and we get back here one frame later.
        app.render();
    }

}



// Application is a global container for all the rest
Application app = Application();  // New global after refactor

// ----------------------
//  Idle Area for python
// ----------------------
void IdleArea(Application& myapp) {
    // An Idle function to drop into after a python script has completed, so the scene can be interacted with for planning the next animation steps.
    // Scans for the various elements created by the python script and takes over by adding GUI / keyboard interaction
    myapp.anim = false;
    RenderLayerGUI* gui = myapp.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    unsigned int lnum = 1;
    char lname[20];
    for (auto& l : myapp.m_layers) {
        if (l->type == LAYER3D) {
            sprintf_s(lname, "Scene %02d", lnum);
            gui->addLayer3D((RenderLayer3D*)l, lname);
            lnum++;
        }
    }
    while (!myapp.shouldClose()) {
        myapp.currentCam->update();
        myapp.update();
        //if (myapp.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        //scene->w_camera->CamUpdate();

        myapp.render();
    }
}



// -------------------------------------
//  Python scripting module definitions
// -------------------------------------
Application* getApplication() {
    // Since the allocated Application instance is currently a global, this function allows the python module to access it
    // Application then has hooks to access all the other items via getXXXX() or newXXXX() depending on whether more than one is allowed
    //  (getXXXX() when all need to share the same instance, newXXXX() when multiple can be used)
    return &app;
}
PYBIND11_EMBEDDED_MODULE(eartharium, m) {
    // FIX: !!! Should probably be moved into a separate translation unit (*.h/*.cpp) !!!
    // IMPORTANT: Define things in order of dependencies. If not, loading the module will hang with no errors. Very annoying!
    m.doc() = "Eartharium Module";
    m.def("getApplication", &getApplication, py::return_value_policy::reference);
    py::class_<LLD>(m, "LLH")
        .def(py::init<double, double, double>())
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
    py::class_<ImFont>(m, "Font") // From https://toscode.gitee.com/lilingTG/bimpy/blob/master/bimpy.cpp it has lots of ImGui .def's
        .def(py::init())
        ;
    py::class_<EDateTime>(m, "EDateTime")
        .def(py::init<>())
        .def(py::init<double>())
        .def(py::init<long, long, double, double, double, double>())
        .def(py::init<long>())
        .def("year", &EDateTime::year, "Obtains the year of current date time.")
        .def("month", &EDateTime::month, "Obtains the month of current date time.")
        .def("day", &EDateTime::day, "Obtains the day of current date time.")
        .def("hour", &EDateTime::hour, "Obtains the hour of current date time.")
        .def("minute", &EDateTime::minute, "Obtains the minute of current date time.")
        .def("second", &EDateTime::second, "Obtains the second of current date time.")
        .def("jd_tt", &EDateTime::jd_tt, "Obtains the JD of current date time in TT (dynamical time).")
        .def("jd_utc", &EDateTime::jd_utc, "Obtains the JD of current date time in (astrnomical) UTC.")
        .def("isLeap", &EDateTime::isLeap, "Returns True if current year is leap, false otherwise (year 1 BCE is year 0).")
        .def("weekday", &EDateTime::weekday, "Returns day of week; Sunday = 0, Monday = 1, etc.")
        .def("jd_utc", &EDateTime::jd_utc, "Obtains the JD of current date time in (astrnomical) UTC.")
        .def("string", &EDateTime::string, "Obtains date time string in UTC.")
        .def("unixTime", &EDateTime::unixTime, "Obtains current date time in (integer) Unix timestamp format.")
        // setTime(long year, long month, double day, double hour, double minute, double second)
        .def("setTime", &EDateTime::setTime,
            "Sets current date time in integer year, month, float day, hour, minute, second.",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        .def("setTimeNow", &EDateTime::setTimeNow,
            "Sets EDateTime to current system date time in UTC."
        )
        .def("setJD_UTC", &EDateTime::setJD_UTC,
            "Sets current date time to JD in UTC.",
            py::arg("jd_utc")
        )
        .def("setJD_TT", &EDateTime::setJD_TT,
            "Sets current date time to JD in TT (dynamical time).",
            py::arg("jd_tt")
        )
        .def("addTime", &EDateTime::setTime,
            "Adds int year, month, float day, hour, minute, second to current date time.",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        // static void normalizeDateTime(long& yr, long& mo, double& da, double& hr, double& mi, double& se)
        // FIX: !!! The above modified parameters will not be passed back to Python I think !!!
        // static int myDivQuotient(const int a, const int b)
        .def_static("myDivQuotient", &EDateTime::myDivQuotient,
            "This and myDivRemainder are required for some JD conversions.",
            py::arg("int1"), py::arg("int2")
        )
        // static int myDivRemainder(const int a, const int b)
        .def_static("myDivRemiander", &EDateTime::myDivRemainder,
            "This and myDivQuotient are required for some JD conversions.",
            py::arg("int1"), py::arg("int2")
        )
        // static double getDateTime2JD_UTC(const long year, const long month, const double day, const double hour, const double minute, const double second)
        .def_static("getDateTime2JD_UTC", &EDateTime::getDateTime2JD_UTC,
            "Convert int year, month, float day, hour, minute, second to JD in UTC.",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        // static double getDateTime2JD_TT(const long year, const long month, const double day, const double hour, const double minute, const double second)
        .def_static("getDateTime2JD_TT", &EDateTime::getDateTime2JD_TT,
            "Convert int year, month, float day, hour, minute, second to JD in TT (dynamical time).",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        // static double getUnixTime2JD_UTC(const long unixtime)
        .def_static("getUnixTime2JD_UTC", &EDateTime::getUnixTime2JD_UTC,
            "Convert Unix timestamp (in UTC) to JD in UTC.",
            py::arg("unixTime")
        )
        // static double getUnixTime2JD_TT(const long unixtime)
        .def_static("getUnixTime2JD_TT", &EDateTime::getUnixTime2JD_TT,
            "Convert Unix timestamp (in UTC) to JD in TT (dynamical time).",
            py::arg("unixTime")
        )
        // static long getDateTime2UnixTime(const long year, const long month, const double day, const double hour, const double minute, const double second)
        .def_static("getDateTime2UnixTime", &EDateTime::getDateTime2UnixTime,
            "Convert int year, month, float day, hour, minute, second to Unix timestamp in UTC.",
            py::arg("year"), py::arg("month"), py::arg("day"), py::arg("hour"), py::arg("minute"), py::arg("second")
        )
        // static double getJD2MJD(const double jd)
        .def_static("getJD2MJD", &EDateTime::getJD2MJD,
            "Convert a JD (in TT or UTC) to Modified Julian Date (also in TT or UTC).",
            py::arg("jd")
        )
        // static double getMJD2JD(const double mjd)
        .def_static("getMJD2JD", &EDateTime::getMJD2JD,
            "Convert a Modified Julian Date (in TT or UTC) to JD (also in TT or UTC).",
            py::arg("mjd")
        )
        // static double getJDUTC2TT(const double jd)
        .def_static("getJDUTC2TT", &EDateTime::getJDUTC2TT,
            "Convert a JD in UTC to JD in TT (dynamic time).",
            py::arg("jd_utc")
        )
        // static double getJDTT2UTC(const double jd)
        .def_static("getJDTT2UTC", &EDateTime::getJDTT2UTC,
            "Convert a JD in TT (dynamic time) to JD in UTC.",
            py::arg("jd_tt")
        )
        // static long calcUnixTimeYearDay(const long year, const long month, const double day)
        .def_static("calcUnixTimeYearDay", &EDateTime::calcUnixTimeYearDay,
            "Calculate Unix time of a date in UTC. Used internally in EDateTime.",
            py::arg("year"), py::arg("month"), py::arg("day")
        )
        // static bool isLeapYear(const long year)
        .def_static("isLeapYear", &EDateTime::isLeapYear,
            "Returns true if provided year is a leap year, false otherwise. Note: year 1 BCE = year 0.",
            py::arg("year")
        )
        ;
    // -----------------
    //  Astronomy class
    // -----------------
    py::class_<Astronomy>(m, "Astronomy")
        .def("setTime", &Astronomy::setTime, "Sets time in (Y,M,D,h,m,s.ss)",
            py::arg("yr"), py::arg("mo"), py::arg("da"), py::arg("hr"), py::arg("mi"), py::arg("se")
        )
        .def("setTimeNow", &Astronomy::setTimeNow, "Sets time to system clock in UTC")
        .def("setJD_TT", &Astronomy::setJD_TT, "Sets time to Julian Day",
            py::arg("jd")
        )
        .def("setJD_UTC", &Astronomy::setJD_UTC, "Sets time to Julian Day",
            py::arg("jd")
        )
        .def("getJD_TT", &Astronomy::getJD_TT, "Returns the current JD in TT (dynamical time)")
        .def("getJD_UTC", &Astronomy::getJD_UTC, "Returns the current JD in UTC")
        .def("addTime", &Astronomy::addTime, "Adjusts current time by provided amount",
            py::arg("d"), py::arg("h"), py::arg("min"), py::arg("sec"), py::arg("eot")
        )
        // FIX: !!! Do we need both of these exposed to Python? !!!
        .def("ApparentGreenwichSiderealTime", &Astronomy::ApparentGreenwichSiderealTime, "Returns Greenwich Sidereal Time for provided JD (in UTC)",
            py::arg("jd_utc"), py::arg("rad")
        )
        .def("MeanGreenwichSiderealTime", &Astronomy::MeanGreenwichSiderealTime, "Returns Greenwich Sidereal Time for provided JD (in UTC), or for current JD if omitted",
            py::arg("jd_utc"), py::arg("rad")
        )
        .def("getTimeString", &Astronomy::getTimeString, "Returns current time & date in string format YYYY-MM-DD HH:MM:SS UTC")
        ;
    // --------------
    //  Camera class
    // --------------
    py::class_<Camera>(m, "Camera")
        .def_readwrite("camFoV", &Camera::camFoV)
        .def_readwrite("camLat", &Camera::camLat)
        .def_readwrite("camLon", &Camera::camLon)
        .def_readwrite("camDst", &Camera::camDst)
        .def("CamUpdate", &Camera::update, "Updates the Camera settings from Application etc")
        ;
    // -------------
    //  Scene class
    // -------------
    py::class_<Scene>(m, "Scene")
        .def("newEarth", &Scene::newEarth, "Adds a new Earth object to the Scene",
            py::arg("mode"),
            py::arg("meshU") = 90,
            py::arg("meshV") = 45
        )
        .def_readwrite("w_camera", &Scene::w_camera, "Stores the default Camera for the Scene. Do not write to this.",
            py::return_value_policy::reference)
        ;
    py::class_<RenderLayerTextLines>(m, "RenderLayerTextLines")
        .def("addLine", &RenderLayerTextLines::addLine, "Adds a line of text to the renderlayer",
            py::arg("line")
        )
        ;
    py::class_<RenderLayerText>(m, "RenderLayerText")
        .def("setAstronomy", &RenderLayerText::setAstronomy, "Sets the Astronomy keeping time, if synchronized UTC display is desired.",
            py::arg("astro")
        )
        .def("setFont", &RenderLayerText::setFont, "Set the predefined font to use for the text in the layer",
            py::arg("font")
        )
        ;
    py::class_<RenderLayer3D>(m, "RenderLayer3D")
        .def("updateView", &RenderLayer3D::updateView, "Updates the width and height of the view. Called automatically when window size changes.",
            py::arg("w"), py::arg("h")
        )
        ;
    py::class_<Application>(m, "Application")
        //.def("getRenderChain", &Application::getRenderChain, py::return_value_policy::reference)
        .def("newScene", &Application::newScene, py::return_value_policy::reference)
        .def("newAstronomy", &Application::newAstronomy, py::return_value_policy::reference)
        .def("update", &Application::update)
        // RenderLayer3D* newLayer3D(float vpx1, float vpy1, float vpx2, float vpy2, Scene* scene, Astronomy* astro, Camera* cam = nullptr, bool overlay = true);
        .def("newLayer3D", &Application::newLayer3D, "Creates a new 3D Render Layer",
            py::arg("vpx1"), py::arg("vpy1"), py::arg("vpx2"), py::arg("vpy2"),
            py::arg("scene"), py::arg("astro"), py::arg("cam") = (Camera*) nullptr,
            py::arg("overlay") = true,
            py::return_value_policy::reference
        )
        // RenderLayerText* newLayerText(float vpx1, float vpy1, float vpx2, float vpy2, RenderLayerTextLines* lines = nullptr);
        .def("newLayerText", &Application::newLayerText, "Creates a new Text Render Layer",
            py::arg("vpx1"), py::arg("vpy1"), py::arg("vpx2"), py::arg("vpy2"),
            py::arg("lines") = (RenderLayerTextLines*) nullptr,
            py::return_value_policy::reference
        )
        // FIX: !!! Clean up the following, they are not logical (do_render & render in particular) !!!
        .def("do_render", &Application::render)
        .def("shouldClose", &Application::shouldClose)
        .def_readwrite("interactive", &Application::interactive)
        .def_readwrite("render", &Application::renderoutput)
        .def_readwrite("currentCam", &Application::currentCam)
        .def_readwrite("currentEarth", &Application::currentEarth)
        .def_readwrite("m_font2", &Application::m_font2)
        ;
    py::class_<Location>(m, "Location")
        .def("addLocDot", &Location::addLocDot, "Adds an icosphere dot to the Location at height 0.",
            py::arg("size") = locdotsize,
            py::arg("color") = DEFAULT_LOCDOT_COLOR)
        ;
    py::class_<LocGroup>(m, "LocGroup")
        .def("clear", &LocGroup::clear, "Clears all Locations from group.")
        .def("size", &LocGroup::size, "Returns current count of Locations.")
        .def("addLocation", &LocGroup::addLocation, "Creates and adds a new Location to the group.",
            py::arg("lat"), py::arg("lon"), py::arg("rad") = true,
            py::arg("rsky") = 0.02f)
        .def("trimLocation", &LocGroup::trimLocation, "Discards the last Location in the group.")
        ;
    py::class_<Earth>(m, "Earth")
        .def_readwrite("flatsunheight", &Earth::flatsunheight)
        .def_readwrite("param", &Earth::param)
        .def_readwrite("tropicsoverlay", &Earth::tropicsoverlay)
        .def_readwrite("arcticsoverlay", &Earth::arcticsoverlay)
        .def("getSubsolarXYZ", &Earth::getSubsolarXYZ, py::return_value_policy::reference)
        .def("addSubsolarPoint", &Earth::addSubsolarPoint, "Adds a subsolar marker, really a flat earth Sun",
            py::arg("size") = 0.03f)
        .def("addGrid", &Earth::addGrid, "Enables the typical lat lon grid",
            py::arg("deg") = 15.0f,
            py::arg("size") = 0.002f,
            py::arg("color") = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
            py::arg("type") = "LALO",
            py::arg("rad") = false,
            py::arg("eq") = true,
            py::arg("pm") = true)
        .def("addEquator", &Earth::addEquator, "Adds equator curve.",
            py::arg("size") = 0.002f,
            py::arg("color") = RED)
        .def("addTropics", &Earth::addTropics, "Adds tropics curves.",
            py::arg("size") = 0.002f,
            py::arg("color") = YELLOW)
        .def("addArcticCirles", &Earth::addArcticCirles, "Adds arctics curves.",
            py::arg("size") = 0.002f,
            py::arg("color") = AQUA)
        .def("addPrimeMeridian", &Earth::addPrimeMeridian, "Adds prime meridian curve.",
            py::arg("size") = 0.002f,
            py::arg("color") = RED)
        .def("addLocGroup", &Earth::addLocGroup, "Adds new empty LocGroup to Earth. Locations are added using LocGroup::addLocation().")
        .def_readwrite("locgroups", &Earth::locgroups)
        .def("addGreatArc", &Earth::addGreatArc, "Great circle segment between two points.",
            py::arg("llh1"), py::arg("llh2"),
            py::arg("color") = LIGHT_YELLOW,
            py::arg("width") = 0.003f,
            py::arg("rad") = false)
        .def("addLerpArc", &Earth::addLerpArc, "Naive lerped segment between two points.",
            py::arg("llh1"), py::arg("llh2"),
            py::arg("color") = LIGHT_YELLOW,
            py::arg("width") = 0.003f,
            py::arg("rad") = false)
        .def("addFlatArc", &Earth::addFlatArc, "AE shortest segment between two points.",
            py::arg("llh1"), py::arg("llh2"),
            py::arg("color") = LIGHT_YELLOW,
            py::arg("width") = 0.003f,
            py::arg("rad") = false)
        ;
}


int main(int argc, char** argv) {
    // FIX: !!! Add cli option parser, so -s can specify a Python script to run !!!
    // 
    // This is main. The entry point of execution when launching
    // Do as little as possible here.
    app.start_fullscreen = false;  // Starting full screen can make debugging difficult during a hang
    if (app.initWindow() == -1) return -1; // Bail if Window and OpenGL context setup fails

    // FIX: !!! This is not very robust, plenty of room for improvement !!!
    std::string pythonscript;
    if (argc > 1) {
        pythonscript = "c:\\Coding\\Eartharium\\Eartharium\\";
        pythonscript += +argv[1];
    }
    if (pythonscript.size() > 0) { // Set to true to run a python script and drop into an idle interactive
        // if (std::filesystem::exists("helloworld.txt"))
        std::cout << "Running Python script: " << pythonscript << '\n';
        app.interactive = true;
        py::scoped_interpreter guard{};
        py::object scope = py::module_::import("__main__").attr("__dict__");
        py::eval_file(pythonscript.c_str(), scope);
        //IdleArea(app);
        glfwTerminate();
        return 0; // FIX: !!! Letting the OS deal with all the memory leaks, Not all destructors are up to date currently !!!
    }
    // If no python script was run, simply drop to a test area that sets up via C++ instead.
    // Good for things not yet implemented in python interface, or while developing things,
    // but requires a recompile for every change. Python scripts can simply be saved after changes, and Eartharium can be run again.
    
    // Call scenario
    //TimeGetDetails(app);
    //testPlanetaryDetail(app);
    testLunarPosition(app);
    //testDetailedEarth(app);

    // Cleanup - Move this to cleanup function in Application.
    glfwTerminate();

    return 0;  // Poor Windows will deal with cleaning up the heap
}


// ----------------
//  GLFW Callbacks
// ----------------
void keyboard_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
    // GLFW has a C interface for callbacks, so this has not yet been moved into Application. I'll have to look into that.
    // Mapped Keys: <ESC>,<SPACE>,a,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,s,u,w,z
    // Unmapped Keys: b,r,v,x,y,1,2,3,4,5,6,7,8,9,0
    if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
        // ESC - End application - may move to end animation when timeline edits have been implemented
        if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(app.window, true); }
        //if (key == GLFW_KEY_L) toggle = !toggle;  // Was once able to toggle between filled triangles and wireframe mode. Shaders no longer support that.
        // SPACE - Toggle Time Animation
        if (key == GLFW_KEY_SPACE) { app.anim = !app.anim; }
        // G - Toggle GUI
        if (key == GLFW_KEY_G) { app.gui = !app.gui; }
        // F - Toggle Full Screen / Windowed mode
        if (key == GLFW_KEY_F) { app.togglefullwin = true; }
        // P - Print Camera Settings - Replace with a button in Camera GUI !!!
        if (key == GLFW_KEY_P) { app.dumpcam = true; }
        // T - Print Current Time - Replace with a button in Time GUI !!!
        if (key == GLFW_KEY_T) { app.dumptime = true; }
        // B - Dump Data - To be used during troubleshooting to dump data in a given function
        if (key == GLFW_KEY_B) { app.dumpdata = true; }
        // N M - Morph Plus/Minus
        if (key == GLFW_KEY_N && app.currentEarth != nullptr) {
            if (app.currentEarth->param == 0.0f) return;
            app.currentEarth->param -= 0.025f;
            if (app.currentEarth->param < 0.0f) app.currentEarth->param = 0.0f;
        }
        if (key == GLFW_KEY_N && app.currentEarth2) {
            float curmorph = app.currentEarth2->getMorphParameter() - 0.025f;
            app.currentEarth2->setMorphParameter(curmorph > 0.0f ? curmorph : 0.0f);
        }
        if (key == GLFW_KEY_M && app.currentEarth != nullptr) {
            if (app.currentEarth->param == 1.0) return;
            app.currentEarth->param += 0.025f;
            if (app.currentEarth->param > 1.0f) app.currentEarth->param = 1.0f;
        }
        if (key == GLFW_KEY_M && app.currentEarth2) {
            float curmorph = app.currentEarth2->getMorphParameter() + 0.025f;
            app.currentEarth2->setMorphParameter(curmorph < 1.0f ? curmorph : 1.0f);
        }
        // V B - Earth Eccentricity Plus / Minus - May remove or replace !!!
        if (key == GLFW_KEY_V && app.currentEarth != nullptr) {
            if (app.currentEarth->eccen == 0.0) return;
            app.currentEarth->eccen -= 0.01;
            if (app.currentEarth->eccen < 0.0) app.currentEarth->eccen = 0.0;
        }
        if (key == GLFW_KEY_B && app.currentEarth != nullptr) {
            if (app.currentEarth->eccen == 1.0) return;
            app.currentEarth->eccen += 0.01;
            if (app.currentEarth->eccen > 1.0) app.currentEarth->eccen = 1.0;
        }
        // W A S D - Spin Camera around LookAt point
        if (key == GLFW_KEY_A && app.currentCam != nullptr) {
            app.currentCam->camLon -= CAMERA_ANGLE_STEP;
            if (app.currentCam->camLon > 180.0f) app.currentCam->camLon -= 360.0f;
            if (app.currentCam->camLon < -180.0f) app.currentCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_D && app.currentCam != nullptr) {
            app.currentCam->camLon += CAMERA_ANGLE_STEP;
            if (app.currentCam->camLon > 180.0f) app.currentCam->camLon -= 360.0f;
            if (app.currentCam->camLon < -179.9f) app.currentCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_W && app.currentCam != nullptr) {
            app.currentCam->camLat += CAMERA_ANGLE_STEP;
            if (app.currentCam->camLat > CAMERA_MAX_LATITUDE) app.currentCam->camLat = CAMERA_MAX_LATITUDE;
        }
        if (key == GLFW_KEY_S && app.currentCam != nullptr) {
            app.currentCam->camLat -= CAMERA_ANGLE_STEP;
            if (app.currentCam->camLat < CAMERA_MIN_LATIDUDE) app.currentCam->camLat = CAMERA_MIN_LATIDUDE;
        }
        // Q E - Increase / Decrease Camera FoV
        if (key == GLFW_KEY_Q && app.currentCam != nullptr) {
            app.currentCam->camFoV += app.currentCam->camFoV * CAMERA_FOV_FACTOR;
            if (app.currentCam->camFoV > CAMERA_MAX_FOV) app.currentCam->camFoV = CAMERA_MAX_FOV;
        }
        if (key == GLFW_KEY_E && app.currentCam != nullptr) {
            app.currentCam->camFoV -= app.currentCam->camFoV * CAMERA_FOV_FACTOR;
            if (app.currentCam->camFoV > CAMERA_MAX_FOV) app.currentCam->camFoV = CAMERA_MAX_FOV;
        }
        // Z C - Increase / Decrease Camera distance to LookAt point
        if (key == GLFW_KEY_C && app.currentCam != nullptr) {
            app.currentCam->camDst -= app.currentCam->camDst * CAMERA_STEP_FACTOR;
            if (app.currentCam->camDst < CAMERA_MIN_DIST) app.currentCam->camDst = CAMERA_MIN_DIST;
        }
        if (key == GLFW_KEY_Z && app.currentCam != nullptr) {
            app.currentCam->camDst += app.currentCam->camDst * CAMERA_STEP_FACTOR;
            if (app.currentCam->camDst > CAMERA_MAX_DIST) app.currentCam->camDst = CAMERA_MAX_DIST;
        }
        // I J K L - Location Camera controls
        if (key == GLFW_KEY_J && app.locationCam != nullptr) {
            app.locationCam->camLon -= CAMERA_ANGLE_STEP;
            if (app.locationCam->camLon > 360.0f) app.locationCam->camLon -= 360.0f;
            if (app.locationCam->camLon < 0.0f) app.locationCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_L && app.locationCam != nullptr) {
            app.locationCam->camLon += CAMERA_ANGLE_STEP;
            if (app.locationCam->camLon > 360.0f) app.locationCam->camLon -= 360.0f;
            if (app.locationCam->camLon < 0.0f) app.locationCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_I && app.locationCam != nullptr) {
            app.locationCam->camLat += CAMERA_ANGLE_STEP;
            if (app.locationCam->camLat > CAMERA_MAX_LATITUDE) app.locationCam->camLat = CAMERA_MAX_LATITUDE;
        }
        if (key == GLFW_KEY_K && app.locationCam != nullptr) {
            app.locationCam->camLat -= CAMERA_ANGLE_STEP;
            if (app.locationCam->camLat < CAMERA_MIN_LATIDUDE) app.locationCam->camLat = CAMERA_MIN_LATIDUDE;
        }
        // U O - Location Cam Field of View
        if (key == GLFW_KEY_U && app.locationCam != nullptr) {
            app.locationCam->camFoV += app.locationCam->camFoV * CAMERA_FOV_FACTOR;
            if (app.locationCam->camFoV > CAMERA_MAX_FOV) app.locationCam->camFoV = CAMERA_MAX_FOV;
        }
        if (key == GLFW_KEY_O && app.locationCam != nullptr) {
            app.locationCam->camFoV -= app.locationCam->camFoV * CAMERA_FOV_FACTOR;
            if (app.locationCam->camFoV > CAMERA_MAX_FOV) app.locationCam->camFoV = CAMERA_MAX_FOV;
        }
    };
}
// Called from GLFW when user resizes window. Registered above in setupEnv()
void framebuffer_size_callback(GLFWwindow* window, int mwidth, int mheight) {
    app.SetWH(mwidth, mheight);
}


// ----------------------
//  OpenGL Error handler
// ----------------------
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    // From: https://www.khronos.org/opengl/wiki/Debug_Output#Examples
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
        type, severity, message);
}
static void GLClearError() {
    unsigned int i = 0;
    while (glGetError() != GL_NO_ERROR) {
        i++;
    }
    std::cout << "Cleared errors: " << i << std::endl;
}
static void GLPrintError() {
    while (GLenum er = glGetError()) {
        std::cout << "New openGL error: " << er << std::endl;
    }
}
