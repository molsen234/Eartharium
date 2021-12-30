
#include "Earth.h"

// ----------
//  LocGroup
// ----------

// NOTE: Implement using std::list rather than tightvec !!!
// - I have forgotten why, will have to revisit
// - Oh! So GUI can eventually easily iterate over it without showing the deleted ones.
LocGroup::LocGroup(Earth* e, unsigned int identifier) {
    // LocGroup allows Earth
    earth = e;
    id = identifier; // Check earth for duplicates? Or return unique id instead? !!!
    //locations.reserve(locgrpreserve);
    //std::cout << "LocGroup created: " << identifier << " (Capacity: " << locations.capacity() <<")\n";
    //world = earth->m_world;
    //std::cout << "LocGroup::Locgroup: Created group " << id << " with reserve " << locations.capacity() << ".\n";
}
LocGroup::~LocGroup() {
    clear();
}
void LocGroup::clear() { 
    locations.clear(); 
}
unsigned int LocGroup::size() { 
    return (unsigned int)locations.size();
}
Location* LocGroup::addLocation(double dLat, double dLon, bool rad, float rsky) {
    locations.emplace_back(new Location(earth, dLat, dLon, rad, rsky));
    return locations.back();
}
void LocGroup::removeLocation(Location* loc) {
    locations.remove(loc);
}
void LocGroup::trimLocation() {
    if (locations.size() == 0) {
        std::cout << "LocGroup::trimLocation(): Was asked to delete locations.back(), but locations.size() is already zero.\n";
        return;
    }
    delete locations.back();
    locations.pop_back();
}


// -----------
//  Planetoid
// -----------
Planetoid::Planetoid(Scene* scene, unsigned int meshU, unsigned int meshV, float radius) : m_scene(scene) {
    // Note: Could this be built into a parent class for solar system bodies?
    //       It would work more like Path3D, as each body would probably have different texture.
    //       Actually, OpenGL supports arrays of samplers. Another option is texture array.
    //std::cout << "Planetoid created.";
    m_verts.reserve(((size_t)meshU + 1) * ((size_t)meshV + 1));
    m_tris.reserve((size_t)meshU * (size_t)meshV * sizeof(Tri));
    genGeom(radius);

    vbl = new VertexBufferLayout;
    vbl->Push<float>(3);   // Vertex pos
    vbl->Push<float>(3);   // Vertex normal
    vbl->Push<float>(2);   // Texture coord
    va = new VertexArray;
    vb = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);
    va->AddBuffer(*vb, *vbl, true);
    //std::cout << "Object1 (vb,ib,va): " << vb.GetRenderID() << "," << ib.GetRenderID() << "," << va.GetRenderID() << std::endl;
    //std::string shadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\planetoid.shader";
    //shdr = new Shader(shadersrc);
    shdr = m_scene->m_app->getShaderLib()->getShader(PLANETOID_SHADER);
    const std::string texfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_sun.png";
    tex = new Texture(texfile, GL_TEXTURE5);
    int at1 = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &at1);
    //std::cout << "Planetoid::Planetoid(): Created TextureSlot " << tex->GetTextureSlot() << ", RenderID " << tex->GetRenderID() << ".\n";
}
Planetoid::~Planetoid() {
    //std::cout << "Planetoid destroyed.";
    delete tex;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
}
void Planetoid::Update(glm::vec3 pos) {
    position = pos;
}
void Planetoid::Draw() {
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
    shdr->SetUniform3f("position", position.x, position.y, position.z);
    shdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
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
void Planetoid::genGeom(float radius) {
    float rLat, rLon;
    unsigned int meshV = 16; // Bands
    unsigned int meshU = 32; // Facets

    for (unsigned int v = 0; v <= meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
        rLat = (pif * v / meshV) - pi2f;
        for (unsigned int u = 0; u <= meshU; u++) {
            rLon = (tauf * u / meshU) - pif;
            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);     // position of vertex
            glm::vec3 nml = glm::normalize(loc);  // Insolation normal, always NS (or G8?)
            m_verts.push_back({ loc, nml, glm::vec2((float)u / (float)meshU, (float)v / (float)meshV) });
            if (u < meshU && v < meshV) {
                m_tris.push_back({ (meshU + 1) * v + u , (meshU + 1) * (v + 1) + u, (meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (meshU + 1) * (v + 1) + 1 + u, (meshU + 1) * v + u + 1, (meshU + 1) * v + u });
            }
        }
    }
}


// ---------
//  SubSolar
// ---------
SubSolar::SubSolar(Scene* scene, unsigned int meshU, unsigned int meshV, float radius) : m_scene(scene) {
    // Note: Could this be built into a parent class for solar system bodies?
    //       It would work more like Path3D, as each body would probably have different texture.
    //       Actually, OpenGL supports arrays of samplers. Another option is texture array.
    //std::cout << "Planetoid created.";
    m_verts.reserve(((size_t)meshU + 1) * ((size_t)meshV + 1));
    m_tris.reserve((size_t)meshU * (size_t)meshV * sizeof(Tri));
    genGeom(radius);

    vbl = new VertexBufferLayout;
    vbl->Push<float>(3);   // Vertex pos
    vbl->Push<float>(3);   // Vertex normal
    vbl->Push<float>(2);   // Texture coord
    va = new VertexArray;
    vb = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);
    va->AddBuffer(*vb, *vbl, true);
    //std::cout << "Object1 (vb,ib,va): " << vb.GetRenderID() << "," << ib.GetRenderID() << "," << va.GetRenderID() << std::endl;
    //std::string shadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\planetoid.shader";
    //shdr = new Shader(shadersrc);
    shdr = m_scene->m_app->getShaderLib()->getShader(PLANETOID_SHADER);
    const std::string texfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_sun.png";
    tex = new Texture(texfile, GL_TEXTURE5);
    int at1 = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &at1);
    //std::cout << "SubSolar::SubSolar(): Created TextureSlot " << tex->GetTextureSlot() << ", RenderID " << tex->GetRenderID() << ".\n";
}
SubSolar::~SubSolar() {
    //std::cout << "SubSolar destroyed.";
    delete tex;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
}
void SubSolar::Update(glm::vec3 pos) {
    position = pos;
}
void SubSolar::Draw() {
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
    shdr->SetUniform3f("position", position.x, position.y, position.z);
    shdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
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
    shdr->Unbind();
    return;
}
glm::vec3 SubSolar::getLoc3D_NS(float lat, float lon, float height, float radius) {
    float m_Radius = 1.0f;
    float w = cos(lat) * (radius + height);
    float x = cos(lon) * w;
    float y = sin(lon) * w;
    float z = sin(lat) * (radius + height);
    return glm::vec3(x, y, z);
}
void SubSolar::genGeom(float radius) {
    float rLat, rLon;
    unsigned int meshV = 16; // Bands
    unsigned int meshU = 32; // Facets

    for (unsigned int v = 0; v <= meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
        rLat = (pif * v / meshV) - pi2f;
        for (unsigned int u = 0; u <= meshU; u++) {
            rLon = (tauf * u / meshU) - pif;
            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);     // position of vertex
            glm::vec3 nml = glm::normalize(loc);  // Insolation normal, always NS (or G8?)
            m_verts.push_back({ loc, nml, glm::vec2((float)u / (float)meshU, (float)v / (float)meshV) });
            if (u < meshU && v < meshV) {
                m_tris.push_back({ (meshU + 1) * v + u , (meshU + 1) * (v + 1) + u, (meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (meshU + 1) * (v + 1) + 1 + u, (meshU + 1) * v + u + 1, (meshU + 1) * v + u });
            }
        }
    }
}


// ---------
//  SubLunar
// ---------
SubLunar::SubLunar(Scene* scene, unsigned int meshU, unsigned int meshV, float radius) : m_scene(scene) {
    // Note: Could this be built into a parent class for solar system bodies?
    //       It would work more like Path3D, as each body would probably have different texture.
    //       Actually, OpenGL supports arrays of samplers. Another option is texture array.
    //std::cout << "Planetoid created.";
    m_verts.reserve(((size_t)meshU + 1) * ((size_t)meshV + 1));
    m_tris.reserve((size_t)meshU * (size_t)meshV * sizeof(Tri));
    genGeom(radius);

    vbl = new VertexBufferLayout;
    vbl->Push<float>(3);   // Vertex pos
    vbl->Push<float>(3);   // Vertex normal
    vbl->Push<float>(2);   // Texture coord
    va = new VertexArray;
    vb = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);
    va->AddBuffer(*vb, *vbl, true);
    //std::cout << "Object1 (vb,ib,va): " << vb.GetRenderID() << "," << ib.GetRenderID() << "," << va.GetRenderID() << std::endl;
    //std::string shadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\planetoid.shader";
    //shdr = new Shader(shadersrc);
    shdr = m_scene->m_app->getShaderLib()->getShader(PLANETOID_SHADER);
    const std::string texfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_moon.png";
    tex = new Texture(texfile, GL_TEXTURE6);
    int at1 = 0;
    glGetIntegerv(GL_ACTIVE_TEXTURE, &at1);
    //std::cout << "SubLunar::SubLunar(): Created TextureSlot " << tex->GetTextureSlot() << ", RenderID " << tex->GetRenderID() << ".\n";
}
SubLunar::~SubLunar() {
    //std::cout << "SubLunar destroyed.";
    delete tex;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
}
void SubLunar::Update(glm::vec3 pos) {
    position = pos;
}
void SubLunar::Draw() {
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
    shdr->SetUniform3f("position", position.x, position.y, position.z);
    shdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
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
    shdr->Unbind();
    return;
}
glm::vec3 SubLunar::getLoc3D_NS(float rLat, float rLon, float height, float radius) { // For generating the mesh
    float m_Radius = 1.0f;
    float w = cos(rLat) * (radius + height);
    float x = cos(rLon) * w;
    float y = sin(rLon) * w;
    float z = sin(rLat) * (radius + height);
    return glm::vec3(x, y, z);
}
void SubLunar::genGeom(float radius) {
    float rLat, rLon;
    unsigned int meshV = 16; // Bands
    unsigned int meshU = 32; // Facets

    for (unsigned int v = 0; v <= meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
        rLat = (pif * v / meshV) - pi2f;
        for (unsigned int u = 0; u <= meshU; u++) {
            rLon = (tauf * u / meshU) - pif;
            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);     // position of vertex
            glm::vec3 nml = glm::normalize(loc);  // Insolation normal, always NS (or G8?)
            m_verts.push_back({ loc, nml, glm::vec2((float)u / (float)meshU, (float)v / (float)meshV) });
            if (u < meshU && v < meshV) {
                m_tris.push_back({ (meshU + 1) * v + u , (meshU + 1) * (v + 1) + u, (meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (meshU + 1) * (v + 1) + 1 + u, (meshU + 1) * v + u + 1, (meshU + 1) * v + u });
            }
        }
    }
}


// -------
//  Earth
// -------
Earth::Earth(Scene* scene, std::string mode, unsigned int meshU, unsigned int meshV)
    : m_meshU(meshU), m_meshV(meshV), m_Mode(mode), m_scene(scene) {
    //std::cout << "Earth{" << this << "}(" << scene << "," << mode << "," << meshU << "," << meshV << ") created!\n";
    m_dots = m_scene->getDotsOb();
    m_arrows = m_scene->getArrowsOb();
    m_cylinders = m_scene->getCylindersOb();
    m_verts.reserve(((size_t)m_meshU + 1) * ((size_t)m_meshV + 1));
    m_tris.reserve((size_t)(m_meshU * (size_t)m_meshV * (size_t)sizeof(Tri)));
    m_dotcache.reserve(1000);  // Not actually removed when dots are deleted, just set to index maxuint
    m_arrowcache.reserve(100);
    m_polycache.reserve(100);
    locgroups.reserve(10);
    current_earth = mode;
    genGeom();
    //m_scene->GetCelestialOb()->enablePlanet(SUN);     // No longer needed? !!!
    updateSun();
    CalcMoon();
    vbl = new VertexBufferLayout;
    vbl->Push<float>(3);   // Vertex pos
    vbl->Push<float>(3);   // Vertex normal NS (for insolation only)
    vbl->Push<float>(3);   // Vertex normal actual
    vbl->Push<float>(2);   // Texture coord
    vbl->Push<float>(4);   // Color (for overlay)
    va = new VertexArray;
    vb = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(EarthV));
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);
    va->AddBuffer(*vb, *vbl, true);
    //std::cout << "Object1 (vb,ib,va): " << vb.GetRenderID() << "," << ib.GetRenderID() << "," << va.GetRenderID() << std::endl;
    //std::string shadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\earth.shader";
    //shdr = new Shader(shadersrc);
    shdr = m_scene->m_app->getShaderLib()->getShader(EARTH_SHADER);
    //std::string smshadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\shadowearth.shader";
    //smshdr = new Shader(smshadersrc);
    smshdr = m_scene->m_app->getShaderLib()->getShader(EARTH_SHADOW_MAP_SHADER);
    //std::string sbshadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\sdwboxearth.shader";
    //sbshdr = new Shader(sbshadersrc);
    sbshdr = m_scene->m_app->getShaderLib()->getShader(EARTH_SHADOW_BOX_SHADER);
    // NOTE: Textures should be pre-flipped in Y, as SOIL2 soils itself when trying to flip large images
    const std::string daytexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\large map 8192x4096.png";
    daytex = new Texture(daytexfile, GL_TEXTURE3);
    //const std::string nighttexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\BlackMarble_2012_8192x4096.png";
    const std::string nighttexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\BlackMarble_2012_8192x4096_ice.png";
    nighttex = new Texture(nighttexfile, GL_TEXTURE4);
    //const std::string normaltexfile = "C:\\Coding\\openGL\\learnopengl-com\\First Lesson\\normal-map2.png";
    //normaltex = new Texture(normaltexfile, GL_TEXTURE4);
}
Earth::~Earth() {
    std::cout << "Earth destroyed.\n";
    //  m_world->GetCelestialOb()->disablePlanet(SUN);
    if (!locgroups.empty()) {
        for (auto& lg : locgroups) {
            delete lg;
        }
    }
    if (!m_polycache.empty()) {
        for (auto& p : m_polycache) {
            m_scene->deletePolyCurve(p.path);
        }
        m_polycache.clear();
    }
    //delete m_sunterm2;
    //delete m_sunterm1;
    delete nighttex;
    delete daytex;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
    if (m_sunob != nullptr) delete m_sunob;
}
void Earth::setFade(float alpha) {
    m_alpha = alpha;
}
void Earth::updateType(std::string type) {
    m_Mode = type;
    paramdirty = true; // trigger updateMorph() during Update()
}
void Earth::Update() {
    // Collect updates
    if (!arcticsoverlay && m_arctics != BLACK) { // just switched off, so update
        m_arctics = BLACK;
        paramdirty = true;
    }
    if (!tropicsoverlay && m_tropics != BLACK) {
        m_tropics = BLACK;
        paramdirty = true;
    }
    if (tropicsoverlay && m_tropics != tropics) { // Just switched on, so update
        m_tropics = tropics;
        paramdirty = true;
    }
    if (arcticsoverlay && m_arctics != arctics) {
        m_arctics = arctics;
        paramdirty = true;
    }
    if (m_param != param) {  // pick up param from gui
        m_param = param;
        paramdirty = true;
    }
    if (m_eccen != eccen) {  // pick up param from gui
        m_eccen = eccen;
        paramdirty = true;
    }
    if (m_Mode != current_earth) {  // Move current_earth to gui to be consistent with param above !!!
        m_Mode = current_earth;
        paramdirty = true;
    }
    if (m_flatsunheight != flatsunheight) {
        //std::cout << "Earth::Update() m_flatsunheight: " << m_flatsunheight << " <- flatsunheight: " << flatsunheight << "\n";
        m_flatsunheight = flatsunheight;
        //mydebug = true;
        flatsundirty = true;
    }
    if (m_JD != m_scene->m_astro->getJD()) {
        m_JD = m_scene->m_astro->getJD();
        timedirty = true;
    }
    // if timedirty
    if (timedirty) {
        CalcMoon();
    }
    // if morphdirty
    if (paramdirty) {
        updateMorph();
        // if morphdirty - paths that are not time sensitive
        for (auto& p : m_polycache) {
            if (p.type == NONE) continue;
            if (p.type == LATITUDE || p.type == GRIDLAT || p.type == EQUATOR || p.type == ARCTIC || p.type == TROPIC)
                updateLatitudeCurve(p);
            if (p.type == LONGITUDE || p.type == GRIDLON || p.type == PRIME_MERIDIAN)
                updateLongitudeCurve(p);
            if (p.type == FLATARC) updateFlatArc(p);
        }
    }
    if (timedirty || paramdirty || flatsundirty) {
        updateSun();
        if (m_sunob != nullptr) updateSubsolarPoint();
        if (m_moonob != nullptr) updateSublunarPoint();
        for (unsigned int i = 0; i < m_dotcache.size(); i++) {
            if (m_dotcache[i].index != maxuint) {
                glm::vec3 pos = getLoc3D(m_dotcache[i].lat, m_dotcache[i].lon, m_dotcache[i].height);
                m_dots->changeXYZ(m_dotcache[i].index, pos, m_dotcache[i].color, m_dotcache[i].size);
            }
        }
        if (m_sunpole != maxuint) updateSubsolarPole();
        if (m_suncone != maxuint) updateSubsolarCone_NS();
        for (auto& p : m_polycache2) {  // Composite paths/arcs
            if (p.type == NONE) continue;
            if (p.type == MOONTERMINATOR) updateTerminatorTrueMoon(p);
            if (p.type == SUNTERMINATOR) updateTerminatorTrueSun(p);
            if (p.type == GREATARC) updateGreatArc(p);
            if (p.type == LERPARC) updateLerpArc(p);
        }
        // Collect in a cache with other celestial paths
        if (suncurve != nullptr && sunpath != nullptr) updateSubsolarPath();
    }
    if (flatsundirty || paramdirty || timedirty) {  // Essentially if ANYTHING has changed, update locations.
        for (auto& lg : locgroups) {
            for (auto& l : lg->locations) {
                l->Update(timedirty, paramdirty, flatsundirty);
            }
        }
    }
    if (timedirty) {
        // Update arrows
        for (auto& a : m_arrowcache) {
            if (a.type == TRUESUN3D) updateArrow3DTrueSun(a);
            //if (a.type == FLATSUN3D) updateArrow3DFlatSun(a);
            //if (a.type == TRUEMOON3D) updateArrow3DTrueMoon(a);
            //if (a.type == TRUEPLANET3D) updateArrow3DTruePlanet(a);
            //if (a.type == RADEC3D) updateArrow3DRADec(a);
        }

    }
    timedirty = false;
    paramdirty = false;
    flatsundirty = false;
}
void Earth::updateMorph() {
    unsigned int i = 0;
    float lat, lon;
    for (unsigned int v = 0; v <= m_meshV; v++) {
        lat = (pif * v / m_meshV) - pi2f;
        for (unsigned int u = 0; u <= m_meshU; u++) {
            lon = (tauf * u / m_meshU) - pif;
            m_verts[i].position = getLoc3D(lat, lon, 0.0f);
            m_verts[i].normal_g = getNml3D(lat, lon, 0.0f);
            // NOTE: This is only partial separation between geometry and insolation, maybe a more general approach will be good
            //       e.g. to show how wrong it looks to have flat Earth insolation on a globe, etc.
            if (do_eccentriclight) m_verts[i].normal_i = getNml3D_EE(lat, lon, 0.0f);
            if (tropicsoverlay && abs(lat) < 24.0*deg2rad) m_verts[i].color = tropics;
            else if (arcticsoverlay && abs(lat) > (90.0 - 23.4) * deg2rad) m_verts[i].color = arctics;
            else m_verts[i].color = BLACK;
            i++;
        }
    }
    vb->LoadData(&m_verts[0], (unsigned int)m_verts.size() * sizeof(EarthV));
    return;
}
void Earth::Draw() {
    // Shader setup
    if (shadows == SHADOW_MAP) { // m_world.shadows
        smshdr->Bind();
        smshdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
        smshdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
        smshdr->SetUniformMatrix4f("lightSpaceMatrix", m_scene->getShadowmapOb()->lightSpaceMatrix);
        if (w_sinsol) smshdr->SetUniform3f("sunDir", SunLightDir.x, SunLightDir.y, SunLightDir.z);
        if (!w_sinsol) smshdr->SetUniform3f("sunDir", 0.0f, 0.0f, 0.0f);
        if (w_linsol) smshdr->SetUniform3f("moonDir", MoonLightDir.x, MoonLightDir.y, MoonLightDir.z);
        if (!w_linsol) smshdr->SetUniform3f("moonDir", 0.0f, 0.0f, 0.0f);
        if (w_refract) smshdr->SetUniform1f("refraction", 1.0f);
        else smshdr->SetUniform1f("refraction", 0.0f);
        smshdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
        smshdr->SetUniform1i("dayTexture", daytex->GetTextureSlot());
        smshdr->SetUniform1i("nightTexture", nighttex->GetTextureSlot());
        smshdr->SetUniform1i("depthTexture", m_scene->getShadowmapOb()->depthmap);
        smshdr->SetUniform1f("alpha", m_alpha);
    }
    else if (shadows == SHADOW_BOX) {
        sbshdr->Bind();
        if (w_sinsol) sbshdr->SetUniform3f("sunDir", SunLightDir.x, SunLightDir.y, SunLightDir.z);
        if (!w_sinsol) sbshdr->SetUniform3f("sunDir", 0.0f, 0.0f, 0.0f);
        if (w_linsol) sbshdr->SetUniform3f("moonDir", MoonLightDir.x, MoonLightDir.y, MoonLightDir.z);
        if (!w_linsol) sbshdr->SetUniform3f("moonDir", 0.0f, 0.0f, 0.0f);
        if (w_refract) sbshdr->SetUniform1f("refraction", 1.0f);
        else sbshdr->SetUniform1f("refraction", 0.0f);
        sbshdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
        sbshdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
        sbshdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());

        sbshdr->SetUniform1f("far_plane", m_scene->getShadowboxOb()->far);
        glm::vec3 lightpos = getSubsolarXYZ();
        sbshdr->SetUniform3f("lightPos", lightpos.x, lightpos.y, lightpos.z);
        sbshdr->SetUniform1i("dayTexture", daytex->GetTextureSlot());
        sbshdr->SetUniform1i("nightTexture", nighttex->GetTextureSlot());
        sbshdr->SetUniform1i("depthTexture", m_scene->getShadowboxOb()->depthCubemap);
        sbshdr->SetUniform1f("alpha", m_alpha);

    }
    else {
        shdr->Bind();
        //shdr->SetUniformMatrix4f("view", m_scene->w_camera->GetViewMat());
        //shdr->SetUniformMatrix4f("projection", m_scene->w_camera->GetProjMat());
        glm::mat4 pv = m_scene->w_camera->getProjMat() * m_scene->w_camera->getViewMat();
        shdr->SetUniformMatrix4f("projview", pv);
        if (w_sinsol) shdr->SetUniform3f("sunDir", SunLightDir.x, SunLightDir.y, SunLightDir.z);
        if (!w_sinsol) shdr->SetUniform3f("sunDir", 0.0f, 0.0f, 0.0f);
        if (w_linsol) shdr->SetUniform3f("moonDir", MoonLightDir.x, MoonLightDir.y, MoonLightDir.z);
        if (!w_linsol) shdr->SetUniform3f("moonDir", 0.0f, 0.0f, 0.0f);
        if (w_refract) shdr->SetUniform1f("refraction", 1.0f);
        else shdr->SetUniform1f("refraction", 0.0f);
        shdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
        shdr->SetUniform1i("dayTexture", daytex->GetTextureSlot());
        shdr->SetUniform1i("nightTexture", nighttex->GetTextureSlot());
        shdr->SetUniform1f("alpha", m_alpha);
        shdr->SetUniform1f("twilight", w_twilight ? 1.0f : 0.0f);
    }
    va->Bind();
    ib->Bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glDisable(GL_CULL_FACE); // It is irritating that Earth disappears when seen from back-/in-side
    glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0);
    glEnable(GL_CULL_FACE);
    // Cleanup
    va->Unbind();
    ib->Unbind();
    shdr->Unbind();
    for (auto& po : m_polycache) {
        if (po.type != NONE) po.path->draw();
    }
    for (auto& p : m_polycache2) {
        if (p.type != NONE) {
            p.path->draw();
            p.path2->draw();
        }
    }
    if (m_sunob != nullptr) m_sunob->Draw();
    if (m_moonob != nullptr) m_moonob->Draw();
    for (auto& lg : locgroups) {
        for (auto l : lg->locations) {
            l->Draw();
        }
    }
    if (suncurve != nullptr && sunpath != nullptr) suncurve->draw();
    if (tissotcurve != nullptr) {
        tissotcurve->draw();
    }
    if (tissotcurve2 != nullptr) {
        tissotcurve2->draw();
    }
    return;
}
unsigned int Earth::addLocGroup() {
    // Could also receive a LocGroup identifier from caller, but would then have to use std::map instead of std::vector
    // So iterating could be less convenient. Learn more about iterating maps and revisit this. !!!
    unsigned int index = (unsigned int)locgroups.size();
    locgroups.emplace_back(new LocGroup(this,index));
    return index;
}
glm::vec3 Earth::getLoc3D(double rLat, double rLon, double height) {
    /// <summary>
    /// Takes Earth latitude and longitude in radians to glm::vec3 Cartesian coordinate in world space
    /// </summary>
    /// <param name="rLat">Latitude given in -pi/2 to pi/2 radians</param>
    /// <param name="rLon">Longitude given in -pi to pi radians from Greenwich meridian (east of south)</param>
    /// <param name="height">Optional height above geoid surface in km</param>
    /// <returns>Cartesian coordinates glm::vec3 in world space units for currently active geoid geometry</returns>
    if (m_Mode.length() == 2) return getLoc3D_XX(m_Mode, rLat, rLon, height);
    if (m_Mode.length() != 4) {
        std::cout << "Earth::getLoc3D(): Invalid mode: " << m_Mode << " (must be 2 or 4 characters)" << '\n';
        return glm::vec3(0.0f);
    }
    // Mode is 4 long so morph
    std::string sMode = m_Mode.substr(0, 2);
    std::string eMode = m_Mode.substr(2, 2);
    glm::vec3 sPos = getLoc3D_XX(sMode, rLat, rLon, height);
    glm::vec3 ePos = getLoc3D_XX(eMode, rLat, rLon, height);
    //glm::vec3 dPos = ePos - sPos;
    //return m_Param * dPos + sPos;
    return m_param * ePos + (1 - m_param) * sPos;
}
glm::vec3 Earth::getNml3D(double rLat, double rLon, double height) {
    if (m_Mode.length() == 2) return getNml3D_XX(m_Mode, rLat, rLon, height);
    if (m_Mode.length() != 4) {
        std::cout << "Earth::getNml3D(): Invalid mode: " << m_Mode << " (must be 2 or 4 characters)" << '\n';
        return glm::vec3(0.0);
    }
    // Mode is 4 long so morph
    std::string sMode = m_Mode.substr(0, 2);
    std::string eMode = m_Mode.substr(2, 2);
    glm::vec3 sPos = getNml3D_XX(sMode, rLat, rLon, height);
    glm::vec3 ePos = getNml3D_XX(eMode, rLat, rLon, height);
    return glm::normalize(m_param * ePos + (1 - m_param) * sPos);
}
glm::vec3 Earth::getNorth3D(double rLat, double rLon, double height) {
    // NOTE: If rLat = pi/2 or -pi/2 (at poles) north is not defined!!
    glm::vec3 l1 = getLoc3D(rLat + NE_SMEAR, rLon);
    glm::vec3 l2 = getLoc3D(rLat - NE_SMEAR, rLon);
    return glm::normalize(l1 - l2);
}
glm::vec3 Earth::getEast3D(double rLat, double rLon, double height) {
    // NOTE: If rLat = pi/2 or -pi/2 (at poles) east is not defined!!
    glm::vec3 l1 = getLoc3D(rLat, rLon + NE_SMEAR);
    glm::vec3 l2 = getLoc3D(rLat, rLon - NE_SMEAR);
    return glm::normalize(l1 - l2);
}
glm::vec3 Earth::getLoc3D_XX(std::string mode, double rLat, double rLon, double height) {
    if (mode == "NS") return getLoc3D_NS(rLat, rLon, height);
    if (mode == "AE") return getLoc3D_AE(rLat, rLon, height);
    if (mode == "ER") return getLoc3D_ER(rLat, rLon, height);
    if (mode == "RC") return getLoc3D_RC(rLat, rLon, height);
    if (mode == "E8") return getLoc3D_E8(rLat, rLon, height);
    if (mode == "EX") return getLoc3D_EX(rLat, rLon, height);
    if (mode == "EE") return getLoc3D_EE(rLat, rLon, height);
    return glm::vec3(0.0f);
}
glm::vec3 Earth::getNml3D_XX(std::string mode, double rLat, double rLon, double height) {
    if (mode == "NS") return getNml3D_NS(rLat, rLon, height);
    if (mode == "AE") return getNml3D_AE(rLat, rLon, height);
    if (mode == "ER") return getNml3D_ER(rLat, rLon, height);
    if (mode == "RC") return getNml3D_RC(rLat, rLon, height);
    if (mode == "E8") return getNml3D_E8(rLat, rLon, height);
    if (mode == "EX") return getNml3D_EX(rLat, rLon, height);
    if (mode == "EE") return getNml3D_EE(rLat, rLon, height);
    return glm::vec3(0.0, 0.0, 0.0);
}
glm::vec3 Earth::getLoc3D_NS(double rLat, double rLon, double height) {
    float w = (float)(cos(rLat) * (m_Radius + height));
    float x = (float)(cos(rLon) * w);
    float y = (float)(sin(rLon) * w);
    float z = (float)(sin(rLat) * (m_Radius + height));
    return glm::vec3(x, y, z);
}
glm::vec3 Earth::getNml3D_NS(double rLat, double rLon, double height) {
    return glm::normalize(getLoc3D_NS(rLat, rLon, height));
}
glm::vec3 Earth::getLoc3D_AE(double rLat, double rLon, double height) {
    double w = (pi2 - rLat) * m_Radius / pi2;
    float x = (float)(cos(rLon) * w);
    float y = (float)(sin(rLon) * w);
    return glm::vec3(x, y, height);
}
glm::vec3 Earth::getNml3D_AE(double rLat, double rLon, double height) {
    return glm::vec3(0.0f, 0.0f, 1.0f);
}
glm::vec3 Earth::getLoc3D_ER(double rLat, double rLon, double height) {
    return glm::vec3(height, rLon * m_Radius, rLat * m_Radius);
}
glm::vec3 Earth::getNml3D_ER(double rLat, double rLon, double height) {
    return glm::vec3(1.0f, 0.0f, 0.0f);
}
glm::vec3 Earth::getLoc3D_RC(double rLat, double rLon, double height) {
    double w = (m_Radius + height);
    return glm::vec3(cos(rLon) * w, sin(rLon) * w, rLat * m_Radius);
}
glm::vec3 Earth::getNml3D_RC(double rLat, double rLon, double height) {
    double w = (m_Radius + height);
    return glm::vec3(cos(rLon) * w, sin(rLon) * w, 0.0f);
}
glm::vec3 Earth::getLoc3D_EE(double rLat, double rLon, double height) {
    // WGS-84 ellipsoid
    // Earth Reference Ellipsoid as described in WGS-84
    double a = 6378137.0f;       // Semimajor axis
    double f = 298.257223563f;   // inverse flattening
    //double b = a - a / f;      // Semiminor axis
    double b = sqrt(1.0 - m_eccen * m_eccen);
    height *= a;                 // passed in Earth radii, we calculate in meters.
    glm::vec3 loc = glm::vec3(0.0f);
    loc.x = (float)(cos(rLat) * cos(rLon));
    loc.y = (float)(cos(rLat) * sin(rLon));
    loc.z = (float)(b * sin(rLat));
    //loc *= 1 / a;
    return loc;
}
glm::vec3 Earth::getNml3D_EE(double rLat, double rLon, double height) {
    // Earth Reference Ellipsoid as described in WGS-84
    // - General Normal of Ellipsoid from Latitude & Longitude
    //   (ref. https://math.stackexchange.com/questions/2974280/normal-vector-to-ellisoid-surface)
    //   (lat,lon) = (cos(lat)*cos(lon)/a, cos(lat)*sin(lon)/b, sin(lat)*c) 
    double a = 6378137.0f;     // Semimajor axis
    //double f = 298.257223563f; // inverse flattening
    //double b = a - a / f;      // Semiminor axis
    double b = a * sqrt(1.0 - m_eccen * m_eccen);
    //glm::vec3 nml;
    //nml.x = cos(rLat) * cos(rLon) / a;
    //nml.y = cos(rLat) * sin(rLon) / a;
    //nml.z = sin(rLat) * b;
    glm::vec3 nml = getLoc3D_EE(rLat, rLon, 0.0f);
    nml.x /= (float)(a * a);
    nml.y /= (float)(a * a);
    nml.z /= (float)(b * b);
    return glm::normalize(nml);
}
glm::vec3 Earth::getLoc3D_EX(double rLat, double rLon, double height) {
    // General ellipsoid - Takes angles in radians and height in METERS
    //double a = 6378137.0f;     // Semimajor axis
    
    ////double f = 298.257223563f; // inverse flattening
    ////double b = a - a / f;      // Semiminor axis
    //double b = a * sqrt(1.0 - m_eccen * m_eccen);
    //double N = a * a / sqrt(a * a * cos(rLat) * cos(rLat) + b * b * sin(rLat) * sin(rLat));
    //height *= a;              // passed in Earth radii, we calculate in meters.
    //glm::vec3 loc = glm::vec3(0.0f);
    //loc.x = (float)((N + height) * cos(rLat) * cos(rLon));
    //loc.y = (float)((N + height) * cos(rLat) * sin(rLon));
    //loc.z = (float)(((((b * b) / (a * a)) * N) + height) * sin(rLat));
    //loc *= 1 / a;             // Really loc *= self.NSscale/a;
    //return loc;

    // From: https://gssc.esa.int/navipedia/index.php/Ellipsoidal_and_Cartesian_Coordinates_Conversion
    double a = 6378137.0f;     // Semimajor axis
    double e2 = m_eccen * m_eccen;
    double N = a / sqrt(1.0 - e2 * sin(rLat) * sin(rLat));  //  prime vertical radius of curvature at latitude

    glm::vec3 loc = glm::vec3(0.0f);
    loc.x = (float)((N + height) * cos(rLat) * cos(rLon));
    loc.y = (float)((N + height) * cos(rLat) * sin(rLon));
    loc.z = (float)(((1.0 - e2) * N + height) * sin(rLat));
    loc *= 1.0 / a;             // Really loc *= self.NSscale/a;     // Scale to graphics frame scale (currently Earth radii)
    return loc;

}
glm::vec3 Earth::getNml3D_EX(double rLat, double rLon, double height) {
    // Earth Reference Ellipsoid as described in WGS-84
    // - General Normal of Ellipsoid from Latitude & Longitude
    //   (ref. https://math.stackexchange.com/questions/2974280/normal-vector-to-ellisoid-surface)
    //   (lat,lon) = (cos(lat)*cos(lon)/a, cos(lat)*sin(lon)/b, sin(lat)*c) 
    double a = 6378137.0f;     // Semimajor axis
    //double f = 298.257223563f; // inverse flattening
    //double b = a - a / f;      // Semiminor axis
    double b = a * sqrt(1.0 - m_eccen * m_eccen);
    //glm::vec3 nml;
    //nml.x = cos(rLat) * cos(rLon) / a;
    //nml.y = cos(rLat) * sin(rLon) / a;
    //nml.z = sin(rLat) * b;
    glm::vec3 nml = getLoc3D_EX(rLat, rLon, 0.0f);
    nml.x /= (float)(a * a);
    nml.y /= (float)(a * a);
    nml.z /= (float)(b * b);
    return glm::normalize(nml);
}
glm::vec3 Earth::getLoc3D_E8(double rLat, double rLon, double height) {
    // WGS-84 ellipsoid
    // Earth Reference Ellipsoid as described in WGS-84
    double a = 6378137.0f;     // Semimajor axis
    double f = 298.257223563f; // inverse flattening
    double b = a - a / f;      // Semiminor axis
    double N = a * a / sqrt(a * a * cos(rLat) * cos(rLat) + b * b * sin(rLat) * sin(rLat));
    height *= a;              // passed in Earth radii, we calculate in meters.
    glm::vec3 loc = glm::vec3(0.0f);
    loc.x = (float)((N + height) * cos(rLat) * cos(rLon));
    loc.y = (float)((N + height) * cos(rLat) * sin(rLon));
    loc.z = (float)(((((b * b) / (a * a)) * N) + height) * sin(rLat));
    loc *= 1 / a;             // Really loc *= self.NSscale/a;
    return loc;
}
glm::vec3 Earth::getNml3D_E8(double rLat, double rLon, double height) {
    // Earth Reference Ellipsoid as described in WGS-84
    // - General Normal of Ellipsoid from Latitude & Longitude
    //   (ref. https://math.stackexchange.com/questions/2974280/normal-vector-to-ellisoid-surface)
    //   (lat,lon) = (cos(lat)*cos(lon)/a, cos(lat)*sin(lon)/b, sin(lat)*c) 
    double a = 6378137.0f;     // Semimajor axis
    double f = 298.257223563f; // inverse flattening
    double b = a - a / f;      // Semiminor axis
    //glm::vec3 nml;
    //nml.x = cos(rLat) * cos(rLon) / a;
    //nml.y = cos(rLat) * sin(rLon) / a;
    //nml.z = sin(rLat) * b;
    glm::vec3 nml = getLoc3D_E8(rLat, rLon, 0.0f);
    nml.x /= (float)(a * a);
    nml.y /= (float)(a * a);
    nml.z /= (float)(b * b);
    return glm::normalize(nml);
}
glm::vec3 Earth::getLoc3D_E7(double rLat, double rLon, double height) {
    // WGS-72 ellipsoid
    height *= 1000;            // passed in km, we calculate in meters.
    // Earth Reference Ellipsoid as described in WGS-72
    double a = 6378135.0f;      // Semimajor axis
    double f = 298.26f;         // inverse flattening
    double b = a - a / f;       // Semiminor axis
    double N = a * a / sqrt(a * a * cos(rLat) * cos(rLat) + b * b * sin(rLat) * sin(rLat));
    glm::vec3 loc = glm::vec3(0.0f);
    loc.x = (float)((N + height) * cos(rLat) * cos(rLon));
    loc.y = (float)((N + height) * cos(rLat) * sin(rLon));
    loc.z = (float)(((((b * b) / (a * a)) * N) + height) * sin(rLat));
    loc *= 1 / a;              // Really loc *= self.NSscale/a;
    return loc;
}
glm::vec3 Earth::getNml3D_E7(double rLat, double rLon, double height) {
    // Earth Reference Ellipsoid as described in WGS-72
    // - General Normal of Ellipsoid from Latitude & Longitude
    //   (ref. https://math.stackexchange.com/questions/2974280/normal-vector-to-ellisoid-surface)
    //   (lat,lon) = (cos(lat)*cos(lon)/a, cos(lat)*sin(lon)/b, sin(lat)*c) 
    double a = 6378135.0f;     // Semimajor axis
    double f = 298.26f;        // inverse flattening
    double b = a - a / f;      // Semiminor axis
    //glm::vec3 nml;
    //nml.x = cos(lat) * cos(lon) / a;
    //nml.y = cos(lat) * sin(lon) / a;
    //nml.z = sin(lat) * b;
    glm::vec3 nml = getLoc3D_E7(rLat, rLon, 0.0f);
    nml.x /= (float)(a * a);
    nml.y /= (float)(a * a);
    nml.z /= (float)(b * b);
    return glm::normalize(nml);
}
LLH Earth::calcRADec2LatLon(LLH radec) {
    if (radec.lon < pi) radec.lon = -radec.lon;
    else radec.lon = tau - radec.lon;                          // subsolar.lon is now -pi to pi east of south
    return radec;
}
glm::vec3 Earth::calcRADec2Dir(LLH radec) {
    //LLH mysun = m_world->GetCelestialOb()->getDecGHA(SUN, jd); // subsolar.lon is west of south, 0 to tau
    radec = calcRADec2LatLon(radec);
    return getLoc3D(radec.lat, radec.lon, 0.0);
}
LLH Earth::getXYZtoLLH_NS(glm::vec3 pos) {
    // NOTE: Gives height above Center of Earth, not height above Surface!! (i.e. geocentric, not topocentric)
    //       To get surface height depends on geometry, so this should really be _NS
    // Keep in mind that calcTerminator() uses this !!! So don't get too smart with it.
    //
    // std::cout << "XYZ: " << pos.x << "," << pos.y << "," << pos.z << "\n";
    return LLH({ atan2(pos.z, sqrt(pos.x * pos.x + pos.y * pos.y)), atan2(pos.y, pos.x), glm::length(pos) });
}
LLH Earth::getXYZtoLLH_AE(glm::vec3 pos) {
    // Check if XY is outside the AE disc !!! - I.e. if w below is larger than radius
    LLH ret;
    ret.dst = pos.z;
    double w = sqrt(pos.x * pos.x + pos.y * pos.y);
    ret.lon = atan2(pos.y, pos.x); // = asin(y/w);
    ret.lat = pi2f - (w * pi2f) / m_Radius;
    return ret;
}
LLH Earth::getSun(double jd) {
    // If jd == 0 get Sun GHA,Dec for current JD, else get Sun for provided JD - handled in getDecGHA()
    return m_scene->m_astro->getDecGHA(SUN, jd);
}
LLH Earth::getPlanet(unsigned int planet, double jd) {
    return m_scene->m_astro->getDecGHA(planet, jd);
}
void Earth::CalcMoon() {
    double currentJD = m_scene->m_astro->getJD();
    if (m_moonJD == currentJD) return;
    double elon = CAAMoon::EclipticLongitude(currentJD);  // lambda
    double elat = CAAMoon::EclipticLatitude(currentJD);   // beta
    double Epsilon = CAANutation::TrueObliquityOfEcliptic(currentJD);
    CAA2DCoordinate equa = CAACoordinateTransformation::Ecliptic2Equatorial(elon, elat, Epsilon);
    m_moonDist = CAAMoon::RadiusVector(m_JD); // RadiusVector() returns in km
    m_moonRA = hrs2rad * equa.X;
    m_moonDec = deg2rad * equa.Y;
    m_moonHour = m_moonRA - m_scene->m_astro->getGsid(); // Gsid returns in radians now, so don't convert!
    double w = cos(m_moonDec);
    MoonLightDir.x = (float)(cos(m_moonHour) * w);
    MoonLightDir.y = (float)(sin(m_moonHour) * w);
    MoonLightDir.z = (float)(sin(m_moonDec));
    m_moonJD = currentJD;
}
LLH Earth::getMoon(double JD) {
    if (JD == 0.0) return LLH{ m_moonDec, m_moonHour, m_moonDist };
    return CalcMoonJD(JD);
}
LLH Earth::CalcMoonJD(double JD) {
    // NOTE: Returns position of Moon at JD, does NOT update m_moon*
    double gsidtime = CAASidereal::ApparentGreenwichSiderealTime(JD); // Nonstd JD, don't just pick from World
    double elon = CAAMoon::EclipticLongitude(JD);  // lambda
    double elat = CAAMoon::EclipticLatitude(JD);   // beta
    double Epsilon = CAANutation::TrueObliquityOfEcliptic(JD);
    CAA2DCoordinate equa = CAACoordinateTransformation::Ecliptic2Equatorial(elon, elat, Epsilon);
    return { deg2rad * equa.Y, hrs2rad * (equa.X - gsidtime), m_moonDist }; // m_moonDist?
}
unsigned int Earth::addDot(double lat, double lon, double height, float size, glm::vec4 color, bool radians) {
    if (!radians) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
    glm::vec3 pos = getLoc3D(lat, lon, height);
    unsigned int index = m_dots->addXYZ(pos, color, size);
    m_dotcache.push_back({ color, lat, lon, height, size, index });
    return (unsigned int)m_dotcache.size() - 1;
}
// ADD: changeDot()
void Earth::deleteDot(unsigned int index) {
    m_dots->remove(m_dotcache[index].index);
    m_dotcache[index].index = maxuint;
}
// Arrows
void Earth::addArrow3DTrueSun(float length, float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present, or risk adding a duplicate
    // NOTE: Arrows start at center of Earth. Is this suitable for all Geometries? !!!
    if (checkit) for (auto& ar : m_arrowcache) { if (ar.type == TRUESUN3D) return; }
    glm::vec3 dir = calcRADec2Dir(getSun());
    unsigned int index = m_arrows->addStartDirLen(glm::vec3(0.0f), dir, length, width, color);
    m_arrowcache.push_back({ glm::vec3(0.0f), dir, color, length, width, 0.0, 0.0, index, TRUESUN3D, maxuint });
}
void Earth::deleteArrow3DTrueSun() {
    for (auto& a : m_arrowcache) {
        if (a.type == TRUESUN3D) {
            m_arrows->remove(a.index);
            a.type = maxuint;
            //return;  // NOTE: Should I let the loop run, in case I have added more than one of this type? !!!
        }
    }
}
void Earth::changeArrow3DTrueSun(float length, float width, glm::vec4 color) {
    for (auto& a : m_arrowcache) {
        if (a.type == TRUESUN3D) {
            m_arrows->changeStartDirLen(a.index, a.position, a.direction, length, width, color);
            a = { a.position,a.direction,color,length,width,a.elevation,a.azimuth,a.index,TRUESUN3D, maxuint };
        }
    }
}
void Earth::updateArrow3DTrueSun(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, glm::vec3(0.0f), calcRADec2Dir(getSun()), ar.length, ar.width, ar.color);
}
void Earth::addLunarUmbraCone() {
    const double radiusdiff = moonradius - sunradius;
    LLH sun = m_scene->m_astro->getDecGHA(SUN);
    glm::vec3 sunpos = getLoc3D_NS(sun.lat, sun.lon, sun.dst);
    glm::vec3 moonpos = getLoc3D_NS(m_moonDec, m_moonHour, m_moonDist);
    glm::vec3 conedir = sunpos - moonpos;
    double slope = radiusdiff / glm::length(conedir);
    float clength = (float) -sunradius * glm::length(conedir) / (float)radiusdiff;
    std::cout << "Conelength: " << clength << "\n";
    glm::vec3 conetip = glm::normalize(conedir);
    conetip = conetip * clength + moonpos;
    unsigned int index = m_dots->addXYZ(moonpos/(float)(earthradius*10.0), WHITE, 0.1f);
}
glm::vec3 Earth::getSubsolarXYZ(double jd) {
    if (jd == m_JD) return flatSun;
    LLH mysun = m_scene->m_astro->getDecGHA(SUN, jd); // subsolar.lon is west of south, 0 to tau
    if (mysun.lon < pi) mysun.lon = -mysun.lon;
    else mysun.lon = tau - mysun.lon;                          // subsolar.lon is now -pi to pi east of south
    return getLoc3D(mysun.lat, mysun.lon, m_flatsunheight / earthradius);
}
void Earth::addSubsolarPoint(float size) {
    //std::cout << "Subsolar point : " << m_sunDec << ", " << m_sunHour << "\n";
    if (m_sunob != nullptr) return;
    m_sunob = new SubSolar(m_scene, 16, 32, size);
    m_sunob->Update(flatSun);
}
void Earth::deleteSubsolarPoint() {
    if (m_sunob != nullptr) delete m_sunob;
    m_sunob = nullptr;
}
void Earth::updateSubsolarPoint() {
    if (m_sunob == nullptr) {
        std::cout << "Earth::updateSubsolarPoint() was called, but there is no sundot to update.\n";
        return;
    }
    m_sunob->Update(flatSun);
}
void Earth::addSubsolarPole(float width) {
    if (m_sunpole != maxuint) return; // There is a SubSolarPole already
    m_sunpole = m_cylinders->addStartEnd(solarGP, flatSun, width, LIGHT_YELLOW);
}
void Earth::deleteSubsolarPole() {
    if (m_sunpole == maxuint) return; // Bail if there is no sunpole
    m_cylinders->remove(m_sunpole);
    m_sunpole = maxuint;
}
void Earth::updateSubsolarPole() {
    if (m_sunpole == maxuint) return; // Bail if there is no sunpole
    //std::cout << "Earth::updateSubsolarPole(): " << m_sunpole << "\n";
    m_cylinders->changeStartEnd(m_sunpole, solarGP, flatSun, 0.003f, LIGHT_YELLOW);
}
void Earth::addSubsolarCone_NS(glm::vec4 color) {
    // https ://stackoverflow.com/questions/64881275/satellite-on-orbit-create-the-tangent-cone-from-point-to-sphere
    // (see also https://en.wikipedia.org/wiki/Tangent_lines_to_circles#Outer_tangent but not as useful)
    //glm::vec3 pos = getLoc3D(m_sunDec, m_sunHour, m_flatsunheight / earthradius);
    double rE = 1.0f;  //self.NSscale # Radius of Earth
    double d = glm::length(flatSun);
    double l = sqrt(d * d - rE * rE);
    double t = asin(rE / d);
    double h = l * cos(t);
    double rc = l * sin(t);
    m_suncone = m_scene->getViewConesOb()->addStartDirLen(flatSun, glm::normalize(flatSun), (float)h - 0.001f, (float)rc + 0.002f, color);
}
void Earth::updateSubsolarCone_NS() {
    // https ://stackoverflow.com/questions/64881275/satellite-on-orbit-create-the-tangent-cone-from-point-to-sphere
    // (see also https://en.wikipedia.org/wiki/Tangent_lines_to_circles#Outer_tangent but not as useful)
    //glm::vec3 pos = getLoc3D(m_sunDec, m_sunHour, m_flatsunheight / earthradius);
    double rE = 1.0f;  //self.NSscale # Radius of Earth
    double d = glm::length(flatSun);
    double l = sqrt(d * d - rE * rE);
    double t = asin(rE / d);
    double h = l * cos(t);
    double rc = l * sin(t);
    m_scene->getViewConesOb()->changeStartDirLen(m_suncone, flatSun, glm::normalize(flatSun), (float)h - 0.001f, (float)rc + 0.002f, NO_COLOR);
}
LLH Earth::getSublunarPoint() {
    return { m_moonDec, m_moonHour, m_flatsunheight };
}
glm::vec3 Earth::getSublunarXYZ() {
    return getLoc3D(m_moonDec, m_moonHour, m_flatsunheight / earthradius);
}
void Earth::addSublunarPoint(float size) {
    //std::cout << "Sublunar point : " << m_moonDec << ", " << m_moonHour << "\n";
    if (m_moonob != nullptr) return;
    if (m_JD == 0.0) CalcMoon();
    m_moonob = new SubLunar(m_scene, 16, 32, size);
    double moon_lon = m_moonHour;
    while (moon_lon > pi) moon_lon -= tau;
    while (moon_lon <= -pi) moon_lon += tau;
    m_moonob->Update(getLoc3D(m_moonDec, moon_lon, m_flatsunheight / earthradius));
    //std::cout << m_moonDist << "\n";
}
void Earth::deleteSublunarPoint() {
    if (m_moonob != nullptr) delete m_moonob;
}
void Earth::updateSublunarPoint() {
    //std::cout << "Sublunar point : " << m_moonDec << ", " << m_moonHour << "\n";
    if (m_moonob == nullptr) {
        std::cout << "Earth::updateSublunarPoint() was called, but there is no moondot to update.\n";
        return;
    }
    double moon_lon = m_moonHour;
    while (moon_lon > pi) moon_lon -= tau;
    while (moon_lon <= -pi) moon_lon += tau;
    m_moonob->Update(getLoc3D(m_moonDec, moon_lon, m_flatsunheight / earthradius));
}
void Earth::addViewConeXYZ_NS(glm::vec3 pos, glm::vec4 color) {
    // https://stackoverflow.com/questions/64881275/satellite-on-orbit-create-the-tangent-cone-from-point-to-sphere
    // (see also https://en.wikipedia.org/wiki/Tangent_lines_to_circles#Outer_tangent but not as useful)
    double rE = 1.0;  //self.NSscale # Radius of Earth
    double d = glm::length(pos);
    double l = sqrt(d * d - rE * rE);
    double t = asin(rE / d);
    double h = l * cos(t);   // Height of cone
    double rc = l * sin(t);  // Radius of cone base - t is asin(rE/d) making this l * sin(asin(rE/d)), so l * rE/d ? !!!
    m_scene->getViewConesOb()->addStartDirLen(pos, glm::normalize(pos), (float)h - 0.001f, (float)rc + 0.001f, color);
}
void Earth::addViewConeLLH_NS(LLH loc, glm::vec4 color) {  // LLH is under used, could be for all the getLoc3D* and getNml3D* functions etc !!!
    addViewConeXYZ_NS(getLoc3D_NS(deg2rad * loc.lat, deg2rad * loc.lon, loc.dst), color);
}
// How to remove View Cones? They don't make sense for morphs, they are specific to NS geometry. !!!
// The object they tie to may move with time though. So there should also be update functions. Thus, a viewconecache is needed. !!!
void Earth::addGrid(float deg, float size, glm::vec4 color, std::string type, bool rad, bool eq, bool pm) {
    if (!rad) deg *= deg2radf;
    // If equator is enabled, draw equator separately
    if (eq) addLatitudeCurve(0.0f, color, size, true, GRIDLAT);
    // Use symmetry to draw remaining parallels
    if (type.find("LA") != std::string::npos) {
        for (float lat = deg; lat < pi2 - tiny; lat += deg) {
            addLatitudeCurve(lat, color, size, true, GRIDLAT);
            addLatitudeCurve(-lat, color, size, true, GRIDLAT);
        }
    }
    // if prime meridian is enabled, draw prime meridian separately
    if (pm) addLongitudeCurve(0.0f, color, size, true, GRIDLON);
    // Use symmetry to draw remaining meridians
    if (type.find("LO") != std::string::npos) {
        for (float lon = deg; lon <= pif; lon += deg) {
            addLongitudeCurve(lon, color, size, true, GRIDLON);
            if (lon < pif - tiny) addLongitudeCurve(-lon, color, size, true, GRIDLON); // Evade singularity at north pole
        }
    }
}
void Earth::removeGrid() {
    for (auto& po : m_polycache) {
        if (po.type == GRIDLAT || po.type == GRIDLON) po.type = NONE;
    }
}
void Earth::addEquator(float size, glm::vec4 color) {
    addLatitudeCurve(0.0f, color, size, false, EQUATOR);
}
void Earth::removeEquator() {
    for (auto& po : m_polycache) {
        if (po.type == EQUATOR) po.type = NONE;
    }
}
void Earth::addTropics(float size, glm::vec4 color) {
    // NOTE: Tropics seem to be fixed by convention but arctics not? That is false according to wikipedia articles on the tropics
    addLatitudeCurve(earthtropics, color, size, false, TROPIC);  // Cancer
    addLatitudeCurve(-earthtropics, color, size, false, TROPIC); // Capricorn
}
void Earth::removeTropics() {
    for (auto& po : m_polycache) {
        if (po.type == TROPIC) po.type = NONE;
    }
}
void Earth::addArcticCirles(float size, glm::vec4 color) {
    // NOTE: Arctic circle location should account for refraction? No, but nutation ideally, according to definition
    addLatitudeCurve(eartharctics, color, size, false, ARCTIC);
    addLatitudeCurve(-eartharctics, color, size, false, ARCTIC);
}
void Earth::removeArcticCircles() {
    for (auto& po : m_polycache) {
        if (po.type == ARCTIC) po.type = NONE;
    }
}
unsigned int Earth::addLatitudeCurve(double lat, glm::vec4 color, float width, bool rad, unsigned int type) {
    if (!rad) lat *= deg2rad;
    PolyCurve* curve = m_scene->newPolyCurve(color, width);
    curve->addPoint(getLoc3D(lat, (-180.0+tiny) * deg2rad, 0.0));
    for (int lon = -179; lon <= 180; lon++) { // Note: we do not want -180, but we do want +180
        curve->addPoint(getLoc3D(lat, lon * deg2rad, 0.0));
    }
    curve->generate();
    m_polycache.push_back({ curve, width, type, color, LLH({ lat,0.0 ,0.0 }), LLH({ 0.0 ,0.0 ,0.0 }) });
    return (unsigned int)m_polycache.size() - 1;
}
void Earth::changeLatitudeCurve(unsigned int index, double lat, glm::vec4 color, float width, bool rad) {
    // lat outside -360 to 360 range is assumed to be NO_LAT.
    if (!rad) lat *= deg2rad;
    if (m_polycache[index].type == LATITUDE) {
        if (color != NO_COLOR) m_polycache[index].color = color;
        if (abs(lat) <= tau) m_polycache[index].llh1.lat = lat;
        if (width > 0.0f) m_polycache[index].width = width;
    }
}
void Earth::updateLatitudeCurve(polycache& p) {
    p.path->clearPoints();
    p.path->addPoint(getLoc3D(p.llh1.lat, (-180.0 + tiny) * deg2rad, 0.0));
    for (int lon = -179; lon <= 180; lon++) { // Note: we do not want -180, but we do want +180
        p.path->addPoint(getLoc3D(p.llh1.lat, lon * deg2rad, 0.0));
    }
    p.path->generate();
}
void Earth::addPrimeMeridian(float size, glm::vec4 color) {
    addLongitudeCurve(0.0, color, size, false, PRIME_MERIDIAN); // Technically 0 degrees = 0 radians, beware when copy/pasting
}
void Earth::removePrimeMeridian() {
    for (auto& po : m_polycache) {
        if (po.type == PRIME_MERIDIAN) po.type = NONE;
    }
}
unsigned int Earth::addLongitudeCurve(double lon, glm::vec4 color, float width, bool rad, unsigned int type) {
    if (!rad) lon *= deg2rad;
    PolyCurve* curve = m_scene->newPolyCurve(color, width);
    for (int lat = -90; lat <= 90; lat++) {
        curve->addPoint(getLoc3D(lat * deg2rad, lon, 0.0));
    }
    curve->generate();
    m_polycache.push_back({ curve,width,type,color, {0.0,lon,0.0},{0.0,0.0,0.0} });
    return (unsigned int)m_polycache.size() - 1;
}
void Earth::changeLongitudeCurve(unsigned int index, double lon, glm::vec4 color, float width, bool rad) {
    // lat outside -360 to 360 range is assumed to be NO_LAT.
    if (!rad) lon *= deg2rad;
    if (m_polycache[index].type == LONGITUDE) {
        if (color != NO_COLOR) m_polycache[index].color = color;
        if (abs(lon) <= tau) m_polycache[index].llh1.lon = lon;
        if (width > 0.0f) m_polycache[index].width = width;
    }
}
void Earth::updateLongitudeCurve(polycache& p) {
    p.path->clearPoints();
    for (int lat = -90; lat <= 90; lat++) {
        p.path->addPoint(getLoc3D(lat * deg2rad, p.llh1.lon, 0.0));
    }
    p.path->generate();
}
double Earth::calcArcDist(LLH llh1, LLH llh2, bool rad) {
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
LLH Earth::calcGreatArc(LLH llh1, LLH llh2, double f, double refraction, bool rad) {
    if (!rad) {
        llh1.lat *= deg2rad; // doubles
        llh1.lon *= deg2rad;
        llh2.lat *= deg2rad;
        llh2.lon *= deg2rad;
    }
    LLH ret;
    double d = acos(sin(llh1.lat) * sin(llh2.lat) + cos(llh1.lat) * cos(llh2.lat) * cos(llh1.lon - llh2.lon));
    double A = sin((1 - f) * d) / sin(d);
    double B = sin(f * d) / sin(d);
    double x = A * cos(llh1.lat) * cos(llh1.lon) + B * cos(llh2.lat) * cos(llh2.lon);
    double y = A * cos(llh1.lat) * sin(llh1.lon) + B * cos(llh2.lat) * sin(llh2.lon);
    double z = A * sin(llh1.lat) + B * sin(llh2.lat);
    ret.lat = atan2(z, sqrt(x * x + y * y));
    ret.lon = atan2(y, x);
    if (abs(ret.lat) == pi2) ret.lon = 0.0; // On the poles longitude is undefined, setting to 0.0.
    if (!rad) {
        ret.lat *= rad2deg;
        ret.lon *= rad2deg;
    }
    return ret;
}
LLH Earth::calcLerpArc(LLH llh1, LLH llh2, double f, double refraction, bool rad) {
    if (!rad) {
        llh1.lat *= deg2rad; // doubles
        llh1.lon *= deg2rad;
        llh2.lat *= deg2rad;
        llh2.lon *= deg2rad;
    }
    LLH ret;
    ret.lat = f * llh2.lat + (1 - f) * llh1.lat;
    ret.lon = f * llh2.lon + (1 - f) * llh1.lon;
    ret.dst = f * llh2.dst + (1 - f) * llh1.dst;
    if (!rad) {
        ret.lat *= rad2deg;
        ret.lon *= rad2deg;
    }
    return ret;
}
void Earth::addLerpArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad) {
    //if (llh1.lon < llh2.lon) { // Do we still strictly need to go from high to low longitude? I guess not, so will comment out for now !!!
    //    LLH llhtmp = llh1;
    //    llh1 = llh2;
    //    llh2 = llhtmp;
    //}
    addArc(llh1, llh2, color, width, rad, &Earth::calcLerpArc, LERPARC);
}
void Earth::addGreatArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad) {
    //if (llh1.lon < llh2.lon) { // Do we still strictly need to go from high to low longitude? I guess not, so will comment out for now !!!
    //    LLH llhtmp = llh1;
    //    llh1 = llh2;
    //    llh2 = llhtmp;
    //}
    addArc(llh1, llh2, color, width, rad, &Earth::calcGreatArc, GREATARC);
}
void Earth::addArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad, calcFunc ca, unsigned int type) {
    if (!rad) {
        llh1.lat *= deg2rad; // double
        llh2.lat *= deg2rad; // 
        llh1.lon *= deg2rad; // 
        llh2.lon *= deg2rad; // 
    }
    // Might check if one of same type is present, and toss a warning to console !!!
    PolyCurve* curve1 = m_scene->newPolyCurve(color, width);
    PolyCurve* curve2 = m_scene->newPolyCurve(color, width);
    m_polycache2.push_back({ curve1, curve2, width, type, color, llh1, llh2, /* refraction */ 0.0, /* fend */ 1.0, ca, /* closed */ false});
    updateCompositePath(m_polycache2.back());
    return;
}
void Earth::addFlatArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad) {
    // The shortest path between two points on the AE map
    // Completely different structure than addArc() et al, so no need to try to share code
    // Also does not have singularities, because LERP is done in Cartesian coordinates.
    float mindist = 0.01f; // Desired step size
    if (!rad) {
        llh1.lat *= deg2rad;
        llh1.lon *= deg2rad;
        llh2.lat *= deg2rad;
        llh2.lon *= deg2rad;
    }
    PolyCurve* curve = m_scene->newPolyCurve(color, width);
    LLH llh;
    glm::vec3 pos2;
    // Get Cartesian endpoints and lerp in xyz space
    glm::vec3 beg = getLoc3D_AE((float)llh1.lat, (float)llh1.lon, (float)llh1.dst);
    glm::vec3 fin = getLoc3D_AE((float)llh2.lat, (float)llh2.lon, (float)llh2.dst);
    float plen = glm::length(fin - beg);
    unsigned int steps = std::max(1, (int)(plen / mindist));
    Lerper<float> lx = Lerper<float>(beg.x, fin.x, steps, false);
    Lerper<float> ly = Lerper<float>(beg.y, fin.y, steps, false);
    Lerper<float> lz = Lerper<float>(beg.z, fin.z, steps, false);
    for (unsigned int i = 0; i < steps; i++) {
        // Convert to AE lat,lon
        llh = getXYZtoLLH_AE(glm::vec3(lx.getNext(), ly.getNext(), lz.getNext()));
        // Convert to Earth shape in use
        pos2 = getLoc3D(llh.lat, llh.lon, llh.dst);
        curve->addPoint(pos2);
    }
    curve->generate();
    m_polycache.push_back({ curve, width, FLATARC, color, llh1,llh2 });
}
void Earth::updateGreatArc(polycache2& p) {
    p.path->clearPoints();
    p.path2->clearPoints();
    updateCompositePath(p);
}
void Earth::updateLerpArc(polycache2& p) {
    p.path->clearPoints();
    p.path2->clearPoints();
    updateCompositePath(p);
}
void Earth::updateFlatArc(polycache p) {
    // The shortest path between two points on the AE map
    float mindist = 0.01f; // Desired step size
    PolyCurve* curve = p.path;
    curve->clearPoints();
    LLH llh;
    glm::vec3 pos2;
    // Get Cartesian endpoints and lerp in xyz space
    glm::vec3 beg = getLoc3D_AE(p.llh1.lat, p.llh1.lon, p.llh1.dst);
    glm::vec3 fin = getLoc3D_AE(p.llh2.lat, p.llh2.lon, p.llh2.dst);
    float plen = glm::length(fin - beg);
    unsigned int steps = std::max(1, (int)(plen / mindist));
    Lerper<float> lx = Lerper<float>(beg.x, fin.x, steps, false);
    Lerper<float> ly = Lerper<float>(beg.y, fin.y, steps, false);
    Lerper<float> lz = Lerper<float>(beg.z, fin.z, steps, false);
    for (unsigned int i = 0;i < steps;i++) {
        // Convert to AE lat,lon
        llh = getXYZtoLLH_AE(glm::vec3(lx.getNext(), ly.getNext(), lz.getNext()));
        // Convert to Earth shape in use
        pos2 = getLoc3D(llh.lat, llh.lon, llh.dst);
        curve->addPoint(pos2);
    }
    curve->generate();
}
void Earth::addArcPoint(glm::vec3 ap, bool first, bool last, glm::vec3& oldap, PolyCurve* curve) {
    static const float mindiff = 0.05f;
    if (!last && !first && glm::length(ap - oldap) < mindiff) return; // Skip of new point is too close to previous one
    curve->addPoint(ap);
    oldap = ap; // Store, since last added point is not available directly from PolyCurve.
    return;
}
void Earth::addTerminatorTrueMoon(glm::vec4 color, float width) {
    //PolyCurve* curve1 = m_scene->newPolyCurve(LIGHT_GREEN, width);  // Show paths in two different colors to troubleshoot
    //PolyCurve* curve2 = m_scene->newPolyCurve(LIGHT_RED, width);
    PolyCurve* curve1 = m_scene->newPolyCurve(color, width);
    PolyCurve* curve2 = m_scene->newPolyCurve(color, width);
    m_polycache2.push_back({ curve1, curve2,
                                width, MOONTERMINATOR, color,
                                {m_moonDec, m_moonHour, 0.0}, {0.0, 0.0, 0.0},
                                w_refract ? w_mrefang : 0.0, tau, &Earth::calcTerminator, /*closed*/ true });
    updateCompositePath(m_polycache2.back());
}
void Earth::deleteTerminatorTrueMoon() {
    for (auto& p : m_polycache2) {
        if (p.type == MOONTERMINATOR) {
            p.type = NONE;         // Hide the path from updaters
            // clearPoints() also clears the geometric segments, so no need to call generate();
            p.path->clearPoints(); // Discard the point list
            p.path2->clearPoints(); // Discard the point list
            break; // Assumes there is only one MOONTERMINATOR
        }
    }
}
void Earth::updateTerminatorTrueMoon(polycache2& p) {
    p.path->clearPoints();
    p.path2->clearPoints();
    p.refraction = w_refract ? w_mrefang : 0.0;
    p.llh1.lat = m_moonDec;
    p.llh1.lon = m_moonHour;
    p.fend = tau;
    updateCompositePath(p);
    return;
}
void Earth::addTerminatorTrueSun(glm::vec4 color, float width) {
    PolyCurve* curve1 = m_scene->newPolyCurve(color, width);
    PolyCurve* curve2 = m_scene->newPolyCurve(color, width);
    m_polycache2.push_back({ curve1, curve2,
                                width, SUNTERMINATOR, color,
                                {subsolar.lat, subsolar.lon, 0.0}, {0.0, 0.0, 0.0},
                                w_refract ? w_srefang : 0.0, tau, &Earth::calcTerminator, /*closed*/ true });
    updateCompositePath(m_polycache2.back());
}
void Earth::deleteTerminatorTrueSun() {
    for (auto& p : m_polycache2) {
        if (p.type == SUNTERMINATOR) {
            p.type = NONE;         // Hide the path from updaters
            // clearPoints() also clears the geometric segments, so no need to call generate();
            p.path->clearPoints(); // Discard the point list
            p.path2->clearPoints(); // Discard the point list
            break; // Assumes there is only one MOONTERMINATOR
        }
    }
}
void Earth::updateTerminatorTrueSun(polycache2& p) {
    p.path->clearPoints();
    p.path2->clearPoints();
    p.refraction = w_refract ? w_srefang : 0.0;
    p.llh1.lat = subsolar.lat;
    p.llh1.lon = subsolar.lon;
    p.fend = tau;
    updateCompositePath(p);
    return;
    return;
}
void Earth::addSubsolarPath(double begin, double finish, unsigned int steps, bool fixed) {
    // Params are in fractional JDs
    if (begin == NO_DOUBLE) begin = -0.5;
    if (finish == NO_DOUBLE) finish = 0.5;
    if (steps == NO_UINT) steps = (finish - begin) * 360.0; // default to 360 degrees per day
    sunpath = m_scene->m_astro->getCelestialPath(SUN, begin, finish, steps, ECGEO, fixed);
    suncurve = m_scene->newPolyCurve(SUNCOLOR, 0.005f, steps);
    updateSubsolarPath();
}
void Earth::updateSubsolarPath() {
    suncurve->clearPoints();
    for (auto& sp : sunpath->entries) {
        suncurve->addPoint(getLoc3D(sp.geodec, -sp.geogha));
    }
    suncurve->generate();
}
LLH Earth::calcTerminator(LLH llh1, LLH llh2, double param, double refang, bool rad) {
    if (!rad) {
        llh1.lat *= deg2rad;
        llh1.lon *= deg2rad;
        param *= deg2rad;
        refang *= deg2rad;
    }
    LLH res;
    double sinparm = sin(param);
    res.lat = asin(cos(llh1.lat) * sinparm);
    double x = -cos(llh1.lon) * sin(llh1.lat) * sinparm - sin(llh1.lon) * cos(param);
    double y = -sin(llh1.lon) * sin(llh1.lat) * sinparm + cos(llh1.lon) * cos(param);
    res.lon = atan2(y, x);
    // Refracted terminator
    if (refang != 0.0) {
        glm::vec3 loc = getLoc3D_NS(res.lat, res.lon);     // Calc as if on sphere
        glm::vec3 sun2 = getLoc3D_NS(llh1.lat, llh1.lon);
        glm::vec3 rloc = glm::rotate(loc, (float)-refang, glm::cross(loc, sun2));
        res = getXYZtoLLH_NS(rloc);
    }
    if (res.lon > pi) res.lon -= tau;
    if (res.lon < -pi) res.lon += tau;
    if (!rad) {
        res.lat *= rad2deg;
        res.lon *= rad2deg;
    }
    return res;
}
void Earth::addTissotIndicatrix(LLH location, double radius, bool rad) {
    // Can draw an indicatrix directly at a pole.
    // To validate, compare with:
    // https://map-projections.net/img/tissot-30-w/azimutal-equidistant-gpolar.jpg?ft=616b1080  radius = 5deg, grid = 10
    // https://map-projections.net/img/tissot-30/rectang-0.jpg?ft=54e394a0 radius 7.5, grid = 15
    if (!rad) {
        location.lat *= deg2rad;
        location.lon *= deg2rad;
        radius *= deg2rad;
    }
    unsigned int steps = 180;  // Might need to be a passed in parameter? !!!
    tissotcurve2 = m_scene->newPolyCurve(LIGHT_BLUE, 0.002f, steps);  // Obviously not scalable. Make into Arc or Composite path
    // Method: Draw a circe around the north pole, of the right size, in lat,lon coordinates.
    // Rotate it into place at provided location, converting to Cartesian world coordinates and plot using PolyCurve.
    double const zangle = location.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
    double const yangle = location.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
    double const cy = cos(yangle);
    double const sy = sin(yangle);
    double const cz = cos(zangle);
    double const sz = sin(zangle);
    double const lz = (float)sin(pi2 - radius);
    //glm::vec3 loc = glm::vec3(0.0f);
    for (double a = 0.0; a <= tau; a += tau / steps) {
        // This is the actual process: (I have condensed it slightly below, tossing variables that are not used, and substituted vars only used once)
        //loc.x = (float)(radius * cos(a));
        //loc.y = (float)(radius * sin(a));
        //loc.z = (float)sin(pi2 - radius);   // Not dependent on a, so can be outside loop
        //loc2 = glm::rotateY(loc, yangle);
        //loc3 = glm::rotateZ(loc2, zangle);
        //LLH sloc = getXYZtoLLH_NS(loc3);   // lat/lon in radians
        //tissotcurve2->addPoint(getLoc3D(sloc.lat, sloc.lon));

        // The biggest saving is precalculating the sines and cosines of yangle and zangle
        // Further reduction by resolving variables and reducing the formulae yielded no better results
        double lx = radius * cos(a);
        double ly = radius * sin(a);
        double l2x = lx * cy + lz * sy;
        double l3x = l2x * cz - ly * sz;
        double l3y = l2x * sz + ly * cz;
        double lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
        double lon = atan2(l3y, l3x);
        tissotcurve2->addPoint(getLoc3D(lat, lon));
    }
    tissotcurve2->generate();
    // This could relatively easily be altered to be a calculation function for updateCompositePath(), at a slight overhead.
}

void Earth::updateCompositePath(polycache2& p) {
    // NOTE: Height is awkward, not currently tracked/lerped !!!
    //       For Moon and Sun groundpath it is not needed. But for the Lerp and Great paths, it might be nice!
    //       Satellites and Planes will be implemented with a separate custom path anyway.

    glm::vec3 oldap = glm::vec3(1000.0);
    PolyCurve* curve = p.path;
    double histep = 1.3;
    double lostep = 0.24;
    // Get useful old location and add first point to curve
    LLH first = (this->*p.ca)(p.llh1, p.llh2, 0.0, p.refraction, true);
    LLH llhf = first;
    glm::vec3 tp = getLoc3D(llhf.lat, llhf.lon, 0.0);
    addArcPoint(tp, true, false, oldap, curve); // First=true
    LLH oldf = llhf;

    // IMPORTANT: df must stay in range [0;p.fend] otherwise it makes no sense !!! Plz revise!
    double dist = calcArcDist(p.llh1, p.llh2, true);
    double df = 1.0 / (rad2deg * dist); // Reasonable initial stepsize of 1 step per degree? !!!
    //std::cout << "Initial df: " << df << "\n";
    if (p.type == MOONTERMINATOR) { 
        df = deg2rad;
    }
    double f = 0.0; // NOTE: df is added right away in while loop below, but f=0.0 point is already added above.
    unsigned int half = 0;
    unsigned int doub = 0;
    double dflat = 0.0;
    double dflon = 0.0;
    double ilat = 0.0;
    glm::vec3 ip1 = glm::vec3(0.0f);
    glm::vec3 ip2 = glm::vec3(0.0f);
    while (f < p.fend) {
        f += df;
        half = 0;
        doub = 0;
        while (true) {
            llhf = (this->*p.ca)(p.llh1, p.llh2, f, p.refraction, true);
            dflat = abs(llhf.lat - oldf.lat) * rad2deg;
            dflon = abs(llhf.lon - oldf.lon) * rad2deg;

            // Doubling
            if (dflat < lostep && dflon < lostep) { // Should really check the sum of squares
                f -= df; // Should this not be -= ? !!! 
                df *= 2.0;
                f += df;
                doub++;
                if (doub == 11) {
                    std::cout << "After *1/2^11, stepsize is still too small, giving up!\n";
                    if (p.fend == 1.0) df = 1.0 / dist;
                    if (p.fend == tau) df = deg2rad;
                    f += df;
                    break;
                }
                continue;
            }

            // Triggers when step is within limits and no lon inversion
            if (dflat < histep && dflon < histep) break;
            // Triggers when step is within limits and lon has inversion
            if (dflat < histep && dflon > 270.0) break;

            // Halving
            if (dflat > histep || (270.0 > dflon && dflon > histep)) {
                f -= df; // Undo the step we took
                df *= 0.5; // was df /= 2
                f += df;
                half++;
                if (half == 11) {
                    std::cout << "After *2^11, stepsize is still too small, giving up!\n";
                    if (p.fend == 1.0) df = 1.0 / dist;
                    if (p.fend == tau) df = deg2rad;
                    f += df;
                    break;
                }
                continue;
            }
        }
        // Here we have the next point ready
        tp = getLoc3D(llhf.lat, llhf.lon, 0.0f);
        // If a seam is crossed, split the path and insert intermediate points
        if (abs(llhf.lon - oldf.lon) > pi2) { // Longitude inversion means crossing a seam
            if (oldf.lon > llhf.lon) { // Passed counter clockwise (+ to -)
                ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tau - oldf.lon);
                ip1 = getLoc3D(ilat, pi - tiny);
                ip2 = getLoc3D(ilat, tiny - pi);
            }
            else { // Passed clockwise
                ilat = oldf.lat - (pif - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tau - oldf.lon);
                ip2 = getLoc3D(ilat, pi - tiny);
                ip1 = getLoc3D(ilat, tiny - pi);
            }
            // Now insert the extra points to end one path and begin the next
            addArcPoint(ip1, false, true, oldap, curve); // last=true so cap path and start new one
            curve->generate();
            curve = p.path2;
            addArcPoint(ip2, true, false, oldap, curve);  // First point in new path
        }
        if (0.0 < f && f < p.fend) addArcPoint(tp, false, false, oldap, curve);
        oldf = llhf;
    }
    if (!p.closed) addArcPoint(getLoc3D(p.llh2.lat, p.llh2.lon), false, true, oldap, curve); // End point of open path - last = true
    if (p.closed) addArcPoint(getLoc3D(first.lat, first.lon), false, true, oldap, curve); // End point of closed path - last = true
    curve->generate();
    return;

}
void Earth::genGeom() {
    double lat, lon;
    for (unsigned int v = 0; v <= m_meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
        lat = (pi * v / m_meshV) - pi2;
        for (unsigned int u = 0; u <= m_meshU; u++) {
            lon = (tau * u / m_meshU) - pi;
            glm::vec3 loc = getLoc3D(lat, lon);     // position of vertex
            glm::vec3 nms = getNml3D_NS(lat, lon);  // Insolation normal, always NS (or G8?)
            glm::vec3 nml = getNml3D(lat, lon);     // Geometry normal, for camera lighting
            m_verts.push_back({ loc, nms, nml, glm::vec2(u / (float)m_meshU, v / (float)m_meshV), BLACK });
            if (u < m_meshU && v < m_meshV) {
                m_tris.push_back({ (m_meshU + 1) * v + u , (m_meshU + 1) * (v + 1) + u, (m_meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (m_meshU + 1) * (v + 1) + 1 + u, (m_meshU + 1) * v + u + 1, (m_meshU + 1) * v + u });
            }
        }
    }
}
void Earth::updateSun() {
    // Get XYZ of subsolar point and flat sun (as per sun height set in GUI)
    subsolar = m_scene->m_astro->getDecGHA(SUN); // subsolar.lon is west of south, 0 to tau
    //std::cout << "Earth::updateSun(): subsolar.gha { " << subsolar.lat << ", " << subsolar.lon << ", " << subsolar.dst << " }\n";

    if (subsolar.lon < pi) subsolar.lon = -subsolar.lon;
    else subsolar.lon = tau - subsolar.lon;  // subsolar.lon is now -pi to pi east of south - Is this the right choice to store? !!!
    double sun_lon = subsolar.lon;
    while (sun_lon <= -pi) sun_lon += tau;   // For SubSolar, not sure if it impacts other parts, so check for that before updating subsolar.lon !!!
    while (sun_lon > pi) sun_lon -= tau;     // Maybe do this lon wrapping in getLoc3D_ER etc. instead !!!
    //std::cout << "Earth::updateSun(): subsolar.geo { " << subsolar.lat << ", " << subsolar.lon << ", " << subsolar.dst << " }\n\n";
    solarGP = getLoc3D(subsolar.lat, sun_lon, 0.0f);
    flatSun = getLoc3D(subsolar.lat, sun_lon, m_flatsunheight / earthradius);
    if (do_eccentriclight) { SunLightDir = getNml3D_EE(subsolar.lat, subsolar.lon, 0.0f); return; }
    float w = (float)cos(subsolar.lat);
    SunLightDir.x = (float)cos(subsolar.lon) * w;
    SunLightDir.y = (float)sin(subsolar.lon) * w;
    SunLightDir.z = (float)sin(subsolar.lat);
}


// ----------
//  Location
// ----------
Location::Location(Earth* earth, double lat, double lon, bool radians, float rsky) {
    m_earth = earth;
    m_scene = m_earth->m_scene;  // Later, refactor this so the Location can be on other planetary bodies, like the Moon or a planet.
    m_radius = rsky;
    m_arrows = m_scene->getArrowsOb();
    m_dots = m_scene->getDotsOb();
    m_planes = m_scene->getPlanesOb();
    m_cylinders = m_scene->getCylindersOb();
    m_anglearcs = m_scene->getAngleArcsOb();
    if (!radians) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
    storeLatLon(lat, lon);
    m_pos = m_earth->getLoc3D(m_lat, m_lon);
    m_east = m_earth->getEast3D(m_lat, m_lon);
    m_north = m_earth->getNorth3D(m_lat, m_lon);
    m_zenith = glm::cross(m_east, m_north);
    m_arrowcache.reserve(locationreserveitems);
    m_dotcache.reserve(locationreserveitems);
    m_planecache.reserve(locationreserveitems);
    m_polycache.reserve(locationreserveitems);
    m_cylindercache.reserve(locationreserveitems);

    // The new way
    truesun = new Location::TrueSun(this); // Construct but don't enable any items within (arrows, paths, ...)
}
Location::~Location() {
    //std::cout << "~Loc (destructor) called.\n";
    Destroy();

    // New way
    delete truesun; // It is always constructed, so always destroy.
}
void Location::Destroy() {
    if (!m_polycache.empty()) {
        for (auto& pa : m_polycache.m_Elements) {  // These are full objects made with new, so delete
            m_scene->deletePolyCurve(pa.path);
            delete pa.planetCP;
        }
        m_polycache.clear();
    }
    if (!m_arrowcache.empty()) {
        for (auto& ar : m_arrowcache) { // These are primitives based, so call Delete()
            if (ar.type != maxuint) {
                m_arrows->remove(ar.index); //  MUST do this, otherwise primitives are dangling if locs are removed during anim
            }
        }
        m_arrowcache.clear();
    }
    if (!m_dotcache.empty()) {
        for (auto& d : m_dotcache) {
            if (d.type != maxuint) {
                m_dots->remove(d.index);
            }
        }
        m_dotcache.clear();
    }
    if (!m_planecache.empty()) {
        for (auto& pl : m_planecache) {
            if (pl.type != maxuint) {
                m_planes->remove(pl.index);
            }
        }
        m_planecache.clear();
    }
    if (!m_cylindercache.empty()) {
        for (auto& cy : m_cylindercache) {
            if (cy.type != maxuint) {
                m_cylinders->remove(cy.index);
            }
        }
        m_cylindercache.clear();
    }
}
void Location::Update(bool time, bool morph, bool flatsun) {
    // External callers: Earth::Update() only
    // If location has moved, pretend it was a morph, the same objects need updating.
    if (morph) {
        // Location geometry
        m_pos = m_earth->getLoc3D(m_lat, m_lon);
        m_east = m_earth->getEast3D(m_lat, m_lon);
        m_north = m_earth->getNorth3D(m_lat, m_lon);
        m_zenith = glm::cross(m_east, m_north);
    }
    // Update dots
    for (auto& d : m_dotcache) {
        if (d.type == LOC && morph) updateLocDot(d);
        if (d.type == LOCSKY && morph) updateLocSky(d);
    }
    // Update planes
    for (auto& p : m_planecache) {
        if (p.type == TANGENT && morph) updateTangentPlane(p);
        if (p.type == MERIDIAN && morph) updateMeridianPlane(p);
    }
    // Update arrows
    for (auto& a : m_arrowcache) {
        if (a.type == ZENITH && morph) updateUpCoord(a);
        if (a.type == EAST && morph) updateEastCoord(a);
        if (a.type == NORTH && morph) updateNorthCoord(a);
        if (a.type == NORMAL && morph) updateNormalCoord(a);
        //if (a.type == TRUESUN3D && (time || morph)) updateArrow3DTrueSun(a);
        if (a.type == FLATSUN3D && (time || morph || flatsun)) updateArrow3DFlatSun(a);
        if (a.type == TRUEMOON3D && (time || morph)) updateArrow3DTrueMoon(a);
        if (a.type == TRUEPLANET3D && (time || morph)) updateArrow3DTruePlanet(a);
        if (a.type == AZIELE3D && morph) updateArrow3DEleAzi(a);
        if (a.type == RADEC3D && (time || morph)) updateArrow3DRADec(a);
    }
    // Update Lines
    for (auto& l : m_cylindercache) {
        if (l.type == FLATSUN3D) updateLine3DFlatSun(l);
    }
    // Update paths
    for (auto& pa : m_polycache.m_Elements) {
        if (pa.type == TRUESUN3D && (time || morph)) updatePlanetaryPath(pa);
        if (pa.type == TRUEANALEMMA3D && (time || morph)) updatePlanetaryPath(pa);
        if (pa.type == FLATSUN3D && (time || morph || flatsun)) updatePath3DFlatSun(pa.path, pa.color, pa.width);
        if (pa.type == TRUEMOON3D && (time || morph)) updatePath3DTrueMoon(pa);
        if (pa.type == TRUEPLANET3D && (time || morph)) updatePlanetaryPath(pa);
        if (pa.type == SIDPLANET3D && (time || morph)) updatePlanetaryPath(pa);
        if (pa.type == RADEC3D && (time || morph)) updatePath3DRADec(pa);
        if (pa.type == ECGEO && (time || morph)) updatePlanetaryPath(pa);
    }
    truesun->update(time, morph, flatsun);
}
void Location::Draw() {
    // Don't draw Primitives derived objects, as they are drawn from World.
    for (auto& pa : m_polycache.m_Elements) {
        pa.path->draw();
    }
    truesun->draw();
}
glm::vec4 Location::getPlanetColor(unsigned int planet, glm::vec4 color) {
    // Might belong somewhere else, like a config object or CelestialMech !!!
    if (color != NO_COLOR) return color;
    if (planet == SUN) return SUNCOLOR;
    if (planet == MERCURY) return MERCURYCOLOR;
    if (planet == VENUS) return VENUSCOLOR;
    if (planet == MARS) return MARSCOLOR;
    if (planet == JUPITER) return JUPITERCOLOR;
    if (planet == SATURN) return SATURNCOLOR;
    if (planet == URANUS) return URANUSCOLOR;
    if (planet == NEPTUNE) return NEPTUNECOLOR;
    std::cout << "WARNING!! Location::getPlanetColor() called with unknown planet: " << planet << "\n";
    return WHITE;
}
void Location::moveLoc(double lat, double lon, bool radians) {
    if (!radians) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
    storeLatLon(lat, lon);
    Update(false, true, false); // Pretend morph since that hits same targets as move.
}
void Location::storeLatLon(double lat, double lon) {
    if (lat > pi2) lat = fmod(lat, pi2);
    if (lat < -pi2) lat = fmod(lat, pi2);
    if (lat == pi2) lat = pi2 - tiny;
    if (lat == -pi2) lat = tiny - pi2;
    m_lat = lat;
    if (lon > pi) lon = fmod(lon, pi);
    if (lon < -pi) lon = fmod(lon, pi);
    if (lon == pi) lon = pi - tiny;    // no singularity here? !!!
    if (lon == -pi) lon = tiny - pi;
    m_lon = lon;
}
double Location::getLat(bool rad) {
    if (rad) return m_lat;
    else return m_lat * rad2deg;
}
double Location::getLon(bool rad) {
    if (rad) return m_lon;
    else return m_lon * rad2deg;
}
glm::vec3 Location::getPosition() { return m_pos; }
// Calculations
glm::vec3 Location::calcDirEleAzi(LLH heading, bool radians) {
    if (!radians) {
        heading.lat *= deg2rad;
        heading.lon *= deg2rad;
    }
    glm::vec3 dir = -m_north; // South
    dir = glm::rotate(dir, (float)-heading.lat, m_east);    // Elevation + from horizon
    dir = glm::rotate(dir, (float)-heading.lon, m_zenith);  // Azimuth W of S
    return glm::normalize(dir);
}
glm::vec3 Location::calcDirRADec(double ra, double dec, double jd) {
    // Takes RA and Dec in degrees and JD, returns normalized vec3 direction
    if (jd == 0.0) jd = m_scene->m_astro->getJD();
    double gsidtime = m_scene->m_astro->getGsid(jd); // In radians
    double lha = gsidtime + m_lon - deg2rad * ra;
    LLH topocentric = m_scene->m_astro->calcGeo2Topo({ deg2rad * dec, lha, 0.0 }, { m_lat, m_lon, 0.0 });
    return calcDirEleAzi(topocentric, true);
}
glm::vec3 Location::getTrueSunDir(double jd) {
    LLH sun = m_scene->m_astro->getDecGHA(SUN, jd);
    localsun = m_scene->m_astro->calcGeo2Topo(sun, { m_lat, m_lon, 0.0 }); // Sun Ele, Azi
    return calcDirEleAzi(localsun, true);
}
glm::vec3 Location::getFlatSunDir(double jd) {
    glm::vec3 sunloc = m_earth->getSubsolarXYZ(jd);
    return glm::normalize(sunloc - m_pos);
}
// ================================= The New Way ================================= //
// ------------------------------- True Sun object ------------------------------- //
Location::TrueSun::TrueSun(Location* location) : m_location(location) {
    update(true, true, true); // Ensure there are valid astronomical values
}
void Location::TrueSun::update(bool time, bool geometry, bool flatsun) {
    sun = m_location->m_scene->m_astro->getDecGHA(SUN); // Current Sun GHA, Dec, no JD specified
    localsun = m_location->m_scene->m_astro->calcGeo2Topo(sun, { m_location->m_lat, m_location->m_lon, 0.0 }); // Sun Ele, Azi
    //std::cout << "Calculated elevation angle: " << rad2deg*localsun.lat << "\n";
    sundir = m_location->calcDirEleAzi(localsun, true);

    if (m_dot != maxuint) m_location->m_scene->getDotsOb()->changeXYZ(m_dot, m_location->m_pos + sundir * m_location->m_radius, NO_COLOR, NO_FLOAT);
    if (m_arrow != maxuint) m_location->m_scene->getArrowsOb()->changeStartDirLen(m_arrow, m_location->m_pos, sundir, NO_FLOAT, NO_FLOAT, NO_COLOR);
    if (m_eleangle != maxuint) { // add checks for dirty flags
        glm::vec3 proj = sundir - m_location->m_zenith * glm::dot(sundir, m_location->m_zenith);
        m_location->m_anglearcs->update(m_eleangle, m_location->m_pos, proj, sundir, m_location->m_radius, defaultcolor, 0.002f);
    }
    if (m_line != maxuint) m_location->m_cylinders->changeStartEnd(m_line, m_location->m_pos, sundir * 100.0f + m_location->m_pos, 0.001f, defaultcolor);
    // No need to update paths, Location takes care of it!
}
void Location::TrueSun::draw() {
    // !!! This duplicates updates if both are enabled. There must be a better way !!!
    if (m_eleangle != maxuint) m_location->m_anglearcs->draw();
    if (m_aziangle != maxuint) m_location->m_anglearcs->draw();
}
void Location::TrueSun::enableArrow3D() { // Add params such as color, length, width (, refraction?)
    if (m_arrow != maxuint) return; // Already enabled
    // Move getTrueSunDir() into Location::TrueSun !!! - in progress, remove from Location once all items have been migrated
    update(true,true,true); // Calculate parameters 
    m_arrow = m_location->m_scene->getArrowsOb()->addStartDirLen(m_location->m_pos, sundir, m_location->m_radius,0.003f, defaultcolor);
}
// updateArrow3D(), or just do it directly in update() ?
void Location::TrueSun::changeArrow3D(glm::vec4 color, float length, float width) {
    m_location->m_scene->getArrowsOb()->changeArrow(m_arrow, color, length, width);
}
void Location::TrueSun::disableArrow3D() {} // Only add if needed
void Location::TrueSun::enableDot3D() {
    m_dot = m_location->m_dots->addXYZ(m_location->m_pos + sundir * m_location->m_radius, defaultcolor, locdotsize);
}
void Location::TrueSun::changeDot3D(glm::vec4 color, float size) {
    m_location->m_scene->getDotsOb()->changeDot(m_dot, color, size);
}
void Location::TrueSun::disableDot3D() {} // Only add if needed
void Location::TrueSun::enableLine3D() {
    m_line = m_location->m_cylinders->addStartEnd(m_location->m_pos, sundir * 100.0f + m_location->m_pos, 0.001f, defaultcolor);
}
void Location::TrueSun::enableAzimuthAngle() {

}
void Location::TrueSun::enableElevationAngle() { // add params for length width color
    // As per https://www.maplesoft.com/support/help/maple/view.aspx?path=MathApps%2FProjectionOfVectorOntoPlane
    // the actual formula is dividing the dot product by the magnitude of m_zenith squared. However, m_zenith is a unit vector.
    // Also: DO NOT use glm::vec3.length(), it returns the NUMBER of elements, not the magnitude!!! use glm::length(glm::vec3)
    glm::vec3 proj = sundir - m_location->m_zenith * glm::dot(sundir, m_location->m_zenith);
    m_eleangle = m_location->m_anglearcs->add(m_location->m_pos, proj, sundir, m_location->m_radius, defaultcolor, 0.001f);
}
void Location::TrueSun::enablePath24() {
    m_location->addPlanetaryPath(SUN, -0.5, 0.5, 100, TRUESUN3D, defaultcolor, 0.002f);
}
// No need for an update function, that is handled in Location
void Location::TrueSun::disablePath24() {
    m_location->deletePlanetaryPath(TRUESUN3D, SUN);
}
void Location::TrueSun::enableAnalemma() {
    m_location->addPlanetaryPath(SUN, -183.0, 183.0, 366, TRUEANALEMMA3D, defaultcolor, 0.002f);
}
void Location::TrueSun::disableAnalemma() {
    m_location->deletePlanetaryPath(TRUEANALEMMA3D, SUN);
}
double Location::TrueSun::getElevation(bool radians) {
    if (radians) return localsun.lat;
    return localsun.lat * rad2deg;
}
double Location::TrueSun::getAzimuth(bool radians) {
    if (radians) return localsun.lon;
    return localsun.lon * rad2deg;
}

// Generics
void Location::addLocSky(float size, glm::vec4 color) {
    size = m_radius;
    unsigned int index = m_dots->addXYZ(m_pos, color, size);
    m_dotcache.push_back({ m_pos, size, color, index, LOCSKY });
}
void Location::changeLocSky(float size, glm::vec4 color) {
    // size = 0.0f -> use m_radius
    // size < 0.0f -> don't update
    // color = NOT_A_COLOR -> don't update
    if (size == 0.0f) size = m_radius;
    for (auto& d : m_dotcache) {
        if (d.type == LOCSKY) {
            if (color != NO_COLOR) d.changeColor(color);
            if (size > 0.0f)  d.changeSize(size);
            return;
        }
    }
}
void Location::deleteLocSky() {
    for (auto& d : m_dotcache) {
        if (d.type == LOCSKY) {
            d.type = maxuint;
            m_dots->remove(d.index);
            return;
        }
    }
}
void Location::updateLocSky(dotcache& d) {
    d.position = m_pos;
    m_dots->changeXYZ(d.index, m_pos, d.color, d.size);
}
void Location::addTangentPlane(glm::vec4 color) {
    const float radiusfactor = 2.1f;
    unsigned int index = m_planes->addStartUV(
        m_pos + m_zenith * 0.0001f, m_east * m_radius * radiusfactor, m_north * m_radius * radiusfactor, color);
    m_planecache.push_back(
        { m_pos, m_zenith, glm::vec2(m_radius * radiusfactor, m_radius * radiusfactor), color, index, TANGENT });
}
void Location::deleteTangentPlane() {
    for (auto& p : m_planecache) {
        if (p.type == TANGENT) {
            p.type = maxuint;
            m_planes->remove(p.index);
            return;
        }
    }
}
void Location::updateTangentPlane(planecache& p) {
    p.position = m_pos;
    p.direction = m_zenith;
    // Raise the tangent plane slightly off the ground (+m_zenith*0.0001f), so there is no Z fighting in flat geometries
    m_planes->changeStartUV(p.index, p.position + m_zenith * 0.0001f, m_east * p.scale.x, m_north * p.scale.y, p.color);
}
void Location::addMeridianPlane(glm::vec4 color) {
    const float radiusfactor = 2.1f;
    unsigned int index = m_planes->addStartUV(
        m_pos, m_zenith * m_radius * radiusfactor, m_north * m_radius * radiusfactor, color);
    m_planecache.push_back(
        { m_pos, m_east, glm::vec2(m_radius * radiusfactor, m_radius * radiusfactor), color, index, MERIDIAN });
}
void Location::deleteMeridianPlane() {
    for (auto& p : m_planecache) {
        if (p.type == MERIDIAN) {
            p.type = maxuint;
            m_planes->remove(p.index);
            return;
        }
    }
}
void Location::updateMeridianPlane(planecache& p) {
    p.position = m_pos;
    p.direction = m_east;
    // Meridian plane is perpendicular to ground, so no need for Z fighting avoidance
    m_planes->changeStartUV(p.index, p.position, m_zenith * p.scale.x, m_north * p.scale.y, p.color);
}
void Location::addCoords(float length) {
    addUpCoord(length);
    addEastCoord(length);
    addNorthCoord(length);
}
void Location::addUpCoord(float length, float width) {
    glm::vec4 upcolor = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
    unsigned int index = m_arrows->addStartDirLen(m_pos, m_zenith, length, width, upcolor);
    m_arrowcache.push_back({ m_pos,m_zenith,upcolor,length,width,0.0f,0.0f,index,ZENITH, maxuint });
}
// ADD: change, delete
void Location::updateUpCoord(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, m_zenith, ar.length, ar.width, ar.color);
}
void Location::addEastCoord(float length, float width) {
    glm::vec4 eastcolor = glm::vec4(1.0f, 0.1f, 0.1f, 1.0f);
    unsigned int index = m_arrows->addStartDirLen(m_pos, m_east, length, width, eastcolor);
    m_arrowcache.push_back({ m_pos,m_east,eastcolor,length,width,0.0f,0.0f,index,EAST, maxuint });
}
void Location::updateEastCoord(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, m_east, ar.length, ar.width, ar.color);
}
void Location::addNorthCoord(float length, float width) {
    glm::vec4 northcolor = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f);
    unsigned int index = m_arrows->addStartDirLen(m_pos, m_north, length, width, northcolor);
    m_arrowcache.push_back({ m_pos,m_north,northcolor,length,width,0.0f,0.0f,index,NORTH, maxuint });
}
void Location::updateNorthCoord(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, m_north, ar.length, ar.width, ar.color);
}
void Location::addNormal(float length, float width) {
    glm::vec4 normalcolor = LIGHT_BLUE; // glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 dir = m_earth->getNml3D(m_lat, m_lon);
    unsigned int index = m_arrows->addStartDirLen(m_pos, dir, length, width, normalcolor);
    m_arrowcache.push_back({ m_pos,dir,normalcolor,length,width,0.0f,0.0f,index,NORMAL, maxuint });
}
void Location::updateNormalCoord(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, m_earth->getNml3D(m_lat, m_lon), ar.length, ar.width, ar.color);
}
void Location::addObserver(float bearing, glm::vec4 color, float height) {
    if (height == 0.0f) height = m_radius;
    bearing = deg2radf * (bearing - 180.0f);
    glm::vec3 dir = m_earth->getNml3D(m_lat, m_lon);
    m_observer = m_scene->newMinifigs()->addStartDirLen(m_pos, dir, height, height, color, bearing);
}
void Location::changeObserver(float bearing, glm::vec4 color, float height) {
    if (color == NO_COLOR) color = m_scene->newMinifigs()->getDetails(m_observer)->color;
    if (height == 0.0f) height = m_scene->newMinifigs()->getDetails(m_observer)->scale.z;
    bearing = deg2radf * (bearing - 180.0f);
    glm::vec3 dir = m_earth->getNml3D(m_lat, m_lon);
    m_scene->newMinifigs()->changeStartDirLen(m_observer, m_pos, dir, height, height, color, bearing);

}
void Location::addArrow3DEleAzi(unsigned int unique, double ele, double azi, float length, float width, glm::vec4 color) {
    // Note: unique ID allows to construct multiple distinguishable arrows at caller's leasure, unique to this location
    glm::vec3 dir = calcDirEleAzi({ ele, azi, 0.0 }, false);
    unsigned int index = m_arrows->addStartDirLen(m_pos, dir, length, width, color);
    m_arrowcache.push_back({ m_pos,dir,color,length,width,ele,azi,index,AZIELE3D, unique });
}
void Location::deleteArrow3DEleAzi(unsigned int unique) {
    for (auto& a : m_arrowcache) { // Cannot delete m_arrowcache here, that will mess up Update() loop! Why?
        if (a.type == AZIELE3D && a.unique == unique) {
            m_arrows->remove(a.index);
            a.type = maxuint; // Mark as inactive
            return;
        }
    }
}
void Location::changeArrow3DEleAzi(unsigned int unique, double ele, double azi, float length, float width, glm::vec4 color) {
    for (auto& a : m_arrowcache) {
        if (a.type == AZIELE3D && a.unique == unique) {
            glm::vec3 dir = calcDirEleAzi({ ele, azi, 0.0 }, false);
            m_arrows->changeStartDirLen(a.index, m_pos, dir, length, width, color);
            a = { m_pos,dir,color,length,width,ele,azi,a.index,AZIELE3D, unique };
        }
    }
}
void Location::changeArrow3DEleAziColor(unsigned int unique, glm::vec4 color) {
    // NOTE: Should take unique ident !!!
    for (auto& a : m_arrowcache) {
        if (a.type == AZIELE3D) {
            m_arrows->changeStartDirLen(a.index, a.position, a.direction, a.length, a.width, color);
            a = { a.position,a.direction,color,a.length,a.width,a.elevation,a.azimuth,a.index,AZIELE3D, unique };
        }
    }
}
void Location::updateArrow3DEleAzi(arrowcache& ar) {
    glm::vec3 dir = calcDirEleAzi({ ar.elevation, ar.azimuth, 0.0 }, false);
    m_arrows->changeStartDirLen(ar.index, m_pos, dir, ar.length, ar.width, ar.color);
}
void Location::addArrow3DRADec(unsigned int unique, double ra, double dec, glm::vec4 color, float width, float length) {
    // Takes RA & Dec in degrees
    // m_lat & m_lon are in rads. Consider using getLat(true) and getLon(true) instead
    double gsidtime = m_scene->m_astro->getGsid(); // In radians
    double gha = gsidtime - (deg2rad * ra); // calcGeo2Topo() needs GHA, not LHA
    LLH topocentric = m_scene->m_astro->calcGeo2Topo({ deg2rad * dec, gha, 0.0 }, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcDirEleAzi(topocentric, true);
    unsigned int index = m_arrows->addStartDirLen(m_pos, dir, length, width, color);
    m_arrowcache.push_back({ m_pos, dir, color, length, width, dec, ra, index, RADEC3D, unique });
    //char sgha[] = "-xxxhxxmxx.xxs";
    //m_world->GetCelestialOb()->stringRad2HMS(gha, sgha);
    //std::cout << "Greenwich Hour Angle & Declination: " << sgha << " " << dec << "\n";
    //NOTE: Stellarium gives Azimuth in clockwise from North, topocentric is clockwise from South
    //char sAzi[] = "-xxxhxxmxx.xxs";
    //char sEle[] = "-xxxhxxmxx.xxs";
    //m_world->GetCelestialOb()->stringRad2DMS(topocentric.lon, sAzi);
    //m_world->GetCelestialOb()->stringRad2DMS(topocentric.lat, sEle);
    //std::cout << "Alnilam - Orion Epsilon (Azi, Ele): (" << sAzi << ", " << sEle << ")\n";
}
// ADD: deleteArrow3DRADec(), changeArrow3DRADec()
void Location::updateArrow3DRADec(arrowcache& ar) {
    //NOTE: Stellarium gives Azimuth in clockwise from North, topocentric is clockwise from South
    double gsidtime = m_scene->m_astro->getGsid(); // In radians
    double gha = gsidtime - (deg2rad * ar.azimuth);
    LLH topocentric = m_scene->m_astro->calcGeo2Topo({ deg2rad * ar.elevation, gha, 0.0 }, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcDirEleAzi(topocentric, true);
    m_arrows->changeStartDirLen(ar.index, m_pos, dir, ar.length, ar.width, ar.color);
}

//-------------------------------------- Dots --------------------------------------//
void Location::addLocDot(float size, glm::vec4 color) {
    unsigned int index = m_dots->addXYZ(m_pos, color, size);
    m_dotcache.push_back({ m_pos, size, color, index, LOC });
}
void Location::changeLocDot(float size, glm::vec4 color) {
    // size <= 0.0f -> don't change
    // color = NOT_A_COLOR -> don't change
    for (auto& d : m_dotcache) {
        if (d.type == LOC) {
            if (color != NO_COLOR) d.changeColor(color);
            if (size > 0.0f)  d.changeSize(size);
            updateLocDot(d);
            return;
        }
    }
}
void Location::deleteLocDot() {
    for (auto& d : m_dotcache) {
        if (d.type == LOC) {
            d.type = maxuint;
            m_dots->remove(d.index);
            return;
        }
    }
}
void Location::updateLocDot(dotcache& d) {
    m_dots->changeXYZ(d.index, m_pos, d.color, d.size);
}
//-------------------------------------- Arrows --------------------------------------//
void Location::addArrow3DFlatSun(float length, float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present
    if (checkit) for (auto& ar : m_arrowcache) { if (ar.type == FLATSUN3D) return; }
    unsigned int index = m_arrows->addStartDirLen(m_pos, getFlatSunDir(), length, width, color);
    m_arrowcache.push_back({ m_pos,getFlatSunDir(),color,length,width,0,0,index,FLATSUN3D, maxuint });
}
// ADD: change, delete
void Location::updateArrow3DFlatSun(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, getFlatSunDir(), ar.length, ar.width, ar.color);
}
void Location::addArrow3DTrueMoon(float length, float width, glm::vec4 color, bool checkit) {
    if (checkit) { // Caller can specify to skip if this type of arrow is already present
        for (auto& ar : m_arrowcache) {
            if (ar.type == TRUEMOON3D) return;
        }
    }
    LLH moon = m_earth->getMoon();    // sun { rad sunDec, rad sunHour, AU sunDist }
    CAA2DCoordinate localmoon = CAACoordinateTransformation::Equatorial2Horizontal(
        rad2hrs * (-moon.lon + m_lon), rad2deg * moon.lat, rad2deg * m_lat);
    glm::vec3 dir = calcDirEleAzi({ localmoon.Y, localmoon.X, 0.0 }, false);
    unsigned int index = m_arrows->addStartDirLen(m_pos, dir, length, width, color);
    m_arrowcache.push_back({ m_pos,dir,color,length,width,localmoon.Y,localmoon.X,index,TRUEMOON3D, maxuint });
}
void Location::updateArrow3DTrueMoon(arrowcache& ar) {
    LLH moon = m_earth->getMoon();    // sun { rad sunDec, rad sunHour, AU sunDist }
    CAA2DCoordinate localmoon = CAACoordinateTransformation::Equatorial2Horizontal(
        rad2hrs * (-moon.lon + m_lon), rad2deg * moon.lat, rad2deg * m_lat);
    glm::vec3 dir = calcDirEleAzi({ localmoon.Y, localmoon.X, 0.0 }, false);
    m_arrows->changeStartDirLen(ar.index, m_pos, dir, ar.length, 0.003f, ar.color);
}

void Location::updateArrow3DTruePlanet(arrowcache& ar) {
    LLH pos = m_scene->m_astro->getDecGHA(ar.unique);
    LLH topo = m_scene->m_astro->calcGeo2Topo(pos, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcDirEleAzi(topo, true);
    m_arrows->changeStartDirLen(ar.index, m_pos, dir, ar.length, 0.003f, ar.color);
}
// ADD: change, delete
void Location::addArrow3DTruePlanet(unsigned int planet, float length, glm::vec4 color, bool checkit) {
    if (checkit) { // Caller can specify to skip if this type of arrow is already present
        for (auto& ar : m_arrowcache) {
            if (ar.unique == planet) return;
        }
    }
    color = getPlanetColor(planet, color);
    m_scene->m_astro->enablePlanet(planet);
    LLH pos = m_scene->m_astro->getDecGHA(planet);
    LLH topo = m_scene->m_astro->calcGeo2Topo(pos, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcDirEleAzi(topo, true);
    unsigned int index = m_arrows->addStartDirLen(m_pos, dir, length, 0.003f, color);
    m_arrowcache.push_back({ m_pos, dir, color, length, 0.003f, topo.lat, topo.lon, index, TRUEPLANET3D, planet });
}
//-------------------------------------- Lines --------------------------------------//
void Location::addLine3DFlatSun(float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present
    if (checkit) for (auto& l : m_cylindercache) { if (l.type == FLATSUN3D) return; }
    glm::vec3 flatsun = m_earth->getSubsolarXYZ();
    unsigned int index = m_cylinders->addStartEnd(m_pos, flatsun, width, color);
    m_cylindercache.push_back({ index,FLATSUN3D, m_pos, flatsun, width, color });
}
// ADD: change, delete
void Location::updateLine3DFlatSun(cylindercache& l) {
    // Caller can specify to skip if this type of arrow is already present
    l.start = m_pos;
    l.end = m_earth->getSubsolarXYZ();
    m_cylinders->changeStartEnd(l.index, m_pos, l.end, l.width, l.color);
}

//-------------------------------------- Paths --------------------------------------//
// To facilitate deleting paths, they should return an id of some sort. Done! No??
// Additionally the polycache must support gap elimination (or mark entries as void). Done by using tightvec!
void Location::addPlanetaryPath(unsigned int planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, glm::vec4 color, float width) {
    CelestialPath* planetCP = m_scene->m_astro->getCelestialPath(planet, startoffset, endoffset, steps, ECGEO);
    color = getPlanetColor(planet, color);
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    unsigned int index = m_polycache.store({ path, planetCP, width, type, color, 0.0, 0.0, planet });
    m_polycache[index].index = index;
    updatePlanetaryPath(m_polycache[index]);
}
// changePlanetaryPath() - What should it change exactly? primitive properties or celestial ones?
// deletePlanetaryPath() - Would need to specify (planet, start, end, steps), or addPlanetaryPath should return an index, or a second unique field is needed.
void Location::updatePlanetaryPath(polycache& pa) {
    pa.path->clearPoints();
    for (auto const& pt : pa.planetCP->entries) {
        LLH topo = m_scene->m_astro->calcGeo2Topo({ pt.geodec, pt.geogha, 0.0 }, { m_lat, m_lon, 0.0 });
        glm::vec3 dir = calcDirEleAzi(topo, true);
        pa.path->addPoint(m_pos + dir * m_radius);
    }
    pa.path->generate();
}
void Location::deletePlanetaryPath(unsigned int type, unsigned int unique) {
    polycache* path = nullptr;
    for (auto& p : m_polycache.m_Elements) {
        if (p.type == type && p.unique == unique) {
            path = &p;
            break;
        }
    }
    if (path != nullptr) {
        delete path->path; // PolyCurve
        m_scene->m_astro->removeCelestialPath(path->planetCP); // CelestialPath
        unsigned int index = path->index;
        m_polycache.remove(index); // Cache
    }
    else std::cout << "WARNING: Location::deletePlanetaryPath() called but no path was found with type = " << type << "!\n";
}
// ADD: change, delete
void Location::addPlanetTruePath24(unsigned int planet, glm::vec4 color, float width) {
    // Planetary path across sky in 24 hrs, roughly follows earth rotation of course
    addPlanetaryPath(planet, -0.5, 0.5, 100, TRUEPLANET3D, color, width);
}
// ADD: change(color, width)
void Location::deletePlanetTruePath24(unsigned int planet) {
    deletePlanetaryPath(TRUEPLANET3D, planet);
}
void Location::addPlanetTruePathSidYear(unsigned int planet, glm::vec4 color, float width) {
    // Planetary path across fixed star background, over approximately 1 year
    double bracket = sidyearparms[planet - SUN].bracket;
    double stepsize = sidyearparms[planet - SUN].stepsize;
    addPlanetaryPath(planet, -bracket * sidereald, bracket * sidereald, 2 * (unsigned int)(bracket / stepsize), SIDPLANET3D, color, width);
}
// ADD: change
void Location::deletePlanetTruePathSidYear(unsigned int planet) {
    deletePlanetaryPath(SIDPLANET3D, planet);
}
void Location::addPath3DFlatSun(glm::vec4 color, float width) {
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    doPath3DFlatSun(path);
    m_polycache.store({ path, nullptr, width, FLATSUN3D, color, 0.0, 0.0, 0 });
}
void Location::updatePath3DFlatSun(PolyCurve* path, glm::vec4 color, float width) {
    path->clearPoints();
    doPath3DFlatSun(path);
    return;
}
void Location::doPath3DFlatSun(PolyCurve* path) {
    // Change to smarter date/time based start and end
    // For now, just sweep 1 JD centered on current epoch
    //float height = 0.0/earthradius; // Observer height in km to Earth radii 
    double myJD = m_scene->m_astro->getJD() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        path->addPoint(m_pos + getFlatSunDir(fday) * m_radius);
    }
    path->generate();
}

void Location::addPath3DTrueMoon(glm::vec4 color, float width) {
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    doPath3DTrueMoon(path);
    m_polycache.store({ path, nullptr, width, TRUEMOON3D, color, 0.0, 0.0, 0 });
}
void Location::updatePath3DTrueMoon(polycache p) {
    p.path->clearPoints();
    doPath3DTrueMoon(p.path);
    return;
}
void Location::doPath3DTrueMoon(PolyCurve* path) {
    // Change to smarter date/time based start and end
    // For now, just sweep 1 JD centered on current epoch
    double myJD = m_scene->m_astro->getJD() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        LLH moon = m_earth->getMoon(fday);
        CAA2DCoordinate localmoon = CAACoordinateTransformation::Equatorial2Horizontal(
            rad2hrs * (-moon.lon + m_lon), rad2deg * moon.lat, rad2deg * m_lat);
        glm::vec3 dir = calcDirEleAzi({ localmoon.Y, localmoon.X, 0.0 }, false);
        path->addPoint(m_pos + dir * m_radius);
    }
    path->generate();
}

// Stars are likely to be added in multitudes, implement a unique id passed by caller (unsigned int)
void Location::addPath3DRADec(unsigned int unique, double ra, double dec, glm::vec4 color, float width) {
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    doPath3DRADec(ra, dec, path);
    m_polycache.store({ path, nullptr, width, RADEC3D, color, dec, ra, unique });
}
void Location::updatePath3DRADec(polycache& pa) {
    pa.path->clearPoints();
    doPath3DRADec(pa.azimuth, pa.elevation, pa.path);
    return;
}
void Location::doPath3DRADec(double ra, double dec, PolyCurve* path) {
    // Change to smarter date/time based start and end
    // For now, just sweep 1 JD centered on current epoch
    double myJD = m_scene->m_astro->getJD() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        glm::vec3 point = m_pos + calcDirRADec(ra, dec, fday) * m_radius;
        path->addPoint(point);
    }
    path->generate();
}


// -------------
//  SolarSystem
// -------------
SolarSystem::SolarSystem(Scene* scene, bool geocentric) : m_scene(scene), m_geocentric(geocentric) {
    //std::cout << "SolarSystem::SolarSystem()\n";
    m_astro = m_scene->m_astro;
    m_jd = m_astro->getJD();
    glm::vec3 earthpos = CalcEarth() + m_sunpos;
    m_sunpos = m_geocentric ? -earthpos : glm::vec3(0.0f);
    m_earthpos = m_geocentric ? glm::vec3(0.0f) : earthpos;

    // Calculate positions and create celestial objects
    SunPos(false);
    EarthPos(false);
    for (unsigned int p = MERCURY; p <= EARTH; p++) {
        PlanetPos(p, false);
        PlanetOrbit(p, false);
    }
}
SolarSystem::~SolarSystem() {
    // Clear orbits here
}
void SolarSystem::Update() {
    // Check if GUI changed: bool geocentric, bool orbits, bool trails, int traillen
    if (geocentric != m_geocentric) changeCentricity();
    if (orbits != m_orbits) changeOrbits();
    if (trails != m_trails) changeTrails();
    if (traillen != m_traillen) changeTraillen();
    m_jd = m_astro->getJD();
    SunPos(true);
    EarthPos(true);
    for (unsigned int p = MERCURY; p <= EARTH; p++) {
        PlanetPos(p, true);
    }
    UpdateDistLines();
    Draw();
}
void SolarSystem::Draw() {
    if (m_orbits) {
        // Note that while orbits are not being sent to GPU and rendered, CelestialPaths are still being updated.
        for (unsigned int p = MERCURY; p <= EARTH; p++) { // EARTH is at the end of the planet list in enum
            PlanetOrbit(p, true);
        }
    }
}
void SolarSystem::addTrails(int traillength) {
    // NOTE: traillen must be positive, but ImGUI just supports signed ints in sliders
    // Geocentric and Heliocentric require different dynamic trail lengths
    //  and densities/spacings. Maybe make an AdjustTrails() method for switching.
    // Also, naming is inconsistent, addTrails() and changeTrails() are better.
    // Further, enabling trails per planet is preferable, use ALL to do all of them.
    m_traillen = abs(traillength); // Force traillen positive. It is int to suit ImGUI::SliderInt()
    for (unsigned int p = MERCURY; p <= EARTH; p++) {
        if (m_PlanetTrail[p] == nullptr) m_PlanetTrail[p] = new ParticleTrail(m_scene, m_traillen, m_planetinfos[p].color);
    }
    m_trails = true;
}
void SolarSystem::clearTrails() {
    for (unsigned int p = MERCURY; p <= EARTH; p++) {
        if (m_PlanetTrail[p] != nullptr) m_PlanetTrail[p]->clear();
    }
}
void SolarSystem::addOrbits() {
    m_orbits = true;
    orbits = true;
    // Calculate fixed orbital paths, they change very little over centuries - if Heliocentric !!!
    for (unsigned int p = MERCURY; p <= EARTH; p++) {
        PlanetOrbit(p, false);
    }
}
glm::vec3 SolarSystem::CalcPlanet(unsigned int planet, double jd) {
    if (jd == 0.0) jd = m_jd;
    const double lon = m_astro->getEcLon(planet, jd);  // Radians
    const double lat = m_astro->getEcLat(planet, jd);  // Radians
    const double dst = m_astro->getRadius(planet, jd, /* km */ false);  // AU
    return Ecliptic2Cartesian(lat, lon, dst);
}
void SolarSystem::PlanetPos(unsigned int planet, bool update) {
    // NOTE: Optionally push to trail depending on distance from last frame. !!!
    glm::vec3 pos = CalcPlanet(planet) + m_sunpos;
    if (!update) m_PlanetDot[planet] = m_scene->getDotsOb()->addXYZ(pos, m_planetinfos[planet].color, planetdot);
    if (update) m_scene->getDotsOb()->changeXYZ(m_PlanetDot[planet], pos, m_planetinfos[planet].color, planetdot);
    if (m_PlanetTrail[planet] != nullptr && m_trails) m_PlanetTrail[planet]->push(pos);
}
void SolarSystem::PlanetOrbit(unsigned int planet, bool update) {
    //std::cout << "SolarSystem::PlanetOrbit(planet = " << planet << ", update = " << update << ") - m_PlanetOrbit, m_PlanetPath: " << m_PlanetOrbit[planet] << ", " << m_PlanetPath[planet] << "\n";
    // Simpler to just detect if m_PlanetOrbit[] and m_PlanetPath are nullptr, rather than passing bool update correctly every time. !!!
    //  Same for PlanetPos() !!!
    if (!update) {
        double siderealyear = m_planetinfos[planet - SUN].sidyear; // Earth days
        glm::vec4 color = m_planetinfos[planet - SUN].color;
        color.a = 0.4f;
        m_PlanetPath[planet - SUN] = m_scene->newPolyCurve(color, solsyspathwidth);
        m_PlanetOrbit[planet - SUN] = m_scene->m_astro->getCelestialPath(planet, -0.5 * siderealyear, 0.5 * siderealyear, orbitsteps, EC);
    }
    if (update) m_PlanetPath[planet - SUN]->clearPoints();
    for (auto const& pt : m_PlanetOrbit[planet - SUN]->entries) {
        if (planet != SUN) m_PlanetPath[planet - SUN]->addPoint(Ecliptic2Cartesian(pt.eclat, pt.eclon, pt.ecdst) + m_sunpos);
        else m_PlanetPath[planet - SUN]->addPoint(Ecliptic2Cartesian(pt.eclat, pt.eclon, pt.ecdst));
    }
    m_PlanetPath[planet]->generate();
    m_PlanetPath[planet]->draw();  // Needed? Yes!
}
void SolarSystem::SunPos(bool update) {
    m_sunpos = m_geocentric ? CalcSun() : glm::vec3(0.0f);

    if (!update) m_SunDot = m_scene->getDotsOb()->addXYZ(m_sunpos, SUNCOLOR, planetdot);
    if (update) m_scene->getDotsOb()->changeXYZ(m_SunDot, m_sunpos, SUNCOLOR, planetdot);
    if (m_PlanetTrail[SUN] != nullptr && m_trails) m_PlanetTrail[SUN]->push(m_sunpos);
}
void SolarSystem::SunOrbit(bool update) {
    m_SunPath = m_scene->newPolyCurve(SUNCOLOR, solsyspathwidth);
    double siderealyear = 366; // Sun days
    for (double jd = m_astro->getJD() - 0.5 * siderealyear; jd < m_astro->getJD() + 0.5 * siderealyear; jd += siderealyear / 360) {
        glm::vec3 pos = CalcSun(jd);
        m_SunPath->addPoint(pos);
    }
    m_SunPath->generate();
    m_SunPath->draw();
}
glm::vec3 SolarSystem::CalcSun(double jd) {
    if (jd == 0.0) jd = m_jd;
    const double lon = m_astro->EcLonEarth(jd);
    const double lat = m_astro->EcLatEarth(jd);
    const double dst = m_astro->EcDstEarth(jd);
    return -Ecliptic2Cartesian(lat,lon,dst);
}
void SolarSystem::EarthPos(bool update) {
    m_earthpos = m_geocentric ? glm::vec3(0.0f) : CalcEarth() + m_sunpos;
    if (!update) m_EarthDot = m_scene->getDotsOb()->addXYZ(m_earthpos, EARTHCOLOR, planetdot);
    if (update) m_scene->getDotsOb()->changeXYZ(m_EarthDot, m_earthpos, EARTHCOLOR, planetdot);
    //pos += m_sunpos;
    if (m_EarthTrail != nullptr && m_trails) m_EarthTrail->push(m_earthpos);
}
glm::vec3 SolarSystem::CalcEarth(double jd) {
    if (jd == 0.0) jd = m_jd;
    const double lon = m_astro->EcLonEarth(jd);
    const double lat = m_astro->EcLatEarth(jd);
    const double dst = m_astro->EcDstEarth(jd);
    return Ecliptic2Cartesian(lat, lon, dst);
}
glm::vec3 SolarSystem::Ecliptic2Cartesian(double Brad, double Lrad, double dst) {
    glm::vec3 pos = glm::vec3(0.0f);
    const double cosB = cos(Brad);
    pos.x = (float)(dst * cosB * cos(Lrad));
    pos.y = (float)(dst * cosB * sin(Lrad));
    pos.z = (float)(dst * sin(Brad));
    return pos;
}
glm::vec3 SolarSystem::GetPlanetPos(unsigned int planet) {  // Optional JD ?
    if (planet == SUN) return m_sunpos;
    else if (planet == EARTH) return m_earthpos;
    else if (planet > SUN && planet < EARTH) return CalcPlanet(planet);
    else std::cout << "SolarSystem::GetPlanetPos() called with invalid planet enum: " << planet << ", returning glm::vec3(0.0f)!\n";
    return glm::vec3(0.0f);
}
void SolarSystem::AddDistLine(unsigned int planet1, unsigned int planet2, glm::vec4 color, float width) {
    if (m_cylinders == nullptr) m_cylinders = m_scene->getCylindersOb();
    unsigned int index = m_cylinders->addStartEnd(GetPlanetPos(planet1), GetPlanetPos(planet2), width, color);
    m_distlines.store({ index, planet1, planet2, color, width });
}
void SolarSystem::UpdateDistLines() {
    for (auto& dl : m_distlines.m_Elements) {
        m_cylinders->changeStartEnd(dl.index, GetPlanetPos(dl.planet1), GetPlanetPos(dl.planet2), dl.width, dl.color);    
    }
}
void SolarSystem::changeCentricity() {
    m_geocentric = geocentric;
}
void SolarSystem::changeOrbits() {
    if (m_orbits == orbits) return; // Should not happen, might log to console
    if (orbits) { // Orbits being enabled
        addOrbits();
        m_orbits = true;
    }
    else {
        // remove orbits, or simply pause drawing them?
        m_orbits = false;
    }
}
void SolarSystem::changeTrails() {
    if (trails) {
        addTrails(m_traillen);
    }
    m_trails = trails;
    // NOTE: This also causes the trails to stop fading
}
void SolarSystem::changeTraillen() {
    if (traillen == m_traillen) return; // called in error
    if (traillen < m_traillen) {
        for (unsigned int p = MERCURY; p <= EARTH; p++) {
            if (m_PlanetTrail[p] != nullptr) m_PlanetTrail[p]->trim(traillen);
        }
    }
    else {
        for (unsigned int p = MERCURY; p <= EARTH; p++) {
            if (m_PlanetTrail[p] != nullptr) m_PlanetTrail[p]->expand(traillen);
        }
    }
    m_traillen = traillen;
}


// -----------
//  SkySphere
// -----------
SkySphere::SkySphere(Scene* scene, unsigned int meshU, unsigned int meshV) : m_scene(scene), m_meshU(meshU), m_meshV(meshV) {
    m_verts.reserve(((size_t)m_meshU + 1) * ((size_t)m_meshV + 1));
    m_tris.reserve((size_t)(m_meshU * (size_t)m_meshV * (size_t)sizeof(Tri)));
    m_dots = m_scene->getDotsOb();
    m_dotcache.reserve(10000);
    genGeom();
    vbl = new VertexBufferLayout;
    vbl->Push<float>(3);   // Vertex pos
    //vbl->Push<float>(3);   // Vertex normal NS (for insolation only)
    //vbl->Push<float>(3);   // Vertex normal actual
    vbl->Push<float>(2);   // Texture coord
    //vbl->Push<float>(4);   // Color (for overlay)
    va = new VertexArray;
    vb = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(EarthV));
    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);
    va->AddBuffer(*vb, *vbl, true);
    //std::cout << "Object1 (vb,ib,va): " << vb.GetRenderID() << "," << ib.GetRenderID() << "," << va.GetRenderID() << std::endl;
    //std::string shadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\skysphere.shader";
    //shdr = new Shader(shadersrc);
    shdr = m_scene->m_app->getShaderLib()->getShader(SKY_SPHERE_SHADER);
    // NOTE: Textures should be pre-flipped in Y, as SOIL2 soils itself when trying to flip large images
    // NOTE: Sky texture should be flipped in X, as it is viewed from the inside of the sphere !!!
    const std::string texfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap-gimp_8k (8192x4096 celestial coords).png";
    tex = new Texture(texfile, GL_TEXTURE2);
    //m_world->SetSkySphereOb(this);
}
SkySphere::~SkySphere() {
    delete tex;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
}
void SkySphere::Draw() {
    //return;
    glm::mat4 proj = m_scene->w_camera->getProjMat();
    //glm::mat4 proj = glm::perspective(glm::radians(30.0f), (float)m_world->w_width / (float)m_world->w_height, 1.0f, 1000.0f);
    //std::cout << "Drawing SkySphere\n";
    // Shader setup
    UpdateTime(0.0);
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->getSkyViewMat());
    shdr->SetUniformMatrix4f("projection", proj); // m_world->w_camera->GetProjMat());
    shdr->SetUniform1i("skytexture", tex->GetTextureSlot());
    shdr->SetUniform1f("gsid", m_gsid);
    va->Bind();
    ib->Bind();
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glDisable(GL_CULL_FACE); // It is irritating that Earth disappears when seen from back-/in-side
    glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0);
    glEnable(GL_CULL_FACE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Cleanup
    va->Unbind();
    ib->Unbind();
    shdr->Unbind();
    return;
}
void SkySphere::UpdateTime(double jd) {
    // HACK to keep steady background when displaying SolarSystem, check that it is orientated correctly !!!
    // Problem with this hack is, it will now create a SolarSystem and move on.
    //if (m_scene->getSolsysOb() != nullptr) return;
    m_gsid = (float)(-m_scene->m_astro->getGsid(jd));
    // Update Dot cache with rotation
    for (auto& d : m_dotcache) {
        updateDotDecRA(d);
    }
}
//void SkySphere::addPlanetDot() {
//
//}

void SkySphere::addStars() {
    unsigned int i = 0;
    for (auto& s : stellarobjects) {
        addDotDecRA(++i, s.dec, s.ra, /*radians*/ false);

    }
}
void SkySphere::addDotDecRA(unsigned int unique, double dec, double ra, bool radians) {
    // default is to deliver position in radians, else in degrees (thus, not hours for RA)
    if (!radians) {
        dec *= deg2rad;
        ra *= deg2rad;
    }
    double ra2;
    if (ra < pi) ra2 = -ra;
    else ra2 = tau - ra;
    glm::vec3 pos = 0.5f * getLoc3D_NS(dec, m_gsid - ra2);
    unsigned int index = m_dots->addXYZ(pos, LIGHT_GREEN, 0.3f);
    m_dotcache.push_back({ unique, LIGHT_GREEN, dec, ra, 0.0f, 0.3f, index });
    return; // (unsigned int)m_dotcache.size() - 1;
}
void SkySphere::updateDotDecRA(dotcache dot) {
    double ra = dot.lon;
    if (ra < pi) ra = -ra;
    else ra = tau - ra;
    glm::vec3 pos = getLoc3D_NS(dot.lat, m_gsid - ra);
    m_dots->changeXYZ(dot.index, pos, dot.color, dot.size);
    return; // (unsigned int)m_dotcache.size() - 1;
}
glm::vec3 SkySphere::getLoc3D_NS(double lat, double lon) {
    float w = (float)(cos(lat) * m_Radius);
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    float z = (float)(sin(lat) * m_Radius);
    return glm::vec3(x, y, z);
}
void SkySphere::genGeom() {
    double lat, lon;
    for (unsigned int v = 0; v <= m_meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
        lat = (pi * v / m_meshV) - pi2;
        for (unsigned int u = 0; u <= m_meshU; u++) {
            lon = (tau * u / m_meshU) - pi;
            glm::vec3 loc = getLoc3D_NS(lat, lon);     // position of vertex
            //glm::vec3 nms = getNml3D_NS(lat, lon);  // Insolation normal, always NS (or G8?)
            //glm::vec3 nml = getNml3D(lat, lon);     // Geometry normal, for camera lighting
            //m_verts.push_back({ loc, nms, nml, glm::vec2(u / (float)m_meshU, v / (float)m_meshV), BLACK });
            m_verts.push_back({ loc, glm::vec2(u / (float)m_meshU, v / (float)m_meshV) });
            if (u < m_meshU && v < m_meshV) {
                m_tris.push_back({ (m_meshU + 1) * v + u , (m_meshU + 1) * (v + 1) + u, (m_meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (m_meshU + 1) * (v + 1) + 1 + u, (m_meshU + 1) * v + u + 1, (m_meshU + 1) * v + u });
            }
        }
    }
}