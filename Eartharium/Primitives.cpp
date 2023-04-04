//#include "config.h"
#include "Primitives.h"  // includes mdoOpenGL.h which includes all other headers
#include "Earth.h"
//#include "Astronomy.h"

//#define _CRT_SECURE_NO_WARNINGS

// --------
//  Lerper
// --------
// See Primitives.h = class templates like to live in *.h files rather than *.cpp





// --------
//  Camera
// --------
Camera::Camera(Scene* scene) {
    m_scene = scene;
    update();
    //Recalc();   NOTE: Set reasonable defaults in Primitives.h and do Recalc() so cam starts in well defined configuration !!!
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
void Camera::update() {
    setPosLLH({ camLat, camLon, camDst });
    updateLight();
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
void Camera::setPosLLH(LLH llh) {
    while (camLon > 180.0) camLon -= 360.0;
    while (camLon < -180.0) camLon += 360.0;
    // How to normalize latitude properly? Not practical as 100 should go to 80 and longitude should change 180 whereas 360 should simply go to 0.
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
    ProjMat = glm::perspective(glm::radians(camFoV), m_scene->getAspect(), camNear, camFar);
    updateLight();
}


// -------
//  Scene
// -------
Scene::Scene(Application* app) : m_app(app) {
    // Set up a default Camera - Why? It doesn't save a lot of work, and is rather inconsistent. Well, to satisfy the keyboard controller.
    w_camera = newCamera("Default Cam");  // Will also be m_cameras[0]
    if (m_app->currentCam == nullptr) m_app->currentCam = w_camera;
    // Cameras need a scene to look at, so they should be derived from Scene::newCamera() or similar
    // Need to support more than one Camera per scene, so it should return a reference that can be passed to RenderLayer3D
    scenetree = new SceneTree;
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
    delete scenetree; // Always created in constructor
}
Camera* Scene::newCamera(const std::string name) {
    Camera* cam = new Camera(this);
    cam->name = name;
    m_cameras.push_back(cam);
    return cam;
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
    scenetree->updateBreathFirst();
    // Should take fbo render target !!!
    if (m_earthOb != nullptr) m_earthOb->Update(); // Make sure primitives are up to date before casting their shadows (Earth updates Locations)
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
    if (earth) earth->draw(cam); // Earth2 experimental
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
    } else {
        m_polylines.pop_back();
        delete curve;
    }
}
Earth2* Scene::newEarth2(std::string mode, const unsigned int mU, const unsigned int mV, SceneObject* parent) {
    earth = new Earth2(this, mode, mU, mV);
    earth->setParent(parent);
    scenetree->addSceneObject(earth, parent);
    return earth;
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

    m_scene->m_astro = m_astro;
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
        std::string datetime;
        m_astro->getTimeString((char*)datetime.c_str());
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
                        l.layer->m_scene->m_app->currentCam = l.layer->m_scene->w_camera;
                        ImGui::SliderFloat("CamLat", &l.layer->m_scene->w_camera->camLat, -90.0f, 90.0f);
                        ImGui::SliderFloat("CamLon", &l.layer->m_scene->w_camera->camLon, -179.9f, 180.0f);
                        ImGui::SliderFloat("FoV", &l.layer->m_scene->w_camera->camFoV, CAMERA_MIN_FOV, CAMERA_MAX_FOV);
                        ImGui::SliderFloat("Dist", &l.layer->m_scene->w_camera->camDst, CAMERA_MIN_DIST, CAMERA_MAX_DIST);
                        ImGui::SliderFloat("Clip Near", &l.layer->m_scene->w_camera->camNear, 0.01f, 5.0f);
                        ImGui::SliderFloat("Clip Far", &l.layer->m_scene->w_camera->camFar, 10.0f, 15000.0f);
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
                        if (ImGui::Button("-")) { l.layer->m_astro->addTime(0.0, 0.0, l.layer->m_scene->m_astro->getEoT(), 0.0, do_eot); }
                        ImGui::SameLine();
                        if (ImGui::Button("+")) { l.layer->m_astro->addTime(0.0, 0.0, -l.layer->m_scene->m_astro->getEoT(), 0.0, do_eot); }
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
                            std::cout << "Subtracted time (d,h,m,s): " << mytimestep.da << "," << mytimestep.hr << "," << mytimestep.mi << "," << mytimestep.se << '\n';
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("+1 Step")) {
                            l.layer->m_astro->addTime(mytimestep.da, mytimestep.hr, mytimestep.mi, mytimestep.se, do_eot);
                            std::cout << "Added time (d,h,m,s): " << mytimestep.da << "," << mytimestep.hr << "," << mytimestep.mi << "," << mytimestep.se << '\n';
                        }
                        ImGui::SliderFloat("Time of Day", &slideday, 0.0f, 24.0f);
                        if (prevslideday != slideday) {
                            // Should probably do two addTime() passes (-prev, then +curr), due to EoT I think.
                            l.layer->m_astro->addTime(0.0, (double)slideday - (double)prevslideday, 0.0, 0.0, do_eot);
                            prevslideday = slideday;
                        }
                        //ImGui::SliderFloat("year", &slideyear, 0.0f, 365.0f);
                    }
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
                            ImGui::Checkbox("Solar Insolation", &l.layer->m_scene->m_app->currentEarth->w_sinsol);
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
        ImGui::ShowDemoWindow();
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
    if (ImPlot::BeginPlot("Line Plot", ImVec2(-1,-1))) {
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
    m_layers.reserve(16); // ?? Might need to be a list instead of a vector

}
// DESTRUCTOR: Should ideally tear down the things we set up

ShaderLibrary* Application::getShaderLib() { return m_shaderlib; }
Astronomy* Application::newAstronomy() { return new Astronomy(); }  // These may want to save references for clean-up, or maybe not !!!
Scene* Application::newScene() { return new Scene(this); }
int Application::initWindow() {
    if (start_fullscreen) w_width = 1920;
    else w_width = 1280;
    if (start_fullscreen) w_height = 1080;
    else w_height = 720;

    // Setup platform environment
    window = setupEnv(w_width, w_height, 4, 6, start_fullscreen);

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
    if (dumptime&& currentEarth != nullptr) {
        currentEarth->m_scene->m_astro->dumpCurrentTime(currentframe);
        dumptime = false;
    }
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
    beginImGUI();
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
}
void Application::writeFrame(unsigned int framebuffer) {
    std::string fullname = "C:\\Coding\\Eartharium\\Eartharium\\AnimOut\\" + basefname;
    char numerator[20];
    sprintf(numerator, "S%03d-%05d.png", currentseq, currentframe);
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


// --------------
//  Text Factory
// --------------
TextFactory::TextFactory(Scene* scene) : m_scene(scene) {
    for (auto& t : m_texts) {
        delete t;
    }
    m_texts.clear();
}
TextFactory::~TextFactory() {}
TextString* TextFactory::newText(Font* font, const std::string& text, const float size, const glm::vec4 color, glm::vec3& position, glm::vec3& direction, glm::vec3& up) {
    m_texts.push_back(new TextString(m_scene, font, text, size, color, position, direction, up));
    return m_texts.back();
}
void TextFactory::draw() {
    for (auto& t : m_texts) {
        t->draw();
    }
}


// -------------
//  Text String
// -------------
TextString::TextString(Scene* scene, Font* font, const std::string& text, float size,  glm::vec4 color, glm::vec3& position, glm::vec3& direction, glm::vec3& up)
// Maybe make Font the last parameter and optional, and have a default font loaded? !!!
    : m_scene(scene), m_font(font), m_text(text), m_size(size), m_color(color), m_position(position), m_direction(direction), m_up(up) {
    m_glyphsOb = new Glyphs(m_scene, m_font);
    genGlyphs();
    m_glyphsOb->drawGlyphs();
}
TextString::~TextString() {}
//void TextString::update() {}
void TextString::draw() {
    m_glyphsOb->drawGlyphs();
}
void TextString::updateText(const std::string& text) {
    // Delete the existing Glyphs and create new ones with genGlyph()
    m_text = text;
    genGlyphs();
}
void TextString::updatePosDirUp(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
    m_position = position;
    m_direction = direction;
    m_up = up;
    genGlyphs();
}
void TextString::genGlyphs() {
    // Then a Glyphs can be allocated by each TextString, or even be absorbed into it. !!!
    m_glyphsOb->clear();
    //m_letters.clear();  // Why are we even saving these? !!!
    glm::vec3 cursor = m_position;
    for (auto& l : m_text) {
        m_glyphsOb->newGlyph(l, m_color, m_size, cursor, m_direction, m_up);
    }
}


// ------
//  Font
// ------
// Font could even be part of Application? !!!
Font::Font(const std::string& fontname) {
    is_valid = loadFont(fontname);
    // Do something with return value? Maybe a bool is better.
}
int Font::createFont(const std::string& font) {
    // Should eventually use msdf-atlas-gen example code from github README.md file !!!
    std::cout << "ERROR! Font::createFont() was called but is not yet implemented!\n";
    return -1;
}
bool Font::loadFont(const std::string& font) {
    // Could use https://github.com/Chlumsky/msdf-atlas-gen README.md as library and avoid metadata file parsing
    // MSDF font bitmap
    const std::string texfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\" + font + ".png";
    tex = new Texture(texfile, GL_TEXTURE8);
    // Mapping metadata
    std::ifstream infile("C:\\Coding\\Eartharium\\Eartharium\\textures\\" + font + ".csv", std::ifstream::in);
    if (!infile.is_open()) {
        std::cout << "ERROR! Glyphs::loadFont(): Could not load CSV for font " << font << " !\n";
        return false;
    }
    std::string line, item;
    std::istringstream parse;
    getline(infile, line); // First line of CSV: font size (pixels), atlas width (pixels), atlas height (pixels), lineheight (EM) 
    parse.str(line);
    std::getline(parse, item, ',');
    font_size = std::stof(item);
    std::getline(parse, item, ',');
    font_atlas_width = std::stof(item);
    std::getline(parse, item, ',');
    font_atlas_height = std::stof(item);
    std::getline(parse, item, ',');
    font_lineheight = std::stof(item);
    unsigned int i = 0; // Remaining lines are glyphs
    while (getline(infile, line)) {
        i++;
        glyphs.push_back(glyphdata());
        parse.clear();
        parse.str(line);
        // character code
        std::getline(parse, item, ',');
        glyphs.back().letter = (char)std::stoi(item);
        // advance
        std::getline(parse, item, ',');
        glyphs.back().advance = std::stof(item);
        // planeBounds
        std::getline(parse, item, ',');
        glyphs.back().p_left = std::stof(item);
        std::getline(parse, item, ',');
        glyphs.back().p_bottom = std::stof(item);
        std::getline(parse, item, ',');
        glyphs.back().p_right = std::stof(item);
        std::getline(parse, item, ',');
        glyphs.back().p_top = std::stof(item);
        // atlasBounds - Converted from pixels to OpenGL texture coordinate range
        std::getline(parse, item, ',');
        glyphs.back().a_left = std::stof(item) / font_atlas_width;
        std::getline(parse, item, ',');
        glyphs.back().a_bottom = (font_atlas_height - std::stof(item)) / font_atlas_height;
        std::getline(parse, item, ',');
        glyphs.back().a_right = std::stof(item) / font_atlas_width;
        std::getline(parse, item, ',');
        glyphs.back().a_top = (font_atlas_height - std::stof(item)) / font_atlas_height;
    }
    glyphs.push_back(glyphdata()); // Last entry is error indicator.
    return true;
    //std::cout << "Font::loadFont(\"" << font << "\"): Glyphs loaded from file : " << i << '\n';
}
glyphdata Font::getGlyphdata(char letter) {
    for (auto& g : glyphs) {
        if (g.letter == letter) return g;
    }
    return glyphs.back(); // By default return the last glyph.
    // No obvious way to pass an error code, perhaps the font loader can explicitly add an extra error indicating glyph
    // at the end, so we don't allocate one here every time (since the glyphs are redrawn often, and the same invalid glyph
    // may come up many times). Perhaps a 
}
Texture* Font::getTexture() {
    return tex;
}


// --------
//  Glyphs
// --------
Glyphs::Glyphs(Scene* scene, Font* font) : m_scene(scene), m_font(font){ // Have one Glyphs object per font, pass in font filename
    //std::string fontname = "CascadiaMono";
    //loadFont(fontname); // When several fonts are available, specify name?
    glyphItems.reserve(30);
    shdr = m_scene->m_app->getShaderLib()->getShader(GLYPH_SHADER);
    vbl = new VertexBufferLayout;
    vbl->Push<float>(3);   // Vertex pos
    vbl->Push<float>(2);   // Vertex UV
    vbl->Push<float>(4);   // Color (for overlay)
}
Glyphs::~Glyphs() {}
unsigned int Glyphs::newGlyph(char letter, glm::vec4 color, float size, glm::vec3& cursor, glm::vec3& direction, glm::vec3& up) {
    // Use msdf from https://github.com/Chlumsky
    // Draw quads relative to cursor with planeBounds, advance cursor by Advance.
    // Texture coordinates in pixels is in atlasBounds.
    // See: https://github.com/Chlumsky/msdf-atlas-gen/issues/2

    // To orientate the writing plane, do we use a normal, or a writing direction vector? Both require an additional direction or angle about the vector.
    // Look up planeBounds, Advance and atlasBounds
    // Add cursor and planeBounds to get world coordinates of glyph quad
    // Add Advance to cursor, in writing direction. Keep orientation/scale/loc separate from stored glyph?
    // Divide atlasBounds by texture size to get texture coordinates
    // Cross planeBoundsX with planeBoundsY to get a normal (for lighting)
    // Create the quad with vertices: pos, normal, uv, color to glyph cache

    direction = glm::normalize(direction);
    up = glm::normalize(up);

    glyphdata myglyph = m_font->getGlyphdata(letter);

    glm::vec3 p_a = cursor + (myglyph.p_left * direction + myglyph.p_bottom * up) * size;
    glm::vec3 p_b = cursor + (myglyph.p_left * direction + myglyph.p_top * up) * size;
    glm::vec3 p_c = cursor + (myglyph.p_right * direction + myglyph.p_top * up) * size;
    glm::vec3 p_d = cursor + (myglyph.p_right * direction + myglyph.p_bottom * up) * size;
    glm::vec2 a_a = glm::vec2(myglyph.a_left, myglyph.a_bottom);
    glm::vec2 a_b = glm::vec2(myglyph.a_left, myglyph.a_top);
    glm::vec2 a_c = glm::vec2(myglyph.a_right, myglyph.a_top);
    glm::vec2 a_d = glm::vec2(myglyph.a_right, myglyph.a_bottom);
    glyphItems.push_back({ p_a, a_a, color });  // First triangle
    glyphItems.push_back({ p_b, a_b, color });
    glyphItems.push_back({ p_c, a_c, color });
    glyphItems.push_back({ p_a, a_a, color });  // Second triangle
    glyphItems.push_back({ p_c, a_c, color });
    glyphItems.push_back({ p_d, a_d, color });  // Add back sides too? !!!
    cursor += direction * myglyph.advance * size;
    // Now that glyphItems is not empty, create Vertex Buffer and Vertex Array if they are missing
    if (va == nullptr && vb == nullptr) {
        vb = new VertexBuffer(&glyphItems[0], (unsigned int)glyphItems.size() * sizeof(GlyphItem));
        va = new VertexArray;
        va->AddBuffer(*vb, *vbl, true);
    }
    return 0; // Need to return an index, so we can come back and update the glyph. No, we will recreate all the Glyphs from TextString every time
}
void Glyphs::clear() {
    glyphItems.clear();
}
void Glyphs::drawGlyphs() {
    if (glyphItems.size() == 0) return; // Not sure that loading empty data to OpenGL would be good
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
    shdr->SetUniform1i("tex", m_font->getTexture()->GetTextureSlot());
    vb->LoadData(&glyphItems[0], (unsigned int)glyphItems.size() * sizeof(GlyphItem));
    va->Bind();
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)glyphItems.size());
} 


// ------------
//  Time Zones
// ------------
// To make a geographically based time zone picker:
// https://github.com/BertoldVdb/ZoneDetect
// Quick sanity check: https://cdn.bertold.org/demo/timezone.html
// https://www.iana.org/time-zones
// https://github.com/evansiroky/timezone-boundary-builder
// https://github.com/evansiroky/timezone-boundary-builder/releases
// - It's complicated -
// It is better to let a Location "pick" a time zone, either manually or by checking bounding boxes,
// and then actual outlines in shapefile from https://github.com/evansiroky/timezone-boundary-builder
// Once a time zone is selected, the Location needs to pick up the local time for the current UTC time
// This is done by first checking the Links data for any redirects of the time zone, then parsing the
// Time zone arrived at, while applying any rules listed. There can be multiple lists of named rules
// involved, see Ireland for a complex example.
// Both Zones and Rules have complex formats that require careful parsing. Additionally some rules may
// overlap, and resolution is done first by time, and if equal, by earliest in zone file. Thus, parse
// the IANA zone files sequentially and assign sequence numbers in addition to indices (in case a storage
// container that might reorder entries is used, or sorting is desirable at some point).
// Apply the Zone offset appropriate to the UTC time, then apply any additional shifts specified in the
// applicable Rule (if any).
// zic, the sample code from IANA tz, compiles the Zones & Rules into a different format. Look into this,
// as that may be easier to parse, thus foregoing subtle parsing bugs.
//
// For geographic zone matching, see: https://web.archive.org/web/20130126163405/http://geomalgorithms.com/a03-_inclusion.html
// Alternatively use this librarry: https://github.com/BertoldVdb/ZoneDetect
TimeZones::TimeZones(Scene* scene, const std::string& filebase) : m_scene(scene) {
    // Data source: https://github.com/evansiroky/timezone-boundary-builder
    // ESRI .shp file format:
    // https://en.wikipedia.org/wiki/Shapefile (basic explanation, enough to get started)
    // https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf (full spec)
    // dBASE XBase .dbf format:
    // https://www.clicketyclick.dk/databases/xbase/format/index.html
    // (for now, just using Excel to open and read the row number (subtract 1 for header before using).
    // Could make a summary CSV file, or build a full .dbl parser. Would it be an interesting challenge, or perhaps even useful?
    // o For now, make a CSV file, the source data is not updated very often.
    // Note: There are libraries available for these sort of things too.
     
    // Load and parse the ShapeFile with time zone outlines
    ShapeFile::parseFile(records, filebase + ".shp");
    // Load and parse the time zone outline names
    parseNames(filebase + ".csv");
    // Load IANA tz database
    tzFile::loadFiles(timezonedb, "C:\\Coding\\IANA-tzdb\\");
}
void TimeZones::addTimeZone(Earth& earth, const std::string timezonename) {
    // Check CSV file for spelling of countries, this does NOT have fuzzy search.
    // Alternatively look up the index manually and pass that instead (as unsigned int)
    size_t index = 0;
    for (auto& cn : timezonenames) { // Should probably use a std::find, this is the 21st century.
        if (timezonename == cn.searchname) index = cn.index; // If there are duplicates, this returns the last
    }
    if (index == 0) {
        std::cout << "WARNING: TimeZones::addTimeZones(): No match found for timezone name: " << timezonename << ", defaulting to timezone_id = 1\n";
        index = 1;
    }
    // A better failure mode is to return NO_UINT or zero instead of a different time zone than the one requested.
    addTimeZone(earth, index);
}
void TimeZones::addTimeZone(Earth& earth, size_t rindex) {
    // This could take Earth (for getLoc3D()) and an array where to drop the points.
    // Ideally I want to be able to draw a border on a globe, then unlink it from the geometry,
    //  have it float up while Earth morphs, and settle down to compare to the new size.
    //  (Of course this requires two borders, one that remains linked and one that floats)
    // But start by simply getting a border onto Earth, then make it morph. I can add a duplicate
    //  function to whatever object ends up holding a border. Begin by using a PolyCurve, change to GL_LINE later.
    // UPD: Now using PolyLine, which is always flat and has width in screen space rather than world space.
    // Note: The GMT offset zones ONLY cover the international waters. So to get the complete zone displayed
    //       I need to collect all the zones where that offset is used at the current time. This would then
    //       display all areas with the same wall clock time I think. Or probably DST is not applied in international waters?

    // Should scan borderparts vector to see if the requested border exists already !!!
    //  o That may conflict with creating floating duplicate as pondered above.
    rindex--;  // Array indices assume array starts at 1
    for (auto& part : records[rindex]->parts) {
        //std::cout << "CountryBorder::addBorder(): part: " << part.partnum << " start: " << part.startindex << " length: " << part.length << "\n";
        timezoneparts.push_back(new timezonepartcache());
        timezoneparts.back()->timezone_id = rindex + 1;
        timezoneparts.back()->part = &part;
        timezoneparts.back()->polyline = m_scene->newPolyLine(LIGHT_RED, 0.001f, part.length);
        timezoneparts.back()->earth = &earth;
        for (size_t i = part.startindex; i < (part.startindex + part.length); i++) {
            timezoneparts.back()->polyline->addPoint(earth.getLoc3D(deg2rad * records[rindex]->points[i].latitude, deg2rad * records[rindex]->points[i].longitude, surface_offset));
        }
        timezoneparts.back()->polyline->generate();
    }
}
void TimeZones::update() { // Updates all of the time zone outline parts at once
    // This is even prepared to update "countries" on other planets, if they have a getLoc3D() function (or separate instances of Earth)
    // NOTE: Since this is plotted at 0.0f terrain height, it will intersect Earth tris !!!
    for (auto& pcache : timezoneparts) {
        //std::cout << "TimeZone::update(): timezone_id = " << pcache->timezone_id << ", polyline = " << pcache->polyline << ", earth = " << pcache->earth << '\n';
        pcache->polyline->clearPoints();
        //std::cout << "TimeZone::update(): part: " << part.partnum << " start: " << part.startindex << " length: " << part.length << "\n";
        for (size_t i = pcache->part->startindex; i < (pcache->part->startindex + pcache->part->length); i++) {
            pcache->polyline->addPoint(pcache->earth->getLoc3D(deg2rad * records[pcache->timezone_id - 1]->points[i].latitude, deg2rad * records[pcache->timezone_id - 1]->points[i].longitude, surface_offset));
        }
        pcache->polyline->generate();
    }
}
void TimeZones::draw() {
    std::cout << "TimeZones::draw(): no need to call draw(), PolyLines were allocated via Scene, so will be drawn there.\n";
    //for (auto& pcache : borderparts) {
    //    pcache->polyline->draw();
    //}
}
int TimeZones::parseNames(const std::string& namefile) {
    std::cout << "TimeZones::parseNames() - Processing file: " << namefile << '\n';
    std::istringstream parse;
    std::string line;
    std::ifstream stream(namefile);
    getline(stream, line); // Skip headers
    unsigned int i = 0;
    while (getline(stream, line)) {
        //std::cout << line << '\n';
        i++;
        timezonenames.emplace_back(TimeZoneName());
        timezonenames.back().index = i;
        parse.clear();
        parse.str(line);
        std::getline(parse, timezonenames.back().searchname, ',');
        //std::getline(parse, timezonenames.back().displayname, ',');
    }
    return 0;
}
std::string TimeZones::getLocalTime(const std::string& timezone, const DateTime& datetime) {
    return tzFile::getLocalTime(timezonedb, timezone, datetime);
}
std::string TimeZones::getLocalTime(const std::string& timezone, long year, long month, double day, double hour, double minute, double second) {
    return tzFile::getLocalTime(timezonedb, timezone, year, month, day, hour, minute, second);
}
void TimeZones::dumpTimeZoneDetails(const std::string& timezone) {
    tzFile::dumpTimeZoneDetails(timezonedb, timezone);
}


// -----------------
//  Country Borders
// -----------------
// Strictly speaking this is Country Outlines
CountryBorders::CountryBorders(Scene* scene, const std::string& filebase) : m_scene(scene) {
    // Data source: http://www.naturalearthdata.com/downloads/
    // ESRI .shp file format:
    // https://en.wikipedia.org/wiki/Shapefile (basic explanation, enough to get started)
    // https://www.esri.com/content/dam/esrisites/sitecore-archive/Files/Pdfs/library/whitepapers/pdfs/shapefile.pdf (full spec)
    // dBASE XBase .dbf format:
    // https://www.clicketyclick.dk/databases/xbase/format/index.html
    // (for now, just using Excel to open and read the row number (subtract 1 for header before using).
    // Could make a summary CSV file, or build a full .dbl parser. Would it be an interesting challenge, or perhaps even useful?
    // o For now, make a CSV file, the source data is not updated very often.
    // Note: There are libraries available for these sort of things too.
    //parseFile(filebase + ".shp");
    ShapeFile::parseFile(records, filebase + ".shp");
    // If a generic shapefile loader is made, pass std::vector<ShapeRecord*> records by reference.
    parseNames(filebase + ".csv");

}
void CountryBorders::addBorder(Earth& earth, const std::string countryname) {
    // Check CSV file for spelling of countries, this does NOT have fuzzy search.
    // Alternatively look up the index manually and pass that instead (as unsigned int)
    size_t index = 0;
    for (auto& cn : countrynames) { // Should probably use a std::find, this is the 21st century.
        if (countryname == cn.searchname) index = cn.index; // If there are duplicates, this returns the last
    }
    if (index == 0) {
        std::cout << "WARNING: CountryBorders::addBorder(): No match found for country name: " << countryname << ", defaulting to country_id = 1\n";
        index = 1;
    }
    addBorder(earth, index);
}
void CountryBorders::addBorder(Earth& earth, size_t rindex) {
    // This could take Earth (for getLoc3D()) and an array where to drop the points.
    // Ideally I want to be able to draw a border on a globe, then unlink it from the geometry,
    //  have it float up while Earth morphs, and settle down to compare to the new size.
    //  (Of course this requires two borders, one that remains linked and one that floats)
    // But start by simply getting a border onto Earth, then make it morph. I can add a duplicate
    //  function to whatever object ends up holding a border. Begin by using a PolyCurve, change to GL_LINE later.
    // UPD: Now using PolyLine, which is always flat and has width in screen space rather than world space.
    // Make an update function for morphing.

    // Should scan borderparts vector to see if the requested border exists already !!!
    //  o That may conflict with creating floating duplicate as pondered above.
    rindex--;  // Array indices assume array starts at 1
    for (auto& part : records[rindex]->parts) {
        //std::cout << "CountryBorder::addBorder(): part: " << part.partnum << " start: " << part.startindex << " length: " << part.length << "\n";
        borderparts.push_back(new borderpartcache());
        borderparts.back()->country_id = rindex + 1;
        borderparts.back()->part = &part;
        borderparts.back()->polyline = m_scene->newPolyLine(LIGHT_RED, 0.001f, part.length);
        borderparts.back()->earth = &earth;
        for (size_t i = part.startindex; i < (part.startindex + part.length); i++) {
            borderparts.back()->polyline->addPoint(earth.getLoc3D(deg2rad * records[rindex]->points[i].latitude, deg2rad * records[rindex]->points[i].longitude, surface_offset));
        }
        borderparts.back()->polyline->generate();
    }
}
void CountryBorders::update() { // Updates all of the country border parts at once
    // This is even prepared to update "countries" on other planets, if they have a getLoc3D() function (or separate instances of Earth)
    // NOTE: Since this is plotted at 0.0f terrain height, it will intersect Earth tris !!!
    for (auto& pcache : borderparts) {
        //std::cout << "CountryBorder::update(): country_id = " << pcache->country_id << ", polyline = " << pcache->polyline << ", earth = " << pcache->earth << '\n';
        pcache->polyline->clearPoints();
        //std::cout << "CountryBorder::update(): part: " << part.partnum << " start: " << part.startindex << " length: " << part.length << "\n";
        for (size_t i = pcache->part->startindex; i < (pcache->part->startindex + pcache->part->length); i++) {
            pcache->polyline->addPoint(pcache->earth->getLoc3D(deg2rad * records[pcache->country_id - 1]->points[i].latitude,
                                       deg2rad * records[pcache->country_id - 1]->points[i].longitude, surface_offset));
        }
        pcache->polyline->generate();
    }
}
void CountryBorders::draw() {
    std::cout << "CountryBorders::draw(): no need to call draw(), PolyLines were allocated via Scene, so will be drawn there.\n";
    //for (auto& pcache : borderparts) {
    //    pcache->polyline->draw();
    //}
}
int CountryBorders::parseNames(const std::string& namefile) {
    std::istringstream parse;
    std::string line;
    std::ifstream stream(namefile);
    getline(stream, line); // Skip headers
    unsigned int i = 0;
    while (getline(stream, line)) {
        i++;
        countrynames.emplace_back(CountryName());
        countrynames.back().index = i;
        parse.clear();
        parse.str(line);
        std::getline(parse, countrynames.back().searchname, ',');
        std::getline(parse, countrynames.back().displayname, ',');
    }
    return 0;
}


// --------
//  SkyBox
// --------
SkyBox::SkyBox(Scene* scene) : m_scene(scene) {
    //m_app = m_scene->m_app;
    loadCubemap(m_faces);
    //std::string shdskyboxsrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\skybox.shader";
    //m_shdrsb = new Shader(shdskyboxsrc);
    m_shdrsb = m_scene->m_app->getShaderLib()->getShader(SKY_BOX_SHADER);
    m_vblsb = new VertexBufferLayout;
    m_vblsb->Push<float>(3);
    m_vasb = new VertexArray;
    m_vbsb = new VertexBuffer(skyboxVertices, 6 * 6 * 3 * 4); // 6 sides of 2*3 verts, each 3 coords of 4 bytes
    m_vasb->AddBuffer(*m_vbsb, *m_vblsb, true);
}
SkyBox::~SkyBox() {
    delete m_vbsb;
    delete m_vasb;
    delete m_vblsb;
    //delete m_shdrsb;
}
void SkyBox::Draw() {
    glm::mat4 proj =  glm::perspective(glm::radians(70.0f), m_scene->m_app->getAspect(), 0.1f, 100.0f);
    glm::mat4 view = glm::mat4(glm::mat3(m_scene->w_camera->getViewMat()));
    glFrontFace(GL_CCW);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_LEQUAL);
    m_shdrsb->Bind();
    m_shdrsb->SetUniformMatrix4f("view", view);
    m_shdrsb->SetUniformMatrix4f("projection", proj);
    m_shdrsb->SetUniform1i("skybox", GL_TEXTURE2 - GL_TEXTURE0);
    m_vasb->Bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glFrontFace(GL_CW);
}
void SkyBox::loadCubemap(std::vector<std::string> faces) {
    glActiveTexture(GL_TEXTURE2);
    // std::cout << "SkyBox::loadCubemap(): TextureSlot " << GL_TEXTURE2 - GL_TEXTURE0 << " Render_ID " << m_textureID << ".\n";
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = SOIL_load_image(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data );
            SOIL_free_image_data(data);
        }
        else {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //glActiveTexture(GL_TEXTURE0);
}


// ---------------
//  ParticleTrail
// ---------------
// NOTE: Implement ability to trim() trail, and possibly to fade out whole trail
ParticleTrail::ParticleTrail(Scene* scene, unsigned int number, glm::vec4 color, float size, unsigned int spacing, bool taper) : m_scene(scene) {
    m_taper = taper;
    m_number = number;
    m_color = color;
    m_size = size;
    m_spacing = spacing;  // Intermediate frames where dots are skipped. 0 = plot every time.
    m_gap = m_spacing;
    m_sizefactor = pow(10, (log10(0.1) / (double)m_number));
    //std::cout << "m_sizefactor: " << m_sizefactor << "\n";
}
ParticleTrail::~ParticleTrail() {}
void ParticleTrail::push(glm::vec3 pos) {
    // Push new position, pop the oldest. Use queue? No, deque!
    if (m_gap > 0) {
        m_gap -= 1;
        return;
    }
    float size = m_size;
    if (m_taper) {
        // Reduce size and opacity of trail
        for (auto& p : m_queue) {  // Here whole trail could be faded out when desired
            p.size *= (float)m_sizefactor;
            //p.color.a *= 0.95f;
            m_scene->getDotsFactory()->changeXYZ(p.index, p.position, p.color, p.size);
        }
    }
    if (m_queue.size() >= m_number) {
        m_scene->getDotsFactory()->remove(m_queue.back().index);
        m_queue.pop_back();
    }
    size_t index = m_scene->getDotsFactory()->addXYZ(pos, m_color, size);
    m_queue.push_front({ m_color, pos, size, index } );
    m_gap = m_spacing;
}
void ParticleTrail::clear() {
    for (auto& p : m_queue) {
        m_scene->getDotsFactory()->remove(p.index);
    }
    m_queue.clear();
}
void ParticleTrail::trim(unsigned int length) {
    // Trims end of deque to length, convenient since new entries are inserted at front
    // This somehow glitches occasionally when the length slider is jiggled vigorously,
    //  and ends up deleting the wrong index. This is probably because it reads a queue
    //  element that does not exist.
    if (length < m_queue.size()) {
        for (unsigned int i = length; i < m_queue.size(); i++) {
            m_scene->getDotsFactory()->remove(m_queue.at(i).index);
        }
        m_queue.resize(length);
    }
    m_number = length;
    m_sizefactor = pow(10, (log10(0.1) / (double)m_number));
}
void ParticleTrail::expand(unsigned int length) {
    m_number = length;
    m_sizefactor = pow(10, (log10(0.1) / (double)m_number));
}
void ParticleTrail::draw() {
    // Loop through the particle queue and draw them. If using Dots, no need to draw anything
}


// -----------
//  AngleArcs
// -----------
AngleArcs::AngleArcs(Scene* scene) : m_scene(scene) { }
AngleArcs::~AngleArcs() {}
double AngleArcs::getAngle(size_t index) {
    return m_arcs[index].angle;
}
size_t AngleArcs::add(glm::vec3 position, glm::vec3 start, glm::vec3 stop, float length, glm::vec4 color, float width, bool wide, glm::vec3 pole) {
    // bool wide enables angles above pi, requires a vec3 pole to tell which direction is clockwise
    glm::vec3 nstart = glm::normalize(start);
    glm::vec3 nstop = glm::normalize(stop);
    glm::vec3 axis = glm::vec3(0.0f);
    double rangle = 0.0;
    if (wide) {
        // From: https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors
        // det = n · (v1 × v2) // triple product
        // angle = atan2(det, v1 · v2)
        rangle = atan2(glm::dot(pole, glm::cross(nstart, nstop)), glm::dot(nstart, nstop)); // det = glm::dot(axis, glm::cross(nstart, nstop))
        if (rangle < 0) rangle += tau;
        axis = pole;
    }
    else {
        rangle = acos(glm::dot(nstart, nstop)); // angle in radians
        axis = glm::cross(nstart, nstop);
    }
    m_arcs.push_back({ nullptr, 0, color, position, start, stop, length, width, rad2deg * rangle, false, wide, pole });
    m_arcs.back().polycurve = m_scene->newPolyCurve(color, width);
    // loop from 0 to rangle in steps of deg2rad (one degree in radians)
    glm::vec3 point = glm::vec3(0.0f);
    nstart *= length;
    for (double a = 0.0; a < rangle; a += deg2rad) {
        point = glm::rotate(nstart, (float)a, axis);
        //VPRINT(point);
        m_arcs.back().polycurve->addPoint(point + position);
    }
    m_arcs.back().polycurve->generate();
    return m_arcs.size() - 1;
}
void AngleArcs::remove(size_t index) {
    m_scene->deletePolyCurve(m_arcs[index].polycurve);
    m_arcs[index].expired = true;
}
void AngleArcs::update(size_t index, glm::vec3 position, glm::vec3 start, glm::vec3 stop, float length, glm::vec4 color, float width, bool wide, glm::vec3 pole) {
    bool dirty = (position != m_arcs[index].position || start != m_arcs[index].start || stop != m_arcs[index].stop || length != m_arcs[index].length || width != m_arcs[index].width);
    if (position != NO_VEC3) m_arcs[index].position = position;
    if (start != NO_VEC3) m_arcs[index].start = start;
    if (stop != NO_VEC3) m_arcs[index].stop = stop;
    if (length != NO_FLOAT) m_arcs[index].length = length;
    if (color != NO_COLOR) m_arcs[index].color = color;
    if (width != NO_FLOAT) m_arcs[index].width = width;
    glm::vec3 nstart = glm::normalize(m_arcs[index].start);
    glm::vec3 nstop = glm::normalize(m_arcs[index].stop);
    glm::vec3 axis = glm::vec3(0.0f);
    double rangle = 0.0;
    if (wide) {
        // From: https://stackoverflow.com/questions/14066933/direct-way-of-computing-clockwise-angle-between-2-vectors
        // det = n · (v1 × v2) // triple product
        // angle = atan2(det, v1 · v2)
        rangle = atan2(glm::dot(pole, glm::cross(nstart, nstop)), glm::dot(nstart, nstop)); // det = glm::dot(axis, glm::cross(nstart, nstop))
        if (rangle < 0) rangle += tau;
        axis = pole;
    }
    else {
        rangle = acos(glm::dot(nstart, nstop)); // angle in radians
        axis = glm::cross(nstart, nstop);
    }
    m_arcs[index].angle = rad2deg * rangle;
    //std::cout << "Angle is: " << m_arcs[index].angle << "\n";
    m_arcs[index].polycurve->clearPoints();
    glm::vec3 point = glm::vec3(0.0f);
    nstart *= m_arcs[index].length;
    for (double a = 0.0; a < rangle; a += deg2rad) {
        point = glm::rotate(nstart, (float)a, axis);
        m_arcs[index].polycurve->addPoint(point + position);
    }
    m_arcs[index].polycurve->generate();
}

void AngleArcs::draw() {
    for (auto& a : m_arcs) {
        a.polycurve->draw(m_scene->w_camera); // Move to be rendered in Scene, default Camera is not always right !!!
    }
}


// ----------
//  PolyLine   - Actually PolyLoop, as it closes the shape automatically (due to ESRI country borders). Make more generic !!!
// ----------
PolyLine::PolyLine(Scene* scene, glm::vec4 color, float width, size_t reserve) : m_scene(scene) {
    // NOTE: It is preferred to instantiate these via the Scene object!
    if (reserve == NO_UINT) reserve = polycurvereserve;
    m_color = color;
    m_width = width;
    m_points.reserve(reserve + 2);   // Add a little headroom
    shdr = m_scene->m_app->getShaderLib()->getShader(LINE_SHADER);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}
PolyLine::~PolyLine() {
}
void PolyLine::change(glm::vec4 color, float width) {
    if (color != NO_COLOR) m_color = color;
    if (width != NO_FLOAT) m_width = width;
}
void PolyLine::addPoint(glm::vec3 point) {
    //if (limit) std::cout << "WARNING PolyPolyCurve (" << this << ") adding beyond capacity, resizing!\n"; // only triggers if coming back after setting limit flag
    m_points.push_back(point);
    if (m_points.size() == m_points.capacity()) {
        std::cout << "WARNING: PolyLine (" << this << ") capacity: " << m_points.capacity() << " reached. It will be SLOW to add new points now!\n";
        //    limit = true;
    }
}
void PolyLine::clearPoints() {
    m_points.clear();
    //limit = false;
}
void PolyLine::generate() {
}
void PolyLine::draw() {
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
    //shdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_points.size()*sizeof(glm::vec3), &m_points[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glLineWidth(4.0f);
    //glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)m_points.size());  // ESRI Polygons are closed, so use GL_LINE_LOOP
    //glEnable(GL_DEPTH_TEST);
    //glDrawElementsInstanced(GL_LINES, ib->GetCount(), GL_UNSIGNED_INT, 0, (GLsizei)m_segments.size());
}


// -----------
//  PolyCurve
// -----------
// ToDo: !!! Make a PolyCurve factory that can render all the PolyCurves in one draw call, it will massively improve performance !!!
PolyCurve::PolyCurve(Scene* scene, glm::vec4 color, float width, size_t reserve) : m_scene(scene) {
    // NOTE: It is preferred to instantiate these via the Scene object!
    if (reserve == NO_UINT) reserve = polycurvereserve; 
    m_color = color;
    m_width = width;
    m_points.reserve(reserve+2);   // Add a little headroom
    m_segments.reserve(reserve+2); // When sizing it is easy to think of the number of points, and forget an extra one is used for closing the curve.
    facets = 16;
    m_verts.reserve(((size_t)facets + 1) * 2);
    m_tris.reserve((size_t)facets * 2);
    genGeom();
    shdr = m_scene->m_app->getShaderLib()->getShader(PRIMITIVE_SHADER);
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
    va->AddBuffer(*vb1, *vbl1, true);
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);  // IB uses COUNT, not BYTES!!!
}
PolyCurve::~PolyCurve() {
    delete vbl2;
    delete vbl1;
    delete ib;
    delete vb1;
    // delete vb2;  // Is deleted on every Draw() call completion
    delete va;
}
void PolyCurve::changePolyCurve(glm::vec4 color, float width) {
    if (color != NO_COLOR) m_color = color;
    if (width != NO_FLOAT) m_width = width;
    generate();
}
void PolyCurve::addPoint(glm::vec3 point) {
    //if (limit) std::cout << "WARNING PolyPolyCurve (" << this << ") adding beyond capacity, resizing!\n"; // only triggers if coming back after setting limit flag
    m_points.push_back(point);
    if (m_points.size() == m_points.capacity()) {
        std::cout << "WARNING: PolyCurve (" << this << ") capacity: " << m_points.capacity() << " reached. It will be SLOW to add new points now!\n";
    //    limit = true;
    }
}
void PolyCurve::clearPoints() {
    //std::cout << "Points: " << m_points.size() << ", Segments: " << m_segments.size() << "\n";
    m_points.clear();
    m_segments.clear(); // Just in case someone would clear the points and not call generate() to update segments.
    //limit = false;
}
void PolyCurve::generate() {
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
void PolyCurve::draw(Camera* cam) {
    //if (m_color == ORANGE) std::cout << "Sumner\n";
    if (m_segments.size() == 0) return;
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", cam->getViewMat());
    shdr->SetUniformMatrix4f("projection", cam->getProjMat());
    shdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);
    vb1->Bind();
    va->Bind();
    ib->Bind();
    va->AddBuffer(*vb1, *vbl1, true);
    vb2 = new VertexBuffer(&m_segments[0], (unsigned int)m_segments.size() * sizeof(Primitive3D));
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
void PolyCurve::genGeom() { // Single segment
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


// --------
//  Arrows
// --------
Arrows::Arrows(Scene* scene) : m_scene(scene) {
    // Composite primitive, generates 1 Cylinder and 1 Cone to build an arrow
    m_cylinders = m_scene->getCylindersFactory();
    m_cones = m_scene->getConesFactory();
    m_arrows.reserve(5000);
}
Arrows::~Arrows() {
    // Delete Cones and Cylinders here, some brutal caller may decide recreating Arrows is the easiest way to delete a bunch of them
    for (auto& a : m_arrows.m_Elements) {
        m_cylinders->remove(a.cylinder);
        m_cones->remove(a.cone);
    }
}
void Arrows::remove(size_t index) {
    m_cylinders->remove(m_arrows[index].cylinder);
    m_cones->remove(m_arrows[index].cone);
    m_arrows.remove(index);
}
size_t Arrows::store(Arrow a) {
    return m_arrows.store(a);
}
void Arrows::draw() {
    // Why do I even have this function here? !!!
    std::cout << "Arrows::Draw() was called. No need to do that, arrows are drawn as Cones and Cylinders.\n";
}
size_t Arrows::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    dir = glm::normalize(dir);
    size_t cone = m_cones->addStartDirLen(pos + dir * len, dir, width * 20, width * 4, color);
    size_t cyl = m_cylinders->addStartDirLen(pos, dir * len, len - width * 20, width, color);
    return store({ cyl, cone, color, pos, dir, len, width });
}
size_t Arrows::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    size_t cone = m_cones->addStartDirLen(end, dir, width * 20, width * 4, color);
    size_t cyl = m_cylinders->addStartDirLen(pos, dir, glm::length(dir) - width * 20, width, color);
    return store({ cyl, cone, color, pos, dir, glm::length(dir), width });
}
void Arrows::changeStartDirLen(size_t arrow, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    if (length == NO_FLOAT) length = m_arrows[arrow].length;
    if (width == NO_FLOAT) width = m_arrows[arrow].width;
    if (color == NO_COLOR) color = m_arrows[arrow].color;
    m_cones->changeStartDirLen(m_arrows[arrow].cone, pos + dir * length, dir, width * 20, width * 4, color);
    m_cylinders->changeStartDirLen(m_arrows[arrow].cylinder, pos, dir, length - width * 20, width, color);
    m_arrows[arrow] = { m_arrows[arrow].cylinder, m_arrows[arrow].cone, color, pos, dir, length, width };
}
void Arrows::changeStartEnd(size_t arrow, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    if (width == NO_FLOAT) width = m_arrows[arrow].width;
    if (color == NO_COLOR) color = m_arrows[arrow].color;
    glm::vec3 dir = end - pos;
    m_cones->changeStartDirLen(m_arrows[arrow].cone, end, dir, width * 20, width * 4, color);
    m_cylinders->changeStartDirLen(m_arrows[arrow].cylinder, pos, dir, glm::length(dir) - width * 20, width, color);
    m_arrows[arrow] = { m_arrows[arrow].cylinder, m_arrows[arrow].cone, color, pos, dir, glm::length(dir), width };
}
void Arrows::changeArrow(size_t index, glm::vec4 color, float length, float width) {
    if (color != NO_COLOR) m_arrows[index].color = color;
    if (length != NO_FLOAT) m_arrows[index].length = length;
    if (width != NO_FLOAT)  m_arrows[index].width = width;
    m_cones->changeColorLengthWidth(m_arrows[index].cone, color, length, width);
    m_cylinders->changeColorLengthWidth(m_arrows[index].cylinder, color, length, width);
}
void Arrows::removeArrow(size_t index) {
    //std::cout << "WARNING! Arrows::deleteArrow() was called but no delete functions exist for Cones and Cylinders, so cannot delete!!\n";
    m_cones->removeCone(m_arrows[index].cone);
    m_cylinders->removeCylinder(m_arrows[index].cylinder);
    m_arrows.remove(index);
}
void Arrows::clear() {
    m_arrows.clear();
}


// ------------
//  Primitives
// ------------
Primitives::Primitives(Scene* scene, unsigned int verts, unsigned int tris) : m_scene(scene) {
    m_Primitives.reserve(5000);
    m_verts.reserve(verts);
    m_tris.reserve(tris);
}
Primitives::~Primitives() {
    //delete shdr;
    //delete smshdr;
    //delete sbshdr;
    delete vbl2;
    delete vbl1;
    delete ib;
    delete vb1;
    // delete vb2;  // Is deleted on every Draw() call completion
    delete va;
}
void Primitives::init() {
    //std::string shdrsrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitive.shader";
    //shdr = new Shader(shdrsrc);
    shdr = m_scene->m_app->getShaderLib()->getShader(PRIMITIVE_SHADER);
    //std::string smshdrsrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitiveshadow.shader";
    //smshdr = new Shader(smshdrsrc);
    smshdr = m_scene->m_app->getShaderLib()->getShader(PRIMITIVE_SHADOW_MAP_SHADER);
    //std::string sbshdrsrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitivesdwbox.shader";
    //sbshdr = new Shader(sbshdrsrc);
    sbshdr = m_scene->m_app->getShaderLib()->getShader(PRIMITIVE_SHADOW_BOX_SHADER);
    vbl1 = new VertexBufferLayout; // Vertices list
    vbl1->Push<float>(3);     // Vertex coord (pos)
    vbl1->Push<float>(3);     // vNormal coord
    vbl1->Push<float>(2);     // Texture coord
    vbl2 = new VertexBufferLayout; // Primitives list
    vbl2->Push<float>(4);     // Instance color
    vbl2->Push<float>(3);     // Instance pos
    vbl2->Push<float>(3);     // Instance dir
    vbl2->Push<float>(3);     // Instance scale
    vbl2->Push<float>(1);     // Rotation about dir
    va = new VertexArray;
    vb1 = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
    vb2 = nullptr; // Will be built dynamically during each Draw() call
    va->AddBuffer(*vb1, *vbl1, true);
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);  // IB uses COUNT, not BYTES!!!
}
size_t Primitives::store(Primitive3D p) {
    return m_Primitives.store(p);
}
void Primitives::update(size_t oid, Primitive3D p) {
    m_Primitives.update(oid, p);
}
void Primitives::remove(size_t oid) {
    m_Primitives.remove(oid);
}
void Primitives::clear() {
    m_Primitives.clear();
}
void Primitives::draw(Camera* cam, unsigned int shadow) {
    if (m_Primitives.size() == 0) return;
    // Create an instance array and render using one allocated Primitive
    // - color4
    // - position3
    // - orientation3
    // - scale3
    // - rotation1

    if (shadow == SHADOW_MAP) { // Directional light source using square depth texture
        smshdr->Bind();
        smshdr->SetUniformMatrix4f("view", cam->getViewMat());
        smshdr->SetUniformMatrix4f("projection", cam->getProjMat());
        smshdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);
        smshdr->SetUniformMatrix4f("lightSpaceMatrix", m_scene->getShadowmapOb()->lightSpaceMatrix);
    }
    else if (shadow == SHADOW_BOX) { // Omnidirectional lightsource using cubemap depth texture
        sbshdr->Bind();
        for (unsigned int i = 0; i < 6; ++i)
            sbshdr->SetUniformMatrix4f("shadowMatrices[" + std::to_string(i) + "]", m_scene->getShadowboxOb()->shadowTransforms[i]);
        // Create a PointLight for this !!! Primitives can't need to know about Earth internals !!!
        glm::vec3 lightpos = m_scene->getEarth()->getSubsolarXYZ();
        sbshdr->SetUniform3f("lightPos", lightpos.x, lightpos.y, lightpos.z);
        sbshdr->SetUniform1f("far_plane", m_scene->getShadowboxOb()->far);
    }
    else { // NONE
        shdr->Bind();
        // NOTE: Consider passing in 1 multiplied matrix instead of these:
        shdr->SetUniformMatrix4f("view", cam->getViewMat());
        shdr->SetUniformMatrix4f("projection", cam->getProjMat());
        shdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);
    }
    // Set up and draw
    vb1->Bind();
    va->Bind();
    ib->Bind();
    // Make new vb2 every draw, since primitives might have been added.
    // NOTE: Or keep flag to track if primitives were added?
    //       Even if none were added, they may well have been modified.
    va->AddBuffer(*vb1, *vbl1, true);
    vb2 = new VertexBuffer(&m_Primitives[0], (unsigned int)m_Primitives.size() * sizeof(Primitive3D));
    vb2->Bind();
    va->AddBuffer(*vb2, *vbl2, false);
    // Primitives list (0,1,2 are in vertex list)
    glVertexAttribDivisor(3, 1);               // Color4
    glVertexAttribDivisor(4, 1);               // Pos3
    glVertexAttribDivisor(5, 1);               // Dir3
    glVertexAttribDivisor(6, 1);               // Scale3
    glVertexAttribDivisor(7, 1);               // Rot1
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDisable(GL_CULL_FACE);
    glDrawElementsInstanced(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0, (GLsizei)m_Primitives.size());
    //glEnable(GL_CULL_FACE);
    vb2->Unbind();
    ib->Unbind();
    va->Unbind();
    vb1->Unbind();
    delete vb2;
    if (shadow == SHADOW_MAP) smshdr->Unbind();
    else if (shadow == SHADOW_BOX) sbshdr->Unbind();
    else shdr->Unbind();
}
Primitive3D* Primitives::getDetails(size_t index) {
    return &m_Primitives[index];
}
glm::vec4 Primitives::getColor(size_t index) {
    return m_Primitives[index].color;
}
void Primitives::setColor(size_t index, glm::vec4 color) {
    m_Primitives[index].color = color;
}
// Adding a new primitive:
// - Add a proto for the class at the top of Primitive.h
// - Add primitive pointer variable to Application
// - Add getter for primitive pointer in Application
// - Add class definition in Primitives.h:
//   o Constructor
//   o add* instancing method(s)
//   o genGeom() method
//   o Optionally others like Print() or Validate() or Compact() etc
// - Add method definitions in Primitives.ccp
//   o Constructor must:
//     . Call Primitives constructor with world, verts, tris
//     . Set World pointer to itself m_world->SetNameOfPrimitive(this);
//     . m_Primitives.reserve(5000)
//     . genGeom() - child method to generate m_verts and m_tris in Primitive
//     . Init() (from Primitives, it needs the m_verts and m_tris populated by genGeom() first)
//   o From* methods must populate the Primitives3D { color, pos, dir, scale } tuple
//   o genGeom() must populate m_verts and m_tris:
//     . m_verts with { pos, normal, UV }
//     . m_tri with { a, b, c } in clockwise winding order
//     . Must not take any arguments because the virtual parent method is void(void). Pass by local or class variable if required
//   o Don't forget to add the new primitive to the appropriate Draw() loops.


// ---------
//  Minifig
// ---------
Minifigs::Minifigs(Scene* scene) : Primitives(scene, 10000, 10000) {
    //std::cout << "Minifigs::Minifigs()\n";
    m_Primitives.reserve(200);
    genGeom();
    init();
}
Minifigs::~Minifigs() {
    // Cleanup
}
size_t Minifigs::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color, float bearing) {
    return store({ color, pos, dir, glm::vec3(width, len, width), bearing });
}
void Minifigs::changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color, float bearing) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), bearing });
}
size_t Minifigs::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
void Minifigs::removeMinifig(size_t index) { remove(index); }
void Minifigs::genGeom() {
    std::string line;
    std::string::size_type sz;
    std::vector<glm::vec3> normals;
    int pos = 0;
    std::ifstream stream(m_objFile);
    //bool status = stream.good();
    //std::cout << "Shader file status: " << status << std::endl;
    // Bounding box
    //float max = 0.0f;
    //float may = 0.0f;
    //float maz = 0.0f;
    //float mix = 0.0f;
    //float miy = 0.0f;
    //float miz = 0.0f;
    while (getline(stream, line)) {
        if (line.find("v ") != std::string::npos) {
            pos = 2;
            // Vertex = 'v -0.747672 -1.724318 -0.418224'
            float x = std::stof(line.substr(pos), &sz);
            pos += (int)sz + 1;
            float y = std::stof(line.substr(pos), &sz);
            pos += (int)sz + 1;
            float z = std::stof(line.substr(pos), &sz);
            m_verts.emplace_back(glm::vec3(x, y, z), glm::vec3(0.0f), glm::vec2(0.0f));
            // Build bounding box
            //if (x > max) max = x;
            //if (y > may) may = y;
            //if (z > maz) maz = z;
            //if (x < mix) mix = x;
            //if (y < miy) miy = y;
            //if (z < miz) miz = z;

        }
        else if (line.find("vn ") != std::string::npos) {
            // Vertex normal = 'vn -0.0000 1.0000 -0.0000'
            pos = 3;
            float x = std::stof(line.substr(pos), &sz);
            pos += (int)sz + 1;
            float y = std::stof(line.substr(pos), &sz);
            pos += (int)sz + 1;
            float z = std::stof(line.substr(pos), &sz);
            normals.emplace_back(glm::vec3(x, y, z));
        }
        else if (line.find("f ") != std::string::npos) {
            // Tri = 'f 8//1 661//1 623//1'
            pos = 2;
            int a = std::stoi(line.substr(pos), &sz);
            pos += (int)sz + 2;
            int an = std::stoi(line.substr(pos), &sz);
            pos += (int)sz + 1;
            int b = std::stoi(line.substr(pos), &sz);
            pos += (int)sz + 2;
            int bn = std::stoi(line.substr(pos), &sz);
            pos += (int)sz + 1;
            int c = std::stoi(line.substr(pos), &sz);
            pos += (int)sz + 2;
            int cn = std::stoi(line.substr(pos), &sz);
            m_verts[(size_t)a-1].normal = normals[(size_t)an-1];
            m_verts[(size_t)b-1].normal = normals[(size_t)bn-1];
            m_verts[(size_t)c-1].normal = normals[(size_t)cn-1];
            m_tris.push_back({ (unsigned int)a-1, (unsigned int)b-1, (unsigned int)c-1 });
        }
    }
    //std::cout << "Max x, y, z: " << max << ", " << may << ", " << maz << "\n";
    //std::cout << "Min x, y, z: " << mix << ", " << miy << ", " << miz << "\n";
}


// -----------
//  UV Sphere
// -----------
SphereUV::SphereUV(Scene* scene) : Primitives(scene, 1000, 1000) {
    m_Primitives.reserve(5000);
    genGeom();
    init();
}
void SphereUV::print() {
    for (size_t i = 0; i < m_Primitives.size(); i++) {
        std::cout << "SphereUV " << i << ":" << std::endl;
        std::cout << " Color:     " << m_Primitives[i].color.r << m_Primitives[i].color.g << m_Primitives[i].color.b << m_Primitives[i].color.a << std::endl;
        std::cout << " Position:  " << m_Primitives[i].position.x << m_Primitives[i].position.y << m_Primitives[i].position.z << std::endl;
        std::cout << " Direction: " << m_Primitives[i].direction.x << m_Primitives[i].direction.y << m_Primitives[i].direction.z << std::endl;
        std::cout << " Scaling:   " << m_Primitives[i].scale.x << m_Primitives[i].scale.y << m_Primitives[i].scale.z << std::endl;
    }
}
size_t SphereUV::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    return store({ color, pos, dir, glm::vec3(width, len, width), 0.0f });
}
size_t SphereUV::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
void SphereUV::changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), 0.0f });
}
void SphereUV::changeStartEnd(size_t index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (width == NO_FLOAT) width = prim->scale.x;
    glm::vec3 dir = end - pos;
    update(index, { color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
void SphereUV::removeSphereUV(size_t index) { remove(index); }
glm::vec3 SphereUV::getLoc3D_NS(float lat, float lon, float height) {
    float m_Radius = 1.0f;
    float w = cos(lat) * (m_Radius + height);
    float x = cos(lon) * w;
    float y = sin(lon) * w;
    float z = sin(lat) * (m_Radius + height);
    return glm::vec3(x, y, z);
}
void SphereUV::genGeom() {
    float lat, lon;
    unsigned int meshV = 32; // Bands
    unsigned int meshU = 64; // Facets
    for (unsigned int v = 0; v <= meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
        lat = (pif * v / meshV) - pi2f;
        for (unsigned int u = 0; u <= meshU; u++) {
            lon = (tauf * u / meshU) - pif;
            glm::vec3 loc = getLoc3D_NS(lat, lon, 0.0f);     // position of vertex
            glm::vec3 nml = glm::normalize(loc);  // Insolation normal, always NS (or G8?)
            m_verts.push_back({ loc, nml, glm::vec2(u / (float)meshU,v / (float)meshV) });
            if (u < meshU && v < meshV) {
                m_tris.push_back({ (meshU + 1) * v + u , (meshU + 1) * (v + 1) + u, (meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (meshU + 1) * (v + 1) + 1 + u, (meshU + 1) * v + u + 1, (meshU + 1) * v + u });
            }
        }
    }
}


// --------
//  Planes
// --------
Planes::Planes(Scene* scene) : Primitives(scene, 8, 4) {
    // Reserve an initial number of primitives, adding beyond the reservation is still possible, but slow.
    m_Primitives.reserve(500);
    genGeom();
    init();
}
// Functions to instantiate the primitive
size_t Planes::addStartNormalLen(glm::vec3 pos, glm::vec3 nml,float rot, float len, glm::vec4 color) {
    // Takes the position of the center of the plane, the direction and rotation of the surface normal, a scale factor and the color
    return store({ color, pos, nml, glm::vec3(len, 1.0f, len), 0.0f });
}
size_t Planes::addStartUV(glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color) {
    // Takes the position of the center of the plane, and two vectors that "span" the plane, and the color
    //  Spanning means the plane diagonal will be -spanU,-spanV to spanU,spanV transported to position.
    float angle = -atan2(spanU.z, sqrt(spanU.x * spanU.x + spanU.y * spanU.y));  // Rotation about normal
    return store({ color, pos, glm::cross(spanU,spanV), glm::vec3(glm::length(spanU), 1.0f, glm::length(spanV)), angle });
}
void Planes::changeStartNormalLen(size_t index, glm::vec3 pos, glm::vec3 nml, float rot, float len, glm::vec4 color) {
    // Note: When specifying a non-infinite plane by normal and position, the rotation about the normal is not defined,
    //       so rot cannot be calculated from those parameters and is thus explicitly required (defaulting to 0 rads).
    update(index, { color, pos, nml, glm::vec3(len, 1.0f, len), rot });
}
void Planes::changeStartUV(size_t index, glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color) {
    float angle = -atan2(spanU.z, sqrt(spanU.x * spanU.x + spanU.y * spanU.y));
    update(index, { color, pos, glm::cross(spanU,spanV), glm::vec3(glm::length(spanU), 1.0f, glm::length(spanV)), angle });
}
void Planes::removePlane(size_t index) { remove(index); }
// Create the geometry (vertices and triangles)
void Planes::genGeom() {
    // Unit square in the XZ plane, centered on the origin, surface normal along Y axis - 2 tris front, 2 tris back
    // Although backface culling is on, there is still some Z fighing, so thickened the plane slightly
    m_verts.push_back({ glm::vec3( 0.5f, 0.00001f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
    m_verts.push_back({ glm::vec3(-0.5f, 0.00001f,  0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
    m_verts.push_back({ glm::vec3( 0.5f, 0.00001f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
    m_verts.push_back({ glm::vec3(-0.5f, 0.00001f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
    m_verts.push_back({ glm::vec3( 0.5f, 0.0f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
    m_verts.push_back({ glm::vec3(-0.5f, 0.0f,  0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
    m_verts.push_back({ glm::vec3( 0.5f, 0.0f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
    m_verts.push_back({ glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
    m_tris.push_back({ 0,3,2 });
    m_tris.push_back({ 0,1,3 });
    m_tris.push_back({ 4,6,7 });
    m_tris.push_back({ 4,7,5 });
}


// -----------
//  ViewCones
// -----------
ViewCones::ViewCones(Scene* scene) : Primitives(scene, 500, 500) {
    // ViewCones have higher tri count, so they are better suited for large cones. Otherwise they are identical to Cones
    m_Primitives.reserve(500);
    genGeom();
    init();
}
size_t ViewCones::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    return store({ color, pos, dir, glm::vec3(width, len, width), 0.0f });
}   
size_t ViewCones::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//unsigned int ViewCones::FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void ViewCones::changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), 0.0f });
}
void ViewCones::changeStartEnd(size_t index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (width == NO_FLOAT) width = prim->scale.x;
    glm::vec3 dir = end - pos;
    update(index, { color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
void ViewCones::removeViewCone(size_t index) { remove(index); }
//void ViewCones::UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void ViewCones::genGeom() {
    // Anchored at tip, to make Arrows and View Cones easier
    unsigned int facets = 64;
    float width = 1.0f;  // actually radius
    float height = 1.0f; // length of cylinder
    double lon;
    float clon;
    float clonw;
    float slon;
    float slonw;
    for (unsigned int u = 0; u <= facets; u++) { // Adds one extra vertex column to close the shape
        lon = tau * u / facets;                 //  This is simpler than indexing tris across seam
        clon = (float)cos(lon);
        clonw = clon * width;
        slon = (float)sin(lon);
        slonw = slon * width;
        // position3, normal3, uv2
        m_verts.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
        m_verts.push_back({ glm::vec3(clonw, -1.0f, slonw), glm::vec3(clon, 0.0f, slon), glm::vec2(0.0f, 0.0f) });
        m_verts.push_back({ glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
        if (u < facets) {
            m_tris.push_back({ u * 3, u * 3 + 1, (u + 1) * 3 + 1 });
            m_tris.push_back({ u * 3 + 1, u * 3 + 2, (u + 1) * 3 + 1 });
        }
    }
}


// -------
//  Cones
// -------
Cones::Cones(Scene* scene) : Primitives(scene, 100, 100) {
    // Cones specific
    m_Primitives.reserve(5000);
    genGeom();
    init();
}
void Cones::print() {
    for (size_t i = 0; i < m_Primitives.size(); i++) {
        std::cout << "Cone " << i << ":" << std::endl;
        std::cout << " Color:     " << m_Primitives[i].color.r << m_Primitives[i].color.g << m_Primitives[i].color.b << m_Primitives[i].color.a << std::endl;
        std::cout << " Position:  " << m_Primitives[i].position.x << m_Primitives[i].position.y << m_Primitives[i].position.z << std::endl;
        std::cout << " Direction: " << m_Primitives[i].direction.x << m_Primitives[i].direction.y << m_Primitives[i].direction.z << std::endl;
        std::cout << " Scaling:   " << m_Primitives[i].scale.x << m_Primitives[i].scale.y << m_Primitives[i].scale.z << std::endl;
    }
}
size_t Cones::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    return store({ color, pos, dir, glm::vec3(width, len, width), 0.0f });
}
size_t Cones::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//size_t Cones::FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void Cones::changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), 0.0f });
}
void Cones::changeStartEnd(size_t index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (width == NO_FLOAT) width = prim->scale.x;
    glm::vec3 dir = end - pos;
    update(index, { color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//void Cones::UpdateStartEleAzi(size_t index, glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void Cones::changeColorLengthWidth(size_t index, glm::vec4 color, float length, float width) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    prim->color = color;
    prim->scale = glm::vec3(width, length, width);
    //Update(index, *prim);
}
void Cones::removeCone(size_t index) { remove(index); }

void Cones::genGeom() {
    // Anchored at tip, to make Arrows and View Cones easier
    unsigned int facets = 16;
    float width = 1.0f;  // actually radius
    float height = 1.0f; // length of cylinder
    double lon;
    float clon,clonw,slon,slonw;
    for (unsigned int u = 0; u <= facets; u++) { // Adds one extra vertex column to close the shape
        lon = tau * u / facets;                 //  This is simpler than indexing tris across seam
        clon = (float)cos(lon);
        clonw = clon * width;
        slon = (float)sin(lon);
        slonw = slon * width;
        // position3, normal3, uv2
        m_verts.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
        m_verts.push_back({ glm::vec3(clonw, -1.0f, slonw), glm::vec3(clon, 0.0f, slon), glm::vec2(0.0f, 0.0f) });
        m_verts.push_back({ glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
        if (u < facets) {
            m_tris.push_back({ u * 3, u * 3 + 1, (u + 1) * 3 + 1 });
            m_tris.push_back({ u * 3 + 1, u * 3 + 2, (u + 1) * 3 + 1 });
        }
    }
}


// -----------
//  Cylinders
// -----------
Cylinders::Cylinders(Scene* scene) : Primitives(scene, 100, 100) {
    m_Primitives.reserve(5000);
    genGeom();
    init();
}
void Cylinders::print() {
    for (size_t i = 0; i < m_Primitives.size(); i++) {
        std::cout << "Cylinder " << i << ":" << std::endl;
        std::cout << " Color:     " << m_Primitives[i].color.r << m_Primitives[i].color.g << m_Primitives[i].color.b << m_Primitives[i].color.a << std::endl;
        std::cout << " Position:  " << m_Primitives[i].position.x << m_Primitives[i].position.y << m_Primitives[i].position.z << std::endl;
        std::cout << " Direction: " << m_Primitives[i].direction.x << m_Primitives[i].direction.y << m_Primitives[i].direction.z << std::endl;
        std::cout << " Scaling:   " << m_Primitives[i].scale.x << m_Primitives[i].scale.y << m_Primitives[i].scale.z << std::endl;
    }
}
size_t Cylinders::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, len, width), 0.0f });
}
size_t Cylinders::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//unsigned int Cylinders::FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void Cylinders::changeStartDirLen(size_t index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), 0.0f });
}
void Cylinders::changeStartEnd(size_t index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (width == NO_FLOAT) width = prim->scale.x;
    glm::vec3 dir = end - pos;
    update(index, { color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//void Cylinders::UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void Cylinders::changeColorLengthWidth(size_t index, glm::vec4 color, float length, float width) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    prim->color = color;
    prim->scale = glm::vec3(width, length, width);
}
void Cylinders::removeCylinder(size_t index) { remove(index); }
void Cylinders::genGeom() {
    unsigned int facets = 16;
    float width = 1.0f;  // actually radius
    float height = 1.0f; // length of cylinder
    double lon;
    float clon;
    float clonw;
    float slon;
    float slonw;
    for (unsigned int u = 0; u <= facets; u++) { // Adds one extra vertex column to close the shape
        lon = tau * u / facets;                 //  This is simpler than indexing tris across seam
        clon = (float)cos(lon);
        clonw = clon * width;
        slon = (float)sin(lon);
        slonw = slon * width;
        // position3, normal3, uv2
        m_verts.push_back({ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
        m_verts.push_back({ glm::vec3(clonw, 1.0f, slonw), glm::vec3(clon, 0.0f, slon), glm::vec2(0.0f, 0.0f) });
        m_verts.push_back({ glm::vec3(clonw, 0.0f, slonw), glm::vec3(clon, 0.0f, slon), glm::vec2(0.0f, 0.0f) });
        m_verts.push_back({ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) });
        if (u < facets) {
            m_tris.push_back({ u * 4    , u * 4 + 1, u * 4 + 5 });
            m_tris.push_back({ u * 4 + 1, u * 4 + 2, u * 4 + 6 });
            m_tris.push_back({ u * 4 + 1, u * 4 + 6, u * 4 + 5 });
            m_tris.push_back({ u * 4 + 2, u * 4 + 3, u * 4 + 6 });
        }
    }
}


// ----------
//  Sky Dots
// ----------
SkyDots::SkyDots(Scene* scene) : Primitives(scene, 2000, 1000) {
    m_Primitives.reserve(20000);
    genGeom();
    init();
}
size_t SkyDots::addXYZ(glm::vec3 pos, glm::vec4 color, float size) {
    return store({ color, pos, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(size,size,size), 0.0f }); // col,pos,dir,scale,rot
}
void SkyDots::changeXYZ(size_t index, glm::vec3 pos, glm::vec4 color, float size) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (size == NO_FLOAT) size = prim->scale.x;
    update(index, { color, pos, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(size,size,size), 0.0f });
}
void SkyDots::changeDot(size_t index, glm::vec4 color, float size) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (size == NO_FLOAT) size = prim->scale.x;
    prim->color = color;
    prim->scale = glm::vec3(size);
}
void SkyDots::removeDot(size_t index) { remove(index); }
void SkyDots::draw(Camera* cam) {
    if (!visible) return;
    if (m_Primitives.size() == 0) return;
    // Create an instance array and render using one allocated Primitive
    // - color4
    // - position3
    // - orientation3
    // - scale3
    // - rotation1

    shdr->Bind();
    // NOTE: Consider passing in 1 multiplied matrix instead of these:
    //shdr->SetUniformMatrix4f("view", cam->getSkyViewMat());
    shdr->SetUniformMatrix4f("view", cam->getViewMat());
    shdr->SetUniformMatrix4f("projection", cam->getProjMat());
    shdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);

    // Set up and draw
    vb1->Bind();
    va->Bind();
    ib->Bind();
    // Make new vb2 every draw, since primitives might have been added.
    // NOTE: Or keep flag to track if primitives were added?
    //       Even if none were added, they may well have been modified.
    va->AddBuffer(*vb1, *vbl1, true);
    vb2 = new VertexBuffer(&m_Primitives[0], (unsigned int)m_Primitives.size() * sizeof(Primitive3D));
    vb2->Bind();
    va->AddBuffer(*vb2, *vbl2, false);
    // Primitives list (0,1,2 are in vertex list)
    glVertexAttribDivisor(3, 1);               // Color4
    glVertexAttribDivisor(4, 1);               // Pos3
    glVertexAttribDivisor(5, 1);               // Dir3
    glVertexAttribDivisor(6, 1);               // Scale3
    glVertexAttribDivisor(7, 1);               // Rot1
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glDisable(GL_CULL_FACE);
    glDrawElementsInstanced(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0, (GLsizei)m_Primitives.size());
    //glEnable(GL_CULL_FACE);
    vb2->Unbind();
    ib->Unbind();
    va->Unbind();
    vb1->Unbind();
    delete vb2;
    shdr->Unbind();
}

void SkyDots::genGeom() {
    // IcoSphere generation from: https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
    // See also: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
    // and: https://observablehq.com/@mourner/fast-icosphere-mesh
    unsigned int order = 3;
    float scale = 1.0f;
    m_verts.reserve((size_t)(10 * pow(4, order) + 2));
    m_tris.reserve((size_t)(20 * pow(4, order)));
    VertexList vertices = icovertices;
    TriangleList triangles = icotriangles;
    for (unsigned int i = 0; i < order; ++i) {
        triangles = subdivide(vertices, triangles);
    }
    // Transfer to "regular" arrays - Why? Because I don't fully understand the subdiv algorithm
    for (auto& v : vertices) {
        m_verts.push_back({ v, v, glm::vec2(0.0f,0.0f) }); // pos,nml,uv
    }
    for (auto& t : triangles) {
        m_tris.push_back({ (unsigned int)t.vertex[0],(unsigned int)t.vertex[1],(unsigned int)t.vertex[2] });
    }
    return;
}
SkyDots::TriangleList SkyDots::subdivide(VertexList& vertices, TriangleList triangles) {
    Lookup lookup;
    TriangleList result;
    for (auto&& each : triangles) {
        std::array<SkyDots::Index, 3> mid = { 0, 0, 0 };      // Index is simply a typedef unsigned int
        for (int edge = 0; edge < 3; ++edge) {
            mid[edge] = vertex_for_edge(lookup, vertices, each.vertex[edge], each.vertex[(edge + 1) % 3]);
        }
        // Return 4 smaller triangles
        result.push_back({ each.vertex[0], mid[0], mid[2] });
        result.push_back({ each.vertex[1], mid[1], mid[0] });
        result.push_back({ each.vertex[2], mid[2], mid[1] });
        result.push_back({ mid[0], mid[1], mid[2] });
    }
    return result;
}
SkyDots::Index SkyDots::vertex_for_edge(Lookup& lookup, VertexList& vertices, Index first, Index second) {
    Lookup::key_type key(first, second);
    if (key.first > key.second) std::swap(key.first, key.second);
    auto inserted = lookup.insert({ key, vertices.size() });
    if (inserted.second) {
        auto& edge0 = vertices[first];
        auto& edge1 = vertices[second];
        auto point = normalize(edge0 + edge1);  // Scale to unit sphere
        vertices.push_back(point);
    }
    return inserted.first->second;
}


// ------
//  Dots
// ------
Dots::Dots(Scene* scene) : Primitives(scene, 2000, 1000) {
    m_Primitives.reserve(20000);
    genGeom();
    init();
}
size_t Dots::addXYZ(glm::vec3 pos, glm::vec4 color, float size) {
    return store({ color, pos, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(size,size,size), 0.0f }); // col,pos,dir,scale,rot
}
void Dots::changeXYZ(size_t index, glm::vec3 pos, glm::vec4 color, float size) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (size == NO_FLOAT) size = prim->scale.x;
    update(index, { color, pos, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(size,size,size), 0.0f });
}
void Dots::changeDot(size_t index, glm::vec4 color, float size) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (size == NO_FLOAT) size = prim->scale.x;
    prim->color = color;
    prim->scale = glm::vec3(size);
}
void Dots::removeDot(size_t index) { remove(index); }
void Dots::genGeom() {
    // IcoSphere generation from: https://schneide.blog/2016/07/15/generating-an-icosphere-in-c/
    // See also: http://blog.andreaskahler.com/2009/06/creating-icosphere-mesh-in-code.html
    // and: https://observablehq.com/@mourner/fast-icosphere-mesh
    unsigned int order = 3;
    float scale = 1.0f;
    m_verts.reserve((size_t)(10 * pow(4, order) + 2));
    m_tris.reserve((size_t)(20 * pow(4, order)));
    VertexList vertices = icovertices;
    TriangleList triangles = icotriangles;
    for (unsigned int i = 0; i < order; ++i) {
        triangles = subdivide(vertices, triangles);
    }
    // Transfer to "regular" arrays - Why? Because I don't fully understand the subdiv algorithm
    for (auto& v : vertices) {
        m_verts.push_back({ v, v, glm::vec2(0.0f,0.0f) }); // pos,nml,uv
    }
    for (auto& t : triangles) {
        m_tris.push_back({ (unsigned int)t.vertex[0],(unsigned int)t.vertex[1],(unsigned int)t.vertex[2] });
    }
    return;
}
Dots::TriangleList Dots::subdivide(VertexList& vertices, TriangleList triangles) {
    Lookup lookup;
    TriangleList result;
    for (auto&& each : triangles) {
        std::array<Dots::Index, 3> mid = { 0, 0, 0 };      // Index is simply a typedef unsigned int
        for (int edge = 0; edge < 3; ++edge) {
            mid[edge] = vertex_for_edge(lookup, vertices, each.vertex[edge], each.vertex[(edge + 1) % 3]);
        }
        // Return 4 smaller triangles
        result.push_back({ each.vertex[0], mid[0], mid[2] });
        result.push_back({ each.vertex[1], mid[1], mid[0] });
        result.push_back({ each.vertex[2], mid[2], mid[1] });
        result.push_back({ mid[0], mid[1], mid[2] });
    }
    return result;
}
Dots::Index Dots::vertex_for_edge(Lookup& lookup, VertexList& vertices, Index first, Index second) {
    Lookup::key_type key(first, second);
    if (key.first > key.second) std::swap(key.first, key.second);
    auto inserted = lookup.insert({ key, vertices.size() });
    if (inserted.second) {
        auto& edge0 = vertices[first];
        auto& edge1 = vertices[second];
        auto point = normalize(edge0 + edge1);  // Scale to unit sphere
        vertices.push_back(point);
    }
    return inserted.first->second;
}
