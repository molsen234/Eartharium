
#include <cmath>

#include <glm/gtx/rotate_vector.hpp>     // Rotation matrices for glm

#include "Earth.h"

#include "../../EAstronomy/aearth.h"
#include "../../EAstronomy/amoon.h"

// =========================
//  Location Specific Items
// =========================

// --------
//  LocDot
// --------
// LocationSO specific interface to Dots primitives
LocDot::LocDot(LocationSO* location) : SceneObject(location->bodygeometry->scene, location), location(location) {
    //  Create Dots primitive
    dot = location->bodygeometry->scene->getDotsFactory()->addXYZ(location->getPosition(), color, radius);
}
void LocDot::setColor(glm::vec4 color) {
    this->color = color;
    m_parent->scene->getDotsFactory()->changeDot(dot, color, radius);
}
void LocDot::setRadius(float radius) {
    this->radius = radius;
    m_parent->scene->getDotsFactory()->changeDot(dot, color, radius);
}
void LocDot::draw(Camera* cam) {
    //  Primitives are drawn in batch by renderer
    return;
}
bool LocDot::update() {  // Update the location
    m_parent->scene->getDotsFactory()->changeXYZ(dot, location->getPosition(), color, radius);
    return false;  // false = no worldmatrix built
}

// -----------
//  LocSundir
// -----------
// Length of arrow always follows LocationSO->getRadius()
LocSundir::LocSundir(LocationSO* location) : SceneObject(location->bodygeometry->scene, location), location(location) {
    //  Calculate parameters
    my_pos = location->getPosition();
    my_dir = location->getSundir();
    //  Create Arrows primitive
    arrow = location->bodygeometry->scene->getArrowsFactory()->addStartDirLen(my_pos, my_dir, location->getRadius(), 0.003f, color);
    //std::cout << "arrow: " << arrow << "\n";
}
void LocSundir::setColor(glm::vec4 color) {
    this->color = color;
    m_parent->scene->getArrowsFactory()->changeArrow(arrow, color);
}
void LocSundir::draw(Camera* cam) {
    //  Primitives are drawn in batch by renderer
    return;
}
bool LocSundir::update() {
    //std::cout << "LocSundir::update()\n";
    my_pos = location->getPosition();
    my_dir = location->getSundir();
    m_parent->scene->getArrowsFactory()->changeStartDirLen(arrow, my_pos, my_dir, location->getRadius(), 0.003f, color);
    return false;  // false = no worldmatrix built
}

// -----------
//  LocZenith
// -----------
LocZenithArrow::LocZenithArrow(LocationSO* location) : SceneObject(location->bodygeometry->scene, location), location(location) {
    //  Calculate parameters
    my_pos = location->getPosition();
    my_dir = location->getZenith();
    //  Create Arrows primitive
    arrow = location->bodygeometry->scene->getArrowsFactory()->addStartDirLen(my_pos, my_dir, location->getRadius(), 0.003f, color);
}
void LocZenithArrow::setColor(glm::vec4 color) {
    this->color = color;
    m_parent->scene->getArrowsFactory()->changeArrow(arrow, color);
}
//void LocDot::setRadius(float radius) {
//    this->radius = radius;
//    m_parent->scene->getDotsFactory()->changeDot(dot, color, radius);
//}
void LocZenithArrow::draw(Camera* cam) {
    //  Primitives are drawn in batch by renderer
    return;
}
bool LocZenithArrow::update() {
    my_pos = location->getPosition();
    my_dir = location->getZenith();
    m_parent->scene->getArrowsFactory()->changeStartDirLen(arrow, my_pos, my_dir, location->getRadius(), 0.003f, color);
    return false;  // false = no worldmatrix built
}

// --------------
//  HorizonPlane
// --------------
HorizonPlane::HorizonPlane(LocationSO* location, glm::vec4 color) : SceneObject(location->bodygeometry->scene, location), location(location), color(color) {
    plane = location->scene->getPlanesFactory()->addStartUV(location->getPosition(), location->getEast() * location->getRadius() * 1.1f, location->getNorth()*location->getRadius() * 1.1f, color);
}
void HorizonPlane::draw(Camera* cam) {
    //  Primitives are drawn in batch by renderer
    return;
}
bool HorizonPlane::update() {
    if (plane != NO_UINT) {
        location->scene->getPlanesFactory()->changeStartUV(plane, location->getPosition(), location->getEast() * location->getRadius() * 1.1f, location->getNorth() * location->getRadius() * 1.1f, color);
    }
    return false;
}

// ------------
//  LocationSO
// ------------
//  !!! FIX: location is always geographic coordinates for now. Planetographic or selenographic may differ in orientation.
LocationSO::LocationSO(BodyGeometry* body, LLD location) : SceneObject(body->scene, body), bodygeometry(body), loc(location) {
    // Initialize from body
}
LocationSO::~LocationSO() {
    if (locdot) delete locdot;
    if (truesundir) delete truesundir;
    if (zenitharrow) delete zenitharrow;
}
float LocationSO::getRadius() {
    return radius;
}
void LocationSO::setRadius(float radius) {
    this->radius = radius;
}
glm::vec3 LocationSO::getPosition() {
    return pos;
}
glm::vec3 LocationSO::getNorth() {
    return north;
}
glm::vec3 LocationSO::getEast() {
    return east;
}
glm::vec3 LocationSO::getZenith() {
    // !!! FIX: Can cache and check BodyGeometry::isUpdatedGeometry() (which is not yet implemented)
    return zenith;
}
glm::vec3 LocationSO::getSundir() {
    // !!! FIX: Here (and in calcEleAzi2Dir()) Azi is west of south. In EAstronomy it is east of north.
    LLD sun = scene->astro->getDecGHA(A_SUN);
    LLD localsun = scene->astro->calcGeo2Topo(sun, loc); // Sun Ele, Azi
    //std::cout << ACoord::formatDecRA(localsun) << '\n';
    localsun.dst = 1.0;
    glm::vec3 sundir = calcEleAzi2Dir(localsun);
    return sundir;
}
glm::vec3 LocationSO::calcEleAzi2Dir(LLD heading) {
    // !!! FIX: Here (and in getSundir()) Azi is west of south. In EAstronomy it is east of north.
    if (abs(pi2 - heading.lat) < tiny) return zenith;
    glm::vec3 dir = -north; // South
    dir = glm::rotate(dir, (float)-heading.lat, east);    // Elevation + from horizonal
    dir = glm::rotate(dir, (float)-heading.lon, zenith);  // Azimuth W of S
    return glm::normalize(dir);
}
LocDot* LocationSO::addLocDot(float size, glm::vec4 color) {
    if (!locdot) {  // Create if needed
        locdot = new LocDot{ this };
    }
    locdot->setColor(color);
    locdot->setRadius(size);
    return locdot;
}
LocSundir* LocationSO::addTrueSundir() {
    if (!truesundir) {  // Create if needed
        truesundir = new LocSundir{ this };
    }
    // Set params here added (color, width, ...)
    return truesundir;
}
LocZenithArrow* LocationSO::addZenithArrow() {
    if (!zenitharrow) {  // Create if needed
        zenitharrow = new LocZenithArrow{ this };
    }
    return zenitharrow;
}
HorizonPlane* LocationSO::addHorizonPlane(glm::vec4 color) {
    if (!horizonplane) {
        horizonplane = new HorizonPlane{ this, color };
    }
    return horizonplane;
}
void LocationSO::draw(Camera* cam) {
    // Nothing to draw here, the associated SceneObjects should draw themselves
    return;
}
bool LocationSO::update() {
    //std::cout << "LocationSO::update():\n";
    pos = bodygeometry->getLoc3D(loc);
    east = bodygeometry->getEast3D(loc);
    north = bodygeometry->getNorth3D(loc);
    zenith = glm::cross(east, north);

    return false;  // false = no worldmatrix built
}


// --------------
//  BodyGeometry
// --------------
BodyGeometry::BodyGeometry(Scene* scene, SceneObject* parent, unsigned int material, std::string mode, unsigned int meshU, unsigned int meshV, float gpuRadius)
    : SceneObject(scene, parent), m_mode(mode), radius(gpuRadius), m_meshU(meshU), m_meshV(meshV) {
    //std::cout << material << " ";
    vertices.reserve(((size_t)meshU + 1) * ((size_t)meshV + 1));
    tris.reserve((size_t)(meshU * (size_t)meshV * (size_t)sizeof(Tri)));

    setMode(mode);
    createShape();

    vbl = new VertexBufferLayout;
    vbl->Push<float>(3);   // Vertex pos
    vbl->Push<float>(3);   // Vertex normal NS (for insolation only)
    vbl->Push<float>(3);   // Vertex normal actual
    vbl->Push<float>(2);   // Texture coord
    vbl->Push<float>(4);   // Color (for overlay)
    va = new VertexArray;
    vb = new VertexBuffer(&vertices[0], (unsigned int)vertices.size() * sizeof(vertex));
    ib = new IndexBuffer((unsigned int*)&tris[0], (unsigned int)tris.size() * 3);
    va->AddBuffer(*vb, *vbl, true);

    // Fold these into a Material library - work in progress
    //shdr = m_scene->m_app->getShaderLib()->getShader(MOON_SHADER);
    //m_texture1 = m_scene->m_app->getTextureLib()->getTexture(MOON_FULL);
    shdr = scene->m_app->getShaderLib()->getShader(Materials[material].shader);
    m_texture1 = scene->m_app->getTextureLib()->getTexture(Materials[material].texture1);
    if (Materials[material].texture2 != NO_UINT) m_texture2 = scene->m_app->getTextureLib()->getTexture(Materials[material].texture2);
    if (Materials[material].texture3 != NO_UINT) m_texture3 = scene->m_app->getTextureLib()->getTexture(Materials[material].texture3);
    //std::cout << m_texture1 << '\n';
}
BodyGeometry::~BodyGeometry() {
    if (m_texture3 != nullptr) delete m_texture3;
    if (m_texture2 != nullptr) delete m_texture2;
    delete m_texture1;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
}
void BodyGeometry::addEquator() {
    equator = new Equator(scene, this, this);
    //equatorOb->setParent(this);
}
void BodyGeometry::addPrimeMeridian() {
    primem = new PrimeMeridian(scene, this, this);
    //primemOb->setParent(this);
}
void BodyGeometry::addGrid(double spacing) {
    grid = new Grid(scene, this, this);
    grid->setSpacing(spacing, false);
}

void BodyGeometry::setMode(const std::string mode) {
    dirty_geometry = true;
    std::string sMode = mode.substr(0, 2);
    std::string eMode = mode.substr(2, 2);
    // Could this be initialized in an array or vector? !!!
    if (sMode == "NS") {
        pos_mode1 = &BodyGeometry::getLoc3D_NS;
        nml_mode1 = &BodyGeometry::getNml3D_NS;
    }
    if (sMode == "AE") {
        pos_mode1 = &BodyGeometry::getLoc3D_AE;
        nml_mode1 = &BodyGeometry::getNml3D_AE;
    }
    if (sMode == "ER") {
        pos_mode1 = &BodyGeometry::getLoc3D_ER;
        nml_mode1 = &BodyGeometry::getNml3D_ER;
    }
    if (sMode == "GE") {
        pos_mode1 = &BodyGeometry::getLoc3D_GE;
        nml_mode1 = &BodyGeometry::getNml3D_GE;
    }
    // Other mode1 assignments here...
    if (pos_mode1 == nullptr) {
        std::cout << "ERROR: BodyGeometry::setMode(): Could not assign geometry function to start mode \"" << sMode << "\", it is unknown! Default: NS\n";
        pos_mode1 = &BodyGeometry::getLoc3D_NS;
        nml_mode1 = &BodyGeometry::getNml3D_NS;
    }
    if (eMode == "NS") {
        pos_mode2 = &BodyGeometry::getLoc3D_NS;
        nml_mode2 = &BodyGeometry::getNml3D_NS;
    }
    if (eMode == "AE") {
        pos_mode2 = &BodyGeometry::getLoc3D_AE;
        nml_mode2 = &BodyGeometry::getNml3D_AE;
    }
    if (eMode == "ER") {
        pos_mode2 = &BodyGeometry::getLoc3D_ER;
        nml_mode2 = &BodyGeometry::getNml3D_ER;
    }
    if (eMode == "GE") {
        pos_mode2 = &BodyGeometry::getLoc3D_GE;
        nml_mode2 = &BodyGeometry::getNml3D_GE;
    }
    // Other mode2 assignments here...
    if (eMode == "") {
        pos_mode2 = nullptr;
        nml_mode2 = nullptr;
    }
    else if (pos_mode2 == nullptr) {
        std::cout << "ERROR: BodyGeometry::setMode(): Could not assign geometry function to end mode \"" << eMode << "\", it is unknown! Default: AE\n";
        pos_mode2 = &BodyGeometry::getLoc3D_AE;
        nml_mode2 = &BodyGeometry::getNml3D_AE;
    }
}
std::string BodyGeometry::getMode() const {
    return m_mode;
}
void BodyGeometry::setMorphParameter(float parameter) {
    //std::cout << "Morph updated!\n";
    morph_param = parameter;
    dirty_geometry = true;
}
float BodyGeometry::getMorphParameter() const {
    return morph_param;
}
void BodyGeometry::setEllipoid(Ellipsoid_2axis ellipsoid) {
    this->ellipsoid = ellipsoid;
    dirty_geometry = true;
    updateShape();
}

LLD BodyGeometry::toLocalCoords(LLD loc) {
    // Local coords are the same as BodyGeometry native
    return loc;
}
LLD BodyGeometry::fromLocalCoords(LLD loc) {
    // Local coords are the same as BodyGeometry native
    return loc;
}
glm::vec3 BodyGeometry::getLoc3D(const LLD loc) {
    /// <summary>
    /// Takes latitude and longitude in radians to glm::vec3 Cartesian coordinate in world/object space
    /// </summary>
    /// <param name="rLat">Latitude given in -pi/2 to pi/2 radians</param>
    /// <param name="rLon">Longitude given in -pi to pi radians from Greenwich meridian (east of south)</param>
    /// <param name="height">Optional height above geoid surface in km, defaults to 0.0</param>
    /// <param name="rad">Optional flag to indicate if angles are in radians, defaults to false</param>
    /// <returns>Cartesian coordinates glm::vec3 in world space units for currently active geoid geometry</returns>
    LLD myloc = toLocalCoords(loc);
    if (pos_mode2 == nullptr) return (this->*pos_mode1)(myloc);
    return morph_param * (this->*pos_mode2)(myloc) + (1.0f - morph_param) * (this->*pos_mode1)(myloc);
}
glm::vec3 BodyGeometry::getNml3D(const LLD loc) {
    LLD myloc = toLocalCoords(loc);
    if (nml_mode2 == nullptr) return (this->*nml_mode1)(myloc);
    return glm::normalize(morph_param * (this->*nml_mode2)(myloc) + (1.0f - morph_param) * (this->*nml_mode1)(myloc));
}
glm::vec3 BodyGeometry::getLoc3D_NS(const LLD loc) {
    double lat = loc.lat, lon = loc.lon;
    double h = (double)radius + loc.dst; // Height above center in world coordinates
    float w = (float)(cos(lat) * h);
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    float z = (float)(sin(lat) * h);
    return glm::vec3(x, y, z);
}
glm::vec3 BodyGeometry::getNml3D_NS(const LLD loc) {
    //return glm::normalize(getLoc3D_NS(rLat, rLon));
    double lat = loc.lat, lon = loc.lon; // To keep const promise
    float w = (float)(cos(lat));                     // Slightly simplified, as radius and height are irrelevant on a sphere
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    float z = (float)(sin(lat));
    return glm::normalize(glm::vec3(x, y, z));
}
glm::vec3 BodyGeometry::getLoc3D_AE(const LLD loc) {
    double lat = loc.lat, lon = loc.lon;
    // NOTE: This choice of w gives an equator equal to that of a spherical Earth !!!
    //       Should really use a scale where north pole to equator distance matches
    double w = (pi2 - lat) * (double)radius / pi2;
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    return glm::vec3(x, y, loc.dst);
}
glm::vec3 BodyGeometry::getNml3D_AE(const LLD loc) {
    return glm::vec3(0.0f, 0.0f, 1.0f);
}
glm::vec3 BodyGeometry::getLoc3D_ER(const LLD loc) {
    double lat = loc.lat, lon = loc.lon;
    // Is a bounds check needed here?
    if (lat > pi2 + tiny || lat < -pi2 - tiny) {
        //std::cout << "WARNING: Earth2::getLoc3D_ER(): lat is out of range: " << lat << "\n";
    }
    if (lon > pi + tiny || lon < -pi - tiny) {
        //std::cout << "WARNING: Earth2::getLoc3D_ER(): lon is out of range: " << lon << "\n";
    }
    // The simple scale used *does* preserve the north pole to equator distance from a sphere, since on a unit sphere that distance is pi/2.
    return glm::vec3(loc.dst, lon * radius, lat * radius);
}
glm::vec3 BodyGeometry::getNml3D_ER(const LLD loc) {
    return glm::vec3(1.0f, 0.0f, 0.0f);
}
glm::vec3 BodyGeometry::getLoc3D_GE(const LLD loc) {
    // !!! FIX: ignores loc.dst for now

    // !!! FIX: Why does the North pole sink into the ellipsoid when as eccentricity increases? Must be due to RhoSinPhiRrime()
    double lat = loc.lat, lon = loc.lon;
    if (lat > pi2 - tiny) lat = pi2 - tiny;
    // w formula from AEllipsoid::RadiusOfParallelOfLatitude()
    //double sinGeo{ sin(lat) };
    //float w = (float)(/* semimajor = 1.0 here */ cos(lat) / (sqrt(1.0 - (ellipsoid.eccentricity * ellipsoid.eccentricity * sinGeo * sinGeo))));
    // w formula from AEllipsoid::RhoCosPhiPrime() with Height = 0.0
    float w = (float)cos(atan((1.0 - ellipsoid.flattening) * tan(lat)));
    float x = ((float)cos(lon) * w);
    float y = ((float)sin(lon) * w);
    float z = (float)(AEllipsoid::RhoSinPhiPrime(lat, 0.0, ellipsoid));
    return glm::vec3(x, y, z);
}
glm::vec3 BodyGeometry::getNml3D_GE(const LLD loc) {
    double lat = loc.lat, lon = loc.lon; // To keep const promise
    //float w = (float)cos(atan((1.0 - ellipsoid.flattening) * tan(lat)));
    float w = (float)(cos(lat));
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    float z = (float)(sin(lat));
    return glm::normalize(glm::vec3(x, y, z));
}
glm::vec3 BodyGeometry::getNorth3D(LLD loc) {
    // NOTE: If rLat = pi/2 or -pi/2 (at poles) north is not defined!!
    glm::vec3 l1 = getLoc3D({ loc.lat + NE_SMEAR, loc.lon, 0.0 });
    glm::vec3 l2 = getLoc3D({ loc.lat - NE_SMEAR, loc.lon, 0.0 });
    return glm::normalize(l1 - l2);
}
glm::vec3 BodyGeometry::getEast3D(LLD loc) {
    // NOTE: If rLat = pi/2 or -pi/2 (at poles) east is not defined!!
    glm::vec3 l1 = getLoc3D({ loc.lat, loc.lon + NE_SMEAR, 0.0 });
    glm::vec3 l2 = getLoc3D({ loc.lat, loc.lon - NE_SMEAR, 0.0 });
    return glm::normalize(l1 - l2);
}

// FIX !!! Add additional geometries here !!!
// Note: Earth IAU 76 shape referenced in Meeus 1992 chapter 11.
//void draw(Camera* cam); // draw() and update() should be supplied by SceneObject, they relate to the rendering.
//void update() override;
// createShape() might get tri and vert vectors if rendering is done in SceneObject or elsewhere - decide in constructor
void BodyGeometry::createShape() {    // Creates the mesh geometry, should probably only be called once at creation
    // Build mesh from bottom left to top right, meridian by meridian
    LLD latlon{};
    for (unsigned int v = 0; v <= m_meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
        latlon.lat = (pi * v / m_meshV) - pi2;
        for (unsigned int u = 0; u <= m_meshU; u++) {
            latlon.lon = (tau * u / m_meshU) - pi;
            //lon = (pi * u / m_meshU) - pi;
            glm::vec3 loc = getLoc3D(latlon);     // position of vertex
            glm::vec3 nms = getNml3D_NS(latlon);  // Insolation normal, always NS (or G8?)
            glm::vec3 nml = getNml3D(latlon);     // Geometry normal, for camera lighting
            vertices.push_back({ loc, nms, nml, glm::vec2(u / (float)(m_meshU) + texoffset_x / 8192.0f, v / (float)m_meshV) + texoffset_y / 4096.0f, BLACK });
            if (u < m_meshU && v < m_meshV) {
                tris.push_back({ (m_meshU + 1) * v + u , (m_meshU + 1) * (v + 1) + u, (m_meshU + 1) * (v + 1) + 1 + u });
                tris.push_back({ (m_meshU + 1) * (v + 1) + 1 + u, (m_meshU + 1) * v + u + 1, (m_meshU + 1) * v + u });
            }
        }
    }
}
void BodyGeometry::updateShape() {    // Updates the mesh geometry, called when tex offset, mode or morph param change
    if (!dirty_geometry) return;
    unsigned int i = 0;
    LLD latlon;
    // Update mesh from bottom left to top right, meridian by meridian
    for (unsigned int v = 0; v <= m_meshV; v++) {
        latlon.lat = (pi * v / m_meshV) - pi2;
        for (unsigned int u = 0; u <= m_meshU; u++) {
            latlon.lon = (tau * u / m_meshU) - pi;
            //lon = (pi * u / m_meshU) - pi;
            vertices[i].position = getLoc3D(latlon);
            vertices[i].surface_normal = getNml3D(latlon);
            // NOTE: This is only partial separation between geometry and insolation, maybe a more general approach will be good
            //       e.g. to show how wrong it looks to have flat Earth insolation on a globe, etc.
            //if (do_eccentriclight) vertices[i].normal_i = getNml3D_EE(lat, lon, 0.0f);
            //if (tropicsoverlay && abs(lat) < 24.0 * deg2rad) vertices[i].color = tropics;
            //else if (arcticsoverlay && abs(lat) > (90.0 - 23.4) * deg2rad) vertices[i].color = arctics;
            //else vertices[i].color = BLACK;
            i++;
        }
    }
    vb->LoadData(&vertices[0], (unsigned int)vertices.size() * sizeof(vertex));

    // Now update children
    if (equator != nullptr) equator->generate();
    if (primem != nullptr) primem->generate();
    if (grid != nullptr) grid->update();
    dirty_geometry = false;
}
void BodyGeometry::draw(Camera* cam) {
    // !!! FIX: Shader and Textures should be a Material !!!
    //          When fixing, add support for single channel and float value textures
    if (hidden) return; // Don't draw anything if this object is marked as hidden
    shdr->Bind();
    // Maybe add a matching cleanup if user allocates and binds special GPU resources?
    bool drawn = drawSpecific(cam, shdr);  // Note DetailedSky calls SkyDots::draw() which uses a different shader. So call out before setting uniforms.
    glm::mat4 pv = cam->getProjMat() * cam->getViewMat();
    shdr->SetUniformMatrix4f("projview", pv);
    shdr->SetUniformMatrix4f("world", worldmatrix);
    if (m_texture1 != nullptr) shdr->SetUniform1i("texture1", m_texture1->GetTextureSlot());
    if (m_texture2 != nullptr) shdr->SetUniform1i("texture2", m_texture2->GetTextureSlot());
    if (m_texture3 != nullptr) shdr->SetUniform1i("texture3", m_texture3->GetTextureSlot());
    va->Bind();
    ib->Bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Note: For GL_FRONT_AND_BACK, also do glDisable(GL_CULL_FACE)
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default mode is GL_FRONT_AND_BACK, GL_FILL
    //glFrontFace(GL_CCW);
    glDisable(GL_CULL_FACE); // It is irritating that Earth disappears when seen from back-/in-side
    if (!drawn) glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0);
    glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CW);
    // Cleanup
    va->Unbind();
    ib->Unbind();
    shdr->Unbind();
}
bool BodyGeometry::drawSpecific(Camera* cam, Shader* shdr) { return false; }


// ------------
//  Smart Path
// ------------
SmartPath::SmartPath(Scene* scene, SceneObject* parent, float width, glm::vec4 color)
    : SceneObject(scene, parent) {
    if (scene == nullptr) return;
    if (color == NO_COLOR) color = GREEN;
    if (width == NO_FLOAT) width = 0.005f;
    name = "SmartPath";
    m_color = color;
    m_width = width;
    m_curves.emplace_back(new PolyCurveSO(scene, this, m_color, m_width, NO_UINT));
    m_curve = 0;
}
SmartPath::~SmartPath() {
    m_curves.clear();
}
void SmartPath::setColor(glm::vec4 color) {
    m_color = color;
    for (auto p : m_curves) {
        p->changePolyCurve(m_color);
    }
}
void SmartPath::setWidth(float width) {
    m_width = width;
    for (auto p : m_curves) {
        p->changePolyCurve(NO_COLOR, m_width);
    }
}
void SmartPath::addPoint(glm::vec3 point) {
    m_curves[m_curve]->addPoint(point);
}
void SmartPath::addSplit(glm::vec3 point1, glm::vec3 point2) {
    // point1 is last point in current PolyCurve, point2 is first point in next PolyCurve
    m_curves[m_curve]->addPoint(point1);
    if (m_curves.size() - 1 <= m_curve) {
        m_curves.emplace_back(new PolyCurveSO(scene, this, m_color, m_width, NO_UINT));
    }
    m_curve++;
    m_curves[m_curve]->addPoint(point2);
}
void SmartPath::clearPoints() {
    // Don't delete PolyCurve objects, reuse them as needed. Empty PolyCurve will be skipped in PolyCurve::draw()
    for (auto& c : m_curves) {
        c->clearPoints();
    }
    m_curve = 0;
}
void SmartPath::generate() {
    for (auto& c : m_curves) {
        c->generate();
    }
}
void SmartPath::draw(Camera* cam) {
    //std::cout << "SmartPath::draw()\n";
} // SceneObject children need a draw() but here the PolyCurveSO takes care of drawing independently


// ----------
//  Meridian   (half Great Circle of Longitude)
// ----------
Longitude::Longitude(Scene* scene, SceneObject* parent, BodyGeometry* geometry, double lon, float width, glm::vec4 color)
    : SceneObject(scene, parent) {
    name = "Longitude " + std::to_string(rad2deg * lon);
    this->lon = lon;
    locref = geometry;
    path = new SmartPath{ scene, this, width, color };
    //path->setParent(this);
    generate();
}
Longitude::~Longitude() {
    m_parent->removeChild(this);
    delete path;
}
void Longitude::setColor(glm::vec4 color) {
    path->setColor(color);
}
void Longitude::setWidth(float width) {
    path->setWidth(width);
}
void Longitude::generate() {
    path->clearPoints();
    LLD latlon{};
    glm::vec3 pos{ 0.0f };
    // A longitude does not encounter an edge in any geometries, so SmartPath will have no splits
    latlon.lon = lon;
    latlon.dst = 0.0;
    for (double lat = -pi2; lat <= pi2+verytiny; lat += deg2rad) {
        latlon.lat = lat;
        pos = (locref->*locpos)(latlon); // Dynamically calls earth->getLoc3D() (Earth2::getLoc3D())
        path->addPoint(pos);
    }
}
bool Longitude::update() { return false; }
void Longitude::draw(Camera* cam) {}


// ----------
//  Parallel   (Circle of Latitude)
// ----------
Latitude::Latitude(Scene* scene, SceneObject* parent, BodyGeometry* geometry, double lat, float width, glm::vec4 color)
    : SceneObject(scene, parent) {
    name = "Latitude " + std::to_string(rad2deg * lat);
    this->lat = lat;
    locref = geometry;
    path = new SmartPath{ scene, this, width, color };
    //path->setParent(this);
    generate();
}
Latitude::~Latitude() {
    // !!! FIX: Also remove own children, or figure out how to deal with them !!!
    //          For now Latitude doesn't have children. Yes, SmartPath is now a child of Latitude!
    //          Can this be handled better in SceneObject rather than all the derived objects?
    // !!! FIX: Can only be drawn on closed geometric surfaces (NS, E8, etc) not open surfaces (ER, AE etc)
    //          depending on the coordinate system orientation. Think about ecliptic coordinates on AE or ER sky.
    //          See Earth::
    m_parent->removeChild(this);
    delete path;
}
void Latitude::setColor(glm::vec4 color) {
    path->setColor(color);
}
void Latitude::setWidth(float width) {
    path->setWidth(width);
}
void Latitude::generate() {
    glm::vec3 pos{ 0.0f };
    path->clearPoints();
    // FIX: Might run to < pi instead of <= pi, and add starting point to end (lat, -pi) to ensure closed path
    // UPD: Latitude is not a closed path on some geometries (e.g. ER)
    LLD latlon{};
    latlon.lat = lat;
    latlon.dst = 0.0;
    for (double lon = -pi; lon <= pi; lon += deg2rad) {
        latlon.lon = lon;
        pos = (locref->*locpos)(latlon);
        path->addPoint(pos);
    }
}
bool Latitude::update() { return false; }  // Let parent call generate() when update is needed
void Latitude::draw(Camera* cam) {}


// ----------------
//  Prime Meridian
// ----------------
PrimeMeridian::PrimeMeridian(Scene* scene, SceneObject* parent, BodyGeometry* geometry, float width, glm::vec4 color)
    : Longitude(scene, parent, geometry, 0.0, width, color) {
    name = "Prime Meridian";
}


// ---------
//  Equator
// ---------
Equator::Equator(Scene* scene, SceneObject* parent, BodyGeometry* geometry, float width, glm::vec4 color)
    : Latitude(scene, parent, geometry, 0.0, width, color) {
    name = "Equator";
}

// NOTES ON THESE ARCS
// It would be nice if GenericPath is a SceneObject so transforms can be applied in shaders.
// Unlike Dots for example, PolyPath are drawn individually. So a transform for each should be doable.

// ------
//  Grid
// ------
Grid::Grid(Scene* scene, SceneObject* parent, BodyGeometry* geometry, std::string objname)
    : SceneObject(scene, parent) {
    // !!! FIX: To be generic, it should take a location function so it can be used for EleAzi or DecRA in Location and LatLon in Earth !!!
    //          Or rather, the geometry passed in should be usable for Location too. This may require more consideration...
    // Upd: Grid will by default be aligned with the parent as a regular lat/lon system on Earth, using worldmatrix to follow parent.
    //      SceneObject parameters (position, rotation, scale) can be used to tilt (and flip) it if desired.
    //      All the astronomical systems count vertical coordinates from equator, but horizontal coordinates (RA vs Lon) may require
    //      more than the standard orientation parameters. Flipping & rotating by 180 degrees, and accepting -360 to 360 degrees may be a solution.
    //      But the parameters are applied to worldmatrix, not to identity, so hmm...
    //      Probably better to supply coordinate orientation to Grid instead, or a select conversion function ala getLoc, but getSpherical or something.
    // For now, get it working for lat/lon surface coordinates, and worry about the rest afterwards.
    // Must be possible to scale radius independently of the parent object!
    // (scenario: DetailedEarth with radius 1.0, DetailedSky with radius 3.0, attach Grid to DetailedSky but with radius 1.0
    //  then scale it up to radius 3.0, to illustrate how the Earth equatorial coordinates define the Celestial ones.
    //  then adjust the rotation to anchor at the March Equinox rather than Greenwich)

    // Does this need to support separate Latitude and Longitude color and width?
    name = objname;
    if (objname == "NO_NAME") name = "Graticule";
    m_geometry = geometry;
    //m_parent = geometry;
    //m_parent->addChild(this);

    // No default "lines", so if user wants only Equator and PrimeMeridian, they can still use Grid

    //equator = new Equator{ scene, geometry, 0.007f };
    //equator->setParent(this);
    //primemeridian = new PrimeMeridian{ scene, geometry, 0.007f };
    //primemeridian->setParent(this);
}
Grid::~Grid() {
    m_parent->removeChild(this);
    delete equator;
    delete primemeridian;
    for (auto lat : latitudes) delete lat;
    for (auto lon : longitudes) delete lon;
}
void Grid::setName(std::string objname) {
    name = objname;
}
void Grid::setColor(glm::vec4 color, bool skip_pm_eq) {
    this->color = color;
    for (auto lat : latitudes) { lat->setColor(color); }
    for (auto lon : longitudes) { lon->setColor(color); }
    if (!skip_pm_eq) {
        equator->setColor(color);
        primemeridian->setColor(color);
    }
}
void Grid::setWidth(float width, bool skip_pm_eq) {
    this->width = width;
    for (auto lat : latitudes) { lat->setWidth(width); }
    for (auto lon : longitudes) { lon->setWidth(width); }
    if (!skip_pm_eq) {
        equator->setWidth(width);
        primemeridian->setWidth(width);
    }
}
// Maybe at Latitudes and Longitudes separately
void Grid::setSpacing(double spacing, bool rad) {
    // Sets the number of degrees (or radians if rad = true) between each graticule line
    if (!rad) spacing *= deg2rad;
    // First clear existing Lat and Lon objects
    for (double lat = -pi2; lat <= pi2; lat += spacing) {
        if (abs(lat) > tiny) {
            latitudes.emplace_back(new Latitude(scene, this, m_geometry, lat));
            //latitudes.back()->setParent(this);
        }
    }
    for (double lon = -pi; lon <= pi; lon += spacing) {
        if (abs(lon) > tiny) {
            longitudes.emplace_back(new Longitude(scene, this, m_geometry, lon));
            //longitudes.back()->setParent(this);
        }
    }
}
void Grid::setSpacing(unsigned int divisions) {
    double spacing = tau / (double)divisions;
    // Code duplication from other setSpacing() above. Refactor? Yes, if I add separate Latitude and Longitude generators later.
    for (double lat = -pi2; lat <= pi2; lat += spacing) {
        if (abs(lat) > tiny) {
            latitudes.emplace_back(new Latitude(scene, this, m_geometry, lat));
            //latitudes.back()->setParent(this);
        }
    }
    for (double lon = -pi; lon <= pi; lon += spacing) {
        if (abs(lon) > tiny) {
            longitudes.emplace_back(new Longitude(scene, this, m_geometry, lon));
            //longitudes.back()->setParent(this);
        }
    }
}
Latitude* Grid::addLatitude(double lat) {
    return latitudes.emplace_back(new Latitude(scene, this, m_geometry, lat));
    //return latitudes.back();
}
Longitude* Grid::addLongitude(double lon) {
    return longitudes.emplace_back(new Longitude(scene, this, m_geometry, lon));
    //return longitudes.back();
}
void Grid::clear() {
    latitudes.clear();
    longitudes.clear();
}
bool Grid::update() {
    for (auto lat : latitudes) { lat->generate(); }
    for (auto lon : longitudes) { lon->generate(); }
    if (equator != nullptr) equator->generate();
    if (primemeridian != nullptr) primemeridian->generate();
    return false;
}
void Grid::draw(Camera* cam) {}


// --------------
//  Small Circle
// --------------
//  Used for Tissot markers, Circle of equal altitude, etc
//  FIX: On open surfaces, SmallCircleArcs can cross a seam or pole more than once (twice it seems).
SmallCircle::SmallCircle(Scene* scene, SceneObject* parent, BodyGeometry* geometry, LLD location, double radius, float width, glm::vec4 color)
    : SceneObject(scene, parent), location(location), radius(radius) {
    // width and color are optional
    name = "Small Circle";
    //m_scene = scene;
    this->color = color;
    locref = geometry; // Using this now, so it can be updated to geometry later, and not be Earth2 specific
    path = new GenericPath{ scene,width,color }; // Use Scene to generate !!! Well, GenericPath uses Scene to create PolyCurve(s).
    generate();
}
SmallCircle::~SmallCircle() {
    // !!! FIX: child removal should probably be done in SceneObject !!!
    m_parent->removeChild(this);
    delete path;
}
// A radius of r degrees on the surface of a sphere with radius R has a chord (flat) radius of  R*sin(r).
// 
void SmallCircle::generate() { // Generate by drawing circle around north pole and rotating into place by lat/lon
    glm::vec3 pos{ 0.0f };
    double const stepsize = tau / 360.0; // deg2rad;
    double const zangle = location.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
    double const yangle = location.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
    double const cy = cos(yangle);
    double const sy = sin(yangle);
    double const cz = cos(zangle);
    double const sz = sin(zangle);
    double const lz = sin(pi2 - radius);
    LLD latlon{};
    latlon.dst = location.dst;
    for (double angle = -pi; angle <= pi; angle += stepsize) {
        double lx = cos(pi2 - radius) * cos(angle);
        double ly = cos(pi2 - radius) * sin(angle);
        double l2x = lx * cy + lz * sy;
        double l3x = l2x * cz - ly * sz;
        double l3y = l2x * sz + ly * cz;
        latlon.lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
        latlon.lon = atan2(l3y, l3x);
        pos = (locref->*locpos)(latlon); // Dynamically calls earth->getLoc3D() (Earth2::getLoc3D())
        path->addPoint(pos);
    }
    path->generate();
}
bool SmallCircle::update() {
    path->clearPoints();
    generate();
    return false;
}
void SmallCircle::draw(Camera* cam) {}


// ------------------
//  Great Circle Arc
// ------------------
// Can not draw on open geometries like ER AE, only closed surfaces.
// See: Earth::updateCompositePath2 for possible solution
GreatCircleArc::GreatCircleArc(Scene* scene, SceneObject* parent, BodyGeometry* geometry, LLD start, LLD end, double steps, float width, glm::vec4 color)
    : SceneObject(scene, parent), start_point(start), end_point(end), steps(steps) {
    // To work on open surfaces (AE, ER, etc), maybe break arc up into sub arcs where it crosses boundaries/poles.
    // Should be done in constructor, and generate() would re-generate multiple arcs based on stored endpoints.
    // NOTE: GreatCircleArcs are special in never being longer than 180 degrees, so they cannot cross both poles.
    //       Likewise, they can only cross the dateline seam once.
    //       If they cross a pole, they must be a meridian section (due to being part of a great circle), and thus never cross the seam.
    //       So they will never split into more than two segments, i.e. have only one split point.
    //       When max(lata,latb) - min(lata,latb) = 180 degrees, the split point is the nearest pole (north if (lata+latb)/2 is positive).
    //       (south if negative, making an arc from equator to equator always pass the north pole, eliminating pole fighting)
    //       When max(lona,lonb) - min(lona,lonb) > 180, the arc crosses the dateline seam.
    //       Is interpolation required to determine the split point? Apparently not: https://math.stackexchange.com/questions/402799/intersection-of-two-arcs-on-sphere
    name = "Circle Arc";
    this->color = color;
    locref = geometry;
    path = new GenericPath{ scene,width,color };
    findSplitPoint();
    generate();
}
GreatCircleArc::~GreatCircleArc() {
    m_parent->removeChild(this);
    delete path;
}
void GreatCircleArc::setStart(LLD start) {
    start_point = start;
    // generate()  // already being called from update() every frame (via SceneTree)
}
void GreatCircleArc::setEnd(LLD end) {
    end_point = end;
}
void GreatCircleArc::setStartEnd(LLD start, LLD end) {
    start_point = start;
    end_point = end;
}
LLD GreatCircleArc::calcGreatArc(const LLD lld1, const LLD lld2, const double f) {
    double lat1 = lld1.lat;
    double lon1 = lld1.lon;
    double lat2 = lld2.lat;
    double lon2 = lld2.lon;
    LLD ret;
    // - d does not depend on f, so could be calculated once per path instead of once per point on the path
    // - Could also put cos(lld1.lat) and cos(lld2.lat) into stack variables, for possible speedup.
    //   Test this with a few hundred GreatCircleArc instances.
    double d = acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lon1 - lon2));
    double A = sin((1 - f) * d) / sin(d);
    double B = sin(f * d) / sin(d);
    double x = A * cos(lat1) * cos(lon1) + B * cos(lat2) * cos(lon2);
    double y = A * cos(lat1) * sin(lon1) + B * cos(lat2) * sin(lon2);
    double z = A * sin(lat1) + B * sin(lat2);
    ret.lat = atan2(z, sqrt(x * x + y * y));
    ret.lon = atan2(y, x);
    // On the poles longitude is undefined, setting to 0.0. Except, it ruins paths passing straight over the pole. !!!
    //if (abs(ret.lat) == pi2) ret.lon = 0.0;
    return ret;
}
void GreatCircleArc::findSplitPoint() {
    // Check for pole crossing
    
    double det = std::max(start_point.lon, end_point.lon) - std::min(start_point.lon, end_point.lon);
    if (std::abs(det - pi) < tiny) {
        std::cout << "GreatCircleArc::findSplitPoint(): Crossing Pole! det=" << det - pi << '\n';
        // The below might not work, see comments about poles in GreatCircleArc::calcGreatArc()
        split_point.lon = (end_point.lon - start_point.lon) / 2;
        split_point.dst = 0.0;
        if (det < 0.0) split_point.lat = -pi2;  // south pole
        else split_point.lat = pi2;             // north pole
        return;
    }
    // Check for dateline seam crossing

    // Indicate no split
    split_point = end_point;
}

void GreatCircleArc::generate() {
    glm::vec3 pos{ 0.0f };
    LLD loc{ };
    //std::cout << start_point << " " << split_point << " " << end_point << '\n';
    if (split_point == end_point) { // There is no split
        for (double t = 0.0; t <= 1.0; t += 1.0 / steps) {
            loc = calcGreatArc(start_point, end_point, t); // For now, start & end are in degrees
            pos = (locref->*locpos)(loc); // Dynamically calls getLoc3D() of the geometry body
            path->addPoint(pos);
        }
    } else {
        // run start to split, then split path, then split to end
        // points spaced according to arc length?
        for (double t = 0.0; t < 1.0; t += 1.0 / steps) {
            loc = calcGreatArc(start_point, split_point, t); // For now, start & end are in degrees
            pos = (locref->*locpos)(loc); // Dynamically calls getLoc3D() of the geometry body
            path->addPoint(pos);
        }
        path->addSplit(
            (locref->*locpos)(calcGreatArc(start_point, split_point, 1.0)),
            (locref->*locpos)(calcGreatArc(split_point, end_point, 0.0))
        );
        for (double t = 0.0 + (1.0 / steps); t < 1.0 + tiny; t += 1.0 / steps) {
            loc = calcGreatArc(split_point, end_point, t); // For now, start & end are in degrees
            pos = (locref->*locpos)(loc); // Dynamically calls getLoc3D() of the geometry body
            path->addPoint(pos);
        }
    }
    path->generate();
}
bool GreatCircleArc::update() {
    path->clearPoints();
    generate();
    return false;
}
void GreatCircleArc::draw(Camera* cam) {}



// -----------
//  Planetoid
// -----------
// https://en.wikipedia.org/wiki/List_of_gravitationally_rounded_objects_of_the_Solar_System#Planets
// https://nssdc.gsfc.nasa.gov/planetary/planetfact.html
Planetoid::Planetoid(Scene* scene, SceneObject* parent, size_t material, unsigned int meshU, unsigned int meshV, float radius)
    : SceneObject(scene, parent), material(material), meshU(meshU), meshV(meshV) {
    m_verts.reserve(((size_t)meshU + 1) * ((size_t)meshV + 1));
    m_tris.reserve((size_t)meshU * (size_t)meshV * sizeof(Tri));
    
    unsigned int tex_tile = Materials[material].texture1;
    unsigned int x = tex_tile % 4;
    unsigned int y = tex_tile / 4;
    // Should read these from texture loader.
    tex_rx = 4096.0f; // atlas width
    tex_ry = 2048.0f; // atlas height
    tex_dx = 1024.0f; // tile width
    tex_dy = 512.0f;  // tile height
    tex_lx = tex_dx * (float)x; // lower x of atlas tile
    tex_ly = tex_dy * (float)y;  // lower y of atlas tile
    //std::cout << "Material: " << material << " -> x=" << x << " y=" << y << '\n';
    //std::cout << "Lower: " << tex_lx << "," << tex_ly << " Upper: " << tex_hx << "," << tex_hy << '\n';
    if (Materials[material].texture2 != NO_UINT) {
        // Planetoid has a ring
        unsigned int texring_tile = Materials[material].texture2;
        unsigned int xring = texring_tile % 4;
        unsigned int yring = texring_tile / 4;
        texring_dx = 1024.0f; // tile width
        texring_dy = 512.0f;  // tile height
        texring_lx = texring_dx * (float)xring; // lower x of atlas tile
        texring_ly = texring_dy * (float)yring;  // lower y of atlas tile
        genGeomRing(radius * 1.11f, radius * 2.32f);
    }
    genGeom(radius);
    vbl = new VertexBufferLayout;
    vbl->Push<float>(3);   // Vertex pos
    vbl->Push<float>(3);   // Vertex normal
    vbl->Push<float>(2);   // Texture coord
    va = new VertexArray;
    vb = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);
    va->AddBuffer(*vb, *vbl, true);
    if (Materials[material].texture2 != NO_UINT) {
        // Planetoid has a ring
        vblring = new VertexBufferLayout;
        vblring->Push<float>(3);   // Vertex pos
        vblring->Push<float>(3);   // Vertex normal
        vblring->Push<float>(2);   // Texture coord
        varing = new VertexArray;
        vbring = new VertexBuffer(&m_vertsring[0], (unsigned int)m_vertsring.size() * sizeof(Vertex));
        ibring = new IndexBuffer((unsigned int*)&m_trisring[0], (unsigned int)m_trisring.size() * 3);
        varing->AddBuffer(*vbring, *vblring, true);
    }
    shdr = scene->m_app->getShaderLib()->getShader(Materials[material].shader);
    tex = scene->m_app->getTextureLib()->getTexture(Materials[material].atlas);
}
Planetoid::~Planetoid() {
    //std::cout << "Planetoid destroyed.";
    delete tex;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
}
void Planetoid::setPosition(glm::vec3 pos) {
    position = pos;
}
void Planetoid::setRadius(float radius) {
    updateGeom(radius);
    if (Materials[material].texture2 != NO_UINT) {
        // Planetoid has a ring
        // https://nssdc.gsfc.nasa.gov/planetary/factsheet/satringfact.html
        updateGeomRing(radius * 1.11f, radius * 7.964f);
    }
}
void Planetoid::draw(Camera* cam) {
    //std::cout << "Drawing Planetoid!\n";
    shdr->Bind();
    glm::mat4 pv = cam->getProjMat() * cam->getViewMat();
    shdr->SetUniformMatrix4f("projview", pv);
    shdr->SetUniformMatrix4f("world", worldmatrix);
    glm::mat3 worldnormal = glm::transpose(glm::inverse(glm::mat3(worldmatrix)));
    shdr->SetUniformMatrix3f("worldnormal", worldnormal);
    //std::cout << glm::to_string(worldmatrix) << '\n';
    //shdr->SetUniform3f("position", position.x, position.y, position.z);
    shdr->SetUniform3f("lightDir", cam->CamLightDir);
    //shdr->SetUniform1i("tex", tex->GetRenderID());  // Wrong! Texture units and textures are not the same.
    //  See: https://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
    shdr->SetUniform1i("tex", tex->GetTextureSlot());
    va->Bind();
    ib->Bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDisable(GL_CULL_FACE); // It is irritating that Earth disappears when seen from back/in side
    glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0);
    //glEnable(GL_CULL_FACE);
    ib->Unbind();
    va->Unbind();

    if (Materials[material].texture2 != NO_UINT) {
        varing->Bind();
        ibring->Bind();
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_CULL_FACE); // It is irritating that Earth disappears when seen from back/in side
        glDrawElements(GL_TRIANGLES, ibring->GetCount(), GL_UNSIGNED_INT, 0);
        glEnable(GL_CULL_FACE);
        ibring->Unbind();
        varing->Unbind();
    }
    shdr->Unbind();
    return;
}
glm::vec3 Planetoid::getLoc3D_NS(float rLat, float rLon, float height, float radius) {
    float m_Radius = 1.0f;
    float w = cos(rLat) * (radius + height);
    float x = cos(rLon) * w;
    float y = sin(rLon) * w;
    float z = sin(rLat) * (radius + height);
    return glm::vec3(x, y, z);
}
void Planetoid::updateGeom(float radius) {
    // Only update the Vertex coordinates and normals, the Tri data is already fine
    // Alternatively use an i index and overwrite existing entries like BodyGeometry does. Not sure which is faster.
    // I suppose clear()ing calls the destructor of each element?
    m_verts.clear();
    float rLat, rLon;
    float tex_x = 0.0f;
    float tex_y = 0.0f;
    const float rl_x = tex_lx / tex_rx;
    const float rl_y = tex_ly / tex_ry;
    const float dr_x = (tex_dx - 1.0f) / (tex_rx * (float)meshU);
    const float dr_y = (tex_dy - 1.0f) / (tex_ry * (float)meshV);

    for (unsigned int v = 0; v <= meshV; v++) {
        rLat = (pif * v / meshV) - pi2f;
        tex_y = rl_y + (dr_y * (float)v);
        for (unsigned int u = 0; u <= meshU; u++) {
            rLon = (tauf * u / meshU) - pif; // could refactor tauf/meshU into a constant, ditto for rLat
            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);
            glm::vec3 nml = glm::normalize(loc); // Lighting normal, add Insolation normal too?
            tex_x = rl_x + (dr_x * (float)u);
            m_verts.push_back({ loc, nml, {tex_x, tex_y} });
        }
    }
    vb->LoadData(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
}
void Planetoid::updateGeomRing(float r_i, float r_o) {
    m_vertsring.clear();
    float rLat, rLon;
    float tex_x = 0.0f;
    float tex_y = 0.0f;
    const float rl_x = texring_lx / tex_rx;
    const float rl_y = texring_ly / tex_ry;
    const float dr_x = (texring_dx - 1.0f) / (tex_rx * (float)meshU);
    const float dr_y = (texring_dy - 1.0f) / (tex_ry * (float)meshV);
    float radius = 0.0f;
    for (unsigned int v = 0; v <= meshV; v++) {
        rLat = 0.0f;
        radius = r_i + (float)v * (r_o - r_i) / (float)meshV;
        tex_y = rl_y + (dr_y * (float)v);
        for (unsigned int u = 0; u <= meshU; u++) {
            rLon = (tauf * u / meshU) - pif;
            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);
            glm::vec3 nml = { 0.0f, 0.0f, 1.0f };  // Insolation normal, add Insolation normal too?
            tex_x = rl_x + (dr_x * (float)u);
            m_vertsring.push_back({ loc, nml, {tex_x, tex_y} });
        }
    }
}
void Planetoid::genGeom(float radius) {
    float rLat, rLon;
    float tex_x = 0.0f;
    float tex_y = 0.0f;
    const float rl_x = tex_lx / tex_rx;
    const float rl_y = tex_ly / tex_ry;
    const float dr_x = (tex_dx - 1.0f) / (tex_rx * (float)meshU);
    const float dr_y = (tex_dy - 1.0f) / (tex_ry * (float)meshV);

    for (unsigned int v = 0; v <= meshV; v++) {
        rLat = (pif * v / meshV) - pi2f;
        //tex_y = (tex_ly / tex_ry) + ((tex_dy - 1.0f) * (float)v) / (tex_ry * (float)meshV);
        tex_y = rl_y + (dr_y * (float)v);
        for (unsigned int u = 0; u <= meshU; u++) {
            rLon = (tauf * u / meshU) - pif;
            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);
            glm::vec3 nml = glm::normalize(loc);  // Insolation normal, add Insolation normal too?
            //tex_x = (tex_lx / tex_rx) + ((tex_dx - 1.0f) * (float)u) / (tex_rx * (float)meshU);
            tex_x = rl_x + (dr_x * (float)u);
            m_verts.push_back({ loc, nml, {tex_x, tex_y} });
            if (u < meshU && v < meshV) {
                m_tris.push_back({ (meshU + 1) * v + u , (meshU + 1) * (v + 1) + u, (meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (meshU + 1) * (v + 1) + 1 + u, (meshU + 1) * v + u + 1, (meshU + 1) * v + u });
            }
        }
    }
}
void Planetoid::genGeomRing(float r_i, float r_o) {
    float rLat, rLon;
    float tex_x = 0.0f;
    float tex_y = 0.0f;
    const float rl_x = texring_lx / tex_rx;
    const float rl_y = texring_ly / tex_ry;
    const float dr_x = (texring_dx - 1.0f) / (tex_rx * (float)meshU);
    const float dr_y = (texring_dy - 1.0f) / (tex_ry * (float)meshV);
    float radius = 0.0f;
    for (unsigned int v = 0; v <= meshV; v++) {
        rLat = 0.0f;
        radius = r_i + (float)v * (r_o - r_i) / (float)meshV;
        //std::cout << "Radius: " << radius << '\n';
        tex_y = rl_y + (dr_y * (float)v);
        for (unsigned int u = 0; u <= meshU; u++) {
            rLon = (tauf * u / meshU) - pif;
            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);
            glm::vec3 nml = { 0.0f, 0.0f, 1.0f };  // Insolation normal, add Insolation normal too?
            tex_x = rl_x + (dr_x * (float)u);
            //std::cout << "Tex coord: " << tex_x << "," << tex_y << '\n';
            m_vertsring.push_back({ loc, nml, {tex_x, tex_y} });
            if (u < meshU && v < meshV) {
                m_trisring.push_back({ (meshU + 1) * v + u , (meshU + 1) * (v + 1) + u, (meshU + 1) * (v + 1) + 1 + u });
                m_trisring.push_back({ (meshU + 1) * (v + 1) + 1 + u, (meshU + 1) * v + u + 1, (meshU + 1) * v + u });
            }
        }
    }
}

// ----------------------
//  General Ground Point
// ----------------------
// Astonishing how little code it takes, when using Planetoid and SceneObject inheritance
PlanetoidGP::PlanetoidGP(Scene* scene, SceneObject* parent, const size_t type, const std::string& name, const glm::vec3 pos)
    : Planetoid(scene, parent, type, 40, 20, 0.02f) {
    this->name = name;
    //setParent(parent); // In SceneObject
    setPosition(pos);  // In Planetoid
}


// ----------
//  Ecliptic
// ----------
Ecliptic::Ecliptic(Scene* scene, SceneObject* parent, BodyGeometry* geometry, float width, glm::vec4 color)
    : SceneObject(scene, parent) {
    name = "Ecliptic";
    locref = geometry;              // Redundant! Use m_geometry OR locref, not both.
    m_geometry = geometry;
    path = new SmartPath{ scene, this, width, color };
    generate();
}
Ecliptic::~Ecliptic() {
    // !!! FIX: Also remove own children, or figure out how to deal with them !!! (No children at the moment)
    m_parent->removeChild(this);
    delete path;
}
void Ecliptic::setColor(glm::vec4 color) {
    path->setColor(color);
}
void Ecliptic::setWidth(float width) {
    path->setWidth(width);
}
void Ecliptic::generate() {
    path->clearPoints();
    LLD gpos, tpos;
    glm::vec3 pos{ 0.0f };
    // According to https://www.aa.quae.nl/en/reken/transformatie.html
    // tan(alpha) = tan(lambda) * cos(epsilon), where alpha is right ascension, lambda is Ecliptic longitude, epsilon is true obliquity
    // So, tan(lambda) = tan(alpha) / cos(epsilon), thus lambda = atan2( tan(aplha), cos(epsilon) )
    const double epsilon = scene->astro->TrueObliquityOfEcliptic(NO_DOUBLE, true);
    const double start_celes_lon = -pi + verytiny + scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    double start_eclip_lon = atan2(tan(start_celes_lon), cos(epsilon));
    if (start_celes_lon <= -pi2 || start_celes_lon > pi2) start_eclip_lon += pi;
    if (start_eclip_lon < 0.0) start_eclip_lon += tau;
    //std::cout << start_eclip_lon << '\n';
    for (double lon = start_eclip_lon; lon <= tau; lon += deg2rad) {
        gpos = scene->astro->calcEc2Geo(0.0, lon, epsilon);
        tpos = scene->astro->calcDecRA2GP(gpos, NO_DOUBLE);
        pos = (locref->*locpos)(tpos);
        path->addPoint(pos);
    }
    for (double lon = 0.0; lon <= start_eclip_lon; lon += deg2rad) {
        gpos = scene->astro->calcEc2Geo(0.0, lon, epsilon);
        tpos = scene->astro->calcDecRA2GP(gpos, NO_DOUBLE);
        pos = (locref->*locpos)(tpos);
        path->addPoint(pos);
    }
    gpos = scene->astro->calcEc2Geo(0.0, start_eclip_lon-tiny, epsilon);
    tpos = scene->astro->calcDecRA2GP(gpos, NO_DOUBLE);
    pos = (locref->*locpos)(tpos);
    path->addPoint(pos);
}
bool Ecliptic::update() { return false; }
void Ecliptic::draw(Camera* cam) {}

// -----------------
//  Precession Path
// -----------------
PrecessionPath::PrecessionPath(Scene* scene, SceneObject* parent, BodyGeometry* geometry, float width, glm::vec4 color)
    : SceneObject(scene, parent) {
    name = "Precession Path";
    locref = geometry;              // Redundant! Use m_geometry OR locref, not both.
    m_geometry = geometry;
    path = new SmartPath{ scene, this, width, color };
    generate();
}
PrecessionPath::~PrecessionPath() {
    // !!! FIX: Also remove own children, or figure out how to deal with them !!! (No children at the moment)
    m_parent->removeChild(this);
    delete path;
}
void PrecessionPath::setColor(glm::vec4 color) {
    path->setColor(color);
}
void PrecessionPath::setWidth(float width) {
    path->setWidth(width);
}
void PrecessionPath::generate() {
    path->clearPoints();
    EDateTime* time = new EDateTime();
    //time->setTime(-2000, 1, 1.0, 0.0, 0.0, 0.0);
    LLD decra{ };
    glm::vec3 pos{ 0.0f, 0.0f, 0.0f };
    for (int year = -26000; year < 26000; year += 50) {
        time->setTime(year, 1, 1.0, 0.0, 0.0, 0.0);
        decra = scene->astro->PrecessJ2000DecRA({ pi2, 0.0, 0.0 }, time->jd_tt()); // precess pole
        //pos = (locref->*locpos)(decra);
        pos = AEarth::EquatorialPoleVondrak(time->jd_tt());  // Most of the precession is in the equatorial pole
        //pos = AEarth::EclipticPoleVondrak(time->jd_tt());  // Most of the precession is in the equatorial pole
        // Convert to spherical
        LLD spos{};
        double diag = sqrt(pos.x * pos.x + pos.y * pos.y);
        // atan2 is a bit sensitive at the poles
        if (diag < 0.0000001 && pos.z + tiny > 1.0) {
            spos.lat = pi2;
            spos.lon = 0.0;
        }
        else if (diag < 0.0000001 && pos.z - tiny < -1.0) {
            spos.lat = -pi2;
            spos.lon = 0.0;
        }
        else {
            spos.lat = atan2(pos.z, diag);
            spos.lon = atan2(pos.y, pos.x);
        }
        spos.dst = 0.0;  // glm::length(pos);
        pos = (locref->*locpos)(spos);
        path->addPoint(pos);
    }
}
bool PrecessionPath::update() { return false; }
void PrecessionPath::draw(Camera* cam) {}



// ------------------
//  Celestial Sphere
// ------------------
// Dec RA coordinate sphere
// Has zero point (Vernal Equinox) at x axis, rotation axis Z aligned
//CelestialSphere::CelestialSphere(Scene* scene, SceneObject* parent, float radius)
//    : SceneObject(scene, parent), radius(radius) {
//
//}
//bool CelestialSphere::update() { return false; }
//void CelestialSphere::draw() {}


// --------------
//  Detailed Sky
// --------------
// !!! FIX: Precession is not applied to the texture, so it is slightly off from SkyDot stars (unless JD_TT is J2000.0) !!!

DetailedSky::DetailedSky(Scene* scene, SceneObject* parent, std::string mode, unsigned int meshU, unsigned int meshV, float radius)
    : BodyGeometry(scene, parent, DETAILED_SKY, mode, meshU, meshV, radius) {
    name = "Detailed Sky";
    hasgui = true;
    // Should all the thousands of stars be children ?? If so, don't use Dots directly.
    skydotsFactory = new SkyDots(scene);
    //addEquator();
    update();
}
DetailedSky::~DetailedSky() {
}
void DetailedSky::setTexture(bool tex) {  // rename to useTexture(), or just let bool textured be the interface
    // true = render the texture, false = do not render the texture
    textured = tex;
}
void DetailedSky::addGridEquatorial() {
    // This is the native grid for DetailedSky, so simply let Grid inherit the orientation parameters via worldmatrix
    // UPD: Now getLoc3D() includes siderealtime, so grid should not inherit.
    equatorialgrid = new Grid(scene, this, this, "EquatorialGrid");
    equatorialgrid->setSpacing(15.0, false);
    // FIX: Add equator and prime meridian
    equatorialgrid->setWidth(0.005f);
    equatorialgrid->addLongitude(0.0);
    equatorialgrid->addLatitude(0.0);
    equatorialgrid->setColor(PURPLE);
    equatorialgrid->setWidth(0.005f);
}
void DetailedSky::addGridEcliptic() {
    eclipticgrid = new Grid(scene, this, this, "EclipticGrid");
    eclipticgrid->setSpacing(15.0, false);
    // FIX: Add equator and prime meridian
    eclipticgrid->setWidth(0.005f);
    eclipticgrid->rotations.x = deg2radf * 23.4f; // !!! FIX: Use proper obliquity of ecliptic. Also, fix model coordinates so this can be a Y axis rotation.
    eclipticgrid->addLongitude(0.0);
    eclipticgrid->addLatitude(0.0);
    eclipticgrid->setColor(GREEN);
    eclipticgrid->setWidth(0.005f);
}
void DetailedSky::addEcliptic() {
    ecliptic = new Ecliptic(scene, this, this, NO_FLOAT, GREEN);
}
void DetailedSky::addPrecessionPath() {
    precessionpath = new PrecessionPath(scene, this, this, NO_FLOAT, NO_COLOR);
}
void DetailedSky::addSun() {
    sundot = new PlanetoidGP(scene, this, SUN, "SunGP", getSunLocation());
}
glm::vec3 DetailedSky::getSunLocation() {
    glm::vec3 sunloc;
    LLD sunpos = scene->astro->getDecRA(A_SUN, NO_DOUBLE);
    sunpos.dst = radius;
    sunloc = Spherical::Spherical2Rectangular(sunpos);
    return sunloc;
}
void DetailedSky::addStars(double magnitude) {
    size_t i = skydotDefs.size();
    for (auto& s : Astronomy::stellarobjects) {
        if (s.vmag < magnitude) addStar(++i, s);
    }
    std::cout << "Stars added: " << i << " of magnitude less than " << magnitude << '\n';
}
void DetailedSky::addStar(size_t unique, Astronomy::stellarobject& star) {
    //std::cout << star.dec << "," << star.ra << " | " << star.pm_dec << "," << star.pm_ra << '\n';
    star.dec *= deg2rad;  // FIX!!! Bruh, these were passed by reference, don't change them!!
    star.ra *= deg2rad;
    star.pm_dec *= deg2rad;
    star.pm_ra *= deg2rad;
    float size = getMagnitude2Radius(star.vmag);
    LLD starpos = scene->astro->applyProperMotionJ2000(NO_DOUBLE, { star.dec,star.ra,0.0 }, { star.pm_dec, star.pm_ra, 0.0 });
    starpos = scene->astro->PrecessJ2000DecRA(starpos);
    glm::vec4 color{ (float)star.red, (float)star.green, (float)star.blue, 1.0f};
    size_t index = skydotsFactory->addXYZ(getDecRA2Pos3D(starpos), color, size); // Split out for readability
    //size_t index = skydotsFactory->addXYZ(getLoc3D(starpos, true), color, size); // Split out for readability
    LLD stardecra{};
    stardecra.lat = star.dec;
    stardecra.lon = star.ra;
    stardecra.dst = 0.0;
    LLD starpm{};
    starpm.lat = star.pm_dec;
    starpm.lon = star.pm_ra;
    starpm.dst = 0.0;
    skydotDefs.push_back({ unique, color, stardecra, starpm, size, index });
    return; // skydotDefs.size() - 1;
}
void DetailedSky::addDotDecRA(size_t unique, double dec, double ra, glm::vec4 color, float size, bool rad) {
    if (!rad) {
        dec *= deg2rad;
        ra *= deg2rad;
    }
    LLD pos{};
    pos.lat = dec;
    pos.lon = ra;
    pos.dst = 0.0;
    size_t index = skydotsFactory->addXYZ(getDecRA2Pos3D(pos), color, size); // Split out for readability
    //size_t index = skydotsFactory->addXYZ(getLoc3D({ dec, ra, 0.0 }, true), color, size); // Split out for readability
    LLD nullpos{};
    nullpos.lat = 0.0;
    nullpos.lon = 0.0;
    nullpos.dst = 0.0;
    skydotDefs.push_back({ unique, color, pos, nullpos, size, index });
    return;
}
glm::vec3 DetailedSky::getDecRA2Pos3D(LLD decra) { // Input MUST be radians !!!

    if (siderealtime) {
        decra.lon -= scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
        //decra.lon = clampmPitoPi(decra.lon);
    }
    return getLoc3D(decra);
}
float DetailedSky::getMagnitude2Radius(const double magnitude) const {
    // This hack seems to match the ESO full sky diameters pretty closely for some reason
    // (apparently depending on Camera settings, not sure which were used when I made the above comment)
    // The scale is really non-linear so should probably involve a logarithm
    //return (float)((4.0 - (magnitude + 1.5) / 2.0) / (200.0 / radius));
    return (float)((4.0 - (magnitude + 1.5) / 2.0) / (200.0 / radius));
}
bool DetailedSky::drawSpecific(Camera* cam, Shader* shdr) { 
    // NOTE: This uses a different shader, so will obliterate any uniforms etc already set in BodyGeometry::draw() or here
    skydotsFactory->draw(cam);
    
    shdr->Bind();
    shdr->SetUniform3f("lDir", glm::normalize(cam->getPosition()));
    //shdr->SetUniform3f("lDir", cam->CamLightDir);

    return !textured;  // if not textured, skip BodyGeometry draw
}
bool DetailedSky::update() {
    // Rotates texture and SceneObject children, but not individual star dots (they are not in SceneTree)
    // (Dec RA Grid is no longer child, as it uses getLoc3D() and would get double GSID applied)
    if (siderealtime) rotations.z = -(float)scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);

    if (eclipticgrid) eclipticgrid->rotations.x = (float)scene->astro->TrueObliquityOfEcliptic(NO_DOUBLE, true);


    if (siderealtime && ecliptic) ecliptic->rotations.z = (float)scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    if (ecliptic)  ecliptic->generate();

    if (precessionpath) precessionpath->generate();

    if (sundot) sundot->setPosition(getSunLocation());

    // Run through skydotDefs and update SkyDots
    // !!! FIX:
    // + Should recalculate proper motion and precession if time since last update is more than some threshold
    // - Should apply nutation
    // - and abberation ?
    // !!!
    for (auto& s : skydotDefs) {
        LLD starpos = s.coordinates;
        if (propermotion) starpos = scene->astro->applyProperMotionJ2000(NO_DOUBLE, starpos, s.propermotion);
        if (precession) starpos = scene->astro->PrecessJ2000DecRA(starpos);
        skydotsFactory->changeXYZ(s.dot_index, getDecRA2Pos3D(starpos), NO_COLOR, NO_FLOAT);
        //skydotsFactory->changeXYZ(s.dot_index, getLoc3D(starpos), NO_COLOR, NO_FLOAT);
    }
    updateShape();  // Update actual geometry provided by BodyGeometry
    return false; // Did not replace worldmatrix already
}
LLD DetailedSky::toLocalCoords(LLD loc) {
    // Convert from Dec/RA (in equinox of date) to planetocentric format
    LLD myloc = loc;
    //if (siderealtime) myloc.lon -= scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    myloc.lon = ACoord::rangezero2tau(myloc.lon);
    if (myloc.lon > pi) myloc.lon = myloc.lon - tau;
    return myloc;
}
LLD DetailedSky::fromLocalCoords(LLD loc) {
    // Convert from planetocentric (native to BodyGeometry) to Dec/RA (native to DetailedSky)
    LLD myloc = loc;
    // !!! FIX: Include Sidereal Time, using siderealtime boolean flag !!!
    myloc.lon = ACoord::rangempi2pi(myloc.lon);
    if (myloc.lon < 0.0) myloc.lon = myloc.lon + tau;
    return myloc;
}
void DetailedSky::myGUI() {
    if (ImGui::CollapsingHeader(name.c_str())) {
        ImGui::Checkbox("Use Texture", &textured);
        ImGui::Checkbox("Proper Motion", &propermotion);
        ImGui::Checkbox("Precession", &precession);
        ImGui::Checkbox("Sidereal Time", &siderealtime);
        ImGui::TreePop();
    }
}


// ----------------
//  Detailed Earth
// ----------------
DetailedEarth::DetailedEarth(Scene* scene, SceneObject* parent, std::string mode, unsigned int meshU, unsigned int meshV, float radius)
    : BodyGeometry(scene, parent, DETAILED_EARTH, mode, meshU, meshV, radius) {

    name = "Detailed Earth";
    hasgui = true;
    update();
}
DetailedEarth::~DetailedEarth() {
}

void DetailedEarth::addSunGP() { // Maybe return the SunGP* so user can access directly if desired
    //m_sundot = m_dotsFactory->addXYZ(worldmatrix * sunDir * m_radius, SUNCOLOR, 0.015f);
    //m_sungp = new SunGP(m_scene, this, getSunGPLocation());
    sungp = new PlanetoidGP(scene, this, SUN, "SunGP", getSunGPLocation());
}

glm::vec3 DetailedEarth::getSunGPLocation() {
    //LLD sunpos = m_scene->astro->getDecRA(SUN);   // Only returns degrees, not radians? Test to see. Looks like only radians, no degrees.
    //std::cout << m_scene->astro->formatDecRA(sunpos.lat, sunpos.lon, true) << '\n';
    //sunpos = m_scene->astro->calcDecRA2GP(sunpos, m_scene->astro->getJD_UTC());
    // subsolar happens to already be calculated for insolation, so using that.
    // Does this need to be available outside the class? I.e. could addSunGP() and update()/updateSunGP() just use subsolar directly?
    //std::cout << "SunGP: " << subsolar.lat << ", " << subsolar.lon << "\n";
    return getLoc3D(subsolar);
}

void DetailedEarth::addMoonGP() {
    moongp = new PlanetoidGP(scene, this, MOON, "MoonGP", getMoonGPLocation());
}
glm::vec3 DetailedEarth::getMoonGPLocation() {
    //std::cout << "MoonGP: " << sublunar.lat << ", " << sublunar.lon << "\n";
    return getLoc3D(sublunar);
}

LLD DetailedEarth::calcMoon() {  // Gives same results as the previous AA+ v2.49 based function
    LLD qmoon = scene->astro->MoonTrueEquatorial(scene->astro->getJD_TT(), ELP_MPP02);
    m_moonDec = qmoon.lat;
    double sidtime = scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    double moonHour = -qmoon.lon + sidtime;
    m_moonHour = moonHour;
    double w = cos(qmoon.lat);
    MoonLightDir.x = (float)(cos(-moonHour) * w);
    MoonLightDir.y = (float)(sin(-moonHour) * w);
    MoonLightDir.z = (float)(sin(qmoon.lat));
    LLD moongp = scene->astro->calcDecHA2GP({ qmoon.lat, moonHour, 0.0 });
    return moongp;
}

void DetailedEarth::addArcticCircles() {
    double obliquity = scene->astro->TrueObliquityOfEcliptic(NO_DOUBLE, true);
    //std::cout << "INFO! DetailedEarth::addArcticCircles() - Obliquity: " << obliquity << '\n';
    arcticcircle = new Latitude(scene, this, this, pi2 - obliquity, NO_FLOAT, AQUA);
    arcticcircle->name = "ArcticCircle";
    antarcticcircle = new Latitude(scene, this, this, obliquity - pi2, NO_FLOAT, AQUA);
    antarcticcircle->name = "AntarcticCircle";
}
void DetailedEarth::removeArcticCircles() {
    delete arcticcircle;
    arcticcircle = nullptr;
    delete antarcticcircle;
    antarcticcircle = nullptr;
}
// Could do subtropic regions but there are multiple contradictory definitions
void DetailedEarth::addTropicCircles() {
    double obliquity = scene->astro->TrueObliquityOfEcliptic(NO_DOUBLE, true);
    //std::cout << obliquity << '\n';
    tropicofcancer = new Latitude(scene, this, this, obliquity, NO_FLOAT, LIGHT_ORANGE);
    tropicofcancer->name = "TropicCancer";
    tropicofcapricorn = new Latitude(scene, this, this, -obliquity, NO_FLOAT, LIGHT_ORANGE);
    tropicofcapricorn->name = "TropicCapricorn";
}
void DetailedEarth::addEcliptic() {
    ecliptic = new Ecliptic(scene, this, this, NO_FLOAT, GREEN);
    //ecliptic->rotationorder = ZYX;
}
bool DetailedEarth::update() {
    if (dirty_geometry && arcticcircle) arcticcircle->generate();
    if (dirty_geometry && antarcticcircle) antarcticcircle->generate();
    if (dirty_geometry && tropicofcancer) tropicofcancer->generate();
    if (dirty_geometry && tropicofcapricorn) tropicofcapricorn->generate();

    if (ecliptic)  ecliptic->generate();

    updateShape();  // Update actual geometry provided by BodyGeometry

    subsolar = scene->astro->calcDecHA2GP(scene->astro->getDecGHA(A_SUN, NO_DOUBLE));
    sublunar = calcMoon();
    if(moontermgen) moontermgen->lld1.lat = m_moonDec;
    if (moontermgen) moontermgen->lld1.lon = m_moonHour;

    //std::cout << sublunar.str() << std::endl;

    //if (do_eccentriclight) { SunLightDir = getNml3D_EE(subsolar.lat, subsolar.lon, 0.0f); return; }
    SunLightDir = getNml3D_NS(subsolar);
    if (sungp != nullptr) sungp->setPosition(getSunGPLocation()); // Uses subsolar calculated above
    if (moongp != nullptr) moongp->setPosition(getMoonGPLocation());
    if (celestialgrid != nullptr) celestialgrid->rotations.y = (float)scene->astro->TrueObliquityOfEcliptic(NO_DOUBLE, true);
    return false;
}
LLD DetailedEarth::toLocalCoords(LLD loc) {
    // Local coords are the same as BodyGeometry native
    return loc;
}
LLD DetailedEarth::fromLocalCoords(LLD loc) {
    // Local coords are the same as BodyGeometry native
    return loc;
}
bool DetailedEarth::drawSpecific(Camera* cam, Shader* shdr) {
    // Object specific shader uniforms etc
    //cam->camDst = m_camDist;
    shdr->SetUniformMatrix4f("world", worldmatrix);
    glm::mat3 worldnormal = glm::transpose(glm::inverse(glm::mat3(worldmatrix)));
    shdr->SetUniformMatrix3f("worldnormal", worldnormal);

    if (insolation) shdr->SetUniform3f("sunDir", SunLightDir);
    if (!insolation) shdr->SetUniform3f("sunDir", 0.0f, 0.0f, 0.0f);
    if (inlunation) shdr->SetUniform3f("moonDir", MoonLightDir);
    if (!inlunation) shdr->SetUniform3f("moonDir", 0.0f, 0.0f, 0.0f);
    shdr->SetUniform3f("lightDir", glm::normalize(cam->getPosition()));

    if (w_refract) shdr->SetUniform1f("refraction", 1.0f);
    else shdr->SetUniform1f("refraction", 0.0f);
    //shdr->SetUniform1f("alpha", m_alpha);
    shdr->SetUniform1f("alpha", 1.0f);
    shdr->SetUniform1f("twilight", w_twilight ? 1.0f : 0.0f);
    // Scale the heightmap to a pleasing effect, which is apparently different for Sun light and scene light.
    //shdr->SetUniform1f("sunBumpScale", 0.0005f); // In DetailedMoon these are member vars.
    shdr->SetUniform1f("sunBumpScale", 0.000f); // In DetailedMoon these are member vars.
    shdr->SetUniform1f("lightBumpScale", 0.015f);

    // tinting arctics and tropics in shader
    if (tintarctics) shdr->SetUniform4f("tintarctics", arcticscolor);
    else shdr->SetUniform4f("tintarctics", glm::vec4(0.0f));
    if (tinttropics) shdr->SetUniform4f("tinttropics", tropicscolor);
    else shdr->SetUniform4f("tinttropics", glm::vec4(0.0f));
    shdr->SetUniform1f("obliquity", (float)(scene->astro->TrueObliquityOfEcliptic() / 180.0));
    //if (earthparticles != nullptr) earthparticles->draw();
    //if (equator != nullptr) equator->draw(cam);
    //if (primem != nullptr) primem->draw(cam);
    return false; // Didn't actually draw yet.
}
void DetailedEarth::myGUI() {
    if (ImGui::CollapsingHeader(name.c_str())) {
        // Texture alignment. Note: Adjusts texture coordinates in mesh, so applies equally to all textures in the same material.
        //ImGui::SliderFloat("Tex X", &l.layer->m_scene->m_app->currentEarth->texture_x, -10.0f, 10.0f);
        //ImGui::SliderFloat("Tex Y", &l.layer->m_scene->m_app->currentEarth->texture_y, -10.0f, 10.0f);
        // Flat Sun height above surface. Earth implementation of this is a mess, so do better in DetailedEarth
        //ImGui::SliderFloat("Sun Height", &l.layer->m_scene->m_app->currentEarth->flatsunheight, 0.0f, 100000.0f);
        if (ImGui::TreeNode("Sun")) {
            ImGui::Checkbox("Solar Insolation", &insolation);
            ImGui::Checkbox("Solar Twilight", &w_twilight);
            ImGui::Checkbox("Solar Refraction", &w_refract);
            // Overlay colors
            ImGui::ColorEdit4("Arctics", (float*)&arcticscolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();
            ImGui::Checkbox("Arctics Overlay", &tintarctics);
            ImGui::ColorEdit4("Tropics", (float *)&tropicscolor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
            ImGui::SameLine();
            ImGui::Checkbox("Tropics Overlay", &tinttropics);
            ImGui::TreePop();
        }
        if (ImGui::TreeNode("Moon")) {
            ImGui::Checkbox("Lunar Insolation", &inlunation);
            //ImGui::Checkbox("Lunar Refraction", &w_mrefract);
            ImGui::TreePop();
        }
        // Ideally this would be in BodyGeometry, but I don't see an easy way to integrate the myGUI()s
        if (ImGui::TreeNode("Shape")) {
            //const char* items[] = { "AENS", "AEER", "AERC", "AEE8", "NSER", "NSRC", "NSE8", "ERRC", "ERE8", "RCE8" };
            const char* items[] = { "AENS", "AEER", "NSER" };
            if (ImGui::BeginCombo("Earth type", getMode().c_str())) {
                for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                    bool is_selected = (getMode().c_str() == items[n]);
                    if (ImGui::Selectable(items[n], is_selected)) {
                        setMode((std::string)items[n]);
                        setMorphParameter(0.0f);
                    }
                    if (is_selected) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            float morph = morph_param;
            ImGui::SliderFloat("Param", &morph_param, 0.0f, 1.0f);
            if (morph != morph_param) dirty_geometry = true;
            ImGui::TreePop();
        }
        // Add bumpmap controls. Perhaps those should also ideally be in BodyGeometry
        ImGui::TreePop(); // Only pop tree at end of TreeNode sequence, not for each of them.
    }
}

// ---------------
//  Detailed Moon
// ---------------
// Because this fixes the Camera position to show varying Earth-Moon distances, it can only
// show the complete Moon at center of the RenderLayer3D. It is possible to orbit the camera.
// But since there can be more than one Camera in a Scene, another Camera may view other parts
// of the Scene. So supply a Camera pointer to DetailedMoon to determine which Camera to use.
// It can then take complete control of that Camera, allowing the DetailedMoon to be placed anywhere.
// Accomodate this in the code!
// NOTE: The Camera is passed into draw() during rendering. It comes from RenderLayer3D, is passed
//       to Scene, which via SceneTree calls the draw() method. Currently DetailedMoon::update()
//       calculates the cam distance, and later applies it to the Camera passed at draw(). So maybe
//       it is fine the way it is.
// Alternatively make a new EarthMoon class that can show the relationship in another Scene. It seems
// like a good idea, so pointing lines/arrows and orbit paths don't need to show up next to DetailedMoon.
// All they need in order to remain in sync is to have the same Astronomy object (which gives details
// for the same instance).
DetailedMoon::DetailedMoon(Scene* scene, SceneObject* parent, std::string mode, unsigned int meshU, unsigned int meshV, float radius)
    : BodyGeometry(scene, parent, DETAILED_MOON, mode, meshU, meshV, radius) {
    name = "Detailed Moon";
    hasgui = true;

    //!!! FIX: Should set up our own camera, since we will be manipulating it. Offer a method to get the camera if desired. !!!
    // FoV to Moon's greatest angular diameter (0.56833 degrees)
    // Distance: Moon's real diameter is 3474.8km, internal rep is 2.0f * m_radius units. Moon's closest distance ~360000km. So 2*360000/3474.8 fills screen
    //           Dist is dynamically updated in DetailedMoon::update() to simulate Earth Moon distance
    // Assumes use of the camera bound to WASD keys:
    scene->w_camera->setLatLonFovDist(0.0f, 0.0f, 0.56833f, 210.0f);
    // With such as small FoV and high distance, the Near Clipping Plane has to be around 5.0f or more to avoid rendering issues.
    // Current camFar clipping plane default of 1500.0f is plenty more than Moon distance
    scene->w_camera->camNear = 5.0f;

    // Moon actual figure geometry: https://articles.adsabs.harvard.edu/pdf/1972Moon....5..149W
    // Ellipsoid approximation and many other parameters: https://nssdc.gsfc.nasa.gov/planetary/factsheet/moonfact.html
    // Moon is only 2-3km out of round in any direction, this is less than 1 pixel when whole Moon is filling a 1080p screen.

    update();   // This is the astronomy based update function, not the geometry one.
}
DetailedMoon::~DetailedMoon() {
    // !!! FIX: Scene does not provide a method to relinguish an obtained DotsFactory. Add one and conditionally call it here. !!!
    // Well, this is not using dots at the moment, Earth and Sun points have been moved to Planetoid implementation.
}
// Should probably have setObserverPos(double,double,bool) and setGeocentricMode(bool)
void DetailedMoon::setTopocentric(const double lat, const double lon, const bool rad) {
    // Set Topological position, i.e. observe latitude and longitude on Earth.
    // Calling without parameters configures for Geocentric rather than Topocentric view.
    if (lat == NO_DOUBLE || lon == NO_DOUBLE) {
        gui_geocentric = true;
        topoLat = NO_DOUBLE;
        topoLon = NO_DOUBLE;
        return;
    }
    gui_geocentric = false;
    topoLat = lat;
    topoLon = lon;
    if (!rad) {
        topoLat *= deg2rad;
        topoLon *= deg2rad;
    }
    return;
}
void DetailedMoon::addSunGP() {
    sungp = new PlanetoidGP(scene, this, SUN, "SunGP", sunDir * radius);
    sungp->setRadius(0.015f);
}
void DetailedMoon::addEarthGP() {
    earthgp = new PlanetoidGP(scene, this, EARTH, "EarthGP", getNml3D({0.0, 0.0, 0.0}));
}
void DetailedMoon::addLibrationTrail() {
    librationtrail = new ParticleTrail(scene, 300, EARTHCOLOR, 0.005f, 5, true);
}
void DetailedMoon::updateLibrationTrail() {
    librationtrail->push(glm::mat3(worldmatrix) * nullpos); // nullpos is selenographic (0,0) in XYZ coordinates, calculated in update()
    //VPRINT(nullpos);
}
bool DetailedMoon::update() {
    //std::cout << "DetailedMoon::update()\n";
    updateShape();  // Update actual geometry provided by BodyGeometry
    //worldmatrix = glm::mat4(1.0f); // Start with no transforms
    // Calculate sunDir and camera orientation/location based on astronomical calculations
    double currentJD = scene->astro->getJD_TT();
    double rotY = 0.0, rotZ = 0.0, rotX = 0.0;

    APhysicalMoonDetails libration;
    // Earth Moon distance
    double moonDist = AMoon::RadiusVector(currentJD); // RadiusVector() returns in km - seems consistently 40km low for Jan 2023.
    if (gui_geocentric) {
        // Verify against: https://lamid58.blogspot.com/2019/09/lunar-libration.html
        // Lunar Libration for current month: https://www.lunarphasepro.com/the-moon-this-month/
        // Note, above are without Position Angle of axis. bool DetailedMoon::gui_positionangle can turn that on (true) or off (false)
        libration = AMoon::CalculateGeocentric(currentJD);
    }
    else {
        // Maybe work out the exact value? OR just ignore it, the difference is 2% in apparent size
        // In addition to the geocentric radius (chapter 11 in Meeus I think, CAAGlobe2),
        // the orientation angle Observer-EarthCenter-Moon must be used.
        moonDist -= earthradius;
        libration = AMoon::CalculateTopocentric(currentJD, topoLon, topoLat);
        // Is the parallactic angle (Chapter 14) included in libration.P ?
    }
    if (gui_librationlatitude) rotY = libration.b;
    if (gui_librationlongitude) rotZ = libration.l;
    if (gui_positionangle) rotX = libration.P;
    //std::cout << "Libration: " << libration.l << "," << libration.b << '\n';

    // Adjust camera distance so the size of the Moon shows correctly (e.g. supermoon)
    // !!! FIX: This only works correctly when camera is on X axis. If viewing side on, the Moon doesn't move sideways !!!
    //     UPD: To be fair, this whole setup is an artificial view of things. The real world camera movement that would
    //          result in the Moon moving towards or away from the Earth would be a perfect circle around Earth slightly
    //          ahead of the Moon, and looking straight at where the Moon would be on that circle. Not a real world scenario,
    //          but illustrative enough to implement it?
    m_camDist = (float)moonDist * radius / (float)moonradius;
    // Selenographic position of the Sun (use for rendering terminator in shader)
    ASelenographicMoonDetails selsun = AMoon::CalculateSelenographicPositionOfSun(currentJD, ELP2000_82);
    SunLightDir = getNml3D_NS({ selsun.b0, selsun.l0, 0.0 });   // Used for insolation in shader
    sunDir = { getLoc3D({ selsun.b0, selsun.l0, 0.0 }), 1.0f }; // Used for Sun location dot, which is morph dependent

    //worldmatrix = glm::rotate(worldmatrix, (float)rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    //worldmatrix = glm::rotate(worldmatrix, (float)rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    //worldmatrix = glm::rotate(worldmatrix, (float)-rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
    rotations = glm::vec3(rotX, rotY, -rotZ);
    //// Why does the above produce the same matrix as the following?
    //// I would expect the order to be reversed!
    ////glm::mat4 rZ = glm::rotate(glm::mat4(1.0), (float)-rotZ, glm::vec3(0.0f, 0.0f, 1.0f));
    ////glm::mat4 rY = glm::rotate(glm::mat4(1.0), (float)rotY, glm::vec3(0.0f, 1.0f, 0.0f));
    ////glm::mat4 rX = glm::rotate(glm::mat4(1.0), (float)rotX, glm::vec3(1.0f, 0.0f, 0.0f));
    ////worldmatrix = (rX * (rY * (rZ * glm::mat4(1.0))));

    earthDir = getLoc3D({ rotY, rotZ, 0.0 });

    //glm::vec3 pos = earthDir;
    //LLD myearth = { atan2(pos.z, sqrt(pos.x * pos.x + pos.y * pos.y)), atan2(pos.y, pos.x), glm::length(pos) };
    //std::cout << "myearth: " << rad2deg * myearth.lat << "," << rad2deg * myearth.lon << '\n';
    //std::cout << "libration: " << libration.b << "," << libration.l << '\n';

    //if (m_sundot != NO_UINT) updateSunGP();
    if (sungp != nullptr) sungp->setPosition(sunDir * radius);
 
    // !!! FIX: This does not work well with topocentric mode. !!!
    //if (m_earthdot != NO_UINT) updateEarthGP();
    if (earthgp != nullptr) earthgp->setPosition(earthDir * radius);

    if (librationtrail != nullptr) {
        nullpos = getLoc3D({ 0.0, 0.0, 0.0 });
        updateLibrationTrail();
    }

    // Primitive Lunar Eclipse finder - distance is ad hoc and probably not entirely right
    //if (abs(selsun.b0 - libration.b) < 0.5 && abs(selsun.l0 - libration.l) < 0.5) {
    //    std::cout << m_scene->m_astro->getTimeString()
    //        << " Sun: " << selsun.b0 << "," << selsun.l0
    //        << " Earth: " << libration.b << "," << libration.l
    //        << " Diff: " << abs(selsun.b0 - libration.b) << "," << abs(selsun.l0 - libration.l) << "\n";
    //}

    if (scene->m_app->dumpdata && false) {  // && true = enable, && false = disable
        std::cout << scene->astro->getTimeString()
            << " Sun: " << selsun.b0 << "," << selsun.l0
            << " Earth: " << libration.b << "," << libration.l
            << "\n";
    }
    return false;
}
LLD DetailedMoon::toLocalCoords(LLD loc) {
    // Local coords same as BodyGeometry native
    return loc;
}
LLD DetailedMoon::fromLocalCoords(LLD loc) {
    // Local coords same as BodyGeometry native
    return loc;
}
bool DetailedMoon::drawSpecific(Camera* cam, Shader* shdr) {
    // Object specific shader uniforms etc
    
    cam->camDst = m_camDist;
    shdr->SetUniformMatrix4f("world", worldmatrix);
    glm::mat3 worldnormal = glm::transpose(glm::inverse(glm::mat3(worldmatrix)));
    shdr->SetUniformMatrix3f("worldnormal", worldnormal);

    if (insolation) shdr->SetUniform3f("sDir", SunLightDir);
    if (!insolation) shdr->SetUniform3f("sDir", 0.0f, 0.0f, 0.0f);
    shdr->SetUniform3f("lDir", glm::normalize(cam->getPosition()));
    shdr->SetUniform1f("sunBumpScale", sunBumpmapScale);
    shdr->SetUniform1f("lightBumpScale", lightBumpmapScale);

    //if (earthparticles != nullptr) earthparticles->draw();
    //if (equatorOb != nullptr) equatorOb->draw(cam);
    //if (primemOb != nullptr) primemOb->draw(cam);
    return false; // Didn't draw yet
}
void DetailedMoon::myGUI() {
    // TODO: Controls for SunGP, EarthGP, libration trail, insolation
    if (ImGui::CollapsingHeader(name.c_str())) {
        if (ImGui::Button("Reset Camera")) {
            scene->w_camera->setLatLonFovDist(0.0f, 0.0f, 0.56833f, 210.0f); // See constructor for notes on these values
        }
        if (ImGui::TreeNode("Libration")) {
            //ImGui::Checkbox("Enable", &gui_libration); // Not implemented yet, although gui_libration variable was added
            ImGui::Checkbox("Libration in Longitude", &gui_librationlongitude);
            ImGui::Checkbox("Libration in Latitude", &gui_librationlatitude);
            ImGui::Checkbox("Axial Position Angle", &gui_positionangle);
            if (ImGui::RadioButton("Geocentric", gui_geocentric)) gui_geocentric = true;
            ImGui::SameLine();
            if (ImGui::RadioButton("Topocentric", !gui_geocentric)) gui_geocentric = false;
            if (!gui_geocentric) {
                // Work around the odd way of marking topocentric vs geocentric
                if (topoLat == NO_DOUBLE) gui_topoLat = 0.0f;
                else gui_topoLat = (float)(topoLat * rad2deg);
                if (topoLon == NO_DOUBLE) gui_topoLon = 0.0f;
                else gui_topoLon = (float)(topoLon * rad2deg);
                ImGui::SliderFloat("Latitude", &gui_topoLat, -89.9999f, 89.9999f);
                ImGui::SliderFloat("Longitude", &gui_topoLon, -179.9999f, 180.0f);
                if (gui_geocentric) topoLat = NO_DOUBLE;
                else topoLat = (double)gui_topoLat * deg2rad;
                if (gui_geocentric) topoLon = NO_DOUBLE;
                else topoLon = (double)gui_topoLon * deg2rad;
            }
            ImGui::TreePop(); // Libration
            // Do TreeNode for SunGP, EarthGP, ParticleTrail (give them GUIs and call those here in TreeNode for each of them,
            //                                                set their hasgui = false so they are only shown once)
        }
        if (ImGui::TreeNode("Bumpmapping (surface detail)")) {
            ImGui::SliderFloat("Sunbump", &sunBumpmapScale, 0.0000f, 0.01f, "%.5f");
            ImGui::SliderFloat("Lightbump", &lightBumpmapScale, 0.0f, 0.1f, "%.5f");
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
}




