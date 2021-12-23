
#include "config.h"
#include "mdoOpenGL.h"
#include "Primitives.h"
#include "Earth.h"
#include "Astronomy.h"

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
class SolarEclipse {

public:
    
};


// ------------
//  BillBoardP
// ------------
//  BillBoard text for primitives
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
class SceneObject {
    union primobj {
        Primitive3D* primitive;
        Earth* earth;
        Location* location;
        Camera* camera;
    };
    unsigned int id = 0;
    unsigned int type = 0;
    //primobj primitiveobject;            // The geometry etc of this object (of type SceneObject::type)
    // std::vector<SceneObject*> children;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec4 orientation = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f); // Up vec3 + rotation about up, from which angle?
    glm::vec4 color = WHITE;
    //Material* material = nullptr;
    glm::mat4 worldmatrix = glm::mat4(1.0f);
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


void AngleArcsDev(Application& myapp) {
    myapp.anim = false;

    Astronomy* astro = myapp.newAstronomy();
    astro->setTimeNow();

    Scene* scene = myapp.newScene();
    AngleArcs* anglearcs = scene->getAngleArcsOb();
    Arrows* arrows = scene->getArrowsOb();

    Camera* cam = scene->w_camera; // Default camera
    cam->camFoV = 22.8f;
    cam->camLat = 90.0f;
    cam->camLon = 0.0f;
    cam->camDst = 10.0f;
    cam->CamUpdate();
    myapp.currentCam = cam;          // Enable kbd controls

    RenderLayer3D* layer1 = myapp.getRenderChain()->newLayer3D(0.0f,0.0f,1.0f,1.0f, scene, astro, cam);

    RenderLayerTextLines textlines = RenderLayerTextLines();
    char angletext[] = "Angle: xxx.xxx\n";
    std::string sangletext = "Angle: xxx.xxx\n";
    textlines.addLine(sangletext);
    RenderLayerText* text = myapp.getRenderChain()->newLayerText(0.0f,0.0f,1.0f,1.0f, &textlines);
    text->setFont(myapp.m_font2);

    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 start = glm::vec3(0.5f, 0.5f, 0.0f);
    glm::vec3 stop = glm::vec3(0.0f, 1.0f, 0.0f);
    unsigned int a1 = arrows->FromStartDirLen(pos, start, 1.0f, 0.01f, LIGHT_BLUE);
    unsigned int a2 = arrows->FromStartDirLen(pos, stop, 1.0f, 0.01f, LIGHT_GREEN);

    unsigned int myangle = anglearcs->add(pos, start, stop, 0.2f, LIGHT_GREY, 0.005f);

    start = glm::vec3(1.0f, 0.0f, 0.0f);
    arrows->UpdateStartDirLen(a1, pos, start, 1.0f, 0.01f, LIGHT_RED);
    anglearcs->update(myangle, NOT_A_VEC3, start, NOT_A_VEC3, NOT_A_FLOAT, NOT_A_COLOR, NOT_A_FLOAT);

    while (!glfwWindowShouldClose(myapp.window)) {
        myapp.currentCam->CamUpdate();
        myapp.update();
        sprintf_s(angletext, "Angle: %03.3f", anglearcs->getAngle(myangle));
        sangletext = angletext;
        //if (myapp.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        //scene->w_camera->CamUpdate();

        myapp.getRenderChain()->do_render();
    }
}

void IdleArea(Application& myapp) {

    myapp.anim = false;
    RenderLayerGUI* gui = myapp.getRenderChain()->newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    unsigned int lnum = 1;
    char lname[20];
    for (auto& l : myapp.getRenderChain()->m_layers) {
        if (l->type == LAYER3D) {
            sprintf_s(lname, "Scene %02d", lnum);
            gui->addLayer3D((RenderLayer3D*)l, lname);
            lnum++;
        }
    }
    while (!glfwWindowShouldClose(myapp.window)) {
        myapp.currentCam->CamUpdate();
        myapp.update();
        //if (myapp.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        //scene->w_camera->CamUpdate();

        myapp.getRenderChain()->do_render();
    }

}


void TestArea6(Application& myapp) {
    // Set up environment - Application could setup a default astro and scene with cam for us. Makes no diff., we'd still need to obtain reference to them
    Astronomy* astro = myapp.newAstronomy();
    astro->setTimeNow();
    astro->addTime(-1.0, 0.0, 0.0, 0.0);
    //astro->setUnixTime(1639884318.0);
    //astro->setTime(2021, 12, 19.0, 12.0 - 9.0, 24.0, 18.0, true);
    Scene* scene = myapp.newScene();
    Camera* cam = scene->w_camera; // Default camera
    cam->camFoV = 22.8f;
    cam->camLat = 90.0f;
    cam->camLon =  0.0f;
    cam->camDst = 10.0f;
    cam->CamUpdate();
    myapp.currentCam = cam;          // Enable kbd controls

    RenderChain* chain = myapp.getRenderChain();   // TODO: Make RenderChain internal to Application, via shim. It is singleton, so we never need more than 1 !!!
    RenderLayer3D* layer1 = chain->newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerGUI* gui = chain->newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    gui->addLayer3D(layer1, "Earth Scene");

    RenderLayerTextLines lines;
    RenderLayerText* text = chain->newLayerText(0.0f, 0.0f, 1.0f, 1.0f, &lines);
    text->setFont(myapp.m_font2);
    text->setCelestialMech(astro);  // Probably change the method name !!!

    Earth* earth = scene->newEarth("AENS", 180, 90);
    //earth->addGrid(15.0f);

    earth->flatsunheight = 0.0f; // Used for both Subsolar and Sublunar points
    earth->addSubsolarPoint();
    earth->w_sinsol = true;
    earth->addSublunarPoint();
    earth->w_linsol = true;
    earth->addTerminatorTrueSun();
    earth->w_twilight = false;
    earth->addTerminatorTrueMoon();
    myapp.anim = false;
    while (!glfwWindowShouldClose(myapp.window)) {

        myapp.update();
        if (myapp.anim) astro->addTime(0.0, 0.0, 5.0, 0.0);
        scene->w_camera->CamUpdate();

        chain->do_render();
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
    cam->CamUpdate();
    myapp.currentCam = cam;          // Enable kbd controls
    RenderChain* chain = myapp.getRenderChain();   // TODO: Make RenderChain internal to Application, via shim. It is singleton, so we never need more than 1 !!!
    RenderLayer3D* layer1 = chain->newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene, astro, cam);
    RenderLayerGUI* gui = chain->newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);

    gui->addLayer3D(layer1, "Earth Scene");
     
    RenderLayerPlot* plot = chain->newLayerPlot(0.0f, 0.0f, 1.0f, 1.0f); // upper right quarter (Y axis is downwards)
    plot->animateView(0.0f, 0.67f, 1.0f, 1.0f, 60);
    // Test Date and Time to Unix Timestamp conversion
    //std::cout << "Unix timestamp: " << astro->getDateTime2UnixTime(1956, 10, 10, 10, 10, 10) << "\n";

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

        astro->setTime(year, month, day, hour, minute-1.0, 0.0, true);
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


    RenderLayerText* text = chain->newLayerText(0.0f, 0.0f, 1.0f, 1.0f, lines);
    text->setFont(myapp.m_font2);
    text->setCelestialMech(astro);  // Probably change the method name !!!

    //glm::vec3 dotpos = glm::vec3(0.0f, 0.0f, 0.0f);
    //scene->getDotsOb()->FromXYZ(dotpos, LIGHT_GREEN, 0.2f);

    //Minifigs* mf = scene->newMinifigs();
    //Arrows* ar = scene->getArrowsOb();
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

    unsigned int locgroup = earth->addLocGroup();
    // Fayetteville in Sandhills of North Carolina: 35.050458, -78.912176
    Location* loc = earth->locgroups[locgroup]->addLocation(l_kabul.lat, l_kabul.lon, false, 0.2f);
    //loc->addObserver(135.0f, LIGHT_RED, 0.1f);
    loc->addArrow3DTrueSun(0.2f, locsunarrowwidth, predictioncolor);

    LLH sunpos = astro->calcGeo2Topo(astro->getDecGHA(SUN, 0.0), { loc->getLat(), loc->getLon(), 1817.522 + 0.5 });
    earth->w_refract = true;
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
    scene->w_camera->CamUpdate();
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
                scene->w_camera->CamUpdate();
                chain->do_render();
            }
            sprintf_s(dstring, "Measured alt: %02.3f\n", (float)(pt.data));
            ele_pred_text = dstring;

            for (int i = 0; i < pauseframes; i++) { // Pause at datapoint
                chain->do_render();
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
        scene->w_camera->CamUpdate();

        sunpos = astro->calcGeo2Topo(astro->getDecGHA(SUN, 0.0), { loc->getLat(),loc->getLon(), 10000.0 });
        sprintf_s(pstring, "Predicted Elevation: %02.3f\n", (float)(sunpos.lat * rad2deg));
        ele_pred_text = pstring;

        //plot->plotSeries(mydata);
        //plot->predictSeries(mypred);

        chain->do_render();
    }

}

// Application is a global container for all the rest
Application app = Application();  // New global after refactor

void TestArea4() {
    app.ipython = false; // drop to interactive python console before exiting script
    py::scoped_interpreter guard{};
    py::object scope = py::module_::import("__main__").attr("__dict__");
    py::eval_file("c:\\Coding\\Eartharium\\Eartharium\\hello.py", scope);

    RenderLayer3D* layer1 = (RenderLayer3D*)app.getRenderChain()->m_layers[0];
    Scene* scene1 = layer1->m_scene;
    while (!glfwWindowShouldClose(app.window)) {
        // Camera transform - orbital
        //scene1->m_app->update();
        app.update();
        //astro->setTimeNow();
        scene1->w_camera->CamUpdate();
        //scene2->w_camera->CamUpdate();

        // Update time ticks etc
        //astro->addTime(1.0, 0.0, 0.0, 0.0); // dhms
        //if (world.GetSolsysOb() != nullptr) world.GetSolsysOb()->Update();

        app.getRenderChain()->do_render();
    }
}
void TestArea3() {

    app.togglefullwin = false;
    //app.camFoV = 50.0f;
    app.anim = false;
    app.update();

    RenderChain* chain = app.getRenderChain();
    chain->basefname = "Eccentric Light ";
    chain->currentseq = 1;
    chain->currentframe = 1;
    chain->renderoutput = false;

    Scene* scene1 = app.newScene();
    Scene* scene2 = app.newScene();
    Astronomy* astro = app.newAstronomy();
    astro->setTime(2021, 11, 7.0, 9.0, 0.0, 0.0, true);

    RenderLayer3D* layer1 = chain->newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene1, astro);
    RenderLayer3D* layer2 = chain->newLayer3D(0.0f, 0.0f, 1.0f, 1.0f, scene2, astro);  // How to draw without depthcheck? And with a nice border? !!!

    RenderLayerTextLines* lines = new RenderLayerTextLines();
    char dstring[] = "1234567890123456789012345678901234567890";
    std::string eccen = "Ecc:";
    lines->addLine(eccen);
    RenderLayerText* text = chain->newLayerText(0.0f, 0.0f, 1.0f, 1.0f, lines);
    text->setFont(app.m_font2);
    text->setCelestialMech(astro);  // Probably change the method name !!!

    RenderLayerGUI* gui = chain->newLayerGUI(0.0f, 0.0f, 1.0f, 1.0f);
    //gui->addScene(scene1, "Earth Scene"); // Should this be addLayer3D instead? It has the Astronomy and the Scene, the GUI can then also control the view
    //gui->addScene(scene2, "Solsys Scene");
    gui->addLayer3D(layer1, "Earth Scene"); // Should this be addLayer3D instead? It has the Astronomy and the Scene, the GUI can then also control the view
    gui->addLayer3D(layer2, "Solsys Scene");

    SkySphere* sky = scene1->newSkysphere(30, 60);
    Earth* earth1 = scene1->newEarth("NSAE", 90, 45);  // Is this better than above? !!! Depends if more than one Earth is allowed
    earth1->do_eccentriclight = false;
    earth1->addEquator();
    earth1->addGrid();
    //earth1->addArrow3DTrueSun();
    earth1->addSubsolarPoint();
    unsigned int lg = earth1->addLocGroup();
    Location* l1 = earth1->locgroups[lg]->addLocation(0.0, 0.0, false, 0.2f);
    //Location* l1 = earth->locgroups[lg]->addLocation(l_phuket.lat, l_phuket.lon, false, 0.2f);
    //l1->addLocDot();
    //l1->addArrow3DTrueSun();
    l1->addNormal(0.3f);
    LLH sunloc;
    sunloc = astro->getDecGHA(SUN);
    if (sunloc.lon < pi) sunloc.lon = -sunloc.lon;
    else sunloc.lon = tau - sunloc.lon;               // subsolar.lon is now -pi to pi east of south
    l1->moveLoc(sunloc.lat, sunloc.lon);

    SolarSystem* solsys = scene2->getSolsysOb();
    solsys->addTrails(400);
    solsys->trails = true;

    //unsigned int longrp = earth1->addLocGroup();
    //Location* l = nullptr;
    //for (double dLon = -30.0; dLon <= 30.0; dLon += 5.0) {
    //    l = earth1->locgroups[longrp]->addLocation(dLon, 45.0, false, 0.2f);
    //    l->addLocDot();
    //    l->addNormal();
    //    l->addArrow3DTrueSun();
    //}

    chain->renderoutput = false;


    //1 year insolation
    unsigned int i = 0;
    unsigned int steps = 366;
    //Lerper<double> hr = Lerper<double>(0.0, 24.0, steps, true);
    for (i = 0; i < steps; i++) {
        astro->addTime(1.0, 0.0, 0.0, 0.0); // A day per frame
        scene1->w_camera->CamUpdate();
        scene2->w_camera->CamUpdate();
        solsys->Update();
        sprintf_s(dstring, "Eccentricity: %02.3f\n", app.currentEarth->eccen);
        eccen = dstring;
        sunloc = astro->getDecGHA(SUN);
        if (sunloc.lon < pi) sunloc.lon = -sunloc.lon;
        else sunloc.lon = tau - sunloc.lon;               // subsolar.lon is now -pi to pi east of south
        l1->moveLoc(sunloc.lat, sunloc.lon);
        chain->do_render();
    }


    chain->renderoutput = false;

    // Calculate the obliqity of the ecliptic (Earth axis tilt) at the time of Eratosthenes (240BC)
    //astro->setTime(-240, 6, 20, 22.0, 30.0, 0.0, true);  // UTC no DST
    //double obliq = astro->TrueObliquityOfEcliptic(astro->getJD());
    //std::cout << "Mean obliquity of Ecliptic: " << obliq << " \n";

    while (!glfwWindowShouldClose(app.window)) {
        // Camera transform - orbital
        app.update();
        sprintf_s(dstring, "Eccentricity: %02.3f\n", app.currentEarth->eccen);
        eccen = dstring;
        //astro->setTimeNow();
        scene1->w_camera->CamUpdate();
        scene2->w_camera->CamUpdate();
        solsys->Update();

        // Update time ticks etc
        if (app.anim) astro->addTime(sidereald, 0.0, 0.0, 0.0); // dhms
        //if (world.GetSolsysOb() != nullptr) world.GetSolsysOb()->Update();

        sunloc = astro->getDecGHA(SUN);
        if (sunloc.lon < pi) sunloc.lon = -sunloc.lon;
        else sunloc.lon = tau - sunloc.lon;               // subsolar.lon is now -pi to pi east of south
        l1->moveLoc(sunloc.lat, sunloc.lon);

        chain->do_render();
    }
    delete lines;
}












// Testing area
void TestArea1(World& world) {

    // Shall explain the basics of the Sun as observed on Earth

    //const float latlonwidth = 0.002f;

    // Local vars here, to get the types done
    //unsigned int i = 0;      // Used in local loops everywhere
    //unsigned int frames = 0; // ditto
    //
    //double dLat = 0.0;
    //double dLon = 0.0;
    //
    //std::vector<Location> locs;
    //locs.reserve(60);  //icsts.size());
    //
    //struct icst {
    //    double lat;
    //    double ele;
    //    double azi;
    //};
    //std::vector<icst> icsts =
    //    /* Latitude   Elevation      Azimuth */
    //{ { -45.2000000, 21.2429888, 180.0000000},
    //  { -41.8000000, 23.8059435, 180.0000000},
    //  { -37.8000000, 28.8022570, 180.0000000},
    //  { -35.2000000, 30.8081089, 180.0000000},
    //  { -30.0000000, 36.8698977, 180.0000000},
    //  { -27.4000000, 39.2373672, 180.0000000},
    //  { -27.2000000, 40.5391518, 180.0000000},
    //  {  34.0000000, 80.4186013,   0.0000000},
    //  {  34.0000000, 81.4225444,   0.0000000},
    //  {  34.1000000, 79.6738896,   0.0000000},
    //  {  37.0000000, 76.9447529,   0.0000000},
    //  {  37.8000000, 75.9637566,   0.0000000},
    //  {  42.0000000, 69.0265067,   0.0000000},
    //  {  44.1000000, 69.7963849,   0.0000000},
    //  {  44.2000000, 69.3602809,   0.0000000},
    //  {  45.4000000, 66.8643028,   0.0000000},
    //  {  45.6000000, 68.4872942,   0.0000000},
    //  {  47.0000000, 65.2604912,   0.0000000},
    //  {  50.9000000, 61.3360430,   0.0000000},
    //  {  52.2000000, 61.0018777,   0.0000000},
    //  {  52.3000000, 60.9592971,   0.0000000},
    //  {  52.5000000, 60.7199246,   0.0000000},
    //  {  53.4000000, 58.8150254,   0.0000000},
    //  {  53.6000000, 60.2093960,   0.0000000},
    //  {  53.8000000, 62.8448447,   0.0000000},
    //  {  59.2000000, 54.2587455,   0.0000000},
    //  {  59.4000000, 53.7461623,   0.0000000},
    //  {  61.5000000, 52.3139138,   0.0000000},
    //  {  61.5000000, 53.7197466,   0.0000000},
    //  {  66.9000000, 46.6683375,   0.0000000} };
    //
    //const unsigned int DATA1 = 1; // Unique id for generic location arrows, in case we need more than one kind

    // Initial camera viewpoint
    //app.camFoV = 6.0f;   // All in Degrees
    //app.camDist = 25.0f; // 15.175f;
    //app.camLat = 0.0f;
    //app.camLon = 0.0f;
    //world.CamUpdate();

    // Setup Eartharium environment
    //world.SetTime(2021, 3, 20, 14.0, 33.0, 0.0);  // UTC no DST
    //world.GetCelestialOb()->setTime(2021, 10, 11, 22.0, 30.0, 0.0, true);  // UTC no DST
    // :TrueObliquityOfEcliptic(double JD)
    //world.SetTimeNow();
    //app.shadows = NONE;
    //world.shadows = SHADOW_MAP;
    //world.shadows = SHADOW_BOX;

    //ShadowMap* shadowmap = new ShadowMap(&world, 2048, 2048);
    //ShadowBox* shadowbox = new ShadowBox(&world, 2048, 2048);

    //SkyBox* sky = new SkyBox(&world);

    //SkySphere* sky = new SkySphere(&world,180,90);
    //sky->addDotDecRA(-1.20191913889, 84.0533889583, false);
    //sky->addStars();

    //Earth* earth = new Earth(&world, "NSAE", 360, 180);
    //earth->flatsunheight = 4500.0f;

    //app.gui = false;
    //app.datetimegui = true;
    //world.do_GUI();

    // Start building frames and sequences
    //world.render = false;
    //world.do_render();

    // float latlonwidth = 0.008f;
    //glm::vec4 gridcolor = LIGHT_GREY;
    //glm::vec4 zerocolor = LIGHT_RED;
    //glm::vec4 tropcolor = LIGHT_YELLOW;
    //glm::vec4 arctcolor = AQUA;
    //earth->addGrid(15.0f, latlonwidth, gridcolor, "LALO", /*rad*/false, /*eq*/true, /*pm*/true);
    //earth->addEquator(0.004f, zerocolor);
    //earth->addPrimeMeridian(0.004f, zerocolor);
    //earth->addTropics(0.004f, tropcolor);
    //earth->addArcticCirles(0.004f, arctcolor);
    //LLH llh1{ -10.0,-10.0,0.0 };
    //LLH llh2{ 30.0,70.0,0.0 };

    //earth->addGreatArc(llh1, llh2, YELLOW, 0.003f, false);
    //earth->addLerpArc(llh1, llh2, ORANGE, 0.003f, false);
    //earth->addFlatArc(llh1, llh2, RED, 0.003f, false);

    // Center star in Orion's belt: Alnilam (RA, Dec) = (84.0533889583, -1.20191913889) # 05h36m12.81335s -01°12'06.9089" eps Ori 

    //double londeg = 15.0; //location spacing in degrees
    //float locskyradius = 0.2f;
    //float arrowlen = 2.2f;
    //float arrowwidth = 0.003f;
    //glm::vec4 arrowcolor = LIGHT_YELLOW;
    //earth->addSubsolarPoint();
    //earth->addSubsolarPole();
    //earth->addSubsolarCone_NS(glm::vec4(1.0f, 1.0f, 0.3f, 0.5f));
    //earth->addTerminatorTrueSun();
    //earth->addSublunarPoint();
    //earth->addArrow3DTrueSun();
    //earth->flatsunheight = 5000.0f; // In kilometers
    //unsigned int longrp = earth->addLocGroup();
    //Location* myloc = earth->locgroups[longrp]->addLocation(l_gnw.lat, l_gnw.lon, false, locskyradius);
    //myloc->addLocDot();
    //myloc->addArrow3DTrueSun();
    //myloc->addArrow3DRADec(1, 84.0533889583, -1.20191913889, LIGHT_BLUE, 0.003f, 15.0f);
    //myloc->addArrow3DRADec(2, deg2rad * 201.298247375, deg2rad * -11.1613194722, LIGHT_BLUE, 0.003f, 15.0f);
    //Location* l = nullptr;
    //for (dLon = -90.0; dLon <= 90.0; dLon += londeg) {
    //    l = earth->locgroups[longrp]->addLocation(dLon, dLon, false, locskyradius);
    //
    //    l->addLocDot();
    //    //l->addUpCoord();
    //    //l->addMeridianPlane();
    //    //l->addTangentPlane();
    //    //l->addLocSky();
    //    //l->addArrow3DRADec(1, 84.0533889583, -1.20191913889, LIGHT_BLUE, 0.003f, 15.0f);
    //    //l->addArrow3DTruePlanet(SUN);
    //    //l->addPlanetTruePath24(SUN);
    //    //l->deletePlanetTruePath24(SUN);
    //    l->addArrow3DTruePlanet(MERCURY);
    //    l->addPlanetTruePath24(MERCURY);
    //    l->addPlanetTruePathSidYear(MERCURY);
    //    l->addArrow3DTruePlanet(VENUS);
    //    l->addPlanetTruePath24(VENUS);
    //    //l->deletePlanetTruePath24(MERCURY);
    //    l->addPlanetTruePathSidYear(VENUS);
    //    //l->addArrow3DTruePlanet(MARS);
    //    //l->addPlanetTruePath24(MARS);
    //    //l->addPlanetTruePathSidYear(MARS);
    //    //l->addArrow3DTruePlanet(JUPITER);
    //    //l->addArrow3DTruePlanet(SATURN);
    //    //l->addPlanetTruePathSidYear(SATURN);
    //    //l->addArrow3DTruePlanet(URANUS);
    //    //l->addArrow3DTruePlanet(NEPTUNE);
    //    //l->addDot3DTrueSun(SUNCOLOR);
    //    l->addArrow3DTrueSun();
    //    l->addPath3DTrueSun();
    //    //l->addArrow3DFlatSun();
    //    //l->addPath3DFlatSun();
    //    //l->addLine3DFlatSun();
    //    //l->addAnalemma3DTrueSun(LIGHT_YELLOW, 0.002f);
    //    //l->addArrow3DEleAzi(1, 0.0, 0.0, 1.0f, 0.003f, GREEN);
    //    //l->addArrow3DRADec(1, 0.0, 0.0, BLUE, 0.003f, 1.0f);
    //    //l->addArrow3DRADec(1, 84.0533889583, -1.20191913889, LIGHT_BLUE, 0.003f, 1.0f);
    //    //l->addPath3DRADec(1, deg2rad * 84.0533889583, deg2rad * -1.20191913889, LIGHT_BLUE, 0.003f);
    //    world.do_render();
    //}
    //for (auto& loc : earth->locgroups[longrp]->locations) {
    //    loc->deletePlanetTruePath24(MERCURY);
    //}
    //for (auto& loc : earth->locgroups[longrp]->locations) {
    //    loc->deletePlanetTruePath24(VENUS);
    //}


    //std::cout << "Scanning locations.\n";
    //std::vector<Location*> dloc;
    //for (auto& loc : earth->locgroups[longrp]->locations) {  // CRASH: Deleting an entry invalidates the loc iterator !!!
    //    //if (abs(loc->getLon(false)) > 50) earth->locgroups[longrp]->removeLocation(loc);
    //    if (abs(loc->getLon(false)) > 50) {
    //        std::cout << &loc << "\n";
    //        dloc.push_back(loc);
    //    }
    //}
    //std::cout << "Deleting locations.\n";
    //for (auto& l : dloc) {
    //    earth->locgroups[longrp]->removeLocation(l);
    //}
    //for (auto& l : earth->
    // locgroups[longrp]->locations) {
    //    std::cout << &l << "\n";
    //}
    //earth->Update();
    //earth.addLunarUmbraCone();
    //world.basefname = "Lesson 001 test";
    //world.currentseq = 1;
    //world.currentframe = 1;
    //
    //world.render = false;

    //// 24 hrs equinox
    ////unsigned int i = 0;
    //unsigned int steps = 30;
    //earth->w_sinsol = false;
    //earth->addSubsolarPoint();
    //earth->addSubsolarPole();
    //earth->flatsunheight = 3000.0f; // In kilometers
    //Lerper<double> hr = Lerper<double>(0.0, 24.0, steps, true);
    //Lerper<double> clat = Lerper<double>(0.0, 30.0, steps, false);
    //for (i = 0; i < steps*4;i++) {
    //    world.SetTime(2021, 9.0, 22.0, 12.0 + hr.getNext(), 1.0, 38.0);
    //    world.camLat = clat.getNext();
    //    world.CamUpdate();
    //    world.do_render();
    //}
    //world.render = false;

    //LLH loc = { deg2rad * 51.477811, deg2rad * 0.001475, 0.0 };  // Greenwich Observatory
    //LLH loc = { deg2rad * 40.7182, deg2rad * -74.0060, 0.0 };      // New York City
    //CelestialMech* astro = world.GetCelestialOb();

    //std::cout << "Enable Mercury: " << astro->enablePlanet(MERCURY) << "\n";
    //astro->enablePlanet(SUN);
    //astro->setTimeNow();
    //LLH pos = astro->getDecGHA(SUN);
    //LLH pos2 = astro->getDecRA(SUN);
    //LLH topos = astro->calcGeo2Topo(pos, loc);
    //char sDec[] = "-xxxdxxmxx.xxs";
    //char sRA[] = "-xxxhxxmxx.xxs";
    //char sGHA[] = "-xxxhxxmxx.xxs";
    //astro->stringRad2DMS(astro->rangepi2pi(pos.lat), sDec);
    //astro->stringRad2HMS(astro->rangezero2tau(pos2.lon), sRA);
    //astro->stringRad2HMS(astro->rangezero2tau(pos.lon), sGHA);
    //astro->stringRad2DMS(pos.lat, sDec);
    //astro->stringRad2HMS(pos2.lon, sRA);
    //astro->stringRad2HMS(pos.lon, sGHA);
    //std::cout << "Sun (GHA, RA, Dec): (" << sGHA << ", " << sRA << ", " << sDec << ")\n";
    //char sAzi[] = "-xxxhxxmxx.xxs";
    //char sEle[] = "-xxxhxxmxx.xxs";
    //astro->stringRad2DMS(astro->rangezero2tau(topos.lon), sAzi);
    //astro->stringRad2DMS(astro->rangepi2pi(topos.lat), sEle);
    //astro->stringRad2DMS(topos.lon, sAzi);
    //astro->stringRad2DMS(topos.lat, sEle);
    //astro->stringDeg2DMS(topos.lon + 180, sAzi);
    //astro->stringDeg2DMS(topos.lat, sEle);
    //std::cout << "Sun (Azi, Ele): (" << sAzi << ", " << sEle << ")\n";
    //// Analemma
    ////unsigned int i = 0;
    ////unsigned int steps = 300;
    //steps = 30;
    //Lerper<double> doy = Lerper<double>(0.0, 365.0, steps, false); // day of year
    //earth->addSubsolarPoint();
    //earth->addSubsolarPole();
    //earth->flatsunheight = 3000.0f; // In kilometers
    //for (i = 0; i < steps;i++) {
    //    world.SetTime(2021, 6.0, 20.0 + round(doy.getNext()), 12.0, 1.0, 38.0);
    //    world.do_render();
    //}


    //world.breakpoint = true;


    //app.ipython = false; // drop to interactive python console before exiting script
    //py::scoped_interpreter guard{};
    //py::object scope = py::module_::import("__main__").attr("__dict__");
    //py::eval_file("c:\\Coding\\Eartharium\\Eartharium\\hello.py", scope);

    // -- Test Scene mechanics and tigtvec.
    //SceneTree myscene;
    //
    //SceneObject ob0;
    //SceneObject ob1;
    //SceneObject ob2;
    //SceneObject ob3;
    //SceneObject ob4;
    //
    //ob0.name = "ob0";
    //ob1.name = "ob1";
    //ob2.name = "ob2";
    //ob3.name = "ob3";
    //ob4.name = "ob4";
    //
    //myscene.addObject(&ob0);
    //myscene.addObject(&ob1);
    //myscene.addObject(&ob2);
    //myscene.addObject(&ob3);
    //myscene.removeObject(&ob1);
    //myscene.addObject(&ob4);
    //myscene.printObjects();

    //world.clearWorld();
    //SkySphere* sky2 = new SkySphere(&world, 180, 90);
    //sky2->addDotDecRA(84.0533889583, -1.20191913889, false);

    //SolarSystem* solsys = new SolarSystem(&world, /* geocentric */ true);
    //solsys->addTrails(366);
    //solsys->addOrbits();
    //// Distance vectors - see: https://en.wikipedia.org/wiki/Apsis#Other_planets
    //solsys->AddDistLine(EARTH, VENUS, EARTHCOLOR, 0.003f); // Varies from 40 to 261 Gm (million km) 652%
    //solsys->AddDistLine(SUN, VENUS,    SUNCOLOR, 0.003f);     // Varies 1.3%
    //solsys->AddDistLine(EARTH, MARS, EARTHCOLOR, 0.003f);  // Varies from 62 to 401 Gm -> 646%
    //solsys->AddDistLine(SUN, MARS, SUNCOLOR, 0.003f);      // Varies 17%
    //solsys->AddDistLine(MARS, VENUS, WHITE, 0.003f);

    //char dstring[25];
    //std::cout << std::fixed << std::setprecision(6);
    //double mercuryKE, mercuryKW, mercuryDTE, mercuryDTW, mercuryEastElon, mercuryWestElon;
    //mercuryKE = CAAPlanetaryPhenomena::K(2020, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, CAAPlanetaryPhenomena::EventType::EASTERN_ELONGATION);
    //mercuryEastElon = CAAPlanetaryPhenomena::ElongationValue((int)mercuryKE, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, /* Eastern */ true);
    //mercuryDTE = CAAPlanetaryPhenomena::True(mercuryKE, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, CAAPlanetaryPhenomena::EventType::EASTERN_ELONGATION);
    //std::cout << "Mercury Max Eastern Elongation 2020: " << mercuryEastElon << " at Dynamical Time: " << mercuryDTE << " (";
    //world.JulianDateTimeString(mercuryDTE, dstring);
    //std::cout << "UTC: " << dstring << ") \n";
    //mercuryKW = CAAPlanetaryPhenomena::K(2020, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, CAAPlanetaryPhenomena::EventType::WESTERN_ELONGATION);
    //mercuryWestElon = CAAPlanetaryPhenomena::ElongationValue((int)mercuryKW, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, /* Eastern */ false);
    //mercuryDTW = CAAPlanetaryPhenomena::True(mercuryKW, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, CAAPlanetaryPhenomena::EventType::WESTERN_ELONGATION);
    //std::cout << "Mercury Max Western Elongation 2020: " << mercuryWestElon << " at Dynamical Time: " << mercuryDTW << " (";
    //world.JulianDateTimeString(mercuryDTW, dstring);
    //std::cout << "UTC: " << dstring << ") \n";
    //mercuryKE = CAAPlanetaryPhenomena::K(2021, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, CAAPlanetaryPhenomena::EventType::EASTERN_ELONGATION);
    //mercuryEastElon = CAAPlanetaryPhenomena::ElongationValue((int)mercuryKE, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, /* Eastern */ true);
    //mercuryDTE = CAAPlanetaryPhenomena::True(mercuryKE, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, CAAPlanetaryPhenomena::EventType::EASTERN_ELONGATION);
    //std::cout << "Mercury Max Eastern Elongation 2021: " << mercuryEastElon << " at Dynamical Time: " << mercuryDTE << " (";
    //world.JulianDateTimeString(mercuryDTE, dstring);
    //std::cout << "UTC: " << dstring << ") \n";
    //mercuryKW = CAAPlanetaryPhenomena::K(2021, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, CAAPlanetaryPhenomena::EventType::WESTERN_ELONGATION);
    //mercuryWestElon = CAAPlanetaryPhenomena::ElongationValue((int)mercuryKW, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, /* Eastern */ false);
    //mercuryDTW = CAAPlanetaryPhenomena::True(mercuryKW, CAAPlanetaryPhenomena::PlanetaryObject::MERCURY, CAAPlanetaryPhenomena::EventType::WESTERN_ELONGATION);
    //std::cout << "Mercury Max Western Elongation 2021: " << mercuryWestElon << " at Dynamical Time: " << mercuryDTW << " (";
    //world.JulianDateTimeString(mercuryDTW, dstring);
    //std::cout << "UTC: " << dstring << ") \n";
    //
    //double startJD, endJD;
    //startJD = CAADate(2020, 1, 1, true).Julian();
    //endJD = CAADate(2022, 1, 1, true).Julian();
    //std::vector<CAAPlanetaryPhenomenaDetails2> mercuryVals = CAAPlanetaryPhenomena2::Calculate(startJD, endJD, CAAPlanetaryPhenomena2::Object::MERCURY, 0.007, false);
    //for (auto& val : mercuryVals) {
    //    if (val.type == CAAPlanetaryPhenomenaDetails2::Type::GreatestEasternElongationInAngularDistance) {
    //        std::cout << "Type: " << "GreatestEasternElongationInAngularDistance(" << (int)val.type << ")\n";
    //        std::cout << "Julian: " << val.JD << "\n";
    //        //std::cout << "UTC: " << CAADate(val.JD, true).Year() << "-" << CAADate(val.JD, true).Month() << "-" << CAADate(val.JD, true).Day()
    //        //    << " " << CAADate(val.JD, true).Hour() << ":" << CAADate(val.JD, true).Minute() << ":" << CAADate(val.JD, true).Second() << "\n";
    //        world.JulianDateTimeString(val.JD, dstring);
    //        std::cout << "UTC: " << dstring << "\n";
    //        std::cout << "Value: " << val.Value << "\n";
    //    }
    //    if (val.type == CAAPlanetaryPhenomenaDetails2::Type::GreatestWesternElongationInAngularDistance) {
    //        std::cout << "Type: " << "GreatestWesternElongationInAngularDistance(" << (int)val.type << ")\n";
    //        std::cout << "Julian: " << val.JD << "\n";
    //        world.JulianDateTimeString(val.JD, dstring);
    //        std::cout << "UTC: " << dstring << "\n";
    //        std::cout << "Value: " << val.Value << "\n";
    //    }
    //    //std::cout << "Type: " << (int)val.type << "\n";
    //    //std::cout << "Julian: " << val.JD << "\n";
    //
    //    //if (val.type == CAAPlanetaryPhenomenaDetails2::Type::GreatestEasternElongationInEclipticLongitude) {
    //    //    std::cout << ""
    //    //}
    //}
    // Drop into experimental environment after render, so next scene can be planned
    //app.render = false;
    //app.gui = true;
    //app.datetimegui = true;

    //CelestialPath test1(0.0, 1.0, 10);
    //CelestialPath test2(0.0, 1.0, 10);
    //if (test1 == test2) std::cout << "They are the same!\n";
    //else std::cout << "They are NOT the same!\n";


    //std::cout << "\n\n\nEntering terminal while loop.\n";
    //const double dayfraction = 1.0 / 1.0; // 1440.0;  // Fraction of a day (24h) to advance each step
    //const double dayfraction = sidereald;  // Fraction of a day (24h) to advance each step
    //while (!glfwWindowShouldClose(app.window))  // && currentframe < 200) // && animframe < 366)
    //{
    //    // Camera transform - orbital
    //    world.CamUpdate();
    //    // Custom time updater that is Equation of Time aware
    //    if (world.do_eot) {
    //        // Update world time - Tracking local solar noon by accounting for EoT
    //        if (world.anim || world.plusday || world.minusday) {
    //            double delta = 1.0 * world.anim + 1.0 * world.plusday - 1.0 * world.minusday;
    //            world.GetCelestialOb()->addTime(delta * dayfraction, -world.oldslideday, world.eot, 0.0); // Move to next day & remove previous EoT adjustment
    //        }
    //        else {
    //            world.GetCelestialOb()->addTime(0.0, -world.oldslideday, world.eot, 0.0);
    //        }
    //        world.eot = CAAEquationOfTime::Calculate(world.GetCelestialOb()->getJD(), true);  // Calculate new EoT
    //        world.GetCelestialOb()->addTime(0.0, world.slideday, -world.eot, 0.0);   // Apply new EoT adjustment
    //    }
    //    if (!world.do_eot) {
    //        // Update world time - Tracking local solar noon by accounting for EoT
    //        if (world.eot != 0.0) {
    //            world.GetCelestialOb()->addTime(0.0, -world.oldslideday, world.eot, 0.0); // Remove previous EoT adjustment if any
    //            world.eot = 0.0;                         // Reset EoT
    //        }
    //        else world.GetCelestialOb()->addTime(0.0, -world.oldslideday, 0.0, 0.0);
    //
    //        if (world.anim || world.plusday || world.minusday) {
    //            double delta = 1.0 * world.anim + 1.0 * world.plusday - 1.0 * world.minusday;
    //            world.GetCelestialOb()->addTime(delta * dayfraction, world.slideday, 0.0, 0.0);     // Apply new non-EoT adjustment
    //        }
    //        else world.GetCelestialOb()->addTime(0.0, world.slideday, 0.0, 0.0);
    //    }
    //    world.plusday = false;
    //    world.minusday = false;
    //
    //    world.oldslideday = world.slideday;
    //
    //    // After calculating time, set global timestr with help of World
    //    //world.SetTimeNow();
    //    world.GetCelestialOb()->getTimeString((char *)world.timestr.c_str());
    //
    //    // Update and draw components
    //    //  Proper order is:
    //    //  - Update time dependent objects
    //    //  - Update composite objects
    //    //  - Update and Draw primitives
    //
    //    if (world.GetSolsysOb() != nullptr) world.GetSolsysOb()->Update();
    //    world.do_render();
    //}
}

Application* getApplication() {
    return &app;
}

PYBIND11_EMBEDDED_MODULE(eartharium, m) {
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
        .def("getRenderChain", &Application::getRenderChain, py::return_value_policy::reference)
        .def("newScene", &Application::newScene, py::return_value_policy::reference)
        .def("newAstronomy", &Application::newAstronomy, py::return_value_policy::reference)
        .def("update", &Application::update)
        .def_readwrite("ipython", &Application::ipython)
        .def_readwrite("render", &Application::render)
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
        .def("CamUpdate", &Camera::CamUpdate, "Updates the Camera settings from Application etc")
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
        .def("setCelestialMech", &RenderLayerText::setCelestialMech, "Sets the Astronomy keeping time, if synchronized UTC display is desired.",
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
    py::class_<RenderChain>(m, "RenderChain")
        .def("newLayer3D", &RenderChain::newLayer3D, "Creates a new 3D Render Layer",
            py::arg("vpx1"), py::arg("vpy1"), py::arg("vpx2"), py::arg("vpy2"),
            py::arg("scene"), py::arg("astro"), py::arg("cam") = (Camera*) nullptr,
            py::return_value_policy::reference
        )
        .def("newLayerText", &RenderChain::newLayerText, "Creates a new Text Render Layer",
            py::arg("vpx1"), py::arg("vpy1"), py::arg("vpx2"), py::arg("vpy2"),
            py::arg("lines") = (RenderLayerTextLines*) nullptr,
            py::return_value_policy::reference
        )
        .def("do_render", &RenderChain::do_render)
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

    app.start_fullscreen = false;
    if (app.initWindow() == -1) return -1; // Bail if Window and OpenGL context setup fails

    if (false) {
        app.interactive = false;
        //std::string pyfile = "c:\\Coding\\Eartharium\\Eartharium\\hello.py";
        std::string pyfile = "c:\\Coding\\Eartharium\\Eartharium\\Lesson001.py";
        py::scoped_interpreter guard{};
        py::object scope = py::module_::import("__main__").attr("__dict__");
        py::eval_file(pyfile.c_str(), scope);
        IdleArea(app);
    }

    //TestArea5(app);

    AngleArcsDev(app);

    // Cleanup
    glfwTerminate();

    return 0;
}


// Textures
// --------
// Shadows = 1, SkyBox = 2, Earth = 3 + 4, Sundot = 5, Moondot = 6, PiP = 7.


// ----------------
//  GLFW Callbacks
// ----------------
void keyboard_callback(GLFWwindow*, int key, int scancode, int action, int mods) {
    // Mapped Keys: <ESC>,<SPACE>,a,c,d,e,f,g,h,j,m,n,p,q,s,w,z
    // Unmapped Keys: b,i,k,l,o,r,u,v,x,y,1,2,3,4,5,6,7,8,9,0
    if (action == GLFW_RELEASE || action == GLFW_REPEAT) {
        // ESC - End application - may move to end animation when timeline edits have been implemented
        if (key == GLFW_KEY_ESCAPE) { glfwSetWindowShouldClose(app.window, true); }
        //if (key == GLFW_KEY_L) toggle = !toggle;
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
            app.currentCam->camLon -= app.camSpd;
            if (app.currentCam->camLon > 180.0f) app.currentCam->camLon -= 360.0f;
            if (app.currentCam->camLon < -180.0f) app.currentCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_D && app.currentCam != nullptr) {
            app.currentCam->camLon += app.camSpd;
            if (app.currentCam->camLon > 180.0f) app.currentCam->camLon -= 360.0f;
            if (app.currentCam->camLon < -179.9f) app.currentCam->camLon += 360.0f;
        }
        if (key == GLFW_KEY_W && app.currentCam != nullptr) {
            app.currentCam->camLat += app.camSpd;
            if (app.currentCam->camLat > CAMERA_MAX_LATITUDE) app.currentCam->camLat = CAMERA_MAX_LATITUDE;
        }
        if (key == GLFW_KEY_S && app.currentCam != nullptr) {
            app.currentCam->camLat -= app.camSpd;
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
