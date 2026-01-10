#pragma once

#include <string>

#include <glm/gtx/rotate_vector.hpp>

#include "Astronomy.h"
#include "Primitives.h"
#include "Renderer.h"  // -> SceneObjects

#include "../../EAstronomy/aellipsoids.h"
#include "../../EAstronomy/aearth.h"
#include "../../EAstronomy/amoon.h"

// =====================================================================================================
//  Experimental area for building SceneObject based modular objects
// =====================================================================================================


// Protos for new SceneObject derived features
class Latitude;
class Longitude;
class Equator;
class PrimeMeridian;
class Grid;
class SmallCircle;
class BodyGeometry;
class LocationSO;



// ------------
//  Image Quad
// ------------
// Loads two pictures and layers them one atop the other
// Allows to transform both pictures (translation, scaling, rotation)
// Allows to swap which is on top, i.e. "blink" the images
// Allows to add transparency
class Quad : public SceneObject {
    struct QuadTri {
        QuadTri(const glm::vec3 pos, const glm::vec2 tex) : position(pos), uv(tex) {}
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec2 uv = glm::vec2(0.0f);
    };
public:
    Quad(Scene* scene, SceneObject* parent, const std::string& image, TextureType textype);
    ~Quad();
    void loadImage(std::string& image);
    bool update() override;
    void draw(Camera* cam) override;
public:
    bool invert{ false };
    bool alpha_on{ false };
    float alpha{ 1.0f };
    float rotation{ 0.0f };
    std::string* filename{ nullptr };
private:
    VertexArray* va{ nullptr };
    VertexBuffer* vb{ nullptr };
    VertexBufferLayout* vbl{ nullptr };
    Shader* shdr{ nullptr };
    Texture* texture{ nullptr };
    TextureType textype = DUMMY;
    float transparancy = 1.0f;  // Opaque by default
    glm::vec3 translate{};
    glm::vec3 rotate{};
    std::vector<QuadTri> quad;
};


// --------------
//  Blink tester
// --------------
// Loads two pictures and layers them one atop the other
// Allows to transform both pictures (translation, scaling, rotation)
// Allows to swap which is on top, i.e. "blink" the images
// Allows to add transparency
class BlinkTester : public SceneObject {
    struct QuadTri {
        QuadTri(const glm::vec3 pos, const glm::vec2 tex) : position(pos), uv(tex) {}
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec2 uv = glm::vec2(0.0f);
    };
public:
    struct Blink_Entry {
        // Use NO_DOUBLE for jd if time is unknown or irrelevant
        double jd;
        std::string filename;
    };
    BlinkTester(Scene* scene, SceneObject* parent, std::vector<Blink_Entry>* image1, std::vector<Blink_Entry>* image2);
    ~BlinkTester();
    void setImage1(size_t index);
    void setImage2(size_t index);
    bool update() override;
    void draw(Camera* cam) override;
    void myGUI();
public:
    float timeshift{ 0.0f };
    float pos_ang{ 0.0f };
    float par_ang{ 0.0f };
    float lat_ang{ 0.0f };
    Quad* blink1{ nullptr };
    Quad* blink2{ nullptr };
private:
    // Blink1
    size_t blink1_num{ 0 };
    int gui_blink1_num{ 0 };
    std::vector<Blink_Entry>* blink1_list{ nullptr };
    // Blink2
    size_t blink2_num{ 0 };
    int gui_blink2_num{ 0 };
    std::vector<Blink_Entry>* blink2_list{ nullptr };
};


// =========================
//  Location Specific Items
// =========================

// --------
//  LocDot
// --------
// LocationSO specific interface to Dots primitives
class LocDot : public SceneObject {
public:
    LocDot(LocationSO* location);
    void setColor(glm::vec4 color);
    void setRadius(float radius);
    void draw(Camera* cam) override;
    bool update() override;
private:
    LocationSO* location{ nullptr };
    glm::vec4 color{ DEFAULT_LOCDOT_COLOR };
    size_t dot{ 0 };
    float radius{ DEFAULT_LOCDOT_SIZE };
};

// ----------------
//  LocZenithArrow
// ----------------
class LocZenithArrow : public SceneObject {
public:
    LocZenithArrow(LocationSO* location);
    void setColor(glm::vec4 color);
    //void setRadius(float radius);
    void draw(Camera* cam) override;
    bool update() override;
private:
    LocationSO* location{ nullptr };
    glm::vec4 color{ LIGHT_BLUE };
    size_t arrow{ NO_UINT };
    //float radius{ locdotsize };
    glm::vec3 my_pos{ 0.0f };
    glm::vec3 my_dir{ 0.0f };
};

// --------------
//  HorizonPlane   (for LocationSO)
// --------------
class HorizonPlane : public SceneObject {
public:
    HorizonPlane() = delete;
    HorizonPlane(LocationSO* location, glm::vec4 color = GREEN);
    void draw(Camera* cam) override;
    bool update() override;
private:
    LocationSO* location{ nullptr };
    glm::vec4 color{ GREEN };
    size_t plane{ NO_UINT };
};

// -----------
//  LocSundir
// -----------
class LocSundir : public SceneObject {
public:
    LocSundir(LocationSO* location);
    void setColor(glm::vec4 color);
    void draw(Camera* cam) override;
    bool update() override;
private:
    LocationSO* location{ nullptr };
    glm::vec4 color{ LIGHT_YELLOW };
    size_t arrow{ 0 };
    glm::vec3 my_pos{ 0.0f };
    glm::vec3 my_dir{ 0.0f };
};

// ------------
//  LocationSO
// ------------
class LocationSO : public SceneObject {  //  Location for BodyGeometry derived objects
public:
    LocationSO(BodyGeometry* body, LLD location);
    ~LocationSO();
    float getRadius();
    void setRadius(float radius);
    glm::vec3 getPosition();
    glm::vec3 getNorth();
    glm::vec3 getEast();
    glm::vec3 getZenith();
    glm::vec3 getSundir();
    glm::vec3 calcEleAzi2Dir(LLD heading);
    LocDot* addLocDot(float size = 0.002f, glm::vec4 color = DEFAULT_LOCDOT_COLOR);
    LocZenithArrow* addZenithArrow();
    HorizonPlane* addHorizonPlane(glm::vec4 color = GREEN);
    LocSundir* addTrueSundir();
    void draw(Camera* cam) override;
    bool update() override;
public:
    BodyGeometry* bodygeometry{ nullptr };  //  !!!FIX: How do I get astronomical positions from this?
    LocDot* locdot{ nullptr };
    LocZenithArrow* zenitharrow{ nullptr };
    HorizonPlane* horizonplane{ nullptr };
    LocSundir* truesundir{ nullptr };
private:
    LLD loc;  //  Current location on body in geographic coordinates (consider generalizing to planetographic / selenographic)
    float radius{ 0.2f };
    glm::vec3 pos{ 0.0f };  // Position in 3D world coordinates
    glm::vec3 zenith{ 0.0f };
    glm::vec3 north{ 0.0f };
    glm::vec3 east{ 0.0f };
    LLD sunpos;
};

// ---------------
//  LocationGroup
// ---------------
class LocationGroup {
public:
    LocationGroup() = delete;
    LocationGroup(std::string name = "NO_NAME") {
        this->name = name;
    }
    ~LocationGroup() {
        for (auto loc : locations) {
            delete loc;
        }
    }
    void setGeometryBody(BodyGeometry* bodygeometry) {
        body = bodygeometry;
    }
    LocationSO* addLocation(LLD loc, std::string name = "NO_NAME") {
        locations.emplace_back(new LocationSO(body, loc));
        locations.back()->name = name;
        return locations.back();
    }
    void removeLocation(LocationSO* location) {
        size_t idx{ NO_UINT };
        for (size_t i = 0; i < locations.size(); i++) {
            if (locations[i] == location) idx = i;
        }
        if (idx != NO_UINT) {
            delete locations[idx];
            locations.erase(locations.begin() + idx);
        }
    }
public:
    std::string name;
    std::vector<LocationSO*> locations;
private:
    BodyGeometry* body{ nullptr };
};


// --------------
//  BodyGeometry
// --------------
// For Loc3D and Nml3D functions, which will be used from other SceneObjects
typedef glm::vec3(BodyGeometry::* bglocPos)(const LLD);
typedef glm::vec3(BodyGeometry::* bglocNml)(const LLD);

enum MaterialID {  // Index into Material array
    DETAILED_EARTH = 0,
    DETAILED_MOON,
    DETAILED_SKY
};
class BodyGeometry : public SceneObject {
    // BodyGeometry TODO:
    // - add more geometries, perhaps also some humourous ones (cube, pear, egg)
    // - Factor out the Material class to a general material handler
    // - Alpha blending to show ellipsoid centric coordinate axes etc
    // - "Peal open" animations, over latitude and longitude

    struct Material {
        unsigned int shader = 0;
        unsigned int texture1 = 0;
        unsigned int texture2 = 0;
        unsigned int texture3 = 0;
    };
    std::array<Material, 3> Materials = { {
        { EARTH_SHADER, EARTH_DAY, EARTH_NIGHT, EARTH_BUMP },
        { MOON_SHADER, MOON_FULL, NO_UINT, MOON_BUMP },
        { SKY_SHADER, SKY_FULL, NO_UINT, NO_UINT } // Will be DetailedSky, if that turns out to be a good path forward.
    } };
public:
    Equator* equator = nullptr;
    PrimeMeridian* primem = nullptr;
    Grid* grid = nullptr;
    float radius = 1.0f;
    float morph_param = 0.0f;   // Valid range is 0.0f to 1.0f
    // Update flags - will probably be handled via update*() parameters
    bool dirty_geometry = true;
    //bool dirty_time = true;

    // Location Groups
    std::vector<LocationGroup*> locgroups;

    // Holds geometry information about a morphable body, e.g. Earth
    // Has methods to convert between spherical and cartesian surface coordinates
    // and supply surface normals.
    // Is always orientated NP up, EQ parallel to XY plane, X axis through prime meridian in model coordinates
    // -> This now implements worldmatrix, so NP is not always UP etc. in world coordinates
    // For ellipsoid (and other?) shapes, body specific parameters are required !!!
    // If those are always supplied, then this can include distance functions etc.
    // But how do we get accurate distances on NSER at 0.3 morph for example?
    // Better to have an AstroObject that can convey the parameters
    // - Equatorial radius
    // - Polar radius
    // - Eccentricity / flattening (or calculate based on above)
    // Question: when using linear morph, are normals always (approximately) correct throughout morph? Probably not!
    BodyGeometry(Scene* scene, SceneObject* parent, unsigned int material, std::string mode, unsigned int meshU, unsigned int meshV, float gpuRadius);
    ~BodyGeometry();

    LocationGroup* addLocationGroup(std::string name) {
        locgroups.emplace_back(new LocationGroup(name));
        locgroups.back()->setGeometryBody(this);
        return locgroups.back();
    }
    void removeLocationGroup(std::string name) {
        size_t idx{ 0 };
        for (auto lg : locgroups) {
            if (lg->name == name) {
                delete lg;
                locgroups.erase(locgroups.begin() + idx);
                return;
            }
            idx++;
        }
    }
    void addEquator();
    void addPrimeMeridian();
    //void addLatitude();  // Should all of these be in Grid? Maybe no. It can be handy to have custom Lat / Lon outside Grid.
    //void addLongitude();
    void addGrid(double spacing = 15.0);

    void draw(Camera* cam) override;
    bool virtual drawSpecific(Camera* cam, Shader* shdr);  // Can be overridden to add specific draw needs (setting up the camera, etc)
    void setMode(const std::string mode);
    std::string getMode() const;
    void setMorphParameter(float parameter);
    float getMorphParameter() const;
    void setEllipoid(Ellipsoid_2axis ellipsoid);
    // createShape() might get tri and vert vectors if rendering is done in SceneObject or elsewhere - decide in constructor
    void createShape();    // Creates the mesh geometry, should probably only be called once at creation
    void updateShape();    // Updates the mesh geometry, called from users like DetailedMoon etc
    // BodyGeometry uses same coordinate system as Earth lat.lon.
    // Derived body might use something else, the following two should be overridden convert, or confirm the format
    LLD virtual toLocalCoords(LLD loc);      // From interface coordinates (e.g. DecRA) to local
    LLD virtual fromLocalCoords(LLD loc);    // From local to interface coordinates (future use)
    glm::vec3 getLoc3D(const LLD loc);
    glm::vec3 getNml3D(const LLD loc);
    glm::vec3 getLoc3D_NS(const LLD loc);  // Normal Sphere
    glm::vec3 getNml3D_NS(const LLD loc);
    glm::vec3 getLoc3D_AE(const LLD loc);  // Azimuthal Equidistant
    glm::vec3 getNml3D_AE(const LLD loc);
    glm::vec3 getLoc3D_ER(const LLD loc);  // Equidistant Rectangular
    glm::vec3 getNml3D_ER(const LLD loc);
    glm::vec3 getLoc3D_GE(const LLD loc);  // Generic Ellipsoid (2-axis)
    glm::vec3 getNml3D_GE(const LLD loc);
    // FIX !!! Add additional geometries here !!!
    glm::vec3 getNorth3D(LLD loc);
    glm::vec3 getEast3D(LLD loc);

private:
    // Body Geometry
    std::string m_mode = "XXXX";
    bglocPos pos_mode1 = nullptr;
    bglocPos pos_mode2 = nullptr;
    bglocNml nml_mode1 = nullptr;  //e.g. &BodyGeometry::getNml3D_NS;
    bglocNml nml_mode2 = nullptr;
    Ellipsoid_2axis ellipsoid{ AEllipsoid::Earth_WGS84 };
    // Used when generating texture coordinates in genMesh()
    float texoffset_x = 0.0f; // -3.5f; // FIXME: use a texture definition file to supply these values
    float texoffset_y = 0.0f; // 1.3f;  // Values given in 1/8192 x 1/4096 units, ideally pixels in the texture

    // Mesh data
    unsigned int m_meshU = 90, m_meshV = 45;
    struct vertex {
        glm::vec3 position = NO_VEC3;
        glm::vec3 light_normal = NO_VEC3;
        glm::vec3 surface_normal = NO_VEC3;
        glm::vec2 texture_uv = NO_VEC2;
        glm::vec4 color = WHITE;
    };
    struct tri {
        unsigned int a = NO_UINT;
        unsigned int b = NO_UINT;
        unsigned int c = NO_UINT;
    };
    std::vector<vertex> vertices;
    std::vector<tri> tris;

    // OpenGL
    VertexBufferLayout* vbl = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb = nullptr;
    IndexBuffer* ib = nullptr;
    Shader* shdr = nullptr;
    Texture* m_texture1 = nullptr;   // E.g. day and night textures
    Texture* m_texture2 = nullptr;   // Might add third for overlays such as temperature maps or eclipse paths
    Texture* m_texture3 = nullptr;   // Currently used for bump height mapping on Earth & Moon
    //unsigned int texture_overlay_mode = NONE;
};


// ------------
//  Smart Path
// ------------
// Can this be made to support both PolyCurveSO and PolyLine ? Yes, derive both from a PolyThing.
class SmartPath : public SceneObject {
    // Flexibly allocates additional PolyCurveSO objects when addSplit() is called
public:
    SmartPath(Scene* scene, SceneObject* parent, float width = NO_FLOAT, glm::vec4 color = NO_COLOR);
    ~SmartPath();
    void setColor(glm::vec4 color);
    void setWidth(float width);
    void addPoint(glm::vec3 point);
    void addSplit(glm::vec3 point1, glm::vec3 point2);
    void clearPoints();
    void generate();
    void draw(Camera* cam);
    bool update() override {
        //std::cout << "SmartPath(" << this << ")::update(): Setting hidden = " << hidden << "\n";
        for (auto& curve : m_curves) {
            curve->hidden = hidden;
            //curve->setColor(LIGHT_RED);
        }
        return false;
    }
private:
    glm::vec4 m_color{ NO_COLOR };
    float m_width{ NO_FLOAT };
    std::vector<PolyCurveSO*> m_curves;
    unsigned int m_curve = 0;
};


// ---------------
//  GeneratedPath
// ---------------
class GeneratedPath;
typedef LLD(GeneratedPath::* calcFuncGP)(LLD, LLD, double, double);

class GeneratedPath : public SceneObject {
public:
    struct PathGenerator {
        GenericPath* gpath{ nullptr };
        itemtype type{ itemtype::NONE };
        float width{ NO_FLOAT };
        glm::vec4 color{ NO_COLOR };
        LLD lld1;
        LLD lld2;
        double refraction{ 0.0 };
        double fend{ 0.0 };                // 1.0 or tau depending on calculation function
        calcFuncGP ca = &calcTerminator;   // calculation function
        bool closed{ false };
    };
    GeneratedPath(Scene* scene, SceneObject* parent, BodyGeometry* body, PathGenerator* pathgen)
        : SceneObject(scene, parent), pathgenerator(pathgen), body(body) {
        pathgenerator->gpath = new GenericPath(scene, pathgen->width, pathgen->color);
        updatePath(pathgenerator);
    }
    ~GeneratedPath() {
        //delete pathgenerator.gpath;  // pathgenerator owned by body
    }

    glm::vec3 getLoc3D_NS(double rLat, double rLon) {  // !!! FIX: Use the one from BodyGeometry
        float w = (float)(cos(rLat) * m_Radius);
        return glm::vec3((float)(cos(rLon) * w), (float)(sin(rLon) * w), (float)(sin(rLat) * m_Radius));
    }
    LLD getXYZtoLLD_NS(glm::vec3 pos) {
        // NOTE: Gives height above Center of Earth, not height above Surface!! (i.e. geocentric, not topocentric)
        // Keep in mind that calcTerminator() uses this !!! So don't get too smart with it.
        // std::cout << "XYZ: " << pos.x << "," << pos.y << "," << pos.z << "\n";
        float diag = sqrt(pos.x * pos.x + pos.y * pos.y);
        if (diag < 0.0000001f && pos.z + (float)tiny > 1.0f) return { pi2, 0.0, 0.0 }; // atan2 seems to flip out at the singularities
        if (diag < 0.0000001f && pos.z - (float)tiny < -1.0f) return { -pi2, 0.0, 0.0 };
        return { atan2(pos.z, diag), atan2(pos.y, pos.x), glm::length(pos) };
    }
    LLD calcTerminator(LLD lld1, LLD lld2, double param, double refang) {
        //std::cout << "calcTerminator(): param=" << param << "\n";
        // Note: This is an entirely geometric calculation, based on an exactly spherical body.
        LLD res;
        double sinparm = sin(param);
        res.lat = asin(cos(lld1.lat) * sinparm);
        double x = -cos(lld1.lon) * sin(lld1.lat) * sinparm - sin(lld1.lon) * cos(param);
        double y = -sin(lld1.lon) * sin(lld1.lat) * sinparm + cos(lld1.lon) * cos(param);
        res.lon = atan2(y, x);
        // Refracted terminator
        if (refang != 0.0) {
            glm::vec3 loc = getLoc3D_NS(res.lat, res.lon);
            glm::vec3 sun2 = getLoc3D_NS(lld1.lat, lld1.lon);
            glm::vec3 rloc = glm::rotate(loc, (float)-refang, glm::cross(loc, sun2));
            res = getXYZtoLLD_NS(rloc);
        }
        if (res.lon > pi) res.lon -= tau;
        if (res.lon < -pi) res.lon += tau;
        res.dst = 0.0;
        res.lon = -res.lon;
        return res;
    }
    double calcArcDist(LLD lld1, LLD lld2) {
        double sin1 = sin(lld1.lat);
        double sin2 = sin(lld2.lat);
        double cos1 = cos(lld1.lat);
        double cos2 = cos(lld2.lat);
        double dlon = lld2.lon - lld1.lon;
        double sind = sin(dlon);
        double cosd = cos(dlon);
        double a = sqrt(pow((cos2 * sind), 2) + pow((cos1 * sin2 - sin1 * cos2 * cosd), 2));
        double b = sin1 * sin2 + cos1 * cos2 * cosd;
        return atan2(a, b);
    }
    void updatePath(PathGenerator* p) {
        // NOTE: Height is awkward, not currently tracked/lerped !!!
        //       For Moon and Sun groundpath it is not needed. But for the Lerp and Great paths, it might be nice!
        //       Maybe Satellites and Planes will be implemented with a separate custom path anyway.
        //std::cout << "Path creation started.\n";
        p->gpath->clearPoints();
        glm::vec3 oldap = glm::vec3(1000.0);
        double histep = 1.3 * deg2rad;
        double lostep = 0.24 * deg2rad;
        double dflonflip = 270.0 * deg2rad; //(180.0 - tiny)* deg2rad;
        // Get useful old location and add first point to curve
        LLD first = (this->*p->ca)(p->lld1, p->lld2, 0.0, p->refraction);
        LLD lldf = first;
        glm::vec3 tp = body->getLoc3D(lldf);  // getLoc3D() takes height above sea level
        p->gpath->addPoint(tp);
        oldap = tp;
        //std::cout << "updatePath(): initial tp=";
        //VPRINT(tp) << "\n";

        //addArcPoint2(tp, true, false, oldap, p.gpath); // First=true
        LLD oldf = lldf;
        // IMPORTANT: df must stay in range [0;p.fend] otherwise it makes no sense !!! Plz revise!
        //       UPD: Changed doubling limit to account for this. May cause an infinite loop though !!!
        double dist = calcArcDist(p->lld1, p->lld2);
        double df = 1.0 / (rad2deg * dist); // Reasonable initial stepsize of 1 step per degree? Seems reasonable during casual testing.
        //std::cout << "Initial df: " << df << "\n";
        if (p->type == MOONTERMINATOR) {
            df = deg2rad;
        }
        double f = 0.0; // NOTE: df is added right away in while loop below, but f=0.0 point is already added above.
        double stepscale = 2.0;
        unsigned int half = 0;
        unsigned int doub = 0;
        double dflat = 0.0;
        double dflon = 0.0;
        double ilat = 0.0;
        bool split = false;
        glm::vec3 ip1 = glm::vec3(0.0f);
        glm::vec3 ip2 = glm::vec3(0.0f);
        while (f < p->fend) {
            f += df;
            half = 0;
            doub = 0;
            while (true) {
                // When crossing seam, lon goes from -180 to 180 or reverse; near 360 difference.
                // When crossing pole, lon goes from 0 to 180 or -180, or from -90 to 90, or from -170 to 10 or reverse; near 180 difference.
                // Generate next point
                lldf = (this->*p->ca)(p->lld1, p->lld2, f, p->refraction);
                //std::cout << "Generated point lat,lon: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                if (lldf.lat + tiny > pi2 && lldf.lon == 0.0) {   // If at pole, fix longitude (calculators consider it undefined)
                    lldf.lon = oldf.lon;
                    lldf.lat = pi2 - tiny;
                    //std::cout << "- Pole detected: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                }
                if (lldf.lat - tiny < -pi2 && lldf.lon == 0.0) {   // If at pole, fix longitude (calculators consider it undefined)
                    lldf.lon = oldf.lon;
                    lldf.lat = tiny - pi2;
                    //std::cout << "- Pole detected: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                }
                dflat = abs(lldf.lat - oldf.lat);
                dflon = abs(lldf.lon - oldf.lon);
                // Doubling / increasing
                if (dflat < lostep && dflon < lostep) { // Should really check the sum of squares ?
                    //std::cout << "+df: " << df << " dflat,dflon: " << dflat << "," << dflon << " oldf: " << oldf.lat << "," << oldf.lon << " lldf: " << lldf.lat << "," << lldf.lon << "\n";
                    f -= df; // Should this not be -= ? !!! 
                    df *= stepscale;
                    //doub++;
                    //if (doub == 11) {
                    if (df > p->fend) {
                        std::cout << "WARNING! Earth::updateCompositePath(): Stepsize (" << df << ") exceeds size of full range (" << p->fend << "), resetting to initial value (";
                        if (p->fend == 1.0) df = 1.0 / (rad2deg * dist);
                        if (p->fend == tau) df = deg2rad;
                        std::cout << df << ")!\n";
                        break;
                    }
                    f += df;
                    continue;
                }
                // Triggers when step is within limits in both directions (Not too far, and no inversion)
                if (dflat < histep && dflon < histep) break;
                // Triggers when step is within limits and lon has seam inversion - inversion dealt with below
                if (dflat < histep && dflon > dflonflip) {
                    //std::cout << "Seam inversion detected\n";
                    break;
                }
                // Triggers when step is within limits and lon has pole inversion
                if (dflat < histep && dflon + tiny > pi) {
                    //std::cout << "Pole inversion detected\n";
                    break;
                }
                // Halving / reducing
                if (dflat > histep || dflon > histep) { // (dflat > histep || (dflonflip > dflon && dflon > histep))
                    //std::cout << "-df: " << df << " dflat,dflon: " << dflat << "," << dflon << " oldf: " << oldf.lat << "," << oldf.lon << " lldf: " << lldf.lat << "," << lldf.lon << "\n";
                    if (half == 25) {
                        //std::cout << "Frame: " << m_scene->m_app->currentframe << "\n";
                        //std::cout << "WARNING! Earth::updateCompositePath(): After 25 halvings, stepsize (" << df << ") is still too big, giving up! dflon = " << rad2deg * dflon << "\n";
                        //std::cout << " - Occurred at location: " << oldf.lat * rad2deg << "," << rad2deg * oldf.lon << "\n";
                        if (p->fend == 1.0) df = 1.0 / (rad2deg * dist);
                        if (p->fend == tau) df = deg2rad;
                        f += df;
                        half = 0;
                        break;
                    }
                    f -= df; // Undo the step we took
                    df /= stepscale;
                    f += df;
                    half++;
                    continue;
                }
            }
            // Here we have the next point ready
            tp = body->getLoc3D(lldf);
            // If a SEAM is crossed, split the path and insert intermediate points
            if (dflon > dflonflip) { // Longitude inversion at seam
                if (oldf.lon > lldf.lon) { // Passed counter clockwise (+ to -)
                    ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - lldf.lat) / (lldf.lon + tau - oldf.lon);
                    //std::cout << "Seam splitting counter cloclwise - End: " << ilat << "," << pi - tiny << " Start: " << ilat << "," << tiny - pi << "\n";
                    ip1 = body->getLoc3D({ ilat, pi - tiny, 0.0 });
                    ip2 = body->getLoc3D({ ilat, tiny - pi, 0.0 });
                }
                else { // Passed clockwise
                    ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - lldf.lat) / (lldf.lon + tau - oldf.lon);
                    //std::cout << "Seam splitting clockwise - End: " << ilat << "," << tiny - pi << " Start: " << ilat << "," << pi - tiny << "\n";
                    ip2 = body->getLoc3D({ ilat, pi - tiny, 0.0 });
                    ip1 = body->getLoc3D({ ilat, tiny - pi, 0.0 });
                }
                split = true;
            }

            // If a POLE is crossed, split the path and insert intermediate points
            if (dflon + tiny > pi && !split) { // Longitude inversion at pole, if not already split at seam
                if (oldf.lon > lldf.lon) { // Passed counter clockwise (+ to -) - Will never happen if addArc() is swapping directions to always be clockwise
                    //std::cout << "Pole splitting counter cloclwise - Old: " << rad2deg * oldf.lat << "," << rad2deg * oldf.lon << " new: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                    ip1 = body->getLoc3D({ lldf.lat > 0 ? pi2 : -pi2, oldf.lon, 0.0 }); // End path1
                    ip2 = body->getLoc3D({ lldf.lat > 0 ? pi2 : -pi2, lldf.lon, 0.0 }); // Start path2
                    //std::cout << " - End: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << rad2deg * lldf.lon << "," << " Start: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * oldf.lon << "\n";
                }
                else { // Passed clockwise
                    //std::cout << "Pole splitting cloclwise - Old: " << rad2deg * oldf.lat << "," << rad2deg * oldf.lon << " new: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                    ip1 = body->getLoc3D({ lldf.lat > 0 ? pi2 : -pi2, oldf.lon, 0.0 }); // End path1
                    ip2 = body->getLoc3D({ lldf.lat > 0 ? pi2 : -pi2, lldf.lon, 0.0 }); // Start path2
                    //std::cout << " - End: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * oldf.lon << " Start: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * lldf.lon << "\n";
                }
                split = true;
            }
            if (split) {
                // Now insert the extra points to end one path and begin the next
                p->gpath->addSplit(ip1, ip2);
                //std::cout << "Splitting!\n";
                oldap = ip2;
                //addArcPoint2(ip1, false, true, oldap, p.gpath); // last=true so cap path and start new one
                //curve->generate();
                //curve = p.path2;
                //addArcPoint2(ip2, true, false, oldap, p.gpath);  // First point in new path
                split = false;
            }
            //std::cout << "  length=" << glm::length(tp - oldap) << "\n";
            if (0.0 < f && f < p->fend){  // && glm::length(tp - oldap) < 0.05f) {
                p->gpath->addPoint(tp);
                //std::cout << "Adding Point!\n";
                oldap = tp;
            }
            //if (0.0 < f && f < p.fend) addArcPoint2(tp, false, false, oldap, p.gpath);
            oldf = lldf;
        }
        if (!p->closed) p->gpath->addPoint(body->getLoc3D(p->lld2));
        if (p->closed) p->gpath->addPoint(body->getLoc3D(first));
        p->gpath->generate();
        return;
    }
    void draw(Camera* cam) { return; }
    bool update() override { updatePath(pathgenerator);  return false; }
private:
    PathGenerator* pathgenerator{ nullptr };
    BodyGeometry* body{ nullptr };
    float m_Radius{ 1.0f };
};


// ----------
//  Meridian   (half Great Circle of Longitude)
// ----------
class Longitude : public SceneObject {
public:
    Longitude(Scene* scene, SceneObject* parent, BodyGeometry* geometry, double lon, float width = 0.003f, glm::vec4 color = LIGHT_GREY);
    ~Longitude();
    void setColor(glm::vec4 color);
    void setWidth(float width);
    void generate();
    bool update() override;
    void draw(Camera* cam) override;
private:
    bglocPos locpos = &BodyGeometry::getLoc3D;
    BodyGeometry* locref{ nullptr };
    SmartPath* path{ nullptr };
    double lon{ 0.0 };
};


// ----------
//  Parallel   (Circle of Latitude)
// ----------
class Latitude : public SceneObject {
public:
    Latitude(Scene* scene, SceneObject* parent, BodyGeometry* geometry, double lat, float width = 0.003f, glm::vec4 color = LIGHT_GREY);
    ~Latitude();
    void setColor(glm::vec4 color);
    void setWidth(float width);
    void setLatitude(double lat);
    void generate();
    bool update() override;
    void draw(Camera* cam) override;
private:
    bglocPos locpos = &BodyGeometry::getLoc3D;
    BodyGeometry* locref{ nullptr };
    SmartPath* path{ nullptr };
    double lat{ 0.0 };
};


// ----------------
//  Prime Meridian
// ----------------
class PrimeMeridian : public Longitude {
public:
    PrimeMeridian(Scene* scene, SceneObject* parent, BodyGeometry* geometry, float width = 0.005f, glm::vec4 color = LIGHT_RED);
};

// ---------
//  Equator    - To test with Earth2 and other celestial objects (those are now BodyGeometry)
// ---------
class Equator : public Latitude {
public:
    Equator(Scene* scene, SceneObject* parent, BodyGeometry* geometry, float width = 0.005f, glm::vec4 color = LIGHT_RED);
};

// ------
//  Grid
// ------
class Grid : public SceneObject {
public:
    Grid(Scene* scene, SceneObject* parent, BodyGeometry* geometry, std::string objname = "Grid");
    ~Grid();
    void setName(std::string name);
    void setColor(glm::vec4 color, bool skip_pm_eq = true);
    void setWidth(float width, bool skip_pm_eq = true);
    void setSpacing(double spacing, bool rad);
    void setSpacing(unsigned int divisions);
    Latitude* addLatitude(double latitude);     // Return object pointer instead of index, so caller can modify properties
    Longitude* addLongitude(double longitude);
    void clear();
    bool update();
    void draw(Camera* cam) override;
private:
    BodyGeometry* m_geometry = nullptr;
    float width = 0.005f; // Might move to SceneObject, since so many objects have a width? Well, some do not.
    Equator* equator{ nullptr };
    PrimeMeridian* primemeridian{ nullptr };
    std::vector<Latitude*> latitudes;         // This might be awkward for SceneObject inheritance. No, these are only pointers.
    std::vector<Longitude*> longitudes;
};


// ---------------------
//  Planetocentric Grid   - (not the same as planetographic)
// ---------------------
// NOTE: If the parent object is a morph and the axis is offset from the parent object, orientation parameters are
//       not enough to handle transformations. I.e. orientations parameters do not take morphing into consideration.
// Grid with (Lat Lon) spherical coordinates
// - Axis aligned with body rotation
// - Latitude measured positive towards North, from Equator (which is normal to rotation axis according to right hand rule)
// - Longitude measured positive towards East, from defined origin (different for each object: Earth, Moon, Mars, etc)
class PlanetocentricGrid : public Grid {

private:
    //float m_radius = 1.0f;                    // GPU scale of coordinate sphere, already present in Grid parent.
    glm::vec3 north = { 0.0f, 0.0f, 1.0f };     // North direction unit vector, determines axial rotational orientation parameters.
    glm::vec3 nullpoint = { 1.0f, 0.0f, 0.0f }; // Longitude zero point direction unit vector, determines the rest of rotational o.p.
};

// Planetographic Grid - Verify definitions before implementing
// - Axis aligned with body rotation
// - Latitude measured positive towards North. North defined by rotation axis of body, where up is as viewed from Earth?




// --------------
//  Small Circle
// --------------
class SmallCircle : public SceneObject {
private:
    LLD location{ };
    double radius = 0.0;
    bglocPos locpos = &BodyGeometry::getLoc3D;
    BodyGeometry* locref{ nullptr };
    GenericPath* path{ nullptr };
public:
    SmallCircle(Scene* scene, SceneObject* parent, BodyGeometry* geometry, LLD location, double radius, float width = 0.005f, glm::vec4 color = LIGHT_ORANGE);
    ~SmallCircle();
    void generate();
    bool update();
    void draw(Camera* cam) override;
};


// ------------------
//  Great Circle Arc
// ------------------
class GreatCircleArc : public SceneObject {
private:
    LLD start_point{ };
    LLD split_point{ };
    LLD end_point{ };
    double steps{ 0.0 };
    double steps2{ 0.0 }; // for second sub-arc if needed
    GenericPath* path{ nullptr };
    bglocPos locpos = &BodyGeometry::getLoc3D;
    BodyGeometry* locref{ nullptr };
public:
    GreatCircleArc(Scene* scene, SceneObject* parent, BodyGeometry* geometry, LLD start, LLD end, double steps, float width, glm::vec4 color);
    ~GreatCircleArc();
    void setStart(LLD start);
    void setEnd(LLD end);
    void setStartEnd(LLD start, LLD end);
    LLD calcGreatArc(LLD lld1, LLD lld2, double f);
    void findSplitPoint();
    void generate();
    bool update();
    void draw(Camera* cam) override;
};

//class GreatCircle : public SceneObject {
// It is not straight forward to generate truly correct Great Ellipses using just Loc3D and Nml3D functions.
// So perhaps it is time to build a more complete geometry library, either in BodyGeometry or separately.
// This would be an excellent opportunity to support more ellipsoid models too.
//};


// -----------
//  Planetoid
// -----------
// See: https://www2.mps.mpg.de/homes/fraenz/systems/systems3art/node18.html for possible orientation parameters
class Planetoid : public SceneObject {

    struct AtlasMaterial {
        unsigned int shader = 0;
        unsigned int atlas = 0;
        unsigned int texture1 = 0; // Index in the texture atlas, assuming evenly 4x4 tiled for now
        unsigned int texture2 = 0;
        unsigned int texture3 = 0;
    };
    std::array<AtlasMaterial, 10> Materials = { {  // Now ordered as per Planet enum in EAstronomy/aconfig.h
            // shader           texture file     day      ring     night
            // 0 = Mercury
            { PLANETOID_SHADER, PLANETOID_ATLAS,       1, NO_UINT, NO_UINT },
            // 1 = Venus
            { PLANETOID_SHADER, PLANETOID_ATLAS,       2, NO_UINT, NO_UINT },
            // 2 = Earth
            { PLANETOID_SHADER, PLANETOID_ATLAS,       3, NO_UINT,      10 },
            // 3 = Mars
            { PLANETOID_SHADER, PLANETOID_ATLAS,       4, NO_UINT, NO_UINT },
            // 4 = Jupiter
            { PLANETOID_SHADER, PLANETOID_ATLAS,       5, NO_UINT, NO_UINT },
            // 5 = Saturn
            { PLANETOID_SHADER, PLANETOID_ATLAS,       6,      11, NO_UINT },
            // 6 = Uranus
            { PLANETOID_SHADER, PLANETOID_ATLAS,       7,      12, NO_UINT },
            // 7 = Neptune
            { PLANETOID_SHADER, PLANETOID_ATLAS,       8,      13, NO_UINT },
            // 8 = Sun
            { PLANETOID_SHADER, PLANETOID_ATLAS,       0, NO_UINT, NO_UINT },
            // 9 = Moon
            { PLANETOID_SHADER, PLANETOID_ATLAS,       9, NO_UINT, NO_UINT }
            // For last two atlas slots: asteroid and comet?, generic star and galaxy?, alternate Sun and Venus surface?
        } };

    Shader* shdr = nullptr;
    Texture* tex = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb = nullptr;
    VertexBufferLayout* vbl = nullptr;
    IndexBuffer* ib = nullptr;
    std::vector<Vertex> m_verts;
    std::vector<Tri> m_tris;
    VertexArray* varing = nullptr;
    VertexBuffer* vbring = nullptr;
    VertexBufferLayout* vblring = nullptr;
    IndexBuffer* ibring = nullptr;
    std::vector<Vertex> m_vertsring;
    std::vector<Tri> m_trisring;
    size_t material = NO_UINT;
    unsigned int meshU = 0;
    unsigned int meshV = 0;
    float tex_rx = 4096.0f; // X resolution of atlas
    float tex_ry = 2048.0f; // Y resolution of atlas
    float tex_dx = 0.0f;    // Width of atlas tile
    float tex_dy = 0.0f;    // height of atlas tile
    float tex_lx = 0.0f;    // atlas x offset to tile
    float tex_ly = 0.0f;    // atlas y offset to tile
    float texring_dx = 0.0f;    // Width of atlas tile
    float texring_dy = 0.0f;    // height of atlas tile
    float texring_lx = 0.0f;    // atlas x offset to tile
    float texring_ly = 0.0f;    // atlas y offset to tile
public:
    //glm::vec3 position = glm::vec3(0.0f);
    Planetoid(Scene* scene, SceneObject* parent, Planet texturetile, unsigned int meshU, unsigned int meshV, float radius);
    ~Planetoid();
    void setPosition(glm::vec3 pos);
    void setRadius(float radius);
    bool update() { return false; }; // Nothing to update
    void draw(Camera* cam);
    glm::vec3 getLoc3D_NS(float rLat, float rLon, float height, float radius);
    //glm::vec3 getNml3D(float lat, float lon, float height = 0.0f);
private:
    void updateGeom(float radius);
    void updateGeomRing(float r_i, float r_o);
    void genGeom(float radius);
    void genGeomRing(float r_i, float r_o);
};


// ----------------------
//  General Ground Point
// ----------------------
class PlanetoidGP : public Planetoid {
    // Build more GP objects and see what they have in common to refactor
    // This is for the Sun, on Earth. How to extend to the Sun on the Moon etc?
    // Well, this far, it seems like there is nothing Earth specific here.
public:
    PlanetoidGP() = delete;
    PlanetoidGP(Scene* scene, SceneObject* parent, const size_t type, const std::string& name, const glm::vec3 pos);
    ~PlanetoidGP() = default;  // Will also call ~Planetoid() implicitly
private:
};

// ----------------------
//  Stellar Ground Point
// ----------------------
// Since the General Ground Point (PlanetoidGP) is textured, and I don't want to texture thousands of stars,
// this will use Dots instead of Planetoid. Might rename in future, if other types of untextured ground points
// are needed. Or, since this will likely do lookup in the stellarobjects database, add a third GP type.


// ----------
//  Ecliptic
// ----------
// Plot Ecliptic on Detailed... based on GSID and Obliquity
class Ecliptic : public SceneObject {
public:
    Ecliptic(Scene* scene, SceneObject* parent, BodyGeometry* geometry, float width = 0.003f, glm::vec4 color = LIGHT_GREY);
    ~Ecliptic();
    void setColor(glm::vec4 color);
    void setWidth(float width);
    void generate();
    bool update() override;
    void draw(Camera* cam) override;
private:
    bglocPos locpos = &BodyGeometry::getLoc3D;
    BodyGeometry* locref{ nullptr };
    SmartPath* path{ nullptr };
    BodyGeometry* m_geometry{ nullptr };
};

// -----------------
//  Precession Path
// -----------------
// Path of Earth's north pole on celestial sphere due to precession
// - Might optionally have nutation
// - 
// !!! FIX: Use https://www.aanda.org/articles/aa/full_html/2011/10/aa17274-11/aa17274-11.html to get accurate precession over long periods
class PrecessionPath : public SceneObject {
public:
    PrecessionPath(Scene* scene, SceneObject* parent, BodyGeometry* geometry, float width = 0.003f, glm::vec4 color = GREEN);
    ~PrecessionPath();
    void setColor(glm::vec4 color);
    void setWidth(float width);
    void generate();
    bool update() override;
    void draw(Camera* cam) override;
private:
    bglocPos locpos = &BodyGeometry::getLoc3D;
    BodyGeometry* locref{ nullptr };
    SmartPath* path{ nullptr };
    BodyGeometry* m_geometry = nullptr;

};

// -----------------
//  CelestialSphere
// -----------------
// not currently used. Prefer DetailedSky when possible, and implement this only if DetailedSky can't work.
// Features:
// ---------
// - Can be placed at locations, earth center, sun center, barycentric center, ...
// - Can show different celestial coordinate systems (ecliptic, equatorial, horizontal)
// - Can morph to planisphere, equirectangular, mollweide
//class CelestialSphere : public SceneObject {
//public:
//    Grid* grid = nullptr;
//    CelestialSphere(Scene*, SceneObject*, float);
//    bool update();
//    void draw();
//private:
//    float radius = 1.0f;
//};

// ---------------------
//  Solar System Object
// ---------------------
// This is meant for small representations of Sun, planets (incl Earth), Moon, everything roughly spherical
// They only need low resolution textures, so they can share an atlas
// Implement as spheres or ellipsoids, only a single geometry. So no need for transformations.
// Some of the planets have rings, it would be nice to add those. Consider if you can do this with ring shadows
// For solar system modeling, the following are objects we would like to associate:
// - Orbit
// - Orbital plane
// - Rotation axis
// - Distance vector (from any to any)
// - Billboard with name
// - Planetoid with texture
// Rendering light/dark sides is important
// PlanetoidGP takes care of GPs, so this would just be for SolarSystem for now
// See Dots in SolarSystem; these should all be redundant after this class is complete



// -------------
//  DetailedSky
// -------------
// Implementation list:
// + Sidereal Time rotation
// - Celestial coordinate grid (Dec, RA)
// - Ecliptic coordinate grid
// - Star population
// - Star trails
// - Asterism lines
// - Constellation borders
// - Precession path
// - Sun and planets
//   - Planetoid
//   - Path
// - Zodiac belt
// - GUI for all of the above
class DetailedSky : public BodyGeometry {
    struct SkyDotDef {
        size_t unique_id = 0;    // stellarobject database index
        glm::vec4 color{ 0.0f };
        LLD coordinates{ }; // Dec, RA, height(0.0);
        LLD propermotion{ };
        float size = 0.0f;
        size_t dot_index = 0;    // SkyDots index (GPU table)
    };
    bool textured = false;
    float m_alpha = 1.0f;
    size_t m_sundot = NO_UINT;
    SkyDots* skydotsFactory = nullptr;
    std::vector<SkyDotDef> skydotDefs;

public:
    bool siderealtime{ true };
    bool propermotion{ true };
    bool precession{ true };
    Grid* equatorialgrid{ nullptr };
    Grid* eclipticgrid{ nullptr };
    Ecliptic* ecliptic{ nullptr };
    PrecessionPath* precessionpath{ nullptr };
    PlanetoidGP* sundot{ nullptr };
    DetailedSky(Scene* scene, SceneObject* parent, std::string mode, unsigned int meshU, unsigned int meshV, float radius = 1.0f);
    ~DetailedSky();
    void setTexture(bool tex);
    void addGridEquatorial();
    void addGridEcliptic();
    void addEcliptic();
    void addPrecessionPath();
    void addSun();
    glm::vec3 getSunLocation();
    void addStars(double magnitude = 6.0);
    void addStar(size_t unique, Astronomy::stellarobject& star);
    void addDotDecRA(size_t unique, double dec, double ra, glm::vec4 color, float size, bool rad = false);
    // addDotLatLon() // Ecliptic coordinate
    glm::vec3 getDecRA2Pos3D(LLD decra);
    float getMagnitude2Radius(const double magnitude) const; // in float as it goes to GPU
    bool update();
    LLD toLocalCoords(LLD loc) override;
    LLD fromLocalCoords(LLD loc) override;
    bool drawSpecific(Camera* cam, Shader* shdr);
    void myGUI();
};

// ----------------
//  Detailed Earth
// ----------------
class DetailedEarth : public BodyGeometry {
// DetailedEarth TODO:
// + add twilight and refraction control, done both in shader and GUI
// + perhaps refraction control can be improved, the current model in Earth is a bit ad hoc
// - it would be nice to have twilight that is gradual from 100% to 0% sunlight instead of just the 3 bands

// + add tropics and arctics, with a generic way to tint coordinate regions
// + (note that Earth has GUI editing of the tint colors, very nice to have)
// + (perhaps texture coordinates can be used to make this pixel based rather than triangle based, i.e. move it to the shader)

// + add Moon direction so the fragment shader can also do lunar insolation areas
// + add GenericPath object derived from SceneObject, so it can be used for the various paths (instead of the current mess in Earth)
//   UPD: SmartPath derives from SceneObject and works like GenericPath (using multiple PolyCurveSO)
// - revisit the Earth modifications for adjustable eccentricity (see special shader)
// + add Location and Location Groups, perhaps to BodyGeometry rather than DetailedEarth and DetailedMoon
// - add GUI adjustable texture offsets and implement cropping - This should ideally be done in BodyGeometry
// - Consider how to re-work the shadow casting options. Currently they are difficult to use, and deliver poor results
// - Certainly move the astronomical tracking to Astronomy instead of local member variables
// = Get everything working with recalculating every frame, before implementing a lot of caching options
//   (that should help identify where caching is useful, and the best place/way to implement)
// + SubLunar and SubSolar should be refactored into a general object that can be used for planets etc too
//   UPD: See PlanetoidGP
//   (allow to integrate these with the SubPointSolver objects?) The Solvers also need refactoring!
// - add shadow cones in a generic way if possible
// = add DetailedSky as a celestial sphere?

    glm::vec3 SunLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 MoonLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec4 sunDir = { 0.0f, 0.0f, 0.0f, 1.0f };
    //float m_alpha = 1.0f;

    // Vector of Location groups
public:
    LLD subsolar{ };
    LLD sublunar{ };
    // Would like the following options for insolation: day, hard, twilight, soft, night
    //  - day = only daylight everywhere
    //  - hard = sharp terminator with or without refraction
    //  - twilight = twilight bands
    //  - soft = smooth gradient from light to darkness
    //  - night = night texture everywhere
    bool insolation = true;
    bool w_refract = true;
    bool w_twilight = true;
    bool inlunation = true; // Add lunar refraction? Account for lunar distance
    bool w_mrefract = true;

    PlanetoidGP* sungp = nullptr;
    PlanetoidGP* moongp = nullptr;
    // Arctics and Tropics
    Latitude* arcticcircle = nullptr;
    Latitude* antarcticcircle = nullptr;
    Latitude* tropicofcancer = nullptr;
    Latitude* tropicofcapricorn = nullptr;
    bool tintarctics = false;
    bool tinttropics = false;
    glm::vec4 arcticscolor = glm::vec4(0.0f, 0.198f, 0.198f, 1.0f);
    glm::vec4 tropicscolor = glm::vec4(0.211f, 0.173f, 0.0f, 1.0f);

    Ecliptic* ecliptic = nullptr;
    Grid* celestialgrid = nullptr;

    // Moon
    double m_moonDec{ 0.0 };
    double m_moonHour{ 0.0 };
    double w_mrefang{ deg2radf * ((50.0f - 31.45f) / 60.0f) };  // 31.45 is the average angular diameter of the Moon
    GeneratedPath* moonterminator{ nullptr };
    GeneratedPath::PathGenerator* moontermgen{ nullptr };

    DetailedEarth(Scene* scene, SceneObject* parent, std::string mode, unsigned int meshU, unsigned int meshV, float radius = 1.0f);
    ~DetailedEarth();
    void addSunGP();
    glm::vec3 getSunGPLocation();
    void addMoonGP();
    glm::vec3 getMoonGPLocation();
    LLD calcMoon();
    void addMoonTerminator() {
        moontermgen = new GeneratedPath::PathGenerator{ nullptr, MOONTERMINATOR, 0.003f, GREY,
                                {m_moonDec, m_moonHour, 0.0}, {0.0, 0.0, 0.0},
                                w_refract ? w_mrefang : 0.0, tau, &GeneratedPath::calcTerminator, /*closed*/ true };
        moonterminator = new GeneratedPath{ scene, this, this, moontermgen };
        //std::cout << "addMoonTerminator() completed.\n";
    }
    void addArcticCircles();
    void removeArcticCircles();
    void addTropicCircles();
    void addEcliptic();
    //glm::vec3 getLocEcliptic(LLD pos, bool rad = false);
    bool update();
    LLD toLocalCoords(LLD loc) override;
    LLD fromLocalCoords(LLD loc) override;
    // Specific GPU parameters that are particular to Earth and thus not set up in BodyGeometry
    bool drawSpecific(Camera* cam, Shader* shdr);
    void myGUI();
};


// ---------------
//  Detailed Moon
// ---------------
class DetailedMoon : public BodyGeometry {
public:
    // For GUI
    bool gui_libration = true;
    bool gui_librationlongitude = true;
    bool gui_librationlatitude = true;
    bool gui_positionangle = true;
    bool gui_geocentric = true;     // true = geocentric mode, false = topocentric (i.e. topoLat,topoLon contains observer position)
    float gui_topoLat = 0.0f;
    float gui_topoLon = 0.0f;
    float sunBumpmapScale = 0.0005f;
    float lightBumpmapScale = 0.015f;
    PlanetoidGP* sungp = nullptr;
    PlanetoidGP* earthgp = nullptr;
    ParticleTrail* librationtrail = nullptr;
private:
    float m_camDist = 384400;       // km
    glm::vec3 earthDir{ 0.0f, 0.0f, 0.0f };
    glm::vec4 sunDir{ 0.0f, 0.0f, 0.0f, 1.0f };  // passed to shader, so using vec4 for homogeneous coordinates
    glm::vec3 SunLightDir{ 0.0f, 1.0f, 0.0f };
    glm::vec3 nullpos{ 0.0f, 0.0f, 0.0f }; // 3D position of null island, used for librationtrail
    double topoLat = NO_DOUBLE;            // NO_DOUBLE when Geocentric rather than Topocentric (e.g. to match NASAs yearly Moon videos)
    double topoLon = NO_DOUBLE;            // These are kept as double radians, and the gui_* versions are presented in float degrees
    bool insolation = true;                // Will darken the part of the Moon that is not lit by the Sun
public:
    DetailedMoon(Scene* scene, SceneObject* parent, std::string mode, unsigned int meshU, unsigned int meshV, float radius = 1.0f);
    ~DetailedMoon();
    void setTopocentric(const double lat = NO_DOUBLE, const double lon = NO_DOUBLE, const bool rad = false);
    void addSunGP();
    void addEarthGP();
    void addLibrationTrail();
    void updateLibrationTrail();
    bool update();
    LLD toLocalCoords(LLD loc) override;
    LLD fromLocalCoords(LLD loc) override;
    bool drawSpecific(Camera* cam, Shader* shdr);
    void myGUI();
};

// --------------
//  EarthMoonSun
// --------------
class EarthMoonSun : public SceneObject {
    // Add ability to show geocentric and heliocentric
public:
    EarthMoonSun() = delete;
    EarthMoonSun(Scene* scene, SceneObject* parent) : SceneObject(scene, parent) {
        // Create Planetoids
        earth = new Planetoid(scene, this, A_EARTH, 40, 20, 0.04f);
        moon = new Planetoid(scene, this, A_MOON, 40, 20, 0.02f);
        sun = new Planetoid(scene, this, A_SUN, 40, 20, 0.08f);
        update();
    }
    void addEclipticPlane() {
        // Use AEarth::EclipticPoleVondrak() for orientation, center on Sun
        // Actually return the plane, in case user wants to modify
    }
    void addEquatorialPlane() {
        // Use AEarth::EquatorialPoleVondrak() for orientation, center on Earth
        // Actually return the plane, in case user wants to modify
    }
    void addMoonOrbitalPlane() {
        // Maybe use coordinates in orbit path...APhysicalMoonDetails AMoon::CalculateGeocentric(double jd_tt) doesn't seem helpful.
        // Actually return the plane, in case user wants to modify
    }
    void addEarthAxis() {
        // AEarth::EquatorialPoleVondrak()
    }
    void addMoonAxis() {
        // Not sure...
    }
    void addEarthTrail() {
        // Will follow orbit,but may show the variations in velocity
    }
    void addMoonTrail() {
        // Will show the added effects of Earth's and Moon's orbits
    }
    bool update() {
        // X axis is Equinox of Date
        double jd_tt = scene->astro->getJD_TT();
        // True Heliocentric Rectangular at Equinox of Date - VSOP87 Ephemeris C
        earthpos = glm::vec3((float)AEarth::VSOP87C_X(jd_tt), (float)AEarth::VSOP87C_Y(jd_tt), (float)AEarth::VSOP87C_Z(jd_tt));
        earth->setPosition(earthpos);
        // Earth orientation - Axis of Rotation
        //  Use AEarth::EquatorialPoleVondrak()
        glm::vec3 equatorialpole = (glm::vec3)AEarth::EquatorialPoleVondrak(jd_tt);  // !!! FIX: Does not work as I expected!
        equatorialpole.x = -equatorialpole.x;
        equatorialpole.y = -equatorialpole.y;
        VPRINT(equatorialpole) << "\n";
        equatorialpole *= 0.15f;
        glm::vec3 earthaxis1 = earthpos + equatorialpole;
        glm::vec3 earthaxis2 = earthpos - equatorialpole;
        if (earthaxis == NO_UINT) earthaxis = scene->getCylindersFactory()->addStartEnd(earthaxis1, earthaxis2, 0.003f, WHITE);
        else scene->getCylindersFactory()->changeStartEnd(earthaxis, earthaxis1, earthaxis2, 0.003f, WHITE);
        // Earth orientation - Rotation Angle
        //  Use AEarth::ApparentGreenwichSiderealTime()
        moonpos = (glm::vec3)AELP2000::EclipticRectangularCoordinates(jd_tt);
        moonpos *= (float)km2au;
        moonpos *= 100.0f;
        moonpos += earthpos;
        moon->setPosition(moonpos);
        // Moon orientation - use APhysicalMoonDetails AMoon::CalculateGeocentric(double jd_tt)
        //  - Rotate to point l,b towards Earth, then rotate to align axis
        //sun->setPosition(sunpos);  // Sun is currently fixed at origin
        // Sun orientation - ignore for now as Sun doesn't rotate as a solid body?
        return false;
    }
    void draw(Camera* cam) { return; };
    
public:

private:
    glm::vec3 earthpos{ 0.0f, 0.0f, 1.0f };
    glm::vec3 moonpos{ 0.0f, 1.0f, 1.0f };
    glm::vec3 sunpos{ 0.0f, 0.0f, 0.0f };
    Planetoid* earth{ nullptr };
    Planetoid* moon{ nullptr };
    Planetoid* sun{ nullptr };
    size_t earthaxis{ NO_UINT };
};
// --------------
//  Solar System
// --------------
#define planetoid_size 0.1f
#define planetoid_U 90  // high, but needed for rings not to look shitty.
#define planetoid_V 45
#define orbit_width 0.01f
#define orbit_steps 100
class SolarSystem : public SceneObject {
    // Uses geometric positions, no parallax / aberration / lightspeed / etc
    struct planetinfo {
        double sidyear = 0.0; // sidereal year of planet in Earth days
        glm::vec4 color = NO_COLOR;
    };
    std::array<planetinfo, 9> m_planetinfos = { {  // sidereal years are a bit long to allow orbits to close
        { 88,    MERCURYCOLOR }, // Mercury
        { 226,   VENUSCOLOR },   // Venus
        { 368,   EARTHCOLOR },   // Earth
        { 688,   MARSCOLOR },    // Mars
        { 4336,  JUPITERCOLOR }, // Jupiter
        { 10776, SATURNCOLOR },  // Saturn
        { 30688, URANUSCOLOR },  // Uranus
        { 60182, NEPTUNECOLOR }, // Neptune
        { 368,   SUNCOLOR }      // Sun
    } };

public:
    SolarSystem() = delete;
    SolarSystem(Scene* scene, SceneObject* parent) : SceneObject(scene, parent) {
        name = "Solar System";
        hasgui = true;

        // create planets
        for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
            // Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Sun
            planetoids.push_back(new Planetoid(scene, this, (Planet)p, planetoid_U, planetoid_V, planetoid_size));
            planetpositions.emplace_back(0.0, 0.0, 0.0);
            planetorbits.push_back(new SmartPath(scene, this, orbit_width));
            planetorbits.back()->setColor(m_planetinfos[p].color);
        }

        // update positions etc to current moment
        update();

        //test = new Planetoid(scene, parent, (Planet)A_SATURN, planetoid_U, planetoid_V, planetoid_size);
    }
    ~SolarSystem() {

    }
    void clearTrails() {}
    bool update() override {
        updPlanets();
        return false; }
    void draw(Camera* cam) override { return; }
    void myGUI() {
        // Solar System object
        if (ImGui::CollapsingHeader("Solar System")) {
            ImGui::Checkbox("Geocentric", &geocentric);
            ImGui::Checkbox("Orbits", &orbits);
            ImGui::Checkbox("Trails", &trails);
            ImGui::SameLine();
            if (ImGui::Button("Clear")) { clearTrails(); }
            ImGui::SliderInt("Trail length", &traillen, 2, 1000);
        }
    }
private:
    void updPlanets() {
        LLD coord{};
        glm::vec3 coord_rect{ 0.0f };
        double jd_tt = scene->astro->getJD_TT();
        double start, stop, step;
        for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
            // Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Sun
            coord = scene->astro->EclipticCoordinates(jd_tt, (Planet)p, EPH_VSOP87_SHORT);
            planetoids[p]->position = Spherical::Spherical2Rectangular(coord);
            start = scene->astro->getJD_TT() - m_planetinfos[p].sidyear * 0.5;
            stop = scene->astro->getJD_TT() + m_planetinfos[p].sidyear * 0.5;
            step = (m_planetinfos[p].sidyear / orbit_steps);
            // clear unconditionally, to get rid of orbits when being disabled.
            //  Maybe there is no need to recalculate the orbits all the time, simply use hidden boolean on SmartPath instead
            planetorbits[p]->clearPoints();
            if (orbits) {
                for (double jd_tt = start; jd_tt <= stop; jd_tt += step) {
                    coord = scene->astro->EclipticCoordinates(jd_tt, (Planet)p, EPH_VSOP87_SHORT);
                    coord_rect = (glm::vec3)Spherical::Spherical2Rectangular(coord);
                    planetorbits[p]->addPoint(coord_rect);
                }
            }
            //double siderealyear = m_planetinfos[p].sidyear;
            //glm::vec4 color = m_planetinfos[p].color;
            //color.a = 0.4f;
            //planetorbits[p] = scene->astro->getCelestialPath(planet, -0.5 * siderealyear, 0.5 * siderealyear, orbitsteps, EC);
        }
    }
public:
    // For GUI
    bool geocentric = false;
    bool orbits = false;
    bool trails = false;
    int traillen = 400;
    int orbitsteps = 100;

private:
    std::vector<Planetoid*> planetoids;
    std::vector<glm::vec3> planetpositions;
    std::vector<SmartPath*> planetorbits;

};

// ------------------
//  Elliptical Orbit
// ------------------
#define eorbit_width 0.01f
#define eorbit_steps 360

class EllipticalOrbit : public SceneObject {
    // Add name billboard
    // Add GUI to play with the orbit parameters?
public:
    EllipticalOrbit() = delete;
    EllipticalOrbit(Scene* scene, SceneObject* parent, AEllipticalObjectElements& orbel, std::string& name) : SceneObject(scene, parent) {
        this->name = name;
        hasgui = false;
        updateOrbit(orbel, name);
        //orbit = new AEllipticalOrbit{ orbel };
        //orbitpath = new SmartPath(scene, this, eorbit_width, NO_COLOR);
    }
    ~EllipticalOrbit() {
        //if (orbitpath) delete orbitpath;
        //orbitpath = nullptr;
        //if (orbit) delete orbit;
        //orbit = nullptr;
    }
    void updateOrbit(AEllipticalObjectElements& orbel, std::string& name) {
        // These cannot be empty, but maybe through future changes...
        if (orbit) delete orbit;
        if (orbitpath) delete orbitpath;
        orbit = new AEllipticalOrbit{ orbel };
        orbitpath = new SmartPath(scene, this, eorbit_width, NO_COLOR);
        double P = tau / orbit->n;   // Orbital period in days
        orbitpath->clearPoints();
        for (double E = 0; E <= tau; E += tau / eorbit_steps) {
            // Use equally spaced Eccentric Anomalies to directly plot the orbit
            // Note: This does not correspond to physical reality, but it allows to plot the orbit at more evenly spaced points.
            // This avoids widely spaced points at perihelion (where the object moves very fast)
            // and very dense points at aphelion (where it moves slowly)
            orbitpath->addPoint(orbit->UniformHeliocentricRectangular(E));
        }
    }
    bool update() override {
        orbitpath->hidden = hidden;
        // No need to update orbit every frame, it only changes via updateOrbit()
        return false;
    }
    void draw(Camera* cam) override { return; }
private:
    AEllipticalOrbit* orbit{ nullptr };
    SmartPath* orbitpath{ nullptr };
};


// --------------
//  Solar System
// --------------
#define planetoid_size 0.1f
#define planetoid_U 90  // high, but needed for rings not to look shitty.
#define planetoid_V 45
#define orbit_width 0.01f
#define orbit_steps 100
#define comets_reserve 1000
class SolarSystem : public SceneObject {
    // Uses geometric positions, no parallax / aberration / lightspeed / etc
    struct planetinfo {
        double sidyear = 0.0; // sidereal year of planet in Earth days
        glm::vec4 color = NO_COLOR;
    };
    std::array<planetinfo, 9> m_planetinfos = { {  // sidereal years are a bit long to allow orbits to close
        { 88,    MERCURYCOLOR }, // Mercury
        { 226,   VENUSCOLOR },   // Venus
        { 368,   EARTHCOLOR },   // Earth
        { 688,   MARSCOLOR },    // Mars
        { 4336,  JUPITERCOLOR }, // Jupiter
        { 10776, SATURNCOLOR },  // Saturn
        { 30688, URANUSCOLOR },  // Uranus
        { 60182, NEPTUNECOLOR }, // Neptune
        { 368,   SUNCOLOR }      // Sun
    } };

public:
    SolarSystem() = delete;
    SolarSystem(Scene* scene, SceneObject* parent) : SceneObject(scene, parent) {
        name = "Solar System";
        hasgui = true;
        planetorbits.reserve(A_SUN + 1);
        cometorbits.reserve(comets_reserve);
        // create planets
        for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
            // Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Sun
            planetoids.push_back(new Planetoid(scene, this, (Planet)p, planetoid_U, planetoid_V, planetoid_size));
            planetpositions.emplace_back(0.0, 0.0, 0.0);
            planetorbits.push_back(new SmartPath(scene, this, orbit_width));
            planetorbits.back()->setColor(m_planetinfos[p].color);
        }

        // update positions etc to current moment
        update();

        //test = new Planetoid(scene, parent, (Planet)A_SATURN, planetoid_U, planetoid_V, planetoid_size);
    }
    ~SolarSystem() {

    }
    void addComets() {
        std::vector<std::string> comet_names{ "1P/Halley", "2P/Encke", "3D-A/Biela", "3D-B/Biela", "5D/Brorsen", "12P/Pons-Brooks", "15P/Finlay", "23P/Brorsen-Metcalf" };
        AEllipticalObjectElements orbel{};
        for (auto& name : comet_names) {
            orbel = scene->astro->getCometOrbitByName(name);
            //std::cout << name << ": " << &orbel << "\n";
            //orbel.print();
            cometorbits.push_back({ scene, this, orbel, name });
            
        }
    }
    void clearTrails() {}
    bool update() override {
        updPlanets();
        updComets();
        return false; }
    void draw(Camera* cam) override { return; }
    void myGUI() {
        // Solar System object
        if (ImGui::CollapsingHeader("Solar System")) {
            ImGui::Checkbox("Geocentric", &geocentric);
            ImGui::Checkbox("Orbits", &show_planet_orbits);
            ImGui::Checkbox("Comet Orbits", &show_comet_orbits);
            //ImGui::Checkbox("Trails", &trails);
            //ImGui::SameLine();
            //if (ImGui::Button("Clear")) { clearTrails(); }
            //ImGui::SliderInt("Trail length", &traillen, 2, 1000);
        }
    }
private:
    void updPlanets() {
        LLD coord{};
        glm::vec3 coord_rect{ 0.0f };
        double jd_tt = scene->astro->getJD_TT();
        double start, stop, step;
        for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
            // Mercury, Venus, Earth, Mars, Jupiter, Saturn, Uranus, Neptune, Sun
            coord = scene->astro->EclipticCoordinates(jd_tt, (Planet)p, EPH_VSOP87_SHORT);
            planetoids[p]->position = Spherical::Spherical2Rectangular(coord);
            start = scene->astro->getJD_TT() - m_planetinfos[p].sidyear * 0.5;
            stop = scene->astro->getJD_TT() + m_planetinfos[p].sidyear * 0.5;
            step = (m_planetinfos[p].sidyear / orbit_steps);
            // clear unconditionally, to get rid of orbits when being disabled.
            //  Maybe there is no need to recalculate the orbits all the time, simply use hidden boolean on SmartPath instead
            planetorbits[p]->clearPoints();
            if (show_planet_orbits) {
                for (double jd_tt = start; jd_tt <= stop; jd_tt += step) {
                    coord = scene->astro->EclipticCoordinates(jd_tt, (Planet)p, EPH_VSOP87_SHORT);
                    coord_rect = (glm::vec3)Spherical::Spherical2Rectangular(coord);
                    planetorbits[p]->addPoint(coord_rect);
                }
            }
            // Astronomy cached planet paths
            //double siderealyear = m_planetinfos[p].sidyear;
            //glm::vec4 color = m_planetinfos[p].color;
            //color.a = 0.4f;
            //planetorbits[p] = scene->astro->getCelestialPath(planet, -0.5 * siderealyear, 0.5 * siderealyear, orbitsteps, EC);
        }
    }
    void updComets() {
        for (auto& comet : cometorbits) {
            comet.hidden = !show_comet_orbits;
        }
    }
public:
    // For GUI
    bool geocentric = false;
    bool show_planet_orbits = false;
    bool show_comet_orbits = false;
    //bool trails = false;
    //int traillen = 400;
    int orbitsteps = orbit_steps;

private:
    std::vector<Planetoid*> planetoids;
    std::vector<glm::vec3> planetpositions;
    std::vector<SmartPath*> planetorbits;
    std::vector<EllipticalOrbit> cometorbits;
};

// --------------------
//  Body Rotation Axis
// --------------------
class RotationAxis : public SceneObject {
    // Get rotation axis from parent Planetoid or DetailedX body?
    RotationAxis() = delete;
    RotationAxis(Scene* scene, SceneObject* parent) : SceneObject(scene, parent) {
        name = "Rotation Axis";
        // Check if parent has required parameters
        // Create arrow primitive
        arrow_factory = scene->getArrowsFactory();
        float half_len = 1.2f * parent->scale.z;
        glm::vec3 my_start = { parent->position.x, parent->position.y, parent->position.z - half_len };
        glm::vec3 my_end = { parent->position.x, parent->position.y, parent->position.z + half_len };
        //arrow = arrow_factory->addStartEnd
    }
    ~RotationAxis() {
        // destroy arrow primitive
        if (NO_UINT != arrow) arrow_factory->removeArrow(arrow);
    }

    bool update() override { return false; }
    void draw(Camera* cam) override {}  // using arrow primitive, it draws itself.
private:
    // Arrow primitive
    Arrows* arrow_factory{ nullptr };
    size_t arrow{ NO_UINT };
};


// -----------
//  SimpleArc
// -----------
class SimpleArc : public SceneObject {
public:
    SimpleArc(Scene* scene, SceneObject* parent, LLD p1, LLD p2)
    : SceneObject(scene, parent), p1(p1), p2(p2) {
        name = "Arc";
        path = new SmartPath(scene, this);
        generate();
    }
    ~SimpleArc() {
        if (path) delete path;
    }
    void setStartEnd(LLD p1, LLD p2) {
        this->p1 = p1;
        this->p2 = p2;
        generate();
    }
    void generate() {
        // Build arc path here
        path->clearPoints();
        double fstep = 1.0 / (rad2deg * calcArcDist(p1, p2));
        for (double f = 0.0; f <= 1.0; f += fstep) {
            path->addPoint(calcGreatArc(p1, p2, f));
        }
        path->addPoint(calcGreatArc(p1, p2, 1.0));  // Make sure we reach the "far" end point exactly.
    }
    double calcArcDist(LLD lld1, LLD lld2) {
        double sin1 = sin(lld1.lat);
        double sin2 = sin(lld2.lat);
        double cos1 = cos(lld1.lat);
        double cos2 = cos(lld2.lat);
        double dlon = lld2.lon - lld1.lon;
        double sind = sin(dlon);
        double cosd = cos(dlon);
        double a = sqrt(pow((cos2 * sind), 2) + pow((cos1 * sin2 - sin1 * cos2 * cosd), 2));
        double b = sin1 * sin2 + cos1 * cos2 * cosd;
        double dist = atan2(a, b);
        return dist;  // Arc distance in radians
    }
    glm::vec3 calcGreatArc(LLD lld1, LLD lld2, double f) {
        // calculate point on great circle arc between lld1 and lld2
        // f is "lerp param" from 0.0 to 1.0
        // LLD.dst is ignored and assumed to be 1.0 (i.e. on a unit sphere)
        glm::vec3 ret;
        double d = acos(sin(lld1.lat) * sin(lld2.lat) + cos(lld1.lat) * cos(lld2.lat) * cos(lld1.lon - lld2.lon));
        double A = sin((1 - f) * d) / sin(d);
        double B = sin(f * d) / sin(d);
        ret.x = (float)(A * cos(lld1.lat) * cos(lld1.lon) + B * cos(lld2.lat) * cos(lld2.lon));
        ret.y = (float)(A * cos(lld1.lat) * sin(lld1.lon) + B * cos(lld2.lat) * sin(lld2.lon));
        ret.z = (float)(A * sin(lld1.lat) + B * sin(lld2.lat));
        return ret;
    }
    void draw(Camera* cam) override {}
    bool update() override { return false; }

public:
    SmartPath* path{ nullptr };
    LLD p1;
    LLD p2;
};

// ------------------
//  Simple Longitude
// ------------------
class SimpleLongitude : public SceneObject {
public:
    // Get rotation axis from parent Planetoid or DetailedX body?
    SimpleLongitude() = delete;
    SimpleLongitude(Scene* scene, SceneObject* parent, double longitude) : SceneObject(scene, parent) {
        name = "Lon";
        lon = longitude;
        path = new SmartPath(scene, this);
        generate();
    }
    ~SimpleLongitude() {
        if (path) delete path;
    }
    void setWidthColor(float width, glm::vec4 color) {
        path->setWidth(width);
        path->setColor(color);
    }
    void generate() {
        glm::vec3 pos{ 0.0f };
        path->clearPoints();
        LLD latlon{};
        latlon.lon = lon;
        latlon.dst = 1.0;
        for (double lat = -pi2; lat <= pi2 + verytiny; lat += deg2rad) {
            latlon.lat = lat;
            pos = Spherical::Spherical2Rectangular(latlon);
            path->addPoint((glm::vec3)pos);
        }
        // added verytiny in loop above to (hopefully) avoid small gaps at the pole, so this is not needed now:
        //path->addPoint((glm::vec3)Spherical::Spherical2Rectangular({ pi2, lon, 1.0 }));
    }
    bool update() override { return false; }
    void draw(Camera* cam) override {}
public:
    SmartPath* path{ nullptr };
    double lon{ 0.0 };
};

// ----------------
//  SimpleLatitude
// ----------------
class SimpleLatitude : public SceneObject {
public:
    SimpleLatitude(Scene* scene, SceneObject* parent, double latitude) : SceneObject(scene, parent) {
        name = "Lat";
        lat = latitude;
        path = new SmartPath(scene, this);
        generate();
    }
    void setWidthColor(float width, glm::vec4 color) {
        path->setWidth(width);
        path->setColor(color);
    }
    void generate() {
        glm::vec3 pos{ 0.0f };
        path->clearPoints();
        LLD latlon{};
        latlon.lat = lat;
        latlon.dst = 1.0;
        for (double lon = -pi; lon <= pi; lon += deg2rad) {
            latlon.lon = lon;
            pos = Spherical::Spherical2Rectangular(latlon) ;
            path->addPoint((glm::vec3)pos);
        }
    }
    ~SimpleLatitude() {
        if (path) delete path;
    }
    void draw(Camera* cam) override {}
    bool update() override { return false; } // Allow world matrix inheritance
public:
    SmartPath* path{ nullptr };
    double lat;
};

// ----------------
//  SimpleUVSphere
// ----------------
class SimpleUVSphere : public SceneObject {
public:
    SimpleUVSphere(Scene* scene, SceneObject* parent, float radius, glm::vec4 color) : SceneObject(scene, parent), r(radius) {
        name = "UVSphere";
        this->color = color;
        sphereuv = scene->getSphereUVFactory()->addStartEnd({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, r }, r, color);
        //  generate();
    }
    void draw(Camera* cam) override {
        //  Using SphereUV primitive, so nothing to draw here
    }
    bool update() override {
        if (m_parent != nullptr) worldmatrix = m_parent->getWorldMat();
        else worldmatrix = glm::mat4(1.0f);
        worldmatrix = glm::scale(worldmatrix, scale);
        if (rotationorder == XYZ) {
            worldmatrix = glm::rotate(worldmatrix, rotations.x, glm::vec3(1.0f, 0.0f, 0.0f));
            worldmatrix = glm::rotate(worldmatrix, rotations.y, glm::vec3(0.0f, 1.0f, 0.0f));
            worldmatrix = glm::rotate(worldmatrix, rotations.z, glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (rotationorder == ZYX) {
            worldmatrix = glm::rotate(worldmatrix, rotations.z, glm::vec3(0.0f, 0.0f, 1.0f));
            worldmatrix = glm::rotate(worldmatrix, rotations.y, glm::vec3(0.0f, 1.0f, 0.0f));
            worldmatrix = glm::rotate(worldmatrix, rotations.x, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        worldmatrix = glm::translate(worldmatrix, position);
        glm::vec3 pos = worldmatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        glm::vec3 dir = worldmatrix * glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

        scene->getSphereUVFactory()->changeStartEnd(sphereuv, pos, dir, r, NO_COLOR);
        return true;  //  worldmatrix has been built already so don't inherit
    }
private:
    size_t sphereuv{ NO_UINT };
    float r{ 1.0f };  // radius
};

// ----------------------
//  SphericalCoordinates
// ----------------------
class SphericalCoordinates : public SceneObject {
public:
    SphericalCoordinates(Scene* scene, SceneObject* parent, unsigned int divisions)
        : SceneObject(scene, parent), divs(divisions) {
        name = "Sphere_Coord";
        sphere = new SimpleUVSphere(scene, this, 0.5f, { 0.0f, 0.0f, 0.0f, 0.4f });
        generate();
    }
    ~SphericalCoordinates() {
        for (auto lat : latitudes) { delete lat; };
        for (auto lon : longitudes) { delete lon; };
    }
    void setWidthColor(float width, glm::vec4 color) {
        for (auto lat : latitudes) {
            lat->setWidthColor(width, color);
        }
        for (auto lon : longitudes) {
            lon->setWidthColor(width, color);
        }
    }
    void generate() {
        // create latitudes and longitudes
        double latstep = tau / (double)divs;
        for (double lati = -pi2; lati < pi2; lati += latstep) {
            latitudes.emplace_back(new SimpleLatitude(scene, this, lati));
        }
        double lonstep = tau / (double)divs;
        for (double lngi = -pi; lngi < pi; lngi += lonstep) {
            longitudes.emplace_back(new SimpleLongitude(scene, this, lngi));
        }
    }
    void draw(Camera* cam) override {}
    bool update() override {
        return false;
    }
public:
    unsigned int divs{ 24 };
    SimpleUVSphere* sphere{ nullptr };
    std::vector<SimpleLatitude*> latitudes;
    std::vector<SimpleLongitude*> longitudes;
};

// -----------------------
//  EquatorialCoordinates
// -----------------------
class EquatorialCoordinates : public SphericalCoordinates {
public:
    EquatorialCoordinates(Scene* scene, SceneObject* parent, unsigned int divisions)
    : SphericalCoordinates(scene, parent, divisions) {
        name = "EQ_Coord";
    }
    bool update() override {
        return false;
    }
};

// ---------------------
//  EclipticCoordinates
// ---------------------
class EclipticCoordinates : public SphericalCoordinates {
public:
    EclipticCoordinates(Scene* scene, SceneObject* parent, unsigned int divisions)
        : SphericalCoordinates(scene, parent, divisions) {
        name = "EC_Coord";
        rotations.x = deg2radf * 23.5f;
    }
    bool update() override {
        rotations.x = (float)scene->astro->TrueObliquityOfEcliptic(scene->astro->getJD_TT(), true);
        return false;
    }
};

// -----------
//  SightLine
// -----------
class SightLine : public SceneObject {
public:
    SightLine(Scene* scene, SceneObject* parent, glm::vec3 p1, glm::vec3 p2, float width = 0.003f, glm::vec4 color = LIGHT_YELLOW)
    : SceneObject(scene, parent) {
        cylinder = scene->getCylindersFactory()->addStartEnd(p1, p2, width, color);
        this->width = width;
        this->color = color;
    }
    ~SightLine() {
        scene->getCylindersFactory()->removeCylinder(cylinder);
    }
    void setStartEnd(glm::vec3 p1, glm::vec3 p2) {
        scene->getCylindersFactory()->changeStartEnd(cylinder, p1, p2, width, color);
    }
    bool update() override { return true; };
    void draw(Camera* cam) override {};
private:
    glm::vec3 p1{ 0.0f };
    glm::vec3 p2{ 0.0f };
    float width;
    glm::vec4 color;
    size_t cylinder{ NO_UINT };
};

// ----------
//  EarthSun
// ----------
class EarthSun : public SceneObject {
    // Natural orientation is Equatorial Celestial coordinates
public:
    EarthSun() = delete;
    EarthSun(Scene* scene, SceneObject* parent) : SceneObject(scene, parent) {
        name = "EarthSun";
        // Create Planetoids
        earth = new Planetoid(scene, this, A_EARTH, 40, 20, 0.08f);
        earth->name = "Earth";
        sun = new Planetoid(scene, this, A_SUN, 40, 20, 0.16f);
        sun->name = "Sun";
        update();
    }
    // Will these be handy to have?
    void addEarthSunLine() {}
    void addEarthAxis() {}
    void addSunAxis() {}

    bool update() {
        // X axis is Equinox of Date, Z towards celestial north pole (Y to form right handed system)
        double jd_tt = scene->astro->getJD_TT();
        // Earth position
        earthloc = AEarth::EclipticCoordinates(jd_tt, EPH_VSOP87_FULL);
        earthloc = Spherical::Ecliptic2Equatorial(earthloc, AEarth::TrueObliquityOfEcliptic(jd_tt));
        earthpos = Spherical::Spherical2Rectangular(earthloc);
        earth->setPosition(earthpos);  // equatorial coordinates
        // Earth orientation - Axis of Rotation
        glm::vec3 equatorialpole = (glm::vec3)AEarth::EquatorialPoleVondrak(jd_tt);
        equatorialpole.x = -equatorialpole.x;
        equatorialpole.y = -equatorialpole.y;
        //VPRINT(equatorialpole) << "\n";
        equatorialpole *= 0.15f;
        glm::vec3 earthaxis1 = earthpos + equatorialpole;
        glm::vec3 earthaxis2 = earthpos - equatorialpole;
        if (earthaxis == NO_UINT) earthaxis = scene->getCylindersFactory()->addStartEnd(earthaxis1, earthaxis2, 0.003f, WHITE);
        else scene->getCylindersFactory()->changeStartEnd(earthaxis, earthaxis1, earthaxis2, 0.003f, WHITE);

        // Sun orientation - Axis of Rotation
        LLD s_pole{ deg2rad * 63.87, deg2rad * 286.13 , 1.0 };  // Solar North Pole
        glm::vec3 sunpole = (glm::vec3)Spherical::Spherical2Rectangular(s_pole);
        //sunpole.x = -sunpole.x;
        //sunpole.y = -sunpole.y;
        //VPRINT(equatorialpole) << "\n";
        sunpole *= 0.25f;
        sunaxis1 = sunpos + sunpole;
        glm::vec3 sunaxis2 = sunpos - sunpole;
        if (sunaxis == NO_UINT) sunaxis = scene->getCylindersFactory()->addStartEnd(sunaxis1, sunaxis2, 0.003f, WHITE);
        else scene->getCylindersFactory()->changeStartEnd(sunaxis, sunaxis1, sunaxis2, 0.003f, WHITE);
        return false;
    }
    void draw(Camera* cam) { return; };
public:
    glm::vec3 earthpos{ 0.0f, 0.0f, 1.0f };
    glm::vec3 sunpos{ 0.0f, 0.0f, 0.0f };
    glm::vec3 sunaxis1{ 0.0f };
    LLD earthloc{ 0.0, 0.0, 0.0 };
    Planetoid* earth{ nullptr };
    Planetoid* sun{ nullptr };
private:
    // Primitives
    size_t earthaxis{ NO_UINT };
    size_t sunaxis{ NO_UINT };
};


// --------------------------
//  Bert's Foucault Pendulum
// --------------------------
#include "glm/gtx/norm.hpp"
// Source: https://github.com/qbertGloberCoder/glober-apps/blob/main/foucault_pendulum/src/main/java/me/qbert/foucault/
// YouTube: https://www.youtube.com/watch?v=FYBPmzw84CY
class FoucaultPendulumBert {
public:
    class Location {
    public:
        double latitude;
        Location() = delete;
        Location(double latitude) {
            this->latitude = latitude;
        }
        double getLatitude() {
            return latitude;
        }
        void setLatitude(double latitude) {
            this->latitude = latitude;
        }
    };
    class PrecessionRate {
    public:
        PrecessionRate() = delete;
        PrecessionRate(Location location) {
            this->location = location;
            omega = 0.0;
        }
        PrecessionRate(Location location, double omega) {
            this->location = location;
            this->omega = omega;
            reinit();
        }
        void setOmega(double omega) {
            this->omega = omega;
            reinit();
        }
        double getOmega() {
            return omega;
        }
        void reinit() {
            omegaP.x = 0.0;
            omegaP.y = omega * cos(location.getLatitude());
            omegaP.z = omega * sin(location.getLatitude());
        }
        bool isPrecessionActive() {
            return precessionActive;
        }
        void setPrecessionActive(bool precessionActive) {
            this->precessionActive = precessionActive;
        }
        double getOmegaX() {
            if (!precessionActive) return 0.0;
            return omegaP.x;
        }
        double getOmegaY() {
            if (!precessionActive) return 0.0;
            return omegaP.y;
        }
        double getOmegaZ() {
            if (!precessionActive) return 0.0;
            return omegaP.z;
        }
    private:
        Location location{ 0.0 };
        double omega;
        glm::dvec3 omegaP{ 0.0 };
        bool precessionActive = true;
    };

    //  Methods

    void setLatitude(double latitude) {
        location.setLatitude(latitude);
        precessionRate.reinit();
        runMode = false;
    }
    void setPrecessionRate(double secondsPerRotation) {
        //statistics.setSiderealTime(secondsPerRotation / 3600.0);
        precessionRate.setOmega(tau / secondsPerRotation);
        runMode = false;
    }
    double getPrecessionRate() {
        return tau / precessionRate.getOmega();
    }
    bool isPrecessionActive() {
        return precessionRate.isPrecessionActive();
    }
    void setPrecessionActive(bool precessionActive) {
        precessionRate.setPrecessionActive(precessionActive);
    }
    void setPendulumLength(double pendulumLength) {
        this->pendulumLength = pendulumLength;
        runMode = false;
        resetPendulum();
    }
    void setStartPosition(double apexRadius, double startAzimuth) {
        this->desiredApexRadius = apexRadius;
        desiredStartAngle = ACoord::rangezero2threesixty(450.0 - startAzimuth);  // DEGREES
        returnAngle = desiredStartAngle;
        runMode = false;
        resetPendulum();
    }
    bool isRunMode() {
        return runMode;
    }
    void setRunMode(bool runMode) {
        this->runMode = runMode;
    }
    void setGravity(double metersPerSecondSquared) {
        this->metersPerSecondSquared = metersPerSecondSquared;
        g = { 0.0, 0.0, -metersPerSecondSquared };
    }
    double getGravity() {
        return metersPerSecondSquared;
    }
    bool isStableSwing() {
        return stableSwing;
    }
    void setStableSwing(bool stableSwing) {
        this->stableSwing = stableSwing;
    }
    bool isApplyDrag() {
        return applyDrag;
    }
    void setApplyDrag(bool applyDrag) {
        this->applyDrag = applyDrag;
    }
    double getDragCoefficent() {
        return dragCoefficent;
    }
    void setDragCoefficent(double dragCoefficent) {
        if ((dragCoefficent > 0.25) && (dragCoefficent < 1.0)) this->dragCoefficent = dragCoefficent;
        else applyDrag = false;
    }
    double getMinTimeStep() {
        return minTimeStep;
    }
    void setMinTimeStep(double minTimeStep) {
        this->minTimeStep = minTimeStep;
    }
    double getMidTimeStep() {
        return midTimeStep;
    }
    void setMidTimeStep(double midTimeStep) {
        this->midTimeStep = midTimeStep;
    }
    double getMaxTimeStep() {
        return maxTimeStep;
    }
    void setMaxTimeStep(double maxTimeStep) {
        this->maxTimeStep = maxTimeStep;
    }
private:
    void resetPendulum() {
        desiredStartAngleRads = deg2rad * desiredStartAngle;
        returnAngle = desiredStartAngleRads;

        apexRadius = desiredApexRadius;
        animationTime = 0;
        lastDragTime = -1;

        returnApex = true;
        currentRadius = desiredStartAngleRads;
        lastForwardApexTime = -1;
        lastReturnApexTime = -1;
        lastForwardApexAngle = desiredStartAngleRads;
        lastReturnApexAngle = pi + desiredStartAngleRads;
        lastNadirRadius = 0.0;
        lastNadirAngle = desiredStartAngleRads;

        //x = desiredApexRadius * cos(deg2rad * desiredStartAngle);
        //y = desiredApexRadius * sin(deg2rad * desiredStartAngle);
        //z = -sqrt(pendulumLength * pendulumLength - x * x - y * y);
        P = {
            desiredApexRadius * cos(deg2rad * desiredStartAngle),
            desiredApexRadius * sin(deg2rad * desiredStartAngle),
            -sqrt(pendulumLength * pendulumLength - P.x * P.x - P.y * P.y) };
        v = { 0.0, 0.0, 0.0 };
    }
    void computeTimeStep() {
        // MOST positions in the swing can get away with maxTimeStep integration intervals...
        // To provide a more robust set of statistics and avoid sampling errors, as the
        // pendulum approaches the nadir and the apex, switch to smaller time intervals.
        if (abs(currentRadius) < 0.005) {
            timeStep = minTimeStep;
        }
        else if ((currentRadius <= desiredApexRadius) && ((currentRadius / desiredApexRadius) > 0.99)) {
            if ((currentRadius / desiredApexRadius) > 0.999) {
                timeStep = minTimeStep;
            }
            else {
                timeStep = midTimeStep;
            }
        }
        else {
            timeStep = maxTimeStep;
        }
    }
public:
    void setTargetFrameRate(double targetFrameRate) {
        this->targetFrameTime = 1.0 / targetFrameRate;
    }
    void stepOnce() {
        computeTimeStep();
        stepToTime(animationTime + (timeStep / 2));
    }
    void stepToNextFrame() {
        frameCount = (int)(animationTime / targetFrameTime);
        stepToTime((double)(frameCount + 1) * targetFrameTime);
    }
    // Main calculation
    void stepToTime(double time) {
        if (!runMode) return;
        if (timeStep <= 0.0) return;

        double omegaX = precessionRate.getOmegaX();
        double omegaY = precessionRate.getOmegaY();
        double omegaZ = precessionRate.getOmegaZ();
        bool precessionActive = precessionRate.isPrecessionActive();

        while ((apexRadius > 0.0001) && (animationTime < time)) {
            /* compute the unit vector along the string */    // MDO: Not a unit vector
            double realLengthSquared = glm::length2(P);
            glm::dvec3 a{ 0.0 };

            /* compute acceleration based on gravity and coriolis */
            if (precessionActive) {
                a.x = g.x - 2.0 * (omegaY * v.z - omegaZ * v.y);
                a.y = g.y - 2.0 * (omegaZ * v.x - omegaX * v.z);
                a.z = g.z - 2.0 * (omegaX * v.y - omegaY * v.x);
            }
            else {
                a = g;
            }
            /* Apply Velocity-Verlet half step correction */
            glm::dvec3 vHalf = v + (0.5 * timeStep * a);
            /* compute tentative position */
            glm::dvec3 T = P + (timeStep * vHalf);
            /* compute RATTLE position constraint */
            double rTSq{ glm::length2(T) };
            double lambda = (pendulumLength * pendulumLength - rTSq) / (2.0 * realLengthSquared);
            glm::dvec3 pNew = T + lambda * P;
            /* compute RATTLE velocity constraint */
            double rNewSq{ glm::length2(pNew) };
            double rDotV{ glm::dot(pNew, vHalf) };
            double mu = -rDotV / rNewSq;
            vHalf = mu * pNew;
            /* recompute acceleration */
            double rNewLen = sqrt(rNewSq);
            glm::dvec3 rhatn{ pNew / rNewLen };
            double gDotRnew = glm::dot(g, rhatn);
            glm::dvec3 gtNew{ g - gDotRnew * rhatn };
            glm::dvec3 aNew;

            if (precessionActive) {
                aNew.x = gtNew.x - 2.0 * (omegaY * vHalf.z - omegaZ * vHalf.y);
                aNew.y = gtNew.y - 2.0 * (omegaZ * vHalf.x - omegaX * vHalf.z);
                aNew.z = gtNew.z - 2.0 * (omegaX * vHalf.y - omegaY * vHalf.x);
            }
            else {
                aNew = gtNew;
            }

            /* complete the velocity calculation */
            v = vHalf + (0.5 * timeStep) * aNew;
            /* update the pendulum position */
            P = pNew;

            /* Apply a correction force to keep the swing stable
             *
             * In the event correction is precisely 1.0, don't bother trying to muck
             * with velocities, introducing possible rounding errors where none was
             * needed
             */
            if ((stableSwing) && (swingCorrection != 1.0)) {
                v *= swingCorrection;
            }

            /* detect if we reached the apex yet */
            currentRadius = sqrt(P.x * P.x + P.y * P.y);

            double swingVelocity;

            if (currentRadius > 0.0)
                swingVelocity = (P.x * v.x + P.y * v.y) / currentRadius;
            else
                swingVelocity = 0.0;

            double newAngle = atan2(P.y, P.x);
            if ((lastSwingVelocity > 0.0) && (swingVelocity < 0.0)) {
                if (stableSwing) {
                    swingCorrection = (swingCorrection + (1.0 + (((desiredApexRadius / currentRadius) - 1.0) / swingCorrectionSteps))) / 2.0;
                }

                apexRadius = currentRadius;

                double testAngle = abs(newAngle - returnAngle);
                testAngle = abs(ACoord::rangempi2pi(testAngle));

                if (testAngle < 0.5) {
                    returnApex = true;

                    if (lastReturnApexTime >= 0.0) {
                        double dAz = ACoord::rangempi2pi(newAngle - lastReturnApexAngle);
                        double dt = animationTime - lastReturnApexTime;
                        double precessionRate = dAz / dt;
                        reportPrecessionRate(precessionRate, returnApex);
                    }

                    lastReturnApexTime = animationTime;
                    lastReturnApexAngle = newAngle;
                    returnAngle = newAngle;
                }
                else {
                    returnApex = false;
                    if (lastForwardApexTime >= 0.0) {
                        double dAz = ACoord::rangempi2pi(newAngle - lastForwardApexAngle);
                        double dt = animationTime - lastForwardApexTime;
                        double precessionRate = dAz / dt;
                        reportPrecessionRate(precessionRate, returnApex);
                    }
                    lastForwardApexTime = animationTime;
                    lastForwardApexAngle = newAngle;
                }
                apexDetected(newAngle, returnApex);
            }
            else if ((lastSwingVelocity < 0.0) && (swingVelocity > 0.0)) {
                nadirDetected(lastNadirRadius, lastNadirAngle, returnApex);
                lastNadirRadius = NO_DOUBLE;
            }
            else {
                if (currentRadius < lastNadirRadius) {
                    lastNadirRadius = currentRadius;
                    lastNadirAngle = newAngle;
                }
            }

            if (swingVelocity != 0.0)
                lastSwingVelocity = swingVelocity;

            if ((int)(time / 2) > (int)(lastDragTime / 2)) {
                if ((stableSwing) && (applyDrag) && (dragCoefficent < 1.0)) {
                    desiredApexRadius *= dragCoefficent;
                }
                lastDragTime = time;
            }
            animationTime += timeStep;
            computeTimeStep();
        }
        //statistics.setSimulationSeconds(animationTime);
    }

    double getX() {
        return P.x;
    }
    double getY() {
        return P.y;
    }
    double getZ() {
        return P.z;
    }

    //public PendulumStatistics getStatistics() {
    //    return statistics.copy();
    //}

    void apexDetected(double angle, bool returnApex) {
        //statistics.getLastApexPosition().updatePosition(x, y, z);
        //if (returnApex)
        //    statistics.setReturnApex(apexRadius, angle);
        //else
        //    statistics.setForwardApex(apexRadius, angle);
        //
        //statistics.setSwingCorrection(swingCorrection);
        //statistics.setSimulationSeconds(animationTime);
        //
        //if (statisticsUpdateListener != null)
        //    statisticsUpdateListener.statisticsUpdated(this, statistics.copy());
        ////		System.out.println("?? " + x + ", " + y + ", " + z);
    }

    void nadirDetected(double radius, double angle, bool forwardNadir) {
        //if (forwardNadir)
        //    statistics.setForwardNadir(radius, angle);
        //else
        //    statistics.setReturnNadir(radius, angle);
        //
        //statistics.setSimulationSeconds(animationTime);
        //
        //if (statisticsUpdateListener != null)
        //    statisticsUpdateListener.statisticsUpdated(this, statistics.copy());
        //
        ///*		if (forwardNadir) {
        //            System.out.println(
        //                    "For.Ap. = " + statistics.getForwardApex().getRadius() + "/" + statistics.getForwardApex().getAzimuth() +
        //                    " Ret.Nad. = " + statistics.getReturnNadir().getRadius() + "/" + statistics.getReturnNadir().getAzimuth() +
        //                    " Ret.Ap. = " + statistics.getReturnApex().getRadius() + "/" + statistics.getReturnApex().getAzimuth() +
        //                    " For.Nad. = " + statistics.getForwardNadir().getRadius() + "/" + statistics.getForwardNadir().getAzimuth() +
        //                    " Prec. Rate. = " + statistics.getPrecessionRate() +
        //                    " Nad. pos[x/y/z] = " + x + "/" + y + "/" + z +
        //                    " Nad. vel[x/y/z] = " + vx + "/" + vy + "/" + vz
        //                    );
        //        } */
        //        //		System.out.println("?? " + x + ", " + y + ", " + z);
    }

    void reportPrecessionRate(double precessionRate, bool returnApex) {
        //statistics.setPrecessionRate(precessionRate);
        //
        ///*		if (returnApex)
        //            System.out.println("PrR: " + precessionRate);
        //        else
        //            System.out.println("PrF: " + precessionRate); */
        //            //		System.out.println("?? RATE? " + Math.toDegrees(precessionRate));
    }


private:
    double earthRotation = 23.0 * 3600.0 + 56 * 60.0 + 4.09;  // Sidereal day in seconds

    Location location{ 40.0 * deg2rad };
    PrecessionRate precessionRate{ location, tau / earthRotation };

    double pendulumLength = 150.0;

    double desiredApexRadius = 2.5;
    double desiredStartAngle = -90.0;
    double desiredStartAngleRads = deg2rad * desiredStartAngle;
    double returnAngle = desiredStartAngleRads;

    bool runMode = false;
    // Current position and velocity
    glm::dvec3 P{ 0.0, desiredApexRadius, -sqrt(pendulumLength * pendulumLength - desiredApexRadius * desiredApexRadius) };
    glm::dvec3 v{ 0.0 };

    double metersPerSecondSquared = 9.80665;
    glm::dvec3 g{ 0.0, 0.0, -metersPerSecondSquared };

    double swingCorrectionSteps = 320000;
    double minTimeStep = 0.000001;
    double midTimeStep = 0.00001;
    double maxTimeStep = 0.0001;
    double timeStep = minTimeStep;
    double animationTime = 0.0;
    int frameCount = 0;
    double targetFrameTime = 1 / 30.0;

    bool stableSwing = true;
    bool applyDrag = false;

    double swingCorrection = 1.0;
    double dragCoefficent = 1.0;
    double lastDragTime = -1.0;
    double lastSwingVelocity = 0.0;

    bool returnApex = true;
    double currentRadius = desiredStartAngleRads;
    double lastForwardApexTime = -1;
    double lastReturnApexTime = -1;
    double lastForwardApexAngle = desiredStartAngleRads;
    double lastReturnApexAngle = pi + desiredStartAngleRads;
    double lastNadirRadius = 0.0;
    double lastNadirAngle = desiredStartAngleRads;

    double apexRadius = desiredApexRadius;

};