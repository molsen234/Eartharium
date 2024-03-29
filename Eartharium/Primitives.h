#pragma once

#include <iterator>
#include <vector>
#include <list>
#include <queue>
#include <map>

#include "ImGUI/imgui.h"         // For ImFont

#include "OpenGL.h"
#include "Utilities.h"
#include "config.h"

struct Primitive3D {
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 direction = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(0.0f);
    float rotation = 0.0f;
};

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    Vertex(glm::vec3 pos, glm::vec3 nml, glm::vec2 tex) : position(pos), normal(nml), uv(tex) {}
};

struct Tri {
    unsigned int a;
    unsigned int b;
    unsigned int c;
    Tri(unsigned int va, unsigned int vb, unsigned int vc) : a(va), b(vb), c(vc) {}
};



// Protos
class Arrows;
class Application;
class AngleArcs;
class Astronomy;
class BodyGeometry;
class Camera;
class Cones;
class CountryBorders;
class Constellations;
class Cylinders;
class DetailedEarth;
class DetailedMoon;
class DetailedSky;
class Dots;
class Earth;
class Font;
class Glyphs;
class Location;
class Minifigs;
class Planes;
class Planetoid;
class PolyCurve;
class PolyLine;
class Primitives;
class Scene;
class SceneObject;
class SceneTree;
class ShadowBox;
class ShadowMap;
//class ShapeFile;
class SkyBox;
class SkyDots;
class SkySphere;
class SolarSystem;
class SphereUV;
class StarTrail;
class TextFactory;
class TextString;
class ThreePointSolver;
class TimeZones;
class ViewCones;


// --------
//  Lerper
// --------
template<typename T>
class Lerper {
    T m_first;
    T m_last;
    T m_inc;

    int m_steps;
    int m_counter;
    bool m_restart = false;
public:
    Lerper<T>(T first, T last, int steps, bool restart);
    T getNext();
    T getNextSmooth();
};
template<typename T>
Lerper<T>::Lerper(T first, T last, int steps, bool restart) :
    m_first(first), m_last(last), m_steps(steps), m_restart(restart)
{
    m_counter = 0;
    m_inc = (m_last - m_first) / (m_steps - 1);
}
template<typename T>
T Lerper<T>::getNext() {
    m_counter++;
    if (m_counter < m_steps) {
        return m_first + (T)(m_counter - 1) * m_inc;
    }
    if (m_restart) m_counter = 0;
    return m_last;
}
template<typename T>
T Lerper<T>::getNextSmooth() {
    m_counter++;
    // x is running parameter
    // k = max(0, min(1, (x-t1)/(t2-t1)))
    // y = k*k*(3.0-2.0*k)
    // y = 0 for x<=t1
    // y = 1 for x>=t2
    // y smoothed increasing between
    // See: https://www.youtube.com/watch?v=60VoL-F-jIQ for details
    if (m_counter < m_steps) {
        double k = std::max(0.0, std::min(1.0, ((double)m_counter - 1.0) / ((double)m_steps - 1.0)));
        double s = k * k * (3.0 - 2.0 * k);
        return (T)(s * (double)m_last + (1.0 - s) * (double)m_first);
    }
    if (m_restart) m_counter = 0;
    return m_last;
}



//struct Animation {
//    // Should be templated so it can work with different objects: floats, doubles, vec3, vec4, LLH, etc.
//    // To work with LLH, LLH will need at least a + operator
//    double* param = nullptr;      // Pointer to parameter to be animated
//    //Lerper<T>* lerper = nullptr;  // Lerper that delivers the next value, constructed by Animator
//    double startvalue = 0.0;
//    double endvalue = 0.0;
//    double step = 0.0;
//    unsigned int sequence = 0;    // The sequence in which it is active
//    unsigned int startframe = 0;  // Frame to start on
//    unsigned int endframe = 0;    // Frame to end on
//    // Is it necessary to allow it to extend across sequences?
//    //  - No because it is unknown how many frames will be in current seq, so impossible to calculate a step size
//    //    Or, pass in a step value and end when reaching endvalue, leaving the frame count undetermined, thus a different mode.
//    // Might also take a PathTracker instead of a Lerper, controlled via a type field
//    // How about Astronomy time animations?
//    // Or to be a bit crazy, animated creation of objects?
//};
//class Animator {
//    // Contains a collection of Animation objects and advances them by one frame on animate() call
//    // Should be instantiated by Scene
//public:
//    Animator(Scene* scene) : m_scene(scene) {
//
//    }
//    Animation* addAnimation(double* parameter, double startval, double endval, unsigned int sequence, unsigned int startframe, unsigned int endframe) {
//        animations.push_back(new Animation());
//        animations.back()->param = parameter;
//        animations.back()->sequence = sequence;
//        animations.back()->startframe = startframe;
//        animations.back()->endframe = endframe;
//        animations.back()->startvalue = startval;
//        animations.back()->endvalue = endval;
//        animations.back()->step = (endval - startval) / (endframe - startframe);
//        return animations.back();
//    }
//    void animate() {
//        for (auto& a : animations) {
//            // If this simply adds step to param, then the first time inside the active interval must set the startvalue.
//            // That will possibly cause a sudden shift, so maybe implement an animateTo() where only the end value is given.
//            if (a->sequence == m_scene->m_app->currentseq && a->startframe <= m_scene->m_app->currentframe && a->endframe >= m_scene->m_app->currentframe) {
//                *a->param = *a->param + a->step;
//            }
//        }
//    }
//private:
//    Scene* m_scene = nullptr;
//    std::vector<Animation*> animations;
//};



// Protos
struct Viewport { // Currently unused, but intended for the RenderLayers
    GLint vp_x = 0;
    GLint vp_y = 0;
    GLint vp_w = 0;
    GLint vp_h = 0;
};


// -------
//  Scene
// -------
class Scene {
    // A Scene holds all the 3D Primitives, Objects, and Camera(s) - basically everything that has spatial or temporal relationships
public:
    SceneTree* scenetree = nullptr;
    Application* m_app = nullptr;
    Astronomy* astro = nullptr; // Why is this here? Oh, so Earth can look it up. Probably should have a RenderLayer3D pointer instead.
    Camera* w_camera = nullptr;
    unsigned int shadows = NONE;  // Create a way to update this from GUI or keyboard or python etc.
    SolarSystem* m_solsysOb = nullptr;
    Earth* m_earthOb = nullptr;
    SkySphere* m_skysphereOb = nullptr;
private:
    std::vector<Camera*> m_cameras;
    std::vector<PolyCurve*> m_polycurves;
    std::vector<PolyLine*> m_polylines;
    std::vector<ThreePointSolver*> m_threepointsolvers;
    float m_aspect = 1.0f;
    Minifigs* m_minifigsOb = nullptr;
    Dots* m_dotsOb = nullptr;
    SkyDots* m_skydotsOb = nullptr;
    Cylinders* m_cylindersOb = nullptr;
    Cones* m_conesOb = nullptr;
    ViewCones* m_viewconesOb = nullptr;
    Planes* m_planesOb = nullptr;
    Glyphs* m_glyphsOb = nullptr;
    SphereUV* m_sphereuvOb = nullptr;
    Arrows* m_arrowsOb = nullptr;
    AngleArcs* m_anglearcsOb = nullptr;
    SkyBox* m_skyboxOb = nullptr;
    CountryBorders* m_countrybordersOb = nullptr;
    TimeZones* m_timezonesOb = nullptr;
    ShadowMap* m_shadowmap = nullptr;
    ShadowBox* m_shadowbox = nullptr;
    TextFactory* m_textFactory = nullptr;
public:
    Scene(Application* app);
    ~Scene();
    Camera* newCamera(const std::string name = "Camera");
    void setAspect(float aspect);
    float getAspect();
    void clearScene();  // ToDo !!!
    void render(Camera* cam = nullptr);
    // Primitive Factories
    // - Some of these may be redundant as SceneObject implementation grows.
    Dots* getDotsFactory();
    SkyDots* getSkyDotsFactory();
    Cylinders* getCylindersFactory();
    Cones* getConesFactory();
    ViewCones* getViewConesFactory();
    Planes* getPlanesFactory();
    TextFactory* getTextFactory();
    SphereUV* getSphereUVFactory();
    Arrows* getArrowsFactory();
    AngleArcs* getAngleArcsFactory();
    CountryBorders* getCountryBordersFactory();
    TimeZones* getTimeZonesFactory();
    // Full objects - revise names !!!
    // - These should probably all be SceneObject derived. Then they will be drawn by SceneTree.
    //   SceneTree will then have to be aware of rendering shadows. The shadow system needs rebuilding anyway.
    SkySphere* newSkysphere(unsigned int mU, unsigned int mV, bool texture);
    SkySphere* getSkysphere();
    SkyBox* getSkyboxOb();
    SolarSystem* getSolsysOb();
    ShadowBox* getShadowboxOb();
    ShadowMap* getShadowmapOb();
    Earth* newEarth(std::string mode, unsigned int mU, unsigned int mV);
    Earth* getEarth(); // May return nullptr, so user must check!
    Minifigs* newMinifigs();
    // Use GenericPath or SmartPath instead of using PolyCurve directly?
    PolyCurve* newPolyCurve(glm::vec4 color, float width, size_t reserve = NO_UINT);
    void deletePolyCurve(PolyCurve* curve);
    PolyLine* newPolyLine(glm::vec4 color, float width, size_t reserve = NO_UINT);
    void deletePolyLine(PolyLine* curve);
    ThreePointSolver* newThreePointSolver(Earth* earth);
};


// -------------
//  SceneObject
// -------------
// Being implemented
// I have yet to fully work out the requirements ...
class SceneObject {
protected:
    //Material* material = nullptr;
    unsigned int id = 0;
public:
    bool hidden = false;  // Hide object by bailing early from draw();
    bool hasgui = false;  // Are there GUI components to render?
    std::string name = "Object";
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotations{0.0f, 0.0f, 0.0f};
    unsigned int rotationorder = XYZ;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 worldmatrix = glm::mat4(1.0f); // Collects parent worldmatrix and applies scale, rotation, position.
    glm::vec4 color = WHITE; // default color, not used by some derived objects
    Scene* scene = nullptr;
    SceneObject* m_parent = nullptr;
    std::list<SceneObject*> children;
    SceneObject(Scene* scene, SceneObject* parent);
    ~SceneObject();
    void addChild(SceneObject* object);
    void removeChild(SceneObject* child);
    // Safer to pass a parent when constructing. If there is ever a need to change parent, destroy the object and create a new one.
    // - Simply setting a parent after construction can lead to loops in the scene tree.
    //void setParent(SceneObject* parent);
    SceneObject* getParent();
    void setID(unsigned int oid);
    unsigned int getID();
    // setWorldMat() removed, as inherit() will overwrite due to being called in the render phase, after setWorldMat() can be called.
    //void setWorldMat(glm::mat4 mat);
    // worldmatrix is only up to date during inherit() calls, so should only be used in derived object's update() function, and nowhere else.
    glm::mat4 getWorldMat();
    void inherit();
    virtual bool update() {  // Return flag indicates if worldmatrix was built by update() true = yes, false = no
        // Override this to supply orientation parameters (i.e. placement relative to parent)
        std::cout << "SceneObject[" << this << "]::update() : derived object \"" << name << "\" has not overridden the update() function, and will not be updated.\n";
        return false;
    }
    void virtual draw(Camera* cam) = 0;
    void virtual myGUI() {};
private:
    SceneObject(Scene* scene, bool isroot = false); // Only used to construct the root in SceneTree.
    friend class SceneTreeRoot;
};


class SceneTreeRoot : public SceneObject {
// Just to have a place to stach the root (unparented) objects.
public:
    SceneTreeRoot(Scene* scene, bool isroot) : SceneObject(scene, isroot) {}
    void draw(Camera* cam) override {}
};

// -----------
//  SceneTree
// -----------
class SceneTree {
    Scene* m_scene = nullptr;
    std::queue<SceneObject*> breathfirst;
public:
    SceneObject* root = nullptr;
    SceneTree(Scene* scene);
    ~SceneTree();
    void updateBreathFirst();
    void drawBreathFirst(Camera* cam); // Honor hidden flag (skip render), and maybe support defer (render last) for semi-transparent objects.
    void guiBreathFirst();
    void addSceneObject(SceneObject* object, SceneObject* parent);
    void rootRemove(SceneObject* object);
    void printSceneTree();
    void doPrintSceneTree(std::list<SceneObject*> node, unsigned int level);
};


// --------
//  Camera
// --------
class Camera : public SceneObject {
public:
    //glm::vec3 m_position = glm::vec3(0.0f);
    // Used by GUI and keyboard, as well as programming from C++ and Python
    float camLat = 0.0;
    float camLon = 0.0;
    float camFoV = 6.0f;
    float camDst = 20.0f;
    float camNear = 1.0f;
    float camFar = 1500.0f;

    glm::vec3 CamLightDir = glm::vec3(0.0f);
    float camlightsep = 0.1f;
private:
    //Scene* m_scene = nullptr;
    glm::mat4 ProjMat = glm::mat4(1.0f);
    glm::mat4 ViewMat = glm::mat4(1.0f);
    glm::vec3 m_target = glm::vec3(0.0f);
    glm::vec3 m_direction = glm::vec3(0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // Part of worldUp that fits with cameraDirection
public:
    Camera(Scene* scene);
    void setLatLonFovDist(float lat, float lon, float fov, float dst);
    void setCamLightPos(glm::vec3 lPos);
    void setLatLon(float lat, float lon);
    bool update(); // Override SceneObject::update()
    void updateLight();
    void setLookAt(glm::vec3 position, glm::vec3 target, glm::vec3 upwards);
    void setPosXYZ(glm::vec3 pos);
    void setPosLLH(LLH llh);
    glm::vec3 getPosXYZ();
    void setTarget(glm::vec3 target);
    void setFoV(float fov);
    float getFoV();
    glm::mat4 getViewMat();
    glm::mat4 getSkyViewMat();
    glm::mat4 getProjMat();
    glm::vec3 getRight();
    glm::vec3 getUp();
    glm::vec3 getPosition();
    void dumpParameters(unsigned int frame = 0);
    void draw(Camera* cam) override {}
private:
    void Recalc();
    //Application* m_app = nullptr;
};


// -------------
//  RenderLayer - base class
// -------------
class RenderLayer {
public:
    unsigned int type = NONE;
protected:
    float view_x1 = 0.0f;  // Where in the viewport should this layer render, in [0.0f;1.0f] coordinates
    float view_y1 = 0.0f;
    float view_x2 = 0.0f;
    float view_y2 = 0.0f;
    float vp_x = 0.0f;  // The resulting glViewport() setting
    float vp_y = 0.0f;
    float vp_w = 0.0f;
    float vp_h = 0.0f;
    float width = 0.0f;
    float height = 0.0f;

    // Animation between positions
    bool animating = false;
    unsigned int frames_left = 0;
    float target_x1 = 0.0f;
    float target_y1 = 0.0f;
    float target_x2 = 0.0f;
    float target_y2 = 0.0f;

    RenderLayer(float vpx1, float vpy1, float vpx2, float vpy2);
    void animate();
public:
    void virtual render() = 0;  // Force children to implement a render() method
    void virtual updateViewport(float width, float height) = 0; // Implemented by children, they may have needs like updating the scene aspect ratio etc.
    void virtual animateViewport() = 0;
    void updateView(float w = 0.0f, float h = 0.0f);  // Called when window size changes
    void animateView(float vx1, float vy1, float vx2, float vy2, unsigned int frames);
};


// ---------------
//  RenderLayer3D
// ---------------
class RenderLayer3D : public RenderLayer {

    // For 3D objects, basically a layered replacement of World for containing the 3D scene objects and camera(s)
private:
    Application* m_app = nullptr;
    // renderable objects
public:
    // These should probably be private, but then we can't pick up the parameters after running a python script for the editor/experimental
    Scene* m_scene = nullptr;
    Camera* m_cam = nullptr;
    Astronomy* m_astro = nullptr;
    bool m_overlay = true; // true = don't clear background before drawing, false = clear to black. Set color somewhere !!!

    RenderLayer3D(float vpx, float vpy, float vpw, float vph, Scene* scene, Astronomy* astro, Camera* cam = nullptr, bool overlay = true);
    void setCamera(Camera* cam);
    void updateViewport(float width, float height);
    void animateViewport();
    void render();
};


// -----------------
//  RenderLayerText
// -----------------
class RenderLayerTextLines {
public:
    std::vector<std::string*> lines;
    void addLine(std::string& line) { lines.push_back(&line); }
    bool empty() { return lines.empty(); }
    unsigned int size() { return (unsigned int)lines.size(); }
    //std::vector<std::string*>::iterator begin() { return lines.begin(); }
};
class RenderLayerText : public RenderLayer {
public:
    std::string timestr;
    RenderLayerText(float vpx1, float vpy1, float vpx2, float vpy2, Application* app, RenderLayerTextLines* lines = nullptr);
    void setFont(ImFont* font);
    //void setFont(std::string* path, unsigned int size);
    void setAstronomy(Astronomy* astro);
    void updateViewport(float w, float h);
    void animateViewport();
    void render();
private:
    Application* m_app = nullptr;
    ImFont* m_font = nullptr;
    RenderLayerTextLines* m_lines = nullptr;
    Astronomy* m_astro = nullptr;
};


// ----------------
//  RenderLayerGUI
// ----------------
class RenderLayerGUI : public RenderLayer {
    Astronomy* m_astro = nullptr;
    Application* m_app = nullptr;
public:
    bool gui = true;            // Display interactive ImGUI
    bool datetimegui = true;    // Display small ImGUI with large date and time
    // ImGui
    ImFont* font1 = nullptr;
    ImFont* font2 = nullptr;
    std::string timestr;
    //char timestr[21]; // Does not specify the formatting, just creates a string of correct length

    // Time Controls
    bool do_eot = false;
    bool m_do_eot = false;
    //bool minusday = false;
    //bool plusday = false;
    float slideday = 0.0f;
    float prevslideday = 0.0f;

    float timeoffset = 0.0f;
    float prevtimeoffset = 0.0f;

    // Earth Controls
    float param = 0.0f;  // Make an Earth control struct, so GUI can display multiple Earths - include a name

    RenderLayerGUI(float vpx1, float vpy1, float vpx2, float vpy2, Application* app);
    void updateViewport(float width, float height) {};
    void animateViewport();
    //void addScene(Scene* scene, std::string scenename);
    void addLayer3D(RenderLayer3D* layer, std::string layername);
    void render();
private:
    //struct scenecache {
    //    Scene* scene = nullptr;
    //    std::string name = "";
    //};
    //std::vector<scenecache> m_scenes;
    struct layercache {
        RenderLayer3D* layer = nullptr;
        std::string name = "";
    };
    std::vector<layercache> m_layers;
};


// -----------------
//  RenderLayerPlot
// -----------------
struct TimePlotData {
    double utime;
    double data;
};
class RenderLayerPlot : public RenderLayer {
public:
    bool interactive = false;
    RenderLayerPlot(float vpx1, float vpy1, float vpx2, float vpy2, Application* app);
    void render();
    void updateViewport(float width, float height);
    void animateViewport();

    unsigned int addTimeSeries(unsigned int type);
    void addTimePoint(unsigned int series, double time, double data);
    void plotSeries(std::vector<TimePlotData>& data, glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, -1.0f));
    void predictSeries(std::vector<TimePlotData>& data, glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, -1.0f));
    void setStartEnd(double start, double end);
    void setTopBottom(double top, double bottom);
    void setCurrentTime(double time);
    //void setXaxis();
    //void setYaxis();
    //void set
private:
    Application* m_app = nullptr;
    std::vector<TimePlotData>* m_data;
    ImVec4 m_datacolor = ImVec4(0.0f, 0.0f, 0.0f, -1.0f); // Default no color marker in ImGui
    std::vector<TimePlotData>* m_pred;
    ImVec4 m_predcolor = ImVec4(0.0f, 0.0f, 0.0f, -1.0f); // Default no color marker in ImGui
    double current_time = 0.0;
    double start_time = 0.0;
    double end_time = 0.0;
    double top_value = 0.0;
    double bottom_value = 0.0;
};


// -------------
//  Application
// -------------
class Application { // Act as an object factory, like world used to do
public:
    bool start_fullscreen = false;
    GLFWwindow* window = nullptr;
    int w_width = 1280;      // Keeps current width and height, whether full screen or windowed
    int w_height = 720;

    // ImGUI
    bool imgui_ready = false; // ImGui and ImPlot have been initialized successfully
    ImFont* m_font1 = nullptr;
    ImFont* m_font2 = nullptr;

    // Keyboard activated items
    Camera* currentCam = nullptr;
    Camera* locationCam = nullptr;
    Earth* currentEarth = nullptr;
    BodyGeometry* currentEarth2 = nullptr;
    bool togglefullwin = false;
    bool isfullscreen = false;

    // Scripting
    bool interactive = false;
    bool gui = false;
    bool anim = false;
    bool do_eot = false;

    // Debugging options
    bool breakpoint = false;
    bool dumpcam = false;
    bool dumptime = false;
    bool dumpdata = false;

    // Lunalemma parameters - here because lunalemma is simply a path allocated by a location
    float lunalemmaOffset = 0.0f;

    // Temporary variables for Science It Out response
    bool sio_dip = false;
    bool sio_refract = false;
    bool sio_sunlimb = false;
    bool sio_local_weather = false;
    unsigned int sio_refmethod = 1;
    float sio_pressure = 1013.25f;
    float sio_temperature = 15.0f;
    float sio_height = 50.0f;
    float sio_pathwidth = 0.002f;

    // Custom parameters - can be used for anything temporary
    float customparam1 = 0.0f;
    float customlow1 = -90.0f;
    float customhigh1 = 90.0f;
    float customparam2 = 0.0f;
    float customlow2 = -180.0f;
    float customhigh2 = 180.0f;

    // ------ Rendering ------ //
    // Rendering to file
    std::string basefname = "Basefilename";
    unsigned int currentframe = 0;
    unsigned int currentseq = 0;
    bool renderoutput = false; // Whether to write image to disk

    // Should ideally be private, but I sometimes need to pick up layers in C++, which were created in Python.
    std::vector<RenderLayer*> m_layers;  // RenderLayer is a parent object to RenderLayer3D, RenderLayerText, RenderLayerGUI, ...

private:
    ShaderLibrary* m_shaderlib = nullptr;
    TextureLibrary* m_texturelib = nullptr;

    // For windowed / fullscreen control
    GLFWmonitor* monitor = nullptr;    // monitor handle, used when setting full screen mode
    const GLFWvidmode* mode = nullptr;
    int win_width = 1280;    // Stores Windowed width/height while in fullscreen
    int win_height = 720;
    int w_posx = 100;        // Ditto for position
    int w_posy = 100;

    // File Output Framebuffer
    unsigned int output_fbo = 0;
    unsigned int output_texture = 0;
    unsigned int output_rbo = 0; // Depth (and possibly stencil) buffers in Render Buffer Object, because we don't sample from these
    unsigned int output_width = 1920;
    unsigned int output_height = 1080;
    unsigned int output_type = NO_UINT;

    // Blit to Application Window
    float quadVertices[24] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
    // positions   // texCoords
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadVAO = 0;
    unsigned int quadVBO = 0;
public:
    Application();
    ShaderLibrary* getShaderLib();
    TextureLibrary* getTextureLib();
    Astronomy* newAstronomy();
    Scene* newScene();
    int initWindow();
    void setFullScreen();
    void setWindowed(int width = 0, int height = 0);  // Should not be negative, but GLFW uses int rather than unsigned int
    void SetWH(int w, int h);
    int getWidth();
    int getHeight();
    void update();
    bool shouldClose();
    float getAspect();
    void initImGUI();
    void beginImGUI();
    void endImGUI();

    RenderLayer3D* newLayer3D(float vpx1, float vpy1, float vpx2, float vpy2, Scene* scene, Astronomy* astro, Camera* cam = nullptr, bool overlay = true);
    void deleteLayer3D(RenderLayer3D* layer);
    RenderLayerText* newLayerText(float vpx1, float vpy1, float vpx2, float vpy2, RenderLayerTextLines* lines = nullptr);
    void deleteLayerText(RenderLayerText* layer);
    RenderLayerGUI* newLayerGUI(float vpx1, float vpy1, float vpx2, float vpy2);
    void deleteLayerGUI(RenderLayerGUI* layer);
    RenderLayerPlot* newLayerPlot(float vpx1, float vpy1, float vpx2, float vpy2);
    void deleteLayerPlot(RenderLayerPlot* layer);
    void updateView(int w, int h);
    void render();
    void writeFrame(unsigned int framebuffer);
    void incSequence();
    unsigned int createFrameBuffer(unsigned int width, unsigned int height, unsigned int type);
    void setupFileRender(unsigned int width, unsigned int height, unsigned int type);
};


// ------------
//  ShadowBox
// ------------
class ShadowBox {
private:
    Scene* m_scene;
    unsigned int depthMapFBO = NO_UINT;
    unsigned int width = 1024;
    unsigned int height = 1024;
public:
    unsigned int depthCubemap = NO_UINT;
    std::vector<glm::mat4> shadowTransforms;
    float near = 0.01f;
    float far = 20.0f;
    ShadowBox(Scene* scene, unsigned int w, unsigned int h);
    ~ShadowBox();
    void Render(Camera* cam, glm::vec3 lightPos);
};


// -----------
//  ShadowMap
// -----------
class ShadowMap {
private:
    Scene* m_scene = nullptr;
    unsigned int m_depthmapFBO{ 0 };
    unsigned int width{ 0 };
    unsigned int height{ 0 };
public:
    Shader* shdr = nullptr;
    unsigned int depthmap{ 0 };
    glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
    ShadowMap(Scene* scene, unsigned int w, unsigned int h);
    ~ShadowMap();
    void Bind();
    void Unbind();
    void Render(Camera* cam);
};



// --------------
//  Text Factory
// --------------
class TextFactory {
public:
    TextFactory(Scene* scene);
    ~TextFactory();
    TextString* newText(Font* font, const std::string& text, const float size, const glm::vec4 color, glm::vec3& position, glm::vec3& direction, glm::vec3& up);
    void draw();
private:
    Scene* m_scene = nullptr;
    std::vector<TextString*> m_texts;
};


// -------------
//  Text String
// -------------
class TextString {
public:
    glm::vec3 m_position = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 m_direction = glm::vec3(0.0f, 1.0f, 0.0f); // Writing direction
    glm::vec3 m_up = glm::vec3(0.0f, 0.0f, 1.0f);        // Up direction
    float m_size = 0.0f;
    glm::vec4 m_color = NO_COLOR;
private:
    std::string m_text = "123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890"; // Causes CRASH !!! When overwritten by longer string
    Scene* m_scene = nullptr;
    Font* m_font = nullptr;
    Glyphs* m_glyphsOb = nullptr;
public:
    TextString(Scene* scene, Font* font, const std::string& text, float size, glm::vec4 color, glm::vec3& position, glm::vec3& direction, glm::vec3& up);
    ~TextString();
    void draw();
    void updateText(const std::string& text);
    void updatePosDirUp(glm::vec3 position, glm::vec3 direction, glm::vec3 up);
private:
    void genGlyphs();
};


// ------
//  Font
// ------
struct glyphdata { // Glyph meta data from msdf-atlas-gen
    char letter = 0;         // ASCII or UTF-8 character code
    float advance = 0.0f;    // How far does cursor move after writitng this character
    float p_left = 0.0f;     // Where is character drawn relative to cursor (before advance)
    float p_bottom = 0.0f;
    float p_right = 0.0f;
    float p_top = 0.0f;
    float a_left = 0.0f;     // Texture coordinates of glyph in pixels (divide by font_atlas_width,font_atlas_height) to get OpenGL tex coords.
    float a_bottom = 0.0f;
    float a_right = 0.0f;
    float a_top = 0.0f;
};
class Font {
public:
    Font(const std::string& fontname);
    //~Font(); - default destructor works fine for now
    glyphdata getGlyphdata(char letter);
    Texture* getTexture();
    bool is_valid = false;
private:
    // Font details
    float font_size = 48.0f;
    float font_lineheight = 0.0f;
    float font_atlas_width = 436.0f;  // CascadiaMono 48 is 436x436 - CourierNew 48 is 400x400
    float font_atlas_height = 436.0f;
    Texture* tex = nullptr;
    std::vector<glyphdata> glyphs;  // Stores read glyph metadata. Converted atlasBounds from pixels to opengl tex coords
    int createFont(const std::string& font);
    bool loadFont(const std::string& font);
};


// --------
//  Glyphs
// --------
struct GlyphItem {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec2 uv = glm::vec2(0.0f);
    glm::vec4 color = NO_COLOR;
};
class Glyphs {
private:
    Scene* m_scene = nullptr;
    Font* m_font = nullptr;
    Shader* shdr = nullptr;
    Texture* tex = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb = nullptr;
    VertexBufferLayout* vbl = nullptr;
    // Font details
    float font_size = 48;
    float font_atlas_width = 436;  // CascadiaMono 48
    float font_atlas_height = 436;
    //float font_atlas_width = 400;    // CourierNew 48
    //float font_atlas_height = 400;
    //std::vector<glyphdata> glyphs;  // Stores read glyph metadata. Converted atlasBounds from pixels to opengl tex coords
    std::vector<GlyphItem> glyphItems;
public:
    Glyphs(Scene* scene, Font* font);
    ~Glyphs();
    void clear();
    unsigned int newGlyph(char letter, glm::vec4 color, float size, glm::vec3& cursor, glm::vec3& direction, glm::vec3& up);
    void drawGlyphs();
};


// ------------
//  Bill Board
// ------------
class BillBoard {
public:
    // Builds a TextString and keeps it oriented towards the camera.
    BillBoard(Scene* scene, Font* font, std::string text, glm::vec3 position, glm::vec4 color, float size);
    void update(glm::vec3 position = NO_VEC3);
    void changeColorSize(glm::vec4 color, float size);
private:
    Scene* m_scene = nullptr;
    Font* m_font = nullptr;
    glm::vec3 m_pos = glm::vec3(0.0f);
    std::string m_text;
    TextString* m_textstring = nullptr;
};


// ------------
//  Time Zones
// ------------
class TimeZones {
public:
    // For the time zone shapes
    struct TimeZoneName {
        size_t index = 0;
        std::string searchname;
        //std::string displayname;
    };
    struct timezonepartcache {
        size_t timezone_id = 0;
        ShapeFile::ShapePart* part = nullptr;
        PolyLine* polyline = nullptr;
        Earth* earth = nullptr;
    };
    // IANA timezone db
    tzFile::iana_tz timezonedb;
private:
    Scene* m_scene = nullptr;
    // For the ESRI data
    std::vector<ShapeFile::ShapeRecord*> records;
    uint32_t bytecnt = 0;
    // For country names and index
    std::vector<TimeZoneName> timezonenames;
    // For drawn borders
    std::vector<timezonepartcache*> timezoneparts;
public:
    // Functions for timezone outlines
    TimeZones(Scene* scene, const std::string& shapefile = "C:\\Coding\\combined-shapefile-with-oceans");
    void addTimeZone(Earth& earth, const std::string timezonename);
    void addTimeZone(Earth& earth, size_t rindex);
    void update();
    void draw();
    int parseNames(const std::string& namefile);
    // Functions for IANA timezone db
    std::string getLocalTime(const std::string& timezone, const DateTime& datetime);
    std::string getLocalTime(const std::string& timezone, long year, long month, double day, double hour, double minute, double second);
    void dumpTimeZoneDetails(const std::string& timezone);
};


// -----------------
//  Country Borders
// -----------------
struct CountryName {
    size_t index = 0;
    std::string searchname;
    std::string displayname;
};
struct borderpartcache {
    size_t country_id = 0;
    ShapeFile::ShapePart* part = nullptr;
    PolyLine* polyline = nullptr;
    Earth* earth = nullptr;
};
class CountryBorders {
public:
    unsigned int fileversion = 0;
private:
    Scene* m_scene = nullptr;
    // For the ESRI data
    std::vector<ShapeFile::ShapeRecord*> records;
    uint32_t bytecnt = 0;
    // For country names and index
    std::vector<CountryName> countrynames;
    // For drawn borders
    std::vector<borderpartcache*> borderparts;
public:
    CountryBorders(Scene* scene, const std::string& shapefile = "C:\\Coding\\ne_10m_admin_0_countries");
    void addBorder(Earth& earth, const std::string countryname);
    void addBorder(Earth& earth, size_t rindex);
    void update();
    void draw();
    int parseNames(const std::string& namefile);
};


// -----------------
//  Constellations2 (ShapeFile based, but the shapefile I use has antimeridian issues)
// -----------------
// !!! FIX: This does not have facilities for removing objects once added !!!
struct ConstellationName {
    size_t index = 0;
    std::string searchname;  // 3 letter abbreviation
    std::string displayname; // Canonical Latin name
};
struct bordeconstellationpartcache {
    size_t country_id = 0;
    ShapeFile::ShapePart* part = nullptr;
    PolyLine* polyline = nullptr;
    BodyGeometry* geom = nullptr;
};
class Constellations2 {
// A Constellation is a region of the celestial sphere containing the asterism it is named after.
// it works pretty much like country borders on Earth, hence I have simply copied the implementation.
public:
    unsigned int fileversion = 0;
    // For the ESRI data
    std::vector<ShapeFile::ShapeRecord*> records;
private:
    Scene* m_scene = nullptr;
    uint32_t bytecnt = 0;
    // For constellation names and index
    std::vector<ConstellationName> constellationnames;
    // For drawn borders
    std::vector<bordeconstellationpartcache*> constellationparts;
public:
    Constellations2(Scene* scene, const std::string& shapefile = "C:\\Coding\\Eartharium\\constellations\\constell");
    void addBorder(BodyGeometry& geom, const std::string constellationname);
    void addBorder(BodyGeometry& geom, size_t rindex);
    void update();
    void draw();
    int parseNames(const std::string& namefile);
};


// ----------------
//  Constellations   - based on better datafiles than above
// ----------------
class Constellations {

};



// --------
//  Skybox
// --------
class SkyBox {
private:
    Scene* m_scene = nullptr;
    unsigned int m_textureID;
    Shader* m_shdrsb;
    VertexBufferLayout* m_vblsb;
    VertexArray* m_vasb;
    VertexBuffer* m_vbsb;
    const float skyboxR = 1.0f; // Has no effect at all
    float skyboxVertices[108] = {
        // positions          
        -skyboxR,  skyboxR, -skyboxR,
        -skyboxR, -skyboxR, -skyboxR,
         skyboxR, -skyboxR, -skyboxR,
         skyboxR, -skyboxR, -skyboxR,
         skyboxR,  skyboxR, -skyboxR,
        -skyboxR,  skyboxR, -skyboxR,

        -skyboxR, -skyboxR,  skyboxR,
        -skyboxR, -skyboxR, -skyboxR,
        -skyboxR,  skyboxR, -skyboxR,
        -skyboxR,  skyboxR, -skyboxR,
        -skyboxR,  skyboxR,  skyboxR,
        -skyboxR, -skyboxR,  skyboxR,

         skyboxR, -skyboxR, -skyboxR,
         skyboxR, -skyboxR,  skyboxR,
         skyboxR,  skyboxR,  skyboxR,
         skyboxR,  skyboxR,  skyboxR,
         skyboxR,  skyboxR, -skyboxR,
         skyboxR, -skyboxR, -skyboxR,

        -skyboxR, -skyboxR,  skyboxR,
        -skyboxR,  skyboxR,  skyboxR,
         skyboxR,  skyboxR,  skyboxR,
         skyboxR,  skyboxR,  skyboxR,
         skyboxR, -skyboxR,  skyboxR,
        -skyboxR, -skyboxR,  skyboxR,

        -skyboxR,  skyboxR, -skyboxR,
         skyboxR,  skyboxR, -skyboxR,
         skyboxR,  skyboxR,  skyboxR,
         skyboxR,  skyboxR,  skyboxR,
        -skyboxR,  skyboxR,  skyboxR,
        -skyboxR,  skyboxR, -skyboxR,

        -skyboxR, -skyboxR, -skyboxR,
        -skyboxR, -skyboxR,  skyboxR,
         skyboxR, -skyboxR, -skyboxR,
         skyboxR, -skyboxR, -skyboxR,
        -skyboxR, -skyboxR,  skyboxR,
         skyboxR, -skyboxR,  skyboxR
    };
    std::vector<std::string> m_faces = {
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_right.png",
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_left.png",
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_top.png",
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_bottom.png",
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_front.png",
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_back.png"
    };
    //std::vector<std::string> m_faces = {
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_front.png",    // +x
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_back.png",     // -x
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_right.png",    // +y
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_left.png",     // -y
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_top.png",      // +z
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_bottom.png"    // -z
    //};
public:
    SkyBox(Scene* scene);
    ~SkyBox();
    void Draw();
    void loadCubemap(std::vector<std::string> faces);
};


// ---------------
//  ParticleTrail
// ---------------
class ParticleTrail {
private:
    struct particle {
        glm::vec4 color = WHITE;
        glm::vec3 position = glm::vec3(0.0f);
        float size = 0.0f;
        size_t index = maxuint;
    };
    Scene* m_scene = nullptr;
    unsigned int m_number = maxuint;
    glm::vec4 m_color = WHITE;
    float m_size = 0.0f;
    unsigned int m_spacing = 0;
    unsigned int m_gap = 0;
    double m_sizefactor = 1.0;
    bool m_taper = true; // apply scalefactor
    std::deque<particle> m_queue;
public:
    ParticleTrail(Scene* scene, unsigned int number, glm::vec4 color, float size = 0.02f, unsigned int spacing = 0, bool taper = true);
    ~ParticleTrail();
    void push(glm::vec3 pos);
    void clear();
    void trim(unsigned int length);
    void expand(unsigned int length);
    void draw();
};


// ----------
//  AngleArc
// ----------
class AngleArcs {
    // Add an optional arrowhead !!!
    // This is a circlearc between two objects, such as two arrows or an arrow and a plane.
    // Ideally it has an arrow head showing the direction of angle measure.
    // Parameters could include a position and two vectors "spanning" the arc by pointing to the start and end points.
    // For arrow to plane for example, the plane endpoint is simply a projection of the arrow onto the plane.
    // Also the basics such as thickness and color. Segment count?
    // In short it should construct a PolyCurve and potentially a Cone. Model after Arrow.
private:
    Scene* m_scene = nullptr;
    struct AngleArc {
        PolyCurve* polycurve = nullptr;
        size_t cone = maxuint;
        glm::vec4 color;
        glm::vec3 position;
        glm::vec3 start;
        glm::vec3 stop;
        float length;
        float width;
        double angle;
        bool expired = false;
        bool wide = false;                // false = always shortest angle, true = always clockwise wrt pole
        glm::vec3 pole = glm::vec3(0.0f); // Pole to determine
    };
    std::vector<AngleArc> m_arcs;
public:
    AngleArcs(Scene* scene);
    ~AngleArcs();
    double getAngle(size_t index);
    size_t add(glm::vec3 position, glm::vec3 start, glm::vec3 stop, float length, glm::vec4 color = LIGHT_GREY, float width = 0.001f, bool wide = false, glm::vec3 pole = glm::vec3(0.0f));
    void remove(size_t index);
    void update(size_t index, glm::vec3 position, glm::vec3 start, glm::vec3 stop, float length, glm::vec4 color = LIGHT_GREY, float width = 0.001f, bool wide = false, glm::vec3 pole = glm::vec3(0.0f));
    void draw();
};


// -----------
//  PolyLine SceneObject aware
// -----------
class PolyLineSO : public SceneObject {
private:
    //Scene* m_scene = nullptr;
    glm::vec4 m_color = NO_COLOR;
    float m_width = 0.0f;
    unsigned int facets = 8;
    std::vector<Vertex> m_verts;
    std::vector<Tri> m_tris;
    std::vector<Primitive3D> m_segments;
    Shader* shdr = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb1 = nullptr;
    VertexBuffer* vb2 = nullptr;
    VertexBufferLayout* vbl1 = nullptr;
    VertexBufferLayout* vbl2 = nullptr;
    IndexBuffer* ib = nullptr;
    bool dirty = false;  // If changes have been made which require regenerating m_segments since last update() call
    //bool limit = false; // For debugging reserve() issues 
public:
    std::vector<glm::vec3> m_points; // Public so we can use it as track for objects (PathTracker in Earth.h)
    PolyLineSO(Scene* scene, SceneObject* parent, glm::vec4 color, float width, size_t reserve = NO_UINT);
    ~PolyLineSO();
    void setColor(glm::vec4 color);
    void setWidth(float width);
    void changePolyCurve(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
    void addPoint(glm::vec3 point);
    void clearPoints();
    void generate();
    bool update() override;
    void draw(Camera* cam);
    void genGeom();
};


// ----------
//  PolyLine
// ----------
class PolyLine {
private:
    Scene* m_scene = nullptr;
    glm::vec4 m_color = NO_COLOR;
    float m_width = 0.0f;
    std::vector<glm::vec3> m_points;
    Shader* shdr = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb1 = nullptr;
    VertexBuffer* vb2 = nullptr;
    VertexBufferLayout* vbl1 = nullptr;
    VertexBufferLayout* vbl2 = nullptr;
    IndexBuffer* ib = nullptr;
    unsigned int VAO = NO_UINT;
    unsigned int VBO = NO_UINT;
    //bool limit = false; // For debugging reserve() issues 
public:
    PolyLine(Scene* scene, glm::vec4 color, float width, size_t reserve = NO_UINT);
    ~PolyLine();
    void change(glm::vec4 color, float width);
    void addPoint(glm::vec3 point);
    void clearPoints();
    void generate();
    void draw();
};


// -----------
//  PolyCurve SceneObject aware
// -----------
class PolyCurveSO : public SceneObject {
private:
    //Scene* m_scene = nullptr;
    glm::vec4 m_color = NO_COLOR;
    float m_width = 0.0f;
    unsigned int facets = 8;
    std::vector<Vertex> m_verts;
    std::vector<Tri> m_tris;
    std::vector<Primitive3D> m_segments;
    Shader* shdr = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb1 = nullptr;
    VertexBuffer* vb2 = nullptr;
    VertexBufferLayout* vbl1 = nullptr;
    VertexBufferLayout* vbl2 = nullptr;
    IndexBuffer* ib = nullptr;
    bool dirty = false;  // If changes have been made which require regenerating m_segments since last update() call
    //bool limit = false; // For debugging reserve() issues 
public:
    std::vector<glm::vec3> m_points; // Public so we can use it as track for objects (PathTracker in Earth.h)
    PolyCurveSO(Scene* scene, SceneObject* parent, glm::vec4 color, float width, size_t reserve = NO_UINT);
    ~PolyCurveSO();
    void setColor(glm::vec4 color);
    void setWidth(float width);
    void changePolyCurve(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
    void addPoint(glm::vec3 point);
    void clearPoints();
    void generate();
    bool update() override;
    void draw(Camera* cam);
    void genGeom();
};


// -----------
//  PolyCurve
// -----------
class PolyCurve {
private:
    Scene* m_scene = nullptr;
    glm::vec4 m_color = NO_COLOR;
    float m_width = 0.0f;
    unsigned int facets = 8;
    std::vector<Vertex> m_verts;
    std::vector<Tri> m_tris;
    std::vector<Primitive3D> m_segments;
    Shader* shdr = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb1 = nullptr;
    VertexBuffer* vb2 = nullptr;
    VertexBufferLayout* vbl1 = nullptr;
    VertexBufferLayout* vbl2 = nullptr;
    IndexBuffer* ib = nullptr;
    //bool limit = false; // For debugging reserve() issues 
public:
    std::vector<glm::vec3> m_points; // Public so we can use it as track for objects (PathTracker in Earth.h)
    PolyCurve(Scene* scene, glm::vec4 color, float width, size_t reserve = NO_UINT);
    ~PolyCurve();
    void changePolyCurve(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
    void addPoint(glm::vec3 point);
    void clearPoints();
    void generate();
    void draw(Camera* cam);
    void genGeom();
};


// --------------
//  Generic Path
// --------------
class GenericPath {   // : public SceneObject {
    // Flexibly allocates additional PolyCurve objects when addSplit() is called
    // Since PolyCurves are obtained from Scene, they are drawn automatically
    // - Add method to change color and width
    // - Add a way to traverse paths backwards and forwards, see Earth.h:ParticleTracker
public:
    GenericPath(Scene* scene, float width = NO_FLOAT, glm::vec4 color = NO_COLOR);
    ~GenericPath();
    void setColor(glm::vec4 color);
    void setWidth(float width);
    void addPoint(glm::vec3 point);
    void addSplit(glm::vec3 point1, glm::vec3 point2);
    void clearPoints();
    void generate();
    void draw(Camera* cam);
private:
    Scene* m_scene = nullptr;
    glm::vec4 m_color{ NO_COLOR };
    float m_width{ NO_FLOAT };
    std::vector<PolyCurve*> m_curves;
    unsigned int m_curve{ 0 };
};


// --------------
//  Path Builder
// --------------
// Utility function to build all the many varied paths that are used by Earth and Location
// Should help refactor those chunky classes.
//class PathBuilder {
//public:
//    static void generate(LLH llh1, LLH llh2, ) {
//
//    }
//};


// --------
//  Arrows
// --------
class Arrows {
private:
    struct Arrow {
        size_t cylinder{ 0 };
        size_t cone{ 0 };
        glm::vec4 color{ 0.0f };
        glm::vec3 position{ 0.0f };
        glm::vec3 direction{ 0.0f };
        float length{ 0.0f };
        float width{ 0.0f };
    };
    Scene* m_scene = nullptr;
    Cylinders* m_cylinders = nullptr;
    Cones* m_cones = nullptr;
    tightvec<Arrow> m_arrows;
public:
    Arrows(Scene* scene);
    ~Arrows();
    void draw();
    size_t store(Arrow a);
    void remove(size_t index);
    size_t addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    size_t addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    void changeStartDirLen(size_t arrow, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void changeStartEnd(size_t arrow, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    void changeArrow(size_t index, glm::vec4 color = NO_COLOR, float length = NO_FLOAT, float width = NO_FLOAT);
    void removeArrow(size_t index); // FIRST implement deletion in Cylinders and Cones (and other Primitive3D children)
    void clear();
};


// ------------
//  Primitives
// ------------
class Primitives {
protected:
    tightvec<Primitive3D> m_Primitives;
    std::vector<Vertex> m_verts;
    std::vector<Tri> m_tris;
private:
    Scene* m_scene = nullptr;
    Shader* shdr = nullptr;
    Shader* smshdr = nullptr;
    Shader* sbshdr = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb1 = nullptr;
    VertexBuffer* vb2 = nullptr;
    VertexBufferLayout* vbl1 = nullptr;
    VertexBufferLayout* vbl2 = nullptr;
    IndexBuffer* ib = nullptr;
    // !!! FIX: Are these dependencies really needed? Seems like they are begging for a refactoring !!!
    friend class SkyDots;
    friend class Glyphs;
public:
    void draw(Camera* cam, unsigned int shadow);
    void clear();
    glm::vec4 getColor(size_t index);
    void setColor(size_t index, glm::vec4 color);
    Primitive3D* getDetails(size_t index);
    void remove(size_t oid);
protected:
    Primitives(Scene* scene, unsigned int verts, unsigned int tris);
    ~Primitives();
    void init();
    size_t store(Primitive3D p);
    void update(size_t oid, Primitive3D p);
    void virtual genGeom() = 0;
};


// ----------
//  Minifigs
// ----------
class Minifigs : public Primitives {
public:
    Minifigs(Scene* scene);
    ~Minifigs();
    size_t addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color, float bearing);
    void changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color, float bearing);
    size_t addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    void removeMinifig(size_t index);
private:
    std::string m_objFile = "C:\\Coding\\Eartharium\\Eartharium\\models\\minifig.obj";
    void genGeom() override;
};


// ----------
//  SphereUV
// ----------
class SphereUV : public Primitives {
public:
    SphereUV(Scene* scene);
    void print();
    size_t addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    size_t addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    void changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void changeStartEnd(size_t index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    void removeSphereUV(size_t index);
private:
    glm::vec3 getLoc3D_NS(float lat, float lon, float height);
    void genGeom() override;
};


// --------
//  Planes
// --------
class Planes : public Primitives {
public:
    Planes(Scene* scene);
    size_t addStartNormalLen(glm::vec3 pos, glm::vec3 nml, float rot, float len, glm::vec4 color);
    size_t addStartUV(glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color);
    void changeStartNormalLen(size_t index, glm::vec3 pos, glm::vec3 nml, float rot, float len, glm::vec4 color);
    void changeStartUV(size_t index, glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color);
    void removePlane(size_t index);
private:
    void genGeom() override;
};


// -----------
//  ViewCones
// -----------
class ViewCones : public Primitives {
public:
    ViewCones(Scene* scene);
    size_t addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    size_t addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //unsigned int addStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void changeStartEnd(size_t index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //void changeStartEleAzi(size_t index, glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void removeViewCone(size_t index);
private:
    void genGeom() override;
};


// -------
//  Cones
// -------
class Cones : public Primitives {
public:
    Cones(Scene* scene);
    void print();
    size_t addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    size_t addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //unsigned int addStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void changeStartEnd(size_t index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //void UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void changeColorLengthWidth(size_t index, glm::vec4 color, float length, float width);
    void removeCone(size_t index);
private:
    void genGeom() override;
};


// -----------
//  Cylinders
// -----------
class Cylinders : public Primitives {
public:
    Cylinders(Scene* scene);
    void print();
    size_t addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    size_t addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //unsigned int FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void changeStartEnd(size_t index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //void UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void changeColorLengthWidth(size_t index, glm::vec4 color, float length, float width);
    void removeCylinder(size_t index);
private:
    void genGeom() override;
};


// ----------
//  Sky Dots
// ----------
class SkyDots : public Primitives {
public:
    typedef unsigned __int64 Index;
    struct Triangle {
        Index vertex[3];
    };
    using TriangleList = std::vector<Triangle>;
    using VertexList = std::vector<glm::vec3>;
    using Lookup = std::map<std::pair<Index, Index>, Index>;
    using IndexedMesh = std::pair<VertexList, TriangleList>;
    bool visible = true;
private:
    const float X = .525731112119133606f;
    const float Z = .850650808352039932f;
    const float N = 0.f;
    const VertexList icovertices = {
      {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
      {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
      {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
    };
    const TriangleList icotriangles = {
      {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
      {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
      {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
      {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
    };
public:
    SkyDots(Scene* scene);
    size_t addXYZ(glm::vec3 pos, glm::vec4 color, float size);
    void changeXYZ(size_t index, glm::vec3 pos, glm::vec4 color, float size);
    void changeDot(size_t index, glm::vec4 color, float size);
    void removeDot(size_t index);
    void draw(Camera* cam);
private:
    void genGeom() override;
    TriangleList subdivide(VertexList& vertices, TriangleList triangles);
    Index vertex_for_edge(Lookup& lookup, VertexList& vertices, Index first, Index second);
};


// ------
//  Dots
// ------
class Dots : public Primitives {
public:
    typedef unsigned __int64 Index;
    struct Triangle {
        Index vertex[3];
    };
    using TriangleList = std::vector<Triangle>;
    using VertexList = std::vector<glm::vec3>;
    using Lookup = std::map<std::pair<Index, Index>, Index>;
    using IndexedMesh = std::pair<VertexList, TriangleList>;
private:
    const float X = .525731112119133606f;
    const float Z = .850650808352039932f;
    const float N = 0.f;
    const VertexList icovertices = {
      {-X,N,Z}, {X,N,Z}, {-X,N,-Z}, {X,N,-Z},
      {N,Z,X}, {N,Z,-X}, {N,-Z,X}, {N,-Z,-X},
      {Z,X,N}, {-Z,X, N}, {Z,-X,N}, {-Z,-X, N}
    };
    const TriangleList icotriangles = {
      {0,4,1},{0,9,4},{9,5,4},{4,5,8},{4,8,1},
      {8,10,1},{8,3,10},{5,3,8},{5,2,3},{2,7,3},
      {7,10,3},{7,6,10},{7,11,6},{11,0,6},{0,1,6},
      {6,1,10},{9,0,11},{9,11,2},{9,2,5},{7,2,11}
    };
public:
    Dots(Scene* scene);
    size_t addXYZ(glm::vec3 pos, glm::vec4 color, float size);
    void changeXYZ(size_t index, glm::vec3 pos, glm::vec4 color = NO_COLOR, float size = NO_FLOAT);
    void changeDot(size_t index, glm::vec4 color, float size);
    void removeDot(size_t index);
private:
    void genGeom() override;
    TriangleList subdivide(VertexList& vertices, TriangleList triangles);
    Index vertex_for_edge(Lookup& lookup, VertexList& vertices, Index first, Index second);
};
