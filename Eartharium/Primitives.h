#pragma once

//#include <iterator>
#include <vector>
#include <map>
#include <deque>

#include "OpenGL.h"
//#include "Utilities.h"
#include "config.h"

// Proto
class Arrows;
class Application;
class AngleArcs;
class Cones;
class Cylinders;
class Dots;
class Font;
class Glyphs;
class Minifigs;
class Planes;
class Planetoid;
class PolyCurve;
class PolyLine;
class Primitives;
class ShadowBox;
class ShadowMap;
class SkyBox;
class SkyDots;
class SphereUV;
class StarTrail;
class TextFactory;
class TextString;
class ViewCones;

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
//    // Should be templated so it can work with different objects: floats, doubles, vec3, vec4, LLD, etc.
//    // To work with LLD, LLD will need at least a + operator
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
//    static void generate(LLD llh1, LLD llh2, ) {
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
