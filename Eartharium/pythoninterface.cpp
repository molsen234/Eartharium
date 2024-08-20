
// -----------------------
//  Pybind11 requirements
// -----------------------
#include <python310/Python.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>
namespace py = pybind11;

//  Self
#include "pythoninterface.h"

// -------------------------------------
//  Used in definitions of Python calls
// -------------------------------------
// External libs
#include <glm/gtx/string_cast.hpp>

// base astronomy functions - these are now in a separate stand-alone pyeastronomy module
//#include "astronomy/datetime.h"
//#include "astronomy/acoordinates.h"
//#include "astronomy/aearth.h"
//#include "astronomy/aelliptical.h"

// Eartharium stuff
//#include "OpenGL.h"
#include "Primitives.h"
#include "Earth.h"
#include "Astronomy.h"
//#include "Utilities.h"
//#include "FigureGeometry.h"


// Access Application which is in Eartharium.cpp
extern Application app;

// ------------------------
//  Python interface class
// ------------------------

void PyIface::runscript(std::string& pythonscript) {
    //std::cout << "Running Python script: " << pythonscript << '\n';
    //m_app->interactive = false;
    py::scoped_interpreter guard{};
    py::object scope = py::module_::import("__main__").attr("__dict__");
    py::eval_file(pythonscript.c_str(), scope);
}
Application* PyIface::getApplication() {
    // A bit of an awkward way to get the current Application to Python, but less awkward than a global function
    //std::cout << "PyIface::getApplication(): was called!\n";
    return &app;
}


// !!! FIX: Might split the below up into includes that live next to the relevant code.
//          That makes it easier to e.g. split off eastronomy into a separate library / module

// -------------------------------------
//  Python scripting module definitions
// -------------------------------------
PYBIND11_EMBEDDED_MODULE(eartharium, m) {
    // IMPORTANT: Define things in order of dependencies. If not, loading the module will hang with no errors. Very annoying!
    m.doc() = "Eartharium Module";
    m.def("getApplication", &PyIface::getApplication, py::return_value_policy::reference)
        ;
    py::class_<ImFont>(m, "Font") // From https://toscode.gitee.com/lilingTG/bimpy/blob/master/bimpy.cpp it has lots of ImGui .def's
        .def(py::init())
        ;

    // NOTE: When using the below without pyeastronomy, at least load the following types:
    //       from pyeastronomy import LLD,dvec3,vec3,vec4

    // -----------------
    //  Astronomy class
    // -----------------
    py::class_<Astronomy>(m, "Astronomy")
        .def(py::init<>())
        .def("setTime", &Astronomy::setTime, "Sets time in (Y,M,D,h,m,s.ss)",
            py::arg("yr"), py::arg("mo"), py::arg("da"), py::arg("hr"), py::arg("mi"), py::arg("se")
        )
        .def("setTimeNow", &Astronomy::setTimeNow, "Sets time to system clock in UTC")
        .def("setJD_TT", &Astronomy::setJD_TT, "Sets time to Julian Day in TT",
            py::arg("jd")
        )
        .def("setJD_UTC", &Astronomy::setJD_UTC, "Sets time to Julian Day in UTC",
            py::arg("jd")
        )
        .def("getJD_TT", &Astronomy::getJD_TT, "Returns the current JD in TT")
        .def("getJD_UTC", &Astronomy::getJD_UTC, "Returns the current JD in UTC")
        .def("addTime", &Astronomy::addTime, "Adjusts current time by provided amount",
            py::arg("d"), py::arg("h"), py::arg("min"), py::arg("sec"), py::arg("eot") = false
        )
        .def("ApparentGreenwichSiderealTime", &Astronomy::ApparentGreenwichSiderealTime, "Returns Greenwich Sidereal Time for provided JD (in UTC)",
            py::arg("jd_utc"), py::arg("rad")
        )
        .def("MeanGreenwichSiderealTime", &Astronomy::MeanGreenwichSiderealTime, "Returns Greenwich Sidereal Time for provided JD (in UTC), or for current JD if omitted",
            py::arg("jd_utc"), py::arg("rad")
        )
        .def("getTimeString", &Astronomy::getTimeString, "Returns current time & date in string format YYYY-MM-DD HH:MM:SS UTC")
        //LLD calcDecHA2GP(LLD decra);
        .def("calcDecHA2GP", &Astronomy::calcDecHA2GP, "Declination and Hour Angle to Ground Point",
            py::arg("decha")
        )
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
    py::class_<RenderLayerGUI>(m, "RenderLayerGUI")
        .def("addLayer3D", &RenderLayerGUI::addLayer3D, "Adds a Layer3D so the GUI can read the Scene objects.",
            py::arg("layer"), py::arg("layername")
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
        // RenderLayerGUI* newLayerGUI(float vpx1, float vpy1, float vpx2, float vpy2);
        .def("newLayerGUI", &Application::newLayerGUI, "Creates a new GUI Render Layer",
            py::arg("vpx1"), py::arg("vpy1"), py::arg("vpx2"), py::arg("vpy2"),
            py::return_value_policy::reference
        )
        .def("render", &Application::render)
        .def("shouldClose", &Application::shouldClose)
        .def_readwrite("anim", &Application::anim)
        .def_readwrite("interactive", &Application::interactive)
        .def_readwrite("render_out", &Application::renderoutput)
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
        .def("addEcliptic", &Earth::addEcliptic, "Adds the Ecliptic. Currently doesn't take any parameters.")
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


};

