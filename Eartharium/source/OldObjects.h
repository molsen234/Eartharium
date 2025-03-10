#pragma once

#include <array>
#include <cmath>
#include <cstdint>
#include <list>
#include <string>
#include <vector>

#include <glm/fwd.hpp>

#include "Astronomy.h"
#include "Utilities.h"    // For ShapeFile
#include "Primitives.h"
#include "Renderer.h"  // -> SceneObjects
#include "config.h"
#include "Earth.h"
#include "OpenGL.h"

#include "../../EAstronomy/aconfig.h"
#include "../../EAstronomy/acoordinates.h"
#include "../../EAstronomy/config.h"

// Protos - Allows classes to use pointers when they are defined above the class they want to point to.
class Location;
class SubStellarPoint;
class SunGP;

//class SubSolar {
//    Scene* m_scene = nullptr;
//    Shader* shdr = nullptr;
//    Texture* tex = nullptr;
//    VertexArray* va;
//    VertexBuffer* vb;
//    VertexBufferLayout* vbl;
//    IndexBuffer* ib;
//    std::vector<Vertex> m_verts;
//    std::vector<Tri> m_tris;
//    glm::vec3 position = glm::vec3(0.0f);
//public:
//    SubSolar(Scene* scene, unsigned int meshU, unsigned int meshV, float radius);
//    ~SubSolar();
//    void Update(glm::vec3 pos);
//    void draw(Camera* cam);
//    glm::vec3 getLoc3D_NS(float rLat, float rLon, float height, float radius);
//    //glm::vec3 getNml3D(float rLat, float rLon, float height = 0.0f);
//private:
//    void genGeom(float radius);
//};


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



class LocGroup {
public:
    std::list<Location*> locations;
private:
    Earth* earth = nullptr;
    unsigned int id = maxuint;
public:
    // !!! TODO: Add ability to update common parameters of location for whole group? !!!
    //           E.g. addPlanetaryPath24(VENUS); or updateTrueSunArrow3D();
    //           This is a lot of work and duplication, is there another way?
    //           Yes, expose an iterator so caller can do a for each loop.
    //           Or, accept a function of Location::member(...) type. Might only work for void return values.
    LocGroup(Earth* e, unsigned int identifier);
    ~LocGroup();
    void clear();
    unsigned int size() const;
    Location* addLocation(double lat, double lon, bool rad, float rsky);
    void removeLocation(Location* loc);
    void trimLocation();
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
        LLD sun = { };       // geocentric Sun Dec, GHA, Distance
        LLD localsun = { };  // topocentric Sun xEle, GHA, Distance
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
        LLD sun = { };       // geocentric Sun GHA, Dec, Distance
        LLD localsun = { };  // topocentric Sun Azi, Ele, Distance
        glm::vec3 sundir = NO_VEC3;        // Cartesian Sun direction in world coordinates
        void update(bool time, bool geometry);
        void draw();
        friend class Location;
    };
    class EleAziGrid {
    public:
        EleAziGrid(Location* location, double stepsize, float width, glm::vec4 color = LIGHT_PURPLE);
        void update(bool time, bool morph);
        void addElevationCircle(double ele, float width, glm::vec4 color);
        void addAzimuthCircle(double azi, float width, glm::vec4 color);
    private:
        double m_stepsize{ 15.0 * deg2rad };
        Location* m_location{ nullptr };
        std::vector<GenericPath*> azicircles;
        std::vector<GenericPath*> elecircles;
        friend class Location;
    };

    Location::TrueSun* truesun = nullptr;
    Location::FlatSun* flatsun = nullptr;
    Location::EleAziGrid* azielegrid = nullptr;
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
    std::array<sidyearparm, 9> sidyearparms // Move to config object ? Or Astronomy.h ? !!!
    { {                                     // Also check similar table in SolarSystem::m_planetinfos
        { 190.0, 1.0 },     // Mercury
        { 205.0, 1.0 },     // Venus
        { 250.0, 1.0 },     // Earth - parameters not tested, so might need tweaking
        { 300.0, 1.0 },     // Mars
        { 2150.0, 5.0 },    // Jupiter
        { 5500.0, 20.0 },   // Saturn
        { 15500.0, 50.0},   // Uranus
        { 60000.0, 200.0 }, // Neptune
        { 183.0, 1.0 }      // Sun
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
    LLD localsun = { };
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
    void moveLoc(double lat, double lon, bool rad = true);
    void storeLatLon(double lat, double lon);
    double getLat(bool rad = true);
    double getLon(bool rad = true);
    glm::vec3 getPosition();
    glm::vec3 getZenith();
    glm::vec3 getNorth();
    void moveToXYZ(glm::vec3 pos);
    void setTimeZone(const std::string& timezonename);

    // Calculations
    glm::vec3 calcEleAzi2Dir(LLD heading, bool rad = true);
    LLD calcDir2EleAzi(glm::vec3 direction, bool rad = true);
    glm::vec3 calcDecRA2Dir(double jd, double dec, double ra, bool rad = false);
    //glm::vec3 calcDirRADec(double ra, double dec, double jd = 0.0);
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

    void addEleAziGrid(double degrees = 15.0, bool radians = false, float width = 0.003f, glm::vec4 color = LIGHT_PURPLE);
    void addObserver(float bearing, glm::vec4 color = LIGHT_GREY, float height = 0.0f);
    void changeObserver(float bearing, glm::vec4 color = NO_COLOR, float height = 0.0f);
    void addArrow3DEleAzi(unsigned int unique, double ele, double azi, float length = 0.2f, float width = locazielewidth, glm::vec4 color = GREEN);
    void deleteArrow3DEleAzi(unsigned int unique);
    void changeArrow3DEleAzi(unsigned int unique, double ele, double azi, float length = 0.2f, float width = locazielewidth, glm::vec4 color = GREEN);
    void changeArrow3DEleAziColor(unsigned int unique, glm::vec4 color = GREEN);
    void updateArrow3DEleAzi(arrowcache& ar);

    void addArrow3DDecRA(unsigned int unique, double dec, double ra, glm::vec4 color, float width, float length);
    void updateArrow3DDecRA(arrowcache& ar);

    // Dots
    void addLocDot(float size = locdotsize, glm::vec4 color = DEFAULT_LOCDOT_COLOR);
    void changeLocDot(float size = -1.0f, glm::vec4 color = NO_COLOR);
    void deleteLocDot();
    void updateLocDot(dotcache& d);
    void addTruePlanetDot(Planet planet, float size, glm::vec4 color, bool checkit);
    void updateTruePlanetDot(dotcache& dc);

    // Arrows
    void addArrow3DFlatSun(float length = 0.2f, float width = locsunarrowwidth, glm::vec4 color = LIGHT_GREEN, bool checkit = false);
    void updateArrow3DFlatSun(arrowcache& ar);

    void addArrow3DTrueMoon(float length = 0.2f, float width = 0.003f, glm::vec4 color = glm::vec4(0.4f, 0.4f, 0.4f, 1.0f), bool checkit = false);
    void updateArrow3DTrueMoon(arrowcache& ar);

    void addArrow3DTruePlanet(Planet planet, float length = 0.2f, glm::vec4 color = NO_COLOR, bool checkit = false);
    void updateArrow3DTruePlanet(arrowcache& ar);

    // Lines
    void addLine3DFlatSun(float width = loclinewidth, glm::vec4 color = LIGHT_GREEN, bool checkit = true);
    //void changeLine3DFlatSun();
    //void deleteLine3DFlatSun();
    void updateLine3DFlatSun(cylindercache& l);
    // TODO: Add rays/lines to an arbitrary point to illustrate view cone angles, horizons, parallelity of incoming light rays etc. !!!

    // Paths
    void addPlanetaryPath(Planet planet, double startoffset, double endoffset, unsigned int steps, unsigned int type = ECGEO, glm::vec4 color = NO_COLOR, float width = 0.003f);
    void updatePlanetaryPath(polycache& pa);
    void deletePlanetaryPath(unsigned int type, size_t unique);

    void addPlanetTruePath24(Planet planet, glm::vec4 color = NO_COLOR, float width = locpathwidth);
    void deletePlanetTruePath24(Planet planet);
    void addPlanetTruePathSidYear(Planet planet, glm::vec4 color = NO_COLOR, float width = locpathwidth);
    void deletePlanetTruePathSidYear(Planet planet);

    void addPath3DFlatSun(glm::vec4 color = LIGHT_GREEN, float width = locsunpathwidth);
    void updatePath3DFlatSun(PolyCurve* path, glm::vec4 color = LIGHT_GREEN, float width = locsunpathwidth);
    void doPath3DFlatSun(PolyCurve* path);

    void addPath3DTrueMoon(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), float width = locpathwidth);
    void updatePath3DTrueMoon(polycache p);
    void doPath3DTrueMoon(PolyCurve* path);

    void addTrueLunalemma(glm::vec4 color = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f), float width = locpathwidth, float size = 0.005f);
    void updateTrueLunalemma(polycache p);
    void doTrueLunalemma(PolyCurve* path);

    void addPath3DDecRA(unsigned int unique, double dec, double ra, glm::vec4 color, float width);
    void updatePath3DRADec(polycache& pa);
    void doPath3DDecRA(double dec, double ra, PolyCurve* path);
};


// -------------
//  SolarSystem
// -------------
class SolarSystem {
public:
    struct distline {
        size_t index;
        Planet planet1;
        Planet planet2;
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
    void PlanetPos(Planet planet, bool update);
    glm::vec3 CalcPlanet(Planet planet, double jd = 0.0);
    void PlanetOrbit(Planet planet);
    void SunPos(bool update = false);
    void SunOrbit(bool update = true);
    glm::vec3 CalcSun(double jd = 0.0);
    void EarthPos(bool update = false);
    glm::vec3 CalcEarth(double jd = 0.0);
    glm::vec3 Ecliptic2Cartesian(double lat, double lon, double dst);
    glm::vec3 GetPlanetPos(Planet planet);
    void AddDistLine(Planet planet1, Planet planet2, glm::vec4 color, float width);
    void UpdateDistLines();
    void changeCentricity();
    void changeOrbits();
    void changeTrails();
    void changeTraillen();
};





// -----------
//  SkySphere
// -----------
typedef LLD(SkySphere::* calcFuncSS)(LLD, LLD, double, double, bool);
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
        double lat;  // !!! FIX: Use LLD ?
        double lon;
        double height;
        float size;
        size_t index;
    };
    struct pathcache {
        GenericPath* path = nullptr;
        float width = 0.0f;
        unsigned int type = NONE;
        glm::vec4 color = NO_COLOR;
        LLD lld1 = { };  // params specific to user func
        LLD lld2 = { };
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
    bool m_movable = true;
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
    //void loadStars();
    void genGeom();
    float getMagnitude2Radius(const double magnitude) const;
};




// -------
//  Earth
// -------
typedef LLD(Earth::* calcFunc)(LLD, LLD, double, double, bool);
class Earth {
public:
    struct Intersection {
        LLD point1 = { };
        LLD point2 = { };
    };
    struct polycache {               // PathTracker needs this, it only references path and path2
        PolyCurve* path = nullptr;
        PolyCurve* path2 = nullptr;
        GenericPath* gpath = nullptr;
        float width = 0.0f;
        unsigned int type = NONE;    // Allows Earth update() to call the right update function
        glm::vec4 color = NO_COLOR;
        LLD lld1 = { };
        LLD lld2 = { };
        double refraction = 0.0;
        double fend = 0.0;           // 1.0 or tau depending on calculation function
        calcFunc ca = nullptr;       // calculation function
        bool closed = false;
    };

    Scene* m_scene = nullptr;
    glm::vec3 SunLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 MoonLightDir = glm::vec3(0.0f, 1.0f, 0.0f);
    bool insolation = true;   // Solar insolation
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
        double lat; // LLD ?
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
    double m_jd_utc = 0.0;
    double m_jd_tt = 0.0;
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
    LLD subsolar = { };     // Subsolar point in Geocentric coordinates
    //LLD sublunar = { };     // Sublunar point in Geocentric coordinates
    glm::vec3 solarGP = glm::vec3(0.0f);  // Subsolar point in Cartesian coordinates
    glm::vec3 flatSun = glm::vec3(0.0f);  // Flat Sun in Cartesian coordinates (solarGP at m_flatsunheight / earthradius from GUI)
    Planetoid* m_subsolar = nullptr;
    //SubSolar* m_sunob = nullptr;
    size_t m_sundot = NO_UINT;
    size_t m_sunpole = NO_UINT;
    size_t m_suncone = NO_UINT;
    size_t m_ecliptic = NO_UINT;
    // As other astro paths are added, they should probably be colleced in a vector and updated in a unified update function
    CelestialPath* sunpath = nullptr;
    PolyCurve* suncurve = nullptr;
    float m_flatsunheight = 3000.0f;
    bool flatsundirty = false;
    Planetoid* m_sublunar = nullptr;
    glm::vec3 lunarGP = glm::vec3(0.0f);
    glm::vec3 flatMoon = glm::vec3(0.0f);
    SubLunar* m_moonob = nullptr;
    double m_moonJD = 0.0; // JD in TT when Moon was last calculated
    double m_moonDec = 0.0;
    double m_moonRA = 0.0;
    double m_moonHour = 0.0;
    double m_moonDist = 0.0;
    Cones* m_lunumbcone = nullptr;
    Shader* shdr;
    Shader* smshdr;
    Shader* sbshdr;
    Texture* daytex;
    Texture* nighttex;
    Texture* bumptex;
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
    // Geomtry calculations
    glm::vec3 getLoc3D(double rLat, double lon, double height = 0.0);
    glm::vec3 getNml3D(double lat, double lon, double height = 0.0);
    glm::vec3 getNorth3D(double lat, double lon, double height = 0.0);
    glm::vec3 getEast3D(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_XX(std::string mode, double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_XX(std::string mode, double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_NS(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_NS(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_AE(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_AE(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_LB(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_LB(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_LA(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_LA(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_MW(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_MW(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_ER(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_ER(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_RC(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_RC(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_EX(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_EX(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_EE(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_EE(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_E8(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_E8(double lat, double lon, double height = 0.0);
    glm::vec3 getLoc3D_E7(double lat, double lon, double height = 0.0);
    glm::vec3 getNml3D_E7(double lat, double lon, double height = 0.0);
    // Astronomy calculations
    LLD calcHADec2LatLon(LLD radec);
    glm::vec3 calcHADec2Dir(LLD radec);
    LLD calcRADec2LatLon(LLD radec, double jd_utc = NO_DOUBLE);
    LLD getXYZtoLLD_NS(glm::vec3 pos);
    LLD getXYZtoLLD_AE(glm::vec3 pos);
    double calcHorizonDip(double height);
    double calcSunSemiDiameter(double jd = NO_DOUBLE);
    double calcRefractionAlmanac(double elevation, double temperature, double pressure);
    double calcRefractionBennett(double elevation, double temperature, double pressure);
    double calcBrianLeakeDistance(const double elevation, bool rad = false);
    double calcBrianLeakeAngle(const double elevation, bool rad = false);
    Intersection calcSumnerIntersection(LLD lld1, LLD lld2, bool rad);
    Intersection calcSumnerIntersection(double lat1, double lon1, double rad1, double lat2, double lon2, double rad2, bool rad = true);
    LLD getSextantLoc(size_t index);
    LLD getSun(double jd = NO_DOUBLE);
    LLD getSubsolar(double jd = 0.0);
    LLD getPlanet(Planet planet, double jd = 0.0);
    LLD getMoon(double jd_utc = 0.0);
    void CalcMoon();
    LLD CalcMoonJD(double jd_utc);
    // Various primitives (dots, arrows, paths ...)
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
    void addUmbraCone();
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
    LLD getSublunarPoint();
    glm::vec3 getSublunarXYZ();
    void addSublunarPoint(float size = 0.03f);
    void deleteSublunarPoint();
    void updateSublunarPoint();
    SubStellarPoint* addSubStellarPoint(const std::string& name, bool lock = false, double jd = NO_DOUBLE, glm::vec4 color = NO_COLOR);
    void removeSubStellarPoint(size_t index);
    void addViewConeXYZ_NS(glm::vec3 pos, glm::vec4 color);
    void addViewConeLLD_NS(LLD loc, glm::vec4 color);
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
    size_t addGreatArc(LLD lld1, LLD lld2, glm::vec4 color, float width, bool rad);
    void removeGreatArc(size_t index);
    void changeGreatArc(size_t index, LLD lld1, LLD lld2, bool rad);
    void updateGreatArc(polycache& p);
    size_t addLerpArc(LLD lld1, LLD lld2, glm::vec4 color, float width, bool rad);
    size_t addFlatArc(LLD lld1, LLD lld2, glm::vec4 color, float width, bool rad);
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
    size_t addSumnerLine(LLD gp, double elevation, glm::vec4 color = SUNMERCOLOR, float width = sumnerlinewidth, bool rad = false);
    size_t addTissotIndicatrix(LLD location, double radius, bool rad = false, glm::vec4 color = LIGHT_ORANGE, float width = 0.005f);
    void removeTissotIndicatrix(size_t index);
    void updateTissotIndicatrix(TissotCache& tissot); // Geometry updates, no time dependence
    size_t addSemiTerminator(double radius, bool rad, glm::vec4 color, float width);
    void updateSemiTerminator(polycache& tissot);
    LLD calcGreatArc(LLD lld1, LLD lld2, double f, double refraction, bool rad = true);
    LLD calcLerpArc(LLD lld1, LLD lld2, double f, double refraction, bool rad = true);
    LLD calcTerminator(LLD lld1, LLD lld2, double param, double refang = 0.0, bool rad = true);
    LLD calcSemiTerminator(LLD lld1, LLD lld2, double param, double refang = 0.0, bool rad = true);
    double calcArcDist(LLD lld1, LLD lld2, bool rad);
    size_t addArc(LLD lld1, LLD lld2, glm::vec4 color, float width, bool rad, calcFunc ca, unsigned int type);
    void changeArc(size_t index, glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
    void deleteArc(size_t index);
    void updateCompositePath2(polycache& p);
    void addArcPoint(glm::vec3 ap, bool first, bool last, glm::vec3& oldap, PolyCurve* curve);
    void updateCompositePath(polycache& p);
private:
    void genGeom();
    void updGeom();
    void updateMorph();
    void updateSun();
};


// ==========================================================
//  Navigation Solvers
// ==========================================================

struct Intersection {
    LLD point1 = { };
    LLD point2 = { };
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
        void enable(Font* font, glm::vec4 color, float size);
        void update();
    private:
        glm::vec4 m_color = LIGHT_BLUE;
        float m_size = 0.0f;
        bool m_enabled = false;
        std::string m_name;
        Font* m_font = nullptr;
        TextBillBoard* m_billboard = nullptr;
        SubStellarPoint* m_ssp = nullptr;
        Name(SubStellarPoint& ssp, std::string starname);
        ~Name();
        friend class SubStellarPoint;
    };
    class Dot {
    public:
        void enable(glm::vec4 color = NO_COLOR, float size = NO_FLOAT);
        void disable();
        void update();
    private:
        glm::vec4 m_color = NO_COLOR;
        float m_size = locdotsize;
        SubStellarPoint* m_ssp = nullptr;
        Dots* m_dotsF = nullptr;
        size_t dotindex = 0;
        bool enabled = false;
        Dot(SubStellarPoint& ssp);
        ~Dot();
        friend class SubStellarPoint;
    };
    class SumnerLine {
        // Must use more complex heuristics, it may cross poles, or seam (TWICE!) !!!
        // Seam/pole crossing can only be determined after rotations of centre to GP
    public:
        void enable(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
        void disable();
        void update();
    private:
        SubStellarPoint* m_ssp = nullptr;
        bool enabled = true;
        glm::vec4 m_color = LIGHT_ORANGE;
        float m_width = pathwidth;
        PolyCurve* curve = nullptr;
        unsigned int steps = 360;
        SumnerLine(SubStellarPoint& ssp);
        ~SumnerLine();
        friend class SubStellarPoint;
    };
    class Radius {
        // Should this allow more than one radius line?
    public:
        void enable();
        void changeAzimuth(double bearing, bool rad);
        void changeAzimuthTo(LLD target, bool rad);
        void update();
    private:
        SubStellarPoint* m_ssp = nullptr;
        bool enabled = false;
        size_t m_radius = NO_UINT; // Earth Arc index
        double m_bearing = 0.0;    // Radians clockwise from north
        glm::vec4 m_color = NO_COLOR;
        Radius(SubStellarPoint& ssp);
        ~Radius();
        friend class SubStellarPoint;
    };
    glm::vec3 m_pos = glm::vec3(0.0f); // XYZ in World
    LLD m_loc = { };     // Lat, Lon
    LLD m_decra = { };   // Declination & right ascension Equatorial Earth Centered - kept in radians
    double m_dist_lat = 0.0;
    bool locked = false;               // Freeze in time, for sextant measurements etc
    std::string name;
    double m_elevation = 0.0;
    double m_rawElevation = 0.0;
    double m_indexError = 0.0; // Simply use zero rather than NO_DOUBLE, subtracting is faster than comparing and then subtracting
    double m_observerHeight = NO_DOUBLE;
    double m_temperature = NO_DOUBLE;
    double m_pressure = NO_DOUBLE;
    double m_jd = NO_DOUBLE; // JD in TT (Dynamical Time)
    glm::vec4 m_color = NO_COLOR;
    Earth* m_earth = nullptr;
    // Scene* m_scene = m_earth->m_scene;
    // Astronomy* m_astro = m_earth->m_scene->m_astro;
    SubStellarPoint::Dot* dot = nullptr;
    SubStellarPoint::SumnerLine* sumner = nullptr;
    SubStellarPoint::Name* nametag = nullptr;
    SubStellarPoint::Radius* radius = nullptr;
    SubStellarPoint(Earth& earth, const std::string& starname, const bool lock = false, const double jd_tt = NO_DOUBLE, const glm::vec4 color = NO_COLOR);
    ~SubStellarPoint();
    void shiftSpeedTime(double bearing, double knots, double minutes);
    void adjustElevation();
    void setElevation(double elevation, bool rad = false);
    void setIndexError(double indexError, bool rad = false);
    void setObserverHeight(double observerHeight);
    void setRefraction(double temperature, double pressure);
    LLD getDetails(bool rad);
    void update();
};


// -----------------
//  SubPoint Solver
// -----------------
class SubPointSolver {
    // Solves a fix for 3 observations using SubPoint - should evolve into n-point solver
    // 1) add points, 2) pull solution
public:
    SubPointSolver(Earth* earth);
    SubStellarPoint* addSubStellarPoint(const std::string starname, const double elevation, bool rad = false, const double jd_tt = NO_DOUBLE);
    void addSubStellarPoint(SubStellarPoint* ssp);
    // Perhaps add feature to show the star in the observable color, which is already available anyway !!!
    // - Best achieved by making the star color the default color in SubStellarPoint
    void showSumnerLines(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
    void showNames(Font* font, glm::vec4 color = NO_COLOR, float size = NO_FLOAT);
    void showDots(glm::vec4 color = NO_COLOR, float size = NO_FLOAT);
    void update();
    LLD calcLocation(bool rad);
    Intersection calcSumnerIntersection(LLD lld1, LLD lld2, bool rad);
    double calcArcDist(LLD lld1, LLD lld2, bool rad);
private:
    Earth* m_earth = nullptr;
    std::vector<SubStellarPoint*> m_ssps;
    Intersection isect12; // Need to be more generic, review after coding 3 point solution and preparing for n-point solution !!!
    Intersection isect23;
    Intersection isect31;
    LLD spoint1; // Solver points - see above for n-point refactor.
    LLD spoint2;
    LLD spoint3;
    LLD solution;
};


// --------------------
//  Three Point Solver
// --------------------
class ThreePointSolver {
    // Solves a fix for 3 observations using StellarSubPoint
    // 1) add points, 2) optionally set other parameters, 3) pull solution
public:
    ThreePointSolver(Earth* earth);
    SubStellarPoint* addSubStellarPoint(const std::string starname, const double elevation, bool rad = false, const double jd_tt = NO_DOUBLE);
    void addSubStellarPoint(SubStellarPoint* ssp);
    void setIndexError(double indexError, bool rad = false);
    void setObserverHeight(double observerHeight);
    void setRefraction(double temperature, double pressure);
    void showSumnerLines(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
    void showNames(Font* font, glm::vec4 color = NO_COLOR, float size = NO_FLOAT);
    void showDots(glm::vec4 color = NO_COLOR, float size = NO_FLOAT);
    void update();
    LLD calcLocation(bool rad);
    Intersection calcSumnerIntersection(LLD lld1, LLD lld2, bool rad);
    double calcArcDist(LLD lld1, LLD lld2, bool rad);
private:
    //double m_obsHeight = 0.0;    // meters
    //double m_temperature = 10.0; // Celsius
    //double m_pressure = 1010.0;  // mbar or hPa
    //double m_indexError = 0.0;   // radians
    Earth* m_earth = nullptr;
    std::vector<SubStellarPoint*> m_ssps;
    Intersection isect12;
    Intersection isect23;
    Intersection isect31;
    LLD spoint1;
    LLD spoint2;
    LLD spoint3;
    LLD solution;
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



// -------------
//  PathTracker
// -------------
// !!! FIX: Convert or update to (also?) use GenericPath !!!
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
    //   When LERPing, this can be difficult to account for. Maybe a flag/test of mode that indicates seam issues,
    //   and a count of where the seam is (pathxyz.size()-1)

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
    // std::vector<LLD>* pathlld = nullptr; // For paths in lat/lon, ele/azi, dec/RA
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