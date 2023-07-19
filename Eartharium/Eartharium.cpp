
#include "config.h"
#include "OpenGL.h"
#include "Primitives.h"
#include "Earth.h"
#include "Astronomy.h"
// It would be good to clean up the header include hierarchy one day !!!

//#include <thread>

#include <python37/Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
namespace py = pybind11;

// Protos
void GLClearError();
void GLPrintError();


// -----------
//  Locations
// -----------
// Just a few handy locations. Eventually a location manager should load from a file and offer interactive maintenance !!!
LLH l_gnw = { 51.477811, 0.001475, 0.0 };      // Greenwich Royal Observatory, UK
LLH l_qqt = { 69.243574, -53.540529, 0.0 };    // Quequertarsuaq, Greenland
LLH l_cph = { 55.6761, 12.5683, 0.0 };         // Copenhagen, Denmark 55.6761° N, 12.5683° E
LLH l_nyc = { 40.7182, -74.0060, 0.0 };        // New York, USA 40.7128° N, 74.0060° W
LLH l_tok = { 35.668009, 139.773577, 0.0 };    // Tokyo, Japan 35.668009, 139.773577
LLH l_sye = { 24.0889, 32.8998, 0.0 };         // Syene, Egypt 24.0889° N, 32.8998° E
LLH l_alx = { 31.2001, 29.9187, 0.0 };         // Alexandria, Egypt 31.2001° N, 29.9187° E
LLH l_clm = { 6.9271, 79.8612, 0.0 };          // Colombo, Sri Lanka 6.9271° N, 79.8612° E
LLH l_rig = { -53.822025, -67.651686, 0.0 };   // Rio Grande, Argentina - 53.822025, -67.651686
LLH l_ams = { 52.3676, 4.9041, 0.0 };          // Amsterdam, The Netherlands 52.3676° N, 4.9041° E
LLH l_perth = { -31.942045, 115.963123, 0.0 }; // Perth airport(PER) Australia
LLH l_cptwn = { -33.974195, 18.602728, 0.0 };  // Cape Town airport South Africa
LLH l_puare = { -53.002324, -70.854556, 0.0 }; // Punta Arenas airport(PUQ) Chile
LLH l_stiag = { -33.3898, -70.7944, 0.0 };     // Santiago airport(SCL) Chile
LLH l_sydny = { -33.9399, 151.1753, 0.0 };     // Sydney airport(SYD) Australia
LLH l_buair = { -34.818463, -58.534176, 0.0 }; // Buenos Aires airport(EZE) Argentina
LLH l_frafu = { 50.037967, 8.561057, 0.0 };    // Frankfurt airport(FRA) Germany
LLH l_seoul = { 37.468028, 126.442011, 0.0 };  // Seoul airport(ICN) South Korea
LLH l_kabul = { 34.543447, 69.177123, 1817.522 };  // Kabul FlatEarthIntel.com measurepoint
LLH l_jamaica = { 18.1096, -77.2975, 0.0 };    // Jamaica according to google. For Columbus Eclipse

// Places to check webcams for sunrise/sunset
LLH l_phuket = { 7.891579, 98.295930, 0.0 };   // Phuket west facing (sunset) webcam: https://www.webcamtaxi.com/en/thailand/phuket/patong-beach-resort.html


// ---------------
//  Solar Eclipse
// ---------------
// Currently unused obviously. Here as a reminder to implement such a class
class SolarEclipse {
    // This is the stuff I'd love to make: https://www.youtube.com/watch?v=IaBt5Zau3ng&ab_channel=NASAScientificVisualizationStudio
    // Along with showing where the Sun and Moon are in relation to Earth, with shadow cones for umbra, penumbra and antumbra
public:
    
};


void SolSysTest(Application& app) {
    // Set up required components
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2019, 3, 20.0, 12.0, 0.0, 0.0);
    astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0); // Set time to 12:00 local solar
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene 1");

    std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);

    SolarSystem* solsys = scene->getSolsysOb();
    solsys->AddDistLine(SUN, MERCURY, MERCURYCOLOR, 0.01f);
    solsys->AddDistLine(SUN, VENUS, VENUSCOLOR, 0.01f);
    solsys->AddDistLine(SUN, MARS, MARSCOLOR, 0.01f);
    app.anim = false;
    app.renderoutput = false;
    while (!glfwWindowShouldClose(app.window)) {

        app.update();
        if (app.anim) astro->addTime(0.0, 10.0, 0.0, 0.0);

        //if (app.anim) astro->setTimeNow();
        scene->w_camera->update();

        app.render();
    }

}


// ---------------------
//  Refraction function
// ---------------------
double calcRefraction(Application& app, double altitude, double pressure = 1013.25, double temperature = 15.0) {
    // pressure in mBar, temperature in Celsius, altitude in degrees, returns arc minutes
    // Source: Almanac method https://www.madinstro.net/sundry/navsext.html
    // Bennett method described in J.Meeus, implemented in AA+
    double retval = NO_DOUBLE;
    if (app.sio_refmethod == REFR_BENNETT) retval = 60.0 * CAARefraction::RefractionFromApparent(altitude, pressure, temperature);
    else if (app.sio_refmethod == REFR_ALMANAC) retval = (0.267 * pressure / (temperature + 273.15))
        / tan(deg2rad * (altitude + 0.04848 / (tan(deg2rad * altitude) + 0.028)));
    else std::cout << "WARNING: calcRefraction() was called while app.sio_refmethod was set to unknown refraction method!\n";
    return retval;
}

void basic_geometry(Application& app) {
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;

    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, nullptr, cam);

    glm::vec4 spherecolor = LIGHT_RED;
    spherecolor.a = 0.7f;
    scene->getSphereUVFactory()->addStartEnd(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, spherecolor);
    //scene->getDotsFactory()->addXYZ(glm::vec3(0.0f), spherecolor, 1.0f);
    scene->getArrowsFactory()->addStartEnd(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -2.5f, 0.0f), 0.01f, YELLOW);
    glm::vec4 tangentcolor = LIGHT_GREEN;
    tangentcolor.a = 0.5f;
    scene->getPlanesFactory()->addStartUV(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f),tangentcolor);

    while (!glfwWindowShouldClose(app.window)) {
        app.render();
    }
}

void ScienceItOut_Response1(Application& app) {
    Astronomy* astro = app.newAstronomy();
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    cam->setLatLonFovDist(0.0f, 0.0f, 6.0f, 20.0f);
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene 1");

    //std::string fontname = "arialbd";
    //std::string fontname = "CourierNew";
    std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);

    app.basefname = "SIO1 ";

    Earth* earth = scene->newEarth("NSAE", 720, 360);
    app.currentEarth = earth;
    unsigned int lg = earth->addLocGroup();
    earth->w_refract = false;
    earth->w_sinsol = false;
    earth->w_twilight = false;
    earth->flatsunheight = 0.0f;

    glm::vec4 locskycolor = LIGHT_BLUE;
    locskycolor.a = 0.30f;
    float locskyradius = 0.3f;
    double loclat = 0.0; // l_kabul.lat;
    double loclon = 0.0; // l_kabul.lon;
    Location* myloc = earth->locgroups[lg]->addLocation(loclat, loclon, false, locskyradius);

    earth->addGrid();

    float pathwidth = 0.002f;

    app.renderoutput = true;

    app.render();

    // Test new SubPointSolver - Example 1
    SubPointSolver* solver1 = new SubPointSolver(earth);
    astro->setTime(2021, 12, 17.0, 10.0, 35.0, 27.0);
    SubStellarPoint* star1 = solver1->addSubStellarPoint("Alkaid", dms2deg(56.0, 7.0, 3.3));
    SubStellarPoint* star2 = solver1->addSubStellarPoint("Capella", dms2deg(33.0, 42.0, 42.5));
    SubStellarPoint* star3 = solver1->addSubStellarPoint("Alphard", dms2deg(38.0, 5.0, 46.3));
    app.render();

    cam->setLatLon((float)star1->m_loc.lat * rad2degf, (float)star1->m_loc.lon * rad2degf);
    app.render();
    solver1->showDots(NO_COLOR, 0.02f);
    app.render();
    solver1->showNames(font, LIGHT_RED, 0.06f);
    app.render();
    solver1->showSumnerLines(RED, pathwidth);
    app.render();
    solver1->showSumnerLines(NO_COLOR, pathwidth/10.0f);

    //solver1->showSumnerLines(RED, 0.001f);
    //solver1->showNames(font, LIGHT_RED, 0.06f);
    //solver1->showDots(NO_COLOR, 0.02f);
    LLH point1 = solver1->calcLocation(false);
    std::cout << "SubPointSolver Ex 1: " << point1.lat << ", " << point1.lon << '\n';
    earth->addDot(point1.lat, point1.lon, 0.0, 0.002f, LIGHT_GREEN, false);



    app.anim = false;
    app.renderoutput = false;
    while (!glfwWindowShouldClose(app.window)) {
        app.update();
        solver1->update();
        //solver2->update();
        //solver3->update();
        //solver4->update();
        if (app.anim) {
            astro->addTime(0.0, 0.0, 1.0, 0.0);
        }
        scene->w_camera->update();
        app.render();
    }

}

void Sandbox2_SIO(Application& app) { //https://www.youtube.com/watch?v=J7XmHIjKaP4&ab_channel=ScienceItOut
    // Set up required components
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2019, 3, 21.0, 7.0, 30.0, 34.0);
    astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0); // Set time to 12:00 local solar
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    //cam->camFoV = 0.14f;
    //cam->camLat = 41.0f;
    //cam->camLon = -86.0f;
    //cam->camDst = 20.0f;
    app.currentCam = cam;
    //cam->setLatLonFovDist(41.0f, -86.0f, 0.14f, 20.0f);
    cam->setLatLonFovDist(0.03f, 140.7f, 21.0f, 5.612f);
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene 1");

    //std::string fontname = "arialbd";
    //std::string fontname = "CourierNew";
    std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);

    app.basefname = "SIO ";

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    unsigned int lg = earth->addLocGroup();
    earth->w_refract = true;
    earth->w_sinsol = true;
    earth->w_twilight = false;
    earth->flatsunheight = 0.0f;

    earth->addSubsolarPoint();
    earth->addSublunarPoint();

    glm::vec4 locskycolor = LIGHT_BLUE;
    locskycolor.a = 0.30f;
    float locskyradius = 0.3f;
    double loclat = 0.0; // l_kabul.lat;
    double loclon = 0.0; // l_kabul.lon;
    Location* myloc = earth->locgroups[lg]->addLocation(loclat, loclon, false, locskyradius);

    earth->addGrid(30);
    //earth->addSubsolarPoint(); // Sun at Earth::flatsunheight
    //earth->addSublunarPoint();

    app.sio_dip = true;
    app.sio_refract = true;
    app.sio_sunlimb = true;
    app.sio_refmethod = 1;
    app.sio_temperature = -4.6f;
    app.sio_pressure = 1055.0f;
    float pathwidth = 0.0002f;
    double mydip = earth->calcHorizonDip(app.sio_height); // in arcmins
    double dip = app.sio_dip ? mydip : 0.0;
    double sunelev = 0.0, sunrefraction = 0.0, sunsemidia = 0.0;

    astro->setTime(2022, 2, 9.0, 14.0, 15.0, 56.0); // 36.716);
    sunelev = 11 * 60.0 + 25.0; // 11°25.0' in arcmins
    sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0, app.sio_pressure, app.sio_temperature) : 0.0;
    sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() /* use current jd*/ : 0.0;
    //std::cout << "\nMeasured Sun elevation, lower limb: " << sunelev << "\'\n";
    //std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    //std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    //std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    //std::cout << "Sun parallax: ignored\n";
    //std::cout << "Sun augmentation of diameter: ignored\n";
    //std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction - sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?
    LLH subsol = earth->getSubsolar(astro->getJD_TT(), false);
    //std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    //unsigned int sun1 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction - sunsemidia) / 60.0, false, LIGHT_YELLOW, pathwidth);

    //Example Location 
    //astro->setTime(2021, 12, 17, 0, 17.0, 33.0, 27.0);
    //double vega_ele = dms2deg(28.0, 53.0, 12.4);
    //vega_ele -= earth->calcRefractionBennett(vega_ele, 10.0, 1010.0);
    //LLH vega_radec = { 38.7836889444, 279.2347347917, 0.0 };
    //double deneb_ele = dms2deg(50.0, 49.0, 08.3);
    //deneb_ele -= earth->calcRefractionBennett(deneb_ele, 10.0, 1010.0);
    //LLH deneb_radec = { 45.2803388056, 310.3579797500, 0.0 };
    //double polaris_ele = dms2deg(52.0, 46.0, 13.7);
    //polaris_ele -= earth->calcRefractionBennett(polaris_ele, 10.0, 1010.0);
    //LLH polaris_radec = { 89.2641089722, 37.9545606667,0.0 };
    //
    //LLH vega_gp = earth->calcRADec2LatLon(vega_radec, false);
    //unsigned int vega = earth->addSumnerLine(vega_gp, vega_ele, false);
    //unsigned int deneb = earth->addSumnerLine(earth->calcRADec2LatLon(deneb_radec, false), deneb_ele, false);
    //LLH polaris_gp = earth->calcRADec2LatLon(polaris_radec, false);
    //unsigned int polaris = earth->addSumnerLine(polaris_gp, polaris_ele, false);

    pathwidth = 0.001f;
    bool do_refraction = false;
    double shiftseconds = 0.0;

    //Example #1 
    // Time     2021-12-17 10:35:27 UTC
    // Alkaid   56°07’03.3”
    // Capella  33°42’42.5”
    // Alphard  38°05’46.3”
    //astro->setTime(2021, 12, 17.0, 10.0, 35.0, 27.0 + shiftseconds);
    //std::string star1_name = "Alkaid";
    //double star1_ele = dms2deg(56.0, 7.0, 3.3);
    //if (do_refraction) star1_ele -= earth->calcRefractionBennett(star1_ele, 10.0, 1013.0);
    //unsigned int star1 = earth->addSextantMeasurement(star1_name, star1_ele, NO_DOUBLE, RED, pathwidth, false);
    //std::string star2_name = "Capella";
    //double star2_ele = dms2deg(33.0, 42.0, 42.5);
    //if (do_refraction) star2_ele -= earth->calcRefractionBennett(star2_ele, 10.0, 1013.0);
    //unsigned int star2 = earth->addSextantMeasurement(star2_name, star2_ele, NO_DOUBLE, RED, pathwidth, false);
    //std::string star3_name = "Alphard";
    //double star3_ele = dms2deg(38.0, 5.0, 46.3);
    //if (do_refraction) star3_ele -= earth->calcRefractionBennett(star3_ele, 10.0, 1013.0);
    //unsigned int star3 = earth->addSextantMeasurement(star3_name, star3_ele, NO_DOUBLE, RED, pathwidth, false);
    ////Location* fwayne = earth->locgroups[lg]->addLocation(40.6, -85.4, false, 0.2f);
    ////fwayne->addLocDot();
    
    //LLH sloc1 = earth->getSextantLoc(star1);
    //LLH sloc2 = earth->getSextantLoc(star2);
    //Earth::Intersection test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //
    //sloc1 = earth->getSextantLoc(star2);
    //sloc2 = earth->getSextantLoc(star3);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //
    //sloc1 = earth->getSextantLoc(star3);
    //sloc2 = earth->getSextantLoc(star1);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    
    
    //Example #2
    // Betelgeuse 45°12’13.4”
    // Canopus    42°36’17.6”
    // Achernar   46°53’16.4”
    // Time : 2021-12-17 18:18:15 UTC
    //astro->setTime(2021, 12, 17.0, 18.0, 18.0, 15.0 + shiftseconds);
    //
    //star1_name = "Betelgeuse";
    //star1_ele = dms2deg(45.0, 12.0, 13.4);
    //if (do_refraction) star1_ele -= earth->calcRefractionBennett(star1_ele, 10.0, 1013.0);
    //star1 = earth->addSextantMeasurement(star1_name, star1_ele, NO_DOUBLE, GREEN, pathwidth, false);
    //
    //star2_name = "Canopus";
    //star2_ele = dms2deg(42.0, 36.0, 17.6);
    //if (do_refraction) star2_ele -= earth->calcRefractionBennett(star2_ele, 10.0, 1013.0);
    //star2 = earth->addSextantMeasurement(star2_name, star2_ele, NO_DOUBLE, GREEN, pathwidth, false);
    //
    //star3_name = "Achernar";
    //star3_ele = dms2deg(46.0, 53.0, 16.4);
    //if (do_refraction) star3_ele -= earth->calcRefractionBennett(star3_ele, 10.0, 1013.0);
    //star3 = earth->addSextantMeasurement(star3_name, star3_ele, NO_DOUBLE, GREEN, pathwidth, false);
    //
    //sloc1 = earth->getSextantLoc(star2);
    //sloc2 = earth->getSextantLoc(star1);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //earth->addDot((float)test.point1.lat, (float)test.point1.lon, 0.0f, pathwidth * 2.0f, ORANGE, true);
    //
    //sloc1 = earth->getSextantLoc(star3);
    //sloc2 = earth->getSextantLoc(star2);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //earth->addDot((float)test.point1.lat, (float)test.point1.lon, 0.0f, pathwidth * 2.0f, ORANGE, true);
    //
    //sloc1 = earth->getSextantLoc(star3);
    //sloc2 = earth->getSextantLoc(star1);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //earth->addDot((float)test.point1.lat, (float)test.point1.lon, 0.0f, pathwidth * 2.0f, ORANGE, true);
    
    
    //Example #3
    // Diphda     47°39’43.8”
    // Hamal      84°45’21.7”
    // Alpheratz  66°42’57.8”
    // Time  2021-08-12 17:18:15 UTC <- the video says 18:18:15, but that was a mistake on my part!
    //astro->setTime(2021, 8, 12.0, 17.0, 18.0, 15.0 + shiftseconds);
    //
    //star1_name = "Diphda";
    //star1_ele = dms2deg(47.0, 39.0, 43.8);
    //if (do_refraction) star1_ele -= earth->calcRefractionBennett(star1_ele, 10.0, 1013.0) / 60.0;
    //star1 = earth->addSextantMeasurement(star1_name, star1_ele, NO_DOUBLE, BLUE, pathwidth, false);
    //
    //star2_name = "Hamal";
    //star2_ele = dms2deg(84.0, 45.0, 21.7);
    //if (do_refraction) star2_ele -= earth->calcRefractionBennett(star2_ele, 10.0, 1013.0) / 60.0;
    //star2 = earth->addSextantMeasurement(star2_name, star2_ele, NO_DOUBLE, BLUE, pathwidth, false);
    //
    //star3_name = "Alpheratz";
    //star3_ele = dms2deg(66.0, 42.0, 57.8);
    //if (do_refraction) star3_ele -= earth->calcRefractionBennett(star3_ele, 10.0, 1013.0) / 60.0; // Stellarium does not apply refraction
    //star3 = earth->addSextantMeasurement(star3_name, star3_ele, NO_DOUBLE, BLUE, pathwidth, false);
    //
    //sloc1 = earth->getSextantLoc(star1);
    //sloc2 = earth->getSextantLoc(star2);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //
    //sloc1 = earth->getSextantLoc(star2);
    //sloc2 = earth->getSextantLoc(star3);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //
    //sloc1 = earth->getSextantLoc(star3);
    //sloc2 = earth->getSextantLoc(star1);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    
    
    // crowxe counter challenge
    // Peacock  56° 39' 57.3"
    // Acrux    63° 12' 52.1"
    // Achernar 57° 06' 1.3"
    // Date	19-Jun-2021
    // Time	23:00:00 UTC
    //astro->setTime(2021, 6, 19.0, 23.0, 0.0, 0.0 + shiftseconds);
    //star1_ele = dms2deg(56.0, 39.0, 57.3);
    //if (do_refraction) star1_ele -= earth->calcRefractionBennett(star1_ele, 10.0, 1013.0);
    //star1 = earth->addSextantMeasurement("Peacock", star1_ele, NO_DOUBLE, LIGHT_ORANGE, pathwidth, false);
    //star2_ele = dms2deg(63.0, 12.0, 52.1);
    //if (do_refraction) star2_ele -= earth->calcRefractionBennett(star2_ele, 10.0, 1013.0);
    //star2 = earth->addSextantMeasurement("Acrux", star2_ele, NO_DOUBLE, LIGHT_ORANGE, pathwidth, false);
    //star3_ele = dms2deg(57.0, 6.0, 1.3);
    //if (do_refraction) star3_ele -= earth->calcRefractionBennett(star3_ele, 10.0, 1013.0);
    //star3 = earth->addSextantMeasurement("Achernar", star3_ele, NO_DOUBLE, LIGHT_ORANGE, pathwidth, false);
    //// South Pole
    //
    //sloc1 = earth->getSextantLoc(star1);
    //sloc2 = earth->getSextantLoc(star2);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //
    //sloc1 = earth->getSextantLoc(star2);
    //sloc2 = earth->getSextantLoc(star3);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //
    //sloc1 = earth->getSextantLoc(star3);
    //sloc2 = earth->getSextantLoc(star1);
    //test = earth->calcSumnerIntersection(sloc1.lat, sloc1.lon, sloc1.dst, sloc2.lat, sloc2.lon, sloc2.dst, true); // Bad: using distance to carry radius
    //std::cout << "Points: " << rad2deg * test.point1.lat << "," << rad2deg * test.point1.lon << " | " << rad2deg * test.point2.lat << "," << rad2deg * test.point2.lon << '\n';
    //earth->addDot((float)test.point1.lat, (float)test.point1.lon, 0.0f, pathwidth * 2.0f, ORANGE, true);

    // addSextantMeasurement() can show a simple circle of equal elevation for the named star.
    // addSubStellarPoint() adds a full object, like a mini-location, that can show radius, center point (GP), and text indicators (eventually)
    // SubStellarPoint is a bit more work to set up, use it when the extra capabilities are needed.
    // Both should have a getLocation() and getRadius() function so an intersection solver can get what it needs.

    //// Testing new SubStellarPoint
    ////SubStellarPoint* ssp1 = new SubStellarPoint(*earth, "Sirius", false);  // Direct instantiation
    //astro->setTime(2021, 8, 12.0, 17.0, 18.0, 15.0 + shiftseconds);
    //SubStellarPoint* ssp1 = earth->addSubStellarPoint("Diphda", false);   // More elegantly via Earth
    //ssp1->setElevation(dms2deg(47.0, 39.0, 43.8), false);
    //ssp1->dot->enable(BLUE, 0.01f);
    //ssp1->sumner->enable(BLUE, 0.003f);
    //ssp1->nametag->enable(font, BLUE, 0.06f);
    //SubStellarPoint* ssp2 = earth->addSubStellarPoint("Hamal", false);
    //ssp2->setElevation(dms2deg(84.0, 45.0, 21.7), false);
    //ssp2->dot->enable(BLUE, 0.01f);
    //ssp2->sumner->enable(BLUE, 0.003f);
    //ssp2->nametag->enable(font, BLUE, 0.06f);
    //SubStellarPoint* ssp3 = earth->addSubStellarPoint("Alpheratz", false);
    //ssp3->setElevation(dms2deg(66.0, 42.0, 57.8), false);
    //ssp3->dot->enable(BLUE, 0.01f);
    //ssp3->sumner->enable(BLUE, 0.003f);
    //ssp3->nametag->enable(font, BLUE, 0.06f);
    //
    //Earth::Intersection isect = earth->calcSumnerIntersection(ssp1->getDetails(true), ssp2->getDetails(true), true);
    //std::cout << "Points: " << rad2deg * isect.point1.lat << "," << rad2deg * isect.point1.lon << " | " << rad2deg * isect.point2.lat << "," << rad2deg * isect.point2.lon << '\n';
    //ssp1->radius->enable();
    ////ssp1->radius->changeBearing(0.3822, true);
    //ssp1->radius->changeBearingTo(isect.point1, true);
    ////earth->addDot(isect.point1.lat, isect.point1.lon, 0.0, 0.01f, LIGHT_BLUE, true);
    //isect = earth->calcSumnerIntersection(ssp2->getDetails(true), ssp3->getDetails(true), true);
    //std::cout << "Points: " << rad2deg * isect.point1.lat << "," << rad2deg * isect.point1.lon << " | " << rad2deg * isect.point2.lat << "," << rad2deg * isect.point2.lon << '\n';
    //ssp2->radius->enable();
    //ssp2->radius->changeBearingTo(isect.point1, true);
    //isect = earth->calcSumnerIntersection(ssp3->getDetails(true), ssp1->getDetails(true), true);
    //std::cout << "Points: " << rad2deg * isect.point1.lat << "," << rad2deg * isect.point1.lon << " | " << rad2deg * isect.point2.lat << "," << rad2deg * isect.point2.lon << '\n';
    //ssp3->radius->enable();
    //ssp3->radius->changeBearingTo(isect.point1, true);

    // Test new SubPointSolver - Example 1
    //SubPointSolver* solver1 = new SubPointSolver(earth);
    //astro->setTime(2021, 12, 17.0, 10.0, 35.0, 27.0);
    //solver1->addSubStellarPoint("Alkaid", dms2deg(56.0, 7.0, 3.3));
    //solver1->addSubStellarPoint("Capella", dms2deg(33.0, 42.0, 42.5));
    //solver1->addSubStellarPoint("Alphard", dms2deg(38.0, 5.0, 46.3));
    //solver1->showSumnerLines(RED, 0.001f);
    //solver1->showNames(font, LIGHT_RED, 0.06f);
    //solver1->showDots(NO_COLOR, 0.02f);
    //LLH point1 = solver1->calcLocation(false);
    //std::cout << "SubPointSolver Ex 1: " << point1.lat << ", " << point1.lon << '\n';
    //earth->addDot(point1.lat, point1.lon, 0.0, 0.002f, LIGHT_GREEN, false);

    // Test new SubPointSolver - Example 2
    //SubPointSolver* solver2 = new SubPointSolver(earth);
    //astro->setTime(2021, 12, 17.0, 18.0, 18.0, 15.0);
    //solver2->addSubStellarPoint("Betelgeuse", dms2deg(45.0, 12.0, 13.4));
    //solver2->addSubStellarPoint("Canopus", dms2deg(42.0, 36.0, 17.6));
    //solver2->addSubStellarPoint("Achernar", dms2deg(46.0, 53.0, 16.4));
    //solver2->showSumnerLines(GREEN, 0.001f);
    //solver2->showNames(font, LIGHT_GREEN, 0.06f);
    //solver2->showDots(NO_COLOR, 0.02f);
    //LLH point2 = solver2->calcLocation(false);
    //std::cout << "SubPointSolver Ex 2: " << point2.lat << ", " << point2.lon << '\n';
    //earth->addDot(point2.lat, point2.lon, 0.0, 0.002f, LIGHT_BLUE, false);

    // Test new SubPointSolver - Example 3
    //SubPointSolver* solver3 = new SubPointSolver(earth);
    //astro->setTime(2021, 8, 12.0, 17.0, 18.0, 15.0);
    //solver3->addSubStellarPoint("Diphda", dms2deg(47.0, 39.0, 43.8));
    //solver3->addSubStellarPoint("Hamal", dms2deg(84.0, 45.0, 21.7));
    //solver3->addSubStellarPoint("Alpheratz", dms2deg(66.0, 42.0, 57.8));
    //solver3->showSumnerLines(BLUE, 0.001f);
    //solver3->showNames(font, LIGHT_BLUE, 0.06f);
    //solver3->showDots(NO_COLOR, 0.02f);
    //LLH point3 = solver3->calcLocation(false);
    //std::cout << "SubPointSolver Ex 3: " << point3.lat << ", " << point3.lon << '\n';
    //earth->addDot(point3.lat, point3.lon, 0.0, 0.002f, LIGHT_RED, false);

    // Test new SubPointSolver - crowxe
    //SubPointSolver* solver4 = new SubPointSolver(earth);
    //astro->setTime(2021, 6, 19.0, 23.0, 0.0, 0.0);
    //solver4->addSubStellarPoint("Peacock", dms2deg(56.0, 39.0, 57.3));
    //solver4->addSubStellarPoint("Acrux", dms2deg(63.0, 12.0, 52.1));
    //solver4->addSubStellarPoint("Achernar", dms2deg(57.0, 6.0, 1.3));
    //solver4->showSumnerLines(ORANGE, 0.001f);
    //solver4->showNames(font, LIGHT_ORANGE, 0.06f);
    //solver4->showDots(NO_COLOR, 0.02f);
    //LLH point4 = solver4->calcLocation(false);
    //std::cout << "SubPointSolver crowxe: " << point4.lat << ", " << point4.lon << '\n';
    //earth->addDot(point4.lat, point4.lon, 0.0, 0.002f, YELLOW, false);

    //cam->setLatLon((float)point.lat, (float)point.lon);

    
    //SubStellarPoint* ssp = earth->addSubStellarPoint("DEBUG!", true);
    //ssp->setElevation(45.0, false);
    //ssp->dot->enable(ORANGE, 0.01f);
    //ssp->sumner->enable(ORANGE, 0.003f);
    //ssp->nametag->enable(font, ORANGE, 0.06f);
    //ssp->radius->enable();
    //
    //double mylat = pi2/3.0;
    //double mylon = -pi2;
    //unsigned int dot = earth->addDot(mylat, mylon, 0.0, 0.01, LIGHT_ORANGE, true);
    //Lerper<double>* lon = new Lerper<double>(-pi2, pi2, 360, true);

    //earth->addEcliptic();
    //astro->convertSIMBAD("Test");

    cam->setLatLon(0.03f, 140.7f);

    for (double elev = 0.0; elev <= 90.0; elev += 1.0) {
        std::cout << "Polaris elevation: " << elev << ", Globe distance: " << (90.0 - elev) * 69.1 << ", Leake adjusted distance: " << earth->calcBrianLeakeDistance(elev, false) << ", Diff: " << (90.0 - elev) * 69.1 - earth->calcBrianLeakeDistance(elev, false) << '\n';
        //std::cout << "Polaris elevation complement: " << elev << ", Globe distance: " << elev * 69.1 << ", Leake angle: " << earth->calcBrianLeakeAngle(elev, false) << '\n';
    }


    app.anim = false;
    app.renderoutput = false;
    while (!glfwWindowShouldClose(app.window)) {
        app.update();
        //ssp1->update();
        //ssp2->update();
        //ssp3->update();
        //solver1->update();
        //solver2->update();
        //solver3->update();
        //solver4->update();
        if (app.anim) {
            //mylon = lon->getNext();
            //earth->changeDotLoc(dot, mylat, mylon, 0.0, true);
            //ssp->radius->changeBearingTo({ mylat, mylon, 0.0 }, true);
            //astro->addTime(0.0, 0.0, 1.0, 0.0);
            astro->setTimeNow();
            //astro->addTime(0.0, 0.0, 0.0, -1030.0);
        }
        scene->w_camera->update();
        app.render();
    }
}

void SandboxSIO(Application& app) { // https://www.youtube.com/watch?v=Sq1V98vkfQw&ab_channel=ScienceItOut
    // Set up required components
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2019, 3, 21.0, 7.0, 30.0, 34.0);
    astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0); // Set time to 12:00 local solar
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    cam->camFoV = 0.4f;
    cam->camLat = 65.0f;
    cam->camLon = -18.0f;
    cam->camDst = 20.0f;
    app.currentCam = cam;
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene 1");

    std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);

    app.basefname = "SIO ";

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    unsigned int lg = earth->addLocGroup();
    earth->w_refract = false;
    earth->w_sinsol = false;
    earth->w_twilight = false;
    earth->flatsunheight = 0.0f;

    glm::vec4 locskycolor = LIGHT_BLUE;
    locskycolor.a = 0.30f;
    float locskyradius = 0.3f;
    double loclat = l_kabul.lat;
    double loclon = l_kabul.lon;
    earth->locgroups[lg]->addLocation(loclat, loclon, false, locskyradius);

    earth->addGrid();
    earth->addSubsolarPoint(); // Sun at flatsunheight
    earth->addSubsolarPole();
    earth->addSubsolarDot();   // Simple Dot at ground level

    earth->addSublunarPoint();

    app.sio_dip = false;
    app.sio_refract = false;
    app.sio_sunlimb = true;
    app.sio_local_weather = false;
    app.sio_refmethod = 1;
    app.sio_temperature = 10.0f;
    app.sio_pressure = 1010.0f;
    app.sio_pathwidth = 0.0005f;
    double mydip = earth->calcHorizonDip(app.sio_height); // in arcmins
    double dip = app.sio_dip ? mydip : 0.0;
    double sunelev = 0.0, sunrefraction = 0.0, sunsemidia = 0.0;
    double temperature = app.sio_temperature, pressure = app.sio_pressure;

    astro->setTime(2022, 2, 9.0, 14.0, 15.0, 56.0); // 36.716);
    sunelev = 11 * 60.0 + 25.0; // 11°25.0' in arcmins
    temperature = -3.2;
    pressure = 980.29;
    sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
        app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
    sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
    std::cout << "\nMeasured Sun elevation, lower limb: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    std::cout << "Sun parallax: ignored\n";
    std::cout << "Sun augmentation of diameter: ignored\n";
    std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction + sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?
    LLH subsol = earth->getSubsolar(astro->getJD_TT(), false);
    std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    size_t sun1 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, LIGHT_YELLOW, app.sio_pathwidth);

    astro->setTime(2022, 2, 9.0, 17.0, 3.0, 41.0); // 36.716);
    sunelev = 2 * 60.0 + 8.8; // 11°25.0' in arcmins
    temperature = -2.7;
    pressure = 980.36;
    sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
        app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
    sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
    std::cout << "\nMeasured Sun elevation, lower limb: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    std::cout << "Sun parallax: ignored\n";
    std::cout << "Sun augmentation of diameter: ignored\n";
    std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction + sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?
    subsol = earth->getSubsolar(astro->getJD_TT(), false);
    std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    size_t sun2 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, YELLOW, app.sio_pathwidth);

    astro->setTime(2022, 2, 9.0, 17.0, 10.0, 5.0); // 36.716);
    sunelev = 1 * 60.0 + 45.7; // 11°25.0' in arcmins
    temperature = -2.6;
    pressure = 980.33;
    sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
        app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
    sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
    std::cout << "\nMeasured Sun elevation, lower limb: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    std::cout << "Sun parallax: ignored\n";
    std::cout << "Sun augmentation of diameter: ignored\n";
    std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction + sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?
    subsol = earth->getSubsolar(astro->getJD_TT(), false);
    std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    size_t sun3 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, LIGHT_ORANGE, app.sio_pathwidth);

    astro->setTime(2022, 2, 9.0, 17.0, 13.0, 30.0); // 36.716);
    sunelev = 1 * 60.0 + 26.6; // 11°25.0' in arcmins
    temperature = -1.7;
    pressure = 980.39;
    sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
        app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
    sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
    std::cout << "\nMeasured Sun elevation, lower limb: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    std::cout << "Sun parallax: ignored\n";
    std::cout << "Sun augmentation of diameter: ignored\n";
    std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction + sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?
    subsol = earth->getSubsolar(astro->getJD_TT(), false);
    std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    size_t sun4 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, ORANGE, app.sio_pathwidth);

    astro->setTime(2022, 2, 9.0, 17.0, 16.0, 20.0); // 36.716);
    sunelev = 1 * 60.0 + 14.6; // 11°25.0' in arcmins
    temperature = -1.7;
    pressure = 980.39;
    sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
        app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
    sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
    std::cout << "\nMeasured Sun elevation, lower limb: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    std::cout << "Sun parallax: ignored\n";
    std::cout << "Sun augmentation of diameter: ignored\n";
    std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction + sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?
    subsol = earth->getSubsolar(astro->getJD_TT(), false);
    std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    size_t sun5 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, LIGHT_RED, app.sio_pathwidth);

    astro->setTime(2022, 2, 9.0, 17.0, 21.0, 30.0); // 36.716);
    sunelev = 1 * 60.0 + 16.6; // 11°25.0' in arcmins
    temperature = -1.1;
    pressure = 980.43;
    sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
        app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
    sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
    std::cout << "\nMeasured Sun elevation, upper limb: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    std::cout << "Sun parallax: ignored\n";
    std::cout << "Sun augmentation of diameter: ignored\n";
    std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction - sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?
    subsol = earth->getSubsolar(astro->getJD_TT(), false);
    std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    size_t sun6 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction - sunsemidia) / 60.0, false, RED, app.sio_pathwidth);

    astro->setTime(2022, 2, 9.0, 17.0, 24.0, 25.0); // 36.716);
    sunelev = 0 * 60.0 + 58.4; // 11°25.0' in arcmins
    temperature = -0.6;
    pressure = 980.46;
    sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
        app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
    sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
    std::cout << "\nMeasured Sun elevation, upper limb: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    std::cout << "Sun parallax: ignored\n";
    std::cout << "Sun augmentation of diameter: ignored\n";
    std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction - sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?
    subsol = earth->getSubsolar(astro->getJD_TT(), false);
    std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    size_t sun7 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction - sunsemidia) / 60.0, false, LIGHT_BLUE, app.sio_pathwidth);


    astro->setTime(2022, 2, 9.0, 18.0, 28.0, 23.0); // 36.716);
    double jupelev = 6 * 60.0 + 23.4; // 6°23.4'
    sunelev = 0 * 60.0 + 58.4; // 11°25.0' in arcmins
    temperature = 0.0;
    pressure = 980.94;
    double juprefraction = app.sio_refract ? calcRefraction(app, (jupelev - dip) / 60.0,
        app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
    std::cout << "\nMeasured Jupiter elevation: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Jupiter: " << juprefraction << "\'\n";
    std::cout << "Jupiter SemiDiameter: ignored\n";
    std::cout << "Jupiter parallax: ignored\n";
    std::cout << "Jupiter augmentation of diameter: ignored\n";
    std::cout << "Corrected Jupiter elevation: " << jupelev - dip - juprefraction << "\'\n"; // Assume Jupiter is point like
    LLH subjup = earth->calcHADec2LatLon(earth->getPlanet(JUPITER, 0.0, false), false);
    std::cout << "Subjovian: " << subjup.lat << "," << subjup.lon << "\n";
    size_t jup1 = earth->addTissotIndicatrix(subjup, 90.0 - (jupelev - dip - juprefraction) / 60.0, false, LIGHT_GREEN, app.sio_pathwidth);

    app.anim = false;
    app.renderoutput = false;
    while (!glfwWindowShouldClose(app.window)) {
        app.update();
        earth->removeTissotIndicatrix(sun1);
        earth->removeTissotIndicatrix(sun2);
        earth->removeTissotIndicatrix(sun3);
        earth->removeTissotIndicatrix(sun4);
        earth->removeTissotIndicatrix(sun5);
        earth->removeTissotIndicatrix(sun6);
        earth->removeTissotIndicatrix(sun7);
        earth->removeTissotIndicatrix(jup1);

        if (app.anim) astro->addTime(0.0, 0.0, 1.0, 0.0);

        mydip = earth->calcHorizonDip(app.sio_height);
        dip = app.sio_dip ? mydip : 0.0;

        astro->setTime(2022, 2, 9.0, 14.0, 15.0, 56.0); // 36.716);
        sunelev = 11 * 60.0 + 25.0; // 11°25.0' in arcmins
        temperature = -3.2;
        pressure = 980.29;
        sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
            app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
        sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
        LLH subsol = earth->getSubsolar(astro->getJD_TT(), false);
        sun1 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, LIGHT_YELLOW, app.sio_pathwidth);

        astro->setTime(2022, 2, 9.0, 17.0, 3.0, 41.0); // 36.716);
        sunelev = 2 * 60.0 + 8.8; // 11°25.0' in arcmins
        temperature = -2.7;
        pressure = 980.36;
        sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
            app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
        sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
        subsol = earth->getSubsolar(astro->getJD_TT(), false);
        sun2 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, YELLOW, app.sio_pathwidth);

        astro->setTime(2022, 2, 9.0, 17.0, 10.0, 5.0); // 36.716);
        sunelev = 1 * 60.0 + 45.7; // 11°25.0' in arcmins
        temperature = -2.6;
        pressure = 980.33;
        sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
            app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
        sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
        subsol = earth->getSubsolar(astro->getJD_TT(), false);
        sun3 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, LIGHT_ORANGE, app.sio_pathwidth);

        astro->setTime(2022, 2, 9.0, 17.0, 13.0, 30.0); // 36.716);
        sunelev = 1 * 60.0 + 26.6; // 11°25.0' in arcmins
        temperature = -1.7;
        pressure = 980.39;
        sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
            app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
        sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
        subsol = earth->getSubsolar(astro->getJD_TT(), false);
        sun4 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, ORANGE, app.sio_pathwidth);

        astro->setTime(2022, 2, 9.0, 17.0, 16.0, 20.0); // 36.716);
        sunelev = 1 * 60.0 + 14.6; // 11°25.0' in arcmins
        temperature = -1.7;
        pressure = 980.39;
        sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
            app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
        sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
        subsol = earth->getSubsolar(astro->getJD_TT(), false);
        sun5 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, LIGHT_RED, app.sio_pathwidth);

        astro->setTime(2022, 2, 9.0, 17.0, 21.0, 30.0); // 36.716);
        sunelev = 1 * 60.0 + 16.6; // 11°25.0' in arcmins
        temperature = -1.1;
        pressure = 980.43;
        sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
            app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
        sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
        subsol = earth->getSubsolar(astro->getJD_TT(), false);
        sun6 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction - sunsemidia) / 60.0, false, RED, app.sio_pathwidth);

        astro->setTime(2022, 2, 9.0, 17.0, 24.0, 25.0); // 36.716);
        sunelev = 0 * 60.0 + 58.4; // 11°25.0' in arcmins
        temperature = -0.6;
        pressure = 980.46;
        sunrefraction = app.sio_refract ? calcRefraction(app, (sunelev - dip) / 60.0,
            app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
        sunsemidia = app.sio_sunlimb ? earth->calcSunSemiDiameter() : 0.0;
        subsol = earth->getSubsolar(astro->getJD_TT(), false);
        sun7 = earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction - sunsemidia) / 60.0, false, LIGHT_BLUE, app.sio_pathwidth);

        astro->setTime(2022, 2, 9.0, 18.0, 28.0, 23.0); // 36.716);
        jupelev = 6 * 60.0 + 23.4; // 6°23.4'
        temperature = 0.0;
        pressure = 980.94;
        juprefraction = app.sio_refract ? calcRefraction(app, (jupelev - dip) / 60.0,
            app.sio_local_weather ? pressure : app.sio_pressure, app.sio_local_weather ? temperature : app.sio_temperature) : 0.0;
        LLH subjup = earth->calcHADec2LatLon(earth->getPlanet(JUPITER, 0.0, false), false);
        jup1 = earth->addTissotIndicatrix(subjup, 90.0 - (jupelev - dip - juprefraction) / 60.0, false, LIGHT_GREEN, app.sio_pathwidth);

        scene->w_camera->update();
        app.render();
    }
}


void SunSectorSandbox(Application& app) {
    // Set up required components
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2019, 3, 21.0, 7.0, 30.0, 34.0);
    astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0); // Set time to 12:00 local solar
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    //cam->camFoV = 2.0f;
    //cam->camLat = 0.0f;
    cam->camLon = -28.0f;
    //cam->camDst = 20.0f;
    app.currentCam = cam;
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene 1");

    std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);

    app.basefname = "ICSTvsCoreyKell ";

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    unsigned int lg = earth->addLocGroup();
    earth->w_refract = false;
    earth->w_sinsol = false;
    earth->w_twilight = false;
    earth->flatsunheight = 0.0f;

    glm::vec4 locskycolor = LIGHT_BLUE;
    locskycolor.a = 0.30f;
    float locskyradius = 0.3f;
    double loclat = l_kabul.lat;
    double loclon = l_kabul.lon;
    earth->locgroups[lg]->addLocation(loclat, loclon, false, locskyradius);

    // Variables used multiple times during animations - defined here so sequences can be rearranged easily
    unsigned int frames = 0;
    unsigned int i = 0;
    Lerper<float>* lat = nullptr;
    Lerper<float>* lon = nullptr;
    Lerper<double>* latd = nullptr;
    Lerper<double>* lond = nullptr;
    Lerper<double>* hr = nullptr;
    Lerper<double>* doy = nullptr;

    glm::vec4 sunindicators = LIGHT_ORANGE;
    float anglewidth = 0.002f;
    //app.setFullScreen();

    earth->addGrid();
    earth->addSubsolarPoint(); // Sun at flatsunheight
    earth->addSubsolarPole();
    earth->addSubsolarDot();   // Simple Dot at ground level

    //scene->getCountryBordersFactory()->addBorder(*earth, "Afghanistan"); // No worries, Earth is passed by reference :)
    //scene->getCountryBordersFactory()->addBorder(*earth, "Denmark"); // No worries, Earth is passed by reference :)
    //scene->getTimeZonesFactory()->addTimeZone(*earth, "Asia/Kabul");
    size_t longc = earth->addLongitudeCurve(l_kabul.lon, BLUE, 0.005f, false);
    size_t longe = earth->addLongitudeCurve(l_kabul.lon + 45.0, BLUE, 0.005f, false);
    size_t longw = earth->addLongitudeCurve(l_kabul.lon - 45.0, BLUE, 0.005f, false);
    size_t longf = earth->addLongitudeCurve(l_kabul.lon + 90.0, BLUE, 0.005f, false);
    size_t longx = earth->addLongitudeCurve(l_kabul.lon - 90.0, BLUE, 0.005f, false);
    size_t latie = earth->addGreatArc({ 0.0, l_kabul.lon, 0.0 }, { 0.0, l_kabul.lon + 45.0, 0.0 }, BLUE, 0.005f, false);
    size_t latiw = earth->addGreatArc({ 0.0, l_kabul.lon, 0.0 }, { 0.0, l_kabul.lon - 45.0, 0.0 }, BLUE, 0.005f, false);

    astro->setTime(2019, 3, 21.0, 7.0, 30.0, 34.0);

    // std::cout << "\nMonth test: " << tzFile::parseMonth("Aug") << "\n\n";
    
    //astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), -0.1); // Set time to 12:00 local solar
    Location* locplus = earth->locgroups[lg]->addLocation(0.0, l_kabul.lon + 45.0, false, locskyradius);
    locplus->addLocDot();
    locplus->truesun->defaultcolor = sunindicators;
    locplus->truesun->enableArrow3D();
    locplus->truesun->enableElevationAngle(NO_COLOR, anglewidth);
    locplus->truesun->enableEleAngText(font);
    Location* locminus = earth->locgroups[lg]->addLocation(0.0, l_kabul.lon - 45.0, false, locskyradius);
    locminus->addLocDot();
    locminus->truesun->defaultcolor = sunindicators;
    locminus->truesun->enableArrow3D();
    locminus->truesun->enableElevationAngle(NO_COLOR, anglewidth);
    locminus->truesun->enableEleAngText(font);

    Location* kabul = earth->locgroups[lg]->addLocation(l_kabul.lat, l_kabul.lon, false, locskyradius);
    kabul->addLocDot();
    kabul->truesun->defaultcolor = sunindicators;
    kabul->truesun->enableArrow3D();
    kabul->truesun->enableElevationAngle(NO_COLOR, anglewidth);
    kabul->truesun->enableAzimuthAngle(NO_COLOR, anglewidth);
    kabul->truesun->enableEleAngText(font);

    earth->addTerminatorTrueSun(sunindicators, 0.005f);
    earth->addSunSectors(0.005f, sunindicators, 45.0);  // Ugly implementation of updates, but is only used for the Kell's 45 degrees Sector test
    //earth->addSemiTerminator(165.0, false, sunindicators, 0.005f);
    //earth->addSemiTerminator(150.0, false, sunindicators, 0.005f);
    //earth->addSemiTerminator(135.0, false, sunindicators, 0.005f);
    //earth->addSemiTerminator(120.0, false, sunindicators, 0.005f);
    //earth->addSemiTerminator(105.0, false, sunindicators, 0.005f);
    //earth->addSemiTerminator(75.0, false, sunindicators, 0.005f);
    //earth->addSemiTerminator(60.0, false, sunindicators, 0.005f);
    size_t semiterm = earth->addSemiTerminator(45.0, false, SUNCOLOR, 0.005f);
    //earth->addSemiTerminator(30.0, false, sunindicators, 0.005f);
    //earth->addSemiTerminator(15.0, false, sunindicators, 0.005f);

    // Test implementation of PathTracker - IT WORKS!!
    //PathTracker<Location>* trackert = new PathTracker<Location>(locplus, earth->getPath(semiterm), BOUNCE);
    //PathTracker<Location>* trackers = new PathTracker<Location>(locminus, earth->getPath(longw), BOUNCE);
    //for (i = 0; i < 3600; i++) {
    //    trackert->moveNext();
    //    trackers->moveNext();
    //    app.render();
    //}
    //delete trackert;
    //delete trackers;

    astro->setTime(2022, 2, 9.0, 14.0, 15.0, 56.0); // 36.716);
    //astro->dumpCurrentTime();
    app.render();

    astro->setTime(2022, 2, 9.0, 17.0, 3.0, 41.0); // 36.716);

    double height = 50.0; // Observer meters above surface
    double sunelev = 2 * 60.0 + 8.0; // 2°08.8' in arcmins
    double jupelev = 6 * 60.0 + 23.4; // 6°23.4'
    double dip = earth->calcHorizonDip(50.0); // in arcmins
    double sunrefraction = calcRefraction(app, sunelev / 60.0);
    double juprefraction = calcRefraction(app, jupelev / 60.0);
    double sunsemidia = earth->calcSunSemiDiameter();
    std::cout << "Measured Sun elevation, lower limb: " << sunelev << "\'\n";
    std::cout << "Horizon dip at 50m: " << dip << "\'\n";
    std::cout << "Atmospheric refraction for Sun: " << sunrefraction << "\'\n";
    std::cout << "Atmospheric refraction for Jupiter: " << juprefraction << "\'\n";
    std::cout << "Sun SemiDiameter: " << sunsemidia << "\'\n"; // By default it calculates for the current JD, add JD if other is desired
    std::cout << "Sun parallax: ignored\n";
    std::cout << "Sun augmentation of diameter: ignored\n";
    std::cout << "Corrected Sun elevation: " << sunelev - dip - sunrefraction + sunsemidia << "\'\n"; // Measure is lower limb, so add sunsemidia
    std::cout << "Corrected Jupiter elevation: " << jupelev - dip - juprefraction << "\'\n"; // Assume Jupiter is point like

    // Compensate for Sun refraction diameter distortion? I.e. refract the sunsemidia?

    astro->setTime(2022, 2, 9.0, 14.0, 15.0, 56.0); // 36.716);
    LLH subsol = earth->getSubsolar(astro->getJD_TT(), false);
    std::cout << "Subsolar: " << subsol.lat << "," << subsol.lon << "\n";
    earth->addTissotIndicatrix(subsol, 90.0 - (sunelev - dip - sunrefraction + sunsemidia) / 60.0, false, LIGHT_RED, 0.005f);

    astro->setTime(2022, 2, 9.0, 18.0, 28.0, 23.0); // 36.716);
    LLH jupradec = earth->getPlanet(JUPITER, 0.0, false);
    LLH subjup = earth->calcHADec2LatLon(earth->getPlanet(JUPITER,0.0, false), false);
    std::cout << "Jupiter RA, Dec: " << jupradec.lon << ", " << jupradec.lat << "\n";
    std::cout << "Subjovian: " << subjup.lat << "," << subjup.lon << "\n";
    earth->addTissotIndicatrix(subjup, 90.0 - (jupelev - dip - juprefraction) / 60.0, false, LIGHT_GREEN, 0.005f);


    // Current test of Local Time
    //TimeZones* tz = scene->getTimeZonesFactory();
    //std::cout << "Local time: " << tz->getLocalTime("Etc/UTC", { 2021, 6, 21.0, 6.0, 0.0, 0.0, 0 }) << '\n'; // Local time: 2021-06-21 08:00:00 CEST
    //
    //std::cout << "Local time: " << tz->getLocalTime("Europe/Copenhagen", 2021, 6, 21.0, 6.0, 0.0, 0.0) << '\n'; // Local time: 2021-06-21 08:00:00 CEST
    //std::cout << "Local time: " << tz->getLocalTime("Europe/Moscow", 1921, 4, 21.0, 6.0, 0.0, 0.0) << '\n'; // Local time: 1921-04-21 11:00:00 +05
    //std::cout << "Local time: " << tz->getLocalTime("Europe/Dublin", 2021, 6, 21.0, 6.0, 0.0, 0.0) << '\n'; // Local time: 2021-06-21 07:00:00 IST
    //
    ////tz->dumpTimeZoneDetails("America/Menominee");
    //std::cout << "Local time: " << tz->getLocalTime("America/Menominee", 1973, 4, 29.0, 1.0, 0.0, 0.0) << '\n';
    //std::cout << "Local time: " << tz->getLocalTime("America/Menominee", 1973, 4, 28.0, 2.0, 0.0, 0.0) << '\n';
    //
    //
    //DateTime testdt;
    //testdt.year = 1971;
    //testdt.month = 5;
    //testdt.day = 19;
    //std::cout << testdt;





    app.anim = false;
    app.renderoutput = false;
    while (!glfwWindowShouldClose(app.window)) {
        app.update();
        if (app.anim) astro->addTime(0.0, 0.0, 1.0, 0.0);
        scene->w_camera->update();
        app.render();
    }
}


void ICSTvsCoreyKell2(Application* app) {

}

void ICSTvsCoreyKell(Application& app) {
    // Set up required components
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2019, 3, 20.0, 12.0, 0.0, 0.0);
    astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0); // Set time to 12:00 local solar
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene 1");

    std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);

    app.basefname = "ICSTvsCoreyKell ";

    SkySphere* sky = scene->newSkysphere(180, 90, true);

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    unsigned int lg = earth->addLocGroup();
    earth->w_refract = false;
    earth->w_sinsol = false;
    earth->w_twilight = false;
    earth->flatsunheight = 0.0f;

    glm::vec4 locskycolor = LIGHT_BLUE;
    locskycolor.a = 0.30f;
    float locskyradius = 0.3f;
    double loclat = l_kabul.lat;
    double loclon = l_kabul.lon;
    earth->locgroups[lg]->addLocation(loclat, loclon, false, locskyradius);

    // Variables used multiple times during animations - defined here so sequences can be rearranged easily
    unsigned int frames = 0;
    unsigned int i = 0;
    Lerper<float>* lat = nullptr;
    Lerper<float>* lon = nullptr;
    Lerper<double>* latd = nullptr;
    Lerper<double>* lond = nullptr;
    Lerper<double>* hr = nullptr;
    Lerper<double>* doy = nullptr;

    glm::vec4 sunindicators = LIGHT_ORANGE;
    float anglewidth = 0.002f;

    app.setFullScreen();

    //app.renderoutput = true;

    // Dry run to ensure everything above has been through an update cycle (some items initialize with odd values when allocated, to be fixed in future !!!) 
    app.update();
    app.render(); // RenderLayerText does not show the UTC datetime at the top in this frame. Why? !!!

    // -- Sequence 001 - Add basic items such as graticule
    app.incSequence();
    earth->addGrid();
    app.render();

    // -- Sequence 002 - Add Sun indicators
    app.incSequence();
    earth->addSubsolarPoint(); // Sun at flatsunheight
    earth->addSubsolarPole();
    earth->addSubsolarDot();   // Simple Dot at ground level
    app.render();

    // -- Sequence 003 - Orbit Sun around Equator
    app.incSequence();
    frames = 60;
    hr = new Lerper<double>(0.0, 24.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2019, 3, 20.0, 12.0 + hr->getNext(), 0.0, 0.0);
        astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0);
        app.render();
    }
    delete hr;

    app.incSequence(); // Add a still
    app.render();

    // -- Sequence 005 - Move to view from north pole
    app.incSequence();
    frames = 30;
    lat = new Lerper<float>(0.0f, 90.0f, frames, false);
    for (i = 0; i < frames; i++) {
        cam->camLat = lat->getNextSmooth();
        cam->update();
        app.render();
    }
    delete lat;

    app.incSequence(); // Add a still
    app.render();

    // -- Sequence 007 - Draw out the blue sector lines, move the Sun +/- 45 degrees
    app.incSequence();
    size_t longc = earth->addLongitudeCurve(0.0, BLUE, 0.005f, false);
    size_t longe = earth->addLongitudeCurve(0.0 + 45.0, BLUE, 0.005f, false);
    size_t longw = earth->addLongitudeCurve(0.0 - 45.0, BLUE, 0.005f, false);
    size_t longf = earth->addLongitudeCurve(0.0 + 90.0, BLUE, 0.005f, false);
    size_t longx = earth->addLongitudeCurve(0.0 - 90.0, BLUE, 0.005f, false);
    size_t latie = earth->addGreatArc({ 0.0, 0.0, 0.0 }, { 0.0, 45.0, 0.0 }, BLUE, 0.005f, false);
    size_t latiw = earth->addGreatArc({ 0.0, 0.0, 0.0 }, { 0.0, -45.0, 0.0 }, BLUE, 0.005f, false);
    app.render();

    app.incSequence();
    app.render();

    // -- Sequence 009 - Rotate to use screen space better
    app.incSequence();
    frames = 30;
    lon = new Lerper<float>(0.0f, -90.0f, frames, false);
    for (i = 0; i < frames; i++) {
        cam->camLon = lon->getNextSmooth();
        cam->update();
        app.render();
    }
    delete lon;

    app.incSequence();
    app.render();

    // -- Sequence 011 - Add Darkness
    app.incSequence();
    earth->w_sinsol = true;
    //app.render();

    app.incSequence();
    //app.render();

    // -- Sequence 013 - Add Twilight
    app.incSequence();
    earth->w_twilight = true;
    app.render();

    app.incSequence();
    app.render();

    // -- Sequence 015 - Remove Twilight
    app.incSequence();
    earth->w_twilight = false;
    app.render();

    app.incSequence();
    app.render();

    // -- Sequence 017 - Move Sun +/- 45 degrees
    app.incSequence();
    astro->setTime(2019, 3, 20.0, 12.0, 0.0, 0.0);
    astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), -0.1); // Set time to 12:00 local solar
    Location* locplus = earth->locgroups[lg]->addLocation(0.0, 45.0, false, locskyradius);
    locplus->addLocDot();
    locplus->truesun->defaultcolor = sunindicators;
    locplus->truesun->enableArrow3D();
    locplus->truesun->enableElevationAngle(NO_COLOR, anglewidth);
    locplus->truesun->enableEleAngText(font);
    Location* locminus = earth->locgroups[lg]->addLocation(0.0, -45.0, false, locskyradius);
    locminus->addLocDot();
    locminus->truesun->defaultcolor = sunindicators;
    locminus->truesun->enableArrow3D();
    locminus->truesun->enableElevationAngle(NO_COLOR, anglewidth);
    locminus->truesun->enableEleAngText(font);
    app.render();

    app.incSequence();
    app.render();

    // -- Sequence 019 - Move Sun +/- 45 degrees, zero to plus
    app.incSequence();
    frames = 30;
    hr = new Lerper<double>(0.0, 3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2019, 3, 20.0, 12.0 + hr->getNextSmooth(), 0.0, 0.0);
        astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0);
        app.render();
    }
    delete hr;

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 021 - Move Sun +/- 45 degrees, plus to minus
    app.incSequence();
    frames = 60;
    hr = new Lerper<double>(3.0, -3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2019, 3, 20.0, 12.0 + hr->getNextSmooth(), 0.0, 0.0);
        astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0);
        app.render();
    }
    delete hr;

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 023 - Move Sun +/- 45 degrees, back to zero
    app.incSequence();
    frames = 30;
    hr = new Lerper<double>(-3.0, 0.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2019, 3, 20.0, 12.0 + hr->getNextSmooth(), 0.0, 0.0);
        astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), -0.1);
        app.render();
    }
    delete hr;

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 025 - Establish Kabul as Location
    app.incSequence();
    scene->getCountryBordersFactory()->addBorder(*earth, "Afghanistan"); // No worries, Earth is passed by reference :)

    Location* kabul = earth->locgroups[lg]->addLocation(l_kabul.lat, l_kabul.lon, false, locskyradius);
    kabul->addLocDot();
    kabul->truesun->defaultcolor = sunindicators;
    kabul->truesun->enableArrow3D();
    kabul->truesun->enableElevationAngle(NO_COLOR, anglewidth);
    kabul->truesun->enableEleAngText(font);
    //camLat = 90f 44f
    //camLon = -90f 70f
    frames = 60;
    lat = new Lerper<float>(90.0f, (float)l_kabul.lat, frames, false);
    lon = new Lerper<float>(-90.0f, (float)l_kabul.lon, frames, false);
    for (i = 0; i < frames; i++) {
        cam->camLat = lat->getNextSmooth();
        cam->camLon = lon->getNextSmooth();
        cam->update();
        app.render();
    }

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 027 - Move ground sectors to Kabul
    app.incSequence();
    frames = 30;
    lond = new Lerper<double>(0.0, l_kabul.lon, frames, false);
    double alon = 0;
    for (i = 0; i < frames; i++) {
        alon = lond->getNextSmooth(); // Don't get multiple times, it increments the return value every time
        earth->changeLongitudeCurve(longc, alon, BLUE, 0.005f, false);
        earth->changeLongitudeCurve(longe, alon + 45.0, BLUE, 0.005f, false);
        earth->changeLongitudeCurve(longw, alon - 45.0, BLUE, 0.005f, false);
        earth->changeLongitudeCurve(longf, alon + 90.0, BLUE, 0.005f, false);
        earth->changeLongitudeCurve(longx, alon - 90.0, BLUE, 0.005f, false);
        earth->changeGreatArc(latie, { 0.0, alon, 0.0 }, { 0.0, alon + 45.0, 0.0 }, false);
        earth->changeGreatArc(latiw, { 0.0, alon, 0.0 }, { 0.0, alon - 45.0, 0.0 }, false);
        locplus->moveLoc(0.0, alon + 45.0, false);
        locminus->moveLoc(0.0, alon - 45.0, false);
        app.render();
    }

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 029 - Move Sun to Kabul
    app.incSequence();
    frames = 60;
    hr = new Lerper<double>(0.0, 19.388, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2019, 3, 20.0, 12.0 + hr->getNextSmooth(), 0.0, 0.6);
        astro->addTime(0.0, 0.0, -CAAEquationOfTime::Calculate(astro->getJD_TT(), true), 0.0);
        app.render();
    }
    delete hr;

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 031 - Move Sun +/- 45 degrees, zero to plus
    app.incSequence();
    frames = 30;
    hr = new Lerper<double>(0.0, 3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2019, 3, 21.0, 7.0 + hr->getNextSmooth(), 30.0, 36.7);
        app.render();
    }
    delete hr;

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 033 - Move Sun +/- 45 degrees, plus to minus
    app.incSequence();
    frames = 60;
    hr = new Lerper<double>(3.0, -3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2019, 3, 21.0, 7.0 + hr->getNextSmooth(), 30.0, 36.7);
        app.render();
    }
    delete hr;

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 035 - Move Sun +/- 45 degrees, back to zero
    app.incSequence();
    frames = 30;
    hr = new Lerper<double>(-3.0, 0.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2019, 3, 21.0, 7.0 + hr->getNextSmooth(), 30.0, 36.7);
        app.render();
    }
    delete hr;

    app.incSequence(); // Get a still
    app.render();

    // -- Sequence 037 - Add tropics
    app.incSequence();
    earth->addTropics(0.005f, SUNCOLOR);
    app.render();
    //astro->dumpCurrentTime(37);

    // -- Sequence 038 - Sun analemma over Kabul
    app.incSequence();
    // Not using a Lerper, as EoT seems to need applying every day
    frames = 121;
    for (i = 0; i < frames; i++) {
        astro->addTime(3.0, 0.0, 0.0, 0.0, true); // Advance 3 days for a total of 121 * 3 = 363
        //astro->addTime(1.0, 0.0, 0.0, 0.0, true);
        //astro->addTime(1.0, 0.0, 0.0, 0.0, true);
        app.render();
    }
    astro->setTime(2019, 3, 21.0, 7.0, 30.0, 36.7);  // Fix exact solar noon, actually rewinding a year.
    app.render();
    astro->dumpCurrentTime(38);

    app.incSequence();
    app.render(); // Still for text

    // -- Sequence 040 - Add Sun sectors
    app.incSequence();
    size_t sunterm = earth->addTerminatorTrueSun(sunindicators, 0.005f);
    earth->addSunSectors(0.005f, sunindicators, 45.0);  // Ugly implementation of updates, but is only used for the Kell's 45 degrees Sector test
    app.render();

    app.incSequence();
    app.render();

    // -- Sequence 042 - Analemma again, with the Sun sectors - I am cheating here, by recycling the time period,
    //                                                          but ending May 3rd where Kell measured elevation of 49 degrees at 9:02.
    app.incSequence();
    frames = 135;
    for (i = 0; i <= frames; i++) {
        astro->addTime(3.0, 0.0, 0.0, 0.0, true); // Advance 3 days for a total of 121 * 3 = 363
        app.render();
    }
    astro->setTime(2020, 5, 3.0, 7.0, 20.0, 9.47);  // Fix exact solar noon, actually rewinding a year.
    //astro->dumpCurrentTime(42);
    app.render();

    app.incSequence(); // Still - "Not quite right either"
    app.render();

    // -- Sequence 044 - Adjust for EoT to local solar noon in Kabul
    app.incSequence();
    //frames = 60;
    //hr = new Lerper<double>(337.0, 9.47, frames, false); // Misusing hr here to count seconds raher than hours
    //for (i = 0; i < frames; i++) {
    //    astro->setTime(2020, 5, 3.0, 7.0, 20.0, hr->getNext(), true);
    //    app.render();
    //}
    //delete hr;

    app.incSequence(); // Still - real solar noon at Kabul on 3rd of May 2020: 7:20:9.47 UTC = 11:50:9.47 Kabul local time
    //app.render();

    // Kell measured 49 degrees at 9:02, which is 3 hours from 12:02. But real solar noon at Kabul on that day was 11:50:9.47
    // So first check that his measurement at 9:02 was good.
    // Then check the measure at 8:50:9.47. It will still show excess.

    // -- Sequence 046 - Show 9:02 local, which is 4:32 UTC
    app.incSequence();
    frames = 30;
    hr = new Lerper<double>(0.0, -3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 5, 3.0, 7.0 + hr->getNextSmooth(), 20.0, 9.47);
        app.render();
    }
    delete hr;
    app.incSequence();
    app.render();
    // -- Sequence 048 - Show 9:02 local, which is 4:32 UTC
    app.incSequence();
    frames = 60;
    hr = new Lerper<double>(-3.0, 3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 5, 3.0, 7.0 + hr->getNextSmooth(), 20.0, 9.47);
        app.render();
    }
    delete hr;
    app.incSequence();
    app.render();
    // -- Sequence 050 - Show 9:02 local, which is 4:32 UTC
    app.incSequence();
    frames = 30;
    hr = new Lerper<double>(3.0, 0.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 5, 3.0, 7.0 + hr->getNextSmooth(), 20.0, 9.47);
        app.render();
    }
    delete hr;

    app.incSequence();  // So what do the Sun sectors show? Now sure. Something azimuth related. A misconception of Sun movement.
    app.render();

    // -- Sequence 052 - Show elevations along ground sectors
    app.incSequence();
    Location* locscan = earth->locgroups[lg]->addLocation(0.0, 45.0, false, locskyradius);
    locscan->addLocDot();
    locscan->truesun->defaultcolor = sunindicators;
    locscan->truesun->enableArrow3D();
    locscan->truesun->enableElevationAngle(NO_COLOR, anglewidth);
    locscan->truesun->enableEleAngText(font);
    frames = 180;
    PathTracker<Location>* trackere = new PathTracker<Location>(locscan, earth->getPath(longe), HALT, frames);
    // frames is actually desired steps in the tracked path.
    for (i = 0; i <= frames; i++) {
        trackere->moveNext();
        app.render();
    }
    delete trackere;
    trackere = new PathTracker<Location>(locscan, earth->getPath(longw), BOUNCE, frames);
    // frames is actually desired steps in the tracked path.
    for (i = 0; i < frames; i++) {
        trackere->moveNext(); // Scan forward until just before reversal point...
        //app.render();
    }
    for (i = 0; i <= frames; i++) {
        trackere->moveNext();
        app.render();
    }
    delete trackere;

    app.incSequence();  // So what do the Sun sectors show? Now sure.
    app.render();

    // -- Sequence 054 - Remove Sun Sectors
    app.incSequence();
    //earth->deleteArc(longc);
    //earth->deleteArc(longe);
    //earth->deleteArc(longw);
    //earth->deleteArc(longf);
    //earth->deleteArc(longx);
    //earth->deleteArc(latie);
    //earth->deleteArc(latiw);
    earth->removeSunSectors();
    earth->changeArc(sunterm, SUNCOLOR);
    app.render();

    // -- Sequence 055 - 
    app.incSequence();
    size_t sunsemi = earth->addSemiTerminator(45.0, false, SUNCOLOR, 0.005f);  // Add default values for color and width
    app.render();

    // -- Sequence 056 - Loop the Semi Terminator to show elevations are constantly 45 degrees
    app.incSequence();
    frames = 180;
    trackere = new PathTracker<Location>(locscan, earth->getPathCache(sunsemi), LOOP, frames);
    // frames is actually desired steps in the tracked path.
    for (i = 0; i <= frames + 1; i++) {
        trackere->moveNext();
        app.render();
    }
    delete trackere;

    // Still - Remove locscan, or maybe move it to the back of Earth
    app.incSequence();
    locscan->moveLoc(-15.0, -100.0, false);
    app.render();

    // -- Sequence 058 - Show Sun at +3 hours from Kabul
    app.incSequence();
    frames = 30;
    hr = new Lerper<double>(0.0, +3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 5, 3.0, 7.0 + hr->getNextSmooth(), 20.0, 9.47);
        app.render();
    }
    delete hr;

    // Still 059
    app.incSequence();
    app.render();

    // -- Sequence 060 - Zoom in on Kabul
    app.incSequence();
    frames = 30;
    Lerper<float>* cfov = new Lerper<float>(6.0f, 1.0f, frames, false);
    lat = new Lerper<float>((float)cam->camLat, (float)l_kabul.lat + 5.0f, frames, false);
    for (i = 0; i < frames; i++) {
        cam->camFoV = cfov->getNext();
        cam->camLat = lat->getNextSmooth();
        cam->update();
        app.render();
    }
    delete cfov;
    delete lat;
    astro->dumpCurrentTime(60);
    cam->dumpParameters(60);
    app.incSequence();
    app.render();

    // -- Sequence 062 - Shift Sun another 199.44 seconds to align SemiTerminator exactly with Kabul
    app.incSequence();
    frames = 10;
    hr = new Lerper<double>(0.0, 199.44, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 5, 3.0, 7.0 + 3.0, 20.0, 9.47 + hr->getNextSmooth());
        app.render();
    }
    delete hr;

    // Still 063
    app.incSequence();
    app.render();


    // -- Sequence 064 - Zoom out again
    app.incSequence();
    frames = 30;
    cfov = new Lerper<float>(1.0f, 6.0f, frames, false);
    lat = new Lerper<float>((float)cam->camLat, (float)l_kabul.lat, frames, false);

    for (i = 0; i < frames; i++) {
        cam->camFoV = cfov->getNext();
        cam->camLat = lat->getNextSmooth();
        cam->update();
        app.render();
    }
    delete cfov;

    app.incSequence();
    app.render();

    // -- Sequence 066 - 3 hours and 199.44 seconds back to Kabul solar noon
    app.incSequence();
    frames = 30;
    hr = new Lerper<double>(3.0, 0.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 5, 3.0, 7.0 + hr->getNextSmooth(), 20.0, 9.47); // Why use EoT here? No! That is Gregorian!
        app.render();
    }
    delete hr;

    app.incSequence();
    app.render();

    // -- Sequence 068 - Advance by solar days (eot) to june solstice
    app.incSequence();
    frames = 49;
    for (i = 0; i < frames; i++) {
        astro->addTime(1.0, 0.0, 0.0, 0.0, true);
        app.render();
    }
    // Time is now 2020-06-21 07:25:9.875 UTC - Kabul solar noon on solstice
    //astro->dumpCurrentTime();

    app.incSequence();  // Highest elevation of the year at Kabul
    app.render();

    // -- Sequence 070 - Check -3 hours
    app.incSequence();
    frames = 60;
    hr = new Lerper<double>(0.0, -3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 6, 21.0, 7.0 + hr->getNextSmooth(), 25.0, 9.875); // Why use EoT here? No! That is Gregorian!
        app.render();
    }
    delete hr;

    app.incSequence();
    app.render();

    // -- Sequence 072 - Do a year at sector, daily with eot - Pause at 2020-08-12, 52 days
    app.incSequence();
    frames = 52;
    for (i = 0; i < frames; i++) {
        astro->addTime(1.0, 0.0, 0.0, 0.0, true);
        app.render();
    }
    // Time is now 2020-08-12 04:28:20.108 UTC - Kabul solar noon on solstice, +3 hours.
    //astro->dumpCurrentTime(1);

    app.incSequence();
    app.render();

    // -- Sequence 074 - Do a year at sector daily with eot - Pause at 2020-08-12 and 2021-04-30
    app.incSequence();
    frames = 261;
    for (i = 0; i < frames; i++) {
        astro->addTime(1.0, 0.0, 0.0, 0.0, true);
        app.render();
    }
    // Time is now 2020-04-30 04:20:36.417 UTC - Kabul solar noon on solstice, +3 hours.
    //astro->dumpCurrentTime(2);

    app.incSequence();
    app.render();

    // -- Sequence 076 - Do a year at sector daily with eot - Pause at 2020-08-12 and 2021-04-30
    app.incSequence();
    frames = 52;
    for (i = 0; i < frames; i++) {
        astro->addTime(1.0, 0.0, 0.0, 0.0, true);
        app.render();
    }
    // Time is now 2021-06-21 04:25:10.325 UTC - Kabul solar noon on solstice, +3 hours.
    //astro->dumpCurrentTime(3);

    app.incSequence();
    app.render();

    // -- Sequence 078 - Move to to noon +3hrs
    app.incSequence();
    frames = 60;
    hr = new Lerper<double>(-3.0, 3.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 6, 21.0, 7.0 + hr->getNextSmooth(), 25.0, 9.875); // Why use EoT here? No! That is Gregorian!
        app.render();
    }
    delete hr;

    app.incSequence();
    app.render();

    // -- Sequence 080 - Year at +3hrs point (while displaying some text)
    app.incSequence();
    frames = 365;
    for (i = 0; i < frames; i++) {
        astro->addTime(1.0, 0.0, 0.0, 0.0, true);
        app.render();
    }
    //astro->dumpCurrentTime(1);

    app.incSequence();
    app.render();

    // -- Sequence 082 - Show that ground sectors are equally wrong - 45 degree circle around Kabul
    earth->addTissotIndicatrix(l_kabul, 45, false, LIGHT_BLUE, 0.005f);

    app.incSequence(); // 083
    app.render();

    // -- Sequence 084 - Year at +3hrs point
    app.incSequence();
    frames = 365;
    for (i = 0; i < frames; i++) {
        astro->addTime(1.0, 0.0, 0.0, 0.0, true);
        app.render();
    }

    app.incSequence();
    app.render();

    // -- Sequence 086 - Sweep +/- 4 hrs to exit Kabul ground circle
    app.incSequence();
    frames = 40;
    hr = new Lerper<double>(3.0, 5.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 6, 21.0, 7.0 + hr->getNextSmooth(), 25.0, 9.875);
        app.render();
    }
    delete hr;

    // -- Sequence 087 - Sweep +/- 4 hrs to exit Kabul ground circle
    app.incSequence();
    frames = 200;
    hr = new Lerper<double>(5.0, -5.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 6, 21.0, 7.0 + hr->getNextSmooth(), 25.0, 9.875);
        app.render();
    }
    delete hr;

    // -- Sequence 088 - Sweep +/- 4 hrs to exit Kabul ground circle
    app.incSequence();
    frames = 100;
    hr = new Lerper<double>(-5.0, 0.0, frames, false);
    for (i = 0; i < frames; i++) {
        astro->setTime(2020, 6, 21.0, 7.0 + hr->getNextSmooth(), 25.0, 9.875);
        app.render();
    }
    delete hr;

    app.incSequence();
    app.render();





    app.anim = false;
    app.renderoutput = false;
    while (!glfwWindowShouldClose(app.window)) {

        app.update();
        if (app.anim) astro->addTime(1.0, 0.0, 0.0, 0.0, true);

        // Do this after updating time, or you obviously get weird lag effects while animating.
        //subsolar = earth->getSubsolar();
        //sunloc->moveLoc(subsolar.lat, subsolar.lon, true);
        //crosspoint->moveLoc((double)app.customparam1, (double)app.customparam2, false);

        //if (app.anim) astro->setTimeNow();
        scene->w_camera->update();

        app.render();
    }
}


void renderFBO(Application& app) {
    // Test Area for implementing render targets
    //  Want: Regular FBO with custom size, cubemap -> equirectangular, with or without saving frames to file
    // Is it flexible enough to simply set a rendertype in Application? 
    // Consider shadows and possible bloom effects.
    // What about GUI, Plot and Text layers?
    
    // - Create a separate FBO for rendering, blit contents to app window, or render a quad with sampler. Save from FBO instead of window.
    // - Handle aspect ratio of FBO vs window
    // - Handle Text and Plot layers

    // Objects might simply have renderCubeMap() and renderProjected() (& renderOrthographic()?) functions? Then shadows can use the
    //  renderCubeMap() function too. What are the differences in the shadows rendering shaders, apart from the rendertarget?

    //app.setupFileRender(1920, 1080, FB_PLAIN);
    //app.renderoutput = true;
    
    // optionally set file basename and sequence/frame numbers
    Astronomy* astro = app.newAstronomy();
    //astro->setTimeNow();
    astro->setTime(2021, 6, 15.0, 9.0, 37.0, 0.0);
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene 1");

    Earth* earth = scene->newEarth("AENS", 180, 90);
    earth->addGrid();
    earth->flatsunheight = 3000.0f;
    earth->addSubsolarPoint();
    earth->addSubsolarPole();
    app.currentEarth = earth;

    //Glyphs* glyphs = scene->getGlyphsFactory();
    ////glyphs->addStartNormalLen(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.0f, 1.0f, LIGHT_RED);
    //std::string fontname = "CascadiaMono"; // Passed by ref, so cannot be Rvalue
    ////std::string fontname = "CourierNew";   // Passed by ref, so cannot be Rvalue
    //glyphs->loadFont(fontname);
    //glm::vec3 cursor = glm::vec3(1.0f, 0.0f, 0.0f); // Passed by reference, so must be L-value
    //glyphs->newGlyph('°', cursor, LIGHT_RED, 0.1f);
    //glyphs->newGlyph('\'', cursor, LIGHT_GREEN, 0.1f);
    //glyphs->newGlyph('\"', cursor, LIGHT_BLUE, 0.1f);
    //std::cout << "Cursor: ";
    //VPRINT(cursor);

    //std::string fontname = "CourierNew";
    std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);

    glm::vec4 locskycolor = LIGHT_BLUE;
    locskycolor.a = 0.30f;
    float locskyradius = 0.2f;
    unsigned int lg = earth->addLocGroup();
    double loclat = 0.0; // l_kabul.lat;
    double loclon = 0.0; // l_kabul.lon;
    earth->locgroups[lg]->addLocation(loclat, loclon, false, locskyradius);
    //earth->locgroups[lg]->addLocation(l_cph.lat, l_cph.lon, false, locskyradius);
    //earth->locgroups[lg]->addLocation(l_tok.lat, l_tok.lon, false, locskyradius);
    //earth->locgroups[lg]->addLocation(l_qqt.lat, l_qqt.lon, false, locskyradius);
    //earth->locgroups[lg]->addLocation(l_perth.lat, l_perth.lon, false, locskyradius);
    //earth->locgroups[lg]->addLocation(0.0, 0.0, false, locskyradius);
    for (auto loc : earth->locgroups[lg]->locations) {
        //loc->addLocSky(locskyradius, locskycolor);
        //loc->flatsun->setGeometry("AE");  // Use "CG" for current geometry, it will then adapt to Earth morph rather than remaining fixed
        //loc->flatsun->enableArrow3D();
        //loc->flatsun->enableDot3D();
        //loc->flatsun->enableLine3D();
        //loc->flatsun->enableElevationAngle();
        //loc->flatsun->enableEleAngText(font);
        //loc->flatsun->enablePath24();
        loc->truesun->enableArrow3D();
        //loc->truesun->enableLine3D();
        loc->truesun->enableElevationAngle();
        loc->truesun->enableEleAngText(font);
        loc->truesun->enablePath24();
    }
    earth->w_refract = false;
    earth->addTerminatorTrueSun(SUNCOLOR, 0.005f);
    earth->addSunSectors(0.005f, LIGHT_ORANGE, 45.0);  // Ugly implementation of updates, but is only used for the Kell's 45 degrees Sector test
    earth->addLongitudeCurve(loclon, BLUE, 0.005f, false);
    earth->addLongitudeCurve(loclon + 45.0, BLUE, 0.005f, false);
    earth->addLongitudeCurve(loclon - 45.0, BLUE, 0.005f, false);
    earth->addLongitudeCurve(loclon + 90.0, BLUE, 0.005f, false);
    earth->addLongitudeCurve(loclon - 90.0, BLUE, 0.005f, false);

    //earth->addTissotIndicatrix({ 90.0, 0.0, 0.0 }, 30.0, false, LIGHT_ORANGE, 0.005f);

    earth->addSemiTerminator(45.0, false, SUNCOLOR, 0.005f);

    Location* sunloc = earth->locgroups[lg]->addLocation(loclat, loclon, false, locskyradius);
    LLH subsolar = earth->getSubsolar(); // Provide JD, or get current Sun
    sunloc->moveLoc(subsolar.lat, subsolar.lon);
    sunloc->addLocDot();
    //sunloc->truesun->enableArrow3D();
    //sunloc->truesun->enableEleAngText(font);

    Location* crosspoint = earth->locgroups[lg]->addLocation(subsolar.lat, subsolar.lon + 45.0, false, locskyradius);
    app.customparam1 = (float)subsolar.lat;
    app.customparam2 = (float)(subsolar.lon + pi4);
    crosspoint->addLocDot();
    //crosspoint->addTangentPlane();
    //crosspoint->truesun->enableArrow3D();
    //crosspoint->truesun->enableElevationAngle();
    //crosspoint->truesun->enableEleAngText(font);

    //earth->addGreatArc({ -81.0, -170.0, 0.0 }, { -81.0, 10.0, 0.0 }, WHITE, 0.003f, false);

    //loc->flatsun->enableAziAngText(font);
    //std::string txt = "Test! abcdef";
    //glm::vec3 pos = glm::vec3(1.0f, 0.0f, 0.0f);
    //glm::vec3 dir = glm::vec3(0.0f, 1.0f, 0.0f);
    //glm::vec3 up = glm::vec3(0.0f, 0.3f, 1.0f);
    //TextFactory* texts = scene->getTextFactory();
    //TextString* testtext = texts->newText(font, txt, 0.1f, LIGHT_YELLOW, pos, dir, up);
    //pos = glm::vec3(0.0f, 0.0f, 1.0f);
    //dir = glm::vec3(1.0f, 0.0f, 0.0f);
    //up = glm::vec3(0.0f, 0.0f, 1.0f);
    //std::string txt2 = "North Pole";
    //TextString* testtext2 = texts->newText(font, txt2, 0.2f, LIGHT_BLUE, pos, dir, up);
    //txt = "Michael :)";
    //testtext->updateText(txt);
    app.update();
    //app.updateView(1920, 1080);
    app.render();
    //for (float p = 0.0f; p < 1.05f; p += 0.05f) { // Fun fact: due to floating point rounding, using p<= 1.0f skips the last step as p is slightly above 1.0f
    //    earth->param = p;
    //    app.render();
    //}

    app.anim = false;
    app.renderoutput = false;
    while (!glfwWindowShouldClose(app.window)) {

        app.update();
        if (app.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        
        // Do this after updating time, or you obviously get weird lag effects while animating.
        subsolar = earth->getSubsolar();
        sunloc->moveLoc(subsolar.lat, subsolar.lon, true);
        crosspoint->moveLoc((double)app.customparam1, (double)app.customparam2, false);

        //if (app.anim) astro->setTimeNow();
        scene->w_camera->update();

        app.render();
    }

}


// Cory Kell 45 degree Sector Test - Kabul
void Sector45_001(Application& myapp) {
    // Simply creates AENS Earth and sets up some basics for viewing Sun/Moon
// 
// Set up environment - Application could setup a default astro and scene with cam for us. Makes no diff., we'd still need to obtain reference to them
    Astronomy* astro = myapp.newAstronomy();
    //astro->setTimeNow();
    //astro->addTime(-1.0, 0.0, 0.0, 0.0);
    //astro->setUnixTime(1639884318.0);
    astro->setTime(2022, 2, 5.0, 8.0, 17.0, 45.0);
    Scene* scene = myapp.newScene();
    Camera* cam = scene->w_camera; // Default camera
    cam->camFoV = 22.8f;
    cam->camLat = 90.0f;
    cam->camLon = 0.0f;
    cam->camDst = 10.0f;
    cam->update();
    myapp.currentCam = cam;          // Enable kbd controls

    RenderLayer3D* layer1 = myapp.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerGUI* gui = myapp.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer1, "Earth Scene");

    RenderLayerTextLines lines;
    RenderLayerText* text = myapp.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, &lines);
    text->setFont(myapp.m_font2);
    text->setAstronomy(astro);  // Probably change the method name !!!

    SkySphere* sky = scene->newSkysphere(180, 90, true);
    sky->addStars();
    Earth* earth = scene->newEarth("AENS", 180, 90);
    earth->addGrid(15.0f);
    //earth->addLongitudeCurve(180.0, WHITE, 0.002f, false);
    //earth->addEquator(0.002f, WHITE);
    earth->flatsunheight = 0.0f; // Used for both Subsolar and Sublunar points
    earth->addSubsolarPoint();
    earth->w_sinsol = true;
    earth->addSublunarPoint();
    //earth->w_linsol = true;
    earth->addTerminatorTrueSun();
    earth->w_twilight = false;
    //earth->addTerminatorTrueMoon();

    unsigned int lgrp = earth->addLocGroup();
    Location* kabul = earth->locgroups[lgrp]->addLocation(l_kabul.lat, l_kabul.lon, false, 0.2f);
    kabul->addLocDot();
    kabul->addLocSky();
    kabul->truesun->enableArrow3D();



    myapp.anim = false;
    while (!glfwWindowShouldClose(myapp.window)) {

        myapp.update();
        if (myapp.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        scene->w_camera->update();

        myapp.render();
    }
}







void CoreyKellDetailed(Application& myapp) {
    // Analysis of Corey Kell's Kabul measurements of the Sun's elevation
    struct datapoints {
        int utime;
        double elevation;
    };
    std::vector<TimePlotData> mydata, mypred;
    mydata.reserve(100);
    mypred.reserve(100);

    std::istringstream parse, dtparse;
    std::string line, item, dtitem;
    double year = 0.0;
    double month = 0.0;
    double day = 0.0;
    double hour = 0.0;
    double minute = 0.0;
    double utcoffset = 0.0;
    double tri = 0.0;
    double ter = 0.0;
    double lat = 0.0;
    double lon = 0.0;
    double elevation = 0.0;

    // Set up environment - Application could set up a default astro and scene with cam for us.
    Astronomy* astro = myapp.newAstronomy();
    astro->setTime(2020, 4, 29.0, 9.0 - 4.5, 2.0, 0.0); // As per flatearthintel.com spreadsheet from Corey
    Scene* scene = myapp.newScene();
    Camera* cam = scene->w_camera; // Default camera
    cam->camFoV = 8.0f;
    cam->camLat = 26.0f;
    cam->camLon = 68.0f;
    cam->camDst = 10.0f;
    cam->update();
    myapp.currentCam = cam;          // Enable kbd controls
    RenderLayer3D* layer1 = myapp.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerGUI* gui = myapp.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer1, "Earth Scene");

    RenderLayerPlot* plot = myapp.newLayerPlot(0.0f, 0.0f, 1.0f, 1.0f); // upper right quarter (Y axis is downwards)
    plot->animateView(0.0f, 0.67f, 1.0f, 1.0f, 60);
    // Test Date and Time to Unix Timestamp conversion
    //std::cout << "Unix timestamp: " << astro->getDateTime2UnixTime(1956, 10, 10, 10, 10, 10) << "\n";

    Font* font = new Font("CascadiaMono");
    glm::vec4 datacolor = LIGHT_ORANGE;
    glm::vec4 predictioncolor = LIGHT_BLUE;

    // Read data file
    //std::cout << "Opening File!\n";
    std::ifstream stream("c:\\flatearthintel.csv");
    getline(stream, line); // Skip headers
    while (getline(stream, line)) {
        //std::cout << line << "\n";
        parse.clear();
        parse.str(line);
        //for (std::string item; std::getline(parse, item, ','); ) {
        //    std::cout << item << '\n';
        //}
        std::getline(parse, item, ','); // Observation number
        //std::cout << "Observation: " << item << "\n";
        //newpoint->obs = std::stoi(item);

        std::getline(parse, item, ','); // Date Time string
        //std::cout << "Date Time string: " << item << "\n";
        dtparse.clear();
        dtparse.str(item);
        std::getline(dtparse, dtitem, '/');  // year
        year = std::stol(dtitem);
        std::getline(dtparse, dtitem, '/');  // month
        month = std::stol(dtitem);
        std::getline(dtparse, dtitem, ' ');  // day
        day = std::stod(dtitem);
        std::getline(dtparse, dtitem, ':');  // hour
        hour = std::stod(dtitem);
        std::getline(dtparse, dtitem);       // minute
        minute = std::stod(dtitem);
        //std::cout << "Read datetime: " << year << month << day << hour << minute << "\n";

        std::getline(parse, item, ',');
        //std::cout << "UTC Offset: " << item << "\n";
        utcoffset = std::stod(item);

        int utime = astro->getDateTime2UnixTime(year, month, day, hour, minute, 0.0);
        utime -= (int)(utcoffset * 3600.0);  // Adjust to UTC after Unix timestamp conversion to avoid issues with day/month/year wrapping
        std::getline(parse, item, ',');
        elevation = std::stod(item);
        //std::cout << "Elevation: " << item << "\n";

        std::getline(parse, item, ',');
        //std::cout << "Azimuth: " << item << "\n";

        std::getline(parse, item, ',');
        tri = std::stod(item);
        //std::cout << "Tripod: " << item << "\n";

        std::getline(parse, item, ',');
        ter = std::stod(item);
        //std::cout << "Terrain: " << item << "\n";

        std::getline(parse, item, ',');
        lat = std::stod(item);
        //std::cout << "Latitude: " << item << "\n";

        std::getline(parse, item, ',');
        lon = std::stod(item);
        //std::cout << "Longitude: " << item << "\n";

        std::getline(parse, item, ',');
        //std::cout << "Claim: " << item << "\n";

        std::getline(parse, item, ',');
        //std::cout << "Eval: " << item << "\n";

        std::getline(parse, item, ',');
        //std::cout << "OS Sun time: " << item << "\n";

        mydata.push_back({ (double)utime, elevation });

        //astro->setTime((long)year, (long)month, day, hour, minute - 1.0, 0.0);
        astro->setUnixTime(utime);
        LLH sunRD = astro->getDecGHA(SUN);
        LLH sunAE = astro->calcGeo2Topo(sunRD, { lat * deg2rad, lon * deg2rad, tri + ter });
        mypred.push_back({ (double)utime, sunAE.lat * rad2deg });
        //std::cout << "Prediction: " << sunAE.lat * rad2deg << "\n";
    }
    //for (auto pt : mydata) { // dump datapoints for validation
    //    std::cout << pt.utime << ", " << pt.data << "\n";
    //}
    mypred.clear(); // Redo mypred to higher precision
    double utstep = 1800.0; // seconds per step, 3600 = 1 hour.
    int predcount = 0;
    std::string timestr;
    for (double ut = 1581073620.0; ut <= 1593340200.0; ut += utstep) {
        astro->setUnixTime(ut);
        //astro->getTimeString(timestr);
        timestr = astro->getTimeString();
        //astro->addTime(0.0, -utcoffset, 0.0, 0.0); // Causes duplicate call to Astronomy::update(), which is slightly slower
        LLH sunRD = astro->getDecGHA(SUN);
        LLH sunAE = astro->calcGeo2Topo(sunRD, { lat * deg2rad, lon * deg2rad, tri + ter });
        //std::cout << timestr << " : " << astro->angle2DMSstring(sunAE.lat * rad2deg, false) << "\n";
        mypred.push_back({ ut, sunAE.lat * rad2deg });
        predcount++;
    }
    //std::cout << "Prediction data points: " << predcount << "\n";  // WARNING: ImPlot uses 16 bit indices by default, so do not exceed 65535 !!!

    //return;

    RenderLayerTextLines* lines = new RenderLayerTextLines();
    char pstring[] = "1234567890123456789012345678901234567890";
    std::string ele_pred_text = "Elevation: ";
    lines->addLine(ele_pred_text);
    char dstring[] = "1234567890123456789012345678901234567890";
    std::string ele_data_text = "Elevation:";
    lines->addLine(ele_data_text);


    RenderLayerText* text = myapp.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, lines);
    text->setFont(myapp.m_font2);
    text->setAstronomy(astro);  // Probably change the method name !!!

    //glm::vec3 dotpos = glm::vec3(0.0f, 0.0f, 0.0f);
    //scene->getDotsFactory()->FromXYZ(dotpos, LIGHT_GREEN, 0.2f);

    //Minifigs* mf = scene->newMinifigs();
    //Arrows* ar = scene->getArrowsFactory();
    //mf->FromStartDirLen(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.1f, 0.1f, LIGHT_RED);
    //mf->FromStartDirLen(glm::vec3(0.0f), glm::vec3(0.0f, 0.1f, 0.9f), 0.1f, 0.1f, LIGHT_YELLOW);
    //ar->FromStartDirLen(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 0.01f, RED);
    //ar->FromStartDirLen(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), 1.0f, 0.01f, GREEN);
    //ar->FromStartDirLen(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, 0.01f, BLUE);

    Earth* earth = scene->newEarth("EX", 180, 90);
    myapp.currentEarth = earth;
    earth->addGrid();
    earth->addEquator();
    earth->addPrimeMeridian();
    earth->w_refract = false;
    earth->addTerminatorTrueSun(SUNCOLOR, 0.005f);
    earth->addSunSectors(0.005f, SUNCOLOR, 45.0);
    earth->addSemiTerminator(45.0, false, SUNCOLOR, 0.005f);

    unsigned int locgroup = earth->addLocGroup();
    // Fayetteville in Sandhills of North Carolina: 35.050458, -78.912176
    Location* loc = earth->locgroups[locgroup]->addLocation(l_kabul.lat, l_kabul.lon, false, 0.2f);
    //loc->addObserver(135.0f, LIGHT_RED, 0.1f);
    loc->addLocDot();
    loc->addTangentPlane(GREEN, 0.5f);
    loc->truesun->enableArrow3D();
    loc->truesun->changeArrow3D(predictioncolor);
    loc->truesun->enablePath24();
    loc->truesun->enableElevationAngle();
    loc->truesun->enableEleAngText(font);
    LLH sunpos = astro->calcGeo2Topo(astro->getDecGHA(SUN, 0.0), { loc->getLat(), loc->getLon(), 1817.522 + 0.5 });

    //Location* loc2 = new Location(earth, l_cph.lat, l_cph.lon, false);
    //loc2->addObserver(270.0f);

    // Animate data in plot
    const double bracket = 7.0 * 86400.0; // 1 week in seconds (for Unix timestamp)

    plot->plotSeries(mydata, datacolor);
    plot->predictSeries(mypred, predictioncolor);
    plot->setTopBottom(-90.0, 90.0);
    plot->setStartEnd(1581073620.0 - bracket, 1593340200.0 + bracket);
    plot->setCurrentTime(1581073620.0);

    myapp.update();
    scene->w_camera->update();
    int pauseframes = 2; // 20;
    int advanceframes = 10; // 60;
    double myutime = 0.0;
    double prevutime = mydata.begin()->utime;
    for (auto& pt : mydata) {
        plot->setStartEnd(pt.utime - bracket, pt.utime + bracket);
        plot->setCurrentTime(pt.utime);
        if (prevutime != pt.utime) {
            Lerper<double>* utimelerp = new Lerper<double>(prevutime, pt.utime, advanceframes, false);
            for (int i = 0; i < advanceframes; i++) { // advance to next datapoint
                myutime = utimelerp->getNextSmooth();
                plot->setStartEnd(myutime - bracket, myutime + bracket);
                plot->setCurrentTime(myutime);
                astro->setUnixTime(myutime);
                scene->w_camera->update();
                myapp.render();
            }
            sprintf_s(dstring, "Measured alt: %02.3f\n", (float)(pt.data));
            ele_pred_text = dstring;

            for (int i = 0; i < pauseframes; i++) { // Pause at datapoint
                myapp.render();
            }
            prevutime = pt.utime;
        }
    }

    plot->interactive = true;  // Enable mouse controls on plot, actually prevent setting the plot limits while drawing.

    layer1->animateView(0.0f, 0.33f, 1.0f, 1.0f, 120);
    //std::cout << "Entering while loop.\n";

    float bearing = 0.0f;
    while (!glfwWindowShouldClose(myapp.window)) {
        //bearing += 1.0f;
        //if (bearing > 360.0f) bearing -= 360.0f;
        //loc->changeObserver(bearing);

        myapp.update();
        if (myapp.anim) astro->addTime(0.0, 0.0, 1.0, 0.0);
        scene->w_camera->update();

        sunpos = astro->calcGeo2Topo(astro->getDecGHA(SUN, 0.0), { loc->getLat(),loc->getLon(), 10000.0 });
        sprintf_s(pstring, "Predicted Elevation: %02.3f\n", (float)(sunpos.lat * rad2deg));
        ele_pred_text = pstring;

        //plot->plotSeries(mydata);
        //plot->predictSeries(mypred);

        myapp.render();
    }

}

// Brian Leake / Flat Earth Tests / 
void Brian_Leake_01(Application & app) {

    const float latlonwidth = 0.002f;

    Astronomy* astro = app.newAstronomy();
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;

    app.basefname = "Brian Leake 01 - ";
    app.renderoutput = false;

    unsigned int i = 0;       // Used in local loops everywhere
    double lat = 0.0;
    double lon = 0.0;

    struct icst {
        double lat;
        double ele;
        double azi;
    };
    std::vector<icst> icsts =
    /* Latitude     Elevation   Azimuth    */
    { {-45.2000000, 21.2429888, 180.0000000},
      {-41.8000000, 23.8059435, 180.0000000},
      {-37.8000000, 28.8022570, 180.0000000},
      {-35.2000000, 30.8081089, 180.0000000},
      {-30.0000000, 36.8698977, 180.0000000},
      {-27.4000000, 39.2373672, 180.0000000},
      {-27.2000000, 40.5391518, 180.0000000},
      { 34.0000000, 80.4186013,   0.0000000},
      { 34.0000000, 81.4225444,   0.0000000},
      { 34.1000000, 79.6738896,   0.0000000},
      { 37.0000000, 76.9447529,   0.0000000},
      { 37.8000000, 75.9637566,   0.0000000},
      { 42.0000000, 69.0265067,   0.0000000},
      { 44.1000000, 69.7963849,   0.0000000},
      { 44.2000000, 69.3602809,   0.0000000},
      { 45.4000000, 66.8643028,   0.0000000},
      { 45.6000000, 68.4872942,   0.0000000},
      { 47.0000000, 65.2604912,   0.0000000},
      { 50.9000000, 61.3360430,   0.0000000},
      { 52.2000000, 61.0018777,   0.0000000},
      { 52.3000000, 60.9592971,   0.0000000},
      { 52.5000000, 60.7199246,   0.0000000},
      { 53.4000000, 58.8150254,   0.0000000},
      { 53.6000000, 60.2093960,   0.0000000},
      { 53.8000000, 62.8448447,   0.0000000},
      { 59.2000000, 54.2587455,   0.0000000},
      { 59.4000000, 53.7461623,   0.0000000},
      { 61.5000000, 52.3139138,   0.0000000},
      { 61.5000000, 53.7197466,   0.0000000},
      { 66.9000000, 46.6683375,   0.0000000} };

    const unsigned int DATA1 = 1; // Unique id for generic location arrows, in case we need more than one kind

    cam->setLatLonFovDist(30.0f, -45.0f, 6.0f, 25.0f);
    astro->setTime(2022, 3, 5, 12.0, 50.0, 0.0);  // UTC no DST


    // Start building frames and sequences

    earth->addEquator(latlonwidth, RED);
    earth->addPrimeMeridian(latlonwidth, RED);
    earth->addGrid(15.0f, latlonwidth, WHITE, "LALO", /*rad*/false, /*eq*/false, /*pm*/false);
    earth->addTropics(latlonwidth, YELLOW);

    earth->flatsunheight = 5769.5f; // https://youtu.be/IiV7UIR1jxI?t=22
    earth->w_refract = false;
    earth->w_sinsol = true;
    earth->w_twilight = true;

    earth->addSubsolarPoint(); // height is earth.m_flatsunheight

    unsigned int lg = earth->addLocGroup();

    // Read data into Locations while adding true and flat Sun arrows (and a locdot)
    for (auto& d : icsts) {
        earth->locgroups[lg]->addLocation(d.lat, 0.0, false, 0.02f);
        earth->locgroups[lg]->locations.back()->addArrow3DEleAzi(DATA1, d.ele, d.azi, 2.0f, 0.003f, LIGHT_GREEN);
        earth->locgroups[lg]->locations.back()->addLocDot(0.005f, LIGHT_RED);
        earth->locgroups[lg]->locations.back()->truesun->enableArrow3D();
        earth->locgroups[lg]->locations.back()->truesun->changeArrow3D(SUNCOLOR, 2.0f, 0.003f);
        earth->locgroups[lg]->locations.back()->flatsun->enableArrow3D();
        earth->locgroups[lg]->locations.back()->flatsun->changeArrow3D(LIGHT_ORANGE, 2.0f, 0.003f);
    }

    //astro->setTime(2021, 12, 21.0, 0.0, 0.0, 0.0);
    //unsigned int slg = earth->addLocGroup();
    //Location* sydney = earth->locgroups[slg]->addLocation(l_sydny.lat, l_sydny.lon, false, 0.2f);
    //sydney->truesun->enableArrow3D();
    //sydney->addLocDot(0.005f, LIGHT_RED);

    // Animate flatsunheight 0 to 100000 in 1000 frames
    //Lerper<float>* height = new Lerper<float>(0.0f, 100000.0f, 1000, false);
    //for (i = 0; i < 1000; i++) {
    //    earth->flatsunheight = height->getNextSmooth();
    //    app.render();
    //}

    // Drop into experimental environment after render, so next scene can be planned
    app.renderoutput = false;

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        //app.update();
        if (app.anim) {
            astro->setTimeNow();
        }
        app.render();
    }
}


void Parenting_test(Application& app) {

    Astronomy* astro = app.newAstronomy();
    //astro->setTimeNow();
    //for (unsigned int i = 0; i <= 240; i++) {
    //    astro->addTime(0.0, 0.0, 10.0, 0.0, false);
    //    astro->updateTimeString();
    //    std::cout << ": GMST = " << astro->getGsid() << '\n';
    //}

    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    cam->setLatLonFovDist(40.0f, -2.0f, 1.4f, 20.0f);
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 0.5f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    float localskyradius = 0.2f;
    
    Earth* earth = scene->newEarth("NSER", 180, 90);
    app.currentEarth = earth;

    earth->w_sinsol = false;
    //earth->addGrid();
    earth->addGreatArc({ 15.0, 170.0, 0.0 }, { 25.0, -10.0, 0.0 }, LIGHT_RED, 0.003f, false);
    earth->flatsunheight = 0.0f;
    earth->addSublunarPoint();
    earth->addTropics();
    unsigned int lg = earth->addLocGroup();
    //double loclatitude = l_cph.lat;
    // Monte Cridola - Bert Rickles lunalemma
    double loclatitude = 46.4648092781465;
    double loclongitude = 12.493651276848878;
    Location* loc = earth->locgroups[lg]->addLocation(loclatitude, loclongitude, false, localskyradius);
    std::string trackstarname = "Alnilam"; // Very near celestial equator: "Sadalmelik"; // Orion's belt west to east: Mintaka, Alnilam and Alnitak
    LLH trackstar = astro->getDecRAbyName(trackstarname);
    glm::vec4 trackcolor = astro->getColorbyName(trackstarname);
    //astro->setTime(2022, 3, 5.0, 12.0, 50.0, 0.0);
    //astro->setTime(2022, 3, 27.0, 17.0, 37.0, 55.0);
    //astro->setTime(2028, 11, 13.0, 19.0, 0.0, 0.0);
    astro->setTime(2000, 1, 1, 7.0, 30.0, 0.0);  // UTC no DST

    loc->addLocDot(0.005f, LIGHT_RED);
    loc->addHorizon(RED, 0.001f);
    //loc->addUpCoord(1.0f);
    //loc->truesun->enableDot3D();
    //loc->addArrow3DRADec(9999, trackstar.lon, trackstar.lat, trackcolor, 0.005f, 1.0f);
    //loc->addTangentPlane(LIGHT_RED, 0.6f);
    //loc->addHorizon(LIGHT_RED, 0.003f);
    //loc->addCircumPolar(GREEN, 0.003f);
    glm::vec4 skycol = BLUE; // BLACK;
    skycol.a = 0.6f;
    loc->addLocSky(localskyradius, skycol);
    //loc->addTruePlanetDot(VENUS, 0.005f, NO_COLOR, false);
    //loc->addTruePlanetDot(MERCURY, 0.005f, NO_COLOR, false);
    //loc->addTruePlanetDot(MARS, 0.005f, NO_COLOR, false);
    //loc->addPlanetTruePathSidYear(VENUS);
    //loc->addPlanetTruePathSidYear(MERCURY);
    //loc->addPlanetTruePathSidYear(MARS);
    loc->addTrueLunalemma(MOONCOLOR, localskyradius/200.0f, localskyradius*tauf/720.0f);
    //Earth2* earth = scene->newEarth2("NSAE", 180, 90);
    //earth->name = "Earth2";
    //app.currentEarth2 = earth;

    SkySphere* sky = scene->newSkysphere(180, 90, true);
    sky->setMode("SE", earth, loc);
    sky->setTexture(false);
    sky->setMovable(true);
    sky->setRadius(localskyradius);
    //sky->addStars(-0.5);
    //sky->addLineStarStar("* alf CMa", "* alf Car");

    //sky->addStarByName(trackstarname);
    //sky->addStarByName("Mintaka");
    //sky->addStarByName("Alnitak");
    //sky->addGrid(0.001f);
    //earth->addSubsolarPoint();
    //earth->addSublunarPoint();
    //earth->addEcliptic();
    //earth->addTropics();
    //unsigned int fpa = earth->addFirstPointAries();

    app.customhigh1 = 90.0f;
    app.customlow1 = -90.0f;
    app.customparam1 = (float)loclatitude;

    app.customhigh2 = 180.0f;
    app.customlow2 = -180.0f;
    app.customparam2 = (float)loclongitude;

    Camera* loccam = scene->newCamera("Location Cam");
    app.locationCam = loccam;
    loccam->setFoV(70.0f);
    loccam->setLookAt(loc->getPosition()*1.01f, loc->getPosition() + loc->getNorth(), loc->getZenith());
    loccam->camNear = 0.0001f;
    loccam->camFar = 100.0f;

    LLH decra = astro->getDecRAbyName("* tet Per", false);
    LLH decrat = astro->getTrueDecRAbyName("* tet Per", false);
    // NOTE: With rad = false, return values are RA in degrees, Dec in degrees
    std::cout << "Star catalogue & true RA(deg), Dec(deg): " << decra.lon << ", " << decra.lat << " " << decrat.lon << ", " << decrat.lat << '\n';

    //loccam->dumpParameters();
    RenderLayer3D* loclayer = app.newLayer3D(0.5f, 0.0f, 1.0f, 1.0f, scene, astro, loccam, false);

    //app.anim = true;
    //app.renderoutput = true;

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    { 
        //app.update();
        if (app.anim) {
            // For satellites, real time -20 minutes
            //astro->setTimeNow();
            //astro->addTime(0.0, 0.0, 0.0, -1200.0, false);
            // For a visually reasonable progression of time in general
            astro->addTime(1.035050, 0.0, 0.0, 0.0, false);
            //astro->addTime(sidereald, 0.0, 0.0, 0.0);
        }
        //earth->updateFirstPointAries(fpa);
        //sky->updateGrid();
        loc->moveLoc((double)app.customparam1, (double)app.customparam2, false);
        sky->updateGrid();
        glm::vec3 lcdir = loc->calcEleAzi2Dir({ loccam->camLat, loccam->camLon, 0.0 }, false);

        loccam->setLookAt(loc->getPosition() * 1.0001f, loc->getPosition() + lcdir, loc->getZenith());
        //loccam->update();
        //std::cout << "Loccam: "; VPRINT(loccam->getPosition());
        //std::cout << " Location: "; VPRINT(loc->getPosition());
        //std::cout << "\n";
        app.render();
    }
}


void AxialPrecession(Application& app) {
    Astronomy* astro = app.newAstronomy();
    //astro->setTimeNow();
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    //cam->setLatLonFovDist(40.0f, -2.0f, 1.4f, 20.0f);
    cam->setLatLonFovDist(-18.0f, -2.0f, 6.43296194076538f, 20.0f);
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    float localskyradius = 0.2f;

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    Arrows* arrowFac = scene->getArrowsFactory();
    float axisWidth = 0.01f;
    size_t axisX = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f }, 1.0f, axisWidth, RED);
    size_t axisY = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, 1.0f, axisWidth, GREEN);
    size_t axisZ = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,1.0f }, 1.0f, axisWidth, BLUE);

    earth->w_sinsol = false;
    earth->addGrid();
    //earth->addGreatArc({ 15.0, 170.0, 0.0 }, { 25.0, -10.0, 0.0 }, LIGHT_RED, 0.003f, false);
    earth->flatsunheight = 0.0f;
    earth->addSublunarPoint();
    earth->addTropics();
    unsigned int lg = earth->addLocGroup();
    //double loclatitude = l_cph.lat;
    // Monte Cridola - Bert Rickles lunalemma
    double loclatitude = 46.4648092781465;
    double loclongitude = 12.493651276848878;
    Location* loc = earth->locgroups[lg]->addLocation(loclatitude, loclongitude, false, localskyradius);
    std::string trackstarname = "Alnilam"; // Very near celestial equator: "Sadalmelik"; // Orion's belt west to east: Mintaka, Alnilam and Alnitak
    LLH trackstar = astro->getDecRAbyName(trackstarname);
    glm::vec4 trackcolor = astro->getColorbyName(trackstarname);
    astro->setTimeNow();
    std::cout << "Earth axial tilt: " << astro->TrueObliquityOfEcliptic(astro->getJD_TT()) << '\n';
    //astro->setTime(2022, 3, 27.0, 17.0, 37.0, 55.0);
    //astro->setTime(2028, 11, 13.0, 19.0, 0.0, 0.0);
    astro->setTime(-3000, 1, 1, 7.0, 30.0, 0.0);  // UTC no DST

    loc->addLocDot(0.005f, LIGHT_RED);
    loc->addHorizon(RED, 0.001f);
    //loc->addUpCoord(1.0f);
    //loc->truesun->enableDot3D();
    //loc->addArrow3DRADec(9999, trackstar.lon, trackstar.lat, trackcolor, 0.005f, 1.0f);
    //loc->addTangentPlane(LIGHT_RED, 0.6f);
    //loc->addHorizon(LIGHT_RED, 0.003f);
    //loc->addCircumPolar(GREEN, 0.003f);
    glm::vec4 skycol = BLACK;
    skycol.a = 0.6f;
    loc->addLocSky(localskyradius, skycol);
    //loc->addTruePlanetDot(VENUS, 0.005f, NO_COLOR, false);
    //loc->addTruePlanetDot(MERCURY, 0.005f, NO_COLOR, false);
    //loc->addTruePlanetDot(MARS, 0.005f, NO_COLOR, false);
    //loc->addPlanetTruePathSidYear(VENUS);
    //loc->addPlanetTruePathSidYear(MERCURY);
    //loc->addPlanetTruePathSidYear(MARS);

    SkySphere* sky = scene->newSkysphere(180, 90, true);
    sky->setMode("SE", earth, loc);
    sky->setTexture(false);
    sky->setMovable(true);
    sky->setRadius(localskyradius);
    sky->addStars(6.0);

    //sky->addStarByName("Polaris");
    //sky->addLineStarStar("* alf CMa", "* alf Car");
    //sky->addStarByName(trackstarname);
    //sky->addStarByName("Mintaka");
    //sky->addStarByName("Alnitak");
    //sky->addGrid(0.001f);
    //earth->addSubsolarPoint();
    //earth->addSublunarPoint();
    //earth->addEcliptic();
    //earth->addTropics();
    //unsigned int fpa = earth->addFirstPointAries();

    app.customhigh1 = 90.0f;
    app.customlow1 = -90.0f;
    app.customparam1 = (float)loclatitude;

    app.customhigh2 = 180.0f;
    app.customlow2 = -180.0f;
    app.customparam2 = (float)loclongitude;

    Camera* loccam = scene->newCamera("Location Cam");
    app.locationCam = loccam;
    loccam->setFoV(70.0f);
    loccam->setLookAt(loc->getPosition() * 1.01f, loc->getPosition() + loc->getNorth(), loc->getZenith());
    loccam->camNear = 0.0001f;
    loccam->camFar = 100.0f;

    LLH decra = astro->getDecRAbyName("* tet Per", false);
    LLH decrat = astro->getTrueDecRAbyName("* tet Per", false);
    // NOTE: With rad = false, return values are RA in degrees, Dec in degrees
    std::cout << "Star catalogue & true RA(deg), Dec(deg): " << decra.lon << ", " << decra.lat << " " << decrat.lon << ", " << decrat.lat << '\n';

    //app.anim = true;
    //app.renderoutput = true;

    astro->setJD_TT(JD2000 - (2500.0 * 365.25));
    std::cout << "Set JD: " << JD2000 - (10000.0*365.25) << ", Got JD: " << astro->getJD_TT() << '\n';

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        //app.update();
        if (app.anim) {
            // For satellites, real time -20 minutes
            //astro->setTimeNow();
            //astro->addTime(0.0, 0.0, 0.0, -1200.0, false);
            // For a visually reasonable progression of time in general
            // Lunar period for LunaLemma
            // astro->addTime(1.035050, 0.0, 0.0, 0.0, false);
            //astro->addTime(sidereald, 0.0, 0.0, 0.0);
            //astro->addTime(365.0, 1.0, 1.0, 0.0);
            astro->setJD_TT(astro->getJD_TT() + 4.0 * 365.25);
        }
        //earth->updateFirstPointAries(fpa);
        //sky->updateGrid();
        loc->moveLoc((double)app.customparam1, (double)app.customparam2, false);
        sky->updateGrid();
        glm::vec3 lcdir = loc->calcEleAzi2Dir({ loccam->camLat, loccam->camLon, 0.0 }, false);

        loccam->setLookAt(loc->getPosition() * 1.0001f, loc->getPosition() + lcdir, loc->getZenith());
        //loccam->update();
        //std::cout << "Loccam: "; VPRINT(loccam->getPosition());
        //std::cout << " Location: "; VPRINT(loc->getPosition());
        //std::cout << "\n";
        app.render();
    }


}


void Lambertian(Application& app) {
    // It is not possible to implement the Lambertian Equal Area mapping with a quad mesh
    // because quads antipodal to the projection point (when proj point is not a pole) are
    // mapped to thin curves along the edge of the map, which will then overlap other parts of the map.
    // It may be possible to solve this using a different mesh, but I can't imagine one that would work
    // with the ER texture. The mesh would have to treat the projection point as a pole, making morphing very difficult too.
    Astronomy* astro = app.newAstronomy();
    astro->setTimeNow();
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;

    cam->setLatLonFovDist(-18.0f, -2.0f, 6.43296194076538f, 20.0f);
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    Earth* earth = scene->newEarth("ERLB", 180, 90);
    app.currentEarth = earth;
    Arrows* arrowFac = scene->getArrowsFactory();
    float axisWidth = 0.01f;
    size_t axisX = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f }, 1.0f, axisWidth, RED);
    size_t axisY = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, 1.0f, axisWidth, GREEN);
    size_t axisZ = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,1.0f }, 1.0f, axisWidth, BLUE);

    earth->w_sinsol = true;
    earth->addGrid();
    earth->addGreatArc({ 90.0-tiny, 180.0, 0.0 }, { -90.0+tiny, 180.0, 0.0 }, LIGHT_RED, 0.003f, false);
    earth->flatsunheight = 0.0f;

    //app.anim = true;
    //app.renderoutput = true;

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        //app.update();
        if (app.anim) {
            //astro->setTimeNow();
        }
        app.render();
    }


}


void SimpleTest(Application& app) {

    Astronomy* astro = app.newAstronomy();
    //astro->setTimeNow();
    astro->setTime(-1, 8, 25, 2, 21, 0);

    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;

    cam->setLatLonFovDist(-18.0f, -2.0f, 6.43296194076538f, 20.0f);
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    Arrows* arrowFac = scene->getArrowsFactory();
    float axisWidth = 0.01f;
    size_t axisX = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f }, 1.0f, axisWidth, RED);
    size_t axisY = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, 1.0f, axisWidth, GREEN);
    size_t axisZ = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,1.0f }, 1.0f, axisWidth, BLUE);

//    Earth2* earth = scene->newEarth2("NSAE", 360, 180);
//    earth->name = "Earth2";
//    app.currentEarth2 = earth;
//
//    Grid grid{ scene, earth, 15.0 * deg2rad };
//
//    SmallCircle sc{ scene, earth, { pi2/2.0, 0.0, 0.0 }, 10 * deg2rad };
//    sc.setParent(earth);

    //Equator2 eq{ scene, earth,0.007f };
    //PrimeMeridian2 pm{ scene,earth,0.007f };
    //
    //std::vector<Latitude2*> lats;
    //for (double lat = -pi2; lat <= pi2; lat += 15 * deg2rad) {
    //    if (abs(lat) > tiny) lats.emplace_back(new Latitude2(scene,earth,lat));
    //}
    //
    //std::vector<Longitude2*> lons;
    //for (double lon = -pi; lon <= pi; lon += 15 * deg2rad) {
    //    if (abs(lon) > tiny) lons.emplace_back(new Longitude2(scene, earth, lon));
    //}

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    //
    //earth->w_sinsol = true;
    earth->addGrid();

    float locskyradius = 0.2f;
    Location* loc = new Location(earth, 45.0, 0.0, false, locskyradius);
    loc->addLocDot();
    loc->addAziEleGrid(15.0, false, 0.002f, LIGHT_RED);
    loc->addTruePlanetDot(MERCURY, 0.007f, MERCURYCOLOR, false);
    loc->addTruePlanetDot(MARS, 0.007f, MARSCOLOR, false);
    loc->addTruePlanetDot(JUPITER, 0.007f, JUPITERCOLOR, false);

    SkySphere* sky = scene->newSkysphere(180, 90, true);
    sky->setMode("SE", earth, loc);
    sky->setTexture(false);
    sky->setMovable(true);
    sky->setRadius(locskyradius);
    sky->addGrid(0.002f);
    //sky->addStars(6.0);

    // Location longitude
    app.customlow1 = -180.0f;
    app.customhigh1 = 180.0f;
    app.customparam1 = 0.0f;
    // Location latitude
    app.customlow2 = -90.0f;
    app.customhigh2 = 90.0f;
    app.customparam2 = 0.0f;

    ////earth->addGreatArc({ 90.0-tiny, 180.0, 0.0 }, { -90.0+tiny, 180.0, 0.0 }, LIGHT_RED, 0.003f, false);
    //earth->flatsunheight = 0.0f;
    //earth->addSubsolarPath();

    //SkySphere* sky = scene->newSkysphere(180, 90, true);
    //sky->setTexture(true);
    //SkyBox* sky = scene->getSkyboxOb();
    //
    //SolarSystem* solsys = scene->getSolsysOb();
    //float planetlinewidth = 0.005f;
    //solsys->AddDistLine(SUN, EARTH, EARTHCOLOR, planetlinewidth);
    //solsys->AddDistLine(SUN, MERCURY, MERCURYCOLOR, planetlinewidth);
    //solsys->AddDistLine(SUN, VENUS, VENUSCOLOR, planetlinewidth);
    //solsys->AddDistLine(SUN, MARS, MARSCOLOR, planetlinewidth);

    //app.anim = true;
    //app.renderoutput = true;

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        app.update();
        if (app.anim) {
            //astro->setTimeNow();
            astro->addTime(1.0, 0.0, 0.0, 0.0, false);
        }
        loc->moveLoc(app.customparam2 * deg2rad, app.customparam1 * deg2rad);
        sky->updateGrid();
        app.render();
    }
}


LLH geocentric2geodesicWGS84(LLH geocentric, bool rad = false) { // Takes height above Earth in km, degrees by default, radians if rad = true
    // Sources: https://nl.mathworks.com/help/aeroblks/geocentrictogeodeticlatitude.html
    //          https://www.osti.gov/servlets/purl/231228
    //          https://www.researchgate.net/publication/233681872_Comparison_of_different_algorithms_to_transform_geocentric_to_geodetic_coordinates/
    double geodLat = rad ? geocentric.lat : geocentric.lat * deg2rad;
    double distance = earthradius + geocentric.dst;
    double rho = distance * cos(geodLat);
    double z = distance * sin(geodLat);

    double mu = 0.0;
    double beta = 0.0;
    double sinBeta = 0.0;
    double cosBeta = 0.0;
    // Initial beta
    beta = atan2(majorAxisWGS84 * z, minorAxisWGS84 * rho);
    sinBeta = sin(beta);
    cosBeta = cos(beta);

    mu = atan2(z + minorAxisWGS84 * ePrimeSquaredWGS84 * sinBeta * sinBeta * sinBeta,
            rho - majorAxisWGS84 * eSquaredWGS84 * cosBeta * cosBeta * cosBeta);
    beta = atan2((1-flatteningWGS84) * sin(mu), cos(mu));
    //sinBeta = sin(beta);
    //cosBeta = cos(beta);
    //mu = atan2(z + minorAxisWGS84 * ePrimeSquaredWGS84 * sinBeta * sinBeta * sinBeta,
    //    rho - majorAxisWGS84 * eSquaredWGS84 * cosBeta * cosBeta * cosBeta);
    //beta = atan2((1 - flatteningWGS84) * sin(mu), cos(mu));
    //sinBeta = sin(beta);
    //cosBeta = cos(beta);
    //mu = atan2(z + minorAxisWGS84 * ePrimeSquaredWGS84 * sinBeta * sinBeta * sinBeta,
    //    rho - majorAxisWGS84 * eSquaredWGS84 * cosBeta * cosBeta * cosBeta);
    //beta = atan2((1 - flatteningWGS84) * sin(mu), cos(mu));
    // Could iterate until mu changes less than epsilon, but source 2 states that one run is accurate for heights less than 400km
    return rad ? LLH{ mu, geocentric.lon, geocentric.dst } : LLH{ mu * rad2deg, geocentric.lon, geocentric.dst };
}

void McToonChallengeV1(Application& app) {
    // Source: https://mctoon.net/global-navigation-challenge/
    // See also: https://www.youtube.com/watch?v=fPN3BgrWaTM&ab_channel=CliveWells
    Astronomy* astro = app.newAstronomy();
    astro->setTimeNow();

    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    //// 29.6757034530339, 172.627471498088
    //cam->setLatLonFovDist(29.6757034530339f, 172.627471498088f, 0.0770187377929688f, 20.0f);
    app.currentCam = cam;

    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    std::string fontname = "arialbd";
    //std::string fontname = "CourierNew";
    //std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);


    Arrows* arrowFac = scene->getArrowsFactory();
    float axisWidth = 0.01f;
    size_t axisX = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f }, 1.0f, axisWidth, RED);
    size_t axisY = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, 1.0f, axisWidth, GREEN);
    size_t axisZ = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,1.0f }, 1.0f, axisWidth, BLUE);

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    //earth->w_sinsol = true;
    earth->addGrid();
    //earth->addTropics();
    //earth->addArcticCirles();


    double observer_height = 384.63; // meters From Exif
    double earth_radius = earthradius * 1000.0; // meters
    double index_error = dms2deg(0.0, 0.0, 0.0);
    double temperature = -8.3;                  // Source: https://www.wunderground.com/history/daily/us/mn/cohasset/KHIB
    double pressure = 983.24;

    SubPointSolver* solver = new SubPointSolver(earth);

    // Set up ThreePointSolver (only supports stars at the moment & assumes all measures are taken at same location)
    ThreePointSolver* tps = scene->newThreePointSolver(earth);
    double seconds_offset = 0.0;
    double elevation_offset = 0.0;
    // Arcturus
    astro->setTime(2022, 3, 28.0, 5.0, 22.0, 30.0 + seconds_offset);  // UTC time is local +5:00
    tps->addSubStellarPoint("Arcturus", dms2deg(46.0 + elevation_offset, 0.0, 0.0));
    // Polaris
    astro->setTime(2022, 3, 28.0, 5.0, 21.0, 45.0 + seconds_offset);
    tps->addSubStellarPoint("Polaris", dms2deg(45.6 + elevation_offset, 0.0, 0.0));
    // Procyon
    astro->setTime(2022, 3, 28.0, 5.0, 19.0, 51.0 + seconds_offset);
    tps->addSubStellarPoint("Procyon", dms2deg(25.2 + elevation_offset, 0.0, 0.0));

    tps->showSumnerLines(LIGHT_ORANGE, 0.0003f);  // 0.0003f is about 1 arc minute wide line or about 1 nautical mile thick
    tps->showNames(font, NO_COLOR, 0.08f);   // LIGHT_ORANGE
    tps->showDots(NO_COLOR, 0.02f);

    // Don't use Dip, the Theodolite iPhone app doesn't measure from apparent horizon, it uses Zenith based on phone gravity sensor
    //tps->setObserverHeight(observer_height);    // meters above MSL (mean sea level)
    tps->setIndexError(index_error);
    tps->setRefraction(temperature, pressure);

    LLH solution = tps->calcLocation(false);  // could pass bools for dip, refraction

    std::cout << "ThreePointSolver McToon location: " << astro->formatLatLon(solution.lat, solution.lon) << "\n";


    //double dip_angle = rad2deg * atan2(sqrt(observer_height * (2.0 * earth_radius + observer_height)), earth_radius);
    double dip_angle = 0.0; // dms2deg(0.0, 1.76 * sqrt(observer_height), 0.0);
    std::cout << " - Index Error: " << astro->angle2DMstring(index_error) << ", - Dip Angle: " << astro->angle2DMstring(dip_angle) << '\n';

    double elev_ssp1 = dms2deg(46.0, 0.0, 0.0);
    elev_ssp1 = elev_ssp1 - index_error - dip_angle;
    std::cout << "Arcturus adjusted measurement: " << Astronomy::angle2DMSstring(elev_ssp1) << "\n";

    double elev_ssp2 = dms2deg(45.6, 0.0, 0.0);
    elev_ssp2 = elev_ssp2 - index_error - dip_angle;
    std::cout << "Polaris adjusted measurement:  " << Astronomy::angle2DMSstring(elev_ssp2) << "\n";

    double elev_ssp3 = dms2deg(25.2, 0.0, 0.0);
    elev_ssp3 = elev_ssp3 - index_error - dip_angle;
    std::cout << "Procyon adjusted measurement:  " << Astronomy::angle2DMSstring(elev_ssp3) << "\n";

    double ref_ssp1 = earth->calcRefractionBennett(elev_ssp1, temperature, pressure);
    double ref_ssp2 = earth->calcRefractionBennett(elev_ssp2, temperature, pressure);
    double ref_ssp3 = earth->calcRefractionBennett(elev_ssp3, temperature, pressure);

    elev_ssp1 -= ref_ssp1;
    elev_ssp2 -= ref_ssp2;
    elev_ssp3 -= ref_ssp3;

    std::cout << "\nRefraction based on " << temperature << " degrees Celsius and " << pressure << " mbar pressure : \n";
    std::cout << " - Arcturus: " << Astronomy::angle2DMSstring(ref_ssp1) << '\n';
    std::cout << " - Polaris:  " << Astronomy::angle2DMSstring(ref_ssp2) << '\n';
    std::cout << " - Procyon:  " << Astronomy::angle2DMSstring(ref_ssp3) << '\n';

    std::cout << "\nFinal True Altitude:\n";
    std::cout << " - Arcturus: " << Astronomy::angle2DMSstring(elev_ssp1) << '\n';
    std::cout << " - Polaris:  " << Astronomy::angle2DMSstring(elev_ssp2) << '\n';
    std::cout << " - Procyon:  " << Astronomy::angle2DMSstring(elev_ssp3) << '\n';

    // Arcturus
    astro->setTime(2022, 3, 28.0, 5.0, 22.0, 30.0);
    double jd_ssp1 = astro->getJD_TT();
    SubStellarPoint* ssp1 = solver->addSubStellarPoint("Arcturus", elev_ssp1, false, jd_ssp1);
    //ssp1->shiftSpeedTime(0.0, 12.0, 4.0); // advance to Dubhe measurement

    // Polaris
    astro->setTime(2022, 3, 28.0, 5.0, 21.0, 45.0);
    double jd_ssp2 = astro->getJD_TT();
    SubStellarPoint* ssp2 = solver->addSubStellarPoint("Polaris", elev_ssp2, false, jd_ssp2);
    //ssp2->shiftSpeedTime(0.0, 12.0, 1.75); // advance to Dubhe measurement

    // Procyon
    astro->setTime(2022, 3, 28.0, 5.0, 19.0, 51.0);
    double jd_ssp3 = astro->getJD_TT();
    SubStellarPoint* ssp3 = solver->addSubStellarPoint("Procyon", elev_ssp3, false, jd_ssp3);
    //ssp3->shiftSpeedTime(0.0, 12.0, 0.0); // advance to Dubhe measurement

    solver->showSumnerLines(NO_COLOR, 0.0003f);  // 0.0003f is about 1 arc minute wide line or about 1 nautical mile thick
    solver->showNames(font, NO_COLOR, 0.08f);  // LIGHT_ORANGE
    solver->showDots(NO_COLOR, 0.02f);

    LLH point = solver->calcLocation(false);
    point.dst = observer_height / 1000.0;  // Use observer height in geodesic calculation, it takes km rather than meters
    LLH geodetic = geocentric2geodesicWGS84(point);
    std::cout << "ThreePointSolver McToon: " << astro->formatLatLon(solution.lat, solution.lon) << "\n";
    std::cout << "\nSubPointSolver McToon:   " << astro->formatLatLon(point.lat, point.lon) << '\n';
    std::cout << "In geodetic coordinates: " << astro->formatLatLon(geodetic.lat, geodetic.lon) << '\n';

    earth->addDot(dms2deg(46.0, 38.0, 36.0), dms2deg(-93.0, 22.0, 31.0), 0.0, 0.0009f, LIGHT_GREEN, false); // McToon's revealed pos from Exif
    earth->addDot(46.7124, -93.2742, 0.0, (6.75f * 0.0003f / 2.0f), YELLOW, false); // Clive Wells calculated pos, revealed accuracy 6.75 miles
    earth->addDot(solution.lat, solution.lon, 0.0, 0.0003f, LIGHT_ORANGE, false); // Calculated pos in ThreePointSolver
    earth->addDot(point.lat, point.lon, 0.0, 0.0003f, LIGHT_RED, false); // Calculated pos in SubPointSover
    earth->addDot(geodetic.lat, geodetic.lon, 0.0, 0.0003f, LIGHT_PURPLE, false); // Calculated pos in SubPointSolver, converted to geodetic

    std::cout << "\nCalculated Azi/Ele at this location: \n";

    // My adaptation of AA+
    astro->setTime(2022, 3, 28.0, 5.0, 22.0, 33.0);
    LLH ssp1_decra = astro->getTrueDecRAbyName("Arcturus", jd_ssp1);
    //LLH ssp1_altaz = astro->calcGeo2Topo(ssp1_decra, point);
    std::cout << " - Arcturus RA/Dec: " << astro->formatDecRA(ssp1_decra.lon, ssp1_decra.lat) << '\n';
    // Actual Catalogue RA/Dec
    LLH reg_cat = astro->getDecRAbyName("Arcturus");
    std::cout << " - Arcturus Catalogue     RA/Dec: " << astro->formatDecRA(reg_cat.lon, reg_cat.lat) << '\n';
    // With Proper Motion applied
    LLH reg_pm = astro->getDecRAwithPMbyName("Arcturus", astro->getJD_TT());
    std::cout << " - Arcturus w/Prop Motion RA/Dec: " << astro->formatDecRA(reg_pm.lon, reg_pm.lat) << '\n';
    // With Precession
    CAA2DCoordinate reg_prec = CAAPrecession::PrecessEquatorial(reg_pm.lon / 15.0, reg_pm.lat, EDateTime::getJDUTC2TT(JD2000), astro->getJD_TT());
    std::cout << " - Arcturus w/Precession  RA/Dec: " << astro->formatDecRA(reg_prec.X * 15.0, reg_prec.Y) << '\n';
    // With Nutation
    double obliq = CAANutation::MeanObliquityOfEcliptic(astro->getJD_TT());
    double nutlon = CAANutation::NutationInLongitude(astro->getJD_TT());
    double nutobl = CAANutation::NutationInObliquity(astro->getJD_TT());
    double reg_nut_ra = CAANutation::NutationInRightAscension(reg_prec.X, reg_prec.Y, obliq, nutlon, nutobl);
    double reg_nut_dec = CAANutation::NutationInDeclination(reg_prec.X, obliq, nutlon, nutobl);
    reg_prec.X += (reg_nut_ra / 3600.0) / 15.0;
    reg_prec.Y += reg_nut_dec / 3600.0;
    std::cout << " - Arcturus w/Nutation    RA/Dec: " << astro->formatDecRA(reg_prec.X * 15.0, reg_prec.Y) << '\n';
    // With Aberration
    CAA2DCoordinate reg_aber = CAAAberration::EquatorialAberration(reg_prec.X, reg_prec.Y, astro->getJD_TT(), true);
    reg_prec.X += reg_aber.X;
    reg_prec.Y += reg_aber.Y;
    std::cout << " - Arcturus w/Aberration  RA/Dec: " << astro->formatDecRA(reg_prec.X * 15.0, reg_prec.Y) << '\n';

    //std::cout << "Acturus Azi/Ele: " << astro->latlonFormat(reg_)
    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        app.update();
        if (app.anim) {
            //astro->setTimeNow();
            //astro->addTime(0.0, 0.0, 0.0, 0.0, false);
        }
        tps->update();
        solver->update();
        //loc->moveLoc(app.customparam2 * deg2rad, app.customparam1 * deg2rad);
        //sky->updateGrid();
        app.render();
    }
}

void NullIslandTest(Application& app) {
    // Source: https://mctoon.net/global-navigation-challenge/
    // See also: https://www.youtube.com/watch?v=fPN3BgrWaTM&ab_channel=CliveWells
    Astronomy* astro = app.newAstronomy();
    astro->setTimeNow();

    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    //// 29.6757034530339, 172.627471498088
    //cam->setLatLonFovDist(29.6757034530339f, 172.627471498088f, 0.0770187377929688f, 20.0f);
    app.currentCam = cam;

    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    std::string fontname = "arialbd";
    //std::string fontname = "CourierNew";
    //std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    //earth->w_sinsol = true;
    earth->addGrid();
    earth->addTropics();
    //earth->addArcticCirles();


    // Test parameters
    astro->setTime(2022, 3, 28.0, 5.0, 22.0, 30.0);
     
    LLH location = LLH{ deg2rad * dms2deg(46.0, 38.0, 36.38), deg2rad * dms2deg(-93.0, 22.0, 31.63), 0.0 };
    std::string star1_name = "Arcturus";
    double star1_ele = 0.0;
    std::string star2_name = "Polaris";
    double star2_ele = 0.0;
    std::string star3_name = "Procyon";
    double star3_ele = 0.0;

    double observer_height = 0.0;
    double index_error = dms2deg(0.0, 0.0, 0.0);
    double temperature = 10.0;                  // Source: https://www.wunderground.com/history/daily/us/mn/cohasset/KHIB
    double pressure = 1010.0;

    std::cout << "Test location: " << astro->formatLatLon(location.lat, location.lon, true) << "\n";

    // Calculate elevations from above parameters
    double gsid = astro->getGsid();
    LLH star1_decra = astro->getTrueDecRAbyName(star1_name, astro->getJD_TT(), true);
    std::cout << "GSID: " << gsid << "\n";
    star1_decra.lon = gsid - star1_decra.lon;
    LLH star1_altaz = astro->calcGeo2Topo(star1_decra, location);
    std::cout << star1_name << " RA/Dec: " << astro->formatDecRA(star1_decra.lat, star1_decra.lon, true) << '\n';
    std::cout << star1_name << " Az/Ele: " << astro->formatEleAz(star1_altaz.lat, star1_altaz.lon, true) << '\n';
    LLH star2_decra = astro->getTrueDecRAbyName(star2_name, astro->getJD_TT(), true);
    star2_decra.lon = gsid - star2_decra.lon;
    LLH star2_altaz = astro->calcGeo2Topo(star2_decra, location);
    std::cout << star2_name << " RA/Dec: " << astro->formatDecRA(star2_decra.lat, star2_decra.lon, true) << '\n';
    std::cout << star2_name << " Az/Ele: " << astro->formatEleAz(star2_altaz.lat, star2_altaz.lon, true) << '\n';
    LLH star3_decra = astro->getTrueDecRAbyName(star3_name, astro->getJD_TT(), true);
    star3_decra.lon = gsid - star3_decra.lon;
    LLH star3_altaz = astro->calcGeo2Topo(star3_decra, location);
    std::cout << star3_name << " RA/Dec: " << astro->formatDecRA(star3_decra.lat, star3_decra.lon, true) << '\n';
    std::cout << star3_name << " Az/Ele: " << astro->formatEleAz(star3_altaz.lat, star3_altaz.lon, true) << '\n';

    // Set up ThreePointSolver (only supports stars at the moment & assumes all measures are taken at same location)
    ThreePointSolver* tps = scene->newThreePointSolver(earth);
    double seconds_offset = 0.0;
    double elevation_offset = 0.0;
    // Star 1
    tps->addSubStellarPoint(star1_name, star1_altaz.lat, true);
    // Star 2
    tps->addSubStellarPoint(star2_name, star2_altaz.lat, true);
    // Star 3
    tps->addSubStellarPoint(star3_name, star3_altaz.lat, true);

    tps->showSumnerLines(NO_COLOR, 0.0003f);  // 0.0003f is about 1 arc minute wide line or about 1 nautical mile thick
    tps->showNames(font, NO_COLOR, 0.08f);
    tps->showDots(NO_COLOR, 0.02f);

    tps->setObserverHeight(observer_height);    // meters above MSL (mean sea level)
    tps->setIndexError(index_error);
    //tps->setRefraction(temperature, pressure);

    LLH solution = tps->calcLocation(false);  // could pass bools for dip, refraction

    std::cout << "ThreePointSolver Null Island: " << astro->formatLatLon(solution.lat, solution.lon) << "\n\n";

 
    // Actual Catalogue RA/Dec
    LLH reg_cat = astro->getDecRAbyName("Arcturus");
    std::cout << " - Arcturus Catalogue     RA/Dec: " << astro->formatDecRA(reg_cat.lon, reg_cat.lat) << '\n';
    // With Proper Motion applied
    LLH reg_pm = astro->getDecRAwithPMbyName("Arcturus", astro->getJD_TT());
    std::cout << " - Arcturus w/Prop Motion RA/Dec: " << astro->formatDecRA(reg_pm.lon, reg_pm.lat) << '\n';
    // With Precession
    //CAA2DCoordinate reg_prec = CAAPrecession::PrecessEquatorial(reg_pm.lon / 15.0, reg_pm.lat, EDateTime::getJDUTC2TT(JD2000), astro->getJD_TT());

    double Alpha = reg_pm.lon / 15.0;
    double Delta = reg_pm.lat;
    double JD0 = EDateTime::getJDUTC2TT(JD2000);
    double JD = astro->getJD_TT();

    const double T{ (JD0 - 2451545) / 36525 };
    const double Tsquared{ T * T };
    const double t{ (JD - JD0) / 36525 };
    const double tsquared{ t * t };
    const double tcubed{ tsquared * t };

    //Now convert to radians
    Alpha = CAACoordinateTransformation::HoursToRadians(Alpha);
    Delta = CAACoordinateTransformation::DegreesToRadians(Delta);
    const double cosDelta{ cos(Delta) };
    const double sinDelta{ sin(Delta) };

    const double sigma{ CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, ((2306.2181 + (1.39656 * T) - (0.000139 * Tsquared)) * t) + ((0.30188 - (0.000344 * T)) * tsquared) + (0.017998 * tcubed))) };
    const double zeta{ CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, ((2306.2181 + (1.39656 * T) - (0.000139 * Tsquared)) * t) + ((1.09468 + (0.000066 * T)) * tsquared) + (0.018203 * tcubed))) };
    const double phi{ CAACoordinateTransformation::DegreesToRadians(CAACoordinateTransformation::DMSToDegrees(0, 0, ((2004.3109 - (0.8533 * T) - (0.000217 * Tsquared)) * t) - ((0.42665 + (0.000217 * T)) * tsquared) - (0.041833 * tcubed))) };
    const double cosphi{ cos(phi) };
    const double sinphi{ sin(phi) };
    const double cosAlphaplussigma{ cos(Alpha + sigma) };
    const double A{ cosDelta * sin(Alpha + sigma) };
    const double B{ (cosphi * cosDelta * cosAlphaplussigma) - (sinphi * sinDelta) };
    const double C{ (sinphi * cosDelta * cosAlphaplussigma) + (cosphi * sinDelta) };
    CAA2DCoordinate reg_prec;
    reg_prec.X = CAACoordinateTransformation::MapTo0To24Range(CAACoordinateTransformation::RadiansToHours(atan2(A, B) + zeta));
    reg_prec.Y = CAACoordinateTransformation::RadiansToDegrees(asin(C));

    std::cout << " - Arcturus w/Precession  RA/Dec: " << astro->formatDecRA(reg_prec.X * 15.0, reg_prec.Y) << '\n';
    // With Nutation
    double obliq = CAANutation::MeanObliquityOfEcliptic(astro->getJD_TT());
    double nutlon = CAANutation::NutationInLongitude(astro->getJD_TT());
    double nutobl = CAANutation::NutationInObliquity(astro->getJD_TT());
    double reg_nut_ra = CAANutation::NutationInRightAscension(reg_prec.X, reg_prec.Y, obliq, nutlon, nutobl);
    double reg_nut_dec = CAANutation::NutationInDeclination(reg_prec.X, obliq, nutlon, nutobl);
    reg_prec.X += (reg_nut_ra / 3600.0) / 15.0;
    reg_prec.Y += reg_nut_dec / 3600.0;
    std::cout << " - Arcturus w/Nutation    RA/Dec: " << astro->formatDecRA(reg_prec.X * 15.0, reg_prec.Y) << '\n';
    // With Aberration
    CAA2DCoordinate reg_aber = CAAAberration::EquatorialAberration(reg_prec.X, reg_prec.Y, astro->getJD_TT(), true);
    reg_prec.X += reg_aber.X;
    reg_prec.Y += reg_aber.Y;
    std::cout << " - Arcturus w/Aberration  RA/Dec: " << astro->formatDecRA(reg_prec.X * 15.0, reg_prec.Y) << '\n';

    //std::cout << "Acturus Azi/Ele: " << astro->latlonFormat(reg_)
    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        app.update();
        if (app.anim) {
            //astro->setTimeNow();
            //astro->addTime(0.0, 0.0, 0.0, 0.0, false);
        }
        tps->update();

        //loc->moveLoc(app.customparam2 * deg2rad, app.customparam1 * deg2rad);
        //sky->updateGrid();
        app.render();
    }
}

void McToonChallengeV2(Application& app) {

    Astronomy* astro = app.newAstronomy();
    astro->setTimeNow();

    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    // 29.6757034530339, 172.627471498088
    cam->setLatLonFovDist(29.6757034530339f, 172.627471498088f, 0.0770187377929688f, 20.0f);
    app.currentCam = cam;

    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    std::string fontname = "arialbd";
    //std::string fontname = "CourierNew";
    //std::string fontname = "CascadiaMono";
    Font* font = new Font(fontname);


    Arrows* arrowFac = scene->getArrowsFactory();
    float axisWidth = 0.01f;
    size_t axisX = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 1.0f,0.0f,0.0f }, 1.0f, axisWidth, RED);
    size_t axisY = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f }, 1.0f, axisWidth, GREEN);
    size_t axisZ = arrowFac->addStartDirLen({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,1.0f }, 1.0f, axisWidth, BLUE);

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    earth->w_sinsol = true;
    earth->addGrid();
    earth->addTropics();
    earth->addArcticCirles();

    SubPointSolver* solver = new SubPointSolver(earth);

    double tmp_out = 0.0, deg_out = 0.0, min_out = 0.0, sec_out = 0.0;

    double observer_height = 2.0;
    double earth_radius = earthradius * 1000.0;  // meters
    double index_error = dms2deg(0.0, 0.3, 0.0);
    //double dip_angle = rad2deg * atan2(sqrt(observer_height * (2.0 * earth_radius + observer_height)), earth_radius);
    double dip_angle = dms2deg(0.0, 1.76 * sqrt(observer_height), 0.0);
    std::cout << " - Index Error: " << astro->angle2DMstring(index_error) << ", - Dip Angle: " << astro->angle2DMstring(dip_angle) << '\n';

    double elev_ssp1 = dms2deg(70.0, 48.7, 0.0);
    elev_ssp1 = elev_ssp1 - index_error - dip_angle;
    std::cout << "Regulus adjusted measurement:  " << astro->angle2DMSstring(elev_ssp1) << "\n";

    double elev_ssp2 = dms2deg(27.0, 9.0, 0.0);
    elev_ssp2 = elev_ssp2 - index_error - dip_angle;
    std::cout << "Arcturus adjusted measurement: " << astro->angle2DMSstring(elev_ssp2) << "\n";

    double elev_ssp3 = dms2deg(55.0, 18.4, 0.0);
    elev_ssp3 = elev_ssp3 - index_error - dip_angle;
    std::cout << "Dubhe adjusted measurement:    " << astro->angle2DMSstring(elev_ssp3) << "\n";

    double ref_ssp1 = earth->calcRefractionBennett(elev_ssp1, 12.0, 975);
    double ref_ssp2 = earth->calcRefractionBennett(elev_ssp2, 12.0, 975);
    double ref_ssp3 = earth->calcRefractionBennett(elev_ssp3, 12.0, 975);

    elev_ssp1 -= ref_ssp1;
    elev_ssp2 -= ref_ssp2;
    elev_ssp3 -= ref_ssp3;

    std::cout << "\nRefraction based on 12 degrees Celsius and 975 milibar pressure:\n";
    std::cout << " - Regulus:  " << astro->angle2DMSstring(ref_ssp1) << '\n';
    std::cout << " - Arcturus: " << astro->angle2DMSstring(ref_ssp2) << '\n';
    std::cout << " - Dubhe:    " << astro->angle2DMSstring(ref_ssp3) << '\n';

    std::cout << "\nFinal True Altitude:\n";
    std::cout << " = Regulus:  " << astro->angle2DMSstring(elev_ssp1) << '\n';
    std::cout << " = Arcturus: " << astro->angle2DMSstring(elev_ssp2) << '\n';
    std::cout << " = Dubhe:    " << astro->angle2DMSstring(elev_ssp3) << '\n';

    // Regulus
    astro->setTime(2018, 11, 15.0, 18.0, 28.0, 15.0);
    double jd_ssp1 = astro->getJD_TT();
    SubStellarPoint* ssp1 = solver->addSubStellarPoint("Regulus", elev_ssp1, false, jd_ssp1);
    //ssp1->shiftSpeedTime(0.0, 12.0, 4.0); // advance to Dubhe measurement

    // Arcturus
    astro->setTime(2018, 11, 15.0, 18.0, 30.0, 30.0);
    double jd_ssp2 = astro->getJD_TT();
    SubStellarPoint* ssp2 = solver->addSubStellarPoint("Arcturus", elev_ssp2, false, jd_ssp2);
    //ssp2->shiftSpeedTime(0.0, 12.0, 1.75); // advance to Dubhe measurement

    // Dubhe
    astro->setTime(2018, 11, 15.0, 18.0, 32.0, 15.0);
    double jd_ssp3 = astro->getJD_TT();
    SubStellarPoint* ssp3 = solver->addSubStellarPoint("Dubhe", elev_ssp3, false, jd_ssp3);
    //ssp3->shiftSpeedTime(0.0, 12.0, 0.0); // advance to Dubhe measurement

    solver->showSumnerLines(NO_COLOR, 0.003f);  // 0.0003f is about 1 arc minute wide line or about 1 nautical mile thick
    solver->showNames(font, NO_COLOR, 0.08f);  // LIGHT_ORANGE
    solver->showDots(NO_COLOR, 0.02f);

    LLH point = solver->calcLocation(false);
    std::cout << "\nSubPointSolver McToon: " << astro->formatLatLon(point.lat, point.lon) << '\n';

    std::cout << "\nCalculated Azi/Ele at this location: \n";

    astro->setTime(2018, 11, 15.0, 18.0, 28.0, 15.0);

    // My adaptation of AA+
    LLH ssp1_decra = astro->getTrueDecRAbyName("Regulus", jd_ssp1);
    //LLH ssp1_altaz = astro->calcGeo2Topo(ssp1_decra, point);
    std::cout << " - Regulus RA/Dec: " << astro->formatDecRA(ssp1_decra.lat, ssp1_decra.lon) << '\n';
    // Actual Catalogue RA/Dec
    LLH reg_cat = astro->getDecRAbyName("Regulus");
    std::cout << " - Regulus Catalogue     RA/Dec: " << astro->formatDecRA(reg_cat.lat, reg_cat.lon) << '\n';
    // With Proper Motion applied
    LLH reg_pm = astro->getDecRAwithPMbyName("Regulus",astro->getJD_TT());
    std::cout << " - Regulus w/Prop Motion RA/Dec: " << astro->formatDecRA(reg_pm.lat, reg_pm.lon) << '\n';
    // With Precession
    CAA2DCoordinate reg_prec = CAAPrecession::PrecessEquatorial(reg_pm.lon / 15.0, reg_pm.lat, EDateTime::getJDUTC2TT(JD2000), astro->getJD_TT());
    std::cout << " - Regulus w/Precession  RA/Dec: " << astro->formatDecRA(reg_prec.Y, reg_prec.X * 15.0) << '\n';
    // With Nutation
    double obliq = CAANutation::MeanObliquityOfEcliptic(astro->getJD_TT());
    double nutlon = CAANutation::NutationInLongitude(astro->getJD_TT());
    double nutobl = CAANutation::NutationInObliquity(astro->getJD_TT());
    double reg_nut_ra = CAANutation::NutationInRightAscension(reg_prec.X, reg_prec.Y, obliq, nutlon, nutobl);
    double reg_nut_dec = CAANutation::NutationInDeclination(reg_prec.X, obliq, nutlon, nutobl);
    reg_prec.X += (reg_nut_ra / 3600.0) / 15.0;
    reg_prec.Y += reg_nut_dec / 3600.0;
    std::cout << " - Regulus w/Nutation    RA/Dec: " << astro->formatDecRA(reg_prec.Y, reg_prec.X * 15.0) << '\n';
    // With Aberration
    CAA2DCoordinate reg_aber = CAAAberration::EquatorialAberration(reg_prec.X, reg_prec.Y, astro->getJD_TT(), true);
    reg_prec.X += reg_aber.X;
    reg_prec.Y += reg_aber.Y;
    std::cout << " - Regulus w/Aberration  RA/Dec: " << astro->formatDecRA(reg_prec.Y, reg_prec.X * 15.0) << '\n';


    astro->setTime(2028, 11, 13.0, 19.0, 0.0, 0.0);
    // My suspect function - Theta Persei examples 21.b & 23.a from AA Meeus.
    LLH tpe_decra = astro->getTrueDecRAbyName("* tet Per", astro->getJD_TT());
    //LLH ssp1_altaz = astro->calcGeo2Topo(ssp1_decra, point);
    std::cout << " - Theta Persei RA/Dec: " << astro->formatDecRA(tpe_decra.lat, tpe_decra.lon) << '\n';
    // Actual Catalogue RA/Dec

    LLH tpe_cat = astro->getDecRAbyName("* tet Per", false);
    std::cout << " - Theta Persei Catalogue     RA/Dec: " << astro->formatDecRA(tpe_cat.lat, tpe_cat.lon, false) << '\n';
    // With Proper Motion applied
    LLH tpe_pm = astro->getDecRAwithPMbyName("* tet Per", astro->getJD_TT());
    std::cout << " - Theta Persei w/Prop Motion RA/Dec: " << astro->formatDecRA(tpe_pm.lat, tpe_pm.lon) << '\n';
    // With Precession
    CAA2DCoordinate tpe_prec = CAAPrecession::PrecessEquatorial(tpe_pm.lon / 15.0, tpe_pm.lat, EDateTime::getJDUTC2TT(JD2000), astro->getJD_TT());
    std::cout << " - Theta Persei w/Precession  RA/Dec: " << astro->formatDecRA(tpe_prec.Y, tpe_prec.X * 15.0) << '\n';
    // With Nutation
    double tpe_obliq = CAANutation::MeanObliquityOfEcliptic(astro->getJD_TT()); // Degrees
    double tpe_nutlon = CAANutation::NutationInLongitude(astro->getJD_TT());    // Arcseconds
    double tpe_nutobl = CAANutation::NutationInObliquity(astro->getJD_TT());    // Arcseconds
    //double tpe_nut_ra = CAANutation::NutationInRightAscension(tpe_prec.X, tpe_prec.Y, tpe_obliq, tpe_nutlon, tpe_nutobl);  // Arcseconds
    double tpe_nut_ra = astro->NutationInRightAscension(tpe_prec.Y, tpe_prec.X * 15.0, tpe_obliq, tpe_nutlon / 3600.0, tpe_nutobl / 3600.0); // Degrees
    //double tpe_nut_dec = CAANutation::NutationInDeclination(tpe_prec.X, tpe_obliq, tpe_nutlon, tpe_nutobl);  // Arcseconds
    double tpe_nut_dec = astro->NutationInDeclination(tpe_prec.X * 15.0, tpe_obliq, tpe_nutlon / 3600.0, tpe_nutobl / 3600.0, false); // Degrees
    std::cout << "   - Mean Obliquity of Ecliptic:  " << astro->angle2DMSstring(tpe_obliq, false) << '\n';
    std::cout << "   - Nutation in Longitude:       " << astro->angle2DMSstring(tpe_nutlon / 3600.0, false) << '\n';
    std::cout << "   - Nutation in Onliquity:       " << astro->angle2DMSstring(tpe_nutobl / 3600.0, false) << '\n';
    //std::cout << "   - Nutation in Right Ascension: " << astro->angle2DMSstring(tpe_nut_ra / 3600.0, false) << '\n';
    std::cout << "   - Nutation in Right Ascension: " << astro->angle2uHMSstring(tpe_nut_ra, false) << '\n';
    //std::cout << "   - Nutation in Declination:     " << astro->angle2DMSstring(tpe_nut_dec / 3600.0, false) << '\n';
    std::cout << "   - Nutation in Declination:     " << astro->angle2DMSstring(tpe_nut_dec, false) << '\n';

    //tpe_prec.X += (tpe_nut_ra / 3600.0) / 15.0;
    tpe_prec.X += (tpe_nut_ra / 15.0);
    //tpe_prec.Y += tpe_nut_dec / 3600.0;
    tpe_prec.Y += tpe_nut_dec;
    std::cout << " - Theta Persei w/Nutation    RA/Dec: " << astro->formatDecRA(tpe_prec.Y, tpe_prec.X * 15.0) << '\n';
    // With Aberration
    CAA2DCoordinate tpe_aber = CAAAberration::EquatorialAberration(tpe_prec.X, tpe_prec.Y, astro->getJD_TT(), true);
    tpe_prec.X += tpe_aber.X;
    tpe_prec.Y += tpe_aber.Y;
    std::cout << " - Theta Persei w/Aberration  RA/Dec: " << astro->formatDecRA(tpe_prec.Y, tpe_prec.X * 15.0) << '\n';

    //astro->setTime(2018, 11, 15.0, 18.0, 28.0, 15.0); // Regulus time
    astro->setTime(2018, 11, 15.0, 18.0, 32.0, 15.0);  // Duhbe time


    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        app.update();
        if (app.anim) {
            //astro->setTimeNow();
            //astro->addTime(0.0, 0.0, 0.0, 0.0, false);
        }
        solver->update();
        //loc->moveLoc(app.customparam2 * deg2rad, app.customparam1 * deg2rad);
        //sky->updateGrid();
        app.render();
    }
}


void SimpleDemo(Application& app) {
    std::cout << (Astronomy::stellarobjects_loaded ? "Stellar objects loaded.\n" : "Stellar objects not loaded.\n");
    Astronomy::loadStellarObjects();
    std::cout << (Astronomy::stellarobjects_loaded ? "Stellar objects loaded.\n" : "Stellar objects not loaded.\n");
    Astronomy* astro = new Astronomy;
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);

    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");

    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;
    earth->w_sinsol = true;
    earth->addGrid();
    earth->flatsunheight = 0.0f;
    earth->addSubsolarPoint();

    app.anim = true;
    while (!app.shouldClose()) {
        //app.update();
        if (app.anim) {
            astro->setTimeNow();  // Has granularity of 1 second
            //astro->addTime(0, 0, 0, 1.0/60.0);
        }
        app.render(); // also calls app.update()
    }

}

void ColumbusEclipse(Application& app) {
    Astronomy* astro = new Astronomy;

    astro->setTime(2023, 03, 21.0, 12.0, 0.0, 0.0);
    //astro->setTime(1504, 03, 11.0, 0.0, 41.0, 35.0);
    //astro->addTime(0.0, 7.5, 0.0, 0.0);

    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera;
    app.currentCam = cam;
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "Scene1");
    Earth* earth = scene->newEarth("NSAE", 180, 90);
    app.currentEarth = earth;

    earth->w_sinsol = true;
    earth->addGrid();
    earth->flatsunheight = 0.0f;
    earth->addSubsolarPoint();
    earth->addSublunarPoint();
    //earth->addLunarUmbraCone();

    earth->addUmbraCone();

    int lg = earth->addLocGroup();
    Location* loc = earth->locgroups[lg]->addLocation(l_jamaica.lat, l_jamaica.lon, false, 0.2f);
    //loc->addArrow3DTrueMoon();
    loc->addPlanetTruePath24(SUN, YELLOW, 0.003f);

    app.anim = false;
    while (!app.shouldClose()) {
        //app.update();
        if (app.anim) {
            astro->setTimeNow();  // Has granularity of 1 second
            //astro->addTime(0, 0, 0, 1.0/60.0);
        }
        app.render(); // also calls app.update()
    }

}




void StarMovement(Application& app) {

    Astronomy* astro = app.newAstronomy();
    astro->setTimeNow();
    //for (unsigned int i = 0; i <= 240; i++) {
    //    astro->addTime(0.0, 0.0, 10.0, 0.0, false);
    //    astro->updateTimeString();
    //    std::cout << ": GMST = " << astro->getGsid() << '\n';
    //}

    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    cam->setLatLonFovDist(40.0f, -2.0f, 1.4f, 20.0f);
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "GlobalView");

    float localskyradius = 0.1f;
    
    Earth* earth = scene->newEarth("NSER", 180, 90);
    app.currentEarth = earth;

    earth->w_sinsol = true;
    earth->addArrow3DTrueSun();
    //earth->addGrid();
    //earth->addGreatArc({ 15.0, 170.0, 0.0 }, { 25.0, -10.0, 0.0 }, LIGHT_RED, 0.003f, false);
    earth->flatsunheight = 0.0f;
    //earth->addSublunarPoint();
    earth->addTropics();
    unsigned int lg = earth->addLocGroup();
    double loclatitude = l_cph.lat;
    double loclongitude = l_cph.lon;
    // Monte Cridola - Bert Rickles lunalemma
    //double loclatitude = 46.4648092781465;
    //double loclongitude = 12.493651276848878;
    Location* loc = earth->locgroups[lg]->addLocation(loclatitude, loclongitude, false, localskyradius);
    std::string trackstarname = "Alnilam"; // Very near celestial equator: "Sadalmelik"; // Orion's belt west to east: Mintaka, Alnilam and Alnitak
    LLH trackstar = astro->getDecRAbyName(trackstarname);
    glm::vec4 trackcolor = astro->getColorbyName(trackstarname);

    loc->addLocDot(0.005f, LIGHT_RED);
    loc->addAziEleGrid(15.0, false, 0.0005f, glm::vec4(0.6f, 0.3f, 0.6f, 1.0f));
    //loc->addUpCoord(1.0f);
    //loc->truesun->enableDot3D();
    //loc->addArrow3DRADec(9999, trackstar.lon, trackstar.lat, trackcolor, 0.005f, 1.0f);
    //loc->addTangentPlane(LIGHT_RED, 0.6f);
    loc->addHorizon(LIGHT_RED, 0.001f);
    loc->addCircumPolar(GREEN, 0.001f);
    glm::vec4 skycol = BLUE; // BLACK;
    skycol.a = 0.6f;
    loc->addLocSky(localskyradius, skycol);
    //loc->addTruePlanetDot(VENUS, 0.005f, NO_COLOR, false);
    //loc->addTruePlanetDot(MERCURY, 0.005f, NO_COLOR, false);
    //loc->addTruePlanetDot(MARS, 0.005f, NO_COLOR, false);
    //loc->addPlanetTruePathSidYear(VENUS);
    //loc->addPlanetTruePathSidYear(MERCURY);
    //loc->addPlanetTruePathSidYear(MARS);
    //loc->addTrueLunalemma(MOONCOLOR, localskyradius/200.0f, localskyradius*tauf/720.0f);
    //Earth2* earth = scene->newEarth2("NSAE", 180, 90);
    //earth->name = "Earth2";
    //app.currentEarth2 = earth;

    SkySphere* sky = scene->newSkysphere(180, 90, true);
    sky->setMode("SE", earth, loc);
    sky->setTexture(false);
    sky->setMovable(true);
    sky->setRadius(localskyradius);
    sky->addStars(4.0); // Magnitude: 1.0 = 19, 2.0 = 58, 3.0 = 197, 4.0 = 561, 5.0 = 1688, 6.0 = 5159, 6.5 = 9031 (all) brightest stars
    //sky->addLineStarStar("* alf CMa", "* alf Car");

    //sky->addStarByName(trackstarname);
    //sky->addStarByName("Mintaka");
    //sky->addStarByName("Alnitak");
    sky->addGrid(0.0005f);
    //earth->addSubsolarPoint();
    //earth->addSublunarPoint();
    //earth->addEcliptic();
    //earth->addTropics();
    //unsigned int fpa = earth->addFirstPointAries();

    app.customhigh1 = 90.0f;
    app.customlow1 = -90.0f;
    app.customparam1 = (float)loclatitude;

    app.customhigh2 = 180.0f;
    app.customlow2 = -180.0f;
    app.customparam2 = (float)loclongitude;

    Camera* loccam = scene->newCamera("Location Cam");
    app.locationCam = loccam;
    loccam->setFoV(70.0f);
    loccam->setLookAt(loc->getPosition()*1.01f, loc->getPosition() + loc->getNorth(), loc->getZenith());
    loccam->camNear = 0.0001f;
    loccam->camFar = 100.0f;

    LLH decra = astro->getDecRAbyName("* tet Per", false);
    LLH decrat = astro->getTrueDecRAbyName("* tet Per", false);
    // NOTE: With rad = false, return values are RA in degrees, Dec in degrees
    std::cout << "Star catalogue & true RA(deg), Dec(deg): " << decra.lon << ", " << decra.lat << " " << decrat.lon << ", " << decrat.lat << '\n';

    //loccam->dumpParameters();
    RenderLayer3D* loclayer = app.newLayer3D(0.666f, 0.0f, 1.0f, 0.333f, scene, astro, loccam, false);
    gui->addLayer3D(loclayer, "LocalView");

    //app.anim = true;
    //app.renderoutput = true;

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        //app.update();
        if (app.anim) {
            // For satellites, real time -20 minutes
            //astro->setTimeNow();
            //astro->addTime(0.0, 0.0, 0.0, -1200.0, false);
            // For a visually reasonable progression of time in general
            //astro->addTime(1.035050, 0.0, 0.0, 0.0, false);
            //astro->addTime(sidereald, 0.0, 0.0, 0.0);
            astro->addTime(0.0, 0.0, 1.0, 0.0);
        }
        //earth->updateFirstPointAries(fpa);
        //sky->updateGrid();
        loc->moveLoc((double)app.customparam1, (double)app.customparam2, false);
        sky->updateGrid();
        sky->UpdateTime(astro->getJD_UTC());
        //std::cout << "Idle: Time = " << astro->getTimeString() << ", gsid = " << astro->getGsid() << ", astro = " << astro << '\n';
        glm::vec3 lcdir = loc->calcEleAzi2Dir({ loccam->camLat, loccam->camLon, 0.0 }, false);

        loccam->setLookAt(loc->getPosition() * 1.0001f, loc->getPosition() + lcdir, loc->getZenith());
        //loccam->update();
        //std::cout << "Loccam: "; VPRINT(loccam->getPosition());
        //std::cout << " Location: "; VPRINT(loc->getPosition());
        //std::cout << "\n";
        app.render();
    }
}

void LunarData(Application& app) {
    Astronomy* astro = app.newAstronomy();

    astro->setJD_UTC(JD2000);
    std::cout << JD2000 << " " << astro->getJD_TT() << "\n";

    // Trying to match J.Meus 1992 Astronomical Algorithms SE, example 47.a (page 342)
    astro->setTime(1992, 4, 12.0, 0.0, 0.0, 0.0);  // Meus example 47.a

    double currentJD = astro->getJD_UTC();         // Should probably be JD_TT. But JD_UTC gives the exact result of Meus example 47.a
    double elon = CAAMoon::EclipticLongitude(currentJD);  // lambda
    double elat = CAAMoon::EclipticLatitude(currentJD);   // beta
    double Epsilon = CAANutation::TrueObliquityOfEcliptic(currentJD);
    CAA2DCoordinate equa = CAACoordinateTransformation::Ecliptic2Equatorial(elon, elat, Epsilon);
    double moonDist = CAAMoon::RadiusVector(currentJD); // RadiusVector() returns in km
    double moonRA = hrs2rad * equa.X;
    double moonDec = deg2rad * equa.Y;
    // precessing the epoch is needed below when matching NASAs 2023 lunar table (see below), not with Meus' example.
    // CAA2DCoordinate j2k = CAAPrecession::PrecessEquatorial(equa.X, equa.Y, astro->getJD_UTC(), JD2000);

    //std::cout << astro->getTimeString() << " " << astro->radecFormat(astro->rangezero2tau(moonRA), astro->rangepi2pi(moonDec), true) << "\n";
    std::cout << astro->getTimeString() << " " << equa.X << " / " << equa.Y << "\n";
    std::cout << astro->getJD_UTC() - astro->getJD_TT() << "  " << elon << "," << elat << " | " << moonDist << ", True Obliq: " << Epsilon << "\n\n\n";
    // Above is completely accurate as per Meus example 47.a
    
    // Trying to match https://svs.gsfc.nasa.gov/vis/a000000/a005000/a005048/mooninfo_2023.txt
    // Which is a data dump of this: https://svs.gsfc.nasa.gov/5048
    // (Moon phases and libration 2023)
    CAA2DCoordinate j2k{ 0.0, 0.0 };
    for (double i = 0; i < 30; i++) {
        astro->setTime(2023, 1, 1.0, i, 0.0, 0.0);
        currentJD = astro->getJD_TT();
        elon = CAAMoon::EclipticLongitude(currentJD);  // lambda
        elat = CAAMoon::EclipticLatitude(currentJD);   // beta
        Epsilon = CAANutation::TrueObliquityOfEcliptic(currentJD);
        equa = CAACoordinateTransformation::Ecliptic2Equatorial(elon, elat, Epsilon);
        moonDist = CAAMoon::RadiusVector(currentJD); // RadiusVector() returns in km - seems consistently 40km low for Jan 2023.
        //moonRA = hrs2rad * equa.X;
        //moonDec = deg2rad * equa.Y;
        // Above NASA datadump appears to list RA/Dec in J2000.0 epoch, we calculated to epoch of date.
        j2k = CAAPrecession::PrecessEquatorial(equa.X, equa.Y, astro->getJD_TT(), 2451545.00074287); // latter is JD2000, but in TT);
        //std::cout << astro->getTimeString() << " " << astro->radecFormat(astro->rangezero2tau(moonRA), astro->rangepi2pi(moonDec), true) << "\n";
        //std::cout << astro->getTimeString() << " " << equa.X << " / " << equa.Y << "\n";

        // Selenographic position of the Sun (use for rendering terminator)
        CAASelenographicMoonDetails selsun = CAAPhysicalMoon::CalculateSelenographicPositionOfSun(astro->getJD_TT(),/* bHighPrecision */ true);

        // Geocentric libration and position angle of lunar axis
        CAAPhysicalMoonDetails libration = CAAPhysicalMoon::CalculateGeocentric(astro->getJD_TT());

        std::cout << "J2000: " << astro->getTimeString() << " " << moonDist << " " << j2k.X << " " << j2k.Y;
        std::cout << " " << selsun.l0 << " " << selsun.b0 << "\n";
        std::cout << "       " << libration.l << " " << libration.b << " " << clamp0to360(libration.P) << "\n";
    }


    // Test new DetailedMoon object
    //astro->setTimeNow();
    astro->setTime(2020, 1, 27.0, 16.0, 0.0, 0.0);
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer,"LunarView");
    
    //RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    //text->setFont(app.m_font2);
    //text->setAstronomy(astro);
    //RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    //gui->addLayer3D(layer, "GlobalView");


    // FoV to Moon's greatest angular diameter (0.56833 degrees)
    // Distance: Moon's real diameter is 3474.8km, internal rep is 2 units. Moon's closest distance ~360000km. So 2*360000/3474.8 fills screen
    //           Dist is dynamically updated in DetailedMoon::update() to simulate Earth Moon distance
    cam->setLatLonFovDist(0.0f, 0.0f, 0.56833f, 210.0f);
    // With such as small FoV and high distance, the Near Clipping Plane has to be around 5.0f or more to avoid rendering issues.
    cam->camNear = 5.0f;
    //cam->setCamLightPos()

    DetailedMoon* moon = scene->newDetailedMoon("NSAE", 180, 90, 1.0f);
    app.currentEarth2 = moon;
    moon->addSunGP();
    moon->addEarthGP();
    moon->addLibrationTrail();
    //moon->setTopocentric(l_ams.lat, l_ams.lon);
    // NOTE: These can all expose their varius confguration settings directly via the object link.
    moon->addEquator();
    moon->equatorOb->setColor(GREY);
    moon->equatorOb->setWidth(0.003f);
    moon->addPrimeMeridian();
    moon->primemOb->setColor(GREY);
    moon->primemOb->setWidth(0.003f);
    //moon->addGrid(10.0);
    
    //DetailedEarth* erf = scene->newDetailedEarth("NS", 180, 90, 1.0f);
    //erf->addEquator();
    //erf->addPrimeMeridian();
    //app.currentEarth2 = erf;
    //DetailedSky* sky = scene->newDetailedSky("NS", 90, 45, 1.2f);
    //sky->setTexture(true);

    //scene->scenetree->printSceneTree();

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            astro->addTime(0.0, 0.0, 30.0, 0.0);
            //astro->setTimeNow();
        }
        app.render();

    }
}

void TestNewSunGP(Application& app) {
    Astronomy* astro = app.newAstronomy();

    astro->setTimeNow();
    Scene* scene = app.newScene();
    Camera* cam = scene->w_camera; // Pick up default camera
    app.currentCam = cam;          // Bind camera to keyboard updates
    RenderLayer3D* layer = app.newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerText* text = app.newLayerText(0.0f, 0.0f, 1.0f, 1.0f, nullptr);
    text->setFont(app.m_font2);
    text->setAstronomy(astro);
    RenderLayerGUI* gui = app.newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer, "EarthView");

    DetailedEarth* erf = scene->newDetailedEarth("NSAE", 180, 90, 1.0f);
    app.currentEarth2 = erf;
    //erf->addEquator();
    //erf->addPrimeMeridian();
    erf->addSunGP();
    //erf->m_sungp->setRadius(0.1f);
    //erf->position = { 0.0f, 0.5f, 0.0f };

    scene->scenetree->printSceneTree();

    while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    {
        if (app.anim) {
            astro->setTimeNow();
            //astro->addTime(0.0, 0.0, 30.0, 0.0);
        }
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
    py::class_<LLH>(m, "LLH")
        .def(py::init<double, double, double>())
        .def_readwrite("lat", &LLH::lat)
        .def_readwrite("lon", &LLH::lon)
        .def_readwrite("dst", &LLH::dst)
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
    py::class_<ImFont>(m, "Font") // "Stolen" from https://toscode.gitee.com/lilingTG/bimpy/blob/master/bimpy.cpp it has lots of ImGui .def's
        .def(py::init())
        ;
    // -----------------
    //  EDateTime class   - Complete, except for static void normalizeDateTime()
    // -----------------
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
        .def("calculateGsid", &Astronomy::calculateGsid, "Returns Greenwich Sidereal Time for provided JD (in UTC)",
            py::arg("jd_utc")
        )
        .def("getGsid", &Astronomy::getGsid, "Returns Greenwich Sidereal Time for provided JD (in UTC), or for current JD if omitted",
            py::arg("jd_utc")
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
    //Sector45_001(app);
    //TestArea5(app);
    //SolSysTest(app);
    //basic_geometry(app);
    //ScienceItOut_Response1(app);
    //Sandbox2_SIO(app);
    //SunSectorSandbox(app);
    //ICSTvsCoreyKell(app);
    //CoreyKellDetailed(app);
    //renderFBO(app);
    //Brian_Leake_01(app);
    
    // Latest workarea - shows 2 views with LunarLemma over time at a given location.
    //Parenting_test(app);

    // Quick test to see if AA includes Earth axial precession
    //AxialPrecession(app);
    //Lambertian(app);
    //SimpleTest(app);
    //McToonChallengeV1(app);
    //NullIslandTest(app);

    //SimpleDemo(app);
    //ColumbusEclipse(app);

    //StarMovement(app);
    LunarData(app);
    //TestNewSunGP(app);


    // Cleanup
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
