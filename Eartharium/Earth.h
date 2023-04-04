#pragma once
#include "Primitives.h"
#include "Astronomy.h"

// Protos - Allows classes to use pointers when they are defined above the class they want to point to.
class Location;
class SubStellarPoint;
class Earth2;


class LocGroup {
// Container vector (or map?) of Locations for SceneGraph and iterating over Locations
// Deleted Locations are retained but marked inactive, to maintain correspondance to
// loaded datasets etc. If there is a need, a xmap-remap architecture can be implemented with the tightvec template
// (see Primitives for an example.) Currently the number of locations used is small so the overhead of scanning over defunct ones is small.
public:
    std::list<Location*> locations; // holds actual locations for easy reference - NO, not at the moment !!!
private:
    Earth* earth = nullptr;
    unsigned int id = maxuint;
public:
    LocGroup(Earth* e, unsigned int identifier);
    ~LocGroup();
    void clear();
    unsigned int size();
    Location* addLocation(double lat, double lon, bool rad, float rsky);
    void removeLocation(Location* loc);
    void trimLocation();
};


class Planetoid {
    Scene* m_scene = nullptr;
    Shader* shdr = nullptr;
    Texture* tex = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb = nullptr;
    VertexBufferLayout* vbl = nullptr;
    IndexBuffer* ib = nullptr;
    std::vector<Vertex> m_verts;
    std::vector<Tri> m_tris;
    glm::vec3 position = glm::vec3(0.0f);
public:
    Planetoid(Scene* scene, unsigned int meshU, unsigned int meshV, float radius);
    ~Planetoid();
    void Update(glm::vec3 pos);
    void Draw();
    glm::vec3 getLoc3D_NS(float rLat, float rLon, float height, float radius);
    //glm::vec3 getNml3D(float lat, float lon, float height = 0.0f);
private:
    void genGeom(float radius);
};


class SubSolar {
    Scene* m_scene = nullptr;
    Shader* shdr = nullptr;
    Texture* tex = nullptr;
    VertexArray* va;
    VertexBuffer* vb;
    VertexBufferLayout* vbl;
    IndexBuffer* ib;
    std::vector<Vertex> m_verts;
    std::vector<Tri> m_tris;
    glm::vec3 position = glm::vec3(0.0f);
public:
    SubSolar(Scene* scene, unsigned int meshU, unsigned int meshV, float radius);
    ~SubSolar();
    void Update(glm::vec3 pos);
    void draw(Camera* cam);
    glm::vec3 getLoc3D_NS(float rLat, float rLon, float height, float radius);
    //glm::vec3 getNml3D(float rLat, float rLon, float height = 0.0f);
private:
    void genGeom(float radius);
};


class SubLunar {
    Scene* m_scene = nullptr;
    Shader* shdr = nullptr;
    Texture* tex = nullptr;
    VertexArray* va;
    VertexBuffer* vb;
    VertexBufferLayout* vbl;
    IndexBuffer* ib;
    std::vector<Vertex> m_verts;
    std::vector<Tri> m_tris;
    glm::vec3 position = glm::vec3(0.0f);
public:
    SubLunar(Scene* scene, unsigned int meshU, unsigned int meshV, float radius);
    ~SubLunar();
    void Update(glm::vec3 pos);
    void draw(Camera* cam);
    glm::vec3 getLoc3D_NS(float rLat, float rLon, float height, float radius);
    //glm::vec3 getNml3D(float rLat, float rLon, float height = 0.0f);
private:
    void genGeom(float radius);
};



// =====================================================================================================
//  Experimental area for building SceneObject based modular objects
// =====================================================================================================

// For Earth Loc3D and Nml3D functions, which will be used from other SceneObjects
typedef glm::vec3(Earth2::* locPos)(const double, const double, const double, const bool);
typedef glm::vec3(Earth2::* locNml)(const double, const double, const bool);

//  Earth v2.0 - Only has the geometry and drawing functions - The rest is in other SceneObjects
class Earth2 : public SceneObject {
public:
    Earth2(Scene* scene, const std::string mode, const unsigned int uMesh, const unsigned int vMesh);
    ~Earth2();
    void setMode(const std::string mode);
    void setMorphParameter(float parameter);
    float getMorphParameter();
    glm::vec3 getLoc3D(const double rLat, const double rLon, const double height, const bool rad);
    glm::vec3 getNml3D(const double rLat, const double rLon, const bool rad);
    glm::vec3 getLoc3D_NS(const double rLat, const double rLon, const double height, const bool rad);
    glm::vec3 getNml3D_NS(const double rLat, const double rLon, const bool rad);
    glm::vec3 getLoc3D_AE(const double rLat, const double rLon, const double height, const bool rad);
    glm::vec3 getNml3D_AE(const double rLat, const double rLon, const bool rad);
    glm::vec3 getLoc3D_ER(const double rLat, const double rLon, const double height, const bool deg);
    glm::vec3 getNml3D_ER(const double rLat, const double rLon, const bool rad);
    void draw(Camera* cam);
    void update() override;
private:
    void genMesh();
    void updateMorph();

private:
    // Earth Geometry
    double radius = 1.0;
    float morph_param = 0.0f;   // Valid range is 0.0f to 1.0f
    locPos pos_mode1 = nullptr;
    locPos pos_mode2 = nullptr;
    locNml nml_mode1 = nullptr;
    locNml nml_mode2 = nullptr;
    // Texture data - ideal texture size is 8192x4096
    // NOTE: Textures should be pre-flipped in Y, as SOIL2 soils itself when trying to flip large images
    const std::string daytexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\large map 8192x4096.png";
    const std::string nighttexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\BlackMarble_2012_8192x4096_ice.png";
    //const std::string nighttexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\large map 8192x4096.png";
    //const std::string daytexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\BlackMarble_2012_8192x4096_ice.png";

    float texoffset_x = -3.5f; // FIXME: use a texture definition file to supply these values
    float texoffset_y = 1.3f;  // Values given in 1/8192 x 1/4096 units, ideally pixels in the texture
    // Mesh data
    unsigned int m_meshU = 180, m_meshV = 90;
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
    // Update flags
    //bool dirty_geometry = true;
    //bool dirty_time = true;
    // OpenGL
    VertexBufferLayout* vbl = nullptr;
    VertexArray* va = nullptr;
    VertexBuffer* vb = nullptr;
    IndexBuffer* ib = nullptr;
    Shader* shdr = nullptr;
    Texture* texture_day = nullptr;
    Texture* texture_night = nullptr;
    //Texture* texture_overlay = nullptr;
    //unsigned int texture_overlay_mode = NONE;
};
// Meridian
class Longitude2 : public SceneObject {
public:
    Longitude2(Scene* scene, Earth2* geometry, double lon, float width = 0.005f, glm::vec4 color = LIGHT_GREY) {
        name = "Longitude"; // Add latitude in degrees?
        this->lon = lon;
        // Build a more generic Geometry object that handles a celestial objects geometry so Sun. Moon & Planets can be ellipsoids or whatever
        locref = geometry; // Using this now, so it can be updated to geometry later, and not be Earth2 specific
        path = new GenericPath{ scene,width,color }; // Use Scene to generate !!! Well, GenericPath uses Scene to create PolyCurve(s).
        generate();
    }
    ~Longitude2() {
        parent->removeChild(this);
        delete path;
    }
    void generate() {
        glm::vec3 pos{ 0.0f };
        for (double lat = -pi2; lat <= pi2; lat += deg2rad) {
            pos = (locref->*locpos)(lat, lon, 0.0, true); // Dynamically calls earth->getLoc3D() (Earth2::getLoc3D())
            path->addPoint(pos);
        }
        path->generate();
    }
    void update() {
        path->clearPoints();
        generate();
    }
private:
    locPos locpos = &Earth2::getLoc3D;
    Earth2* locref{ nullptr };
    GenericPath* path{ nullptr };
    double lon{ 0.0 };
};
// Circle of Latitude
class Latitude2 : public SceneObject {
public:
    Latitude2(Scene* scene, Earth2* geometry, double lat, float width = 0.005f, glm::vec4 color = LIGHT_GREY) {
        name = "Latitude"; // Add latitude in degrees?
        this->lat = lat;
        // Build a more generic Geometry object that handles a celestial objects geometry so Sun. Moon & Planets can be ellipsoids or whatever
        locref = geometry; // Using this now, so it can be updated to geometry later, and not be Earth2 specific
        path = new GenericPath{ scene,width,color }; // Use Scene to generate !!! Well, GenericPath uses Scene to create PolyCurve(s).
        generate();
    }
    ~Latitude2() {
        parent->removeChild(this);
        delete path;
    }
    void generate() {
        glm::vec3 pos{ 0.0f };
        for (double lon = -pi; lon <= pi; lon += deg2rad) {
            pos = (locref->*locpos)(lat, lon, 0.0, true); // Dynamically calls earth->getLoc3D() (Earth2::getLoc3D())
            path->addPoint(pos);
        }
        path->generate();
    }
    void update() {
        path->clearPoints();
        generate();
    }
private:
    locPos locpos = &Earth2::getLoc3D;
    Earth2* locref{ nullptr };
    GenericPath* path{ nullptr };
    double lat{ 0.0 };
};
// Prime Meridian
class PrimeMeridian2 : public Longitude2 {
public:
    PrimeMeridian2(Scene* scene, Earth2* geometry, float width = 0.007f, glm::vec4 color = LIGHT_RED) : Longitude2(scene, geometry, 0.0, width, color) {
        name = "Prime Meridian";
    }
};
// Equator v2.0 - To test with Earth2 and other celestial objects
class Equator2 : public Latitude2 {
public:
    Equator2(Scene* scene, Earth2* geometry, float width = 0.005f, glm::vec4 color = LIGHT_RED) : Latitude2(scene, geometry, 0.0, width, color) {
        name = "Equator";
    }
};
// Grid
class Grid : public SceneObject {
public:
    Grid(Scene* scene, Earth2* geometry, double spacing) {
        name = "Graticule";
        parent = geometry;
        parent->addChild(this);
        equator = new Equator2{ scene, geometry, 0.007f };
        equator->setParent(this);
        primemeridian = new PrimeMeridian2{ scene, geometry, 0.007f };
        primemeridian->setParent(this);
        for (double lat = -pi2; lat <= pi2; lat += 15 * deg2rad) {
            if (abs(lat) > tiny) {
                latitudes.emplace_back(new Latitude2(scene, geometry, lat));
                latitudes.back()->setParent(this);
            }
        }
        for (double lon = -pi; lon <= pi; lon += 15 * deg2rad) {
            if (abs(lon) > tiny) {
                longitudes.emplace_back(new Longitude2(scene, geometry, lon));
                longitudes.back()->setParent(this);
            }
        }
    }
    ~Grid() {
        parent->removeChild(this);
        delete equator;
        delete primemeridian;
        for (auto lat : latitudes) delete lat;
        for (auto lon : longitudes) delete lon;
    }
    void update() {
        for (auto child : children) {
            child->update();
        }
    }
private:
    Equator2* equator{ nullptr };
    PrimeMeridian2* primemeridian{ nullptr };
    std::vector<Latitude2*> latitudes;
    std::vector<Longitude2*> longitudes;
};
// Small Circle - Circle with given radius and lat lon position
//  Used for Tissot markers, Circle of equal altitude, etc
class SmallCircle : public SceneObject {
private:
    LLH location{ 0.0,0.0,0.0 };
    double radius = 0.0;
    locPos locpos = &Earth2::getLoc3D;
    Earth2* locref{ nullptr };
    GenericPath* path{ nullptr };
public:
    SmallCircle(Scene* scene, Earth2* geometry, LLH location, double radius, float width = 0.005f, glm::vec4 color = LIGHT_ORANGE)
    : location(location), radius(radius) {
        name = "Small Circle";
        m_scene = scene;
        this->color = color;
        locref = geometry; // Using this now, so it can be updated to geometry later, and not be Earth2 specific
        path = new GenericPath{ scene,width,color }; // Use Scene to generate !!! Well, GenericPath uses Scene to create PolyCurve(s).
        generate();
    }
    ~SmallCircle() {
        parent->removeChild(this);
        delete path;
    }
    // A radius of r degrees on the surface of a sphere with radius R has a chord (flat) radius of  R*sin(r).
    // 
    void generate() { // Generate by drawing circle around north pole and rotating into place by lat/lon
        glm::vec3 pos{ 0.0f };
        double const stepsize = tau / 360.0; // deg2rad;
        double const zangle = location.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
        double const yangle = location.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
        double const cy = cos(yangle);
        double const sy = sin(yangle);
        double const cz = cos(zangle);
        double const sz = sin(zangle);
        double const lz = sin(pi2 - radius);
        for (double angle = -pi; angle <= pi; angle += stepsize) {
            double lx = cos(pi2 - radius) * cos(angle);
            double ly = cos(pi2 - radius) * sin(angle);
            double l2x = lx * cy + lz * sy;
            double l3x = l2x * cz - ly * sz;
            double l3y = l2x * sz + ly * cz;
            double lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
            double lon = atan2(l3y, l3x);
            pos = (locref->*locpos)(lat, lon, location.dst, true); // Dynamically calls earth->getLoc3D() (Earth2::getLoc3D())
            path->addPoint(pos);
        }
        path->generate();
    }
    void update() {
        path->clearPoints();
        generate();
    }
};
class GreatCircle : public SceneObject {

};


// -------
//  Earth
// -------
typedef LLH(Earth::* calcFunc)(LLH, LLH, double, double, bool);
class Earth {
public:
    struct Intersection {
        LLH point1 = { 0.0, 0.0, 0.0 };
        LLH point2 = { 0.0, 0.0, 0.0 };
    };
    struct polycache {               // PathTracker needs this, it only references path and path2
        PolyCurve* path = nullptr;
        PolyCurve* path2 = nullptr;
        GenericPath* gpath = nullptr;
        float width = 0.0f;
        unsigned int type = NONE;    // Allows Earth update() to call the right update function
        glm::vec4 color = NO_COLOR;
        LLH llh1 = { 0.0, 0.0, 0.0 };
        LLH llh2 = { 0.0, 0.0, 0.0 };
        double refraction = 0.0;
        double fend = 0.0;           // 1.0 or tau depending on calculation function
        calcFunc ca = nullptr;                 // calculation function
        bool closed = false;
    };

    Scene* m_scene = nullptr;
    glm::vec3 SunLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 MoonLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
    bool w_sinsol = true;   // Solar insolation
    bool w_linsol = false;  // Lunar insolation
    bool w_twilight = true; // Solar twilight (civil, naval, astro)
    bool w_refract = true;  // Solar refraction. Do lunar refraction too ? !!!
    float w_srefang = deg2radf * 50.0f / 60.0f;
    float w_mrefang = deg2radf * ((50.0f - 31.45f) / 60.0f);
    float flatsunheight = 3000.0f;  // kilometers
    glm::vec4 tropics = glm::vec4(0.211f, 0.173f, 0.0f, 1.0f);
    bool tropicsoverlay = false;
    glm::vec4 arctics = glm::vec4(0.0f, 0.198f, 0.198f, 1.0f);
    bool arcticsoverlay = false;
    float param = 0.0f;
    double eccen = 0.0;
    std::string current_earth = "XXXX";
    bool do_eccentriclight = false;  // Can simulate real insolation from distant Sun on ellipsoids using eccentricity from m_eccen (updates from app.eccen)
    std::vector<LocGroup*> locgroups;

    // GUI Fields
    //float param = 0.0f;    // Moved to Application so GUI can access during refactor !!!
    unsigned int shadows = NONE;

    // Texture offsets
    float texture_x = -3.5f;
    float texture_y = 2.6f;
    float gen_texture_x = -3.5f;
    float gen_texture_y = 2.6f;
private:
    struct EarthV {
        glm::vec3 position;
        glm::vec3 normal_i; // For insolation calculations, so always derived from NS
        glm::vec3 normal_g; // For lighting the object for the Camera, so derived from the geometry
        glm::vec2 uv;
        glm::vec4 color;
    };
    struct dotcache {
        glm::vec4 color; // Store in Dots
        double lat; // LLH ?
        double lon;
        double height;
        float size;      // Store in Dots
        size_t index;
    };
    struct arrowcache {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec4 color;
        float length;
        float width;
        double elevation;
        double azimuth;
        size_t index;
        unsigned int type;
        size_t unique;
    };
    struct TissotCache {
        double lat = 0.0;
        double lon = 0.0;
        double radius = 0.0;
        glm::vec4 color = NO_COLOR;
        float width = 0.0f;
        PolyCurve* curve = nullptr;
        //bool active = true;
    };
    float m_alpha = 1.0f; // Not working well at all - probably due to backface culling being switched off !!!
    bool mydebug = false; // Set to trip breakpoints during debugging
    double m_JD = 0.0;
    bool timedirty = false;
    float m_param = 0.0f;
    double m_eccen = 0.0;
    bool paramdirty = false;
    float m_Radius = 1.0f; // 6178.137f; // Equatorial radius of Earth in world space
    unsigned int m_meshU = 0;
    unsigned int m_meshV = 0;
    std::string m_Mode = "XXXX";
    std::string m_sMode = "XX";
    std::string m_eMode = "XX";
    // Projection point
    double m_lon0 = 0.0;
    double m_lat0 = 0.0; //pi2;

    glm::vec4 m_tropics = BLACK; // Overlay colors are moved here when activated. Avoids updating
    glm::vec4 m_arctics = BLACK; // Earth geometry when colors are same and morph has not changed.

    // Astronomy positions
    LLH subsolar = { 0.0, 0.0, 0.0 };     // Subsolar point in Geocentric coordinates
    //LLH sublunar = { 0.0, 0.0, 0.0 };     // Sublunar point in Geocentric coordinates
    glm::vec3 solarGP = glm::vec3(0.0f);  // Subsolar point in Cartesian coordinates
    glm::vec3 flatSun = glm::vec3(0.0f);  // Flat Sun in Cartesian coordinates (solarGP at m_flatsunheight / earthradius from GUI)
    SubSolar* m_sunob = nullptr;
    size_t m_sundot = NO_UINT;
    size_t m_sunpole = NO_UINT;
    size_t m_suncone = NO_UINT;
    size_t m_ecliptic = NO_UINT;
    // As other astro paths are added, they should probably be colleced in a vector and updated in a unified update function
    CelestialPath* sunpath = nullptr;
    PolyCurve* suncurve = nullptr;
    float m_flatsunheight = 3000.0f;
    bool flatsundirty = false;
    double m_moonJD = 0.0; // Time Moon was calculated
    double m_moonDec = 0.0;
    double m_moonRA = 0.0;
    double m_moonHour = 0.0;
    double m_moonDist = 0.0;
    Cones* m_lunumbcone = nullptr;
    SubLunar* m_moonob = nullptr;
    Shader* shdr;
    Shader* smshdr;
    Shader* sbshdr;
    Texture* daytex;
    Texture* nighttex;
    VertexArray* va;
    VertexBuffer* vb;
    VertexBufferLayout* vbl;
    IndexBuffer* ib;
    std::vector<EarthV> m_verts;
    std::vector<Tri> m_tris;
    Dots* m_dots = nullptr;
    Arrows* m_arrows = nullptr;
    Cylinders* m_cylinders = nullptr;
    std::vector<dotcache> m_dotcache;
    std::vector<arrowcache> m_arrowcache;
    std::vector<polycache> m_polycache;  // These PolyCurves are owned by Scene, so don't delete them directly
    //std::vector<TissotCache> tissotcache;
    tightvec<TissotCache> tissotcache;
    std::vector<SubStellarPoint*> substellarpoints;
public:
    Earth(Scene* scene, std::string mode, unsigned int meshU, unsigned int meshV);
    ~Earth();
    void setFade(float alpha);
    void updateType(std::string type);
    void Update();
    void draw(Camera* cam);
    unsigned int addLocGroup();
    glm::vec3 getLoc3D(double rLat, double lon, double height = 0.0f);
    glm::vec3 getNml3D(double lat, double lon, double height = 0.0f);
    glm::vec3 getNorth3D(double lat, double lon, double height = 0.0f);
    glm::vec3 getEast3D(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_XX(std::string mode, double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_XX(std::string mode, double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_NS(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_NS(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_AE(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_AE(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_LB(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_LB(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_LA(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_LA(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_MW(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_MW(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_ER(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_ER(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_RC(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_RC(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_EX(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_EX(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_EE(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_EE(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_E8(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_E8(double lat, double lon, double height = 0.0f);
    glm::vec3 getLoc3D_E7(double lat, double lon, double height = 0.0f);
    glm::vec3 getNml3D_E7(double lat, double lon, double height = 0.0f);
    LLH calcHADec2LatLon(LLH radec, bool rad = true);
    glm::vec3 calcHADec2Dir(LLH radec);
    LLH calcRADec2LatLon(LLH radec, bool rad = true, double jd = NO_DOUBLE);
    LLH getXYZtoLLH_NS(glm::vec3 pos);
    LLH getXYZtoLLH_AE(glm::vec3 pos);
    double calcHorizonDip(double height);
    double calcSunSemiDiameter(double jd = NO_DOUBLE);
    double calcRefractionAlmanac(double elevation, double temperature, double pressure);
    double calcRefractionBennett(double elevation, double temperature, double pressure);
    double calcBrianLeakeDistance(const double elevation, bool rad = false);
    double calcBrianLeakeAngle(const double elevation, bool rad = false);
    Intersection calcSumnerIntersection(LLH llh1, LLH llh2, bool rad);
    Intersection calcSumnerIntersection(double lat1, double lon1, double rad1, double lat2, double lon2, double rad2, bool rad = true);
    LLH getSextantLoc(size_t index);
    LLH getSun(double jd = 0.0);
    LLH getSubsolar(double jd = 0.0, bool rad = true);
    LLH getPlanet(size_t planet, double jd = 0.0, bool rad = true);
    LLH getMoon(double JD = 0.0);
    void CalcMoon();
    LLH CalcMoonJD(double JD);
    size_t addDot(double lat, double lon, double height, float size, glm::vec4 color = LIGHT_RED, bool radians = true);
    void changeDotLoc(size_t index, double lat, double lon, double height, bool radians);
    void deleteDot(size_t index);
    size_t addFirstPointAries();
    void updateFirstPointAries(size_t index);
    void addArrow3DTrueSun(float length = 1.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_YELLOW, bool checkit = false);
    void deleteArrow3DTrueSun();
    void changeArrow3DTrueSun(float length = 1.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_YELLOW);
    void updateArrow3DTrueSun(arrowcache& ar);
    void addLunarUmbraCone();
    glm::vec3 getSubsolarXYZ(double jd = 0.0);
    void addSubsolarPoint(float size = 0.03f);
    void deleteSubsolarPoint();
    void updateSubsolarPoint();
    void addSubsolarDot();
    void deleteSubsolarDot();
    void updateSubsolarDot();
    void addSubsolarPole(float width = 0.003f);
    void deleteSubsolarPole();
    void updateSubsolarPole();
    void addSubsolarCone_NS(glm::vec4 color);
    void updateSubsolarCone_NS();
    LLH getSublunarPoint();
    glm::vec3 getSublunarXYZ();
    void addSublunarPoint(float size = 0.03f);
    void deleteSublunarPoint();
    void updateSublunarPoint();
    SubStellarPoint* addSubStellarPoint(const std::string& name, bool lock = false, double jd = NO_DOUBLE, glm::vec4 color = NO_COLOR);
    void removeSubStellarPoint(size_t index);
    void addViewConeXYZ_NS(glm::vec3 pos, glm::vec4 color);
    void addViewConeLLH_NS(LLH loc, glm::vec4 color);
    PolyCurve* getPath(size_t index);
    polycache* getPathCache(size_t index);
    std::vector<glm::vec3>* getPathData(size_t index);
    void addSunSectors(float width = 0.003f, glm::vec4 color = SUNCOLOR, double degrees = 45.0);
    void updateSunSectors();
    void removeSunSectors();
    void addGrid(float deg = 15.0f, float size = 0.002f, glm::vec4 color = WHITE, std::string type = "LALO", bool rad = false, bool eq = true, bool pm = true);
    void removeGrid(); // Removes all latitudes and longitudes drawn by addGrid(), they are marked by type GRID
    void addTropics(float size = 0.002f, glm::vec4 color = YELLOW);
    void removeTropics();
    void addArcticCirles(float size = 0.002f, glm::vec4 color = AQUA);
    void removeArcticCircles();
    size_t addEquator(float size = 0.002f, glm::vec4 color = RED);
    void removeEquator();
    size_t addPrimeMeridian(float size = 0.002f, glm::vec4 color = RED);
    void removePrimeMeridian();
    size_t addLatitudeCurve(double lat, glm::vec4 color = WHITE, float width = 0.01f, bool rad = true, unsigned int type = LATITUDE);
    void changeLatitudeCurve(size_t index, double lat, glm::vec4 color = NO_COLOR, float width = 0.0f, bool rad = true);
    void updateLatitudeCurve(polycache& p);
    size_t addLongitudeCurve(double lon, glm::vec4 color = WHITE, float width = 0.01f, bool rad = true, unsigned int type = LONGITUDE);
    void changeLongitudeCurve(size_t index, double lon, glm::vec4 color = NO_COLOR, float width = 0.0f, bool rad = true);
    void updateLongitudeCurve(polycache& p);
    void addEcliptic(); // Uses TissotIndicatrix, should really refactor that into a better named primitive !!!
    void updateEcliptic();
    size_t addGreatArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad);
    void removeGreatArc(size_t index);
    void changeGreatArc(size_t index, LLH llh1, LLH llh2, bool rad);
    void updateGreatArc(polycache& p);
    size_t addLerpArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad);
    size_t addFlatArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad);
    void updateLerpArc(polycache& p);
    void updateFlatArc(polycache p);
    void addTerminatorTrueMoon(glm::vec4 color = LIGHT_GREY, float width = 0.003f);
    void deleteTerminatorTrueMoon();
    void updateTerminatorTrueMoon(polycache& p);
    size_t addTerminatorTrueSun(glm::vec4 color = SUNCOLOR, float width = 0.003f);
    void deleteTerminatorTrueSun();
    void updateTerminatorTrueSun(polycache& p);
    void addSubsolarPath(double begin = NO_DOUBLE, double finish = NO_DOUBLE, unsigned int steps = NO_UINT, bool fixed = false);
    void updateSubsolarPath();
    size_t addSumnerLine(LLH gp, double elevation, glm::vec4 color = SUNMERCOLOR, float width = sumnerlinewidth, bool rad = false);
    size_t addSextantMeasurement(std::string starname, double elevation, double jd = NO_DOUBLE, glm::vec4 color = NO_COLOR, float width = pathwidth, bool rad = false);
    size_t addTissotIndicatrix(LLH location, double radius, bool rad = false, glm::vec4 color = LIGHT_ORANGE, float width = 0.005f);
    void removeTissotIndicatrix(size_t index);
    void updateTissotIndicatrix(TissotCache& tissot); // Geometry updates, no time dependence
    size_t addSemiTerminator(double radius, bool rad, glm::vec4 color, float width);
    void updateSemiTerminator(polycache& tissot);
    LLH calcGreatArc(LLH llh1, LLH llh2, double f, double refraction, bool rad = true);
    LLH calcLerpArc(LLH llh1, LLH llh2, double f, double refraction, bool rad = true);
    LLH calcTerminator(LLH llh1, LLH llh2, double param, double refang = 0.0, bool rad = true);
    LLH calcSemiTerminator(LLH llh1, LLH llh2, double param, double refang = 0.0, bool rad = true);
    double calcArcDist(LLH llh1, LLH llh2, bool rad);
    size_t addArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad, calcFunc ca, unsigned int type);
    void changeArc(size_t index, glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
    void deleteArc(size_t index);
    void updateCompositePath3(polycache& p);
    void updateCompositePath2(polycache& p);
    void addArcPoint(glm::vec3 ap, bool first, bool last, glm::vec3& oldap, PolyCurve* curve);
    void updateCompositePath(polycache& p);
    //void updateCompositePathOld(polycache& p);
private:
    void genGeom();
    void updGeom();
    void updateMorph();
    void updateSun();
};

struct Intersection {
    LLH point1 = { 0.0, 0.0, 0.0 };
    LLH point2 = { 0.0, 0.0, 0.0 };
};


// REFACTOR? Could SubStellarPoint, SubPlanetaryPoint, SubSolarPoint, SubLunarPoint, SubSatellitePoint, etc share a common parent?
// Could that parent be inherit from SceneObject ?
// If SubPoints have a common parent, they can be passed to solvers, so a fix can be obtained from the Sun, a Star and a Planet?

// -------------------
//  Sub Stellar Point
// -------------------
class SubStellarPoint {
    // GP of a star, along with Sumner line and useful calculations
    // Might contain
    // + a locator dot
    // + a circle of elevation (Sumner line)
    // + radius at adjustable bearing (clockwise from north)
    // + billboard text with name (need to add color)
    // - Improve color handling, maybe set a default color in SubStellarPoint, which is adopted (and can be overridden in components)
    // - Implement calls to update() such that they are only called when something has changed
    // - getter for paths in SumnerLine and Radius, so they can be used by PathTracker
    // - changeXX functions for all parameters
    // - Might add a pole straight up (simple long Cylinder), as Earth::*SubSolarpoint*() has that
public:
    class Name {
    public:
        void enable(Font* font, glm::vec4 color, float size) {
            m_font = font;
            if (color != NO_COLOR) m_color = color;
            if (size != NO_FLOAT) m_size = size;
            if (m_billboard == nullptr) m_billboard = new BillBoard(m_ssp->m_earth->m_scene, font, m_ssp->name, m_ssp->m_pos, m_color, m_size);
            m_enabled = true;
            update();
        }
        void update() {
            m_billboard->update(m_ssp->m_pos + 0.1f * m_ssp->m_earth->getNml3D(m_ssp->m_loc.lat, m_ssp->m_loc.lon));
        }
    private:
        glm::vec4 m_color = LIGHT_BLUE;
        float m_size = 0.0f;
        bool m_enabled = false;
        std::string m_name;
        Font* m_font = nullptr;
        BillBoard* m_billboard = nullptr;
        SubStellarPoint* m_ssp = nullptr;
        Name(SubStellarPoint& ssp, std::string starname) {
            m_name = starname;
            m_ssp = &ssp;
            m_color = m_ssp->m_color;
        }
        ~Name() {
            if (m_billboard != nullptr) delete m_billboard;
        }
        friend class SubStellarPoint;
    };
    class Dot {
    public:
        void enable(glm::vec4 color = NO_COLOR, float size = NO_FLOAT) {
            if (color != NO_COLOR) m_color = color;
            if (size != NO_FLOAT) m_size = size;
            update();
            enabled = true;
        }
        void disable() { enabled = false; }
        void update() {
            m_dotsF->changeXYZ(dotindex, m_ssp->m_pos, m_color, m_size);
            //std::cout << "Dot position: " << m_ssp->m_pos.x << "," << m_ssp->m_pos.y << "," << m_ssp->m_pos.z << '\n';
        }
    private:
        glm::vec4 m_color = NO_COLOR;
        float m_size = locdotsize;
        SubStellarPoint* m_ssp = nullptr;
        Dots* m_dotsF = nullptr;
        size_t dotindex = 0;
        bool enabled = false;
        Dot(SubStellarPoint& ssp) {
            m_ssp = &ssp;
            m_color = m_ssp->m_color;
            m_dotsF = m_ssp->m_earth->m_scene->getDotsFactory();
            dotindex = m_dotsF->addXYZ(m_ssp->m_pos, glm::vec4(0.0f), m_size); // Color with alpha = 0.0f is not rendered (shader discards explicitly)
        }
        ~Dot() {
            if (m_dotsF != nullptr) m_dotsF->remove(dotindex);
        }
        friend class SubStellarPoint;
    };
    class SumnerLine {
        // Must use more complex heuristics, it may cross poles, or seam (TWICE!) !!!
        // Seam/pole crossing can only be determined after rotations of centre to GP
    public:
        void enable(glm::vec4 color = NO_COLOR, float width = NO_FLOAT) {
            if (color != NO_COLOR) m_color = color;
            if (width != NO_FLOAT) m_width = width;
            update();
            enabled = true;
        }
        void disable() { enabled = false; }
        void update() {
            //std::cout << "SubStellarPoint::SumnerLine::update() called\n";
            // This is a copy of TissotIndicatrix, it is not happy near the south pole, so consider using Earth::updateCompositePath()
            // mechanism, or extract that into a full object on it's own. Figure out if passing a calc function outside the class is 
            // different than what Earth uses now.
            // Be aware that these are small circles, so they typically cross the seam TWICE when they do cross the seam!

            // Additionally, simulating equal elevation curves on other Earth geometries would be interesting!
            // Can this be done with a mode setting? Only update() needs to be changed, all other functions are agnostic.
            // Of course, the SubPointSolver class also needs to be enhanced.
            curve->changePolyCurve(m_color, m_width);
            curve->clearPoints();
            double const zangle = m_ssp->m_loc.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
            double const yangle = m_ssp->m_loc.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
            double const cy = cos(yangle);
            double const sy = sin(yangle);
            double const cz = cos(zangle);
            double const sz = sin(zangle);
            double const lz = sin(pi2 - m_ssp->m_elevation);
            for (double a = 0.0; a <= tau; a += tau / steps) {
                double lx = cos(pi2 - m_ssp->m_elevation) * cos(a);
                double ly = cos(pi2 - m_ssp->m_elevation) * sin(a);
                double l2x = lx * cy + lz * sy;
                double l3x = l2x * cz - ly * sz;
                double l3y = l2x * sz + ly * cz;
                double lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
                double lon = atan2(l3y, l3x);
                curve->addPoint(m_ssp->m_earth->getLoc3D(lat, lon));
            }
            curve->generate();
        }
    private:
        SubStellarPoint* m_ssp = nullptr;
        bool enabled = true;
        glm::vec4 m_color = LIGHT_ORANGE;
        float m_width = pathwidth;
        PolyCurve* curve = nullptr;
        unsigned int steps = 360;
        SumnerLine(SubStellarPoint& ssp) {
            m_ssp = &ssp;
            if (m_ssp->m_color != NO_COLOR) m_color = m_ssp->m_color;
            curve = m_ssp->m_earth->m_scene->newPolyCurve(m_color, m_width, steps);
            update();
        }
        ~SumnerLine() {
            if (curve != nullptr) m_ssp->m_earth->m_scene->deletePolyCurve(curve);
        }
        friend class SubStellarPoint;
    };
    class Radius {
        // Should this allow more than one radius line?
    public:
        void enable() { enabled = true; };
        void changeAzimuth(double bearing, bool rad) {
            if (!rad) bearing *= deg2rad;
            m_bearing = tau - bearing;
            update();
        }
        void changeAzimuthTo(LLH target, bool rad) {
            if (!rad) { target.lat *= deg2rad; target.lon *= deg2rad; }
            // Sets the radius bearing to point towards the supplied point
            m_bearing = atan2(cos(target.lat) * sin(target.lon - m_ssp->m_loc.lon),
                cos(m_ssp->m_loc.lat) * sin(target.lat) - sin(m_ssp->m_loc.lat) * cos(target.lat) * cos(target.lon - m_ssp->m_loc.lon));
            while (m_bearing > tau) m_bearing -= tau;
            while (m_bearing < 0.0) m_bearing += tau;
            //std::cout << "Location: " << m_ssp->m_loc.lat * rad2deg << "," << m_ssp->m_loc.lon * rad2deg << "\n";
            //std::cout << "Target: " << target.lat * rad2deg << "," << target.lon * rad2deg << "\n";
            //std::cout << "Bearing: " << m_bearing * rad2deg << '\n';
            update();
        }
        void update() {
            if (!enabled) return; // Skip update if not enabled
            // Uses only stored values which are all already converted to radians.
            double const zangle = m_ssp->m_loc.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
            double const yangle = m_ssp->m_loc.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
            double const cy = cos(yangle);
            double const sy = sin(yangle);
            double const cz = cos(zangle);
            double const sz = sin(zangle);
            double const lz = sin(pi2 - m_ssp->m_elevation);
            double lx = cos(pi2 - m_ssp->m_elevation) * cos(tau - m_bearing);
            double ly = cos(pi2 - m_ssp->m_elevation) * sin(tau - m_bearing);
            double l2x = lx * cy + lz * sy;
            double l3x = l2x * cz - ly * sz;
            double l3y = l2x * sz + ly * cz;
            double lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
            double lon = atan2(l3y, l3x);
            // lat,lon is now the radius end point
            if (m_radius == NO_UINT) m_radius = m_ssp->m_earth->addGreatArc(m_ssp->m_loc, { lat, lon, 0.0 }, LIGHT_BLUE, 0.003f, true);
            else m_ssp->m_earth->changeGreatArc(m_radius, m_ssp->m_loc, { lat, lon, 0.0 }, true);
        }
    private:
        SubStellarPoint* m_ssp = nullptr;
        bool enabled = false;
        size_t m_radius = NO_UINT; // Earth Arc index
        double m_bearing = 0.0;    // Radians clockwise from north
        glm::vec4 m_color = NO_COLOR;
        Radius(SubStellarPoint& ssp) : m_ssp(&ssp) {
        }
        ~Radius() {
            if (m_radius != NO_UINT) m_ssp->m_earth->removeGreatArc(m_radius);
        }
        friend class SubStellarPoint;
    };

    glm::vec3 m_pos = glm::vec3(0.0f); // XYZ in World
    LLH m_loc = { 0.0, 0.0, 0.0 };     // Lat, Lon
    LLH m_decra = { 0.0, 0.0, 0.0 };   // Declination & right ascension Equatorial Earth Centered - kept in radians
    double m_dist_lat = 0.0;
    bool locked = false;               // Freeze in time, for sextant measurements etc
    std::string name;
    double m_elevation = pi4;
    double m_jd = NO_DOUBLE;
    glm::vec4 m_color = NO_COLOR;
    Earth* m_earth = nullptr;
    // Scene* m_scene = m_earth->m_scene;
    // Astronomy* m_astro = m_earth->m_scene->m_astro;
    SubStellarPoint::Dot* dot = nullptr;
    SubStellarPoint::SumnerLine* sumner = nullptr;
    SubStellarPoint::Name* nametag = nullptr;
    SubStellarPoint::Radius* radius = nullptr;

    SubStellarPoint(Earth& earth, const std::string& starname, const bool lock = false, const double jd = NO_DOUBLE, const glm::vec4 color = NO_COLOR) {
        m_earth = &earth;
        name = starname;
        locked = lock;
        m_jd = jd;
        if (color == NO_COLOR) m_color = m_earth->m_scene->m_astro->getColorbyName(name);
        else m_color = color;
        m_decra = m_earth->m_scene->m_astro->getTrueDecRAbyNameJD(name, m_jd, true);
        //m_decra = m_earth->m_scene->m_astro->getDecRAbyName(name, true);
        m_loc = m_earth->calcRADec2LatLon(m_decra, true, m_jd);
        m_loc.lat += m_dist_lat; // Is always 0.0 at construction time, is changed by shiftSpeedTime()
        //m_earth->addDot(m_decra.lat, m_decra.lon, 0.0, 0.02f, LIGHT_RED, true);
        m_pos = m_earth->getLoc3D(m_loc.lat, m_loc.lon);
        dot = new SubStellarPoint::Dot(*this);
        sumner = new SubStellarPoint::SumnerLine(*this);
        nametag = new SubStellarPoint::Name(*this, name);
        radius = new SubStellarPoint::Radius(*this);

    }
    ~SubStellarPoint() {
        if (dot != nullptr) delete dot;
        if (sumner != nullptr) delete sumner;
        if (nametag != nullptr) delete nametag;
        if (radius != nullptr) delete radius;
    }
    void shiftSpeedTime(double bearing, double knots, double minutes) {
        // Assume bearing is 0 degrees North
        // !!! FIX: This is not the correct way !!!
        // Check the following resources:
        // https://apps.dtic.mil/sti/pdfs/ADA423226.pdf
        // https://aa.usno.navy.mil/downloads/reports/ghk_posmo.pdf
        // file:///C:/Users/micha/Downloads/New_Computational_Methods_for_Solving_Problems_of_.pdf
        if (bearing != 0.0) {
            std::cout << "ERROR: SubStellarPoint::shiftSpeedTime() only supports a bearing of 0 degrees north.\n";
            return;
        }
        // 1 knot equals 1 nautical mile per hour
        m_dist_lat = deg2rad * minutes * knots / 3600.0;  // knots/60 = NM per minute, * minutes = NMs in interval, /60 = degrees in interval
        m_loc.lat += m_dist_lat;
    }
    void setElevation(double elevation, bool rad = false) {
        // Actually stores the radius of the SumnerLine, so maybe rename the variable, at best it is the co-elevation !!!
        m_elevation = pi2 - (rad ? elevation : elevation * deg2rad);
    }
    LLH getDetails(bool rad) {
        //if (rad) return { m_loc.lat, m_loc.lon, m_elevation };
        //else return { rad2deg * m_loc.lat, rad2deg * m_loc.lon, rad2deg * m_elevation };
        // Ternary might be faster than if (), and using initializers is preferred.
        return rad ? LLH{ m_loc.lat, m_loc.lon, m_elevation } : LLH{ rad2deg * m_loc.lat, rad2deg * m_loc.lon, rad2deg * m_elevation };
    }
    void update() {
        //std::cout << "SubStellarPoint::update()\n";
        if (!locked) { // Locked means don't update location with time
            m_loc = m_earth->calcRADec2LatLon(m_decra); // m_jd not passed, so Earth calculates new location based on current time
            m_loc.lat += m_dist_lat;
        }
        // position may still change with the same location if Earth morphs
        m_pos = m_earth->getLoc3D(m_loc.lat, m_loc.lon);
        // Still update to geometry and camera orientation
        dot->update();
        sumner->update();
        radius->update();
        nametag->update();
    }
};


// -----------------
//  SubPoint Solver
// -----------------
class SubPointSolver {
    // Solves a fix for 3 observations using SubPoint - should evolve into n-point solver
    // 1) add points, 2) pull solution
public:
    SubPointSolver(Earth* earth) : m_earth(earth) {

    }
    SubStellarPoint* addSubStellarPoint(const std::string starname, const double elevation, bool rad = false, const double jd = NO_DOUBLE) {
        // Create a new SubStellarPoint and add it to the solver, returning a reference to the caller
        double mjd = jd;
        if (mjd == NO_DOUBLE) mjd = m_earth->m_scene->m_astro->getJD();
        m_ssps.push_back(new SubStellarPoint(*m_earth, starname, true, mjd, NO_COLOR));
        m_ssps.back()->setElevation(elevation, rad);
        return m_ssps.back();
    }
    void addSubStellarPoint(SubStellarPoint* ssp) {
        // Add an existing SubStellarPoint to the solver
        m_ssps.push_back(ssp);
    }
    // Perhaps add feature to show the star in the observable color, which is already available anyway !!!
    // - Best achieved by making the star color the default color in SubStellarPoint
    void showSumnerLines(glm::vec4 color = NO_COLOR, float width = NO_FLOAT) {
        for (auto& s : m_ssps) {
            s->sumner->enable(color, width);
        }
    }
    void showNames(Font* font, glm::vec4 color = NO_COLOR, float size = NO_FLOAT) {
        for (auto& s : m_ssps) {
            s->nametag->enable(font, color, size);
        }
    }
    void showDots(glm::vec4 color = NO_COLOR, float size = NO_FLOAT) {
        for (auto& s : m_ssps) {
            s->dot->enable(color, size);
        }
    }
    void update() {
        for (auto& s : m_ssps) {
            s->update();
        }
    }
    LLH calcLocation(bool rad) { // Calculates intersections on a spherical Earth (NS). Possibly add AE, ER etc.
        // Should check if at least 3 points were supplied already !!!

        // Calculate intersections of the SumnerLines
        isect12 = calcSumnerIntersection(m_ssps[0]->getDetails(true), m_ssps[1]->getDetails(true), true);
        isect23 = calcSumnerIntersection(m_ssps[1]->getDetails(true), m_ssps[2]->getDetails(true), true);
        isect31 = calcSumnerIntersection(m_ssps[2]->getDetails(true), m_ssps[0]->getDetails(true), true);
        // Should sanity check the results, if the circles do not intersect !!!

        // From Intersections pick the points that are near each other.
        // - Since the radii of the SumnerLines are always 90 degrees or less,
        //   this means from the Intersections of A and B, pick the one nearest to C's elevation.
        //   When doing so, consider that the distance may cross the seam or a pole,
        //   so use a proper distance function from Bowditch or similar.
        // - UPD: It turns out the above assumption is incorrect, what is needed is the distance closest to the radius of C
        //   (which is obvious once it is realized). NOTE: SubStellarPoint stores RADIUS in m_elevation !!!
        double dst1 = 0.0, dst2 = 0.0;
        dst1 = abs(calcArcDist(isect12.point1, m_ssps[2]->m_loc, true) - m_ssps[2]->m_elevation); // Note m_ssps indexes from 0
        dst2 = abs(calcArcDist(isect12.point2, m_ssps[2]->m_loc, true) - m_ssps[2]->m_elevation);
        spoint1 = (dst1 < dst2) ? isect12.point1 : isect12.point2;
        dst1 = abs(calcArcDist(isect23.point1, m_ssps[0]->m_loc, true) - m_ssps[0]->m_elevation);
        dst2 = abs(calcArcDist(isect23.point2, m_ssps[0]->m_loc, true) - m_ssps[0]->m_elevation);
        spoint2 = (dst1 < dst2) ? isect23.point1 : isect23.point2;
        dst1 = abs(calcArcDist(isect31.point1, m_ssps[1]->m_loc, true) - m_ssps[1]->m_elevation);
        dst2 = abs(calcArcDist(isect31.point2, m_ssps[1]->m_loc, true) - m_ssps[1]->m_elevation);
        spoint3 = (dst1 < dst2) ? isect31.point1 : isect31.point2;
        // Use those nearest points to find Most Plausible Position (MPP).
        // = Correct method is to calculate the centroid of the points,
        //   but there are other interesting possibilities that one could show, just to rule them out (phrased in terms of triangles):
        //   o orthocentre - shortest distance from the sides of the triangle (is it?), intersection of perpendiculars through opposite vertex
        //   o circumcentre - The centre of the circumscribing circle, this is equidistant from the triangle vertices
        //   o inscribed centre - centre of inscribed circle
        //   v centroid - centre of mass, intersection of median lines which go from middle of side to opposite corner
        //   o It is the centroid that is considered the right way to interpolate. Fortunately it is also the simplest
        //     to calculate, just average the coordinates of the 3 points. On a sphere this is slightly more complex,
        //     but for small triangles there is little difference, except for seam and pole considerations.

        // This is not great at the seam. OR if the points surround a pole
        //solution = { (spoint1.lat + spoint2.lat + spoint3.lat) / 3.0, (spoint1.lon + spoint2.lon + spoint3.lon) / 3.0, 0.0 };
        // Another way is to average spherical to cartesian coordinates, and normalize the resulting position vector to lie on the surface,
        // then project back to spherical coordinates. That can then be projected with the actual geometry in use.
        glm::vec3 p1 = m_earth->getLoc3D_NS(spoint1.lat, spoint1.lon);
        glm::vec3 p2 = m_earth->getLoc3D_NS(spoint2.lat, spoint2.lon);
        glm::vec3 p3 = m_earth->getLoc3D_NS(spoint3.lat, spoint3.lon);
        solution = m_earth->getXYZtoLLH_NS(glm::normalize((p1 + p2 + p3) / 3.0f)); // Earth radius is 1.0f, otherwise multiply by earth_r

        //
        // centroid calculation with more points is slightly less straight forward, see:
        // https://gis.stackexchange.com/questions/164267/how-exactly-is-the-centroid-of-polygons-calculated#:~:text=The%20centroid%20(a.k.a.%20the%20center,%2B%20y2%20%2B%20y3)%2F3.
        // https://drive.google.com/file/d/0B6wCLzdYQE_gOUVTc0FuOVFZbHM/view?usp=sharing&resourcekey=0-4Vdlynw2E9fKwpHclOJxEQ
        // (last one from https://sites.google.com/site/navigationalalgorithms/Home/papersnavigation which has other great reads too)
        if (rad) return solution;
        else return { rad2deg * solution.lat, rad2deg * solution.lon, 0.0 };
    }
    Intersection calcSumnerIntersection(LLH llh1, LLH llh2, bool rad) {
        // Returns NO_DOUBLE for all values if there are no intersections (circles are concentric or antipodal)
        // Source: https://gis.stackexchange.com/questions/48937/calculating-intersection-of-two-circles
        if (!rad) {
            llh1.lat *= deg2rad; llh1.lon *= deg2rad; llh1.dst *= deg2rad;
            llh2.lat *= deg2rad; llh2.lon *= deg2rad; llh2.dst *= deg2rad;
        }
        // transform from spherical to cartesian coordinates using LLH to store coordinates as doubles: (lat,lon,dst) <- (x,y,z)
        LLH pos1{ cos(llh1.lon) * cos(llh1.lat), sin(llh1.lon) * cos(llh1.lat), sin(llh1.lat) };
        LLH pos2{ cos(llh2.lon) * cos(llh2.lat), sin(llh2.lon) * cos(llh2.lat), sin(llh2.lat) };
        // q equal to pos1 dot pos2
        double q = pos1.lat * pos2.lat + pos1.lon * pos2.lon + pos1.dst * pos2.dst;
        double q2 = q * q;
        // q2 == 1.0 gives DIV0 in the following, and indicates that the points coincide or are antipodal.
        if (abs(q2 - 1.0) < verytiny) {
            std::cout << "Earth::calcSumnerIntersection() the circles are not intersecting! (q*q is very close to 1.0)\n";
            return { {NO_DOUBLE, NO_DOUBLE, NO_DOUBLE},{NO_DOUBLE, NO_DOUBLE, NO_DOUBLE} };
        }
        // pos0 will be a unique point on the line of intersection of the two planes defined by the two distance circles
        double a = (cos(llh1.dst) - cos(llh2.dst) * q) / (1 - q2);
        double b = (cos(llh2.dst) - cos(llh1.dst) * q) / (1 - q2);
        // pos0 is a linear combination of pos1 and pos2 with the parameters a and b
        LLH pos0 = { a * pos1.lat + b * pos2.lat, a * pos1.lon + b * pos2.lon, a * pos1.dst + b * pos2.dst };
        // n equal to pos1 cross pos2, normal to both
        LLH n = { pos1.lon * pos2.dst - pos1.dst * pos2.lon, pos1.dst * pos2.lat - pos1.lat * pos2.dst, pos1.lat * pos2.lon - pos1.lon * pos2.lat };
        // t = sqrt((1.0 - dot(pos0, pos0)) / glm::dot(n, n)); (a vector dot itself is of course the square of its magnitude
        double t = sqrt((1.0 - (pos0.lat * pos0.lat + pos0.lon * pos0.lon + pos0.dst * pos0.dst)) / (n.lat * n.lat + n.lon * n.lon + n.dst * n.dst));
        //isect1 = pos0 + t * n and isect2 = pos0 - t * n, where t is a scalar
        LLH isect1 = { pos0.lat + t * n.lat, pos0.lon + t * n.lon, pos0.dst + t * n.dst };
        LLH isect2 = { pos0.lat - t * n.lat, pos0.lon - t * n.lon, pos0.dst - t * n.dst };
        // Transform back to spherical coordinates - Are isect1 & 2 always unit vectors? !!!
        LLH ll1 = { atan2(isect1.dst, sqrt(isect1.lat * isect1.lat + isect1.lon * isect1.lon)),
            atan2(isect1.lon, isect1.lat), sqrt(isect1.lat * isect1.lat + isect1.lon * isect1.lon + isect1.dst * isect1.dst) };
        LLH ll2 = { atan2(isect2.dst, sqrt(isect2.lat * isect2.lat + isect2.lon * isect2.lon)),
            atan2(isect2.lon, isect2.lat), sqrt(isect2.lat * isect2.lat + isect2.lon * isect2.lon + isect2.dst * isect2.dst) };
        // if degrees were passed in, return degrees rather than radians
        if (!rad) {
            ll1 = { ll1.lat * rad2deg, ll1.lon * rad2deg, ll1.dst * rad2deg };
            ll2 = { ll2.lat * rad2deg, ll2.lon * rad2deg, ll2.dst * rad2deg };
        }
        return { ll1, ll2 };
    }
    double calcArcDist(LLH llh1, LLH llh2, bool rad) {
        // Calculate great circle distance using Vincenty formula simplified for sphere rather than ellipsoid.
        // Source: https://en.wikipedia.org/wiki/Great-circle_distance#Computational_formulas
        // NOTE: The Vector version given in the same source is interesting.
        //       If locations are given by position normals rather than lat&lon, then:
        //       ArcDistance = atan2( |n1 x n2| / (n1 . n2) ) on a unit sphere.
        if (!rad) {
            llh1.lat *= deg2rad; // doubles
            llh1.lon *= deg2rad;
            llh2.lat *= deg2rad;
            llh2.lon *= deg2rad;
        }
        double sin1 = sin(llh1.lat);
        double sin2 = sin(llh2.lat);
        double cos1 = cos(llh1.lat);
        double cos2 = cos(llh2.lat);
        double dlon = llh2.lon - llh1.lon;
        double sind = sin(dlon);
        double cosd = cos(dlon);
        double a = sqrt(pow((cos2 * sind), 2) + pow((cos1 * sin2 - sin1 * cos2 * cosd), 2));
        double b = sin1 * sin2 + cos1 * cos2 * cosd;
        double dist = atan2(a, b);
        if (!rad) dist *= rad2deg;
        return dist;
    }

private:
    Earth* m_earth = nullptr;
    std::vector<SubStellarPoint*> m_ssps;
    Intersection isect12; // Need to be more generic, review after coding 3 point solution and preparing for n-point solution !!!
    Intersection isect23;
    Intersection isect31;
    LLH spoint1; // Solver points - see above for n-point refactor.
    LLH spoint2;
    LLH spoint3;
    LLH solution;
};              


// ----------
//  Location
// ----------
const unsigned int locationreserveitems = 50;   // Defaults should be in config.h !!!

class Location {
public:
    class TrueSun {
        // This kind of class avoids the need for the xxxxcache vectors, and
        //  collects the astronomy data locally so it can be shared between arrows and dots etc.
        // Can it be generalized as a parent object with just calculation functions being overridden?
    public:
        glm::vec4 defaultcolor = SUNCOLOR; // Defaults go in config.h !!!
        void enableArrow3D();
        void disableArrow3D();
        void changeArrow3D(glm::vec4 color = NO_COLOR, float length = NO_FLOAT, float width = NO_FLOAT);
        void enableLine3D();
        // void disableLine3D();
        // void changeLine3D(glm::vec4 color = NO_COLOR, float length = NO_FLOAT, float width = NO_FLOAT);
        void enableDot3D();
        void disableDot3D();
        void changeDot3D(glm::vec4 color, float size);
        void enableAzimuthAngle(glm::vec4 color = NO_COLOR, float width = 0.001f);
        // void disableAzimuthAngle();
        // void changeAzimuthAngle(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void enableElevationAngle(glm::vec4 color = NO_COLOR, float width = 0.001f);
        // void disableElevationAngle();
        // void changeElevationAngle(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void enablePath24();
        // void changePath24(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void disablePath24();
        void enableAnalemma();
        // void changeAnalemma(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void disableAnalemma();
        void enableEleAngText(Font* font);
        void updateEleAngText();
        // void disableEleAngText();
        void enableAziAngText(Font* font);
        void updateAziAngText();
        // void disableAziAngText();
        double getElevation(bool radians);
        double getAzimuth(bool radians);
        double getSolarNoonJD();
    private:
        TrueSun(Location* location);  //ctor
        Location* m_location = nullptr;
        unsigned int m_type = TRUESUN3D;
        size_t m_dot = maxuint;
        size_t m_arrow = maxuint;
        size_t m_line = maxuint;
        size_t m_aziangle = maxuint;
        size_t m_eleangle = maxuint;
        TextString* m_eleangtext = nullptr;
        TextString* m_aziangtext = nullptr;
        LLH sun = { 0.0, 0.0, 0.0 };       // geocentric Sun GHA, Dec, Distance
        LLH localsun = { 0.0, 0.0, 0.0 };  // topocentric Sun Azi, Ele, Distance
        glm::vec3 sundir = NO_VEC3;        // Cartesian Sun direction in world coordinates
        void update(bool time, bool geometry);
        void draw();
        friend class Location;
    };
    class FlatSun {  // Experiment, not completely useful yet.
        // This kind of class avoids the need for the xxxxcache vectors, and
        //  collects the astronomy data locally so it can be shared between arrows and dots etc.
        // Can it be generalized as a parent object with just calculation functions being overridden?
    public:
        glm::vec4 defaultcolor = LIGHT_RED;
        void setGeometry(const std::string& geometry);
        glm::vec3 getLoc3D_XX(double lat, double lon, float height);
        void enableArrow3D();
        void disableArrow3D();
        void changeArrow3D(glm::vec4 color = NO_COLOR, float length = NO_FLOAT, float width = NO_FLOAT);
        void enableLine3D();
        // void disableLine3D();
        // void changeLine3D(glm::vec4 color = NO_COLOR, float length = NO_FLOAT, float width = NO_FLOAT);
        void enableDot3D();
        void disableDot3D();
        void changeDot3D(glm::vec4 color, float size);
        void enableAzimuthAngle();
        // void disableAzimuthAngle();
        // void changeAzimuthAngle(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void enableElevationAngle(glm::vec4 color = NO_COLOR, float width = 0.001f);
        // void disableElevationAngle();
        // void changeElevationAngle(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void enablePath24();
        void disablePath24();
        // void changePath24(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void enableAnalemma();
        void disableAnalemma();
        // void changeAnalemma(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void enableEleAngText(Font* font);
        // void disableEleAngText();
        void updateEleAngText();
        void enableAziAngText(Font* font);
        // void disableAziAngText();
        void updateAziAngText();
        double getElevation(bool radians);
        double getAzimuth(bool radians);
        void doPath24();
    private:
        FlatSun(Location* location);
        Location* m_location = nullptr;
        std::string m_Geometry = "CG";
        unsigned int m_mode = FLATSKY_GP;
        unsigned int m_type = FLATSUN3D;
        size_t m_dot = maxuint;
        size_t m_arrow = maxuint;
        size_t m_line = maxuint;
        size_t m_aziangle = maxuint;
        size_t m_eleangle = maxuint;
        TextString* m_eleangtext = nullptr;
        TextString* m_aziangtext = nullptr;
        PolyCurve* m_path24 = nullptr;
        LLH sun = { 0.0, 0.0, 0.0 };       // geocentric Sun GHA, Dec, Distance
        LLH localsun = { 0.0, 0.0, 0.0 };  // topocentric Sun Azi, Ele, Distance
        glm::vec3 sundir = NO_VEC3;        // Cartesian Sun direction in world coordinates
        void update(bool time, bool geometry);
        void draw();
        friend class Location;
    };
    class AziEleGrid {
    public:
        AziEleGrid(Location* location, double stepsize, float width, glm::vec4 color = LIGHT_PURPLE) : m_location(location) {
            m_stepsize = stepsize; // always given in radians
            for (double angle = 0.0; angle < tau; angle += m_stepsize) {
                addAzimuthCircle(angle, width, color);
            }
            for (double angle = -pi2; angle < pi2; angle += m_stepsize) {
                addElevationCircle(angle, width, color);
            }
        }
        void update(bool time, bool morph) {
            if (!morph) return; // Time passing does not change azi ele grid
            if (azicircles.size() != 0) {
                unsigned int i = 0;
                double azi = 0.0;
                for (auto c : azicircles) {
                    azi = m_stepsize * (double)i;
                    c->clearPoints();
                    for (double ele = -pi2; ele <= pi2 + tiny; ele += deg2rad) {
                        glm::vec3 point = m_location->getPosition() + m_location->m_radius * m_location->calcEleAzi2Dir({ ele,azi,0.0 });
                        c->addPoint(point);
                    }
                    i++;
                    c->generate();
                }
            }
            if (elecircles.size() != 0) {
                unsigned int i = 0;
                double ele = 0.0;
                for (auto c : elecircles) {
                    ele = m_stepsize * (double)i;
                    c->clearPoints();
                    for (double azi = 0.0; azi <= tau; azi += deg2rad) {
                        glm::vec3 point = m_location->getPosition() + m_location->m_radius * m_location->calcEleAzi2Dir({ ele,azi,0.0 });
                        c->addPoint(point);
                    }
                    i++;
                    c->generate();
                }
            }
        }
        void addAzimuthCircle(double azi, float width, glm::vec4 color) {
            // Actually a half-circle
            azicircles.emplace_back(new GenericPath(m_location->m_scene, width, color));
            for (double ele = -pi2; ele <= pi2 + tiny; ele += deg2rad) {
                glm::vec3 point = m_location->getPosition() + m_location->m_radius * m_location->calcEleAzi2Dir({ ele,azi,0.0 });
                azicircles.back()->addPoint(point);
            }
            azicircles.back()->generate();
        }
        void addElevationCircle(double ele, float width, glm::vec4 color) {
            elecircles.emplace_back(new GenericPath(m_location->m_scene, width, color));
            for (double azi = 0.0; azi <= tau; azi += deg2rad) {
                glm::vec3 point = m_location->getPosition() + m_location->m_radius * m_location->calcEleAzi2Dir({ ele,azi,0.0 });
                elecircles.back()->addPoint(point);
            }
            elecircles.back()->generate();
        }
    private:
        double m_stepsize{ 15.0 * deg2rad };
        Location* m_location{ nullptr };
        std::vector<GenericPath*> azicircles;
        std::vector<GenericPath*> elecircles;
        friend class Location;
    };

    Location::TrueSun* truesun = nullptr;
    Location::FlatSun* flatsun = nullptr;
    Location::AziEleGrid* azielegrid{ nullptr };
private:
    struct arrowcache {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec4 color;
        float length;
        float width;
        double elevation;
        double azimuth;
        size_t index;
        unsigned int type;
        size_t unique;
    };
    struct dotcache {
        glm::vec3 position;
        float size;
        glm::vec4 color;
        size_t index;
        unsigned int type;
        size_t unique;
        void changeColor(glm::vec4 c) { color = c; }
        void changeSize(float s) { size = s; }
        void changePosition(glm::vec3 pos) { position = pos; }
    };
    struct polycache {
        PolyCurve* path;
        CelestialPath* planetCP;
        float width;
        unsigned int type;
        glm::vec4 color;
        double elevation;
        double azimuth;
        size_t unique;          // Used for planet id
        size_t index = maxuint; // index into m_polycache tightvec, needed for delete/change
    };
    struct cylindercache {
        size_t index;
        unsigned int type;
        glm::vec3 start;
        glm::vec3 end;
        float width;
        glm::vec4 color;
    };
    struct planecache {
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 direction = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec2 scale = glm::vec2(1.0f, 1.0f);
        glm::vec4 color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        size_t index;
        unsigned int type;
    };
    struct sidyearparm {
        double bracket;
        double stepsize;
    };
    std::array<sidyearparm, 8> sidyearparms // Move to config object ? Or Astronomy.h ? !!!
    { {                                     // Also check similar table in SolarSystem::m_planetinfos
        { 183.0, 1.0 },    // Sun
        { 190.0, 1.0 },    // Mercury
        { 205.0, 1.0 },    // Venus
        { 300.0, 1.0 },    // Mars
        { 2150.0, 5.0 },   // Jupiter
        { 5500.0, 20.0 },  // Saturn
        { 15500.0, 50.0},  // Uranus
        { 60000.0, 200.0 } // Neptune
        // Earth would be here, inconveniently in an odd position in the enum
    } };
    Earth* m_earth = nullptr;
    Scene* m_scene = nullptr;
    Arrows* m_arrows = nullptr;
    Dots* m_dots = nullptr;
    Planes* m_planes = nullptr;
    Cylinders* m_cylinders = nullptr;
    AngleArcs* m_anglearcs = nullptr;
    glm::vec3 m_pos = glm::vec3(0.0f);
    double m_lat = 0.0;  // in radians
    double m_lon = 0.0;
    float m_radius = 0.0f;
    glm::mat4 m_world2local = glm::mat4(0.0f);
    std::string m_timezone = "LongestLongest/LongestLongest";
    // height?? !!!
    glm::vec3 m_east;
    glm::vec3 m_north;
    glm::vec3 m_zenith;
    LLH localsun = { 0.0, 0.0, 0.0 };
    std::vector<arrowcache> m_arrowcache;
    std::vector<dotcache> m_dotcache;
    std::vector<planecache> m_planecache;
    tightvec<polycache> m_polycache;
    std::vector<cylindercache> m_cylindercache;
    ParticleTrail* m_lunalemmatrail = nullptr;
    size_t m_observer = maxuint;
public:
    Location(Earth* earth, double lat, double lon, bool radians = true, float rsky = 0.2f);
    ~Location();
    void Destroy();
    void Update(bool time, bool morph, bool flatsun);
    void Draw(Camera* cam);
    glm::vec4 getPlanetColor(size_t planet, glm::vec4 color = NO_COLOR);
    void moveLoc(double lat, double lon, bool radians = true);
    void storeLatLon(double lat, double lon);
    double getLat(bool rad = true);
    double getLon(bool rad = true);
    glm::vec3 getPosition();
    glm::vec3 getZenith();
    glm::vec3 getNorth();
    void moveToXYZ(glm::vec3 pos);
    void setTimeZone(const std::string& timezonename);

    // Calculations
    glm::vec3 calcEleAzi2Dir(LLH heading, bool radians = true);
    LLH calcDir2EleAzi(glm::vec3 direction, bool rads = true);
    glm::vec3 calcDecRA2Dir(double jd, double dec, double ra, bool rad = false);
    glm::vec3 calcDirRADec(double ra, double dec, double jd = 0.0);
    glm::vec3 getTrueSunDir(double jd = 0.0);
    glm::vec3 getFlatSunDir(double jd = 0.0);
    glm::mat4 calcWorld2LocalMatrix();
    // Generics
    void addLocSky(float size = 0.2f, glm::vec4 color = glm::vec4(0.1f, 1.0f, 0.1f, 0.25f));
    void changeLocSky(float size = 0.0f, glm::vec4 color = NO_COLOR);
    void deleteLocSky();
    void updateLocSky(dotcache& d);

    void addHorizon(glm::vec4 color = RED, float width = 0.005f);
    void updatePathHorizon(polycache& pc);
    void doPathHorizon(PolyCurve* path);
    void addCircumPolar(glm::vec4 color = GREEN, float width = 0.005f);
    void updatePathCircumPolar(polycache& pc);
    void doPathCircumPolar(PolyCurve* path);

    void addTangentPlane(glm::vec4 color = glm::vec4(0.2f, 1.0f, 0.2f, 0.25f), float alpha = NO_FLOAT);
    void deleteTangentPlane();
    void updateTangentPlane(planecache& p);

    void addMeridianPlane(glm::vec4 color = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f));
    void deleteMeridianPlane();
    void updateMeridianPlane(planecache& p);

    void addCoords(float length = 0.2f);
    void addUpCoord(float length = 0.2f, float width = loccoordarrowwidth);
    void updateUpCoord(arrowcache& ar);
    void addEastCoord(float length = 0.2f, float width = loccoordarrowwidth);
    void updateEastCoord(arrowcache& ar);
    void addNorthCoord(float length = 0.2f, float width = loccoordarrowwidth);
    void updateNorthCoord(arrowcache& ar);

    void addNormal(float length = 0.2f, float width = loccoordarrowwidth);
    void updateNormalCoord(arrowcache& ar);

    void addAziEleGrid(double degrees = 15.0, bool radians = false, float width = 0.003f, glm::vec4 color = LIGHT_PURPLE);
    void addObserver(float bearing, glm::vec4 color = LIGHT_GREY, float height = 0.0f);
    void changeObserver(float bearing, glm::vec4 color = NO_COLOR, float height = 0.0f);
    void addArrow3DEleAzi(unsigned int unique, double ele, double azi, float length = 0.2f, float width = locazielewidth, glm::vec4 color = GREEN);
    void deleteArrow3DEleAzi(unsigned int unique);
    void changeArrow3DEleAzi(unsigned int unique, double ele, double azi, float length = 0.2f, float width = locazielewidth, glm::vec4 color = GREEN);
    void changeArrow3DEleAziColor(unsigned int unique, glm::vec4 color = GREEN);
    void updateArrow3DEleAzi(arrowcache& ar);

    void addArrow3DRADec(unsigned int unique, double ra, double dec, glm::vec4 color, float width, float length);
    void updateArrow3DRADec(arrowcache& ar);

    // Dots
    void addLocDot(float size = locdotsize, glm::vec4 color = DEFAULT_LOCDOT_COLOR);
    void changeLocDot(float size = -1.0f, glm::vec4 color = NO_COLOR);
    void deleteLocDot();
    void updateLocDot(dotcache& d);
    void addTruePlanetDot(size_t planet, float size, glm::vec4 color, bool checkit);
    void updateTruePlanetDot(dotcache& dc);

    // Arrows
    void addArrow3DFlatSun(float length = 0.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_GREEN, bool checkit = false);
    void updateArrow3DFlatSun(arrowcache& ar);

    void addArrow3DTrueMoon(float length = 0.2f, float width = 0.003f, glm::vec4 color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), bool checkit = false);
    void updateArrow3DTrueMoon(arrowcache& ar);

    void addArrow3DTruePlanet(size_t planet, float length = 0.2f, glm::vec4 color = NO_COLOR, bool checkit = false);
    void updateArrow3DTruePlanet(arrowcache& ar);

    // Lines
    void addLine3DFlatSun(float width = loclinewidth, glm::vec4 color = LIGHT_GREEN, bool checkit = true);
    //void changeLine3DFlatSun();
    //void deleteLine3DFlatSun();
    void updateLine3DFlatSun(cylindercache& l);
    // TODO: Add rays/lines to an arbitrary point to illustrate view cone angles, horizons, parallelity of incoming light rays etc. !!!

    // Paths
    void addPlanetaryPath(size_t planet, double startoffset, double endoffset, unsigned int steps, unsigned int type = ECGEO, glm::vec4 color = NO_COLOR, float width = 0.003f);
    void updatePlanetaryPath(polycache& pa);
    void deletePlanetaryPath(unsigned int type, size_t unique);

    void addPlanetTruePath24(size_t planet, glm::vec4 color = NO_COLOR, float width = locpathwidth);
    void deletePlanetTruePath24(size_t planet);
    void addPlanetTruePathSidYear(size_t planet, glm::vec4 color = NO_COLOR, float width = locpathwidth);
    void deletePlanetTruePathSidYear(size_t planet);

    void addPath3DFlatSun(glm::vec4 color = LIGHT_GREEN, float width = locsunpathwidth);
    void updatePath3DFlatSun(PolyCurve* path, glm::vec4 color = LIGHT_GREEN, float width = locsunpathwidth);
    void doPath3DFlatSun(PolyCurve* path);

    void addPath3DTrueMoon(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), float width = locpathwidth);
    void updatePath3DTrueMoon(polycache p);
    void doPath3DTrueMoon(PolyCurve* path);

    void addTrueLunalemma(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), float width = locpathwidth, float size = 0.005f);
    void updateTrueLunalemma(polycache p);
    void doTrueLunalemma(PolyCurve* path);

    void addPath3DRADec(unsigned int unique, double ra, double dec, glm::vec4 color, float width);
    void updatePath3DRADec(polycache& pa);
    void doPath3DRADec(double ra, double dec, PolyCurve* path);
};


// -------------
//  SolarSystem
// -------------
class SolarSystem {
public:
    struct distline {
        size_t index;
        size_t planet1;
        size_t planet2;
        glm::vec4 color;
        float width;
    };
    // For GUI
    bool geocentric = false;
    bool orbits = false;
    bool trails = false;
    int traillen = 400;
    int orbitsteps = 100;
private:
    struct planetinfo {
        double sidyear = 0.0; // sidereal year of planet in Earth days
        glm::vec4 color = NO_COLOR;
    };
    std::array<planetinfo, 9> m_planetinfos = {{  // sidereal years are a bit long to allow orbits to close
        { 368,   SUNCOLOR },     // Sun
        { 88,    MERCURYCOLOR }, // Mercury
        { 226,   VENUSCOLOR },   // Venus
        { 688,   MARSCOLOR },    // Mars
        { 4336,  JUPITERCOLOR }, // Jupiter
        { 10776, SATURNCOLOR },  // Saturn
        { 30688, URANUSCOLOR },  // Uranus
        { 60182, NEPTUNECOLOR }, // Neptune
        { 368,   EARTHCOLOR }    // Earth
    }};
    bool m_geocentric = false;
    bool m_orbits = false;
    bool m_trails = false;
    int m_traillen = 366;
    Cylinders* m_cylinders = nullptr;  // Used for DistLines
    tightvec<distline> m_distlines;
    float planetdot = 0.07f;
    float solsyspathwidth = 0.015f;
    Scene* m_scene = nullptr;
    Astronomy* m_astro = nullptr;
    double m_jd = 0.0;

    glm::vec3 m_earthpos = glm::vec3(0.0f);
    glm::vec3 m_sunpos = glm::vec3(0.0f);

    size_t m_PlanetDot[10] = { maxuint, maxuint, maxuint, maxuint, maxuint, maxuint, maxuint, maxuint, maxuint, maxuint };
    PolyCurve* m_PlanetPath[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    CelestialPath* m_PlanetOrbit[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    ParticleTrail* m_PlanetTrail[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    size_t m_SunDot = 0;
    PolyCurve* m_SunPath = nullptr;
    ParticleTrail* m_SunTrail = nullptr;

    size_t m_EarthDot = 0;
    ParticleTrail* m_EarthTrail = nullptr;
public:
    SolarSystem(Scene* scene, bool geocentric);
    ~SolarSystem();
    void Update();
    void Draw();
    void addTrails(int traillen);
    void clearTrails();
    void PlanetPos(size_t planet, bool update);
    glm::vec3 CalcPlanet(size_t planet, double jd = 0.0);
    void PlanetOrbit(size_t planet);
    void SunPos(bool update = false);
    void SunOrbit(bool update = true);
    glm::vec3 CalcSun(double jd = 0.0);
    void EarthPos(bool update = false);
    glm::vec3 CalcEarth(double jd = 0.0);
    glm::vec3 Ecliptic2Cartesian(double lat, double lon, double dst);
    glm::vec3 GetPlanetPos(size_t planet);
    void AddDistLine(size_t planet1, size_t planet2, glm::vec4 color, float width);
    void UpdateDistLines();
    void changeCentricity();
    void changeOrbits();
    void changeTrails();
    void changeTraillen();
};


// -----------
//  SkySphere
// -----------
typedef LLH(SkySphere::* calcFuncSS)(LLH, LLH, double, double, bool);
class SkySphere {
public:
    bool has_grid = false;
    bool m_texture = true;
    unsigned int mode = FLATSKY_GP;
    float domeheight = 5004.0f; // float domeheight / (float)earthradius;
private:
    float m_gridwidth = 0.003f;
    struct EarthV {
        glm::vec3 position;
        //glm::vec3 normal_i; // For insolation calculations, so always derived from NS
        //glm::vec3 normal_g; // For lighting the object, so derived from the geometry
        glm::vec2 uv;
        //glm::vec4 color;
    };
    struct dotcache {
        unsigned int unique;
        glm::vec4 color;
        double lat;
        double lon;
        double height;
        float size;
        size_t index;
    };
    struct pathcache {
        GenericPath* path = nullptr;
        //PolyCurve* path = nullptr;
        //PolyCurve* path2 = nullptr;
        float width = 0.0f;
        unsigned int type = NONE;
        glm::vec4 color = NO_COLOR;
        LLH llh1 = { 0.0, 0.0, 0.0 };  // params specific to user func
        LLH llh2 = { 0.0, 0.0, 0.0 };
        double refraction = 0.0;
        double fend = 0.0;           // 1.0 or tau depending on calculation function
        calcFuncSS ca;                 // calculation function
        bool closed = false;
    };
    // Dots, Arrows, Paths, etc. Exactly like for Earth, should they derive from a parent object?
    Scene* m_scene = nullptr;
    Earth* m_earth = nullptr; // Used for some modes, such as GP
    Location* m_location = nullptr;
    //double m_JD = 0.0;
    float m_gsid = 0.0f; // Greenwich Hour Angle
    bool m_movable;
    float m_Radius = 2.0f;
    unsigned int m_meshU = 0;
    unsigned int m_meshV = 0;
    double gridspacing = 15.0;
    Shader* shdr;
    Texture* tex;
    VertexArray* va;
    VertexBuffer* vb;
    VertexBufferLayout* vbl;
    IndexBuffer* ib;
    std::vector<EarthV> m_verts;
    std::vector<Tri> m_tris;
    SkyDots* m_dots = nullptr;
    std::vector<dotcache> m_dotcache;
    unsigned int curr_unique = 0;
    std::vector<PolyCurve*> pv;  // Used for RA Dec grid
    std::vector<GenericPath*> m_paths;

    //std::vector<Astronomy::stellarobject> m_stellarobjects;
public:
    SkySphere(Scene* scene, unsigned int meshU, unsigned int meshV, bool texture);
    ~SkySphere();
    void setRadius(float radius);
    void setTexture(bool texture);
    void setMode(const std::string& mode, Earth* earth = nullptr, Location* location = nullptr);
    void setMovable(bool movable);
    void UpdateTime(double jd);
    void draw();
    void addStars(double magnitude = 6.0); // The star database currently only holds all stars with magnitude 6.0 or brighter
    unsigned int addStarByName(const std::string name);
    void addDotDecRA(unsigned int unique, double dec, double ra, glm::vec4 color, float size, bool radians = true);
    void updateDotDecRA(dotcache& dot);
    void addGrid(float width = 0.003f);
    void updateGrid();
    glm::vec3 getDecRA2Pos3D(double dec, double ra);
    glm::vec3 getLoc3D_NS(double lat, double lon);
    void addLineStarStar(const std::string star1, const std::string star2, glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
private:
    void loadStars();
    void genGeom();
    float getMagnitude2Radius(double magnitude);
};


// -------------
//  PathTracker
// -------------
template<typename T>
class PathTracker {
    // Similar to a Lerper, but able to move a Location, Camera LookAt, etc. along one or two std::vector<glm::vec3> path(s)
    // Each moveable MUST implement a moveToXYZ() method. Could enforce with inheritance, but it seems like overkill.

    // Improvements:
    // v Bounce - run path back and forth rather than stopping at end
    // v Loop - run path forwards, and wrap to beginning when reaching the end
    // v Halt - run start to finish and stop, endlessly returning the last value
    // = Read across end of path1 into path2
    //   o UPD: Works, but should consider skipping duplicate datapoints at path seams, see below.
    // - getNextSmooth() function? Requires interpolating values, and analyzing velocities/accelerations, so maybe not.
    // = At least attempt a frames feature with nearest point assuming linear time.
    //   v UPD: Did a LERP between consequtive values, should work for both sparse and dense steps
    //   o For LOOP mode, there is still a consideration around the end of path2 to the start of path1: 
    // - When more than one path is used, check if the end of one and the beginning of the following coincides (same XYZ).
    //   In that case skip one data point. This goes both for returning data points, AND for calculating the length in getLength() !!!
    //   Also, whether to skip the end of path2<->start of path1 should be decided based on the mode:
    //   HALT and BOUNCE should retain the value, LOOP should not !!!
    //   When LERPing, this can be difficult to account for. Maybe a flag/test of mode that indicates seam issues, and a count of where the seam is (pathxyz.size()-1)

    // Could also support PolyLine objects.
    unsigned int m_mode = HALT;
    bool forward = true;
    T* movable = nullptr;  // Movable object, such as Location, Camera, Satellite & Airplane
    std::vector<glm::vec3>* pathxyz = nullptr;
    std::vector<glm::vec3>* pathxyz2 = nullptr;
    unsigned int count = 0;
    bool lerp = false;
    unsigned int lsteps = 0; // desired number of steps, lerp if this doesn't match the data length
    double fcount = 0.0;     // fractional count for lerping
    double fstep = 0.0;      // fractional step size for lerping
    // std::vector<LLH>* pathllh = nullptr; // For paths in lat/lon, ele/azi, dec/RA
public:
    PathTracker(T* movable, PolyCurve* path, unsigned int mode, unsigned int steps = 0);
    PathTracker(T* movable, PolyCurve* path, PolyCurve* path2, unsigned int mode, unsigned int steps = 0);
    PathTracker(T* movable, Earth::polycache* pcache, unsigned int mode, unsigned int steps = 0);
    unsigned int getLength();
    void moveNext();
    glm::vec3 getNext();
    glm::vec3 lookup(unsigned int index);
    glm::vec3 getCurrent();
private:
    glm::vec3 next(); // Simply the next position in the sequence, without incrementing counters or anything else.
    glm::vec3 calcLerp(double fraction, glm::vec3 svalue, glm::vec3 evalue);
    glm::vec3 nextLerped();
    void incCount();
};
template<typename T>
PathTracker<T>::PathTracker(T* object, PolyCurve* path, unsigned int mode, unsigned int steps) {
    m_mode = mode;
    pathxyz = &path->m_points;
    movable = object;
    if (steps == 0 || steps == getLength()) lerp = false;
    else {
        lerp = true;
        fstep = (double)(getLength() - 1) / (double)steps;
    }
    lsteps = steps;
}
template<typename T>
PathTracker<T>::PathTracker(T* object, PolyCurve* path, PolyCurve* path2, unsigned int mode, unsigned int steps) {
    m_mode = mode;
    pathxyz = &path->m_points;
    pathxyz2 = &path2->m_points;
    movable = object;
    if (steps == 0 || steps == getLength()) lerp = false;
    else {
        lerp = true;
        fstep = (double)(getLength() - 1) / (double)steps;
    }
    lsteps = steps;
}
template<typename T>
PathTracker<T>::PathTracker(T* object, Earth::polycache* pcache, unsigned int mode, unsigned int steps) {
    m_mode = mode;
    pathxyz = &pcache->path->m_points;
    pathxyz2 = &pcache->path2->m_points;
    movable = object;
    if (steps == 0 || steps == getLength()) lerp = false;
    else {
        lerp = true;
        fstep = (double)(getLength() - 1) / (double)steps;
    }
    lsteps = steps;
}
template<typename T>
unsigned int PathTracker<T>::getLength() {
    return (unsigned int)(pathxyz->size() + ((pathxyz2 == nullptr) ? 0 : pathxyz2->size()));
}
template<typename T>
void PathTracker<T>::moveNext() {
    // NOTE: If the path is updated, it is possible that we end up pointing outside the path data.
    if (!lerp) movable->moveToXYZ(next());
    else movable->moveToXYZ(nextLerped());
    incCount();
}
template<typename T>
glm::vec3 PathTracker<T>::getNext() {
    // NOTE: Gives next value directly, without advancing counter. Make flag to inc or not, or a peakNext() !!!
    // Multiple consequtive calls will return the same value.
    return next();
}
template<typename T>
glm::vec3 PathTracker<T>::getCurrent() {
    if (count < pathxyz->size()) {
        return pathxyz->at(count);
    }
    return pathxyz->back(); // If we are at the end, use end value
}
template<typename T>
glm::vec3 PathTracker<T>::next() {
    if (count < pathxyz->size()) return pathxyz->at(count);
    if (pathxyz2 == nullptr) return pathxyz->back(); // Revise when adding bounce and loop
    if (count < pathxyz->size() + pathxyz2->size() - 2) return pathxyz2->at(count - pathxyz->size() + 2);
    return pathxyz2->back();
}
template<typename T>
glm::vec3 PathTracker<T>::lookup(unsigned int index) {
    if (index < pathxyz->size()) return pathxyz->at(index);
    if (pathxyz2 == nullptr) return pathxyz->back(); // Revise when adding bounce and loop
    if (index < pathxyz->size() + pathxyz2->size() - 2) return pathxyz2->at(index - pathxyz->size() + 2);
    return pathxyz2->back();
}
template<typename T>
glm::vec3 PathTracker<T>::calcLerp(double fraction, glm::vec3 svalue, glm::vec3 evalue) {
    return evalue * (float)fraction + svalue * (1.0f - (float)fraction);
}
template<typename T>
glm::vec3 PathTracker<T>::nextLerped() {
    fcount = (double)count * fstep;
    //std::cout << "nextLerped() fcount: " << fcount << "\n";
    // At end points, there is nothing to lerp
    if (fcount == 0.0) return pathxyz->at(0); // equivalent to count == 0
    if (fcount >= getLength() && pathxyz2 != nullptr) return pathxyz2->back(); 

    // Splayed out for easy debugging. Collect into single call to calcLerp() when verified.
    unsigned int sindex = (unsigned int)floor(fcount);
    unsigned int eindex = (unsigned int)ceil(fcount);
    double fraction = fcount - sindex;

    glm::vec3 svalue = lookup(sindex);
    glm::vec3 evalue = lookup(eindex);

    return calcLerp(fraction, svalue, evalue);
}
template<typename T>
void PathTracker<T>::incCount() { // Moves count ahead by one, in whichever direction is currently applicable. For now only Halt works.
    //std::cout << "Count: " << count << "\n";
    if (lerp) { // Simpler case, count simply runs in [0;steps] interval
        if (m_mode == LOOP) {
            count++;
            if (count > lsteps) count = 0;
            return;
        }
        if (m_mode == HALT) {
            count++;
            if (count > lsteps) count = lsteps;
            return;
        }
        if (m_mode == BOUNCE) {
            if (forward) {
                if (count >= lsteps) {
                    forward = false;
                    count--;  // If path has been updated to less entries, this might stay outside the range !!!
                    return;
                }
                // otherwise advance
                count++;
                return;
            }
            if (!forward) { // Backwards
                if (count == 0) {
                    forward = true;
                    count++;
                    return;
                }
                count--;
                return;
            }
        }
    }
    else {
        if (m_mode == HALT) { // Halt implementation
            count++;
            if (pathxyz2 == nullptr) {
                if (count >= pathxyz->size()) count = (unsigned int)pathxyz->size() - 1;
                return;
            }
            // pathxyz2 potentially contains data
            if (count >= pathxyz->size() + pathxyz2->size() - 2) count = (unsigned int)pathxyz->size() + (unsigned int)pathxyz2->size() - 2;
        }
        if (m_mode == LOOP) {
            count++;
            if (pathxyz2 == nullptr) {
                if (count >= pathxyz->size()) count = 0;
                return;
            }
            // pathxyz2 potentially contains data
            if (count >= pathxyz->size() + pathxyz2->size() - 2) count = 0;
        }
        if (m_mode == BOUNCE) {
            if (forward) {
                // If at end, reverse
                //if (count >= pathxyz->size() - 1 && pathxyz2 == nullptr) {
                //    forward = false;
                //    count--;
                //    return;
                //}
                if (count >= pathxyz->size() + (pathxyz2 == nullptr ? 0 : pathxyz2->size()) - 1) {
                    forward = false;
                    count--;  // If path has been updated to less entries, this might stay outside the range !!!
                    return;
                }
                // otherwise advance
                count++;
                return;    // When we bounce from forward to backward, both the above and the below would trigger
            }
            if (!forward) { // Backwards
                if (count == 0) {
                    forward = true;
                    count++;
                    return;
                }
                count--;
                return;
            }
        }
    }
}