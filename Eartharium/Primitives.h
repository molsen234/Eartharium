#pragma once
#include "mdoOpenGL.h"
#include <iterator>

struct LLH {
    double lat = 0.0;
    double lon = 0.0;
    double dst = 0.0;
};

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

enum itemtype {
    SUN = CAAElliptical::EllipticalObject::SUN,
    MERCURY,
    VENUS,
    MARS,
    JUPITER,
    SATURN,
    URANUS,
    NEPTUNE,
    EARTH,
    MOON,
    LOC,
    ZENITH,
    NORTH,
    EAST,
    NORMAL,  // Calculated differently than Zenith
    LOCSKY,
    TRUESUN3D,
    TRUEANALEMMA3D,
    FLATSUN3D,
    TRUEMOON3D,
    TRUEPLANET3D,
    TRUEMERCURY3D,  // To be retired, use TRUEPLANET3D in conjunction with planet id in .unique field, just like for stars.
    TRUEVENUS3D,    //   :
    TRUEMARS3D,     //   :
    TRUEJUPITER3D,  //   :
    TRUESATURN3D,   //   :
    TRUEURANUS3D,   //   : (Also, you forgot Neptune)
    SIDPLANET3D,
    AZIELE3D,
    RADEC3D,
    ARROWEXTENT,
    TANGENT,
    MERIDIAN,
    LATITUDE,
    LONGITUDE,
    DOT,
    CURVE,
    GREATARC,
    LERPARC,
    FLATARC,    // Aka Derp Arc - shortest distance on AE map
    SUNTERMINATOR,
    SHADOW_MAP,
    SHADOW_BOX,
    EC,
    ECGEO,
    LAYER3D,
    LAYERTEXT,
    LAYERGUI,
    LAYERPLOT,
    ALL,
    NONE = maxuint,
};


// Protos
class Camera;
class Dots;
class Planes;
class Letters;
class Cones;
class Primitives;
class ViewCones;
class Cylinders;
class Arrows;
class Minifigs;
class SkyBox;
class SkySphere;
class Earth;
class Location;
class SphereUV;
class SolarSystem;
class StarTrail;
class Astronomy;
class RenderChain;
class Application;
class Scene;
class PolyCurve;
class AngleArcs;


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
        double k = std::max(0.0, std::min(1.0, (double)(m_counter - 1) / (double)(m_steps - 1)));
        double s = k * k * (3.0 - 2.0 * k);
        return (T)(s * (double)m_last + (1 - s) * (double)m_first);
    }
    if (m_restart) m_counter = 0;
    return m_last;
}


// ----------
//  TightVec
// ----------
template<typename T>
class tightvec {
public:
    std::vector<T> m_Elements;
    std::map<unsigned int, unsigned int> m_remap;
    std::map<unsigned int, unsigned int> m_xmap;
    std::vector<unsigned int> m_freed;
    //tightvec<T>();
    //~tightvec<T>();
    unsigned int size();
    unsigned int capacity();
    void clear();
    bool empty();
    T& operator[](unsigned int oid);
    void reserve(unsigned int reserve);
    unsigned int store(T element, bool debug = false);
    T& retrieve(unsigned int oid);
    void update(unsigned int oid, T element);
    void remove(unsigned int oid, bool debug = false);
};
template<typename T>
unsigned int tightvec<T>::size() {
    return (unsigned int)m_Elements.size();
}
template<typename T>
unsigned int tightvec<T>::capacity() {
    return (unsigned int)m_Elements.capacity();
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
T& tightvec<T>::operator[](unsigned int oid) {
    //std::cout << "oid: " << oid << "\n";
    return retrieve(oid);
}
template<typename T>
void tightvec<T>::reserve(unsigned int reserve) {
    m_Elements.reserve(reserve);
}
template<typename T>
unsigned int tightvec<T>::store(T element, bool debug) {
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
    unsigned int oid, iid;
    iid = (unsigned int)m_Elements.size(); // vector is compact, so always add to end.
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
T& tightvec<T>::retrieve(unsigned int oid) {
    if (m_remap.count(oid)) return m_Elements[m_remap[oid]];
    else {
        std::cout << "ERROR: tightvec::retrieve() or tightvec[] called with non-existing index: " << oid << "\n";
        return m_Elements.back();
    }
}
template<typename T>
void tightvec<T>::update(unsigned int oid, T element) {
    unsigned int iid = m_remap[oid];
    m_Elements[iid] = element;
}
template<typename T>
void tightvec<T>::remove(unsigned int oid, bool debug) {
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
    unsigned int iid = m_remap[oid];
    if (debug) std::cout << "oid, iid: " << oid << ", " << iid << "\n";
    unsigned int last = (unsigned int)m_Elements.size() - 1;
    //std::cout << "oid: " << oid << "\n";
    //std::cout << "iid: " << iid << "\n";
    //std::cout << "last: " << last << "\n";
    if (iid != last) { // Removing element in middle of vector
        m_Elements[iid] = m_Elements[last]; //.back();
        //std::cout << "Replace element: " << iid << " with " << last << "\n";
        unsigned int loid = m_xmap[last]; // If last was previously relocated, get oid of last. If not relocated, it still points last.
        m_remap[loid] = iid;
        if (debug) std::cout << "setting m_remap[" << loid << "] = " << iid << "\n";
        m_xmap[iid] = loid;
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


// Proto
class Scene;


// --------
//  Camera
// --------
class Camera {
public:
    // Being set from GUI, should be set from keyboard as well !!!
    float camLat = 0.0f;
    float camLon = 0.0f;
    float camDst = 10.0f;
    float camFoV = 6.0f;
    float camNear = 1.0f;
    float camFar = 1500.0f;

    glm::vec3 CamLightDir = glm::vec3(0.0f);
    float camlightsep = 0.1f;
private:
    Scene* m_scene = nullptr;
    glm::mat4 ProjMat = glm::mat4(1.0f);
    glm::mat4 ViewMat = glm::mat4(1.0f);
    glm::vec3 m_position = glm::vec3(0.0f);
    glm::vec3 m_target = glm::vec3(0.0f);
    glm::vec3 m_direction = glm::vec3(0.0f);
    //float m_fov = 30.0f;
    glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0, 1.0, 0.0); // Part of worldUp that fits with cameraDirection
public:
    Camera(Scene* scene);
    void SetCamLightPos(glm::vec3 lPos);
    void CamUpdate();
    void setLookAt(glm::vec3 position, glm::vec3 target, glm::vec3 upwards);
    void setPosXYZ(glm::vec3 pos);
    void setPosLLH(LLH llh);
    glm::vec3 GetPosXYZ();
    void SetTarget(glm::vec3 target);
    void setFoV(float fov);
    float getFoV();
    glm::mat4 GetViewMat();
    glm::mat4 GetSkyViewMat();
    glm::mat4 GetProjMat();
    glm::vec3 GetRight();
    glm::vec3 GetUp();
private:
    void setCamLightPos(glm::vec3 lPos);
    void Recalc();
    //Application* m_app = nullptr;
};

// Protos
struct Viewport {
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
    Application* m_app = nullptr;
    Astronomy* m_celestOb = nullptr;
    Camera* w_camera = nullptr;
    unsigned int shadows = NONE;  // Create a way to update this from GUI or keyboard or python etc.
    SolarSystem* m_solsysOb = nullptr;
    Earth* m_earthOb = nullptr;
private:
    std::vector<Camera*> m_cameras;
    float m_aspect = 1.0f;
    Minifigs* m_minifigsOb = nullptr;
    Dots* m_dotsOb = nullptr;
    Cylinders* m_cylindersOb = nullptr;
    Cones* m_conesOb = nullptr;
    ViewCones* m_viewconesOb = nullptr;
    Planes* m_planesOb = nullptr;
    //Letters* m_lettersOb = nullptr;
    SphereUV* m_sphereuvOb = nullptr;
    Arrows* m_arrowsOb = nullptr;
    AngleArcs* m_anglearcsOb = nullptr;
    SkyBox* m_skyboxOb = nullptr;
    SkySphere* m_skysphereOb = nullptr;
    ShadowMap* m_shadowmap = nullptr;
    ShadowBox* m_shadowbox = nullptr;
    //PictureInPicture* m_pipOb = nullptr;
public:
    Scene(Application* app);
    ~Scene();
    Camera* newCamera();
    void setAspect(float aspect);
    float getAspect();
    void clearScene();  // ToDo !!!
    void render();
    Dots* getDotsOb();
    Cylinders* getCylindersOb();
    Cones* getConesOb();
    ViewCones* getViewConesOb();
    Planes* getPlanesOb();
    //Letters* getLettersOb();
    SphereUV* getSphereUVOb();
    Arrows* getArrowsOb();
    AngleArcs* getAngleArcsOb();
    SkySphere* newSkysphere(unsigned int mU, unsigned int mV);
    SkySphere* getSkysphere();
    SkyBox* getSkyboxOb();
    SolarSystem* getSolsysOb();
    ShadowBox* getShadowboxOb();
    ShadowMap* getShadowmapOb();
    Earth* newEarth(std::string mode, unsigned int mU, unsigned int mV);
    Earth* getEarth();
    Minifigs* newMinifigs();
};


// -------------
//  RenderLayer - base class
// -------------
class RenderLayer {
public:
    unsigned int type = NONE;
protected:
    float view_x1 = 0.0;  // Where in the viewport should this layer render, in [0.0f;1.0f] coordinates
    float view_y1 = 0.0;
    float view_x2 = 0.0;
    float view_y2 = 0.0;
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

    RenderLayer3D(float vpx, float vpy, float vpw, float vph, Scene* scene, Astronomy* astro, Camera* cam = nullptr);
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
    unsigned int size() { return lines.size(); }
    //std::vector<std::string*>::iterator begin() { return lines.begin(); }
};
class RenderLayerText : public RenderLayer {
public:
    std::string timestr;
    RenderLayerText(float vpx1, float vpy1, float vpx2, float vpy2, Application* app, RenderLayerTextLines* lines = nullptr);
    void setFont(ImFont* font);
    //void setFont(std::string* path, unsigned int size);
    void setCelestialMech(Astronomy* astro);
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
    bool minusday = false;
    bool plusday = false;
    float slideday = 0.0f;

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
//  RenderChain
// -------------
class RenderChain {
    Application* m_app = nullptr;
public:
    // Rendering to file
    std::string basefname = "Basefilename";
    unsigned int currentframe = 0;
    unsigned int currentseq = 0;
    bool renderoutput = false;
    // Should probably be private, but how to pick up layers from python then? !!!
    std::vector<RenderLayer*> m_layers;  // RenderLayer is a parent object to RenderLayer3D, RenderLayerText, RenderLayerGUI, ...

private:

public:
    RenderChain(Application* app);
    RenderLayer3D* newLayer3D(float vpx1, float vpy1, float vpx2, float vpy2, Scene* scene, Astronomy* astro, Camera* cam = nullptr);
    RenderLayerText* newLayerText(float vpx1, float vpy1, float vpx2, float vpy2, RenderLayerTextLines* lines = nullptr);
    RenderLayerGUI* newLayerGUI(float vpx1, float vpy1, float vpx2, float vpy2);
    RenderLayerPlot* newLayerPlot(float vpx1, float vpy1, float vpx2, float vpy2);
    void updateView(int w, int h);
    void do_render();
    void RenderFrame(unsigned int framebuffer);
    void incSequence();
};


// -------------
//  Application
// -------------
class Application { // Act as an object factory, like world used to do
public:
    bool start_fullscreen = false;
    GLFWwindow* window = nullptr;
    int w_width = 800;      // Save window width when going full screen
    int w_height = 600;

    // ImGUI
    bool imgui_ready = false;
    ImFont* m_font1 = nullptr;
    ImFont* m_font2 = nullptr;

    // Keyboard activated items
    Camera* currentCam = nullptr;
    Earth* currentEarth = nullptr;
    bool togglefullwin = false;
    bool isfullscreen = false;

    // Debugging options
    bool interactive = false;
    bool breakpoint = false;
    bool dumpcam = false;

    // Scripting
    bool ipython = false;

    bool gui = false;
    //bool datetimegui = false;
    bool render = false; // Whether to write image to disk
    bool anim = false;

    bool do_eot = false;
    // Camera trims - global so kbd handler can access it
    float camSpd = CAMERA_ANGLE_STEP;
    float camlightsep = 2.2f;  // Factor to offset light from camera
    glm::vec3 CamLightDir = glm::vec3(0.0f, 0.0f, 1.0f);

private:
    RenderChain* m_renderchain = nullptr;
    ShaderLibrary* m_shaderlib = nullptr;

    // For windowed / fullscreen control
    GLFWmonitor* monitor = nullptr;    // monitor handle, used when setting full screen mode
    const GLFWvidmode* mode = nullptr;
    int win_width = 800;    // Stores Windowed width/height while in fullscreen
    int win_height = 600;
    int w_posx = 0;         // Ditto for position
    int w_posy = 0;

public:
    Application();
    RenderChain* getRenderChain();
    ShaderLibrary* getShaderLib();
    Astronomy* newAstronomy();
    Scene* newScene();
    int initWindow();
    void SetWH(int w, int h);
    int getWidth();
    int getHeight();
    void update();
    //Viewport* getWindow2Viewport();
    float getAspect();
    void initImGUI();
    void beginImGUI();
    void endImGUI();
};


// ------------------
//  PictureInPicture
// ------------------
//class PictureInPicture {
//public:
//
//private:
//    Scene* m_scene = nullptr;
//    unsigned int quadVAO, quadVBO;
//    GLint m_width = 0;
//    GLint m_height = 0;
//    GLint m_viewport[4] = { 0, 0, 0, 0 };  //startx,starty, width,height
//    unsigned int m_fbo = maxuint;  // Frame Buffer Object
//    unsigned int m_tex = maxuint;  // Texture for color data (RenderID)
//    unsigned int m_rbo = maxuint;  // Render Buffer Object for Depth and Stencil buffers
//    unsigned int m_texslot = GL_TEXTURE7;
//    Shader* m_shdr;
//    VertexArray* va = nullptr;
//    VertexBuffer* vb1 = nullptr;
//    VertexBufferLayout* vbl1 = nullptr;
//    IndexBuffer* ib = nullptr;
//
//    // PiP Geometry
//    struct SimpleVertex {
//        glm::vec3 pos;
//        glm::vec2 uv;
//    };
//    std::vector<SimpleVertex> m_verts;
//    std::vector<Tri> m_tris;
//
//public:
//    PictureInPicture(Scene* scene, GLsizei width = 600, GLsizei height = 400) : m_scene(scene), m_width(width), m_height(height) {
//        // Frame Buffer Object
//
//        glGenFramebuffers(1, &m_fbo);
//        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//        // Texture Attachment
//        glActiveTexture(m_texslot);
//        glGenTextures(1, &m_tex);
//        glBindTexture(GL_TEXTURE_2D, m_tex);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); // May need to set glViewport
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glBindTexture(GL_TEXTURE_2D, 0);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);
//        // Depth and Stencil Buffer Attachments - Using Render Buffer rather than Texture, so can only write and not sample in shader
//        glGenRenderbuffers(1, &m_rbo);
//        glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
//        glBindRenderbuffer(GL_RENDERBUFFER, 0);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
//
//        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//            std::cout << "ERROR: PictureInPicture() Framebuffer is not complete!\n";
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        // Set up a basic shader
//        m_shdr = m_scene->m_app->getShaderLib()->getShader(PIP_SHADER);
//
//        // Set up for rendering quad
//        //genGeom();
//        //vbl1 = new VertexBufferLayout; // Vertices list
//        //vbl1->Push<float>(3);     // Vertex coord (pos)
//        //vbl1->Push<float>(2);     // UV coord
//        //va = new VertexArray;
//        //vb1 = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(SimpleVertex));
//        //va->AddBuffer(*vb1, *vbl1, true);
//        //ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);  // IB uses COUNT, not BYTES!!!
//
//        // From LearnOpenGL
//        float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
//        // positions   // texCoords  // in OpenGL NDC (-1;1), -Z facing camera
//        //-1.0f,  1.0f,  0.0f, 1.0f,
//        //-1.0f, -1.0f,  0.0f, 0.0f,
//        // 1.0f, -1.0f,  1.0f, 0.0f,
//        //
//        //-1.0f,  1.0f,  0.0f, 1.0f,
//        // 1.0f, -1.0f,  1.0f, 0.0f,
//        // 1.0f,  1.0f,  1.0f, 1.0f
//         0.5f,  1.0f,  0.0f, 1.0f,  // Smaller quad upper right 1/16th of screen
//         0.5f,  0.5f,  0.0f, 0.0f,
//         1.0f,  0.5f,  1.0f, 0.0f,
//        
//         0.5f,  1.0f,  0.0f, 1.0f,
//         1.0f,  0.5f,  1.0f, 0.0f,
//         1.0f,  1.0f,  1.0f, 1.0f
//        };
//        // screen quad VAO
//        glGenVertexArrays(1, &quadVAO);
//        glGenBuffers(1, &quadVBO);
//        glBindVertexArray(quadVAO);
//        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
//        glEnableVertexAttribArray(0);
//        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//        glEnableVertexAttribArray(1);
//        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//
//    }
//    ~PictureInPicture() {
//        //delete ib;   // Disabled while using the crude OpebGL code from LearnOpenGL
//        //delete va;
//        //delete vb1;
//        //delete vbl1;
//        glDeleteFramebuffers(1, &m_fbo);
//    }
//    unsigned int getFB() {
//        return m_fbo;
//    }
//    void Bind() {
//        // Store current viewport
//        glGetIntegerv(GL_VIEWPORT, m_viewport);
//        //std::cout << "PiP viewport: " << m_viewport[0] << "," << m_viewport[1] << "," << m_viewport[2] << "," << m_viewport[3] << "\n";
//        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
//        glViewport(0, 0, m_width, m_height);
//    }
//    void Unbind() {
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);
//        glViewport(m_viewport[0], m_viewport[1], m_viewport[2], m_viewport[3]);
//    }
//    void Draw() {
//        // Simply draw the quad with the texture from the frame buffer
//        glActiveTexture(m_texslot);
//        glBindTexture(GL_TEXTURE_2D, m_tex);
//        m_shdr->Bind();
//        m_shdr->SetUniform1i("tex", m_texslot-GL_TEXTURE0);
//        //m_shdr->SetUniformMatrix4f("view", m_world->w_camera->GetViewMat());
//        //m_shdr->SetUniformMatrix4f("projection", m_world->w_camera->GetProjMat());
//
//        //vb1->Bind(); // Bound by va->AddBuffer
//        //va->Bind();
//        //va->AddBuffer(*vb1, *vbl1, true);
//        //ib->Bind();
//        ////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//        //glDisable(GL_DEPTH_TEST);
//        //glDisable(GL_CULL_FACE);
//        //glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, (const void*)&m_tris);
//        ////glDrawElements(GL_TRIANGLES, 2, GL_UNSIGNED_INT, (const void*)&m_tris);
//        //glEnable(GL_DEPTH_TEST);
//        //glEnable(GL_CULL_FACE);
//        //ib->Unbind();
//        //va->Unbind();
//        //vb1->Unbind();
//
//        // From LearnOpenGL
//        glBindVertexArray(quadVAO);
//        glDisable(GL_CULL_FACE);
//        glDisable(GL_DEPTH_TEST);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        glEnable(GL_DEPTH_TEST);
//        glEnable(GL_CULL_FACE);
//
//        m_shdr->Unbind();
//    }
//    void genGeom() {
//        //                  pos                          uv
//        m_verts.push_back({ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) });
//        m_verts.push_back({ glm::vec3(0.5f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) });
//        m_verts.push_back({ glm::vec3(1.0f, 0.5f, 0.0f), glm::vec2(1.0f, 0.0f) });
//        m_verts.push_back({ glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 0.0f) });
//        //m_verts.push_back({ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) });
//        //m_verts.push_back({ glm::vec3(0.5f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) });
//        //m_verts.push_back({ glm::vec3(1.0f, 0.5f, 0.0f), glm::vec2(1.0f, 0.0f) });
//        //m_verts.push_back({ glm::vec3(0.5f, 0.5f, 0.0f), glm::vec2(0.0f, 0.0f) });
//        m_tris.push_back({ 0,3,2 });
//        m_tris.push_back({ 0,1,3 });
//        //m_tris.push_back({ 4,6,7 });
//        //m_tris.push_back({ 4,7,5 });
//    }
//};


// --------
//  Skybox
// --------
class SkyBox {
private:
    //World* m_world;
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
    //std::vector<std::string> m_faces = {
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_right.png",
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_left.png",
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_top.png",
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_bottom.png",
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_front.png",
    //    "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap_8k_back.png"
    //};
    std::vector<std::string> m_faces = {
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_front.png",    // +x
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_back.png",     // -x
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_right.png",    // +y
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_left.png",     // -y
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_top.png",      // +z
        "C:\\Coding\\Eartharium\\Eartharium\\textures\\cubemap_bottom.png"    // -z
    };
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
        unsigned int index = maxuint;
    };
    Scene* m_scene = nullptr;
    unsigned int m_number = maxuint;
    glm::vec4 m_color = WHITE;
    float m_size = 0.0f;
    unsigned int m_spacing = 0;
    unsigned int m_gap = 0;
    double m_sizefactor = 1.0;
    std::deque<particle> m_queue;

public:
    ParticleTrail(Scene* scene, unsigned int number, glm::vec4 color, unsigned int spacing = 0);
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
        unsigned int cone = maxuint;
        glm::vec4 color;
        glm::vec3 position;
        glm::vec3 start;
        glm::vec3 stop;
        float length;
        float width;
        double angle;
    };
    std::vector<AngleArc> m_arcs;
public:
    AngleArcs(Scene* scene);
    ~AngleArcs();
    double getAngle(unsigned int index);
    unsigned int add(glm::vec3 position, glm::vec3 start, glm::vec3 stop, float length, glm::vec4 color = LIGHT_GREY, float width = 0.001f);
    void update(unsigned int index, glm::vec3 position, glm::vec3 start, glm::vec3 stop, float length, glm::vec4 color = LIGHT_GREY, float width = 0.001f);
    void draw();

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
    std::vector<glm::vec3> m_points;
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
public:
    PolyCurve(Scene* scene, glm::vec4 color, float width);
    ~PolyCurve();
    void AddPoint(glm::vec3 point);
    void ClearPoints();
    void Generate();
    void Draw();
    void genGeom();
};


// --------
//  Arrows
// --------
class Arrows {
private:
    struct Arrow {
        unsigned int cylinder;
        unsigned int cone;
        glm::vec4 color;
        glm::vec3 position;
        glm::vec3 direction;
        float length;
        float width;
    };
    Scene* m_scene = nullptr;
    Cylinders* m_cylinders = nullptr;
    Cones* m_cones = nullptr;
    tightvec<Arrow> m_arrows;
public:
    Arrows(Scene* scene);
    ~Arrows();
    void Draw();
    unsigned int Store(Arrow a);
    void Delete(unsigned int index);
    unsigned int FromStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    unsigned int FromStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    void UpdateStartDirLen(unsigned int arrow, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void UpdateStartEnd(unsigned int arrow, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
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
public:
    void Draw(unsigned int shadow);
    void clear();
    Primitive3D* getDetails(unsigned int index);
protected:
    Primitives(Scene* scene, unsigned int verts, unsigned int tris);
    ~Primitives();
    void Init();
    unsigned int Store(Primitive3D p);
    void Update(unsigned int oid, Primitive3D p);
    void Remove(unsigned int oid);
    void virtual genGeom() = 0;
    glm::vec4 getColor(unsigned int index);
};


// ----------
//  Minifigs
// ----------
class Minifigs : public Primitives {
public:
    Minifigs(Scene* scene);
    ~Minifigs();
    unsigned int FromStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color, float bearing);
    void UpdateStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color, float bearing);
    unsigned int FromStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
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
    void Print();
    void Delete(unsigned int index);
    unsigned int FromStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    unsigned int FromStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //unsigned int FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void UpdateStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void UpdateStartEnd(unsigned int index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //void UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color);
    glm::vec3 getLoc3D_NS(float lat, float lon, float height);
private:
    void genGeom() override;
};


// ---------
//  Letters
// ---------
class Letters : public Primitives {
public:
    Letters(Scene* scene);
    void Delete(unsigned int index);
    unsigned int FromStartNormalLen(glm::vec3 pos, glm::vec3 nml, float len, glm::vec4 color);
    unsigned int FromStartUV(glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color);
    void UpdateStartNormalLen(unsigned int index, glm::vec3 pos, glm::vec3 nml, float len, glm::vec4 color);
    void UpdateStartUV(unsigned int index, glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color);
private:
    void genGeom() override;
};


// --------
//  Planes
// --------
class Planes : public Primitives {
public:
    Planes(Scene* scene);
    void Delete(unsigned int index);
    unsigned int FromStartNormalLen(glm::vec3 pos, glm::vec3 nml, float rot, float len, glm::vec4 color);
    unsigned int FromStartUV(glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color);
    void UpdateStartNormalLen(unsigned int index, glm::vec3 pos, glm::vec3 nml, float rot, float len, glm::vec4 color);
    void UpdateStartUV(unsigned int index, glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color);
private:
    void genGeom() override;
};


// -----------
//  ViewCones
// -----------
class ViewCones : public Primitives {
public:
    ViewCones(Scene* scene);
    void Delete(unsigned int index);
    unsigned int FromStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    unsigned int FromStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //unsigned int FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void UpdateStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void UpdateStartEnd(unsigned int index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //void UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color);
private:
    void genGeom() override;
};


// -------
//  Cones
// -------
class Cones : public Primitives {
public:
    Cones(Scene* scene);
    void Print();
    void Delete(unsigned int index);
    unsigned int FromStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    unsigned int FromStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //unsigned int FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void UpdateStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void UpdateStartEnd(unsigned int index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //void UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color);
private:
    void genGeom() override;
};


// -----------
//  Cylinders
// -----------
class Cylinders : public Primitives {
public:
    Cylinders(Scene* scene);
    void Print();
    void Delete(unsigned int index);
    unsigned int FromStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    unsigned int FromStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //unsigned int FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color);
    void UpdateStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color);
    void UpdateStartEnd(unsigned int index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color);
    //void UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color);
private:
    void genGeom() override;
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
    //~Dots();
    //void Draw();             // Implement !!!
    void Delete(unsigned int index);
    unsigned int FromXYZ(glm::vec3 pos, glm::vec4 color, float size);
    void UpdateXYZ(unsigned int index, glm::vec3 pos, glm::vec4 color, float size);
private:
    void genGeom() override;
    TriangleList subdivide(VertexList& vertices, TriangleList triangles);
    Index vertex_for_edge(Lookup& lookup, VertexList& vertices, Index first, Index second);
};


