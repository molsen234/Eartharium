
#include "config.h"
#include "mdoOpenGL.h"
#include "Primitives.h"
#include "Earth.h"
#include "Astronomy.h"
// It would be good to clean up the header include hierarchy one day !!!

#include <thread>

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


// ------------
//  BillBoardP
// ------------
//  BillBoard text for primitives - currently unused placeholder
//  Will eventually provide texts hovering above objects, to indicate angles, lengths or names
class BillBoardP {
    std::string text = "TEST!";
public:
    BillBoardP(std::string t) {
        text = t;
    }
};


// -------------
//  SceneObject
// -------------
// Currently unused
// Meant to facilitate measuring distances and angles between any two objects, as well as implementing parenting of transformations
// The plan is to have locations be children of a planetary object, arrows be children of locations, angle/distance texts be children of arrows,
// and offer a way to indicate what to measure to etc. I have yet to work out the requirements ...
class SceneObject {
    union primobj { // Allow all types of objects to be SceneObject
        Primitive3D* primitive;
        Earth* earth;
        Location* location;
        Camera* camera;
        // add the rest ...
    };
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);  //PolyCurve, how does this apply???
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec4 orientation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Up vec3 + rotation about up, from which angle?
    glm::vec4 color = WHITE;
    //Material* material = nullptr;
    glm::mat4 worldmatrix = glm::mat4(1.0f);
    unsigned int id = 0;
    unsigned int type = 0;
    //primobj primitiveobject;            // The geometry etc of this object (of type SceneObject::type)
    // std::vector<SceneObject*> children;
public:
    std::string name = "Object";
    SceneObject* parent = nullptr;
    void setParent(SceneObject* oid) { parent = oid; }
    SceneObject* getParent() { return parent; }
    unsigned int getID() { return id; }
    void setID(unsigned int oid) { id = oid; }
};


// -----------
//  SceneTree
// -----------
// Currently unused
// Meant to facilitate measuring distances and angles between any two objects, as well as implementing parenting of transformations and GUI traversal
// See SceneObject above for more info
class SceneTree {
    Camera* camera = nullptr;
    tightvec<SceneObject*> objects;
public:
    SceneTree() { 
        objects.reserve(20); // Could not figure out how to pass the reserve value in constructor
        return;
    }
    void addObject(SceneObject* ob) {
        unsigned int id = objects.store(ob);
        ob->setID(id);
    }
    void removeObject(SceneObject* ob) {
        objects.remove(ob->getID());
    }
    void printObjects() {
        for (auto o : objects.m_Elements) {    // iterator runs over inner id directly, i.e. o is in underlying std::vector order.
            std::cout << "Object: " << o->name << " " << o->getID() << "\n";
        }
        std::cout << "\n";
        for (unsigned int oid = 0; oid < objects.size(); oid++) {   // Iterates over outer id, i.e. tightvec translates the [] index internally.
            std::cout << "Object: " << objects[oid]->name << " " << objects[oid]->getID() << "\n";
        }
    }
};


// Idle Area for python
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
    while (!glfwWindowShouldClose(myapp.window)) {
        myapp.currentCam->update();
        myapp.update();
        //if (myapp.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        //scene->w_camera->CamUpdate();

        myapp.render();
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
    astro->setTime(2021, 6, 15.0, 9.0, 37.0, 0.0, true);
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
        loc->addLocSky(locskyradius, locskycolor);
        //loc->flatsun->setGeometry("AE");  // Use "CG" for current geometry, it will then adapt to Earth morph rather than remaining fixed
        //loc->flatsun->enableArrow3D();
        //loc->flatsun->enableDot3D();
        //loc->flatsun->enableLine3D();
        //loc->flatsun->enableElevationAngle();
        //loc->flatsun->enableEleAngText(font);
        //loc->flatsun->enablePath24();
        loc->truesun->enableArrow3D();
        loc->truesun->enableLine3D();
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
    sunloc->truesun->enableArrow3D();
    //sunloc->truesun->enableEleAngText(font);

    Location* crosspoint = earth->locgroups[lg]->addLocation(subsolar.lat, subsolar.lon + 45.0, false, locskyradius);
    app.customparam1 = (float)subsolar.lat;
    app.customparam2 = (float)(subsolar.lon + pi4);
    crosspoint->addLocDot();
    crosspoint->addTangentPlane();
    crosspoint->truesun->enableArrow3D();
    crosspoint->truesun->enableElevationAngle();
    crosspoint->truesun->enableEleAngText(font);

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
    astro->setTime(2022, 2, 5.0, 8.0, 17.0, 45.0, true);
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







void TestArea5(Application& myapp) {
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
    astro->setTime(2020, 4, 29.0, 9.0 - 4.5, 2.0, 0.0, true); // As per flatearthintel.com spreadsheet from Corey
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
    std::cout << "Opening File!\n";
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

        astro->setTime((long)year, (long)month, day, hour, minute - 1.0, 0.0, true);
        astro->addTime(0.0, -utcoffset, 0.0, 0.0); // Causes duplicate call to Astronomy::update(), which is slightly slower
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
    for (double ut = 1581073620.0; ut <= 1593340200.0; ut += utstep) {
        astro->setUnixTime(ut);
        //astro->addTime(0.0, -utcoffset, 0.0, 0.0); // Causes duplicate call to Astronomy::update(), which is slightly slower
        LLH sunRD = astro->getDecGHA(SUN);
        LLH sunAE = astro->calcGeo2Topo(sunRD, { lat * deg2rad, lon * deg2rad, tri + ter });
        mypred.push_back({ (double)ut, sunAE.lat * rad2deg });
        predcount++;
    }
    std::cout << "Prediction data points: " << predcount << "\n";  // WARNING: ImPlot uses 16 bit indices by default, so do not exceed 65535 !!!

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




// Application is a global container for all the rest
Application app = Application();  // New global after refactor

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
    // Should probably be moved into a separate translation unit (*.h/*.cpp)
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
        .def(py::init([]() { std::cout << "C++ creating vec4!\n"; return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); }))
        .def(py::init<float, float, float, float>())
        .def_readwrite("x", &glm::vec4::x)
        .def_readwrite("y", &glm::vec4::y)
        .def_readwrite("z", &glm::vec4::z)
        .def_readwrite("w", &glm::vec4::w)
        ;
    py::class_<ImFont>(m, "Font") // Stolen from https://toscode.gitee.com/lilingTG/bimpy/blob/master/bimpy.cpp it has lots of ImGui .def's
        .def(py::init())
        ;
    py::class_<Application>(m, "Application")
        //.def("getRenderChain", &Application::getRenderChain, py::return_value_policy::reference)
        .def("newScene", &Application::newScene, py::return_value_policy::reference)
        .def("newAstronomy", &Application::newAstronomy, py::return_value_policy::reference)
        .def("update", &Application::update)
        .def("newLayer3D", &Application::newLayer3D, "Creates a new 3D Render Layer",
            py::arg("vpx1"), py::arg("vpy1"), py::arg("vpx2"), py::arg("vpy2"),
            py::arg("scene"), py::arg("astro"), py::arg("cam") = (Camera*) nullptr,
            py::return_value_policy::reference
        )
        .def("newLayerText", &Application::newLayerText, "Creates a new Text Render Layer",
            py::arg("vpx1"), py::arg("vpy1"), py::arg("vpx2"), py::arg("vpy2"),
            py::arg("lines") = (RenderLayerTextLines*) nullptr,
            py::return_value_policy::reference
        )
        .def("do_render", &Application::render)
        .def_readwrite("ipython", &Application::ipython)
        .def_readwrite("render", &Application::renderoutput)
        .def_readwrite("currentCam", &Application::currentCam)
        .def_readwrite("m_font2", &Application::m_font2)
        ;
    py::class_<Astronomy>(m, "Astronomy")
        .def("setTime", &Astronomy::setTime, "Sets time in (Y,M,D,h,m,s.ss)",
            py::arg("yr"), py::arg("mo"), py::arg("da"), py::arg("hr"), py::arg("mi"), py::arg("se"), py::arg("gre")
        )
        .def("setTimeNow", &Astronomy::setTimeNow, "Sets time to system clock in UTC")
        .def("setJD", &Astronomy::setJD, "Sets time to Julian Day",
            py::arg("jd"), py::arg("gregorian")
        )
        .def("getJD", &Astronomy::getJD, "Returns the current JD")
        .def("addTime", &Astronomy::addTime, "Adjusts current time by provided amount",
            py::arg("d"), py::arg("h"), py::arg("min"), py::arg("sec")
        )
        .def("calculateGsid", &Astronomy::calculateGsid, "Returns Greenwich Sidereal Time for provided Julian Day",
            py::arg("jd")
        )
        .def("getGsid", &Astronomy::getGsid, "Returns Greenwich Sidereal Time for provided Julian Day, or for current JD if omitted",
            py::arg("jd")
        )
        .def("getTimeString", &Astronomy::getTimeString, "Returns current time & date in string format YYYY-MM-DD HH:MM:SS",
            py::arg("dstring")
        )
        ;
    py::class_<Camera>(m, "Camera")
        .def_readwrite("camFoV", &Camera::camFoV)
        .def_readwrite("camLat", &Camera::camLat)
        .def_readwrite("camLon", &Camera::camLon)
        .def_readwrite("camDst", &Camera::camDst)
        .def("CamUpdate", &Camera::update, "Updates the Camera settings from Application etc")
        ;
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


int main() {
    // This is main. The entry point of execution when launching
    // Do as little as possible here.
    app.start_fullscreen = false;  // Starting full screen can make debugging difficult during a hang
    if (app.initWindow() == -1) return -1; // Bail if Window and OpenGL context setup fails

    if (false) { // Set to true to run a python script and drop into an idle interactive
        app.interactive = false;
        //std::string pyfile = "c:\\Coding\\Eartharium\\Eartharium\\hello.py";
        std::string pyfile = "c:\\Coding\\Eartharium\\Eartharium\\Lesson001.py";  // Hardcoded which python script to run until a script manager is built
        py::scoped_interpreter guard{};
        py::object scope = py::module_::import("__main__").attr("__dict__");
        py::eval_file(pyfile.c_str(), scope);
        IdleArea(app);
        glfwTerminate();
        return 0; // Let the OS deal with all the memory leaks, Not all destructors are up to date currently
    }
    // If no python script was run, simply drop to a test area that sets up via C++ instead.
    // Good for things not yet implemented in python interface, or while developing things,
    // but requires a recompile for every change. Python scripts can simply be saved after changes, and Eartharium can be run again.
    
    // Call scenario
    //Sector45_001(app);
    //TestArea5(app);
    renderFBO(app);
    // Cleanup
    glfwTerminate();

    return 0;  // Poor Windows will deal with cleaning up the heap
}


// Textures - Just a note to remember which texture slots are in use on the GPU
// --------
// Shadows = 1, SkyBox = 2, Earth = 3 + 4, Sundot = 5, Moondot = 6, OutputRender = 7, Font = 8.


// ----------------
//  GLFW Callbacks
// ----------------
void keyboard_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
    // GLFW has a C interface for callbacks, so this has not yet been moved into Application. I'll have to look into that.
    // Mapped Keys: <ESC>,<SPACE>,a,c,d,e,f,g,h,j,m,n,p,q,s,w,z
    // Unmapped Keys: b,i,k,l,o,r,u,v,x,y,1,2,3,4,5,6,7,8,9,0
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
        // Morph Plus/Minus
        if (key == GLFW_KEY_N && app.currentEarth != nullptr) {
            if (app.currentEarth->param == 0.0f) return;
            app.currentEarth->param -= 0.05f;
            if (app.currentEarth->param < 0.0f) app.currentEarth->param = 0.0f;
        }
        if (key == GLFW_KEY_M && app.currentEarth != nullptr) {
            if (app.currentEarth->param == 1.0) return;
            app.currentEarth->param += 0.05f;
            if (app.currentEarth->param > 1.0f) app.currentEarth->param = 1.0f;
        }
        // Earth Eccentricity Plus / Minus - May remove or replace !!!
        if (key == GLFW_KEY_H && app.currentEarth != nullptr) {
            if (app.currentEarth->eccen == 0.0) return;
            app.currentEarth->eccen -= 0.01;
            if (app.currentEarth->eccen < 0.0) app.currentEarth->eccen = 0.0;
        }
        if (key == GLFW_KEY_J && app.currentEarth != nullptr) {
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
        // Z C - Increase / Decrease Camera distance to LookAt point
        if (key == GLFW_KEY_C && app.currentCam != nullptr) {
            app.currentCam->camDst -= app.currentCam->camDst * CAMERA_STEP_FACTOR;
            if (app.currentCam->camDst < CAMERA_MIN_DIST) app.currentCam->camDst = CAMERA_MIN_DIST;
        }
        if (key == GLFW_KEY_Z && app.currentCam != nullptr) {
            app.currentCam->camDst += app.currentCam->camDst * CAMERA_STEP_FACTOR;
            if (app.currentCam->camDst > CAMERA_MAX_DIST) app.currentCam->camDst = CAMERA_MAX_DIST;
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
