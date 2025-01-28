#pragma once

#include <vector>
#include <list>
#include <queue>
#include <string>

#include "../ImGUI/imgui.h"  // For ImVec4

#include "config.h"       // For enums and glm
#include "Primitives.h"   // For Vertex Tri etc.

// Forward references
//  Astronomy.h
class Astronomy;
//  Earth.h
class SolarSystem;
class Earth;
class SkySphere;
class BodyGeometry;
class ThreePointSolver;
class CountryBorders;
class TimeZones;
//  astronomy/acoordinates.h
struct LLD;
//  OpenGl.h
class ShaderLibrary;
class TextureLibrary;
struct GLFWmonitor;
struct GLFWvidmode;
struct GLFWwindow;

//  Renderer.h - i.e. local to this file
class Application;
class Scene;
class SceneTree;
class SceneObject;
class SceneTreeRoot;
class Camera;

// -------------
//  SceneObject
// -------------
// Being implemented
// I have yet to fully work out the requirements ...
class SceneObject {
public:
    bool hidden = false;  // Hide object by bailing early from draw();
    bool hasgui = false;  // Are there GUI components to render?
    std::string name = "Object";
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotations{ 0.0f, 0.0f, 0.0f };
    unsigned int rotationorder = XYZ;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 worldmatrix = glm::mat4(1.0f); // Collects parent worldmatrix and applies scale, rotation, position.
    glm::vec4 color = WHITE; // default color, not used by some derived objects (e.g. textured objects)
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
    virtual void draw(Camera* cam) = 0;
    virtual void myGUI() {};
protected:
    //Material* material = nullptr;
    unsigned int id = 0;
private:
    SceneObject(Scene* scene, bool isroot = false); // Only used to construct the root in SceneTree.
    friend class SceneTreeRoot;
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
    void setPosLLH(LLD llh);
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
    bool opengl_debug_info = false;

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
