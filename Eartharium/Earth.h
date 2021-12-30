#pragma once
#include "Primitives.h"
#include "Astronomy.h"

// Protos - Allows classes to use pointers when they are defined above the class they want to point to.
class Location;

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
    void Draw();
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
    void Draw();
    glm::vec3 getLoc3D_NS(float rLat, float rLon, float height, float radius);
    //glm::vec3 getNml3D(float rLat, float rLon, float height = 0.0f);
private:
    void genGeom(float radius);
};


// -------
//  Earth
// -------
typedef LLH(Earth::* calcFunc)(LLH, LLH, double, double, bool);
class Earth {
public:
    Scene* m_scene = nullptr;
    //World* m_world;  // public so Location can pick it up when handed an Earth to bind to
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

private:
    struct EarthV {
        glm::vec3 position;
        glm::vec3 normal_i; // For insolation calculations, so always derived from NS
        glm::vec3 normal_g; // For lighting the object, so derived from the geometry
        glm::vec2 uv;
        glm::vec4 color;
    };
    struct dotcache {
        glm::vec4 color;
        double lat;
        double lon;
        double height;
        float size;
        unsigned int index;
    };
    struct arrowcache {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec4 color;
        float length;
        float width;
        double elevation;
        double azimuth;
        unsigned int index;
        unsigned int type;
        unsigned int unique;
    };
    struct polycache {
        PolyCurve* path;
        float width;
        unsigned int type;
        glm::vec4 color;
        LLH llh1;
        LLH llh2;
    };
    struct polycache2 {
        PolyCurve* path = nullptr;
        PolyCurve* path2 = nullptr;
        float width = 0.0f;
        unsigned int type = NONE;    // Allows Earth update() to call the right update fuunction
        glm::vec4 color = NO_COLOR;
        LLH llh1 = { 0.0, 0.0, 0.0 };
        LLH llh2 = { 0.0, 0.0, 0.0 };
        double refraction = 0.0;
        double fend = 0.0;           // 1.0 or tau depending on calculation function
        calcFunc ca;      // calculation function
        bool closed = false;
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
    glm::vec4 m_tropics = BLACK; // Overlay colors are moved here when activated. Avoids updating
    glm::vec4 m_arctics = BLACK; // Earth geometry when colors are same and morph has not changed.

    // Astronomy positions
    LLH subsolar = { 0.0, 0.0, 0.0 };     // Subsolar point in Geocentric coordinates
    //LLH sublunar = { 0.0, 0.0, 0.0 };     // Sublunar point in Geocentric coordinates
    glm::vec3 solarGP = glm::vec3(0.0f);  // Subsolar point in Cartesian coordinates
    glm::vec3 flatSun = glm::vec3(0.0f);  // Flat Sun in Cartesian coordinates (solarGP at m_flatsunheight / earthradius from GUI)
    SubSolar* m_sunob = nullptr;
    unsigned int m_sunpole = maxuint;
    unsigned int m_suncone = maxuint;
    // As other astro paths are added, they should probably be colleced in a vector and updated in a unified update function
    CelestialPath* sunpath = nullptr;
    PolyCurve* suncurve = nullptr;
    float m_flatsunheight = 3000.0f;
    bool flatsundirty = false;
    //PolyCurve* m_sunterm1 = nullptr;
    //PolyCurve* m_sunterm2 = nullptr;
    //PolyCurve* m_moonterm1 = nullptr;
    //PolyCurve* m_moonterm2 = nullptr;
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
    std::vector<polycache> m_polycache;
    std::vector<polycache2> m_polycache2;
    PolyCurve* tissotcurve = nullptr;
    PolyCurve* tissotcurve2 = nullptr;
public:
    Earth(Scene* scene, std::string mode, unsigned int meshU, unsigned int meshV);
    ~Earth();
    void setFade(float alpha);
    void updateType(std::string type);
    void Update();
    void updateMorph();
    void Draw();
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
    LLH calcRADec2LatLon(LLH radec);
    glm::vec3 calcRADec2Dir(LLH radec);
    LLH getXYZtoLLH_NS(glm::vec3 pos);
    LLH getXYZtoLLH_AE(glm::vec3 pos);

    LLH getSun(double jd = 0.0);
    LLH getPlanet(unsigned int planet, double jd = 0.0);
    LLH getMoon(double JD = 0.0);
    void CalcMoon();
    LLH CalcMoonJD(double JD);

    unsigned int addDot(double lat, double lon, double height, float size, glm::vec4 color = LIGHT_RED, bool radians = true);
    void deleteDot(unsigned int index);
    void addArrow3DTrueSun(float length = 1.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_YELLOW, bool checkit = false);
    void deleteArrow3DTrueSun();
    void changeArrow3DTrueSun(float length = 1.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_YELLOW);
    void updateArrow3DTrueSun(arrowcache& ar);
    void addLunarUmbraCone();
    glm::vec3 getSubsolarXYZ(double jd = 0.0);
    void addSubsolarPoint(float size = 0.03f);
    void deleteSubsolarPoint();
    void updateSubsolarPoint();
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
    void addViewConeXYZ_NS(glm::vec3 pos, glm::vec4 color);
    void addViewConeLLH_NS(LLH loc, glm::vec4 color);
    void addGrid(float deg = 15.0f, float size = 0.002f, glm::vec4 color = WHITE, std::string type = "LALO", bool rad = false, bool eq = true, bool pm = true);
    void removeGrid(); // Removes all latitudes and longitudes drawn by addGrid(), they are marked by type GRID
    void addEquator(float size = 0.002f, glm::vec4 color = RED);
    void removeEquator();
    void addTropics(float size = 0.002f, glm::vec4 color = YELLOW);
    void removeTropics();
    void addArcticCirles(float size = 0.002f, glm::vec4 color = AQUA);
    void removeArcticCircles();
    unsigned int addLatitudeCurve(double lat, glm::vec4 color = WHITE, float width = 0.01f, bool rad = true, unsigned int type = LATITUDE);
    void changeLatitudeCurve(unsigned int index, double lat, glm::vec4 color = NO_COLOR, float width = 0.0f, bool rad = true);
    void updateLatitudeCurve(polycache& p);
    void addPrimeMeridian(float size = 0.002f, glm::vec4 color = RED);
    void removePrimeMeridian();
    unsigned int addLongitudeCurve(double lon, glm::vec4 color = WHITE, float width = 0.01f, bool rad = true, unsigned int type = LONGITUDE);
    void changeLongitudeCurve(unsigned int index, double lon, glm::vec4 color = NO_COLOR, float width = 0.0f, bool rad = true); 
    void updateLongitudeCurve(polycache& p);
    double calcArcDist(LLH llh1, LLH llh2, bool rad);
    LLH calcGreatArc(LLH llh1, LLH llh2, double f, double refraction, bool rad = true);
    LLH calcLerpArc(LLH llh1, LLH llh2, double f, double refraction, bool rad = true);
    void addGreatArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad);
    void addLerpArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad);
    void addArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad, calcFunc ca, unsigned int type);
    void addFlatArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad);
    void updateGreatArc(polycache2& p);
    void updateLerpArc(polycache2& p);
    //void updateArc(polycache p, calcFunc ca);
    void updateFlatArc(polycache p);
    void addArcPoint(glm::vec3 ap, bool first, bool last, glm::vec3& oldap, PolyCurve* curve);
    void addTerminatorTrueMoon(glm::vec4 color = LIGHT_GREY, float width = 0.003f);
    void deleteTerminatorTrueMoon();
    void updateTerminatorTrueMoon(polycache2& p);
    void addTerminatorTrueSun(glm::vec4 color = LIGHT_YELLOW, float width = 0.003f);
    void deleteTerminatorTrueSun();
    void updateTerminatorTrueSun(polycache2& p);
    void addSubsolarPath(double begin = NO_DOUBLE, double finish = NO_DOUBLE, unsigned int steps = NO_UINT, bool fixed = false);
    void updateSubsolarPath();
    LLH calcTerminator(LLH llh1, LLH llh2, double param, double refang = 0.0, bool rad = true);
    void addTissotIndicatrix(LLH location, double radius, bool rad);
    void updateCompositePath(polycache2& p);
private:
    void genGeom();
    void updateSun();
};


// ----------
//  Location
// ----------
const unsigned int locationreserveitems = 50;

class Location {
public:
    class TrueSun {  // Experiment, not completely useful yet.
        // This kind of class avoids the need for the xxxxcache vectors, and collects the astronomy data locally so it can be shared between arrows and dots etc.
    public:
        glm::vec4 defaultcolor = SUNCOLOR;
        void enableArrow3D();
        void changeArrow3D(glm::vec4 color = NO_COLOR, float length = NO_FLOAT, float width = NO_FLOAT);
        void disableArrow3D();
        void enableLine3D();
        void enableDot3D();
        void changeDot3D(glm::vec4 color, float size);
        void disableDot3D();
        void enableAzimuthAngle();
        void enableElevationAngle();
        void enablePath24();
        void disablePath24();
        void enableAnalemma();
        void disableAnalemma();
        double getElevation(bool radians);
        double getAzimuth(bool radians);
    private:
        TrueSun(Location* location);  //ctor
        Location* m_location = nullptr;
        unsigned int m_type = TRUESUN3D;
        unsigned int m_dot = maxuint;
        unsigned int m_arrow = maxuint;
        unsigned int m_line = maxuint;
        unsigned int m_aziangle = maxuint;
        unsigned int m_eleangle = maxuint;
        LLH sun = { 0.0, 0.0, 0.0 };       // geocentric Sun GHA, Dec, Distance
        LLH localsun = { 0.0, 0.0, 0.0 };  // topocentric Sun Azi, Ele, Distance
        glm::vec3 sundir = NO_VEC3;        // Cartesian Sun direction in world coordinates
        void update(bool time, bool geometry, bool flatsun);
        void draw();
        friend class Location;
    };

    class TruePlanet {

    };

    Location::TrueSun* truesun = nullptr;  // No need to make private, it is constructed in Location ctor, so always available.

private:
    struct arrowcache {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec4 color;
        float length;
        float width;
        double elevation;
        double azimuth;
        unsigned int index;
        unsigned int type;
        unsigned int unique;
    };
    struct dotcache {
        glm::vec3 position;
        float size;
        glm::vec4 color;
        unsigned int index;
        unsigned int type;
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
        unsigned int unique;  // Used for planet id
        unsigned int index = maxuint; // index into m_polycache tightvec, needed for delete/change
    };
    struct cylindercache {
        unsigned int index;
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
        unsigned int index;
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
    // height?? !!!
    glm::vec3 m_east;
    glm::vec3 m_north;
    glm::vec3 m_zenith;
    LLH localsun = { 0.0, 0.0, 0.0 };
    std::vector<arrowcache> m_arrowcache;
    std::vector<dotcache> m_dotcache;
    std::vector<planecache> m_planecache;
    //std::vector<polycache> m_polycache;
    tightvec<polycache> m_polycache;
    std::vector<cylindercache> m_cylindercache;
    unsigned int m_observer = maxuint;
public:
    Location::TrueSun* enableTrueSun(); // Is this even needed? We construct it in Location ctor !!!
    Location(Earth* earth, double lat, double lon, bool radians = true, float rsky = 0.2f);
    ~Location();
    void Destroy();
    void Update(bool time, bool morph, bool flatsun);
    void Draw();
    glm::vec4 getPlanetColor(unsigned int planet, glm::vec4 color = NO_COLOR);
    void moveLoc(double lat, double lon, bool radians = true);
    void storeLatLon(double lat, double lon);
    double getLat(bool rad = true);
    double getLon(bool rad = true);
    glm::vec3 getPosition();
    // Calculations
    glm::vec3 calcDirEleAzi(LLH heading, bool radians = true);
    glm::vec3 calcDirRADec(double ra, double dec, double jd = 0.0);
    glm::vec3 getTrueSunDir(double jd = 0.0);
    glm::vec3 getFlatSunDir(double jd = 0.0);

    // Generics
    void addLocSky(float size = 0.2f, glm::vec4 color = glm::vec4(0.1f, 1.0f, 0.1f, 0.25f));
    void changeLocSky(float size = 0.0f, glm::vec4 color = NO_COLOR);
    void deleteLocSky();
    void updateLocSky(dotcache& d);

    void addTangentPlane(glm::vec4 color = glm::vec4(0.2f, 1.0f, 0.2f, 1.0f));
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

    void addDot3DTrueSun(glm::vec4 color = LIGHT_YELLOW, float size = locdotsize);
    void changeDot3DTrueSun(float size = locdotsize, glm::vec4 color = NO_COLOR);
    void deleteDot3DTrueSun();
    void updateDot3DTrueSun(dotcache& d);
    // Add planetary and moon dots

    // Arrows
    //unsigned int addArrow3DTrueSun(float length = 0.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_YELLOW, bool checkit = false);
    //void changeArrow3DTrueSun(float length = 0.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_YELLOW);
    //void deleteArrow3DTrueSun();
    //void updateArrow3DTrueSun(arrowcache& ar);

    void addArrow3DFlatSun(float length = 0.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_GREEN, bool checkit = false);
    void updateArrow3DFlatSun(arrowcache& ar);

    void addArrow3DTrueMoon(float length = 0.2f, float width = 0.003f, glm::vec4 color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), bool checkit = false);
    void updateArrow3DTrueMoon(arrowcache& ar);

    void addArrow3DTruePlanet(unsigned int planet, float length = 0.2f, glm::vec4 color = NO_COLOR, bool checkit = false);
    void updateArrow3DTruePlanet(arrowcache& ar);

    // Lines
    void addLine3DTrueSun(float width = loclinewidth, glm::vec4 color = SUNCOLOR, bool checkit = true);
    //void changeLine3DTrueSun();
    //void deleteLine3DTrueSun();
    void updateLine3DTrueSun(cylindercache& l);

    void addLine3DFlatSun(float width = loclinewidth, glm::vec4 color = LIGHT_GREEN, bool checkit = true);
    //void changeLine3DFlatSun();
    //void deleteLine3DFlatSun();
    void updateLine3DFlatSun(cylindercache& l);
    // TODO: Add rays/lines to an arbitrary point to illustrate view cone angles, horizons, parallelity of incoming light rays etc. !!!

    // Paths
    void addPlanetaryPath(unsigned int planet, double startoffset, double endoffset, unsigned int steps, unsigned int type = ECGEO, glm::vec4 color = NO_COLOR, float width = 0.003f);
    void updatePlanetaryPath(polycache& pa);
    void deletePlanetaryPath(unsigned int type, unsigned int unique);

    void addPlanetTruePath24(unsigned int planet, glm::vec4 color = NO_COLOR, float width = locpathwidth);
    void deletePlanetTruePath24(unsigned int planet);
    void addPlanetTruePathSidYear(unsigned int planet, glm::vec4 color = NO_COLOR, float width = locpathwidth);
    void deletePlanetTruePathSidYear(unsigned int planet);
    void addPath3DTrueSun(glm::vec4 color = SUNCOLOR, float width = locsunpathwidth);
    void deletePath3DTrueSun();
    void addAnalemma3DTrueSun(glm::vec4 color = SUNCOLOR, float width = locanalemmawidth);
    void deleteAnalemma3DTrueSun();

    void addPath3DFlatSun(glm::vec4 color = LIGHT_GREEN, float width = locsunpathwidth);
    void updatePath3DFlatSun(PolyCurve* path, glm::vec4 color = LIGHT_GREEN, float width = locsunpathwidth);
    void doPath3DFlatSun(PolyCurve* path);

    void addPath3DTrueMoon(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), float width = locpathwidth);
    void updatePath3DTrueMoon(polycache p);
    void doPath3DTrueMoon(PolyCurve* path);

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
        unsigned int index;
        unsigned int planet1;
        unsigned int planet2;
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
    float planetdot = 0.05f;
    float solsyspathwidth = 0.02f;
    Scene* m_scene = nullptr;
    Astronomy* m_astro = nullptr;
    double m_jd = 0.0;

    glm::vec3 m_earthpos = glm::vec3(0.0f);
    glm::vec3 m_sunpos = glm::vec3(0.0f);

    unsigned int m_PlanetDot[10] = { maxuint, maxuint, maxuint, maxuint, maxuint, maxuint, maxuint, maxuint, maxuint, maxuint };
    PolyCurve* m_PlanetPath[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    CelestialPath* m_PlanetOrbit[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    ParticleTrail* m_PlanetTrail[10] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

    unsigned int m_SunDot = 0;
    PolyCurve* m_SunPath = nullptr;
    ParticleTrail* m_SunTrail = nullptr;

    unsigned int m_EarthDot = 0;
    ParticleTrail* m_EarthTrail = nullptr;
public:
    SolarSystem(Scene* scene, bool geocentric);
    ~SolarSystem();
    void Update();
    void Draw();
    void addTrails(int traillen);
    void clearTrails();
    void addOrbits();
    void PlanetPos(unsigned int planet, bool update);
    glm::vec3 CalcPlanet(unsigned int planet, double jd = 0.0);
    void PlanetOrbit(unsigned int planet, bool update);
    void SunPos(bool update = false);
    void SunOrbit(bool update = true);
    glm::vec3 CalcSun(double jd = 0.0);
    void EarthPos(bool update = false);
    glm::vec3 CalcEarth(double jd = 0.0);
    glm::vec3 Ecliptic2Cartesian(double lat, double lon, double dst);
    glm::vec3 GetPlanetPos(unsigned int planet);
    void AddDistLine(unsigned int planet1, unsigned int planet2, glm::vec4 color, float width);
    void UpdateDistLines();
    void changeCentricity();
    void changeOrbits();
    void changeTrails();
    void changeTraillen();
};


// -----------
//  SkySphere
// -----------
class SkySphere {
public:
private:
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
        unsigned int index;
    };
    // Dots, Arrows, Paths, etc. Exactly like for Earth, should they derive from a parent object?
    Scene* m_scene;
    double m_JD = 0.0;
    float m_gsid = 0.0f; // Greenwich Hour Angle
    float m_Radius = 1000.0f;
    unsigned int m_meshU = 0;
    unsigned int m_meshV = 0;
    Shader* shdr;
    Texture* tex;
    VertexArray* va;
    VertexBuffer* vb;
    VertexBufferLayout* vbl;
    IndexBuffer* ib;
    std::vector<EarthV> m_verts;
    std::vector<Tri> m_tris;
    Dots* m_dots = nullptr;
    std::vector<dotcache> m_dotcache;
public:
    SkySphere(Scene* scene, unsigned int meshU, unsigned int meshV);
    ~SkySphere();
    void UpdateTime(double jd);
    void Draw();
    void addStars();
    void addDotDecRA(unsigned int unique, double dec, double ra, bool radians = true);
    void updateDotDecRA(dotcache dot);
    glm::vec3 getLoc3D_NS(double lat, double lon);
private:
    void genGeom();
};
