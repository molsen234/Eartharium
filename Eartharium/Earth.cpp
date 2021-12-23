
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
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->GetViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->GetProjMat());
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
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->GetViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->GetProjMat());
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
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->GetViewMat());
    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->GetProjMat());
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
            delete p.path;
        }
        m_polycache.clear();
    }
    delete m_sunterm2;
    delete m_sunterm1;
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

    if (m_JD != m_scene->m_celestOb->getJD()) {
        m_JD = m_scene->m_celestOb->getJD();
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
            if (p.type == LATITUDE) updateLatitudeCurve(p);
            if (p.type == LONGITUDE) updateLongitudeCurve(p);
            if (p.type == GREATARC) updateGreatArc(p);
            if (p.type == LERPARC) updateLerpArc(p);
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
                m_dots->UpdateXYZ(m_dotcache[i].index, pos, m_dotcache[i].color, m_dotcache[i].size);
            }
        }
        if (m_sunpole != maxuint) updateSubsolarPole();
        if (m_suncone != maxuint) updateSubsolarCone_NS();

        // if timedirty or morphdirty
        if (m_sunterm1 != nullptr && m_sunterm2 != nullptr) {
            m_sunterm1->ClearPoints();
            m_sunterm2->ClearPoints();
            updateTerminatorTrueSun();
        }
        if (m_moonterm1 != nullptr && m_moonterm2 != nullptr) {
            m_moonterm1->ClearPoints();
            m_moonterm2->ClearPoints();
            updateTerminatorTrueMoon();
        }
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
        smshdr->SetUniformMatrix4f("view", m_scene->w_camera->GetViewMat());
        smshdr->SetUniformMatrix4f("projection", m_scene->w_camera->GetProjMat());
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
        sbshdr->SetUniformMatrix4f("view", m_scene->w_camera->GetViewMat());
        sbshdr->SetUniformMatrix4f("projection", m_scene->w_camera->GetProjMat());

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
        glm::mat4 pv = m_scene->w_camera->GetProjMat() * m_scene->w_camera->GetViewMat();
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
        po.path->Draw();
    }
    if (m_sunob != nullptr) m_sunob->Draw();
    if (m_moonob != nullptr) m_moonob->Draw();
    if (m_sunterm1 != nullptr) m_sunterm1->Draw();
    if (m_sunterm2 != nullptr) m_sunterm2->Draw();
    if (m_moonterm1 != nullptr) m_moonterm1->Draw();
    if (m_moonterm2 != nullptr) m_moonterm2->Draw();
    for (auto& lg : locgroups) {
        for (auto l : lg->locations) {
            l->Draw();
        }
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
    /// <param name="rLat">Latitude given in 0 to pi radians</param>
    /// <param name="rLon">Longitude given in -pi to pi radians from Greenwich meridian (east of south)</param>
    /// <param name="height">Optional height above geoid surface in km</param>
    /// <returns>Cartesian coordinates glm::vec3 in world space units for currently active geoid geometry</returns>
    if (m_Mode.length() == 2) return getLoc3D_XX(m_Mode, rLat, rLon, height);
    if (m_Mode.length() != 4) {
        std::cout << "Earth::getLoc3D(): Invalid mode: " << m_Mode << " (must be 2 or 4 characters)" << std::endl;
        return glm::vec3(0.0f);
    }
    // Mode is 4 long so morph
    if (m_Mode == "A2NS") return getLoc3D_A2NS(rLat, rLon, height);
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
        std::cout << "Earth::getNml3D(): Invalid mode: " << m_Mode << " (must be 2 or 4 characters)" << std::endl;
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
glm::vec3 Earth::getLoc3D_A2NS(double rLat, double rLon, double height) {
    float w = (float)(cos(rLat) * (m_Radius + height) * (1.0 - m_param));
    float x = (float)(cos(rLon) * w);
    float y = (float)(sin(rLon) * w);
    float z = (float)(sin(rLat) * (m_Radius + height) * m_param);
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
    //float y = lon * m_Radius;
    //float z = rLat * m_Radius;
    return glm::vec3(height, rLon * m_Radius, rLat * m_Radius);
}
glm::vec3 Earth::getNml3D_ER(double rLat, double rLon, double height) {
    return glm::vec3(1.0f, 0.0f, 0.0f);
}
glm::vec3 Earth::getLoc3D_RC(double rLat, double rLon, double height) {
    double w = (m_Radius + height);
    //float x = cos(rLon) * w;
    //float y = sin(rLon) * w;
    //float z = rLat * m_Radius;
    return glm::vec3(cos(rLon) * w, sin(rLon) * w, rLat * m_Radius);
}
glm::vec3 Earth::getNml3D_RC(double rLat, double rLon, double height) {
    double w = (m_Radius + height);
    //float x = cos(rLon) * w;
    //float y = sin(rLon) * w;
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
    return m_scene->m_celestOb->getDecGHA(SUN, jd);
}
LLH Earth::getPlanet(unsigned int planet, double jd) {
    return m_scene->m_celestOb->getDecGHA(planet, jd);
}
void Earth::CalcMoon() {
    double currentJD = m_scene->m_celestOb->getJD();
    if (m_moonJD == currentJD) return;
    double elon = CAAMoon::EclipticLongitude(currentJD);  // lambda
    double elat = CAAMoon::EclipticLatitude(currentJD);   // beta
    double Epsilon = CAANutation::TrueObliquityOfEcliptic(currentJD);
    CAA2DCoordinate equa = CAACoordinateTransformation::Ecliptic2Equatorial(elon, elat, Epsilon);
    m_moonDist = CAAMoon::RadiusVector(m_JD); // RadiusVector() returns in km
    m_moonRA = hrs2rad * equa.X;
    m_moonDec = deg2rad * equa.Y;
    m_moonHour = m_moonRA - m_scene->m_celestOb->getGsid(); // Gsid returns in radians now, so don't convert!
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
    unsigned int index = m_dots->FromXYZ(pos, color, size);
    m_dotcache.push_back({ color, lat, lon, height, size, index });
    return (unsigned int)m_dotcache.size() - 1;
}
// ADD: changeDot()
void Earth::deleteDot(unsigned int index) {
    m_dots->Delete(m_dotcache[index].index);
    m_dotcache[index].index = maxuint;
}
// Arrows
void Earth::addArrow3DTrueSun(float length, float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present, or risk adding a duplicate
    // NOTE: Arrows start at center of Earth. Is this suitable for all Geometries? !!!
    if (checkit) for (auto& ar : m_arrowcache) { if (ar.type == TRUESUN3D) return; }
    glm::vec3 dir = calcRADec2Dir(getSun());
    unsigned int index = m_arrows->FromStartDirLen(glm::vec3(0.0f), dir, length, width, color);
    m_arrowcache.push_back({ glm::vec3(0.0f), dir, color, length, width, 0.0, 0.0, index, TRUESUN3D, maxuint });
}
void Earth::deleteArrow3DTrueSun() {
    for (auto& a : m_arrowcache) {
        if (a.type == TRUESUN3D) {
            m_arrows->Delete(a.index);
            a.type = maxuint;
            //return;  // NOTE: Should I let the loop run, in case I have added more than one of this type? !!!
        }
    }
}
void Earth::changeArrow3DTrueSun(float length, float width, glm::vec4 color) {
    for (auto& a : m_arrowcache) {
        if (a.type == TRUESUN3D) {
            m_arrows->UpdateStartDirLen(a.index, a.position, a.direction, length, width, color);
            a = { a.position,a.direction,color,length,width,a.elevation,a.azimuth,a.index,TRUESUN3D, maxuint };
        }
    }
}
void Earth::updateArrow3DTrueSun(arrowcache& ar) {
    m_arrows->UpdateStartDirLen(ar.index, glm::vec3(0.0f), calcRADec2Dir(getSun()), ar.length, ar.width, ar.color);
}
void Earth::addLunarUmbraCone() {
    const double radiusdiff = moonradius - sunradius;
    LLH sun = m_scene->m_celestOb->getDecGHA(SUN);
    glm::vec3 sunpos = getLoc3D_NS(sun.lat, sun.lon, sun.dst);
    glm::vec3 moonpos = getLoc3D_NS(m_moonDec, m_moonHour, m_moonDist);
    glm::vec3 conedir = sunpos - moonpos;
    double slope = radiusdiff / glm::length(conedir);
    float clength = (float) -sunradius * glm::length(conedir) / (float)radiusdiff;
    std::cout << "Conelength: " << clength << "\n";
    glm::vec3 conetip = glm::normalize(conedir);
    conetip = conetip * clength + moonpos;
    unsigned int index = m_dots->FromXYZ(moonpos/(float)(earthradius*10.0), WHITE, 0.1f);
}
glm::vec3 Earth::getSubsolarXYZ(double jd) {
    if (jd == m_JD) return flatSun;
    LLH mysun = m_scene->m_celestOb->getDecGHA(SUN, jd); // subsolar.lon is west of south, 0 to tau
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
    m_sunpole = m_cylinders->FromStartEnd(solarGP, flatSun, width, LIGHT_YELLOW);
}
void Earth::deleteSubsolarPole() {
    if (m_sunpole == maxuint) return; // Bail if there is no sunpole
    m_cylinders->Delete(m_sunpole);
    m_sunpole = maxuint;
}
void Earth::updateSubsolarPole() {
    if (m_sunpole == maxuint) return; // Bail if there is no sunpole
    //std::cout << "Earth::updateSubsolarPole(): " << m_sunpole << "\n";
    m_cylinders->UpdateStartEnd(m_sunpole, solarGP, flatSun, 0.003f, LIGHT_YELLOW);
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
    m_suncone = m_scene->getViewConesOb()->FromStartDirLen(flatSun, glm::normalize(flatSun), (float)h - 0.001f, (float)rc + 0.002f, color);
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
    m_scene->getViewConesOb()->UpdateStartDirLen(m_suncone, flatSun, glm::normalize(flatSun), (float)h - 0.001f, (float)rc + 0.002f, NO_COLOR);
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
    m_moonob->Update(getLoc3D(m_moonDec, m_moonHour, m_flatsunheight / earthradius));
    std::cout << m_moonDist << "\n";
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
    m_moonob->Update(getLoc3D(m_moonDec, m_moonHour, m_flatsunheight / earthradius));
}
void Earth::addViewConeXYZ_NS(glm::vec3 pos, glm::vec4 color) {
    // https://stackoverflow.com/questions/64881275/satellite-on-orbit-create-the-tangent-cone-from-point-to-sphere
    // (see also https://en.wikipedia.org/wiki/Tangent_lines_to_circles#Outer_tangent but not as useful)
    double rE = 1.0f;  //self.NSscale # Radius of Earth
    double d = glm::length(pos);
    double l = sqrt(d * d - rE * rE);
    double t = asin(rE / d);
    double h = l * cos(t);
    double rc = l * sin(t);
    m_scene->getViewConesOb()->FromStartDirLen(pos, glm::normalize(pos), (float)h - 0.001f, (float)rc + 0.001f, color);
}
//void Earth::addViewConeLLH_NS(LLH) {
//    def addViewCone_NS(self, lat, lon, height, mode = 'LLH', rad = False) :
//        # mode is LLH or XYZ
//        # LLH: lat = lat lon = lon height = height
//        # XYZ : lat = x, lon = y, height = z
//        # https ://stackoverflow.com/questions/64881275/satellite-on-orbit-create-the-tangent-cone-from-point-to-sphere
//# (see also https://en.wikipedia.org/wiki/Tangent_lines_to_circles#Outer_tangent but not as useful)
//    rE = self.NSscale # Radius of Earth
//    locE = mathutils.Vector((0.0, 0.0, 0.0)) # Location of Earth
//    # Observer position
//    if mode == 'LLH': posO = self.getLoc3D_NS(lat, lon, height, rad = False);
//    if mode == 'XYZ' : posO = mathutil.Vector((lat * km2U, lon * km2U, height * km2U));
//        d = posO.length
//        l = math.sqrt(d * d - rE * rE)
//        t = math.asin(rE / d)
//        h = l * math.cos(t)
//        rc = l * math.sin(t)
//        bpy.ops.mesh.primitive_cone_add(vertices = 64, radius1 = rc, depth = h, end_fill_type = 'NOTHING', calc_uvs = True, enter_editmode = False, align = 'WORLD', location = (0, 0, -h / 2.0))
//        bpy.ops.object.origin_set(type = 'ORIGIN_CURSOR', center = 'MEDIAN')
//        cone = bpy.context.view_layer.objects.active
//        cone.rotation_mode = "YZX"
//        cone.rotation_euler = (0.0, math.radians(lat - 90), math.radians(lon))
//        cone.location = posO
//        return
//}
//
void Earth::addGrid(float deg, float size, glm::vec4 color, std::string type, bool rad, bool eq, bool pm) {
    if (!rad) deg *= deg2radf;
    // If equator is enabled, draw equator separately
    if (eq) addLatitudeCurve(0.0f, color, size, true);
    // Use symmetry to draw remaining parallels
    if (type.find("LA") != std::string::npos) {
        for (float lat = deg; lat < pi2 - tiny; lat += deg) {
            addLatitudeCurve(lat, color, size, true);
            addLatitudeCurve(-lat, color, size, true);
        }
    }
    // if prime meridian is enabled, draw prime meridian separately
    if (pm) addLongitudeCurve(0.0f, color, size, true);
    // Use symmetry to draw remaining meridians
    if (type.find("LO") != std::string::npos) {
        for (float lon = deg; lon <= pif; lon += deg) {
            addLongitudeCurve(lon, color, size, true);
            if (lon < pif - tiny) addLongitudeCurve(-lon, color, size, true); // Evade singularity at north pole
        }
    }
}
void Earth::addEquator(float size, glm::vec4 color) {
    addLatitudeCurve(0.0f, color, size, false);
}
void Earth::addTropics(float size, glm::vec4 color) {
    // NOTE: Tropics seem to be fixed by convention but arctics not? That is false according to wikipedia articles on the tropics
    addLatitudeCurve(earthtropics, color, size, false);  // Cancer
    addLatitudeCurve(-earthtropics, color, size, false); // Capricorn
}
void Earth::addArcticCirles(float size, glm::vec4 color) {
    // NOTE: Arctic circle location should account for refraction? No, but nutation ideally, according to definition
    addLatitudeCurve(eartharctics, color, size, false);
    addLatitudeCurve(-eartharctics, color, size, false);
}
unsigned int Earth::addLatitudeCurve(double lat, glm::vec4 color, float width, bool rad) {
    if (!rad) lat *= deg2rad;
    PolyCurve* curve = new PolyCurve(m_scene, color, width);
    curve->AddPoint(getLoc3D(lat, -179.999 * deg2rad, 0.0));
    for (int lon = -179; lon <= 180; lon++) { // Note: we do not want -180, but we do want +180
        curve->AddPoint(getLoc3D(lat, lon * deg2rad, 0.0));
    }
    curve->Generate();
    m_polycache.push_back({ curve,width,LATITUDE,color, {lat,0.0f,0.0f},{0.0f,0.0f,0.0f} });
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
    p.path->ClearPoints();
    p.path->AddPoint(getLoc3D(p.llh1.lat, -179.999 * deg2rad, 0.0));
    for (int lon = -179; lon <= 180; lon++) { // Note: we do not want -180, but we do want +180
        p.path->AddPoint(getLoc3D(p.llh1.lat, lon * deg2rad, 0.0));
    }
    p.path->Generate();
}
void Earth::addPrimeMeridian(float size, glm::vec4 color) {
    addLongitudeCurve(0.0, color, size, false); // Technically 0 degrees = 0 radians, beware when copy/pasting
}
unsigned int Earth::addLongitudeCurve(double lon, glm::vec4 color, float width, bool rad) {
    if (!rad) lon *= deg2rad;
    PolyCurve* curve = new PolyCurve(m_scene, color, width);
    for (int lat = -90; lat <= 90; lat++) {
        curve->AddPoint(getLoc3D(lat * deg2rad, lon, 0.0));
    }
    curve->Generate();
    m_polycache.push_back({ curve,width,LONGITUDE,color, {0.0,lon,0.0},{0.0,0.0,0.0} });
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
    p.path->ClearPoints();
    for (int lat = -90; lat <= 90; lat++) {
        p.path->AddPoint(getLoc3D(lat * deg2rad, p.llh1.lon, 0.0));
    }
    p.path->Generate();
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
LLH Earth::calcGreatArc(LLH llh1, LLH llh2, double f, bool rad) {
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
LLH Earth::calcLerpArc(LLH llh1, LLH llh2, double f, bool rad) {
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
    addArc(llh1, llh2, color, width, rad, &Earth::calcLerpArc, LERPARC);
}
void Earth::addGreatArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad) {
    addArc(llh1, llh2, color, width, rad, &Earth::calcGreatArc, GREATARC);
}
void Earth::addArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad, calcFunc ca, unsigned int type) {
    // The concept is to generate approximately evenly spaced points by scaling the stepsize
    // throughout the run. Meanwhile, tracking the coordinates allow for determining when
    // a seam or pole is crossed, and to split the curve as needed.
    // Note: If you pass over a pole, your longitude inverts sign.
    //       If you cross the seam, your longitude inverts sign.
    //       So all seam/singularity detection can be done by monitoring longitude.
    if (!rad) {
        llh1.lat *= deg2rad; // double
        llh2.lat *= deg2rad; // 
        llh1.lon *= deg2rad; // 
        llh2.lon *= deg2rad; // 
    }
    if (llh1.lon < llh2.lon) {
        LLH llhtmp = llh1;
        llh1 = llh2;
        llh2 = llhtmp;
    }
    glm::vec3 oldap = glm::vec3(1000.0);
    PolyCurve* curve1 = new PolyCurve(m_scene, color, width);
    PolyCurve* curve = curve1;
    double histep = 1.3;
    double lostep = 0.5;
    // Get useful old location and add first point to curve
    LLH llhf = (this->*ca)(llh1, llh2, 0.0, true);
    glm::vec3 tp = getLoc3D((float)llhf.lat, (float)llhf.lon, 0.0f);
    addArcPoint(tp, true, false, oldap, curve); // First=true
    LLH oldf = llhf;
    double dist = calcArcDist(llh1, llh2, true);
    double df = 1.0 / (rad2deg * dist); // Reasonable initial stepsize (may need adjustment to Earth that is radius 1 rather than 4)
    double f = 0.0; // NOTE: df is added right away in while loop below, but f=0.0 point is already added above.
    double half = 0.0;
    double doub = 0.0;
    double dflat = 0.0;
    double dflon = 0.0;
    double ilat = 0.0;
    bool split = false;
    glm::vec3 ip1 = glm::vec3(0.0f);
    glm::vec3 ip2 = glm::vec3(0.0f);
    while (f < 1.0) {
        f += df;
        half, doub = 0.0;
        while (true) {
            llhf = (this->*ca)(llh1, llh2, f, true);
            dflat = abs(llhf.lat - oldf.lat) * rad2deg;
            dflon = abs(llhf.lon - oldf.lon) * rad2deg;
            // Doubling
            if (dflat < lostep && dflon < lostep) {
                f += df;
                df *= 2;
                doub += 1;
                if (doub == 10) {
                    std::cout << "After *1/2^10, stepsize is still too small, giving up!\n";
                    df = 1.0 / dist;
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
                df *= 0.5; // was df /= 2
                f -= df;
                half += 1;
                if (half == 10) {
                    std::cout << "After *2^10, stepsize is still too small, giving up!\n";
                    df = 1.0 / dist;
                    break;
                }
                continue;
            }
        }
        // Here we have the next point ready
        doub = 0.0;
        half = 0.0;
        //  print(f, df, math.degrees(lat_f),math.degrees(lon_f)) #,math.degrees(oldlat),math.degrees(oldlon))
        tp = getLoc3D((float)llhf.lat, (float)llhf.lon, 0.0f);
        //std::cout << "Point: (" << tp.x << "," << tp.y << "," << tp.z << ")\n";
        // If a seam is crossed, split the path and insert intermediate points
        if (abs(llhf.lon - oldf.lon) > pi2) { // Longitude inversion means crossing a seam
            if (oldf.lon > llhf.lon) { // Passed counter clockwise (+ to -)
                ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tau - oldf.lon);
                ip1 = getLoc3D(ilat, pi - tiny);
                ip2 = getLoc3D(ilat, tiny - pi);
                split = true;
            }
            else {
                ilat = oldf.lat - (pif - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tauf - oldf.lon);
                ip2 = getLoc3D(ilat, pi - tiny);
                ip1 = getLoc3D(ilat, tiny - pi);
            }
            // Now insert the extra points to end one path and begin the next
            addArcPoint(ip1, false, true, oldap, curve); // last=true so cap path and start new one
            m_polycache.push_back({ curve, width, type, color, llh1, { ilat, pi - tiny, 0.0} });  // Store midpoint to make update simple
            curve->Generate();
            PolyCurve* curve2 = new PolyCurve(m_scene, color, width);
            PolyCurve* curve = curve2;
            split = true;
            addArcPoint(ip2, true, false, oldap, curve);  // First point in new path
        }
        if (0.0 < f && f < 1.0) addArcPoint(tp, false, false, oldap, curve);
        oldf = llhf;
    }
    glm::vec3 ip = getLoc3D((float)llh2.lat, (float)llh2.lon);
    addArcPoint(ip, false, true, oldap, curve); // Last=true
    if (split) m_polycache.push_back({ curve, width, type, color, {ilat, tiny-pi, 0.0}, llh2 }); // Start at midpoint next time
    else m_polycache.push_back({ curve, width, type, color, llh1, llh2 }); // No split, store whole path
    curve->Generate();
    return;
}
void Earth::addFlatArc(LLH llh1, LLH llh2, glm::vec4 color, float width, bool rad) {
    // The shortest path between two points on the AE map
    float mindist = 0.01f; // Desired step size
    if (!rad) {
        llh1.lat *= deg2rad;
        llh1.lon *= deg2rad;
        llh2.lat *= deg2rad;
        llh2.lon *= deg2rad;
    }
    PolyCurve* curve = new PolyCurve(m_scene, color, width);
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
        curve->AddPoint(pos2);
    }
    curve->Generate();
    m_polycache.push_back({ curve, width, FLATARC, color, llh1,llh2 });
}
void Earth::updateGreatArc(polycache p) {
    updateArc(p, &Earth::calcGreatArc);
}
void Earth::updateLerpArc(polycache p) {
    updateArc(p, &Earth::calcLerpArc);
}
void Earth::updateArc(polycache p, calcFunc ca) {
    // NOTE: Height is awkward, not currently tracked/lerped !!!
    LLH llh1 = p.llh1;
    LLH llh2 = p.llh2;
    if (llh1.lon < llh2.lon) {
        LLH llhtmp = llh1;
        llh1 = llh2;
        llh2 = llhtmp;
    }
    glm::vec3 oldap = glm::vec3(1000.0);
    PolyCurve* curve = p.path;
    curve->ClearPoints();
    double histep = 1.3;
    double lostep = 0.5;
    // Get useful old location and add first point to curve
    LLH llhf = (this->*ca)(llh1, llh2, 0.0, true);
    glm::vec3 tp = getLoc3D(llhf.lat, llhf.lon, 0.0);
    addArcPoint(tp, true, false, oldap, curve); // First=true
    LLH oldf = llhf;
    double dist = calcArcDist(llh1, llh2, true);
    double df = 1.0 / (rad2deg * dist); // Reasonable initial stepsize (may need adjustment to Earth that is radius 1 rather than 4)
    double f = 0.0; // NOTE: df is added right away in while loop below, but f=0.0 point is already added above.
    double half = 0.0;
    double doub = 0.0;
    double dflat = 0.0;
    double dflon = 0.0;
    double ilat = 0.0;
    bool split = false;
    glm::vec3 ip1 = glm::vec3(0.0f);
    glm::vec3 ip2 = glm::vec3(0.0f);
    while (f < 1.0) {
        f += df;
        half, doub = 0.0;
        while (true) {
            llhf = (this->*ca)(llh1, llh2, f, true);
            dflat = abs(llhf.lat - oldf.lat) * rad2deg;
            dflon = abs(llhf.lon - oldf.lon) * rad2deg;
            // Doubling
            if (dflat < lostep && dflon < lostep) {
                f += df;
                df *= 2;
                doub += 1;
                if (doub == 11) {
                    std::cout << "After *1/2^11, stepsize is still too small, giving up!\n";
                    df = 1.0 / dist;
                    break;
                }
                continue;
            }
            // Triggers when step is within limits and no lon inversion
            if (dflat < histep && dflon < histep) break;
            // Triggers when step is within limits and lon has inversion
            if (dflat > histep && dflon > 270.0) break;
            // Halving
            if (dflat > histep || (270.0 > dflon && dflon > histep)) {
                df *= 0.5; // was df /= 2
                f -= df;
                half += 1;
                if (half == 11) {
                    std::cout << "After *2^11, stepsize is still too small, giving up!\n";
                    df = 1.0 / dist;
                    break;
                }
                continue;
            }
        }
        // Here we have the next point ready
        doub = 0.0;
        half = 0.0;
        tp = getLoc3D(llhf.lat, llhf.lon, 0.0f);
        // If a seam is crossed, split the path and insert intermediate points
        if (abs(llhf.lon - oldf.lon) > pi2) { // Longitude inversion means crossing a seam
            if (oldf.lon > llhf.lon) { // Passed counter clockwise (+ to -)
                ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tau - oldf.lon);
                ip1 = getLoc3D(ilat, pi - tiny);
                ip2 = getLoc3D(ilat, tiny - pi);
            }
            else {
                ilat = oldf.lat - (pif - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tauf - oldf.lon);
                ip2 = getLoc3D(ilat, pi - tiny);
                ip1 = getLoc3D(ilat, tiny - pi);
            }
            // Now insert the extra points to end one path and begin the next
            addArcPoint(ip1, false, true, oldap, curve); // last=true so cap path and start new one
            curve->Generate();
            std::cout << "Earth::updateArc(): This code should never be reached!!!\n";
            split = true;
            addArcPoint(ip2, true, false, oldap, curve);  // First point in new path
        }
        if (0.0 < f && f < 1.0) addArcPoint(tp, false, false, oldap, curve);
        oldf = llhf;
    }
    glm::vec3 ip = getLoc3D(llh2.lat, llh2.lon);
    addArcPoint(ip, false, true, oldap, curve); // Last=true
    curve->Generate();
    return;
}
void Earth::updateFlatArc(polycache p) {
    // The shortest path between two points on the AE map
    float mindist = 0.01f; // Desired step size
    PolyCurve* curve = p.path;
    curve->ClearPoints();
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
        curve->AddPoint(pos2);
    }
    curve->Generate();
}
void Earth::addArcPoint(glm::vec3 ap, bool first, bool last, glm::vec3& oldap, PolyCurve* curve) {
    float mindiff = 0.05f; // Minimum difference in coordinates (radians or XYZ?)
    //std::cout << "Point: oldap=(" << oldap.x << "," << oldap.y << "," << oldap.z << ")\n";
    // Check distance to previous point, to ensure even distribution
    glm::vec3 aodiff = ap - oldap;
    if (!last && !first && glm::length(aodiff) < mindiff) return;
    curve->AddPoint(ap);
    oldap = ap;  // Store most recently ADDED point
    //std::cout << "Point: (" << ap.x << "," << ap.y << "," << ap.z << ")\n";
    return;
}
void Earth::addTerminatorTrueMoon(glm::vec4 color, float width) {
    // TO FIX: 2022-02-05 08:15:45 UTC has a defect in Earth::addTerminatorTrueMoon() !!!
    if (m_moonterm1 == nullptr && m_moonterm2 == nullptr) {
        m_moonterm1 = new PolyCurve(m_scene, color, width);
        m_moonterm2 = new PolyCurve(m_scene, color, width);
        updateTerminatorTrueMoon();
    }
}
void Earth::deleteTerminatorTrueMoon() {
    // There is always either two or none, so this could be simplified !!!
    if (m_moonterm2 != nullptr) {
        m_moonterm2->ClearPoints();
        delete m_moonterm2;
        m_moonterm2 = nullptr;
    }
    if (m_moonterm1 != nullptr) {
        m_moonterm1->ClearPoints();
        delete m_moonterm1;
        m_moonterm1 = nullptr;
    }
}
void Earth::updateTerminatorTrueMoon() {
    bool rad = true;
    double refang = w_refract ? w_mrefang : 0.0;
    glm::vec3 oldap = glm::vec3(1000.0f);
    PolyCurve* curve = m_moonterm1;
    double histep = 1.3;
    double lostep = 0.3;
    LLH llhf = calcTerminator(m_moonDec, m_moonHour, 0.0, refang, true);
    glm::vec3 tp = getLoc3D(llhf.lat, llhf.lon, 0.0);
    addArcPoint(tp, true, false, oldap, curve); // First=true
    LLH oldf = llhf;
    double df = deg2rad; // Reasonable initial stepsize (may need adjustment to Earth that is radius 1 rather than 4)
    double f = 0.0; // NOTE: df is added right away in while loop below, but f=0.0 point is already added above.
    double half = 0.0;
    double doub = 0.0;
    double dflat = 0.0;
    double dflon = 0.0;
    double ilat = 0.0;
    glm::vec3 ip1 = glm::vec3(0.0f);
    glm::vec3 ip2 = glm::vec3(0.0f);
    while (f < tau) {
        f += df;
        while (true) {
            llhf = calcTerminator(m_moonDec, m_moonHour, f, refang, true);
            dflat = abs(llhf.lat - oldf.lat) * rad2deg;
            dflon = abs(llhf.lon - oldf.lon) * rad2deg;
            // Doubling
            if (dflat < lostep && dflon < lostep) {
                f += df;
                df *= 2;
                doub += 1;
                if (doub == 11) {
                    std::cout << "After *1/2^11, stepsize is still too small, giving up!\n";
                    df = deg2rad;
                    break;
                }
                continue;
            }
            // Triggers when step is within limits and no lon inversion
            if (dflat < histep && dflon < histep) break;
            // Triggers when step is within limits and lon has inversion
            if (dflat < histep && dflon > 270.0) break;   // inverted dflat test !!!
            // Halving
            if (dflat > histep || (270.0 > dflon && dflon > histep)) {
                df *= 0.5;
                f -= df;
                half += 1;
                if (half == 11) {
                    std::cout << "After *2^11, stepsize is still too big, giving up!\n";
                    df = deg2rad;
                    break;
                }
                continue;
            }
        }
        // Here we have the next point ready
        doub = 0.0;
        half = 0.0;
        tp = getLoc3D((float)llhf.lat, (float)llhf.lon, 0.0f);
        // If a seam is crossed, split the path and insert intermediate points
        if (abs(llhf.lon - oldf.lon) > pi2f) { // Longitude inversion means crossing a seam
            if (oldf.lon > llhf.lon) { // Passed counter clockwise (+ to -)
                //std::cout << "Passed counterclockwise! \n";
                ilat = oldf.lat - (pif - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tauf - oldf.lon);
                ip1 = getLoc3D(ilat, pi - tiny);
                ip2 = getLoc3D(ilat, tiny - pi);
            }
            else {
                ilat = oldf.lat - (pif - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tauf - oldf.lon);
                ip2 = getLoc3D(ilat, pi - tiny);
                ip1 = getLoc3D(ilat, tiny - pi);
            }
            // Now insert the extra points to end one path and begin the next
            addArcPoint(ip1, false, true, oldap, curve); // last=true so cap path and start new one
            curve->Generate();
            curve = m_moonterm2;
            addArcPoint(ip2, true, false, oldap, curve);  // First point in new path
        }
        if (0.0 < f && f < tau) addArcPoint(tp, false, false, oldap, curve);
        oldf = llhf;
    }
    LLH pnt = calcTerminator(m_moonDec, m_moonHour, tau, refang, true);
    glm::vec3 ip = getLoc3D(pnt.lat, pnt.lon);
    addArcPoint(ip, false, true, oldap, curve); // Last=true
    curve->Generate();
    return;
}
void Earth::addTerminatorTrueSun(glm::vec4 color, float width) {
    if (m_sunterm1 == nullptr && m_sunterm2 == nullptr) {
        m_sunterm1 = new PolyCurve(m_scene, color, width);
        m_sunterm2 = new PolyCurve(m_scene, color, width);
        updateTerminatorTrueSun();
    }
}
void Earth::deleteTerminatorTrueSun() {
    // There is always either two or none, so this could be simplified !!!
    if (m_sunterm2 != nullptr) {
        m_sunterm2->ClearPoints();
        delete m_sunterm2;
        m_sunterm2 = nullptr;
    }
    if (m_sunterm1 != nullptr) {
        m_sunterm1->ClearPoints();
        delete m_sunterm1;
        m_sunterm1 = nullptr;
    }
}
void Earth::updateTerminatorTrueSun() {
    bool rad = true;
    double refang = w_refract ? w_srefang : 0.0;
    glm::vec3 oldap = glm::vec3(1000.0f);
    PolyCurve* curve = m_sunterm1;
    double histep = 1.3;
    double lostep = 0.3;
    LLH llhf = calcTerminator(subsolar.lat, subsolar.lon, 0.0, refang, true);
    glm::vec3 tp = getLoc3D(llhf.lat, llhf.lon, 0.0);
    addArcPoint(tp, true, false, oldap, curve); // First=true
    LLH oldf = llhf;
    double df = deg2rad; // Reasonable initial stepsize (may need adjustment to Earth that is radius 1 rather than 4)
    double f = 0.0; // NOTE: df is added right away in while loop below, but f=0.0 point is already added above.
    double half = 0.0;
    double doub = 0.0;
    double dflat = 0.0;
    double dflon = 0.0;
    double ilat = 0.0;
    glm::vec3 ip1 = glm::vec3(0.0f);
    glm::vec3 ip2 = glm::vec3(0.0f);
    while (f < tau) {
        f += df;
        while (true) {
            llhf = calcTerminator(subsolar.lat, subsolar.lon, f, refang, true);
            dflat = abs(llhf.lat - oldf.lat) * rad2deg;
            dflon = abs(llhf.lon - oldf.lon) * rad2deg;
            // Doubling
            if (dflat < lostep && dflon < lostep) {
                f += df;
                df *= 2;
                doub += 1;
                if (doub == 11) {
                    std::cout << "After *1/2^11, stepsize is still too small, giving up!\n";
                    df = deg2rad;
                    break;
                }
                continue;
            }
            // Triggers when step is within limits and no lon inversion
            if (dflat < histep && dflon < histep) break;
            // Triggers when step is within limits and lon has inversion
            if (dflat < histep && dflon > 270.0) break;   // inverted dflat test !!!
            // Halving
            if (dflat > histep || (270.0 > dflon && dflon > histep)) {
                df *= 0.5;
                f -= df;
                half += 1;
                if (half == 11) {
                    std::cout << "After *2^11, stepsize is still too big, giving up!\n";
                    df = deg2rad;
                    break;
                }
                continue;
            }
        }
        // Here we have the next point ready
        doub = 0.0;
        half = 0.0;
        tp = getLoc3D((float)llhf.lat, (float)llhf.lon, 0.0f);
        // If a seam is crossed, split the path and insert intermediate points
        if (abs(llhf.lon - oldf.lon) > pi2f) { // Longitude inversion means crossing a seam
            if (oldf.lon > llhf.lon) { // Passed counter clockwise (+ to -)
                //std::cout << "Passed counterclockwise! \n";
                ilat = oldf.lat - (pif - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tauf - oldf.lon);
                ip1 = getLoc3D(ilat, pi - tiny);
                ip2 = getLoc3D(ilat, tiny - pi);
            }
            else {
                ilat = oldf.lat - (pif - oldf.lon) * (oldf.lat - llhf.lat) / (llhf.lon + tauf - oldf.lon);
                ip2 = getLoc3D(ilat, pi - tiny);
                ip1 = getLoc3D(ilat, tiny - pi);
            }
            // Now insert the extra points to end one path and begin the next
            addArcPoint(ip1, false, true, oldap, curve); // last=true so cap path and start new one
            curve->Generate();
            curve = m_sunterm2;
            addArcPoint(ip2, true, false, oldap, curve);  // First point in new path
        }
        if (0.0 < f && f < tau) addArcPoint(tp, false, false, oldap, curve);
        oldf = llhf;
    }
    LLH pnt = calcTerminator(subsolar.lat, subsolar.lon, tau, refang, true);
    glm::vec3 ip = getLoc3D(pnt.lat, pnt.lon);
    addArcPoint(ip, false, true, oldap, curve); // Last=true
    curve->Generate();
    return;
}
LLH Earth::calcTerminator(double slat, double slon, double parm, double refang, bool rad) {
    if (!rad) {
        slat *= deg2rad;
        slon *= deg2rad;
        parm *= deg2rad;
        refang *= deg2rad;
    }
    LLH res;
    double sinparm = sin(parm);
    res.lat = asin(cos(slat) * sinparm);
    double x = -cos(slon) * sin(slat) * sinparm - sin(slon) * cos(parm);
    double y = -sin(slon) * sin(slat) * sinparm + cos(slon) * cos(parm);
    res.lon = atan2(y, x);
    // Refracted terminator
    if (refang != 0.0) {
        glm::vec3 loc = getLoc3D_NS(res.lat, res.lon);     // Calc as if on sphere
        glm::vec3 sun2 = getLoc3D_NS(slat, slon);
        glm::vec3 rloc = glm::rotate(loc, (float)-refang, glm::cross(loc, sun2));
        res = getXYZtoLLH_NS(rloc);
    }
    if (!rad) {
        res.lat *= rad2deg;
        res.lon *= rad2deg;
    }
    return res;
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
    subsolar = m_scene->m_celestOb->getDecGHA(SUN); // subsolar.lon is west of south, 0 to tau
    //std::cout << "Earth::updateSun(): subsolar.gha { " << subsolar.lat << ", " << subsolar.lon << ", " << subsolar.dst << " }\n";
    if (subsolar.lon < pi) subsolar.lon = -subsolar.lon;
    else subsolar.lon = tau - subsolar.lon;               // subsolar.lon is now -pi to pi east of south
    //std::cout << "Earth::updateSun(): subsolar.geo { " << subsolar.lat << ", " << subsolar.lon << ", " << subsolar.dst << " }\n\n";
    solarGP = getLoc3D(subsolar.lat, subsolar.lon, 0.0f);
    flatSun = getLoc3D(subsolar.lat, subsolar.lon, m_flatsunheight / earthradius);
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
    m_scene = m_earth->m_scene;
    m_radius = rsky;
    m_arrows = m_scene->getArrowsOb();
    m_dots = m_scene->getDotsOb();
    m_planes = m_scene->getPlanesOb();
    m_cylinders = m_scene->getCylindersOb();
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
}
Location::~Location() {
    //std::cout << "~Loc (destructor) called.\n";
    Destroy();
}
void Location::Destroy() {
    if (!m_polycache.empty()) {
        for (auto& pa : m_polycache.m_Elements) {  // These are full objects made with new, so delete
            delete pa.path;             // But isn't this messing the vector length while iterating? - No. We delete path not cache.
            delete pa.planetCP;
        }
        m_polycache.clear();
    }
    if (!m_arrowcache.empty()) {
        for (auto& ar : m_arrowcache) { // These are primitives based, so call Delete()
            if (ar.type != maxuint) {
                m_arrows->Delete(ar.index); //  MUST do this, otherwise primitives are dangling if locs are removed during anim
            }
        }
        m_arrowcache.clear();
    }
    if (!m_dotcache.empty()) {
        for (auto& d : m_dotcache) {
            if (d.type != maxuint) {
                m_dots->Delete(d.index);
            }
        }
        m_dotcache.clear();
    }
    if (!m_planecache.empty()) {
        for (auto& pl : m_planecache) {
            if (pl.type != maxuint) {
                m_planes->Delete(pl.index);
            }
        }
        m_planecache.clear();
    }
    if (!m_cylindercache.empty()) {
        for (auto& cy : m_cylindercache) {
            if (cy.type != maxuint) {
                m_cylinders->Delete(cy.index);
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
        if (d.type == TRUESUN3D && (time || morph)) updateDot3DTrueSun(d);
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
        if (a.type == TRUESUN3D && (time || morph)) updateArrow3DTrueSun(a);
        if (a.type == FLATSUN3D && (time || morph || flatsun)) updateArrow3DFlatSun(a);
        if (a.type == TRUEMOON3D && (time || morph)) updateArrow3DTrueMoon(a);
        if (a.type == TRUEPLANET3D && (time || morph)) updateArrow3DTruePlanet(a);
        if (a.type == AZIELE3D && morph) updateArrow3DEleAzi(a);
        if (a.type == RADEC3D && (time || morph)) updateArrow3DRADec(a);
    }
    // Update Lines
    for (auto& l : m_cylindercache) {
        if (l.type == TRUESUN3D && (time || morph)) updateLine3DTrueSun(l);
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
}
void Location::Draw() {
    // Don't draw Primitives derived objects, as they are drawn from World.
    for (auto& pa : m_polycache.m_Elements) {
        pa.path->Draw();
    }
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
    if (jd == 0.0) jd = m_scene->m_celestOb->getJD();
    double gsidtime = m_scene->m_celestOb->getGsid(jd); // In radians
    double lha = gsidtime + m_lon - deg2rad * ra;
    LLH topocentric = m_scene->m_celestOb->calcGeo2Topo({ deg2rad * dec, lha, 0.0 }, { m_lat, m_lon, 0.0 });
    return calcDirEleAzi(topocentric, true);
}
glm::vec3 Location::getTrueSunDir(double jd) {
    LLH sun = m_scene->m_celestOb->getDecGHA(SUN, jd);
    LLH localsun = m_scene->m_celestOb->calcGeo2Topo(sun, { m_lat, m_lon, 0.0 }); // Sun Ele, Azi
    return calcDirEleAzi(localsun, true);
}
glm::vec3 Location::getFlatSunDir(double jd) {
    glm::vec3 sunloc = m_earth->getSubsolarXYZ(jd);
    return glm::normalize(sunloc - m_pos);
}
void Location::enableTrueSun() {
    if (m_truesun == nullptr) m_truesun = new Location::TrueSun(this);
}
Location::TrueSun::TrueSun(Location* location) : m_location(location) {}
void Location::TrueSun::enableArrow3D() {
    m_arrow = m_location->m_scene->getArrowsOb()->FromStartDirLen(m_location->m_pos, m_location->getTrueSunDir(), m_location->m_radius,0.003f, LIGHT_YELLOW);
}
// Generics
void Location::addLocSky(float size, glm::vec4 color) {
    size = m_radius;
    unsigned int index = m_dots->FromXYZ(m_pos, color, size);
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
            m_dots->Delete(d.index);
            return;
        }
    }
}
void Location::updateLocSky(dotcache& d) {
    d.position = m_pos;
    m_dots->UpdateXYZ(d.index, m_pos, d.color, d.size);
}
void Location::addTangentPlane(glm::vec4 color) {
    const float radiusfactor = 2.1f;
    unsigned int index = m_planes->FromStartUV(
        m_pos + m_zenith * 0.0001f, m_east * m_radius * radiusfactor, m_north * m_radius * radiusfactor, color);
    m_planecache.push_back(
        { m_pos, m_zenith, glm::vec2(m_radius * radiusfactor, m_radius * radiusfactor), color, index, TANGENT });
}
void Location::deleteTangentPlane() {
    for (auto& p : m_planecache) {
        if (p.type == TANGENT) {
            p.type = maxuint;
            m_planes->Delete(p.index);
            return;
        }
    }
}
void Location::updateTangentPlane(planecache& p) {
    p.position = m_pos;
    p.direction = m_zenith;
    // Raise the tangent plane slightly off the ground (+m_zenith*0.0001f), so there is no Z fighting in flat geometries
    m_planes->UpdateStartUV(p.index, p.position + m_zenith * 0.0001f, m_east * p.scale.x, m_north * p.scale.y, p.color);
}
void Location::addMeridianPlane(glm::vec4 color) {
    const float radiusfactor = 2.1f;
    unsigned int index = m_planes->FromStartUV(
        m_pos, m_zenith * m_radius * radiusfactor, m_north * m_radius * radiusfactor, color);
    m_planecache.push_back(
        { m_pos, m_east, glm::vec2(m_radius * radiusfactor, m_radius * radiusfactor), color, index, MERIDIAN });
}
void Location::deleteMeridianPlane() {
    for (auto& p : m_planecache) {
        if (p.type == MERIDIAN) {
            p.type = maxuint;
            m_planes->Delete(p.index);
            return;
        }
    }
}
void Location::updateMeridianPlane(planecache& p) {
    p.position = m_pos;
    p.direction = m_east;
    // Meridian plane is perpendicular to ground, so no need for Z fighting avoidance
    m_planes->UpdateStartUV(p.index, p.position, m_zenith * p.scale.x, m_north * p.scale.y, p.color);
}
void Location::addCoords(float length) {
    addUpCoord(length);
    addEastCoord(length);
    addNorthCoord(length);
}
void Location::addUpCoord(float length, float width) {
    glm::vec4 upcolor = glm::vec4(0.1f, 0.1f, 1.0f, 1.0f);
    unsigned int index = m_arrows->FromStartDirLen(m_pos, m_zenith, length, width, upcolor);
    m_arrowcache.push_back({ m_pos,m_zenith,upcolor,length,width,0.0f,0.0f,index,ZENITH, maxuint });
}
// ADD: change, delete
void Location::updateUpCoord(arrowcache& ar) {
    m_arrows->UpdateStartDirLen(ar.index, m_pos, m_zenith, ar.length, ar.width, ar.color);
}
void Location::addEastCoord(float length, float width) {
    glm::vec4 eastcolor = glm::vec4(1.0f, 0.1f, 0.1f, 1.0f);
    unsigned int index = m_arrows->FromStartDirLen(m_pos, m_east, length, width, eastcolor);
    m_arrowcache.push_back({ m_pos,m_east,eastcolor,length,width,0.0f,0.0f,index,EAST, maxuint });
}
void Location::updateEastCoord(arrowcache& ar) {
    m_arrows->UpdateStartDirLen(ar.index, m_pos, m_east, ar.length, ar.width, ar.color);
}
void Location::addNorthCoord(float length, float width) {
    glm::vec4 northcolor = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f);
    unsigned int index = m_arrows->FromStartDirLen(m_pos, m_north, length, width, northcolor);
    m_arrowcache.push_back({ m_pos,m_north,northcolor,length,width,0.0f,0.0f,index,NORTH, maxuint });
}
void Location::updateNorthCoord(arrowcache& ar) {
    m_arrows->UpdateStartDirLen(ar.index, m_pos, m_north, ar.length, ar.width, ar.color);
}
void Location::addNormal(float length, float width) {
    glm::vec4 normalcolor = LIGHT_YELLOW;  glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 dir = m_earth->getNml3D(m_lat, m_lon);
    unsigned int index = m_arrows->FromStartDirLen(m_pos, dir, length, width, normalcolor);
    m_arrowcache.push_back({ m_pos,dir,normalcolor,length,width,0.0f,0.0f,index,NORMAL, maxuint });
}
void Location::updateNormalCoord(arrowcache& ar) {
    m_arrows->UpdateStartDirLen(ar.index, m_pos, m_earth->getNml3D(m_lat, m_lon), ar.length, ar.width, ar.color);
}
void Location::addObserver(float bearing, glm::vec4 color, float height) {
    if (height == 0.0f) height = m_radius;
    bearing = deg2radf * (bearing - 180.0f);
    glm::vec3 dir = m_earth->getNml3D(m_lat, m_lon);
    m_observer = m_scene->newMinifigs()->FromStartDirLen(m_pos, dir, height, height, color, bearing);
}
void Location::changeObserver(float bearing, glm::vec4 color, float height) {
    if (color == NO_COLOR) color = m_scene->newMinifigs()->getDetails(m_observer)->color;
    if (height == 0.0f) height = m_scene->newMinifigs()->getDetails(m_observer)->scale.z;
    bearing = deg2radf * (bearing - 180.0f);
    glm::vec3 dir = m_earth->getNml3D(m_lat, m_lon);
    m_scene->newMinifigs()->UpdateStartDirLen(m_observer, m_pos, dir, height, height, color, bearing);

}
void Location::addArrow3DEleAzi(unsigned int unique, double ele, double azi, float length, float width, glm::vec4 color) {
    // Note: unique ID allows to construct multiple distinguishable arrows at caller's leasure, unique to this location
    glm::vec3 dir = calcDirEleAzi({ ele, azi, 0.0 }, false);
    unsigned int index = m_arrows->FromStartDirLen(m_pos, dir, length, width, color);
    m_arrowcache.push_back({ m_pos,dir,color,length,width,ele,azi,index,AZIELE3D, unique });
}
void Location::deleteArrow3DEleAzi(unsigned int unique) {
    for (auto& a : m_arrowcache) { // Cannot delete m_arrowcache here, that will mess up Update() loop! Why?
        if (a.type == AZIELE3D && a.unique == unique) {
            m_arrows->Delete(a.index);
            a.type = maxuint; // Mark as inactive
            return;
        }
    }
}
void Location::changeArrow3DEleAzi(unsigned int unique, double ele, double azi, float length, float width, glm::vec4 color) {
    for (auto& a : m_arrowcache) {
        if (a.type == AZIELE3D && a.unique == unique) {
            glm::vec3 dir = calcDirEleAzi({ ele, azi, 0.0 }, false);
            m_arrows->UpdateStartDirLen(a.index, m_pos, dir, length, width, color);
            a = { m_pos,dir,color,length,width,ele,azi,a.index,AZIELE3D, unique };
        }
    }
}
void Location::changeArrow3DEleAziColor(unsigned int unique, glm::vec4 color) {
    // NOTE: Should take unique ident !!!
    for (auto& a : m_arrowcache) {
        if (a.type == AZIELE3D) {
            m_arrows->UpdateStartDirLen(a.index, a.position, a.direction, a.length, a.width, color);
            a = { a.position,a.direction,color,a.length,a.width,a.elevation,a.azimuth,a.index,AZIELE3D, unique };
        }
    }
}
void Location::updateArrow3DEleAzi(arrowcache& ar) {
    glm::vec3 dir = calcDirEleAzi({ ar.elevation, ar.azimuth, 0.0 }, false);
    m_arrows->UpdateStartDirLen(ar.index, m_pos, dir, ar.length, ar.width, ar.color);
}
void Location::addArrow3DRADec(unsigned int unique, double ra, double dec, glm::vec4 color, float width, float length) {
    // Takes RA & Dec in degrees
    // m_lat & m_lon are in rads. Consider using GetLat(true) and GetLon(true) instead
    double gsidtime = m_scene->m_celestOb->getGsid(); // In radians
    double gha = gsidtime - (deg2rad * ra); // calcGeo2Topo() needs GHA, not LHA
    LLH topocentric = m_scene->m_celestOb->calcGeo2Topo({ deg2rad * dec, gha, 0.0 }, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcDirEleAzi(topocentric, true);
    unsigned int index = m_arrows->FromStartDirLen(m_pos, dir, length, width, color);
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
    double gsidtime = m_scene->m_celestOb->getGsid(); // In radians
    double gha = gsidtime - (deg2rad * ar.azimuth);
    LLH topocentric = m_scene->m_celestOb->calcGeo2Topo({ deg2rad * ar.elevation, gha, 0.0 }, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcDirEleAzi(topocentric, true);
    m_arrows->UpdateStartDirLen(ar.index, m_pos, dir, ar.length, ar.width, ar.color);
}

//-------------------------------------- Dots --------------------------------------//
void Location::addLocDot(float size, glm::vec4 color) {
    unsigned int index = m_dots->FromXYZ(m_pos, color, size);
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
            m_dots->Delete(d.index);
            return;
        }
    }
}
void Location::updateLocDot(dotcache& d) {
    m_dots->UpdateXYZ(d.index, m_pos, d.color, d.size);
}
void Location::addDot3DTrueSun(glm::vec4 color, float size) {
    glm::vec3 dir = getTrueSunDir();
    unsigned int index = m_dots->FromXYZ(m_pos + dir * m_radius, color, size);
    m_dotcache.push_back({ m_pos, size, color, index, TRUESUN3D });
}
void Location::changeDot3DTrueSun(float size, glm::vec4 color) {
    // size <= 0.0f -> don't update
    // color = NOT_A_COLOR -> don't update
    for (auto& d : m_dotcache) {
        if (d.type == TRUESUN3D) {
            if (color != NO_COLOR) d.changeColor(color);
            if (size > 0.0f)  d.changeSize(size);
            return;
        }
    }
}
void Location::deleteDot3DTrueSun() {
    for (auto& d : m_dotcache) {
        if (d.type == TRUESUN3D) {
            d.type = maxuint;
            m_dots->Delete(d.index);
            return;
        }
    }
}
void Location::updateDot3DTrueSun(dotcache& d) {
    m_dots->UpdateXYZ(d.index, m_pos + getTrueSunDir() * m_radius, d.color, d.size);
}
//-------------------------------------- Arrows --------------------------------------//
void Location::addArrow3DTrueSun(float length, float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present
    if (checkit) for (auto& ar : m_arrowcache) { if (ar.type == TRUESUN3D) return; }
    glm::vec3 dir = getTrueSunDir();
    unsigned int index = m_arrows->FromStartDirLen(m_pos, dir, length, width, color);
    m_arrowcache.push_back({ m_pos,dir,color,length,width,0.0,0.0,index,TRUESUN3D, maxuint });
}
void Location::changeArrow3DTrueSun(float length, float width, glm::vec4 color) {
    for (auto& a : m_arrowcache) {
        if (a.type == TRUESUN3D) {
            m_arrows->UpdateStartDirLen(a.index, a.position, a.direction, length, width, color);
            a = { a.position,a.direction,color,length,width,a.elevation,a.azimuth,a.index,TRUESUN3D, maxuint };
        }
    }
}
void Location::deleteArrow3DTrueSun() {
    for (auto& a : m_arrowcache) {
        if (a.type == TRUESUN3D) {
            m_arrows->Delete(a.index);
            a.type = maxuint;
            //return;  // NOTE: Should I let the loop run, in case I have added more than one of this type? !!!
        }
    }
}
void Location::updateArrow3DTrueSun(arrowcache& ar) {
    m_arrows->UpdateStartDirLen(ar.index, m_pos, getTrueSunDir(), ar.length, ar.width, ar.color);
}
void Location::addArrow3DFlatSun(float length, float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present
    if (checkit) for (auto& ar : m_arrowcache) { if (ar.type == FLATSUN3D) return; }
    unsigned int index = m_arrows->FromStartDirLen(m_pos, getFlatSunDir(), length, width, color);
    m_arrowcache.push_back({ m_pos,getFlatSunDir(),color,length,width,0,0,index,FLATSUN3D, maxuint });
}
// ADD: change, delete
void Location::updateArrow3DFlatSun(arrowcache& ar) {
    m_arrows->UpdateStartDirLen(ar.index, m_pos, getFlatSunDir(), ar.length, ar.width, ar.color);
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
    unsigned int index = m_arrows->FromStartDirLen(m_pos, dir, length, width, color);
    m_arrowcache.push_back({ m_pos,dir,color,length,width,localmoon.Y,localmoon.X,index,TRUEMOON3D, maxuint });
}
void Location::updateArrow3DTrueMoon(arrowcache& ar) {
    LLH moon = m_earth->getMoon();    // sun { rad sunDec, rad sunHour, AU sunDist }
    CAA2DCoordinate localmoon = CAACoordinateTransformation::Equatorial2Horizontal(
        rad2hrs * (-moon.lon + m_lon), rad2deg * moon.lat, rad2deg * m_lat);
    glm::vec3 dir = calcDirEleAzi({ localmoon.Y, localmoon.X, 0.0 }, false);
    m_arrows->UpdateStartDirLen(ar.index, m_pos, dir, ar.length, 0.003f, ar.color);
}

void Location::updateArrow3DTruePlanet(arrowcache& ar) {
    LLH pos = m_scene->m_celestOb->getDecGHA(ar.unique);
    LLH topo = m_scene->m_celestOb->calcGeo2Topo(pos, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcDirEleAzi(topo, true);
    m_arrows->UpdateStartDirLen(ar.index, m_pos, dir, ar.length, 0.003f, ar.color);
}
// ADD: change, delete
void Location::addArrow3DTruePlanet(unsigned int planet, float length, glm::vec4 color, bool checkit) {
    if (checkit) { // Caller can specify to skip if this type of arrow is already present
        for (auto& ar : m_arrowcache) {
            if (ar.unique == planet) return;
        }
    }
    color = getPlanetColor(planet, color);
    m_scene->m_celestOb->enablePlanet(planet);
    LLH pos = m_scene->m_celestOb->getDecGHA(planet);
    LLH topo = m_scene->m_celestOb->calcGeo2Topo(pos, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcDirEleAzi(topo, true);
    unsigned int index = m_arrows->FromStartDirLen(m_pos, dir, length, 0.003f, color);
    m_arrowcache.push_back({ m_pos, dir, color, length, 0.003f, topo.lat, topo.lon, index, TRUEPLANET3D, planet });
}
//-------------------------------------- Lines --------------------------------------//
void Location::addLine3DTrueSun(float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present
    if (checkit) for (auto& l : m_cylindercache) { if (l.type == TRUESUN3D) return; }
    glm::vec3 dir = getTrueSunDir();
    unsigned int index = m_cylinders->FromStartEnd(m_pos, dir*100.0f, width, color);
    m_cylindercache.push_back({ index,TRUESUN3D, m_pos, dir*100.0f, width, color });
}
// ADD: change, delete
void Location::updateLine3DTrueSun(cylindercache& l) {
    // Caller can specify to skip if this type of arrow is already present
    l.end = getTrueSunDir() * 100.0f;
    l.start = m_pos;
    m_cylinders->UpdateStartEnd(l.index, l.start, l.end, l.width, l.color);
}
void Location::addLine3DFlatSun(float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present
    if (checkit) for (auto& l : m_cylindercache) { if (l.type == FLATSUN3D) return; }
    glm::vec3 flatsun = m_earth->getSubsolarXYZ();
    unsigned int index = m_cylinders->FromStartEnd(m_pos, flatsun, width, color);
    m_cylindercache.push_back({ index,FLATSUN3D, m_pos, flatsun, width, color });
}
// ADD: change, delete
void Location::updateLine3DFlatSun(cylindercache& l) {
    // Caller can specify to skip if this type of arrow is already present
    l.start = m_pos;
    l.end = m_earth->getSubsolarXYZ();
    m_cylinders->UpdateStartEnd(l.index, m_pos, l.end, l.width, l.color);
}

//-------------------------------------- Paths --------------------------------------//
// To facilitate deleting paths, they should return an id of some sort. Done!
// Additionally the polycache must support gap elimination (or mark entries as void). Done by using tightvec!
void Location::addPlanetaryPath(unsigned int planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, glm::vec4 color, float width) {
    CelestialPath* planetCP = m_scene->m_celestOb->getCelestialPath(planet, startoffset, endoffset, steps, ECGEO);
    color = getPlanetColor(planet, color);
    PolyCurve* path = new PolyCurve(m_scene, color, width);
    unsigned int index = m_polycache.store({ path, planetCP, width, type, color, 0.0, 0.0, planet });
    m_polycache[index].index = index;
    updatePlanetaryPath(m_polycache[index]);
}
// changePlanetaryPath() - What should it change exactly? primitive properties or celestial ones?
// deletePlanetaryPath() - Would need to specify (planet, start, end, steps), or addPlanetaryPath should return an index, or a second unique field is needed.
void Location::updatePlanetaryPath(polycache& pa) {
    pa.path->ClearPoints();
    for (auto const& pt : pa.planetCP->entries) {
        LLH topo = m_scene->m_celestOb->calcGeo2Topo({ pt.geodec, pt.geogha, 0.0 }, { m_lat, m_lon, 0.0 });
        glm::vec3 dir = calcDirEleAzi(topo, true);
        pa.path->AddPoint(m_pos + dir * m_radius);
    }
    pa.path->Generate();
}
// ADD: change, delete
void Location::addPlanetTruePath24(unsigned int planet, glm::vec4 color, float width) {
    // Planetary path across sky in 24 hrs, roughly follows earth rotation of course
    addPlanetaryPath(planet, -0.5, 0.5, 100, TRUEPLANET3D, color, width);
}
// ADD: change(color, width)
void Location::deletePlanetTruePath24(unsigned int planet) {
    polycache* path = nullptr;
    for (auto& p : m_polycache.m_Elements) {
        if (p.type == TRUEPLANET3D && p.unique == planet) {
            path = &p;
            break;
        }
    }
    if (path != nullptr) {
        delete path->path; // PolyCurve
        m_scene->m_celestOb->removeCelestialPath(path->planetCP); // CelestialPath
        unsigned int index = path->index;
        m_polycache.remove(index); // Cache
    }
    else std::cout << "WARNING: Location::deletePlanetTruePath24() called with non-existant planet: " << planet << "\n";
}
void Location::addPlanetTruePathSidYear(unsigned int planet, glm::vec4 color, float width) {
    // Planetary path across fixed star background, over approximately 1 year
    double bracket = sidyearparms[planet - SUN].bracket;
    double stepsize = sidyearparms[planet - SUN].stepsize;
    //CelestialPath* planetCP = m_world->GetCelestialOb()->getCelestialPath(planet, -bracket * sidereald, bracket * sidereald, 2 * (unsigned int)bracket / stepsize, ECGEO);
    //PolyCurve* path = new PolyCurve(m_world, color, width);
    //m_polycache.store({ path, planetCP, width, SIDPLANET3D, color, 0.0, 0.0, planet }); // SIDPLANET3D
    //updatePlanetaryPath(m_polycache.m_Elements.back());
    addPlanetaryPath(planet, -bracket * sidereald, bracket * sidereald, 2 * (unsigned int)(bracket / stepsize), SIDPLANET3D, color, width);
}
// ADD: change
void Location::deletePlanetTruePathSidYear(unsigned int planet) {
    polycache* path = nullptr;
    for (auto& p : m_polycache.m_Elements) {
        if (p.type == SIDPLANET3D && p.unique == planet) {
            path = &p;
            break;
        }
    }
    if (path != nullptr) {
        delete path->path; // PolyCurve
        m_scene->m_celestOb->removeCelestialPath(path->planetCP); // CelestialPath
        unsigned int index = path->index;
        m_polycache.remove(index); // Cache
    }
    else std::cout << "WARNING: Location::deletePlanetTruePathSidYear() called with non-existant planet: " << planet << "\n";
}
void Location::addPath3DTrueSun(glm::vec4 color, float width) {
    // Check if there is one already before creating. Could increase a refcnt, but seems unnecessary
    addPlanetaryPath(SUN, -0.5, 0.5, 100, TRUESUN3D, color, width);
}
void Location::deletePath3DTrueSun() {
    polycache* path = nullptr;
    for (auto& p : m_polycache.m_Elements) {
        if (p.type == TRUESUN3D) {
            path = &p;
            break;
        }
    }
    if (path != nullptr) {
        delete path->path; // PolyCurve
        m_scene->m_celestOb->removeCelestialPath(path->planetCP); // CelestialPath
        unsigned int index = path->index;
        m_polycache.remove(index); // Cache
    }
    else std::cout << "WARNING: Location::deletePath3DTrueSun() called but no TRUESUN3D path was found!\n";
}
void Location::addAnalemma3DTrueSun(glm::vec4 color, float width) {
    addPlanetaryPath(SUN, -183.0, 183.0, 366, TRUEANALEMMA3D, color, width);  // Maybe just set planet to TRUEANALEMMA3D and test for it in addPlanetaryPath()
}
void Location::deleteAnalemma3DTrueSun() {
    polycache* path = nullptr;
    for (auto& p : m_polycache.m_Elements) {
        if (p.type == TRUEANALEMMA3D) {
            path = &p;
            break;
        }
    }
    if (path != nullptr) {
        delete path->path; // PolyCurve
        m_scene->m_celestOb->removeCelestialPath(path->planetCP); // CelestialPath
        unsigned int index = path->index;
        m_polycache.remove(index); // Cache
    }
    else std::cout << "WARNING: Location::deleteAnalemma3DTrueSun() called but no TRUEANALEMMA3D path was found!\n";
}
void Location::addPath3DFlatSun(glm::vec4 color, float width) {
    PolyCurve* path = new PolyCurve(m_scene, color, width);
    doPath3DFlatSun(path);
    m_polycache.store({ path, nullptr, width, FLATSUN3D, color, 0.0, 0.0, 0 });
}
void Location::updatePath3DFlatSun(PolyCurve* path, glm::vec4 color, float width) {
    path->ClearPoints();
    doPath3DFlatSun(path);
    return;
}
void Location::doPath3DFlatSun(PolyCurve* path) {
    // Change to smarter date/time based start and end
    // For now, just sweep 1 JD centered on current epoch
    //float height = 0.0/earthradius; // Observer height in km to Earth radii 
    double myJD = m_scene->m_celestOb->getJD() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        path->AddPoint(m_pos + getFlatSunDir(fday) * m_radius);
    }
    path->Generate();
}

void Location::addPath3DTrueMoon(glm::vec4 color, float width) {
    PolyCurve* path = new PolyCurve(m_scene, color, width);
    doPath3DTrueMoon(path);
    m_polycache.store({ path, nullptr, width, TRUEMOON3D, color, 0.0, 0.0, 0 });
}
void Location::updatePath3DTrueMoon(polycache p) {
    p.path->ClearPoints();
    doPath3DTrueMoon(p.path);
    return;
}
void Location::doPath3DTrueMoon(PolyCurve* path) {
    // Change to smarter date/time based start and end
    // For now, just sweep 1 JD centered on current epoch
    double myJD = m_scene->m_celestOb->getJD() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        LLH moon = m_earth->getMoon(fday);
        CAA2DCoordinate localmoon = CAACoordinateTransformation::Equatorial2Horizontal(
            rad2hrs * (-moon.lon + m_lon), rad2deg * moon.lat, rad2deg * m_lat);
        glm::vec3 dir = calcDirEleAzi({ localmoon.Y, localmoon.X, 0.0 }, false);
        path->AddPoint(m_pos + dir * m_radius);
    }
    path->Generate();
}

// Stars are likely to be added in multitudes, implement a unique id passed by caller (unsigned int)
void Location::addPath3DRADec(unsigned int unique, double ra, double dec, glm::vec4 color, float width) {
    PolyCurve* path = new PolyCurve(m_scene, color, width);
    doPath3DRADec(ra, dec, path);
    m_polycache.store({ path, nullptr, width, RADEC3D, color, dec, ra, unique });
}
void Location::updatePath3DRADec(polycache& pa) {
    pa.path->ClearPoints();
    doPath3DRADec(pa.azimuth, pa.elevation, pa.path);
    return;
}
void Location::doPath3DRADec(double ra, double dec, PolyCurve* path) {
    // Change to smarter date/time based start and end
    // For now, just sweep 1 JD centered on current epoch
    double myJD = m_scene->m_celestOb->getJD() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        glm::vec3 point = m_pos + calcDirRADec(ra, dec, fday) * m_radius;
        path->AddPoint(point);
    }
    path->Generate();
}


// -------------
//  SolarSystem
// -------------
SolarSystem::SolarSystem(Scene* scene, bool geocentric) : m_scene(scene), m_geocentric(geocentric) {
    //std::cout << "SolarSystem::SolarSystem()\n";
    m_astro = m_scene->m_celestOb;
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
    if (!update) m_PlanetDot[planet] = m_scene->getDotsOb()->FromXYZ(pos, m_planetinfos[planet].color, planetdot);
    if (update) m_scene->getDotsOb()->UpdateXYZ(m_PlanetDot[planet], pos, m_planetinfos[planet].color, planetdot);
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
        m_PlanetPath[planet - SUN] = new PolyCurve(m_scene, color, solsyspathwidth);
        m_PlanetOrbit[planet - SUN] = m_scene->m_celestOb->getCelestialPath(planet, -0.5 * siderealyear, 0.5 * siderealyear, orbitsteps, EC);
    }
    if (update) m_PlanetPath[planet - SUN]->ClearPoints();
    for (auto const& pt : m_PlanetOrbit[planet - SUN]->entries) {
        if (planet != SUN) m_PlanetPath[planet - SUN]->AddPoint(Ecliptic2Cartesian(pt.eclat, pt.eclon, pt.ecdst) + m_sunpos);
        else m_PlanetPath[planet - SUN]->AddPoint(Ecliptic2Cartesian(pt.eclat, pt.eclon, pt.ecdst));
    }
    m_PlanetPath[planet]->Generate();
    m_PlanetPath[planet]->Draw();  // Needed? Yes!
}
void SolarSystem::SunPos(bool update) {
    m_sunpos = m_geocentric ? CalcSun() : glm::vec3(0.0f);

    if (!update) m_SunDot = m_scene->getDotsOb()->FromXYZ(m_sunpos, SUNCOLOR, planetdot);
    if (update) m_scene->getDotsOb()->UpdateXYZ(m_SunDot, m_sunpos, SUNCOLOR, planetdot);
    if (m_PlanetTrail[SUN] != nullptr && m_trails) m_PlanetTrail[SUN]->push(m_sunpos);
}
void SolarSystem::SunOrbit(bool update) {
    m_SunPath = new PolyCurve(m_scene, SUNCOLOR, solsyspathwidth);
    double siderealyear = 366; // Sun days
    for (double jd = m_astro->getJD() - 0.5 * siderealyear; jd < m_astro->getJD() + 0.5 * siderealyear; jd += siderealyear / 360) {
        glm::vec3 pos = CalcSun(jd);
        m_SunPath->AddPoint(pos);
    }
    m_SunPath->Generate();
    m_SunPath->Draw();
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
    if (!update) m_EarthDot = m_scene->getDotsOb()->FromXYZ(m_earthpos, EARTHCOLOR, planetdot);
    if (update) m_scene->getDotsOb()->UpdateXYZ(m_EarthDot, m_earthpos, EARTHCOLOR, planetdot);
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
    unsigned int index = m_cylinders->FromStartEnd(GetPlanetPos(planet1), GetPlanetPos(planet2), width, color);
    m_distlines.store({ index, planet1, planet2, color, width });
}
void SolarSystem::UpdateDistLines() {
    for (auto& dl : m_distlines.m_Elements) {
        m_cylinders->UpdateStartEnd(dl.index, GetPlanetPos(dl.planet1), GetPlanetPos(dl.planet2), dl.width, dl.color);    
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
    m_dotcache.reserve(1000);
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
    glm::mat4 proj = m_scene->w_camera->GetProjMat();
    //glm::mat4 proj = glm::perspective(glm::radians(30.0f), (float)m_world->w_width / (float)m_world->w_height, 1.0f, 1000.0f);
    //std::cout << "Drawing SkySphere\n";
    // Shader setup
    UpdateTime(0.0);
    shdr->Bind();
    shdr->SetUniformMatrix4f("view", m_scene->w_camera->GetSkyViewMat());
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
    m_gsid = (float)(-m_scene->m_celestOb->getGsid(jd));
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
    glm::vec3 pos = getLoc3D_NS(dec, m_gsid - ra2);
    unsigned int index = m_dots->FromXYZ(pos, LIGHT_GREEN, 0.3f);
    m_dotcache.push_back({ unique, LIGHT_GREEN, dec, ra, 0.0f, 0.3f, index });
    return; // (unsigned int)m_dotcache.size() - 1;
}
void SkySphere::updateDotDecRA(dotcache dot) {
    double ra = dot.lon;
    if (ra < pi) ra = -ra;
    else ra = tau - ra;
    glm::vec3 pos = getLoc3D_NS(dot.lat, m_gsid - ra);
    m_dots->UpdateXYZ(dot.index, pos, dot.color, dot.size);
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