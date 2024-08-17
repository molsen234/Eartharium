
#include "Renderer.h"

#include <glm/gtc/matrix_transform.hpp>  // OpenGL projection and view matrices
#include <glm/gtx/rotate_vector.hpp>     // Rotation matrices for glm
#include <glm/gtx/string_cast.hpp>       // For to_string()

#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"
#include "ImGUI/ImPlot.h"

#include "Earth.h"

// -------------
//  SceneObject
// -------------
SceneObject::SceneObject(Scene* scene, SceneObject* parent) : scene(scene) {
    if (parent == nullptr) {
        m_parent = scene->scenetree->root;
    }
    else m_parent = parent;
    m_parent->addChild(this);
}
SceneObject::~SceneObject() {
    if (m_parent) m_parent->removeChild(this);
}
void SceneObject::addChild(SceneObject* object) {
    children.push_back(object);
}
void SceneObject::removeChild(SceneObject* child) {
    children.remove(child);
    // !!! FIX: What about child's children??? !!!
}
//void SceneObject::setParent(SceneObject* parent) {
//    // !!! FIX: !!!
//    // setParent() is dangerous, it is possible to create loops in the family tree
//    // Is it enough to scan back up to the SceneTree root to prevent this?
//    if (parent == nullptr) return; // Allow unparenting objects??
//    m_parent = parent;
//    m_scene->scenetree->rootRemove(this);
//    m_parent->addChild(this);
//}
void SceneObject::setID(unsigned int oid) {
    id = oid;
}
unsigned int SceneObject::getID() {
    return id;
}
glm::mat4 SceneObject::getWorldMat() {
    return worldmatrix;
}
void SceneObject::inherit() {
    // allow object to update orientation parameters
    bool worldmat_done = update();
    if (worldmat_done) return;
    // If parented, collect parent world matrix and apply scale,rots,pos
    if (m_parent != nullptr) worldmatrix = m_parent->getWorldMat();
    else worldmatrix = glm::mat4(1.0f);
    //std::cout << glm::to_string(worldmatrix) << '\n';
    //std::cout << name << " inheriting from " << m_parent->name << "\n";
    //scale
    worldmatrix = glm::scale(worldmatrix, scale);
    //rotate
    //VPRINT(rotations);
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
    //translate
    //VPRINT(position);
    worldmatrix = glm::translate(worldmatrix, position);
    //std::cout << glm::to_string(worldmatrix) << '\n';
}
SceneObject::SceneObject(Scene* scene, bool isroot) : scene(scene) {
    if (!isroot) scene->scenetree->addSceneObject(this, nullptr);
    //std::cout << "SceneObject::SceneObject(" << this << ": " << m_scene->scenetree << "\n";
}


// -------
//  Scene
// -------
Scene::Scene(Application* app) : m_app(app) {
    // Do first! E.g. Camera is a SceneObject, so Camera cannot be created before SceneTree!
    scenetree = new SceneTree(this);
    // Set up a default Camera - Why? It doesn't save a lot of work, and is rather inconsistent. Well, to satisfy the keyboard controller.
    w_camera = newCamera("Default Cam");  // Will also be m_cameras[0]
    if (m_app->currentCam == nullptr) m_app->currentCam = w_camera;
    // Cameras need a scene to look at, so they should be derived from Scene::newCamera() or similar
    // Need to support more than one Camera per scene, so it should return a reference that can be passed to RenderLayer3D
}
Scene::~Scene() {
    // Verify that all objects are deleted here !!!
    // Should this clearScene() first?? !!!
    if (m_arrowsOb != nullptr) delete m_arrowsOb;
    if (m_sphereuvOb != nullptr) delete m_sphereuvOb;
    if (m_planesOb != nullptr) delete m_planesOb;
    if (m_cylindersOb != nullptr) delete m_cylindersOb;
    if (m_conesOb != nullptr) delete m_conesOb;
    if (m_viewconesOb != nullptr) delete m_viewconesOb;
    if (m_skydotsOb != nullptr) delete m_skydotsOb;
    if (m_dotsOb != nullptr) delete m_dotsOb;
    //if (m_threepointsolvers.size() > 0) {
    //    for (auto tps : m_threepointsolvers) { delete tps; }
    //}
    delete scenetree; // Always created in constructor
}
Camera* Scene::newCamera(const std::string name) {
    Camera* cam = new Camera(this);
    cam->name = name;
    m_cameras.push_back(cam);
    return cam;
    // As camera is SceneObject, is it still necessary to keep a list of cameras?
    // Apparently the list is never used for anything anyway.
}
void Scene::setAspect(float aspect) { m_aspect = aspect; }
float Scene::getAspect() { return m_aspect; }
void Scene::clearScene() {
    // Delete Cameras
    w_camera = nullptr;
    for (auto& cam : m_cameras) delete cam;
    // Set up new default camera?
    // w_camera = newCamera();    // OR reset camera settings with something like w_camera.reset();
    // Delete all objects and reset things to default !!!
    if (m_earthOb != nullptr) {
        delete m_earthOb;
        m_earthOb = nullptr;
    }
    //if (m_dmoonOb != nullptr) {
    //    delete m_dmoonOb;
    //    m_dmoonOb = nullptr;
    //}
    if (m_solsysOb != nullptr) {
        delete m_solsysOb;
        m_solsysOb = nullptr;
    }
    if (m_shadowmap != nullptr) {
        delete m_shadowmap;
        shadows = NONE;
        m_shadowmap = nullptr;
    }
    if (m_shadowbox != nullptr) {
        delete m_shadowbox;
        shadows = NONE;
        m_shadowbox = nullptr;
    }
    if (m_dotsOb != nullptr) { m_dotsOb->clear(); }
    if (m_sphereuvOb != nullptr) { m_sphereuvOb->clear(); }
    if (m_arrowsOb != nullptr) { m_arrowsOb->clear(); }
    if (m_cylindersOb != nullptr) { m_cylindersOb->clear(); }
    if (m_conesOb != nullptr) { m_conesOb->clear(); }
    if (m_viewconesOb != nullptr) { m_viewconesOb->clear(); }
    if (m_planesOb != nullptr) { m_planesOb->clear(); }
}
void Scene::render(Camera* cam) {
    // Might split into an update() member function? Or do all updates via SceneTree?
    if (!cam) cam = w_camera;
    scenetree->updateBreathFirst();  // Allow all SceneObjects to update their internals, e.g. position etc
    // Should take fbo render target !!!
    if (m_earthOb != nullptr) m_earthOb->Update(); // Make sure primitives are up to date before casting their shadows (Earth updates Locations)
    //if (m_dskyOb != nullptr) m_dskyOb->update();
    if (m_solsysOb != nullptr) m_solsysOb->Update();
    if (m_skysphereOb != nullptr) m_skysphereOb->UpdateTime(0.0); // Default time
    // Do shadow pass here
    if (shadows == SHADOW_MAP) {
        if (m_shadowmap != nullptr) m_shadowmap->Render(cam);
        else std::cout << "Scene::render(): Map shadows have been enabled, but no ShadowMap object was found.\n";
    }
    if (shadows == SHADOW_BOX) { // For now hardcoded to subsolar point, also in Primitives
        if (m_shadowbox != nullptr) m_shadowbox->Render(cam, m_earthOb->getSubsolarXYZ());
        else std::cout << "Scene::render(): Box shadows have been enabled, but no ShadowBox object was found.\n";
    }
    // Do render pass here
    if (m_skyboxOb != nullptr) {
        m_skyboxOb->Draw();
    }
    // These should be drawn via SceneTree instead of here.
    //if (m_dskyOb) m_dskyOb->draw(cam);
    //if (m_dmoonOb != nullptr) m_dmoonOb->draw(cam);
    //if (m_dearthOb != nullptr) m_dearthOb->draw(cam);
    scenetree->drawBreathFirst(cam);

    if (m_earthOb != nullptr) m_earthOb->draw(cam);
    if (m_solsysOb != nullptr) m_solsysOb->Draw();
    if (m_countrybordersOb != nullptr) m_countrybordersOb->update();
    if (m_timezonesOb != nullptr) m_timezonesOb->update();
    for (auto& p : m_polylines) {
        p->draw();
    }
    if (m_minifigsOb != nullptr) m_minifigsOb->draw(cam, NONE);
    if (m_cylindersOb != nullptr) m_cylindersOb->draw(cam, NONE);
    if (m_conesOb != nullptr) m_conesOb->draw(cam, NONE);
    if (m_sphereuvOb != nullptr) m_sphereuvOb->draw(cam, NONE);
    // NOTE: When drawing dots with reduced alpha, they still update the depth buffer
    //  This has the pleasant effect that when drawing a sky sphere at a Location,
    //  enabling the LocationSky will prevent the stars (SkyDot) and grid (Scene Paths) from being drawn
    //  on the far side of the SkySphere. This is a problem that is otherwise harder
    //  to solve. Thus, be CAREFUL to draw reduced alpha dots BEFORE drawing the SkySphere(s).
    //  Keep this in mind if creating a separate object for transparent dots!

    // Old - Draw PolyCurves individually
    // This is slow, as it has an OpenGL draw call for each PolyCurve
    for (auto& p : m_polycurves) {
        p->draw(cam);
    }
    // New - Collect all PolyCurve meshes into one and draw together
    //for (auto& p : m_polycurves) {
    //    p->drawToCollector(m_polycurvemeshes);
    //    //Implement a VBO = m_polycurvemeshes and set up draw calls here. This is not clean.
    //}

    if (m_skydotsOb != nullptr) m_skydotsOb->draw(cam); // SkyDots does not need to support shadows. But it does have a customized draw() function
    if (m_dotsOb != nullptr) m_dotsOb->draw(cam, NONE);   // Make second Dots primitive that draws only transparent Dots and place it near end of render chain !!!
    if (m_anglearcsOb != nullptr) m_anglearcsOb->draw();
    //for (auto& p : m_polycurves) {
    //    p->draw();
    //}
    if (m_skysphereOb != nullptr) m_skysphereOb->draw();
    if (m_planesOb != nullptr) m_planesOb->draw(cam, NONE);
    if (m_viewconesOb != nullptr) m_viewconesOb->draw(cam, NONE);
    if (m_textFactory != nullptr) m_textFactory->draw();
}
Dots* Scene::getDotsFactory() {
    if (m_dotsOb == nullptr) m_dotsOb = new Dots(this);
    return m_dotsOb;
}
SkyDots* Scene::getSkyDotsFactory() {
    if (m_skydotsOb == nullptr) m_skydotsOb = new SkyDots(this);
    return m_skydotsOb;
}
Cylinders* Scene::getCylindersFactory() {
    if (m_cylindersOb == nullptr) m_cylindersOb = new Cylinders(this);
    return m_cylindersOb;
}
Cones* Scene::getConesFactory() {
    if (m_conesOb == nullptr) m_conesOb = new Cones(this);
    return m_conesOb;
}
ViewCones* Scene::getViewConesFactory() {
    if (m_viewconesOb == nullptr) m_viewconesOb = new ViewCones(this);
    return m_viewconesOb;
}
Planes* Scene::getPlanesFactory() {
    if (m_planesOb == nullptr) m_planesOb = new Planes(this);
    return m_planesOb;
}
TextFactory* Scene::getTextFactory() {
    if (m_textFactory == nullptr) m_textFactory = new TextFactory(this);
    return m_textFactory;
}
SphereUV* Scene::getSphereUVFactory() {
    if (m_sphereuvOb == nullptr) m_sphereuvOb = new SphereUV(this);
    return m_sphereuvOb;
}
Arrows* Scene::getArrowsFactory() {
    if (m_arrowsOb == nullptr) m_arrowsOb = new Arrows(this);
    return m_arrowsOb;
}
AngleArcs* Scene::getAngleArcsFactory() {
    if (m_anglearcsOb == nullptr) m_anglearcsOb = new AngleArcs(this);
    return m_anglearcsOb;
}
CountryBorders* Scene::getCountryBordersFactory() {
    if (m_countrybordersOb == nullptr) m_countrybordersOb = new CountryBorders(this);
    return m_countrybordersOb;
}
TimeZones* Scene::getTimeZonesFactory() {
    if (m_timezonesOb == nullptr) m_timezonesOb = new TimeZones(this);
    return m_timezonesOb;
}
SkySphere* Scene::newSkysphere(unsigned int mU, unsigned int mV, bool texture) {
    if (m_skysphereOb == nullptr) m_skysphereOb = new SkySphere(this, mU, mV, texture);  // Make geometry configurable
    return m_skysphereOb;
}
SkySphere* Scene::getSkysphere() {
    return m_skysphereOb;
}
SkyBox* Scene::getSkyboxOb() {
    if (m_skyboxOb == nullptr) m_skyboxOb = new SkyBox(this);
    return m_skyboxOb;
}
SolarSystem* Scene::getSolsysOb() {
    if (m_solsysOb == nullptr) m_solsysOb = new SolarSystem(this, /* geocentric */ false);
    return m_solsysOb;
}
ShadowBox* Scene::getShadowboxOb() {
    if (m_shadowbox == nullptr) m_shadowbox = new ShadowBox(this, 2048, 2048);
    return m_shadowbox;
}
ShadowMap* Scene::getShadowmapOb() {
    if (m_shadowmap == nullptr) m_shadowmap = new ShadowMap(this, 2048, 2048);
    return m_shadowmap;
}
Earth* Scene::newEarth(std::string mode, unsigned int mU, unsigned int mV) {
    if (m_earthOb == nullptr) m_earthOb = new Earth(this, mode, mU, mV);
    return m_earthOb;
}
Earth* Scene::getEarth() {
    // NOTE: Don't use this unless you know what you are doing. It is meant to be an internal function.
    if (m_earthOb != nullptr) return m_earthOb;
    else {
        std::cout << "WARNING: Scene::getEarth(): was asked for Earth object, but none is available!\n";
        std::cout << " (ideally Scene::getEarth() should never be called from anywhere, is there for shadows (using SubSolar) until PointLight is implemented)\n";
        return nullptr;
    }
}
Minifigs* Scene::newMinifigs() { // Only single observer at the moment, fix this (like PolyCurve for example) !!!
    if (m_minifigsOb == nullptr) m_minifigsOb = new Minifigs(this);
    return m_minifigsOb;
}
PolyCurve* Scene::newPolyCurve(glm::vec4 color, float width, size_t reserve) {
    //std::cout << "Scene::newPolyCurve(): " << this << "\n";
    m_polycurves.emplace_back(new PolyCurve(this, color, width, reserve));
    return m_polycurves.back();
}
void Scene::deletePolyCurve(PolyCurve* curve) {
    auto it = std::find(m_polycurves.begin(), m_polycurves.end(), curve);
    if (it != m_polycurves.end()) std::swap(*it, m_polycurves.back());
    m_polycurves.pop_back();
    delete curve;
}
PolyLine* Scene::newPolyLine(glm::vec4 color, float width, size_t reserve) {
    m_polylines.emplace_back(new PolyLine(this, color, width, reserve));
    return m_polylines.back();
}
void Scene::deletePolyLine(PolyLine* curve) {
    auto it = std::find(m_polylines.begin(), m_polylines.end(), curve);
    if (it != m_polylines.end()) {
        std::swap(*it, m_polylines.back());
        m_polylines.pop_back();
        delete curve;
    }
    else {
        m_polylines.pop_back();
        delete curve;
    }
}
ThreePointSolver* Scene::newThreePointSolver(Earth* earth) {
    m_threepointsolvers.push_back(new ThreePointSolver(earth));
    return m_threepointsolvers.back();
}


// -----------
//  SceneTree
// -----------
// Meant to facilitate parenting of transformations and GUI traversal
SceneTree::SceneTree(Scene* scene) : m_scene(scene) {
    root = new SceneTreeRoot(m_scene, true);
    root->name = "root";
    return;
}
SceneTree::~SceneTree() {
    //delete root; // cascaded delete in SceneObject?
}
void SceneTree::updateBreathFirst() {
    //std::cout << "SceneTree::updateBreathFirst()\n";
    for (auto c : root->children) { // root is not a real scene object, so just add the children directly.
        breathfirst.push(c);
    }
    while (!breathfirst.empty()) {
        breathfirst.front()->inherit(); // Inherit world transformation of parent
        for (auto c : breathfirst.front()->children) {
            breathfirst.push(c);
        }
        breathfirst.pop();
    }
}
void SceneTree::drawBreathFirst(Camera* cam) {
    //std::cout << "SceneTree::updateBreathFirst()\n";
    for (auto c : root->children) { // root is not a real scene object, so just add the children directly.
        breathfirst.push(c);
    }
    while (!breathfirst.empty()) {
        if (!breathfirst.front()->hidden) breathfirst.front()->draw(cam); // Is hidden inheritable?
        for (auto c : breathfirst.front()->children) {
            breathfirst.push(c);
        }
        breathfirst.pop();
    }
}
void SceneTree::guiBreathFirst() {
    //std::cout << "SceneTree::updateBreathFirst()\n";
    for (auto c : root->children) { // root is not a real scene object, so just add the children directly.
        breathfirst.push(c);
    }
    while (!breathfirst.empty()) {
        if (breathfirst.front()->hasgui) breathfirst.front()->myGUI(); // Is hidden inheritable?
        for (auto c : breathfirst.front()->children) {
            breathfirst.push(c);
        }
        breathfirst.pop();
    }
}
void SceneTree::addSceneObject(SceneObject* object, SceneObject* parent) {
    //std::cout << "SceneTree::addSceneObject(): " << object << "." << object->name;

    if (parent == nullptr) {
        parent = root;
    }
    //std::cout << " Parent: " << parent << "." << parent->name; 
    //std::cout << "\n";
    parent->addChild(object);
}
void SceneTree::rootRemove(SceneObject* object) {
    root->children.remove(object);
}
void SceneTree::printSceneTree() {
    std::cout << "\nSceneTree dump:\n" << root << ":" << root->name << '\n';
    doPrintSceneTree(root->children, 0);
    std::cout << '\n';
}
void SceneTree::doPrintSceneTree(std::list<SceneObject*> node, unsigned int level) {
    // Recurse depth first
    std::string indent;
    for (auto n : node) { // bails gracefully if node is empty
        indent = "";
        for (unsigned int i = 0; i < level; i++) { indent += "   "; }
        indent += "-> ";
        std::cout << indent << n << ":" << n->name << '\n';
        doPrintSceneTree(n->children, level + 1);
    }
}


// --------
//  Camera
// --------
Camera::Camera(Scene* scene) : SceneObject(scene, nullptr) {
    //m_scene = scene;
    name = "camera";
    update();
}
void Camera::setLatLonFovDist(float lat, float lon, float fov, float dst) {
    camLat = lat;
    camLon = lon;
    camFoV = fov;
    camDst = dst;
    update();
}
void Camera::setCamLightPos(glm::vec3 lPos) {
    CamLightDir = glm::normalize(lPos - m_target);
}
void Camera::setLatLon(float lat, float lon) {
    if (lat != NO_FLOAT) camLat = lat;
    if (lon != NO_FLOAT) camLon = lon;
    update();
}
bool Camera::update() {
    setPosLLH({ camLat, camLon, camDst });
    updateLight();
    return false;
}
void Camera::updateLight() {
    glm::vec3 lPos = getPosXYZ();
    lPos += getRight() * -camlightsep;
    lPos += getUp() * camlightsep;
    setCamLightPos(lPos);
}
void Camera::setLookAt(glm::vec3 position, glm::vec3 target, glm::vec3 upwards) {
    this->position = position;
    m_target = target;
    worldUp = upwards;
    Recalc();
}
void Camera::setPosXYZ(glm::vec3 position) {
    this->position = position;
    Recalc();
}
void Camera::setPosLLH(LLD llh) {
    while (camLon > 180.0) camLon -= 360.0;
    while (camLon < -180.0) camLon += 360.0;
    // How to normalize latitude properly? Not practical as 100 should go to 80 and longitude should change 180 whereas 360 should simply go to 0.
    // More importantly, panning across a pole would end up with the camera upside down and ruin the up vector.
    // Better to just snap to valid range.
    if (camLat > 90.0) camLat = 90.0;
    if (camLat < -90.0) camLat = -90.0;
    float camW = (float)cos(deg2rad * llh.lat) * (float)llh.dst;
    position.x = (float)cos(deg2rad * llh.lon) * camW;
    position.y = (float)sin(deg2rad * llh.lon) * camW;
    position.z = (float)sin(deg2rad * llh.lat) * (float)llh.dst;
    Recalc();
}
glm::vec3 Camera::getPosXYZ() { return position; }
void Camera::setTarget(glm::vec3 target) {
    m_target = target;
    ViewMat = glm::lookAt(position, m_target, cameraUp);
    //Recalc();
}
void Camera::setFoV(float fov) { // !!! Is a public variable, so probably get rid of get/set methods and do one Recalc() every frame (after GUI etc)
    camFoV = fov;
    Recalc();
    //ProjMat = glm::perspective(glm::radians(m_fov), (float)m_world->w_width / (float)m_world->w_height, 0.1f, 100.0f);
}
float Camera::getFoV() { return camFoV; }
glm::mat4 Camera::getViewMat() { return ViewMat; }
glm::mat4 Camera::getSkyViewMat() { return glm::lookAt(glm::vec3(0.0f), m_target - position, cameraUp); }
glm::mat4 Camera::getProjMat() { return ProjMat; }
glm::vec3 Camera::getRight() { return cameraRight; }
glm::vec3 Camera::getUp() { return cameraUp; }
glm::vec3 Camera::getPosition() { return position; }
void Camera::dumpParameters(unsigned int frame) {
    if (frame == 0) std::cout << "Camera dump at frame: none \n";
    std::cout << "Camera dump at frame: " << frame << "\n";
    std::cout << " - camFoV = " << camFoV << "f\n";  // TODO: Add .0f formatting
    std::cout << " - camLat = " << camLat << "f\n";
    std::cout << " - camLon = " << camLon << "f\n";
    std::cout << " - camDst = " << camDst << "f\n";
}
void Camera::Recalc() {
    m_direction = glm::normalize(position - m_target);
    cameraRight = glm::normalize(glm::cross(worldUp, m_direction));
    cameraUp = glm::cross(m_direction, cameraRight); // Part of worldUp that fits with cameraDirection
    ViewMat = glm::lookAt(position, m_target, cameraUp);
    ProjMat = glm::perspective(glm::radians(camFoV), scene->getAspect(), camNear, camFar);
    updateLight();
}



// -----------
//  PolyCurve  SceneObject aware
// -----------
PolyCurveSO::PolyCurveSO(Scene* scene, SceneObject* parent, glm::vec4 color, float width, size_t reserve)
    : SceneObject(scene, parent) {
    name = "PolyCurve";
    if (reserve == NO_UINT) reserve = polycurvereserve;
    m_color = color;
    m_width = width;
    m_points.reserve(reserve + 2);   // Add a little headroom
    m_segments.reserve(reserve + 2); // When sizing it is easy to think of the number of points, and forget an extra one is used for closing the curve.
    facets = 16;
    m_verts.reserve(((size_t)facets + 1) * 2);
    m_tris.reserve((size_t)facets * 2);
    genGeom();
    shdr = scene->m_app->getShaderLib()->getShader(PRIMITIVESO_SHADER);
    vbl1 = new VertexBufferLayout;  // Vertices list
    vbl1->Push<float>(3);           // Vertex coord (pos)
    vbl1->Push<float>(3);           // vNormal coord
    vbl1->Push<float>(2);           // Texture coord
    vbl2 = new VertexBufferLayout;  // Primitives list
    vbl2->Push<float>(4);           // Instance color
    vbl2->Push<float>(3);           // Instance pos
    vbl2->Push<float>(3);           // Instance dir
    vbl2->Push<float>(3);           // Instance scale
    vbl2->Push<float>(1);           // Rotation about dir
    va = new VertexArray;
    vb1 = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
    vb2 = nullptr; // Will be built dynamically during each Draw() call
    //vb2 = new VertexBuffer(&m_segments[0], m_segments.capacity() * sizeof(Primitive3D));

    va->AddBuffer(*vb1, *vbl1, true);
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);  // IB uses COUNT, not BYTES!!!
}
PolyCurveSO::~PolyCurveSO() {
    delete vbl2;
    delete vbl1;
    delete ib;
    delete vb1;
    //delete vb2;  // Is deleted on every draw() call completion
    delete va;
}
void PolyCurveSO::setColor(glm::vec4 color) {
    m_color = color;
    // Should set a dirty flag and postpone generate() until update() is called.
    //generate();
    dirty = true;
}
void PolyCurveSO::setWidth(float width) {
    m_width = width;
    // Should set a dirty flag and postpone generate() until update() is called.
    //generate();
    dirty = true;
}
void PolyCurveSO::changePolyCurve(glm::vec4 color, float width) {
    if (color != NO_COLOR) m_color = color;
    if (width != NO_FLOAT) m_width = width;
    // Should set a dirty flag and postpone generate() until update() is called.
    //generate();
    dirty = true;
}
void PolyCurveSO::addPoint(glm::vec3 point) {
    //if (limit) std::cout << "WARNING PolyPolyCurve (" << this << ") adding beyond capacity, resizing!\n"; // only triggers if coming back after setting limit flag
    m_points.push_back(point);
    // Should set a dirty flag and let update() call generate() instead of hoping user will
    dirty = true;
    if (m_points.size() == m_points.capacity()) {
        std::cout << "WARNING: PolyCurve (" << this << ") capacity: " << m_points.capacity() << " reached. It will be SLOW to add new points now!\n";
        //    limit = true;
    }
}
void PolyCurveSO::clearPoints() {
    //std::cout << "Points: " << m_points.size() << ", Segments: " << m_segments.size() << "\n";
    m_points.clear();
    // Should set a dirty flag and let update() call generate() instead of hoping user will
    //m_segments.clear(); // Just in case someone would clear the points and not call generate() to update segments.
    dirty = true;
    //limit = false;
}
void PolyCurveSO::generate() {
    //std::cout << "PolyCurveSO::generate(): " << m_points.size() << " points." << "\n";
    bool debug = false; // Set to true to get green start and red end markers
    // Simply figure out the position, orientation and scale of each cylinder segment
    // and build instance table. Cylinders are actually instantiated on GPU
    m_segments.clear();
    if (m_points.size() == 0) return;
    for (size_t i = 1; i < m_points.size(); i++) {
        glm::vec3 pos = m_points[i - 1];
        glm::vec3 dir = m_points[i] - m_points[i - 1];
        glm::vec3 scale = glm::vec3(m_width, glm::length(dir), m_width);
        // color(4), pos(3), dir(3), scale(3), rot(1)
        if (debug) {
            if (i == 1) m_segments.push_back({ GREEN, pos, dir, scale, 0.0 });
            else if (i == m_points.size() - 1) m_segments.push_back({ RED, pos, dir, scale, 0.0 });
            else m_segments.push_back({ m_color, pos, dir, scale, 0.0 });
        }
        else m_segments.push_back({ m_color, pos, dir, scale, 0.0 });
    }
}
bool PolyCurveSO::update() {
    if (!dirty) return false;
    generate();
    dirty = false;
    return false;
}
void PolyCurveSO::draw(Camera* cam) {
    //std::cout << "PolyCurveSO::draw()\n";
    if (m_segments.size() == 0) return;
    shdr->Bind();
    glm::mat4 pv = cam->getProjMat() * cam->getViewMat();
    shdr->SetUniformMatrix4f("projview", pv);
    shdr->SetUniformMatrix4f("world", worldmatrix);
    //std::cout << glm::to_string(worldmatrix) << '\n';
    shdr->SetUniform3f("lightDir", cam->CamLightDir);
    vb1->Bind();
    va->Bind();
    ib->Bind();
    va->AddBuffer(*vb1, *vbl1, true);
    vb2 = new VertexBuffer(&m_segments[0], (unsigned int)m_segments.size() * sizeof(Primitive3D));
    // Can't use vb2->UpdateData(&m_segments[0], (unsigned int)m_segments.size() * sizeof(Primitive3D)); if size increases
    //std::cout << "PolyCurveSO::draw(): m_segments.size() = " << (unsigned int)m_segments.size() << '\n';
    va->AddBuffer(*vb2, *vbl2, false);
    // Primitives list (0,1,2 are in vertex list)
    glVertexAttribDivisor(3, 1);    // Color4
    glVertexAttribDivisor(4, 1);    // Pos3
    glVertexAttribDivisor(5, 1);    // Dir3
    glVertexAttribDivisor(6, 1);    // Scale3
    glVertexAttribDivisor(7, 1);    // Rot1
    glDrawElementsInstanced(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0, (GLsizei)m_segments.size());
    delete vb2;
}
void PolyCurveSO::genGeom() { // Single segment
    // NOT using the Cylinders primitive here, as we can make do with less facets
    float width = 1.0f;  // actually radius
    float height = 1.0f; // length of cylinder
    double lon;
    float clon;
    float clonw;
    float slon;
    float slonw;
    for (unsigned int u = 0; u <= facets; u++) { // Adds one extra vertex column to close the shape
        lon = tau * u / facets;                  //  This is simpler than indexing tris across seam
        clon = (float)cos(lon);
        clonw = clon * width;
        slon = (float)sin(lon);
        slonw = slon * width;
        // position(3), normal(3), uv(2)
        m_verts.emplace_back(glm::vec3(clonw, 1.0f, slonw), glm::vec3(clon, 0.0f, slon), glm::vec2(0.0f, 0.0f));
        m_verts.emplace_back(glm::vec3(clonw, 0.0f, slonw), glm::vec3(clon, 0.0f, slon), glm::vec2(0.0f, 0.0f));
        if (u < facets) {
            // indexA(1), indexB(1), indexC(1) CCW
            m_tris.emplace_back(u * 2, u * 2 + 1, u * 2 + 3);
            m_tris.emplace_back(u * 2, u * 2 + 3, u * 2 + 2);
        }
    }
}


// -------------
//  RenderLayer - base class
// -------------
RenderLayer::RenderLayer(float vpx1, float vpy1, float vpx2, float vpy2)
    : view_x1(vpx1), view_y1(vpy1), view_x2(vpx2), view_y2(vpy2) {
}
void RenderLayer::updateView(float w, float h) {
    // Called when window size changes. Also called without parameters when animating the view size/location
    if (w != 0.0f) width = w;
    if (h != 0.0f) height = h;
    vp_x = view_x1 * width;
    vp_y = view_y1 * height;
    vp_w = (view_x2 - view_x1) * width;
    vp_h = (view_y2 - view_y1) * height;  // Might check if zero, or risk the wrath of a DIV0 exception (when calculating aspect ratio in RenderLayer3D for example)!!!
}
void RenderLayer::animateView(float vx1, float vy1, float vx2, float vy2, unsigned int frames) {
    target_x1 = vx1;
    target_y1 = vy1;
    target_x2 = vx2;
    target_y2 = vy2;
    frames_left = frames;
    animating = true;
}
void RenderLayer::animate() {
    if (!animating) return;
    view_x1 += (target_x1 - view_x1) / frames_left;
    view_y1 += (target_y1 - view_y1) / frames_left;
    view_x2 += (target_x2 - view_x2) / frames_left;
    view_y2 += (target_y2 - view_y2) / frames_left;
    updateView();
    frames_left--;
    if (frames_left == 0) animating = false;
}


// ---------------
//  RenderLayer3D
// ---------------
RenderLayer3D::RenderLayer3D(float vpx1, float vpy1, float vpx2, float vpy2, Scene* scene, Astronomy* astro, Camera* cam, bool overlay)
    : m_scene(scene), m_astro(astro), m_cam(cam), m_overlay(overlay), RenderLayer(vpx1, vpy1, vpx2, vpy2) {

    m_scene->astro = m_astro;
    float w = (float)m_scene->m_app->getWidth();
    float h = (float)m_scene->m_app->getHeight();
    updateViewport(w, h);
    type = LAYER3D;
}
void RenderLayer3D::setCamera(Camera* cam) {
    m_cam = cam;
}
void RenderLayer3D::render() {
    glViewport((GLint)vp_x, (GLint)vp_y, (GLsizei)vp_w, (GLsizei)vp_h);
    //std::cout << "RenderLayer3D vp: " << vp_x << ", " << vp_y << ", " << vp_w << ", " << vp_h << "\n";
    if (!m_overlay) {
        glScissor((GLint)vp_x, (GLint)vp_y, (GLsizei)vp_w, (GLsizei)vp_h);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // GL_COLOR_BUFFER_BIT |  Does not clear correctly, it blanks the whole window instead !!!
        glDisable(GL_SCISSOR_TEST);
    }
    else { glClear(GL_DEPTH_BUFFER_BIT); }
    m_scene->render(m_cam);
}
void RenderLayer3D::updateViewport(float w, float h) {
    updateView(w, h);

    m_scene->setAspect(vp_w / vp_h);  // Camera(s) can pick up the aspect ratio from the Scene
}
void RenderLayer3D::animateViewport() {
    animate();
    m_scene->setAspect(vp_w / vp_h);  // Camera(s) can pick up the aspect ratio from the Scene
}


// -----------------
//  RenderLayerText
// -----------------
// Ideally the following parameters can be controlled in individual lines of text: Actual text, Color, Position

RenderLayerText::RenderLayerText(float vpx1, float vpy1, float vpx2, float vpy2, Application* app, RenderLayerTextLines* lines)
    : m_lines(lines), m_app(app), RenderLayer(vpx1, vpy1, vpx2, vpy2) {
    // If lines == nullptr no lines are printed, except UTC datetime if setAstronomy() has been called
    // also astro access so it can print datetimes etc autonomously? No, submit text changes via lines
    float w = (float)m_app->getWidth();
    float h = (float)m_app->getHeight();
    updateView(w, h);
    type = LAYERTEXT;

    timestr = "yyyy-mm-dd hh:mm:ss\n";
}
void RenderLayerText::setFont(ImFont* font) {
    m_font = font;
}
//void RenderLayerText::setFont(std::string* path, unsigned int size) {
//    ImGuiIO& io = ImGui::GetIO();
//    m_font = io.Fonts->AddFontFromFileTTF("C:\\Coding\\Eartharium\\Eartharium\\textures\\cour.ttf", 36);
//    // AddFont* is called, but maybe font is already loaded. Can various ImGUI layers cooperate on this sort of thing?
//}
void RenderLayerText::setAstronomy(Astronomy* astro) {
    m_astro = astro;
}
void RenderLayerText::updateViewport(float w, float h) {} // TODO: Scale texts to fit viewport? Render to temp fbo first? !!!
void RenderLayerText::animateViewport() {
    animate();
}
void RenderLayerText::render() {
    // Compose elements when they are configured, only do actual render here?
    if (!m_app->imgui_ready) return;
    //m_app->beginImGUI();

    // Always display UTC time if there is an astro object tracking time
    if (m_astro != nullptr) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 0.75f));
        ImGui::PushFont(m_font);
        ImGui::Begin("Date and Time UTC", nullptr, ImGuiWindowFlags_NoDecoration); // | ImGuiWindowFlags_NoBackground); // Create a window and append into it.
        //        std::string datetime;
        //        m_astro->getTimeString(datetime);
        std::string datetime = m_astro->getTimeString();
        ImGui::Text(datetime.c_str());
        ImGui::PopFont();
        ImGui::PopStyleColor();
        ImGui::End();
    }
    // TODO: Set back to check if there are text lines when done debugging !!!
    if (m_lines != nullptr && !m_lines->empty()) {
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.06f, 0.06f, 0.06f, 0.75f));
        ImGui::PushFont(m_font);
        ImGui::Begin("Text Lines", nullptr, ImGuiWindowFlags_NoDecoration); // | ImGuiWindowFlags_NoBackground); // Create a window and append into it.
        //ImGui::Text("Line 1");
        //ImGui::Text("Line 2");
        for (unsigned int i = 0; i < m_lines->size(); i++) {
            // Note: in case of missing lines at the end, check size in imgui.ini (in Release folder), or set up with ImGui::SetWindowSize()/SetWindowPos()
            ImGui::Text(m_lines->lines[i]->c_str());
        }
        ImGui::PopFont();
        ImGui::PopStyleColor();
        ImGui::End();
    }
    //m_app->endImGUI();
}


// ----------------
//  RenderLayerGUI
// ----------------
RenderLayerGUI::RenderLayerGUI(float vpx1, float vpy1, float vpx2, float vpy2, Application* app)
    : m_app(app), RenderLayer(vpx1, vpy1, vpx2, vpy2) {
    timestr = "yyyy-mm-dd hh:mm:ss\n";
    type = LAYERGUI;
}
//void RenderLayerGUI::addScene(Scene* scene, std::string scenename) {
//    m_scenes.push_back({ scene, scenename });
//}
void RenderLayerGUI::addLayer3D(RenderLayer3D* layer, std::string layername) {
    m_layers.push_back({ layer, layername });
}
void RenderLayerGUI::animateViewport() {
    animate();
}
void RenderLayerGUI::render() {
    //return;
    if (!m_app->imgui_ready) return;
    if (m_app->gui) {
        ImGui::Begin("Eartharium"); // Create a window and append into it.
        //ImGui::PushItemWidth(12.0f * ImGui::GetFontSize());
        ImGui::Separator();
        ImGui::Text("Computational load:");
        ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        //ImGui::PopItemWidth();

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {

            for (auto& l : m_layers) {
                if (ImGui::BeginTabItem(l.name.c_str())) {
                    // Camera
                    // Sets app.currentCam so keyboard handler can update the parameters
                    if (ImGui::CollapsingHeader("Camera")) {
                        //l.layer->m_scene->m_app->currentCam = l.layer->m_scene->w_camera;
                        Camera* curcam = l.layer->m_cam;
                        //ImGui::SliderFloat("CamLat", &l.layer->m_scene->w_camera->camLat, -90.0f, 90.0f);
                        //ImGui::SliderFloat("CamLon", &l.layer->m_scene->w_camera->camLon, -179.9f, 180.0f);
                        //ImGui::SliderFloat("FoV", &l.layer->m_scene->w_camera->camFoV, CAMERA_MIN_FOV, CAMERA_MAX_FOV);
                        //ImGui::SliderFloat("Dist", &l.layer->m_scene->w_camera->camDst, CAMERA_MIN_DIST, CAMERA_MAX_DIST);
                        //ImGui::SliderFloat("Clip Near", &l.layer->m_scene->w_camera->camNear, 0.01f, 5.0f);
                        //ImGui::SliderFloat("Clip Far", &l.layer->m_scene->w_camera->camFar, 10.0f, 15000.0f);
                        ImGui::SliderFloat("CamLat", &curcam->camLat, -90.0f, 90.0f);
                        ImGui::SliderFloat("CamLon", &curcam->camLon, -179.9f, 180.0f);
                        ImGui::SliderFloat("FoV", &curcam->camFoV, CAMERA_MIN_FOV, CAMERA_MAX_FOV);
                        ImGui::SliderFloat("Dist", &curcam->camDst, CAMERA_MIN_DIST, CAMERA_MAX_DIST);
                        ImGui::SliderFloat("Clip Near", &curcam->camNear, 0.01f, 5.0f);
                        ImGui::SliderFloat("Clip Far", &curcam->camFar, 10.0f, 15000.0f);
                        //ImGui::SliderFloat("Cam<->Light", &camlightsep, 0.0f, 10.0f);  // Edit 1 float using a slider from 0.0f to 1.0f
                        if (ImGui::Button("Himawari-8")) {
                            l.layer->m_scene->w_camera->setLatLonFovDist(0.03f, 140.7f, 21.0f, 5.612f);
                        }
                        if (ImGui::Button("GOES-17")) {
                            l.layer->m_scene->w_camera->setLatLonFovDist(0.1f, -137.0f, 21.0f, 5.612f);
                        }
                    }
                    // Time
                    static double eot = 0.0;
                    if (ImGui::CollapsingHeader("Time")) {
                        ImGui::Text(l.layer->m_astro->timestr.c_str()); // At top because it should show the current time. Below may modify time.
                        ImGui::Checkbox("Equation of Time", &do_eot);
                        ImGui::SameLine();
                        if (ImGui::Button("-")) { l.layer->m_astro->addTime(0.0, 0.0, l.layer->m_scene->astro->getEoT(), 0.0, do_eot); }
                        ImGui::SameLine();
                        if (ImGui::Button("+")) { l.layer->m_astro->addTime(0.0, 0.0, -l.layer->m_scene->astro->getEoT(), 0.0, do_eot); }
                        struct DefTimeStep {
                            long yr; long mo; double da; double hr; double mi;  double se;
                            bool operator==(DefTimeStep& other) {
                                return (yr == other.yr && mo == other.mo && da == other.da && hr == other.hr && mi == other.mi && se == other.se);
                            }
                        };
                        static DefTimeStep mytimestep = { 0, 0, 0.0, 0.0, 0.0, 0.0 };
                        struct EnumDesc { DefTimeStep Value; const char* Name; };
                        static EnumDesc timesteps[] =
                        {
                            { { 0, 0, 0.0, 0.0, 0.0, 0.0 }, "None" },
                            { { 0, 0, 365.256363004, 0.0, 0.0, 0.0 }, "Sidereal Year" },
                            { { 0, 0, 365.242190402, 0.0, 0.0, 0.0 }, "Tropical Year" },
                            { { 0, 0, 365.259636, 0.0, 0.0, 0.0 }, "Anomalistic Year" },
                            // { { 1, 0, 0.0, 0.0, 0.0, 0.0 }, "Calendar Year" },  // Can't pass year or month to Astronomy->addTime() !!!
                            //{ { 0, 1, 0.0, 0.0, 0.0, 0.0 }, "Calendar Month" },
                            { { 0, 0, 0.0, 23.0, 56.0, 4.0905 }, "Sidereal Day" },
                            { { 0, 0, 1.0, 0.0, 0.0, 0.0 }, "Calendar Day" },
                            { { 0, 0, 1.035028, 0.0, 0.0, 0.0 }, "Lunar Day" },
                            { { 0, 0, 0.0, 1.0, 0.0, 0.0 }, "1 Hour" },
                            { { 0, 0, 0.0, 0.0, 15.0, 0.0 }, "15 Minutes" },
                            { { 0, 0, 0.0, 0.0, 4.0, 0.0 }, "4 Minutes" }, // Sun moves ~1 degree
                            { { 0, 0, 0.0, 0.0, 1.0, 0.0 }, "1 Minute" }
                        };
                        static int idx;
                        //mytimestep = timesteps[6].Value; // Hours by default
                        for (idx = 0; idx < IM_ARRAYSIZE(timesteps); idx++) {
                            if (timesteps[idx].Value == mytimestep) break;
                        }
                        static const char* preview_text = timesteps[idx].Name;
                        mytimestep = timesteps[idx].Value;
                        if (ImGui::BeginCombo("Step", preview_text)) {
                            for (int n = 0; n < IM_ARRAYSIZE(timesteps); n++)
                                if (ImGui::Selectable(timesteps[n].Name, idx == n)) {
                                    mytimestep = timesteps[n].Value;
                                    preview_text = timesteps[n].Name;
                                }
                            ImGui::EndCombo();
                        }
                        //ImGui::SameLine();
                        if (ImGui::Button("-1 Step")) {
                            l.layer->m_astro->addTime(-mytimestep.da, -mytimestep.hr, -mytimestep.mi, -mytimestep.se, do_eot);
                            //std::cout << "Subtracted time (d,h,m,s): " << mytimestep.da << "," << mytimestep.hr << "," << mytimestep.mi << "," << mytimestep.se << '\n';
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("+1 Step")) {
                            l.layer->m_astro->addTime(mytimestep.da, mytimestep.hr, mytimestep.mi, mytimestep.se, do_eot);
                            //std::cout << "Added time (d,h,m,s): " << mytimestep.da << "," << mytimestep.hr << "," << mytimestep.mi << "," << mytimestep.se << '\n';
                        }
                        ImGui::SliderFloat("Time of Day", &slideday, 0.0f, 24.0f);
                        if (prevslideday != slideday) {
                            // Should probably do two addTime() passes (-prev, then +curr), due to EoT I think.
                            l.layer->m_astro->addTime(0.0, (double)slideday - (double)prevslideday, 0.0, 0.0, do_eot);
                            prevslideday = slideday;
                        }
                        //ImGui::SliderFloat("year", &slideyear, 0.0f, 365.0f);
                    }
                    // Loop through SceneTree and add GUI elements as needed
                    l.layer->m_scene->scenetree->guiBreathFirst();
                    // DetailedEarth
                    //if (l.layer->m_scene->m_dearthOb) l.layer->m_scene->m_dearthOb->myGUI();
                    // DetailedMoon
                    //if (l.layer->m_scene->m_dmoonOb) l.layer->m_scene->m_dmoonOb->myGUI();
                    // DetailedSky
                    //if (l.layer->m_scene->m_dskyOb) l.layer->m_scene->m_dskyOb->myGUI();
                    // Solar System object
                    if (l.layer->m_scene->m_solsysOb != nullptr) {
                        if (ImGui::CollapsingHeader("Solar System")) {
                            ImGui::Checkbox("Geocentric", &l.layer->m_scene->m_solsysOb->geocentric);
                            ImGui::Checkbox("Orbits", &l.layer->m_scene->m_solsysOb->orbits);
                            ImGui::Checkbox("Trails", &l.layer->m_scene->m_solsysOb->trails);
                            ImGui::SameLine();
                            if (ImGui::Button("Clear")) { l.layer->m_scene->m_solsysOb->clearTrails(); }
                            ImGui::SliderInt("Trail length", &l.layer->m_scene->m_solsysOb->traillen, 2, 1000);
                        }
                    }
                    // Earth object
                    if (l.layer->m_scene->m_earthOb != nullptr) {
                        // Earth is expanded, so set current Earth in Application
                        l.layer->m_scene->m_app->currentEarth = l.layer->m_scene->m_earthOb;
                        if (ImGui::CollapsingHeader("Earth")) {
                            ImGui::SliderFloat("Tex X", &l.layer->m_scene->m_app->currentEarth->texture_x, -10.0f, 10.0f);
                            ImGui::SliderFloat("Tex Y", &l.layer->m_scene->m_app->currentEarth->texture_y, -10.0f, 10.0f);
                            const char* items[] = { "AENS", "AEER", "AERC", "AEE8", "NSER", "NSRC", "NSE8", "ERRC", "ERE8", "RCE8" };
                            if (ImGui::BeginCombo("Earth type", l.layer->m_scene->m_app->currentEarth->current_earth.c_str())) {
                                for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
                                    bool is_selected = (l.layer->m_scene->m_app->currentEarth->current_earth == items[n]);
                                    if (ImGui::Selectable(items[n], is_selected)) {
                                        l.layer->m_scene->m_app->currentEarth->current_earth = (std::string)items[n];
                                        param = 0.0f;
                                    }
                                    if (is_selected) ImGui::SetItemDefaultFocus();
                                }
                                ImGui::EndCombo();
                            }
                            ImGui::SliderFloat("Param", &l.layer->m_scene->m_app->currentEarth->param, 0.0f, 1.0f);
                            // Overlay colors
                            ImGui::ColorEdit4("Tropics", (float*)&l.layer->m_scene->m_app->currentEarth->tropics, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                            ImGui::SameLine();
                            ImGui::Checkbox("Tropics Overlay", &l.layer->m_scene->m_app->currentEarth->tropicsoverlay);
                            ImGui::ColorEdit4("Arctics", (float*)&l.layer->m_scene->m_app->currentEarth->arctics, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
                            ImGui::SameLine();
                            ImGui::Checkbox("Arctics Overlay", &l.layer->m_scene->m_app->currentEarth->arcticsoverlay);
                            ImGui::Checkbox("Lunar insolation", &l.layer->m_scene->m_app->currentEarth->w_linsol);
                        }
                        if (ImGui::CollapsingHeader("Sun")) {
                            ImGui::Checkbox("Solar Insolation", &l.layer->m_scene->m_app->currentEarth->insolation);
                            ImGui::Checkbox("Solar Twilight", &l.layer->m_scene->m_app->currentEarth->w_twilight);
                            ImGui::Checkbox("Solar Refraction", &l.layer->m_scene->m_app->currentEarth->w_refract);
                            ImGui::SliderFloat("Sun Height", &l.layer->m_scene->m_app->currentEarth->flatsunheight, 0.0f, 100000.0f);
                        }
                    }
                    // Sky Sphere object - must revise to allow multiple SkySphere objects !!!
                    if (l.layer->m_scene->m_skysphereOb != nullptr) { // Oh, referencing m_scene without a scene added is causing a crash
                        if (ImGui::CollapsingHeader("Sky Sphere")) {
                            ImGui::Checkbox("Textured", &l.layer->m_scene->m_skysphereOb->m_texture);
                            ImGui::Checkbox("Sky Dots", &l.layer->m_scene->getSkyDotsFactory()->visible);
                            // FIXME: Mode selector here !!!
                            ImGui::SliderFloat("Dome Height", &l.layer->m_scene->m_skysphereOb->domeheight, 0.1f, 10000.0f);

                        }
                    }
                    if (ImGui::CollapsingHeader("SIO Sextant")) {
                        ImGui::Checkbox("Dip correction", &l.layer->m_scene->m_app->sio_dip);
                        ImGui::Checkbox("Refraction", &l.layer->m_scene->m_app->sio_refract);
                        ImGui::Checkbox("Solar Limb", &l.layer->m_scene->m_app->sio_sunlimb);
                        ImGui::Checkbox("Local Weather", &l.layer->m_scene->m_app->sio_local_weather);
                        if (ImGui::RadioButton("Bennett", (l.layer->m_scene->m_app->sio_refmethod == REFR_BENNETT))) l.layer->m_scene->m_app->sio_refmethod = REFR_BENNETT;
                        if (ImGui::RadioButton("Almanac", (l.layer->m_scene->m_app->sio_refmethod == REFR_ALMANAC))) l.layer->m_scene->m_app->sio_refmethod = REFR_ALMANAC;
                        ImGui::SliderFloat("Pressure", &l.layer->m_scene->m_app->sio_pressure, 800.0f, 1100.0f);
                        ImGui::SliderFloat("Temperature", &l.layer->m_scene->m_app->sio_temperature, -40.0f, 50.0f);
                        ImGui::SliderFloat("Obs. Height", &l.layer->m_scene->m_app->sio_height, 0.0f, 250.0f);
                        ImGui::SliderFloat("Curve width", &l.layer->m_scene->m_app->sio_pathwidth, 0.0001f, 0.0010f, "%.4g");
                    }
                    // Lunalemma object
                    if (ImGui::CollapsingHeader("Lunalemma")) {
                        ImGui::SliderFloat("Seconds offset: ", &l.layer->m_scene->m_app->lunalemmaOffset, -600.0f, 600.0f);
                    }
                    ImGui::EndTabItem();
                }
            }

            ImGui::SliderFloat("Custom 1", &m_app->customparam1, m_app->customlow1, m_app->customhigh1);
            ImGui::SliderFloat("Custom 2", &m_app->customparam2, m_app->customlow2, m_app->customhigh2);
            ImGui::EndTabBar();
        }

        //if (m_shadowbox != nullptr) {
        //    if (ImGui::CollapsingHeader("Shadows")) {
        //        ImGui::SliderFloat("Near", &GetShadowBoxOb()->near, 0.00001f, 1.0f);
        //        ImGui::SliderFloat("Far", &GetShadowBoxOb()->far, 1.0f, 25.0f);
        //        //ImGui::SliderFloat("Cam<->Light", &camlightsep, 0.0f, 10.0f);
        //    }
        //}
        //ImGui::ShowDemoWindow();
        ImGui::End();
    }
}


// -----------------
//  RenderLayerPlot
// -----------------
// Should offer to plot any parameter of any object in the scenes. So will need pointers, types etc. Maybe accept a CelestialPath and expand from there?
// NOTE: Add a plot tab in GUI and implement an interactive checkbox to set/clear RenderLayerPlot::interactive
RenderLayerPlot::RenderLayerPlot(float vpx1, float vpy1, float vpx2, float vpy2, Application* app) : m_app(app), RenderLayer(vpx1, vpy1, vpx2, vpy2) {
    // Set up plot specific stuff
    type = LAYERPLOT;
    // Get Window size from app
    updateViewport((float)m_app->getWidth(), (float)m_app->getHeight());
}
void RenderLayerPlot::updateViewport(float w, float h) {
    updateView(w, h); // Call base class function
}
void RenderLayerPlot::animateViewport() {
    animate();
}
void RenderLayerPlot::render() {
    if (!m_app->imgui_ready) return;
    //ImPlot::ShowDemoWindow();

    // NOTE: When plotting time dependent data, use Unix Timestamps and enable ImPlotAxisFlags_Time on x-axis
    // (Astronomy::getJD2UnixTime() may help, pass 0.0 or nothing to get current JD as Unix TS)
    ImGui::SetNextWindowPos(ImVec2(vp_x, vp_y)); // , ImGuiCond_Always);  // , ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(vp_w, vp_h)); //, ImGuiCond_Always); // , ImGuiCond_Once);
    ImGui::Begin("Test", nullptr, ImGuiWindowFlags_NoDecoration);

    ImGui::Checkbox("Local Time", &ImPlot::GetStyle().UseLocalTime);
    ImGui::SameLine();
    ImGui::Checkbox("ISO 8601", &ImPlot::GetStyle().UseISO8601);
    ImGui::SameLine();
    ImGui::Checkbox("24 Hour Clock", &ImPlot::GetStyle().Use24HourClock);
    if (ImPlot::BeginPlot("Line Plot", ImVec2(-1, -1))) {
        ImPlot::SetupAxesLimits(start_time, end_time, bottom_value, top_value, interactive ? 0 : ImPlotCond_Always);
        ImPlot::SetupAxis(ImAxis_X1, "Time in UTC", ImPlotAxisFlags_Time); // | ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxis(ImAxis_Y1, "Elevation Angle");
        //std::cout << "Start, End time: " << start_time << ", " << end_time << "\n";
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Circle);
        ImPlot::SetNextLineStyle(m_predcolor, 2.0f);
        ImPlot::PlotLine("Globe prediction", &m_pred->begin()->utime, &m_pred->begin()->data, (int)m_pred->size(), 0, (int)sizeof(TimePlotData));
        ImPlot::SetNextMarkerStyle(ImPlotMarker_Diamond);
        ImPlot::SetNextLineStyle(m_datacolor, 1.0f);
        ImPlot::PlotStems("Measurements", &m_data->begin()->utime, &m_data->begin()->data, (int)m_data->size(), 0.0, 0, (int)sizeof(TimePlotData));
        ImPlot::PlotVLines("VLines", &current_time, 1);
        ImPlot::EndPlot();
    }
    //std::cout << "m_data.size(): " << m_data->size() << "\n";
    //for (auto pt : *m_data) {
    //    std::cout << pt.utime << " : " << pt.data << "\n";
    //}
    ImGui::End();
}
unsigned int RenderLayerPlot::addTimeSeries(unsigned int type) { return 0; }
void RenderLayerPlot::addTimePoint(unsigned int series, double time, double data) { }
void RenderLayerPlot::plotSeries(std::vector<TimePlotData>& data, glm::vec4 color) {
    m_datacolor = ImVec4(color.r, color.g, color.b, color.a);
    m_data = &data;
}
void RenderLayerPlot::predictSeries(std::vector<TimePlotData>& data, glm::vec4 color) {
    m_predcolor = ImVec4(color.r, color.g, color.b, color.a);
    m_pred = &data;
}
void RenderLayerPlot::setStartEnd(double start, double end) {
    start_time = start;
    end_time = end;
}
void RenderLayerPlot::setTopBottom(double top, double bottom) {
    top_value = top;
    bottom_value = bottom;
}
void RenderLayerPlot::setCurrentTime(double time) {
    current_time = time;
}


// -------------
//  Application
// -------------
Application::Application() {
    // Set up everyhing that is non-optional and singular (e.g. Main Window)
    m_shaderlib = new ShaderLibrary();
    m_texturelib = new TextureLibrary();
    m_layers.reserve(16); // ?? Might need to be a list instead of a vector

}
// DESTRUCTOR: Should ideally tear down the things we set up

ShaderLibrary* Application::getShaderLib() { return m_shaderlib; }
TextureLibrary* Application::getTextureLib() { return m_texturelib; }
Astronomy* Application::newAstronomy() { return new Astronomy(); }  // These may want to save references for clean-up, or maybe not !!!
Scene* Application::newScene() { return new Scene(this); }          // Or they may go the way of the Dodo :)
int Application::initWindow() {
    if (start_fullscreen) w_width = 1920;
    else w_width = 1280;
    if (start_fullscreen) w_height = 1080;
    else w_height = 720;
    // Setup platform environment
    window = setupEnv(w_width, w_height, 4, 6, start_fullscreen, opengl_debug_info);

    if (window == NULL) {
        std::cout << "ERROR! Application::initWindow(): Call to setupEnv() failed. Unable to create window or openGL context.\n";
        return -1;
    }
    // Save settings for switching to and from fullscreen
    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);
    isfullscreen = false;
    togglefullwin = start_fullscreen;    // If start_fullscreen is true, window will toggle to full on next loop

    glfwGetWindowPos(window, &w_posx, &w_posy);

    initImGUI();  // Might need to check for success and pass return code

    return 0;
}
void Application::setFullScreen() {
    if (isfullscreen) return;
    // Toggle to full screen mode
    win_width = w_width;
    win_height = w_height;
    glfwGetWindowPos(window, &w_posx, &w_posy);
    glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    w_width = mode->width;
    w_height = mode->height;
    togglefullwin = false;
    isfullscreen = true;
    updateView(w_width, w_height);
}
void Application::setWindowed(int width, int height) {
    // Toggle to windowed mode - values defaults to previous windowed size if none are given
    // Don't check if windowed or fullscreen, is also used to change window size while windowed.
    if (width == 0) width = win_width;
    if (height == 0) height = win_height;
    w_width = width;
    w_height = height;
    glfwSetWindowMonitor(window, nullptr, w_posx, w_posy, w_width, w_height, mode->refreshRate);
    togglefullwin = false;
    isfullscreen = false;
    updateView(w_width, w_height);
}
void Application::SetWH(int w, int h) {
    // NOTE: This is an internal callback for GFLW, use setWindowed(int width, int height) to set window size
    w_width = w;
    w_height = h;
    updateView(w_width, w_height);
}
int Application::getWidth() { return w_width; }
int Application::getHeight() { return w_height; }
void Application::update() {
    glfwPollEvents();    // Should cause callbacks to be triggered
    if (togglefullwin && !isfullscreen) { setFullScreen(); }
    if (togglefullwin && isfullscreen) { setWindowed(win_width, win_height); }
    if (currentCam != nullptr) currentCam->update();
    if (dumpcam && currentCam != nullptr) {
        currentCam->dumpParameters(currentframe);
        dumpcam = false;
    }
    if (dumptime && currentEarth != nullptr) {
        currentEarth->m_scene->astro->dumpCurrentTime(currentframe);
        dumptime = false;
    }
}
bool Application::shouldClose() {
    return glfwWindowShouldClose(window);
}
float Application::getAspect() {
    // When rendering to the application window, and that window is out of focus/view on the screen, this returns 0.0f !!!
    if (w_height == 0) {
        //std::cout << "ERROR! Application::getAspect(): m_winVP.vp_h is zero, so aspect ratio will cause DIV0 exception! Returning 0.0f instead.\n";
        return 0.0f;
    }
    return (float)w_width / (float)w_height;
}
// ImGUI stuff
void Application::initImGUI() {
    if (imgui_ready) return; // Don't init twice
    const char* glsl_version = "#version 460 core";
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGuiIO& io = ImGui::GetIO();
    m_font1 = io.Fonts->AddFontFromFileTTF("C:\\Coding\\Eartharium\\Eartharium\\textures\\cour.ttf", 16);
    m_font2 = io.Fonts->AddFontFromFileTTF("C:\\Coding\\Eartharium\\Eartharium\\textures\\cour.ttf", 36);

    ImPlot::CreateContext(); // For plot layers only needs init once

    imgui_ready = true;
}
void Application::beginImGUI() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void Application::endImGUI() {
    //ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
RenderLayer3D* Application::newLayer3D(float vpx1, float vpy1, float vpx2, float vpy2, Scene* scene, Astronomy* astro, Camera* cam, bool overlay) {
    if (cam == nullptr) cam = scene->w_camera;
    RenderLayer3D* layer = new RenderLayer3D(vpx1, vpy1, vpx2, vpy2, scene, astro, cam, overlay);
    m_layers.push_back(layer);
    return layer;
}
void Application::deleteLayer3D(RenderLayer3D* layer) {
    delete layer;
}
RenderLayerText* Application::newLayerText(float vpx1, float vpy1, float vpx2, float vpy2, RenderLayerTextLines* lines) {
    RenderLayerText* layer = new RenderLayerText(vpx1, vpy1, vpx2, vpy2, this, lines);
    m_layers.push_back(layer);
    return layer;
}
void Application::deleteLayerText(RenderLayerText* layer) {
    delete layer;
}
RenderLayerGUI* Application::newLayerGUI(float vpx1, float vpy1, float vpx2, float vpy2) {
    RenderLayerGUI* layer = new RenderLayerGUI(vpx1, vpy1, vpx2, vpy2, this);
    m_layers.push_back(layer);
    return layer;
}
void Application::deleteLayerGUI(RenderLayerGUI* layer) {
    delete layer;
}
RenderLayerPlot* Application::newLayerPlot(float vpx1, float vpy1, float vpx2, float vpy2) {
    RenderLayerPlot* layer = new RenderLayerPlot(vpx1, vpy1, vpx2, vpy2, this);
    m_layers.push_back(layer);
    return layer;
}
void Application::deleteLayerPlot(RenderLayerPlot* layer) {
    delete layer;
}
void Application::updateView(int w, int h) {
    // Called from Application when switching between full screen and windowed mode, ADD: or when window is resized !!!
    float width = (float)w;
    float height = (float)h;
    if (output_fbo == 0) {
        for (auto& l : m_layers) {
            l->updateViewport(width, height);
        }
    }
    else {
        for (auto& l : m_layers) {
            l->updateViewport((float)output_width, (float)output_height);
        }
    }
}
void Application::render() {
    // Avoid to render to a window that is minimized or fully obscured, as that will have zero aspect ratio !!!
    update();
    if (output_fbo != 0 && renderoutput == true) {
        glBindFramebuffer(GL_FRAMEBUFFER, output_fbo);
        glViewport(0, 0, output_width, output_height);
        glScissor(0, 0, output_width, output_height);
    }
    else {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, w_width, w_height);
        glScissor(0, 0, w_width, w_height);
    }
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto& l : m_layers) {
        if (l->type == LAYER3D) {
            l->animateViewport();
            l->render();
        }
    }
    beginImGUI();  // RenderLayerText and RenderLayerPlot uses ImGUI to draw
    for (auto& l : m_layers) {       // This won't work for output_fbo, it will render to the window and not to the output framebuffer !!!
        if (l->type == LAYERPLOT) {
            l->animateViewport();
            l->render();
        }
    }
    for (auto& l : m_layers) {
        if (l->type == LAYERTEXT) {
            l->animateViewport();
            l->render();
        }
    }
    if (output_fbo != 0 && renderoutput == true) {
        // Blit to the app window
        // Create 2 tri's to fill the window
        // Render them with the texture of the fbo using a simple blitting shader
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, w_width, w_height);
        glScissor(0, 0, w_width, w_height);
        // No need to clear the window, we will paint all of it with the RenderLayer3D output
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Shader* shdr = getShaderLib()->getShader(BLIT_SHADER);
        shdr->Bind();
        shdr->SetUniform1i("screenTexture", GL_TEXTURE7 - GL_TEXTURE0);
        glBindVertexArray(quadVAO);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        //glFrontFace(GL_CCW);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, output_texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glFrontFace(GL_CW);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        shdr->Unbind();
    }
    ////unsigned int i = 0;
    ////while (glGetError() != GL_NO_ERROR) {
    ////    i++;
    ////}
    //// Resolve simply by blitting to window
    //// NOTE: Switched off MS in application window, to avoid GL_INVALID_OPERATION when output_fbo and window are not the same size !!!
    ////       Even so, I still get errors when window and output_fbo have different sizes. Might be a mismatch between float/int internal formats? !!!
    //
    //// Note: Intel UHD 630 drivers give incorrect values for GL_SAMPLES!
    ////GLint parm = 0;
    ////glGetNamedFramebufferParameteriv(output_fbo, GL_SAMPLES, &parm);
    ////std::cout << "output_fbo(" << output_fbo << ") GL_SAMPLES: " << parm << "\n";
    ////GLint parm2 = 0;
    ////glGetNamedFramebufferParameteriv(0, GL_SAMPLES, &parm2);
    ////std::cout << "default fbo(0) GL_SAMPLES: " << parm2 << "\n";
    //glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    //glDrawBuffer(GL_BACK);
    //glBindFramebuffer(GL_READ_FRAMEBUFFER, output_fbo);
    //glBlitFramebuffer(0, 0, output_width, output_height, 0, 0, w_width, w_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    ////while (GLenum er = glGetError()) {
    ////    std::cout << "New openGL error: " << er << std::endl;
    ////}

    for (auto& l : m_layers) {
        if (l->type == LAYERGUI) {
            l->animateViewport();
            l->render();
        }
    }
    endImGUI();
    if (output_fbo != 0 && renderoutput) writeFrame(output_fbo); // Save PNG of off-screen buffer
    if (output_fbo == 0 && renderoutput) writeFrame(0);          // Save PNG of window contents (back buffer)
    glfwSwapBuffers(window); // Swap rendered screen to front
    currentframe++;
    if (currentframe == 100000) incSequence(); // Protect against issues with frame counter wrapping in filename which only allows 5 digits
    dumpdata = false;
}
void Application::writeFrame(unsigned int framebuffer) {
    std::string fullname = "C:\\Coding\\Eartharium\\Eartharium\\AnimOut\\" + basefname;
    char numerator[20];
    snprintf(numerator, sizeof(numerator), "S%03d-%05d.png", currentseq, currentframe);
    fullname.append(numerator);
    if (framebuffer == 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        saveImage(fullname, window, 0);               // default frame buffer
    }
    else {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glViewport(0, 0, output_width, output_height);
        glScissor(0, 0, output_width, output_height);
        saveImage(fullname, window, framebuffer, (int)output_width, (int)output_height);
    }
    std::cout << "Rendered Frame " << currentframe << " to " << fullname << " from Framebuffer " << framebuffer << "\n";
    //delete[] numerator;  // NO! It is stack allocated!
}
void Application::incSequence() {
    currentseq++;
    currentframe = 0;
}
unsigned int Application::createFrameBuffer(unsigned int width, unsigned int height, unsigned int type) {
    GLsizei samples = 4;
    glGenFramebuffers(1, &output_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, output_fbo);
    //glViewport(0, 0, width, height);
    //glScissor(0, 0, width, height);

    // Configure a multisample texture to hold color data and attach it
    //glActiveTexture(GL_TEXTURE7);
    //glGenTextures(1, &output_texture);
    //glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, output_texture);
    //glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, output_texture, 0);
    // Configure a texture to hold color data and attach it
    glActiveTexture(GL_TEXTURE7);
    glGenTextures(1, &output_texture);
    glBindTexture(GL_TEXTURE_2D, output_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, output_texture, 0);

    // Configure a depth attachment using RBO (not using stencil attachments currently)
    //glGenRenderbuffers(1, &output_rbo);
    //glBindRenderbuffer(GL_RENDERBUFFER, output_rbo);
    //glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, /*GL_DEPTH_COMPONENT32*/ GL_DEPTH24_STENCIL8, width, height); // Could use GL_DEPTH24_STENCIL8 if stencils are needed
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, /*GL_DEPTH_ATTACHMENT*/ GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, output_rbo); // Then this would be GL_DEPTH_STENCIL_ATTACHMENT
    //glBindRenderbuffer(GL_RENDERBUFFER, 0); // Now that it is attached, we can unbind.It could be unbound right after creation, above takes the name.
    // Configure a depth attachment using RBO (not using stencil attachments currently)
    glGenRenderbuffers(1, &output_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, output_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height); // Could use GL_DEPTH24_STENCIL8 if stencils are needed
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, output_rbo); // Then this would be GL_DEPTH_STENCIL_ATTACHMENT
    glBindRenderbuffer(GL_RENDERBUFFER, 0); // Now that it is attached, we can unbind.It could be unbound right after creation, above takes the name.

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer was not created successfully! Status: " << status << "\n";
        return NO_UINT;
    }
    // GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE - Returned because glTexImage2DMultisample() set GL_TEXTURE_FIXED_SAMPLE_LOCATIONS to GL_FALSE,
    //  and we use a mixture of texture and render buffers

    // Note: Intel UHD 630 drivers give incorrect values for GL_SAMPLES!
    //GLint parm = 0;
    //glGetNamedFramebufferParameteriv(output_fbo, GL_SAMPLES, &parm);
    //std::cout << "output_fbo(" << output_fbo << ") GL_SAMPLES: " << parm << "\n";

    // Set up primitive for blitter
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    output_width = width;
    output_height = height;
    return output_fbo;
}
void Application::setupFileRender(unsigned int width, unsigned int height, unsigned int type) {
    output_width = width;
    output_height = height;
    output_type = type;
    output_fbo = createFrameBuffer(output_width, output_height, output_type);
    // Figure out consistent glViewport settings working well with the View settings of the Layers.
}
