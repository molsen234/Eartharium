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
Camera::Camera(Scene* scene) : m_scene(scene) {
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
void Camera::setCamLightPos(glm::vec3 lPos) { CamLightDir = glm::normalize(lPos); }
void Camera::setLatLon(float lat, float lon) {
    if (lat != NO_FLOAT) camLat = lat;
    if (lon != NO_FLOAT) camLon = lon;
    update();
}
void Camera::update() {
    setPosLLH({ camLat, camLon, camDst });
    glm::vec3 lPos = getPosXYZ();
    lPos += getRight() * -camlightsep;
    lPos += getUp() * camlightsep;
    setCamLightPos(lPos);
}
void Camera::setLookAt(glm::vec3 position, glm::vec3 target, glm::vec3 upwards) {
    m_position = position;
    m_target = target;
    worldUp = upwards;
    Recalc();
}
void Camera::setPosXYZ(glm::vec3 pos) {
    m_position = pos;
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
    m_position.x = (float)cos(deg2rad * llh.lon) * camW;
    m_position.y = (float)sin(deg2rad * llh.lon) * camW;
    m_position.z = (float)sin(deg2rad * llh.lat) * (float)llh.dst;
    Recalc();
}
glm::vec3 Camera::getPosXYZ() { return m_position; }
void Camera::setTarget(glm::vec3 target) {
    m_target = target;
    ViewMat = glm::lookAt(m_position, m_target, cameraUp);
    //Recalc();
}
void Camera::setFoV(float fov) { // !!! Is a public variable, so probably get rid of get/set methods and do one Recalc() every frame (after GUI etc)
    camFoV = fov;
    Recalc();
    //ProjMat = glm::perspective(glm::radians(m_fov), (float)m_world->w_width / (float)m_world->w_height, 0.1f, 100.0f);
}
float Camera::getFoV() { return camFoV; }
glm::mat4 Camera::getViewMat() { return ViewMat; }
glm::mat4 Camera::getSkyViewMat() { return glm::lookAt(glm::vec3(0.0f), m_target - m_position, cameraUp); }
glm::mat4 Camera::getProjMat() { return ProjMat; }
glm::vec3 Camera::getRight() { return cameraRight; }
glm::vec3 Camera::getUp() { return cameraUp; }
glm::vec3 Camera::getPosition() { return m_position; }
void Camera::Recalc() {
    m_direction = glm::normalize(m_position - m_target);
    cameraRight = glm::normalize(glm::cross(worldUp, m_direction));
    cameraUp = glm::cross(m_direction, cameraRight); // Part of worldUp that fits with cameraDirection
    ViewMat = glm::lookAt(m_position, m_target, cameraUp);
    ProjMat = glm::perspective(glm::radians(camFoV), m_scene->getAspect(), camNear, camFar);
}


// -------
//  Scene
// -------
Scene::Scene(Application* app) : m_app(app) {
    // Set up a default Camera
    w_camera = newCamera();  // Will also be m_cameras[0]
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
    if (m_dotsOb != nullptr) delete m_dotsOb;
    if (m_astro != nullptr) delete m_astro;
}
Camera* Scene::newCamera() {
    Camera* cam = new Camera(this);
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
    //if (m_lettersOb != nullptr) { m_lettersOb->clear(); }
    // Should delete shader library here !!!
}
void Scene::render() {
    // Should take fbo render target !!!
    // Might take CelestialMech to be able to switch between eras? 
    if (m_earthOb != nullptr) m_earthOb->Update(); // Make sure primitives are up to date before casting their shadows (earth updates locations)
    // Do shadow pass here
    if (shadows == SHADOW_MAP) {
        if (m_shadowmap != nullptr) m_shadowmap->Render();
        else std::cout << "World::do_render(): Map shadows have been enabled, but no ShadowMap object was found.\n";
    }
    if (shadows == SHADOW_BOX) { // For now hardcoded to subsolar point, also in Primitives
        if (m_shadowbox != nullptr) m_shadowbox->Render(m_earthOb->getSubsolarXYZ());
        else std::cout << "World::do_render(): Box shadows have been enabled, but no ShadowBox object was found.\n";
    }
    // Do render pass here
    if (m_skyboxOb != nullptr) {
        m_skyboxOb->Draw();
    }
    if (m_skysphereOb != nullptr) {
        m_skysphereOb->Draw();
    }
    if (m_earthOb != nullptr) m_earthOb->Draw();
    if (m_solsysOb != nullptr) m_solsysOb->Draw();
    if (m_minifigsOb != nullptr) m_minifigsOb->draw(NONE);
    if (m_sphereuvOb != nullptr) m_sphereuvOb->draw(NONE);
    if (m_cylindersOb != nullptr) m_cylindersOb->draw(NONE);
    if (m_planesOb != nullptr) m_planesOb->draw(NONE);
    if (m_conesOb != nullptr) m_conesOb->draw(NONE);
    if (m_dotsOb != nullptr) m_dotsOb->draw(NONE);
    if (m_viewconesOb != nullptr) m_viewconesOb->draw(NONE);
    if (m_anglearcsOb != nullptr) m_anglearcsOb->draw();
    for (auto& p : m_polycurves) {
        p->draw();
    }
}
Dots* Scene::getDotsOb() {
    if (m_dotsOb == nullptr) m_dotsOb = new Dots(this);
    return m_dotsOb;
}
Cylinders* Scene::getCylindersOb() {
    if (m_cylindersOb == nullptr) m_cylindersOb = new Cylinders(this);
    return m_cylindersOb;
}
Cones* Scene::getConesOb() {
    if (m_conesOb == nullptr) m_conesOb = new Cones(this);
    return m_conesOb;
}
ViewCones* Scene::getViewConesOb() {
    if (m_viewconesOb == nullptr) m_viewconesOb = new ViewCones(this);
    return m_viewconesOb;
}
Planes* Scene::getPlanesOb() {
    if (m_planesOb == nullptr) m_planesOb = new Planes(this);
    return m_planesOb;
}
//Letters* Scene::getLettersOb() {
//    if (m_lettersOb == nullptr) m_lettersOb = new Letters(htis);
//    return m_lettersOb;
//}
SphereUV* Scene::getSphereUVOb() {
    if (m_sphereuvOb == nullptr) m_sphereuvOb = new SphereUV(this);
    return m_sphereuvOb;
}
Arrows* Scene::getArrowsOb() {
    if (m_arrowsOb == nullptr) m_arrowsOb = new Arrows(this);
    return m_arrowsOb;
}
AngleArcs* Scene::getAngleArcsOb() {
    if (m_anglearcsOb == nullptr) m_anglearcsOb = new AngleArcs(this);
    return m_anglearcsOb;
}

SkySphere* Scene::newSkysphere(unsigned int mU, unsigned int mV) {
    if (m_skysphereOb == nullptr) m_skysphereOb = new SkySphere(this, mU, mV);  // Make geometry configurable
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
PolyCurve* Scene::newPolyCurve(glm::vec4 color, float width, unsigned int reserve) {
    m_polycurves.emplace_back(new PolyCurve(this, color, width, reserve));
    return m_polycurves.back();
}
void Scene::deletePolyCurve(PolyCurve* curve) {
    auto it = std::find(m_polycurves.begin(), m_polycurves.end(), curve);
    if (it != m_polycurves.end()) {
        std::swap(*it, m_polycurves.back());
        m_polycurves.pop_back();
        delete curve;
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
RenderLayer3D::RenderLayer3D(float vpx1, float vpy1, float vpx2, float vpy2, Scene* scene, Astronomy* astro, Camera* cam)
    : m_scene(scene), m_astro(astro), RenderLayer(vpx1, vpy1, vpx2, vpy2) {
    m_scene->m_astro = m_astro;
    float w = (float)m_scene->m_app->getWidth();
    float h = (float)m_scene->m_app->getHeight();
    updateViewport(w, h);
    // Should take Viewport parameters, and maybe an alpha value or blend function id
    // Viewport parameters should probably be in NDC, to work with window resizing
    // Also take a render target, i.e. a (complete) frame buffer object
    // Perhaps user requests a new RenderLayer from the RenderChain (or Application?) class,
    //  somewhere that keeps track of the layer order and allows insertions (e.g. a PiP, under GUI, but above default)
    // In that case, the constructor might be protected, and RenderChain a friend.
    type = LAYER3D;
}
void RenderLayer3D::render() {
    glViewport((GLint)vp_x, (GLint)vp_y, (GLsizei)vp_w, (GLsizei)vp_h);
    glClear(GL_DEPTH_BUFFER_BIT);
    m_scene->render();
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
                    }
                    // Time
                    if (ImGui::CollapsingHeader("Time")) {
                        ImGui::Checkbox("Equation of Time", &do_eot);
                        if (l.layer->m_scene->m_app->do_eot) ImGui::Text("Local Solar Noon:");
                        else ImGui::Text("12:00:00 Local Time:");
                        ImGui::Text(l.layer->m_astro->timestr.c_str());
                        // This clearly needs more work... For now can select a time interval to step forwards/backwards with buttons.
                        // Make this work with the slider for 24hrs, EoT, and anim (SPACE)
                        // Also make a Solar year/day where the Sun returns to the same Longitude
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
                            // { { 1, 0, 0.0, 0.0, 0.0, 0.0 }, "Calendar Year" },  // Can't pass year or month to Astronomy->addTime() !!!
                            //{ { 0, 1, 0.0, 0.0, 0.0, 0.0 }, "Calendar Month" },
                            { { 0, 0, 0.0, 23.0, 56.0, 4.0905 }, "Sidereal Day" },
                            { { 0, 0, 1.0, 0.0, 0.0, 0.0 }, "Calendar Day" },
                            { { 0, 0, 0.0, 1.0, 0.0, 0.0 }, "1 Hour" },
                            { { 0, 0, 0.0, 0.0, 15.0, 0.0 }, "15 Minutes" },
                            { { 0, 0, 0.0, 0.0, 4.0, 0.0 }, "4 Minutes" }, // Sun moves ~1 degree
                            { { 0, 0, 0.0, 0.0, 1.0, 0.0 }, "1 Minute" }
                        };
                        static int idx;
                        for (idx = 0; idx < IM_ARRAYSIZE(timesteps); idx++)
                            if (timesteps[idx].Value == mytimestep)
                                break;
                        static const char* preview_text = timesteps[idx].Name;
                        //const char* preview_text = timesteps[idx].Name;
                        if (ImGui::BeginCombo("Step", preview_text))
                        {
                            for (int n = 0; n < IM_ARRAYSIZE(timesteps); n++)
                                if (ImGui::Selectable(timesteps[n].Name, idx == n)) {
                                    mytimestep = timesteps[n].Value;
                                    preview_text = timesteps[n].Name;
                                }
                            ImGui::EndCombo();
                        }

                        //ImGui::SameLine();
                        if (ImGui::Button("-1 Step")) {
                            //minusday = true;
                            l.layer->m_astro->addTime(-mytimestep.da, -mytimestep.hr, -mytimestep.mi, -mytimestep.se);
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("+1 Step")) {
                            //plusday = true;
                            l.layer->m_astro->addTime(mytimestep.da, mytimestep.hr, mytimestep.mi, mytimestep.se);
                        }
                        ImGui::SliderFloat("Time of Day", &slideday, 0.0f, 24.0f);
                        //ImGui::SliderFloat("year", &slideyear, 0.0f, 365.0f);
                    }
                    // Solar System object
                    if (l.layer->m_scene->m_solsysOb != nullptr) { // Oh, referencing m_scene without a scene added is causing a crash
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
                            const char* items[] = { "AENS", "AEER", "AERC", "AEE8", "NSER", "NSRC", "NSE8", "ERRC", "ERE8", "RCE8" };
                            if (ImGui::BeginCombo("Earth type", l.layer->m_scene->m_app->currentEarth->current_earth.c_str()))
                            {
                                for (int n = 0; n < IM_ARRAYSIZE(items); n++)
                                {
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
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }

        //if (m_shadowbox != nullptr) {
        //    if (ImGui::CollapsingHeader("Shadows")) {
        //        ImGui::SliderFloat("Near", &GetShadowBoxOb()->near, 0.00001f, 1.0f);
        //        ImGui::SliderFloat("Far", &GetShadowBoxOb()->far, 1.0f, 25.0f);
        //        //ImGui::SliderFloat("Cam<->Light", &camlightsep, 0.0f, 10.0f);
        //    }
        //}
        // ImGui::ShowDemoWindow();
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
//    m_renderchain = new RenderChain(this);
    m_shaderlib = new ShaderLibrary();
    m_layers.reserve(16); // ?? Might need to be a list instead of a vector
//m_layers.push_back(new RenderLayer(m_application->getWindow2Viewport())); // Default render layer

}
// DESTRUCTOR: Should ideally tear down the things we set up

//RenderChain* Application::getRenderChain() { return m_renderchain; }
ShaderLibrary* Application::getShaderLib() { return m_shaderlib; }
Astronomy* Application::newAstronomy() { return new Astronomy(); }  // These may want to save references for clean-up, or maybe not !!!
Scene* Application::newScene() { return new Scene(this); }
int Application::initWindow() {
    if (start_fullscreen) w_width = 1920;
    else w_width = 800;
    if (start_fullscreen) w_height = 1080;
    else w_height = 600;

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
        std::cout << "Camera dump at frame: " << currentframe << "\n";
        std::cout << "camFoV = " << currentCam->camFoV << "f\n";  // TODO: Add .0f formatting
        std::cout << "camLat = " << currentCam->camLat << "f\n";
        std::cout << "camLon = " << currentCam->camLon << "f\n";
        std::cout << "camDst = " << currentCam->camDst << "f\n";
        dumpcam = false;
    }
}
float Application::getAspect() {
    if (w_height == 0) {
        std::cout << "ERROR! Application::getAspect(): m_winVP.vp_h is zero, so aspect ratio will cause DIV0 exception! Returning 0.0f instead.\n";
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
RenderLayer3D* Application::newLayer3D(float vpx1, float vpy1, float vpx2, float vpy2, Scene* scene, Astronomy* astro, Camera* cam) {
    if (cam == nullptr) cam = scene->w_camera;
    RenderLayer3D* layer = new RenderLayer3D(vpx1, vpy1, vpx2, vpy2, scene, astro, cam);
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
    for (auto& l : m_layers) {
        l->updateViewport(width, height);
    }
}
void Application::render() {
    update();
    //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    beginImGUI();
    for (auto& l : m_layers) {
        l->animateViewport();
        l->render();
    }
    endImGUI();
    if (renderoutput) RenderFrame(0); // Save PNG of front buffer.
    glfwSwapBuffers(window); // Swap rendered screen to front
    currentframe++;
    if (currentframe == 100000) incSequence(); // Protect against issues with frame counter wrapping in filename which only allows 5 digits
}
void Application::RenderFrame(unsigned int framebuffer) {
    std::string fullname = "C:\\Coding\\Eartharium\\Eartharium\\AnimOut\\" + basefname;
    char numerator[20];
    sprintf(numerator, "S%03d-%05d.png", currentseq, currentframe);
    fullname.append(numerator);
    saveImage(fullname, window, 0);               // default frame buffer
    //saveImage(fullname, window, framebuffer);
    std::cout << "Rendered Frame " << currentframe << " to " << fullname << "\n";
    //delete[] numerator;  // NO! It is stack allocated!
}
void Application::incSequence() {
    currentseq++;
    currentframe = 0;
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
    double timerot = hrs2rad * rad2deg * -m_scene->m_astro->getGsid(); //  -100.0; NOTE: Probably because cubemap is loaded incorrectly !!!
    m_scene->w_camera->camLon -= (float)timerot;
    m_scene->w_camera->update();
    glm::mat4 view = glm::mat4(glm::mat3(m_scene->w_camera->getViewMat()));
    m_scene->w_camera->camLon += (float)timerot;
    m_scene->w_camera->update();
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
ParticleTrail::ParticleTrail(Scene* scene, unsigned int number, glm::vec4 color, unsigned int spacing) : m_scene(scene) {
    m_number = number;
    m_color = color;
    m_size = 0.02f;
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
    // Reduce size and opacity of trail
    for (auto& p : m_queue) {  // Here whole trail could be faded out when desired
        p.size *= (float)m_sizefactor;
        //p.color.a *= 0.95f;
        m_scene->getDotsOb()->changeXYZ(p.index, p.position, p.color, p.size);
    }
    if (m_queue.size() >= m_number) {
        m_scene->getDotsOb()->remove(m_queue.back().index);
        m_queue.pop_back();
    }
    unsigned int index = m_scene->getDotsOb()->addXYZ(pos, m_color, size);
    m_queue.push_front({ m_color, pos, size, index } );
    m_gap = m_spacing;
}
void ParticleTrail::clear() {
    for (auto& p : m_queue) {
        m_scene->getDotsOb()->remove(p.index);
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
            m_scene->getDotsOb()->remove(m_queue.at(i).index);
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
double AngleArcs::getAngle(unsigned int index) {
    return m_arcs[index].angle;
}
unsigned int AngleArcs::add(glm::vec3 position, glm::vec3 start, glm::vec3 stop, float length, glm::vec4 color, float width) {
    glm::vec3 nstart = glm::normalize(start);
    glm::vec3 nstop = glm::normalize(stop);
    glm::vec3 axis = glm::cross(nstart, nstop); // Use nstart early, it is rescaled later when generating points!
    double rangle = acos(glm::dot(nstart, nstop)); // angle in radians

    m_arcs.push_back({ nullptr, 0, color, position, start, stop, length, width, rad2deg * rangle, false });

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
    return (unsigned int)m_arcs.size() - 1;
}
void AngleArcs::remove(unsigned int index) {
    m_scene->deletePolyCurve(m_arcs[index].polycurve);
    m_arcs[index].expired = true;
}
void AngleArcs::update(unsigned int index, glm::vec3 position, glm::vec3 start, glm::vec3 stop, float length, glm::vec4 color, float width) {
    bool dirty = (position != m_arcs[index].position || start != m_arcs[index].start || stop != m_arcs[index].stop || length != m_arcs[index].length || width != m_arcs[index].width);
    if (position != NO_VEC3) m_arcs[index].position = position;
    if (start != NO_VEC3) m_arcs[index].start = start;
    if (stop != NO_VEC3) m_arcs[index].stop = stop;
    if (length != NO_FLOAT) m_arcs[index].length = length;
    if (color != NO_COLOR) m_arcs[index].color = color;
    if (width != NO_FLOAT) m_arcs[index].width = width;
    // could check if position/start/stop have changed, and skip generator.
    if (true) {
        glm::vec3 nstart = glm::normalize(m_arcs[index].start);
        glm::vec3 nstop = glm::normalize(m_arcs[index].stop);
        glm::vec3 axis = glm::cross(nstart, nstop); // Use nstart early, it is rescaled later when generating points!
        double rangle = acos(glm::dot(nstart, nstop)); // angle in radians
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
}

void AngleArcs::draw() {
    for (auto& a : m_arcs) {
        a.polycurve->draw();
    }
}


// -----------
//  PolyCurve
// -----------
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
void PolyCurve::addPoint(glm::vec3 point) {
    //if (limit) std::cout << "WARNING PolyPolyCurve (" << this << ") adding beyond capacity, resizing!\n"; // only triggers if coming back after setting limit flag
    m_points.push_back(point);
    if (m_points.size() == m_points.capacity()) {
        std::cout << "WARNING: PolyCurve (" << this << ") capacity: " << m_points.capacity() << " reached. It will be SLOW to add new points now!\n";
    //    limit = true;
    }
}
void PolyCurve::clearPoints() {
    m_points.clear();
    m_segments.clear(); // Just in case someone would clear the points and not call generate() to update segments.
    //limit = false;
}
void PolyCurve::generate() {
    // Simply figure out the position, orientation and scale of each cylinder segment
    // and build instance table. Cylinders are actually instantiated on GPU
    m_segments.clear();
    for (unsigned int i = 1; i < m_points.size(); i++) {
        glm::vec3 pos = m_points[i - 1];
        glm::vec3 dir = m_points[i] - m_points[i - 1];
        glm::vec3 scale = glm::vec3(m_width, glm::length(dir), m_width);
        // color(4), pos(3), dir(3), scale(3)
        m_segments.push_back({ m_color, pos, dir, scale, 0.0 });
    }
}
void PolyCurve::draw() {
    if (m_segments.size() == 0) return;
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
    shdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
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
    m_cylinders = m_scene->getCylindersOb();
    m_cones = m_scene->getConesOb();
    m_arrows.reserve(5000);
}
Arrows::~Arrows() {
    // Delete Cones and Cylinders here, some brutal caller may decide recreating Arrows is the easiest way to delete a bunch of them
    for (auto& a : m_arrows.m_Elements) {
        m_cylinders->remove(a.cylinder);
        m_cones->remove(a.cone);
    }
}
void Arrows::remove(unsigned int index) {
    m_cylinders->remove(m_arrows[index].cylinder);
    m_cones->remove(m_arrows[index].cone);
    m_arrows.remove(index);
}
unsigned int Arrows::store(Arrow a) {
    return m_arrows.store(a);
}
void Arrows::draw() {
    // Why do I even have this function here? !!!
    std::cout << "Arrows::Draw() was called. No need to do that, arrows are drawn as Cones and Cylinders.\n";
}
unsigned int Arrows::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    dir = glm::normalize(dir);
    unsigned int cone = m_cones->addStartDirLen(pos + dir * len, dir, width * 20, width * 4, color);
    unsigned int cyl = m_cylinders->addStartDirLen(pos, dir * len, len - width * 20, width, color);
    return store({ cyl, cone, color, pos, dir, len, width });
}
unsigned int Arrows::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    unsigned int cone = m_cones->addStartDirLen(end, dir, width * 20, width * 4, color);
    unsigned int cyl = m_cylinders->addStartDirLen(pos, dir, glm::length(dir) - width * 20, width, color);
    return store({ cyl, cone, color, pos, dir, glm::length(dir), width });
}
void Arrows::changeStartDirLen(unsigned int arrow, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    if (length == NO_FLOAT) length = m_arrows[arrow].length;
    if (width == NO_FLOAT) width = m_arrows[arrow].width;
    if (color == NO_COLOR) color = m_arrows[arrow].color;
    m_cones->changeStartDirLen(m_arrows[arrow].cone, pos + dir * length, dir, width * 20, width * 4, color);
    m_cylinders->changeStartDirLen(m_arrows[arrow].cylinder, pos, dir, length - width * 20, width, color);
    m_arrows[arrow] = { m_arrows[arrow].cylinder, m_arrows[arrow].cone, color, pos, dir, length, width };
}
void Arrows::changeStartEnd(unsigned int arrow, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    if (width == NO_FLOAT) width = m_arrows[arrow].width;
    if (color == NO_COLOR) color = m_arrows[arrow].color;
    glm::vec3 dir = end - pos;
    m_cones->changeStartDirLen(m_arrows[arrow].cone, end, dir, width * 20, width * 4, color);
    m_cylinders->changeStartDirLen(m_arrows[arrow].cylinder, pos, dir, glm::length(dir) - width * 20, width, color);
    m_arrows[arrow] = { m_arrows[arrow].cylinder, m_arrows[arrow].cone, color, pos, dir, glm::length(dir), width };
}
void Arrows::changeArrow(unsigned int index, glm::vec4 color, float length, float width) {
    if (color != NO_COLOR) m_arrows[index].color = color;
    if (length != NO_FLOAT) m_arrows[index].length = length;
    if (width != NO_FLOAT)  m_arrows[index].width = width;
    m_cones->changeColorLengthWidth(m_arrows[index].cone, color, length, width);
    m_cylinders->changeColorLengthWidth(m_arrows[index].cylinder, color, length, width);
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
unsigned int Primitives::store(Primitive3D p) {
    return m_Primitives.store(p);
}
void Primitives::update(unsigned int oid, Primitive3D p) {
    m_Primitives.update(oid, p);
}
void Primitives::remove(unsigned int oid) {
    m_Primitives.remove(oid);
}
void Primitives::clear() {
    m_Primitives.clear();
}
void Primitives::draw(unsigned int shadow) {
    if (m_Primitives.size() == 0) return;
    // Create an instance array and render using one allocated Primitive
    // - color4
    // - position3
    // - orientation3
    // - scale3
    // - rotation1

    if (shadow == SHADOW_MAP) { // Directional light source using square depth texture
        smshdr->Bind();
        smshdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
        smshdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
        smshdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
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
        shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
        shdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
        shdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
    }
    // Set up and draw
    vb1->Bind();
    va->Bind();
    ib->Bind();
    // Make new vb2 every draw, since primitives might have been added.
    // NOTE: Or keep flag to track if priimitives were added?
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
    glDisable(GL_CULL_FACE);
    glDrawElementsInstanced(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0, (GLsizei)m_Primitives.size());
    glEnable(GL_CULL_FACE);
    vb2->Unbind();
    ib->Unbind();
    va->Unbind();
    vb1->Unbind();
    delete vb2;
    if (shadow == SHADOW_MAP) smshdr->Unbind();
    else if (shadow == SHADOW_BOX) sbshdr->Unbind();
    else shdr->Unbind();
}
Primitive3D* Primitives::getDetails(unsigned int index) {
    return &m_Primitives[index];
}
glm::vec4 Primitives::getColor(unsigned int index) {
    return m_Primitives[index].color;
}
void Primitives::setColor(unsigned int index, glm::vec4 color) {
    m_Primitives[index].color = color;
}
// Adding a new primitive:
// - Add a proto for the class at the top of Primitive.h
// - Add primitive pointer variable to Application
// - Add getter for primitive pointer in Application
// - Add class definition in Primitives.h:
//   o Constructor
//   o From* instancing method(s)
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
unsigned int Minifigs::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color, float bearing) {
    return store({ color, pos, dir, glm::vec3(width, len, width), bearing });
}
void Minifigs::changeStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color, float bearing) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), bearing });
}
unsigned int Minifigs::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}

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
    // Cones specific
    m_Primitives.reserve(5000);
    genGeom();
    init();
}
void SphereUV::print() {
    for (unsigned int i = 0; i < m_Primitives.size(); i++) {
        std::cout << "SphereUV " << i << ":" << std::endl;
        std::cout << " Color:     " << m_Primitives[i].color.r << m_Primitives[i].color.g << m_Primitives[i].color.b << m_Primitives[i].color.a << std::endl;
        std::cout << " Position:  " << m_Primitives[i].position.x << m_Primitives[i].position.y << m_Primitives[i].position.z << std::endl;
        std::cout << " Direction: " << m_Primitives[i].direction.x << m_Primitives[i].direction.y << m_Primitives[i].direction.z << std::endl;
        std::cout << " Scaling:   " << m_Primitives[i].scale.x << m_Primitives[i].scale.y << m_Primitives[i].scale.z << std::endl;
    }
}
unsigned int SphereUV::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    return store({ color, pos, dir, glm::vec3(width, len, width), 0.0f });
}
unsigned int SphereUV::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//unsigned int SphereUV::FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void SphereUV::changeStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), 0.0f });
}
void SphereUV::changeStartEnd(unsigned int index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (width == NO_FLOAT) width = prim->scale.x;
    glm::vec3 dir = end - pos;
    update(index, { color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//void SphereUV::UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
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
    unsigned int meshV = 16; // Bands
    unsigned int meshU = 32; // Facets
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
unsigned int Planes::addStartNormalLen(glm::vec3 pos, glm::vec3 nml,float rot, float len, glm::vec4 color) {
    // Takes the position of the center of the plane, the direction and rotation of the surface normal, a scale factor and the color
    return store({ color, pos, nml, glm::vec3(len, 1.0f, len), 0.0f });
}
unsigned int Planes::addStartUV(glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color) {
    // Takes the position of the center of the plane, and two vectors that "span" the plane, and the color
    //  Spanning means the plane diagonal will be -spanU,-spanV to spanU,spanV transported to position.
    float angle = -atan2(spanU.z, sqrt(spanU.x * spanU.x + spanU.y * spanU.y));
    return store({ color, pos, glm::cross(spanU,spanV), glm::vec3(glm::length(spanU), 1.0f, glm::length(spanV)), angle });
}
void Planes::changeStartNormalLen(unsigned int index, glm::vec3 pos, glm::vec3 nml, float rot, float len, glm::vec4 color) {
    // Note: When specifying a non-infinite plane by normal and position, the rotation about the normal is not defined,
    //       so rot cannot be calculated from those parameters and is thus explicitly required (defaulting to 0 rads).
    update(index, { color, pos, nml, glm::vec3(len, 1.0f, len), rot });
}
void Planes::changeStartUV(unsigned int index, glm::vec3 pos, glm::vec3 spanU, glm::vec3 spanV, glm::vec4 color) {
    float angle = -atan2(spanU.z, sqrt(spanU.x * spanU.x + spanU.y * spanU.y));
    update(index, { color, pos, glm::cross(spanU,spanV), glm::vec3(glm::length(spanU), 1.0f, glm::length(spanV)), angle });
}
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
unsigned int ViewCones::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    return store({ color, pos, dir, glm::vec3(width, len, width), 0.0f });
}   
unsigned int ViewCones::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//unsigned int ViewCones::FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void ViewCones::changeStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), 0.0f });
}
void ViewCones::changeStartEnd(unsigned int index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (width == NO_FLOAT) width = prim->scale.x;
    glm::vec3 dir = end - pos;
    update(index, { color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
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
    for (unsigned int i = 0; i < m_Primitives.size(); i++) {
        std::cout << "Cone " << i << ":" << std::endl;
        std::cout << " Color:     " << m_Primitives[i].color.r << m_Primitives[i].color.g << m_Primitives[i].color.b << m_Primitives[i].color.a << std::endl;
        std::cout << " Position:  " << m_Primitives[i].position.x << m_Primitives[i].position.y << m_Primitives[i].position.z << std::endl;
        std::cout << " Direction: " << m_Primitives[i].direction.x << m_Primitives[i].direction.y << m_Primitives[i].direction.z << std::endl;
        std::cout << " Scaling:   " << m_Primitives[i].scale.x << m_Primitives[i].scale.y << m_Primitives[i].scale.z << std::endl;
    }
}
unsigned int Cones::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    return store({ color, pos, dir, glm::vec3(width, len, width), 0.0f });
}
unsigned int Cones::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//unsigned int Cones::FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void Cones::changeStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), 0.0f });
}
void Cones::changeStartEnd(unsigned int index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (width == NO_FLOAT) width = prim->scale.x;
    glm::vec3 dir = end - pos;
    update(index, { color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//void Cones::UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void Cones::changeColorLengthWidth(unsigned int index, glm::vec4 color, float length, float width) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    prim->color = color;
    prim->scale = glm::vec3(width, length, width);
    //Update(index, *prim);
}

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
    for (unsigned int i = 0; i < m_Primitives.size(); i++) {
        std::cout << "Cylinder " << i << ":" << std::endl;
        std::cout << " Color:     " << m_Primitives[i].color.r << m_Primitives[i].color.g << m_Primitives[i].color.b << m_Primitives[i].color.a << std::endl;
        std::cout << " Position:  " << m_Primitives[i].position.x << m_Primitives[i].position.y << m_Primitives[i].position.z << std::endl;
        std::cout << " Direction: " << m_Primitives[i].direction.x << m_Primitives[i].direction.y << m_Primitives[i].direction.z << std::endl;
        std::cout << " Scaling:   " << m_Primitives[i].scale.x << m_Primitives[i].scale.y << m_Primitives[i].scale.z << std::endl;
    }
}
unsigned int Cylinders::addStartDirLen(glm::vec3 pos, glm::vec3 dir, float len, float width, glm::vec4 color) {
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, len, width), 0.0f });
}
unsigned int Cylinders::addStartEnd(glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    glm::vec3 dir = end - pos;
    return store({ color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//unsigned int Cylinders::FromStartEleAzi(glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void Cylinders::changeStartDirLen(unsigned int index, glm::vec3 pos, glm::vec3 dir, float length, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    update(index, { color, pos, dir, glm::vec3(width, length, width), 0.0f });
}
void Cylinders::changeStartEnd(unsigned int index, glm::vec3 pos, glm::vec3 end, float width, glm::vec4 color) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (width == NO_FLOAT) width = prim->scale.x;
    glm::vec3 dir = end - pos;
    update(index, { color, pos, glm::normalize(dir), glm::vec3(width, glm::length(dir), width), 0.0f });
}
//void Cylinders::UpdateStartEleAzi(unsigned int index, glm::vec3 pos, float ele, float azi, glm::vec4 color) {}
void Cylinders::changeColorLengthWidth(unsigned int index, glm::vec4 color, float length, float width) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (length == NO_FLOAT) length = prim->scale.y;
    if (width == NO_FLOAT) width = prim->scale.x;
    prim->color = color;
    prim->scale = glm::vec3(width, length, width);
}
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


// ------
//  Dots
// ------
Dots::Dots(Scene* scene) : Primitives(scene, 2000, 1000) {
    m_Primitives.reserve(20000);
    genGeom();
    init();
}
unsigned int Dots::addXYZ(glm::vec3 pos, glm::vec4 color, float size) {
    return store({ color, pos, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(size,size,size), 0.0f }); // col,pos,dir,scale
}
void Dots::changeXYZ(unsigned int index, glm::vec3 pos, glm::vec4 color, float size) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (size == NO_FLOAT) size = prim->scale.x;
    update(index, { color, pos, glm::vec3(0.0f,0.0f,1.0f), glm::vec3(size,size,size), 0.0f });
}
void Dots::changeDot(unsigned int index, glm::vec4 color, float size) {
    Primitive3D* prim = getDetails(index);
    if (color == NO_COLOR) color = prim->color;
    if (size == NO_FLOAT) size = prim->scale.x;
    prim->color = color;
    prim->scale = glm::vec3(size);
}
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
        std::array<Dots::Index, 3> mid;      // Index is simply a typedef unsigned int
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
