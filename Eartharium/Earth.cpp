

#include <glm/gtx/rotate_vector.hpp>     // Rotation matrices for glm

#include "Earth.h"
//#include "Renderer.h"

//  #include "astronomy/aearth.h"
//  #include "astronomy/amoon.h"

// ----------
//  LocGroup
// ----------
LocGroup::LocGroup(Earth* e, unsigned int identifier) {
    earth = e;
    id = identifier;
}
LocGroup::~LocGroup() {
    clear();
}
void LocGroup::clear() { 
    locations.clear(); 
}
unsigned int LocGroup::size() const {
    return (unsigned int)locations.size();
}
Location* LocGroup::addLocation(double dLat, double dLon, bool rad, float rsky) {
    // implemented with std::list of Location pointers to keep a dense datastructure for easy GUI construction etc.
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
//Planetoid::Planetoid(Scene* scene, unsigned int meshU, unsigned int meshV, float radius) : m_scene(scene) {
//    // Note: Could this be built into a parent class for solar system bodies?
//    //       It would work more like Path3D, as each body would probably have different texture.
//    //       Actually, OpenGL supports arrays of samplers. Another option is texture array.
//    //std::cout << "Planetoid created.";
//    m_verts.reserve(((size_t)meshU + 1) * ((size_t)meshV + 1));
//    m_tris.reserve((size_t)meshU * (size_t)meshV * sizeof(Tri));
//    genGeom(radius);
//
//    vbl = new VertexBufferLayout;
//    vbl->Push<float>(3);   // Vertex pos
//    vbl->Push<float>(3);   // Vertex normal
//    vbl->Push<float>(2);   // Texture coord
//    va = new VertexArray;
//    vb = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
//    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);
//    va->AddBuffer(*vb, *vbl, true);
//    //std::cout << "Object1 (vb,ib,va): " << vb.GetRenderID() << "," << ib.GetRenderID() << "," << va.GetRenderID() << std::endl;
//    //std::string shadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\planetoid.shader";
//    //shdr = new Shader(shadersrc);
//    shdr = m_scene->m_app->getShaderLib()->getShader(PLANETOID_SHADER);
//    const std::string texfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_sun.png";
//    tex = new Texture(texfile, GL_TEXTURE5);
//    int at1 = 0;
//    glGetIntegerv(GL_ACTIVE_TEXTURE, &at1);
//    //std::cout << "Planetoid::Planetoid(): Created TextureSlot " << tex->GetTextureSlot() << ", RenderID " << tex->GetRenderID() << ".\n";
//}
//Planetoid::~Planetoid() {
//    //std::cout << "Planetoid destroyed.";
//    delete tex;
//    delete ib;
//    delete vb;
//    delete va;
//    delete vbl;
//}
//void Planetoid::Update(glm::vec3 pos) {
//    position = pos;
//}
//void Planetoid::Draw() {
//    shdr->Bind();
//    shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
//    shdr->SetUniformMatrix4f("projection", m_scene->w_camera->getProjMat());
//    shdr->SetUniform3f("position", position.x, position.y, position.z);
//    shdr->SetUniform3f("lightDir", m_scene->w_camera->CamLightDir.x, m_scene->w_camera->CamLightDir.y, m_scene->w_camera->CamLightDir.z);
//    //shdr->SetUniform1i("tex", tex->GetRenderID());  // Wrong! Texture units and textures are not the same.
//    //  See: https://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
//    shdr->SetUniform1i("tex", tex->GetTextureSlot());
//    va->Bind();
//    ib->Bind();
//    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    //glDisable(GL_CULL_FACE); // It is irritating that Earth disappears when seen from back/in side
//    glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0);
//    //glEnable(GL_CULL_FACE);
//    ib->Unbind();
//    va->Unbind();
//    shdr->Unbind();
//    return;
//}
//glm::vec3 Planetoid::getLoc3D_NS(float rLat, float rLon, float height, float radius) {
//    float m_Radius = 1.0f;
//    float w = cos(rLat) * (radius + height);
//    float x = cos(rLon) * w;
//    float y = sin(rLon) * w;
//    float z = sin(rLat) * (radius + height);
//    return glm::vec3(x, y, z);
//}
//void Planetoid::genGeom(float radius) {
//    float rLat, rLon;
//    unsigned int meshV = 16; // Bands
//    unsigned int meshU = 32; // Facets
//
//    for (unsigned int v = 0; v <= meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
//        rLat = (pif * v / meshV) - pi2f;
//        for (unsigned int u = 0; u <= meshU; u++) {
//            rLon = (tauf * u / meshU) - pif;
//            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);     // position of vertex
//            glm::vec3 nml = glm::normalize(loc);  // Insolation normal, always NS (or G8?)
//            m_verts.push_back({ loc, nml, glm::vec2((float)u / (float)meshU, (float)v / (float)meshV) });
//            if (u < meshU && v < meshV) {
//                m_tris.push_back({ (meshU + 1) * v + u , (meshU + 1) * (v + 1) + u, (meshU + 1) * (v + 1) + 1 + u });
//                m_tris.push_back({ (meshU + 1) * (v + 1) + 1 + u, (meshU + 1) * v + u + 1, (meshU + 1) * v + u });
//            }
//        }
//    }
//}
//

//// ---------
////  SubSolar
//// ---------
//SubSolar::SubSolar(Scene* scene, unsigned int meshU, unsigned int meshV, float radius) : m_scene(scene) {
//    // Note: Could this be built into a parent class for solar system bodies?
//    //       It would work more like Path3D, as each body would probably have different texture.
//    //       Actually, OpenGL supports arrays of samplers. Another option is texture array.
//    //std::cout << "Planetoid created.";
//    m_verts.reserve(((size_t)meshU + 1) * ((size_t)meshV + 1));
//    m_tris.reserve((size_t)meshU * (size_t)meshV * sizeof(Tri));
//    genGeom(radius);
//
//    vbl = new VertexBufferLayout;
//    vbl->Push<float>(3);   // Vertex pos
//    vbl->Push<float>(3);   // Vertex normal
//    vbl->Push<float>(2);   // Texture coord
//    va = new VertexArray;
//    vb = new VertexBuffer(&m_verts[0], (unsigned int)m_verts.size() * sizeof(Vertex));
//    ib = new IndexBuffer((unsigned int*)&m_tris[0], (unsigned int)m_tris.size() * 3);
//    va->AddBuffer(*vb, *vbl, true);
//    //std::cout << "Object1 (vb,ib,va): " << vb.GetRenderID() << "," << ib.GetRenderID() << "," << va.GetRenderID() << std::endl;
//    //std::string shadersrc = "C:\\Coding\\Eartharium\\Eartharium\\shaders\\planetoid.shader";
//    //shdr = new Shader(shadersrc);
//    shdr = m_scene->m_app->getShaderLib()->getShader(PLANETOID_SHADER);
//    const std::string texfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_sun.png";
//    tex = new Texture(texfile, GL_TEXTURE5);
//    int at1 = 0;
//    glGetIntegerv(GL_ACTIVE_TEXTURE, &at1);
//    //std::cout << "SubSolar::SubSolar(): Created TextureSlot " << tex->GetTextureSlot() << ", RenderID " << tex->GetRenderID() << ".\n";
//}
//SubSolar::~SubSolar() {
//    //std::cout << "SubSolar destroyed.";
//    delete tex;
//    delete ib;
//    delete vb;
//    delete va;
//    delete vbl;
//}
//void SubSolar::Update(glm::vec3 pos) {
//    position = pos;
//}
//void SubSolar::draw(Camera* cam) {
//    shdr->Bind();
//    shdr->SetUniformMatrix4f("view", cam->getViewMat());
//    shdr->SetUniformMatrix4f("projview", cam->getProjMat());
//    shdr->SetUniform3f("position", position.x, position.y, position.z);
//    shdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);
//    //shdr->SetUniform1i("tex", tex->GetRenderID());  // Wrong! Texture units and textures are not the same.
//    //  See: https://stackoverflow.com/questions/8866904/differences-and-relationship-between-glactivetexture-and-glbindtexture
//    shdr->SetUniform1i("tex", tex->GetTextureSlot());
//    va->Bind();
//    ib->Bind();
//    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    //glDisable(GL_CULL_FACE); // It is irritating that Earth disappears when seen from back/in side
//    glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0);
//    //glEnable(GL_CULL_FACE);
//    ib->Unbind();
//    va->Unbind();
//    shdr->Unbind();
//    return;
//}
//glm::vec3 SubSolar::getLoc3D_NS(float lat, float lon, float height, float radius) {
//    float m_Radius = 1.0f;
//    float w = cos(lat) * (radius + height);
//    float x = cos(lon) * w;
//    float y = sin(lon) * w;
//    float z = sin(lat) * (radius + height);
//    return glm::vec3(x, y, z);
//}
//void SubSolar::genGeom(float radius) {
//    float rLat, rLon;
//    unsigned int meshV = 16; // Bands
//    unsigned int meshU = 32; // Facets
//
//    for (unsigned int v = 0; v <= meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
//        rLat = (pif * v / meshV) - pi2f;
//        for (unsigned int u = 0; u <= meshU; u++) {
//            rLon = (tauf * u / meshU) - pif;
//            glm::vec3 loc = getLoc3D_NS(rLat, rLon, 0.0f, radius);     // position of vertex
//            glm::vec3 nml = glm::normalize(loc);  // Insolation normal, always NS (or G8?)
//            m_verts.push_back({ loc, nml, glm::vec2((float)u / (float)meshU, (float)v / (float)meshV) });
//            if (u < meshU && v < meshV) {
//                m_tris.push_back({ (meshU + 1) * v + u , (meshU + 1) * (v + 1) + u, (meshU + 1) * (v + 1) + 1 + u });
//                m_tris.push_back({ (meshU + 1) * (v + 1) + 1 + u, (meshU + 1) * v + u + 1, (meshU + 1) * v + u });
//            }
//        }
//    }
//}


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
    //int at1 = 0;
    //glGetIntegerv(GL_ACTIVE_TEXTURE, &at1);
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
void SubLunar::draw(Camera* cam) {
    //std::cout << "Drawing SubLunar\n";
    shdr->Bind();
    // These are wrong! planetoid.glsl takes world and projview matrices
    //shdr->SetUniformMatrix4f("view", cam->getViewMat());
    //shdr->SetUniformMatrix4f("projection", cam->getProjMat());
    shdr->SetUniform3f("position", position.x, position.y, position.z);
    shdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);
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
LLD BodyGeometry::toLocalCoords(LLD loc, const bool rad) {
    // Local coords are the same as BodyGeometry native
    return loc;
}
LLD BodyGeometry::fromLocalCoords(LLD loc, const bool rad) {
    // Local coords are the same as BodyGeometry native
    return loc;
}
glm::vec3 BodyGeometry::getLoc3D(const LLD loc, const bool rad) {
    /// <summary>
    /// Takes latitude and longitude in radians to glm::vec3 Cartesian coordinate in world/object space
    /// </summary>
    /// <param name="rLat">Latitude given in -pi/2 to pi/2 radians</param>
    /// <param name="rLon">Longitude given in -pi to pi radians from Greenwich meridian (east of south)</param>
    /// <param name="height">Optional height above geoid surface in km, defaults to 0.0</param>
    /// <param name="rad">Optional flag to indicate if angles are in radians, defaults to false</param>
    /// <returns>Cartesian coordinates glm::vec3 in world space units for currently active geoid geometry</returns>
    LLD myloc = toLocalCoords(loc, rad);
    if (pos_mode2 == nullptr) return (this->*pos_mode1)(myloc, rad);
    return morph_param * (this->*pos_mode2)(myloc, rad) + (1.0f - morph_param) * (this->*pos_mode1)(myloc, rad);
}
glm::vec3 BodyGeometry::getNml3D(const LLD loc, const bool rad) {
    LLD myloc = toLocalCoords(loc, rad);
    if (nml_mode2 == nullptr) return (this->*nml_mode1)(myloc, rad);
    return glm::normalize(morph_param * (this->*nml_mode2)(myloc, rad) + (1.0f - morph_param) * (this->*nml_mode1)(myloc, rad));
}
glm::vec3 BodyGeometry::getLoc3D_NS(const LLD loc, const bool rad) {
    double lat = loc.lat, lon = loc.lon;
    if (!rad) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
    double h = (double)radius + loc.dst; // Height above center in world coordinates
    float w = (float)(cos(lat) * h);
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    float z = (float)(sin(lat) * h);
    return glm::vec3(x, y, z);
}
glm::vec3 BodyGeometry::getNml3D_NS(const LLD loc, const bool rad) {
    //return glm::normalize(getLoc3D_NS(rLat, rLon));
    double lat = loc.lat, lon = loc.lon; // To keep const promise
    if (!rad) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
    float w = (float)(cos(lat));                     // Slightly simplified, as radius and height are irrelevant on a sphere
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    float z = (float)(sin(lat));
    return glm::normalize(glm::vec3(x, y, z));
}
glm::vec3 BodyGeometry::getLoc3D_AE(const LLD loc, const bool rad) {
    double lat = loc.lat, lon = loc.lon;
    if (!rad) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
    // NOTE: This choice of w gives an equator equal to that of a spherical Earth !!!
    //       Should really use a scale where north pole to equator distance matches
    double w = (pi2 - lat) * (double)radius / pi2;
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    return glm::vec3(x, y, loc.dst);
}
glm::vec3 BodyGeometry::getNml3D_AE(const LLD loc, const bool rad) {
    return glm::vec3(0.0f, 0.0f, 1.0f);
}
glm::vec3 BodyGeometry::getLoc3D_ER(const LLD loc, const bool rad) {
    double lat = loc.lat, lon = loc.lon;
    if (!rad) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
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
glm::vec3 BodyGeometry::getNml3D_ER(const LLD loc, const bool rad) {
    return glm::vec3(1.0f, 0.0f, 0.0f);
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
            glm::vec3 loc = getLoc3D(latlon, true);     // position of vertex
            glm::vec3 nms = getNml3D_NS(latlon, true);  // Insolation normal, always NS (or G8?)
            glm::vec3 nml = getNml3D(latlon, true);     // Geometry normal, for camera lighting
            vertices.push_back({ loc, nms, nml, glm::vec2(u / (float)(m_meshU) + texoffset_x / 8192.0f, v / (float)m_meshV) + texoffset_y / 4096.0f, BLACK });
            //vertices.push_back({ loc, nms, nml, glm::vec2(u / (float)(m_meshU*2) + texoffset_x / 8192.0f, v / (float)m_meshV) + texoffset_y / 4096.0f, BLACK });
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
            vertices[i].position = getLoc3D(latlon, true);
            vertices[i].surface_normal = getNml3D(latlon, true);
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
        pos = (locref->*locpos)(latlon, true); // Dynamically calls earth->getLoc3D() (Earth2::getLoc3D())
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
        pos = (locref->*locpos)(latlon, true);
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
        pos = (locref->*locpos)(latlon, true); // Dynamically calls earth->getLoc3D() (Earth2::getLoc3D())
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
LLD GreatCircleArc::calcGreatArc(const LLD lld1, const LLD lld2, const double f, const bool rad) {
    double lat1 = lld1.lat;
    double lon1 = lld1.lon;
    double lat2 = lld2.lat;
    double lon2 = lld2.lon;
    if (!rad) {
        lat1 *= deg2rad; // doubles
        lon1 *= deg2rad;
        lat2 *= deg2rad;
        lon2 *= deg2rad;
    }
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
    if (!rad) ret *= rad2deg;
    return ret;
}
void GreatCircleArc::findSplitPoint() {
    // Check for pole crossing
    
    double det = std::max(start_point.lon, end_point.lon) - std::min(start_point.lon, end_point.lon);
    if (std::abs(det - 180.0) < tiny) {
        std::cout << "GreatCircleArc::findSplitPoint(): Crossing Pole! det=" << det - 180.0 << '\n';
        // The below might not work, see comments about poles in GreatCircleArc::calcGreatArc()
        split_point.lon = (end_point.lon - start_point.lon) / 2;
        split_point.dst = 0.0;
        if (det < 0.0) split_point.lat = -90.0;  // south pole
        else split_point.lat = 90.0;             // north pole
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
            loc = calcGreatArc(start_point, end_point, t, false); // For now, start & end are in degrees
            pos = (locref->*locpos)(loc, false); // Dynamically calls getLoc3D() of the geometry body
            path->addPoint(pos);
        }
    } else {
        // run start to split, then split path, then split to end
        // points spaced according to arc length?
        for (double t = 0.0; t < 1.0; t += 1.0 / steps) {
            loc = calcGreatArc(start_point, split_point, t, false); // For now, start & end are in degrees
            pos = (locref->*locpos)(loc, false); // Dynamically calls getLoc3D() of the geometry body
            path->addPoint(pos);
        }
        path->addSplit(
            (locref->*locpos)(calcGreatArc(start_point, split_point, 1.0, false), false),
            (locref->*locpos)(calcGreatArc(split_point, end_point, 0.0, false), false)
        );
        for (double t = 0.0 + (1.0 / steps); t < 1.0 + tiny; t += 1.0 / steps) {
            loc = calcGreatArc(split_point, end_point, t, false); // For now, start & end are in degrees
            pos = (locref->*locpos)(loc, false); // Dynamically calls getLoc3D() of the geometry body
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
        pos = (locref->*locpos)(tpos, true);
        path->addPoint(pos);
    }
    for (double lon = 0.0; lon <= start_eclip_lon; lon += deg2rad) {
        gpos = scene->astro->calcEc2Geo(0.0, lon, epsilon);
        tpos = scene->astro->calcDecRA2GP(gpos, NO_DOUBLE);
        pos = (locref->*locpos)(tpos, true);
        path->addPoint(pos);
    }
    gpos = scene->astro->calcEc2Geo(0.0, start_eclip_lon-tiny, epsilon);
    tpos = scene->astro->calcDecRA2GP(gpos, NO_DOUBLE);
    pos = (locref->*locpos)(tpos, true);
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
        //pos = (locref->*locpos)(decra, true);
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
        pos = (locref->*locpos)(spos, true);
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
void DetailedSky::addStars(double magnitude) {
    size_t i = skydotDefs.size();
    for (auto& s : Astronomy::stellarobjects) {
        if (s.vmag < magnitude) addStar(++i, s);
    }
    std::cout << "Stars added: " << i << " of magnitude less than " << magnitude << '\n';
}
void DetailedSky::addStar(size_t unique, Astronomy::stellarobject& star) {
    //std::cout << star.dec << "," << star.ra << " | " << star.pm_dec << "," << star.pm_ra << '\n';
    star.dec *= deg2rad;
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
    return getLoc3D(decra, true);
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
LLD DetailedSky::toLocalCoords(LLD loc, const bool rad) {
    // Convert from Dec/RA (in equinox of date) to planetocentric format
    LLD myloc = loc;
    if (!rad) {
        myloc.lat *= deg2rad;
        myloc.lon *= deg2rad;
    }
    //if (siderealtime) myloc.lon -= scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    myloc.lon = ACoord::rangezero2tau(myloc.lon);
    if (myloc.lon > pi) myloc.lon = myloc.lon - tau;
    if (!rad) {
        myloc.lat *= rad2deg;
        myloc.lon *= rad2deg;
    }
    return myloc;
}
LLD DetailedSky::fromLocalCoords(LLD loc, const bool rad) {
    // Convert from planetocentric (native to BodyGeometry) to Dec/RA (native to DetailedSky)
    LLD myloc = loc;
    if (!rad) {
        myloc.lat *= deg2rad;
        myloc.lon *= deg2rad;
    }
    // !!! FIX: Include Sidereal Time, using siderealtime boolean flag !!!
    myloc.lon = ACoord::rangempi2pi(myloc.lon);
    if (myloc.lon < 0.0) myloc.lon = myloc.lon + tau;
    if (!rad) {
        myloc.lat *= rad2deg;
        myloc.lon *= rad2deg;
    }
    return myloc;
}
void DetailedSky::myGUI() {
    if (ImGui::CollapsingHeader(name.c_str())) {
        ImGui::Checkbox("Use Texture", &textured);
        ImGui::Checkbox("Proper Motion", &propermotion);
        ImGui::Checkbox("Precession", &precession);
        ImGui::Checkbox("Sidereal Time", &siderealtime);
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
    return getLoc3D(subsolar, true);
}

void DetailedEarth::addMoonGP() {
    moongp = new PlanetoidGP(scene, this, MOON, "MoonGP", getMoonGPLocation());
}
glm::vec3 DetailedEarth::getMoonGPLocation() {
    //std::cout << "MoonGP: " << sublunar.lat << ", " << sublunar.lon << "\n";
    return getLoc3D(sublunar, true);
}

LLD DetailedEarth::calcMoon() {  // Gives same results as the previous AA+ v2.49 based function
    double currentJD = scene->astro->getJD_TT();
    LLD qmoon = scene->astro->MoonTrueEquatorial(currentJD, ELP_MPP02);
    double sidtime = scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    double moonHour = -qmoon.lon + sidtime;
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
    //std::cout << sublunar.str() << std::endl;

    //if (do_eccentriclight) { SunLightDir = getNml3D_EE(subsolar.lat, subsolar.lon, 0.0f); return; }
    SunLightDir = getNml3D_NS(subsolar, true);
    if (sungp != nullptr) sungp->setPosition(getSunGPLocation()); // Uses subsolar calculated above
    if (moongp != nullptr) moongp->setPosition(getMoonGPLocation());
    if (celestialgrid != nullptr) celestialgrid->rotations.y = (float)scene->astro->TrueObliquityOfEcliptic(NO_DOUBLE, true);
    return false;
}
LLD DetailedEarth::toLocalCoords(LLD loc, const bool rad) {
    // Local coords are the same as BodyGeometry native
    return loc;
}
LLD DetailedEarth::fromLocalCoords(LLD loc, const bool rad) {
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
        // "Inlunation" the Moon's light falling upon earth
        //ImGui::Checkbox("Lunar insolation", &l.layer->m_scene->m_app->currentEarth->insolation);

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
    earthgp = new PlanetoidGP(scene, this, EARTH, "EarthGP", getNml3D({0.0, 0.0, 0.0}, false));
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
    SunLightDir = getNml3D_NS({ selsun.b0, selsun.l0 }, true);   // Used for insolation in shader
    sunDir = { getLoc3D({ selsun.b0, selsun.l0 }, true), 1.0f }; // Used for Sun location dot, which is morph dependent

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

    earthDir = getLoc3D({ rotY, rotZ, 0.0 }, true);

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
        nullpos = getLoc3D({ 0.0, 0.0, 0.0 }, false);
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
LLD DetailedMoon::toLocalCoords(LLD loc, const bool rad) {
    // Local coords same as BodyGeometry native
    return loc;
}
LLD DetailedMoon::fromLocalCoords(LLD loc, const bool rad) {
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
    }
}


// -------
//  Earth
// -------
Earth::Earth(Scene* scene, std::string mode, unsigned int meshU, unsigned int meshV)
    : m_meshU(meshU), m_meshV(meshV), m_Mode(mode), m_scene(scene) {
    //std::cout << "Earth{" << this << "}(" << scene << "," << mode << "," << meshU << "," << meshV << ") created!\n";
    if (m_Mode.length() == 2) {
        m_sMode = m_Mode;
        m_eMode = "YY";         // Default value
    }
    else if (m_Mode.length() == 4) {
        m_sMode = m_Mode.substr(0, 2);
        m_eMode = m_Mode.substr(2, 2);
    }
    else std::cout << "WARNING! Earth::Earth() requested mode set to value of unsupported length: " << m_Mode << " (must be 2 or 4 long)\n";
    m_dots = m_scene->getDotsFactory();
    m_arrows = m_scene->getArrowsFactory();
    m_cylinders = m_scene->getCylindersFactory();
    m_verts.reserve(((size_t)m_meshU + 1) * ((size_t)m_meshV + 1));
    m_tris.reserve((size_t)(m_meshU * (size_t)m_meshV * (size_t)sizeof(Tri)));
    m_dotcache.reserve(1000);  // Not actually removed when dots are deleted, just set to index maxuint
    m_arrowcache.reserve(100);
    m_polycache.reserve(100);
    locgroups.reserve(10);
    current_earth = mode;
    genGeom();
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
    shdr = m_scene->m_app->getShaderLib()->getShader(EARTH_SHADER);
    smshdr = m_scene->m_app->getShaderLib()->getShader(EARTH_SHADOW_MAP_SHADER);
    sbshdr = m_scene->m_app->getShaderLib()->getShader(EARTH_SHADOW_BOX_SHADER);
    // NOTE: Textures should be pre-flipped in Y, as SOIL2 soils itself when trying to flip large images
    //const std::string daytexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\land_shallow_topo_8192_mdo.png";
    //const std::string daytexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\large map 8192x4096.png";
    //daytex = new Texture(daytexfile, GL_TEXTURE3);
    //const std::string nighttexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\BlackMarble_2012_8192x4096.png";
    //const std::string nighttexfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\BlackMarble_2012_8192x4096_ice.png";
    //nighttex = new Texture(nighttexfile, GL_TEXTURE4);
    daytex = scene->m_app->getTextureLib()->getTexture(EARTH_DAY);
    nighttex = scene->m_app->getTextureLib()->getTexture(EARTH_NIGHT);
    bumptex = scene->m_app->getTextureLib()->getTexture(EARTH_BUMP);
}
Earth::~Earth() {
    std::cout << "Earth destroyed.\n";
    if (!locgroups.empty()) {
        for (auto& lg : locgroups) {
            delete lg;
        }
    }
    //if (!m_polycache.empty()) {
    //    for (auto& p : m_polycache) {
    //        m_scene->deletePolyCurve(p.path);
    //    }
    //    m_polycache.clear();
    //}
    // If there are SubStellarPoints, destroy them if they are not nullptr
    if (!substellarpoints.empty()) for (auto& ssp : substellarpoints) { if (ssp != nullptr) delete ssp; };
    delete nighttex;
    delete daytex;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
    //if (m_sunob != nullptr) delete m_sunob;
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
    if (texture_x != gen_texture_x || texture_y != gen_texture_y) {
        updGeom();
    }
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
        m_param = param; // Use a setter instead of testing every frame !!!
        paramdirty = true;
    }
    if (m_eccen != eccen) {  // pick up param from gui
        m_eccen = eccen;
        paramdirty = true;
    }
    if (m_Mode != current_earth) {  // Move current_earth to gui to be consistent with param above !!!
        m_Mode = current_earth;
        if (m_Mode.length() == 2) {
            m_sMode = m_Mode;
            m_eMode = "YY";         // Default value
        }
        else if (m_Mode.length() == 4) {
            m_sMode = m_Mode.substr(0, 2);
            m_eMode = m_Mode.substr(2, 2);
        }
        else std::cout << "WARNING! Earth::Update() current_earth set to value of unsupported length: " << current_earth << " (must be 2 or 4 long)\n";
        paramdirty = true;
    }
    if (m_flatsunheight != flatsunheight) {
        //std::cout << "Earth::Update() m_flatsunheight: " << m_flatsunheight << " <- flatsunheight: " << flatsunheight << "\n";
        m_flatsunheight = flatsunheight;
        //mydebug = true;
        flatsundirty = true;
    }
    if (m_jd_utc != m_scene->astro->getJD_UTC()) {
        m_jd_utc = m_scene->astro->getJD_UTC();
        m_jd_tt = m_scene->astro->getJD_TT();
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
        for (auto& t : tissotcache.m_Elements) {
            updateTissotIndicatrix(t);
        }
    }
    if (timedirty || paramdirty || flatsundirty) {
        updateSun();
        if (m_subsolar) m_subsolar->setPosition(flatSun);
        if (m_sublunar) m_sublunar->setPosition(flatMoon);
        //if (m_sunob != nullptr) updateSubsolarPoint();
        if (m_moonob != nullptr) updateSublunarPoint();
        for (unsigned int i = 0; i < m_dotcache.size(); i++) {
            if (m_dotcache[i].index != maxuint) {
                glm::vec3 pos = getLoc3D(m_dotcache[i].lat, m_dotcache[i].lon, m_dotcache[i].height);
                m_dots->changeXYZ(m_dotcache[i].index, pos, m_dotcache[i].color, m_dotcache[i].size);
            }
        }
        updateEcliptic();
        if (m_sunpole != maxuint) updateSubsolarPole();
        if (m_sundot != NO_UINT) updateSubsolarDot();
        if (m_suncone != maxuint) updateSubsolarCone_NS();
        for (auto& p : m_polycache) {  // Composite paths/arcs
            if (p.type == NONE) continue;
            if (p.type == MOONTERMINATOR) updateTerminatorTrueMoon(p);
            if (p.type == SUNTERMINATOR) updateTerminatorTrueSun(p);
            if (p.type == SEMITERMINATOR) updateSemiTerminator(p);
            if (p.type == GREATARC) updateGreatArc(p);
            if (p.type == LERPARC) updateLerpArc(p);
        }
        updateSunSectors();  // Scans m_polycache directly, maybe there is a better way !!!
        // Collect in a cache with other celestial paths
        if (suncurve != nullptr && sunpath != nullptr) updateSubsolarPath();
        //for (auto& st : semiterminatorcache) {
        //    updateSemiTerminator(st);
        //}
        for (auto ssp : substellarpoints) {
            if (ssp != nullptr) ssp->update();
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
void Earth::draw(Camera* cam) {  // Alternatively pass a pv matrix = projMat * viewMat, it also saves on mat mults.
    // Shader setup
    if (shadows == SHADOW_MAP) { // m_world.shadows
        smshdr->Bind();
        smshdr->SetUniformMatrix4f("view", cam->getViewMat());
        smshdr->SetUniformMatrix4f("projection", cam->getProjMat());
        smshdr->SetUniformMatrix4f("lightSpaceMatrix", m_scene->getShadowmapOb()->lightSpaceMatrix);
        if (insolation) smshdr->SetUniform3f("sunDir", SunLightDir.x, SunLightDir.y, SunLightDir.z);
        if (!insolation) smshdr->SetUniform3f("sunDir", 0.0f, 0.0f, 0.0f);
        if (w_linsol) smshdr->SetUniform3f("moonDir", MoonLightDir.x, MoonLightDir.y, MoonLightDir.z);
        if (!w_linsol) smshdr->SetUniform3f("moonDir", 0.0f, 0.0f, 0.0f);
        if (w_refract) smshdr->SetUniform1f("refraction", 1.0f);
        else smshdr->SetUniform1f("refraction", 0.0f);
        smshdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);
        smshdr->SetUniform1i("dayTexture", daytex->GetTextureSlot());
        smshdr->SetUniform1i("nightTexture", nighttex->GetTextureSlot());
        smshdr->SetUniform1i("depthTexture", m_scene->getShadowmapOb()->depthmap);
        smshdr->SetUniform1f("alpha", m_alpha);
    }
    else if (shadows == SHADOW_BOX) {
        sbshdr->Bind();
        if (insolation) sbshdr->SetUniform3f("sunDir", SunLightDir.x, SunLightDir.y, SunLightDir.z);
        if (!insolation) sbshdr->SetUniform3f("sunDir", 0.0f, 0.0f, 0.0f);
        if (w_linsol) sbshdr->SetUniform3f("moonDir", MoonLightDir.x, MoonLightDir.y, MoonLightDir.z);
        if (!w_linsol) sbshdr->SetUniform3f("moonDir", 0.0f, 0.0f, 0.0f);
        if (w_refract) sbshdr->SetUniform1f("refraction", 1.0f);
        else sbshdr->SetUniform1f("refraction", 0.0f);
        sbshdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);
        sbshdr->SetUniformMatrix4f("view", cam->getViewMat());
        sbshdr->SetUniformMatrix4f("projection", cam->getProjMat());

        sbshdr->SetUniform1f("far_plane", m_scene->getShadowboxOb()->far);
        glm::vec3 lightpos = getSubsolarXYZ();
        sbshdr->SetUniform3f("lightPos", lightpos.x, lightpos.y, lightpos.z);
        sbshdr->SetUniform1i("dayTexture", daytex->GetTextureSlot());
        sbshdr->SetUniform1i("nightTexture", nighttex->GetTextureSlot());
        sbshdr->SetUniform1i("depthTexture", m_scene->getShadowboxOb()->depthCubemap);
        sbshdr->SetUniform1f("alpha", m_alpha);

    }
    else { // earth.glsl shader
        shdr->Bind();
        glm::mat4 pv = cam->getProjMat() * cam->getViewMat();
        shdr->SetUniformMatrix4f("projview", pv);
        shdr->SetUniformMatrix4f("world", glm::mat4(1.0f)); // Identity matrix, as shader is now shard with DetailedEarth (a SceneObject)
        //glm::mat3 worldnormal = glm::transpose(glm::inverse(glm::mat3(worldmatrix)));
        shdr->SetUniformMatrix3f("worldnormal", glm::mat3(1.0f));
        if (insolation) shdr->SetUniform3f("sunDir", SunLightDir.x, SunLightDir.y, SunLightDir.z);
        if (!insolation) shdr->SetUniform3f("sunDir", 0.0f, 0.0f, 0.0f);
        if (w_linsol) shdr->SetUniform3f("moonDir", MoonLightDir.x, MoonLightDir.y, MoonLightDir.z);
        if (!w_linsol) shdr->SetUniform3f("moonDir", 0.0f, 0.0f, 0.0f);
        if (w_refract) shdr->SetUniform1f("refraction", 1.0f);
        else shdr->SetUniform1f("refraction", 0.0f);
        shdr->SetUniform3f("lightDir", cam->CamLightDir.x, cam->CamLightDir.y, cam->CamLightDir.z);
        // Following 4 entries adjusted to be DetailedEarth compatible (so they can share shader, and not cause further shader multiplication)
        shdr->SetUniform1i("texture1", daytex->GetTextureSlot());
        shdr->SetUniform1i("texture2", nighttex->GetTextureSlot());
        shdr->SetUniform1i("texture3", bumptex->GetTextureSlot());
        shdr->SetUniform1f("sunBumpScale", 0.0005f);
        shdr->SetUniform1f("lightBumpScale", 0.015f);
        shdr->SetUniform1f("alpha", m_alpha);
        shdr->SetUniform1f("twilight", w_twilight ? 1.0f : 0.0f);
        // tinting arctics and tropics
        shdr->SetUniform4f("tintarctics", glm::vec4(0.0f));
        shdr->SetUniform4f("tinttropics", glm::vec4(0.0f));
        shdr->SetUniform1f("obliquity", 0.0f);
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
    //if (m_sunob != nullptr) m_sunob->draw(cam);
    if (m_moonob != nullptr) m_moonob->draw(cam);
    for (auto& lg : locgroups) {
        for (auto l : lg->locations) {
            l->Draw(cam);
        }
    }
    if (suncurve != nullptr && sunpath != nullptr) suncurve->draw(m_scene->w_camera);
    //for (auto& t : tissotcache.m_Elements) {
    //    t.curve->draw();
    //}
}
unsigned int Earth::addLocGroup() {
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
    glm::vec3 sPos = getLoc3D_XX(m_sMode, rLat, rLon, height);
    glm::vec3 ePos = getLoc3D_XX(m_eMode, rLat, rLon, height);
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
    m_sMode = m_Mode.substr(0, 2);
    m_eMode = m_Mode.substr(2, 2);
    glm::vec3 sPos = getNml3D_XX(m_sMode, rLat, rLon, height);
    glm::vec3 ePos = getNml3D_XX(m_eMode, rLat, rLon, height);
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
    // "CG" is used by Location::FlatSun to signify Current Geometry, whichever that happens to be. So don't use that here.
    if (mode == "NS") return getLoc3D_NS(rLat, rLon, height); // Normal Sphere
    if (mode == "AE") return getLoc3D_AE(rLat, rLon, height); // Azimuthal Equidistant
    if (mode == "LB") return getLoc3D_LB(rLat, rLon, height); // Lambertian Azimuthal Equal Area
    if (mode == "LA") return getLoc3D_LA(rLat, rLon, height); // Lambertian ditto
    if (mode == "MW") return getLoc3D_MW(rLat, rLon, height); // Mollweide
    if (mode == "ER") return getLoc3D_ER(rLat, rLon, height);
    if (mode == "RC") return getLoc3D_RC(rLat, rLon, height);
    if (mode == "E8") return getLoc3D_E8(rLat, rLon, height);
    if (mode == "EX") return getLoc3D_EX(rLat, rLon, height);
    if (mode == "EE") return getLoc3D_EE(rLat, rLon, height);
    return glm::vec3(0.0f);
}
glm::vec3 Earth::getNml3D_XX(std::string mode, double rLat, double rLon, double height) {
    // "CG" is used by Location::FlatSun to signify Current Geometry, whichever that happens to be. So don't use that here.
    if (mode == "NS") return getNml3D_NS(rLat, rLon, height);
    if (mode == "AE") return getNml3D_AE(rLat, rLon, height);
    if (mode == "LB") return getNml3D_LB(rLat, rLon, height);
    if (mode == "LA") return getNml3D_LA(rLat, rLon, height);
    if (mode == "MW") return getNml3D_MW(rLat, rLon, height);
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
    // Projection point
    //double lon0 = 0.0;
    //double lat0 = 0.0;
    // NOTE: This choice of w gives an equator equal to that of a spherical Earth !!!
    //       Should really use a scale where north pole to equator distance matches
    double w = (pi2 - rLat) * m_Radius / pi2;
    float x = (float)(cos(rLon) * w);
    float y = (float)(sin(rLon) * w);
    return glm::vec3(x, y, height);
}
glm::vec3 Earth::getNml3D_AE(double rLat, double rLon, double height) {
    return glm::vec3(0.0f, 0.0f, 1.0f);
}
glm::vec3 Earth::getLoc3D_LB(double rLat, double rLon, double height) {

    // Use Earth::m_lat0 and Earth::m_lon0 as projection point
    //if (rLat == 0.0 && rLon == 0.0) return glm::vec3(height-(float)tiny, 0.0f, 0.0f);
    if (abs(rLat) <= tiny) rLat = -tiny;
    if (rLat + pi2 < verytiny) rLat = -(pi2 - verytiny); // South pole
    // FIXME! Can only use poles as projection points because vertices not on pole or seam are shared
    // CANT FIX! When poles are not on the edge, some quads (those antipodal to projection point) are mapped to thin curves along edge of map
    //if (rLon + pi < verytiny) rLon = -(pi - verytiny);   // Pacific seam
    // https://mathworld.wolfram.com/LambertAzimuthalEqual-AreaProjection.html
    double kprime = sqrt(2 / (1 + sin(m_lat0) * sin(rLat) + cos(m_lat0) * cos(rLat) * cos(rLon - m_lon0)));
    float y = (float)(kprime * cos(rLat) * sin(rLon - m_lon0));
    float z = (float)(kprime * (cos(m_lat0) * sin(rLat) - sin(m_lat0) * cos(rLat) * cos(rLon - m_lon0)));
    // Debug weird edge phenomena
    //if (y < -2.0f) {
    //    std::cout << "LB Geometry Overflow at (lat,lon): (" << rLat * rad2deg << "," << rLon * rad2deg << ")\n";
    //}
    
    // https://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projection
//    glm::vec3 loc = getLoc3D_NS(rLat, rLon, height);
//    double w = sqrt(2 / (1 + loc.z));  // Project to plane z=1
//    double x = (float)(w * loc.x);
//    double y = (float)(w * loc.y);
    //float r = (float)(sqrt(2) * sin(0.5 * (pi2 - rLat)));
    //float y = (float)(r * sin(rLon));
    //float x = (float)(r * cos(rLon));
    return glm::vec3(height, y, z);
}
glm::vec3 Earth::getNml3D_LB(double rLat, double rLon, double height) {
    return glm::vec3(1.0f, 0.0f, 0.0f);
}
glm::vec3 Earth::getLoc3D_LA(double rLat, double rLon, double height) {
    // Projection point
    //double lon0 = 0.0;
    //double lat0 = pi2;
    // https://mathworld.wolfram.com/LambertAzimuthalEqual-AreaProjection.html
    //double kprime = sqrt(2 / (1 + sin(lat0) * sin(rLat) + cos(lat0) * cos(rLat) * (cos(rLon - lon0))));
    //float x = (float)(kprime * cos(rLat) * sin(rLon - lon0));
    //float y = (float)(kprime * (cos(lat0) * sin(rLat) - sin(lat0) * cos(rLat) * cos(rLon - lon0)));
    // https://en.wikipedia.org/wiki/Lambert_azimuthal_equal-area_projection
    //glm::vec3 loc = getLoc3D_NS(rLat, rLon, height);
    //double w = sqrt(2 / (1 - loc.z));
    //double x = (float)(w * loc.x);
    //double y = (float)(w * loc.y);
    float r = (float)(sqrt(2) * sin(0.5 * (pi2 - rLat)));
    float y = (float)(r * sin(rLon));
    float x = (float)(r * cos(rLon));
    return glm::vec3(x, y, height);
}
glm::vec3 Earth::getNml3D_LA(double rLat, double rLon, double height) {
    return glm::vec3(0.0f, 0.0f, 1.0f);
}
glm::vec3 Earth::getLoc3D_MW(double rLat, double rLon, double height) {
    while (rLon > pi+verytiny) rLon -= tau;
    while (rLon < -pi-verytiny) { rLon += tau; }
    // Projection point
    double lon0 = 0.0;
    //double lat0 = 0.0;   // My texturemap does not allow shifting latitudes
    // https://mathworld.wolfram.com/MollweideProjection.html
    double theta = 0.0;
    if (pi2 - abs(rLat) > tiny) {
        double thetaprev = rLat;
        theta = 2.0 * asin(2 * rLat / pi);
        double dtheta = pi; // Just to make sure the loop runs at least once
        while (abs(dtheta) > 0.000001) {
            thetaprev = theta;
            dtheta = -((thetaprev + sin(thetaprev) - pi * sin(rLat)) / (1 + cos(thetaprev)));
            theta = thetaprev + dtheta;
        }
        theta *= 0.5;
    }
    else theta = rLat;
    double w = sqrt(2.0);
    float y = (float)(w * 2.0 / pi * (rLon - lon0) * cos(theta)); // twopi is 2/pi
    float x = (float)-(w * sin(theta));
    return glm::vec3(x, y, height);
}
glm::vec3 Earth::getNml3D_MW(double rLat, double rLon, double height) {
    return glm::vec3(0.0f, 0.0f, 1.0f);
}
glm::vec3 Earth::getLoc3D_ER(double rLat, double rLon, double height) {
    while (rLon > pi + verytiny) rLon -= tau;
    while (rLon < -pi - verytiny) rLon += tau;
    //if (rLat > pi2+tiny || rLat < -pi2-tiny) std::cout << "WARNING: Earth::getLoc3D_ER(): rLat is out of range: " << rLat << "\n";
    //if (rLon > pi+tiny || rLon < -pi-tiny) std::cout << "WARNING: Earth::getLoc3D_ER(): rLon is out of range: " << rLon << "\n";
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
    glm::vec3 nml = getLoc3D_E8(rLat, rLon, 0.0f);
    double a = 6378137.0f;     // Semimajor axis
    double f = 298.257223563f; // inverse flattening
    double b = a - a / f;      // Semiminor axis
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
LLD Earth::calcHADec2LatLon(LLD radec) {
    return m_scene->astro->calcDecHA2GP(radec);
    //// Map HA [0 ; pi) -> [0 ; -pi) and [pi ; tau) -> [pi ; 0)
    //if (!rad) {
    //    radec.lat *= deg2rad;
    //    radec.lon *= deg2rad;
    //}
    //if (radec.lon < pi) radec.lon = -radec.lon;
    //else radec.lon = tau - radec.lon;  // subsolar.lon is now -pi to pi east of south
    //if (!rad) {
    //    radec.lat *= rad2deg;
    //    radec.lon *= rad2deg;
    //}
    //return radec;
}
glm::vec3 Earth::calcHADec2Dir(LLD radec) {
    radec = calcHADec2LatLon(radec);
    return getLoc3D(radec.lat, radec.lon, 0.0);
}
LLD Earth::calcRADec2LatLon(LLD radec, double jd_utc) { // Needs JD as UTC
    return m_scene->astro->calcDecRA2GP(radec, jd_utc);
}
LLD Earth::getXYZtoLLD_NS(glm::vec3 pos) {
    // NOTE: Gives height above Center of Earth, not height above Surface!! (i.e. geocentric, not topocentric)
    // Keep in mind that calcTerminator() uses this !!! So don't get too smart with it.
    // std::cout << "XYZ: " << pos.x << "," << pos.y << "," << pos.z << "\n";
    float diag = sqrt(pos.x * pos.x + pos.y * pos.y);
    if (diag < 0.0000001f && pos.z + (float)tiny > 1.0f) return { pi2, 0.0, 0.0 }; // atan2 seems to flip out at the singularities
    if (diag < 0.0000001f && pos.z - (float)tiny < -1.0f) return { -pi2, 0.0, 0.0 };
    return { atan2(pos.z, diag), atan2(pos.y, pos.x), glm::length(pos) };
}
LLD Earth::getXYZtoLLD_AE(glm::vec3 pos) {
    // Check if XY is outside the AE disc !!! - I.e. if w below is larger than radius
    LLD ret;
    ret.dst = pos.z;
    double w = sqrt(pos.x * pos.x + pos.y * pos.y);
    ret.lon = atan2(pos.y, pos.x); // = asin(y/w);
    ret.lat = pi2f - (w * pi2f) / m_Radius;
    return ret;
}
double Earth::calcHorizonDip(double height) {
    // For sextant dip correction, so maybe rename this function to something more appropriate.
    // height in meters above surface, returns arc minutes - Figure out validity for large heights in comparison to full formula
    // Source: https://www.madinstro.net/sundry/navsext.html
    return 1.753 * sqrt(height);
}
double Earth::calcSunSemiDiameter(double jd_tt) {
    // Sun semidiameter in arc minutes as apparent from Earth - should it support radians too? Add when required.
    if (jd_tt == NO_DOUBLE) jd_tt = m_scene->astro->getJD_TT();
    // 955.63 from CAADiameters::SunSemidiameterA()
    return (959.63 / m_scene->astro->getEcDst(A_EARTH, jd_tt)) / 60.0;
}
double Earth::calcRefractionAlmanac(double elevation, double temperature, double pressure) {
    // returns refraction correction in arcminutes. Takes elevation in degrees, temperature in Celcius (centigrade), pressure in hPa (mbar)
    // Source: https://www.madinstro.net/sundry/navsext.html
    return (0.267 * pressure / (temperature + 273.15)) / tan(deg2rad * (elevation + 0.04848 / (tan(deg2rad * elevation) + 0.028)));
}
double Earth::calcRefractionBennett(double elevation, double temperature = 10, double pressure = 1010) {
    // returns refraction correction in arcminutes. Takes elevation in degrees, temperature in Celcius (centigrade), pressure in hPa (mbar)
    // Source: J.Meeus AA1998
    // AA+: CAARefraction::RefractionFromApparent()
    if (elevation <= -1.6962987799993996)
        elevation = -1.6962987799993996;
    return (1 / (tan(deg2rad * (elevation + (7.31 / (elevation + 4.4))))) + 0.0013515) * (pressure / 1010 * 283 / (273 + temperature)) / 60;
}
double Earth::calcBrianLeakeDistance(const double elevation, bool rad) {
    double ea = elevation;

    // Convert to degrees if input was in radians
    if (rad) ea *= rad2deg;
    
    // "triangulated distance" in miles
    // Adjacent side of triangle with angle elevation and opposite height of polaris
    // Brian Leake used https://www.calculator.net/right-triangle-calculator.html for this part
    double da = sqrt(pow(3958.73635 / sin(ea * deg2rad), 2) - 3958.73635 * 3958.73635);
    //std::cout << "BL: at " << ea << "degrees, triangle side b is " << da << '\n';
    // "celestial refraction" in miles
    // - Formula provided to me in youtube comment
    //double rc = ((90.0 - ea) / 90.0) * (da * (ea / 100.0));
    // - What seems to be a more reasonable formula
    double rc = ((90.0 - ea) / 90.0) * (da * ((90.0 - ea) / 100.0));

    // "true distance" in miles
    double dt = da - rc;

    return dt;
}
double Earth::calcBrianLeakeAngle(double elevation, bool rad) {
    return rad ? (calcBrianLeakeDistance(elevation, rad) / 69.1) * deg2rad : (calcBrianLeakeDistance(elevation, rad) / 69.1);
}
Earth::Intersection Earth::calcSumnerIntersection(LLD lld1, LLD lld2, bool rad) {
    // Returns NO_DOUBLE for all values if there are no intersections (circles are concentric or antipodal)
    // Source: https://gis.stackexchange.com/questions/48937/calculating-intersection-of-two-circles
    if (!rad) {
        lld1.lat *= deg2rad; lld1.lon *= deg2rad; lld1.dst *= deg2rad;
        lld2.lat *= deg2rad; lld2.lon *= deg2rad; lld2.dst *= deg2rad;
    }
    // transform from spherical to cartesian coordinates
    LLD pos1 = { cos(lld1.lon) * cos(lld1.lat), sin(lld1.lon) * cos(lld1.lat), sin(lld1.lat) };
    LLD pos2 = { cos(lld2.lon) * cos(lld2.lat), sin(lld2.lon) * cos(lld2.lat), sin(lld2.lat) };
    // q equal to pos1 dot pos2
    double q = pos1.lat * pos2.lat + pos1.lon * pos2.lon + pos1.dst * pos2.dst;
    double q2 = q * q;
    // q2 == 1.0 gives DIV0 in the following, and indicates that the points coincide or are antipodal.
    if (abs(q2 - 1.0) < verytiny) {
        std::cout << "Earth::calcSumnerIntersection() the circles are not intersecting! (q*q is very close to 1.0)\n";
        return { {NO_DOUBLE, NO_DOUBLE, NO_DOUBLE},{NO_DOUBLE, NO_DOUBLE, NO_DOUBLE} };
    }
    // pos0 will be a unique point on the line of intersection of the two planes defined by the two distance circles
    double a = (cos(lld1.dst) - cos(lld2.dst) * q) / (1 - q2);
    double b = (cos(lld2.dst) - cos(lld1.dst) * q) / (1 - q2);
    // pos0 is a linear combination of pos1 and pos2 with the parameters a and b
    LLD pos0 = { a * pos1.lat + b * pos2.lat, a * pos1.lon + b * pos2.lon, a * pos1.dst + b * pos2.dst };
    // n equal to pos1 cross pos2, normal to both
    LLD n = { pos1.lon * pos2.dst - pos1.dst * pos2.lon, pos1.dst * pos2.lat - pos1.lat * pos2.dst, pos1.lat * pos2.lon - pos1.lon * pos2.lat };
    // t = sqrt((1.0 - dot(pos0, pos0)) / glm::dot(n, n)); (a vector dot itself is of course the square of its magnitude
    double t = sqrt((1.0 - (pos0.lat * pos0.lat + pos0.lon * pos0.lon + pos0.dst * pos0.dst)) / (n.lat * n.lat + n.lon * n.lon + n.dst * n.dst));
    //isect1 = pos0 + t * n and isect2 = pos0 - t * n, where t is a scalar
    LLD isect1 = { pos0.lat + t * n.lat, pos0.lon + t * n.lon, pos0.dst + t * n.dst };
    LLD isect2 = { pos0.lat - t * n.lat, pos0.lon - t * n.lon, pos0.dst - t * n.dst };
    // Transform back to spherical coordinates - Are isect1 & 2 always unit vectors? !!!
    LLD ll1 = { atan2(isect1.dst, sqrt(isect1.lat * isect1.lat + isect1.lon * isect1.lon)),
        atan2(isect1.lon, isect1.lat), sqrt(isect1.lat * isect1.lat + isect1.lon * isect1.lon + isect1.dst * isect1.dst) };
    LLD ll2 = { atan2(isect2.dst, sqrt(isect2.lat * isect2.lat + isect2.lon * isect2.lon)),
        atan2(isect2.lon, isect2.lat), sqrt(isect2.lat * isect2.lat + isect2.lon * isect2.lon + isect2.dst * isect2.dst) };
    // if degrees were passed in, return degrees rather than radians
    if (!rad) {
        ll1 = { ll1.lat * rad2deg, ll1.lon * rad2deg, ll1.dst * rad2deg };
        ll2 = { ll2.lat * rad2deg, ll2.lon * rad2deg, ll2.dst * rad2deg };
    }
    return { ll1, ll2 };
}

Earth::Intersection Earth::calcSumnerIntersection(double lat1, double lon1, double rad1, double lat2, double lon2, double rad2, bool rad) {
    return calcSumnerIntersection({ lat1, lon1, rad1 }, { lat2, lon2, rad2 }, rad);
}
LLD Earth::getSextantLoc(size_t index) {
    // Should probably verify vector bounds and such, but this will be replaced with a more integrated cache soon,
    // or entirely replaced by SubStellarPoint if that turns out well.
    return { tissotcache[index].lat, tissotcache[index].lon, tissotcache[index].radius };
}

LLD Earth::getSun(double jd) {
    // If jd == 0 get Sun GHA,Dec for current JD, else get Sun for provided JD - handled in getDecGHA()
    return m_scene->astro->getDecGHA(A_SUN, jd);
}
LLD Earth::getSubsolar(double jd) {
    return calcHADec2LatLon(m_scene->astro->getDecGHA(A_SUN, jd));
}
LLD Earth::getPlanet(Planet planet, double jd) {
    return m_scene->astro->getDecGHA(planet, jd);
}
void Earth::CalcMoon() {
    double currentJD = m_scene->astro->getJD_TT();
    if (m_moonJD == currentJD) return;
    double elon = AMoon::EclipticLongitude(currentJD);  // lambda
    double elat = AMoon::EclipticLatitude(currentJD);   // beta
    double Epsilon = AEarth::TrueObliquityOfEcliptic(currentJD);
    LLD equa = Spherical::Ecliptic2Equatorial2(elon, elat, Epsilon);
    m_moonDist = AMoon::RadiusVector(currentJD); // RadiusVector() returns in km
    m_moonRA = equa.lon;
    m_moonDec = equa.lat;
    m_moonHour = m_moonRA - m_scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    double w = cos(m_moonDec);
    MoonLightDir.x = (float)(cos(m_moonHour) * w);
    MoonLightDir.y = (float)(sin(m_moonHour) * w);
    MoonLightDir.z = (float)(sin(m_moonDec));
    m_moonJD = currentJD;
}
LLD Earth::getMoon(double jd_utc) {
    if (jd_utc == 0.0) return LLD{ m_moonDec, m_moonHour, m_moonDist };
    return CalcMoonJD(jd_utc);
}
LLD Earth::CalcMoonJD(double jd_utc) {
    // NOTE: Returns position of Moon at JD, does NOT update m_moon*
    //double gsidtime = CAASidereal::ApparentGreenwichSiderealTime(jd_utc); // Nonstd JD, don't just pick from World
    double gsidtime = rad2hrs * m_scene->astro->ApparentGreenwichSiderealTime(jd_utc, true); // Nonstd JD, don't just pick from World
    double elon = AMoon::EclipticLongitude(EDateTime::getJDUTC2TT(jd_utc));  // lambda
    double elat = AMoon::EclipticLatitude(EDateTime::getJDUTC2TT(jd_utc));   // beta
    double Epsilon = AEarth::TrueObliquityOfEcliptic(EDateTime::getJDUTC2TT(jd_utc));
    LLD equa = Spherical::Ecliptic2Equatorial2(elon, elat, Epsilon);
    return { equa.lat, equa.lon - gsidtime, m_moonDist }; // m_moonDist?
}
size_t Earth::addDot(double lat, double lon, double height, float size, glm::vec4 color, bool radians) {
    if (!radians) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
    glm::vec3 pos = getLoc3D(lat, lon, height);
    size_t index = m_dots->addXYZ(pos, color, size);
    m_dotcache.push_back({ color, lat, lon, height, size, index });
    return m_dotcache.size() - 1;
}
void Earth::changeDotLoc(size_t dotindex, double lat, double lon, double height, bool radians) {
    if (!radians) {
        lat *= deg2rad;
        lon *= deg2rad;
    }
    m_dots->changeXYZ(m_dotcache[dotindex].index, getLoc3D(lat, lon, height), m_dotcache[dotindex].color, m_dotcache[dotindex].size);
}
void Earth::deleteDot(size_t index) {
    m_dots->remove(m_dotcache[index].index);
    m_dotcache[index].index = maxuint;
}
size_t Earth::addFirstPointAries() {
    double lon = -m_scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    lon = ACoord::rangempi2pi(lon);
    glm::vec3 pos = getLoc3D(0.0, lon, 0.0);
    size_t index = m_dots->addXYZ(pos, GREEN, 0.01f);
    m_dotcache.push_back({ GREEN, 0.0, lon, 0.0, 0.01f, index });
    return m_dotcache.size() - 1;
}
void Earth::updateFirstPointAries(size_t index) {
    double lon = -m_scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true);
    lon = ACoord::rangempi2pi(lon);
    m_dotcache[index].lon = lon;
    glm::vec3 pos = getLoc3D(0.0, lon, 0.0);
    m_dots->changeXYZ(m_dotcache[index].index, pos, m_dotcache[index].color, m_dotcache[index].size);
}
// Arrows
void Earth::addArrow3DTrueSun(float length, float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present, or risk adding a duplicate
    // NOTE: Arrows start at center of Earth. Is this suitable for all Geometries? !!!
    if (checkit) for (auto& ar : m_arrowcache) { if (ar.type == TRUESUN3D) return; }
    glm::vec3 dir = calcHADec2Dir(getSun());
    size_t index = m_arrows->addStartDirLen(glm::vec3(0.0f), dir, length, width, color);
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
    //LLD sun = getSun();
    //std::cout << "Earth::updateArrow3DTrueSun(): getSun: " << sun.lat << "," << sun.lon << "\n";
    m_arrows->changeStartDirLen(ar.index, glm::vec3(0.0f), calcHADec2Dir(getSun()), ar.length, ar.width, ar.color);
}
void Earth::addLunarUmbraCone() {
    const double radiusdiff = moonradius - sunradius;
    LLD sun = m_scene->astro->getDecGHA(A_SUN, NO_DOUBLE);
    glm::vec3 sunpos = getLoc3D_NS(sun.lat, sun.lon, sun.dst);
    glm::vec3 moonpos = getLoc3D_NS(m_moonDec, m_moonHour, m_moonDist);
    glm::vec3 conedir = sunpos - moonpos;
    double slope = radiusdiff / glm::length(conedir);
    float clength = (float) -sunradius * glm::length(conedir) / (float)radiusdiff;
    std::cout << "Conelength: " << clength << "\n";
    glm::vec3 conetip = glm::normalize(conedir);
    conetip = conetip * clength + moonpos;
    size_t index = m_dots->addXYZ(moonpos/(float)(earthradius*10.0), WHITE, 0.1f);
    // !!! FIX: the Dot index is not stored, are we missing a delete? !!!
}
void Earth::addUmbraCone() {
    // Interesting: https://oldblog.erikras.com/2011/12/16/how-big-is-the-earths-shadow-on-the-moon
    const float shadowlen = 219.5f; // Shadow cone length in Earth radii
    LLD sun = m_scene->astro->getDecGHA(A_SUN, NO_DOUBLE); // sun.dst = 0.0
    if (sun.lon < pi) sun.lon = -sun.lon;
    else sun.lon = tau - sun.lon; // subsolar.lon is now -pi to pi east of south
    glm::vec3 sunpos = getLoc3D_NS(sun.lat, sun.lon, sun.dst);
    size_t index = m_scene->getViewConesFactory()->addStartDirLen(-shadowlen * sunpos, -sunpos, shadowlen, 1.01f, glm::vec4(0.4f, 0.4f, 0.4f, 0.6f));
}


glm::vec3 Earth::getSubsolarXYZ(double jd_utc) {
    if (jd_utc == 0.0) return flatSun;
    if (jd_utc == m_jd_utc) return flatSun;
    LLD mysun = m_scene->astro->getDecGHA(A_SUN, EDateTime::getJDUTC2TT(jd_utc)); // subsolar.lon is west of south, 0 to tau
    if (mysun.lon < pi) mysun.lon = -mysun.lon;
    else mysun.lon = tau - mysun.lon;                          // subsolar.lon is now -pi to pi east of south
    return getLoc3D(mysun.lat, mysun.lon, m_flatsunheight / earthradius);
}
void Earth::addSubsolarPoint(float size) {
    //std::cout << "Subsolar point : " << m_sunDec << ", " << m_sunHour << "\n";
    //if (m_sunob != nullptr) return;
    //m_sunob = new SubSolar(m_scene, 16, 32, size);
    //m_sunob->Update(flatSun);
    if (m_subsolar) return;
    m_subsolar = new Planetoid(m_scene, nullptr, SUN, 16, 32, size);
}
void Earth::deleteSubsolarPoint() {
    //if (m_sunob != nullptr) delete m_sunob;
    //m_sunob = nullptr;
    if (m_subsolar) delete m_subsolar;
    m_subsolar = nullptr;
}
void Earth::updateSubsolarPoint() {
    //if (m_sunob == nullptr) {
    //    std::cout << "Earth::updateSubsolarPoint() was called, but there is no sundot to update.\n";
    //    return;
    //}
    //m_sunob->Update(flatSun);
}
void Earth::addSubsolarDot() {
    if (m_sundot != NO_UINT) return;
    m_sundot = m_dots->addXYZ(solarGP, LIGHT_RED, 0.01f);
}
void Earth::deleteSubsolarDot() {
    if (m_sundot != NO_UINT) m_dots->removeDot(m_sundot);
    m_sundot = NO_UINT;
}
void Earth::updateSubsolarDot() {
    if (m_sundot == NO_UINT) {
        std::cout << "Earth::updateSubsolarDot() was called, but there is no sundot to update.\n";
        return;
    }
    m_dots->changeXYZ(m_sundot, solarGP, LIGHT_ORANGE, 0.05f);
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
    // https://stackoverflow.com/questions/64881275/satellite-on-orbit-create-the-tangent-cone-from-point-to-sphere
    // (see also https://en.wikipedia.org/wiki/Tangent_lines_to_circles#Outer_tangent but not as useful)
    //glm::vec3 pos = getLoc3D(m_sunDec, m_sunHour, m_flatsunheight / earthradius);
    double rE = 1.0f;  //self.NSscale # Radius of Earth
    double d = glm::length(flatSun);
    double l = sqrt(d * d - rE * rE);
    double t = asin(rE / d);
    double h = l * cos(t);
    double rc = l * sin(t);
    m_suncone = m_scene->getViewConesFactory()->addStartDirLen(flatSun, glm::normalize(flatSun), (float)h - 0.001f, (float)rc + 0.002f, color);
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
    m_scene->getViewConesFactory()->changeStartDirLen(m_suncone, flatSun, glm::normalize(flatSun), (float)h - 0.001f, (float)rc + 0.002f, NO_COLOR);
}
LLD Earth::getSublunarPoint() {
    return { m_moonDec, m_moonHour, m_flatsunheight };
}
glm::vec3 Earth::getSublunarXYZ() {
    return getLoc3D(m_moonDec, m_moonHour, m_flatsunheight / earthradius);
}
void Earth::addSublunarPoint(float size) {
    //std::cout << "Sublunar point : " << m_moonDec << ", " << m_moonHour << "\n";
    if (m_moonob != nullptr) return;
    if (m_jd_utc == 0.0) CalcMoon();
    m_moonob = new SubLunar(m_scene, 16, 32, size);
    updateSublunarPoint();
}
void Earth::deleteSublunarPoint() {
    if (m_moonob != nullptr) delete m_moonob;
}
void Earth::updateSublunarPoint() {
    if (m_moonob == nullptr) {
        std::cout << "Earth::updateSublunarPoint() was called, but there is no moondot to update.\n";
        return;
    }
    double moon_lon = m_moonHour;
    while (moon_lon > pi) moon_lon -= tau;
    while (moon_lon <= -pi) moon_lon += tau;
    if (moon_lon < pi) moon_lon = -moon_lon;
    else moon_lon = tau - moon_lon;                          // moon_lon is now -pi to pi east of south

    LLD testmoon = m_scene->astro->calcDecHA2GP(LLD{ m_moonDec, -m_moonHour, 0.0 });
    //m_moonob->Update(getLoc3D(m_moonDec, moon_lon, m_flatsunheight / earthradius));
    m_moonob->Update(getLoc3D(testmoon.lat, testmoon.lon, m_flatsunheight / earthradius));
    //m_moonob->Update(glm::vec3(0.0,0.0,0.0));
    //std::cout << "Sublunar point : " << m_moonDec << ", " << moon_lon << "\n";
    //std::cout << "Sublunar LLD : " << testmoon.lat << ", " << testmoon.lon << "\n";
}
SubStellarPoint* Earth::addSubStellarPoint(const std::string& name, bool lock, double jd, glm::vec4 color) {
    substellarpoints.push_back(new SubStellarPoint(*this, name, lock, jd, color));
    return substellarpoints.back();
}
void Earth::removeSubStellarPoint(size_t index) {
    delete substellarpoints[index];
    substellarpoints[index] = nullptr;
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
    m_scene->getViewConesFactory()->addStartDirLen(pos, glm::normalize(pos), (float)h - 0.001f, (float)rc + 0.001f, color);
}
void Earth::addViewConeLLD_NS(LLD loc, glm::vec4 color) {  // LLD is under used, could be for all the getLoc3D* and getNml3D* functions etc !!!
    addViewConeXYZ_NS(getLoc3D_NS(deg2rad * loc.lat, deg2rad * loc.lon, loc.dst), color);
}
// How to remove View Cones? They don't make sense for morphs, they are specific to NS geometry. !!!
// The object they tie to may move with time though. So there should also be update functions. Thus, a viewconecache is needed. !!!

PolyCurve* Earth::getPath(size_t index) {
    // !!! This is a mess, how do we know if the requested curve is in m_polycache or m_poycache2 ?
    // The answer is to only use m_polycache2, and rename it to m_polycache.
    // Also look into whether we want scene to draw these, or do it in Earth::Draw()
    return m_polycache[index].path;
}
Earth::polycache* Earth::getPathCache(size_t index) {
    // !!! This is a mess, how do we know if the requested curve is in m_polycache or m_poycache2 ?
    // The answer is to only use m_polycache2, and rename it to m_polycache. UPD: DONE!
    // Also look into whether we want Scene to draw these, or do it in Earth::Draw() - Certainly Scene!
    return &m_polycache[index];
}
std::vector<glm::vec3>* Earth::getPathData(size_t index) {
    return &m_polycache[index].path->m_points;
}

void Earth::addSunSectors(float width, glm::vec4 color, double degrees) {
    // Primary arc is obtained in two parts (because the path is at least 180 degrees long, so the algorithm would pick the antipodal path):
    // 1) From subsolar point to north pole shifted by solar latitude, onto antimeridian if solar lat is positive, otherwise meridian
    // 2) From subsolar point to south pole shifted by solar latitude onto meridian if solar lat is positive, otherwise antimeridian
    // Secondary arcs are obtained by shifting subsolar point 45 degrees east and west, and repeating above.
    // NO !!! The subsolar 45 degree sector points must be rotated about the shifted poles, not due east/west !!!
    // NOTE: The problem with this approach is crossing the south pole it seems.
    // Perhaps a more robust approach is using Great Circles, so implement one and test
    // UPD: I have solved the issue with pole crossing in updateCompositePath()
    // 
    // NOTE: Add refraction flag? No, this indicates how far the Sun GP travels in 3 hours, not where the Sun appears.
    //       So, if the terminator is used to indicate a further 
    LLD subsolar = calcHADec2LatLon(m_scene->astro->getDecGHA(A_SUN, NO_DOUBLE));
    LLD top = { 0.0, 0.0, 0.0 };
    LLD btm = { 0.0, 0.0, 0.0 };
    if (subsolar.lat >= 0.0) {
        top.lat = pi2 - subsolar.lat;
        top.lon = pi + subsolar.lon;
        if (top.lon > pi) top.lon -= tau;
        btm.lat = subsolar.lat - pi2;
        btm.lon = subsolar.lon;
    }
    else { // subsolar.lat is negative!
        top.lat = pi2 + subsolar.lat;
        top.lon = subsolar.lon;
        btm.lat = -pi2 - subsolar.lat;
        btm.lon = subsolar.lon + pi;
        if (btm.lon > pi) btm.lon -= tau;
    }
    // This is wrong, it must be rotated 45 degrees about the SHIFTED poles, sector is relative to the SUN !!!
    //LLD lft = { subsolar.lat, subsolar.lon - pi4, 0.0 };
    //LLD rgt = { subsolar.lat, subsolar.lon + pi4, 0.0 };
    // The right way. It may be possible to optimize this a bit.
    glm::vec3 subsolxyz = getLoc3D_NS(subsolar.lat, subsolar.lon);
    glm::vec3 topxyz = getLoc3D_NS(top.lat, top.lon);
    glm::vec3 left = glm::rotate(subsolxyz, pi4f, topxyz);
    glm::vec3 right = glm::rotate(subsolxyz, -pi4f, topxyz);
    LLD lft = getXYZtoLLD_NS(left);
    LLD rgt = getXYZtoLLD_NS(right);
    addArc(subsolar, top, color, width, true, &Earth::calcGreatArc, SUNSECTOR);
    addArc(subsolar, btm, color, width, true, &Earth::calcGreatArc, SUNSECTOR);
    addArc(lft, top, color, width, true, &Earth::calcGreatArc, SUNSECTOR);
    addArc(lft, btm, color, width, true, &Earth::calcGreatArc, SUNSECTOR);
    addArc(rgt, top, color, width, true, &Earth::calcGreatArc, SUNSECTOR);
    addArc(rgt, btm, color, width, true, &Earth::calcGreatArc, SUNSECTOR);
    addArc(subsolar, lft, color, width, true, &Earth::calcGreatArc, SUNSECTOR);
    addArc(subsolar, rgt, color, width, true, &Earth::calcGreatArc, SUNSECTOR);
}
void Earth::updateSunSectors() {
    LLD subsolar = calcHADec2LatLon(m_scene->astro->getDecGHA(A_SUN, NO_DOUBLE));
    LLD top = { 0.0, 0.0, 0.0 };
    LLD btm = { 0.0, 0.0, 0.0 };
    if (subsolar.lat >= 0.0) {
        top.lat = pi2 - subsolar.lat;
        top.lon = pi + subsolar.lon;
        if (top.lon > pi) top.lon -= tau;
        btm.lat = subsolar.lat - pi2;
        btm.lon = subsolar.lon;
    }
    else { // subsolar.lat is negative!
        top.lat = pi2 + subsolar.lat;
        top.lon = subsolar.lon;
        btm.lat = -pi2 - subsolar.lat;
        btm.lon = subsolar.lon + pi;
        if (btm.lon > pi) btm.lon -= tau;
    }
    //LLD lft = { subsolar.lat, subsolar.lon - pi4, 0.0 };
    //LLD rgt = { subsolar.lat, subsolar.lon + pi4, 0.0 };
    glm::vec3 subsolxyz = getLoc3D_NS(subsolar.lat, subsolar.lon);
    glm::vec3 topxyz = getLoc3D_NS(top.lat, top.lon);
    glm::vec3 left = glm::rotate(subsolxyz, pi4f, topxyz);
    glm::vec3 right = glm::rotate(subsolxyz, -pi4f, topxyz);
    LLD lft = getXYZtoLLD_NS(left);
    LLD rgt = getXYZtoLLD_NS(right);

    int sector = 1;
    for (auto&p : m_polycache) {   // Ugly but it works for now.
        if (p.type == SUNSECTOR) {
            p.gpath->clearPoints();
            //p.path2->clearPoints();
            if (sector == 1) { p.lld1 = subsolar; p.lld2 = top; }
            if (sector == 2) { p.lld1 = subsolar; p.lld2 = btm; }
            if (sector == 3) { p.lld1 = lft; p.lld2 = top; }
            if (sector == 4) { p.lld1 = lft; p.lld2 = btm; }
            if (sector == 5) { p.lld1 = rgt; p.lld2 = top; }
            if (sector == 6) { p.lld1 = rgt; p.lld2 = btm; }
            if (sector == 7) { p.lld1 = subsolar; p.lld2 = lft; }
            if (sector == 8) { p.lld1 = subsolar; p.lld2 = rgt; }
            updateCompositePath2(p);
            sector++;
        }
    }
}
void Earth::removeSunSectors() {
    for (auto& p : m_polycache) {
        if (p.type == SUNSECTOR) {
            if (p.gpath != nullptr) m_scene->deletePolyCurve(p.path);
            //if (p.path2 != nullptr) m_scene->deletePolyCurve(p.path2);
            p.type = NONE;
            p.gpath = nullptr;
            //p.path2 = nullptr;
        }
    }
}

void Earth::addGrid(float deg, float size, glm::vec4 color, std::string type, bool rad, bool eq, bool pm) {
    // Might be more intuitive to specify desired number of divisions i.e. 24 divisions rather than 15 degrees
    // Could also implement major and minor divisions one day, like 24 3 giving major line every 15 degrees, and minor every 5 (15/3)
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
        if (po.type == GRIDLAT || po.type == GRIDLON) po.type = NONE;  // Also clear the PolyCurve? !!!
    }
}
void Earth::addTropics(float size, glm::vec4 color) {
    double earthtropics = m_scene->astro->TrueObliquityOfEcliptic(m_scene->astro->getJD_TT());
    addLatitudeCurve(earthtropics, color, size, false, TROPIC);  // Cancer
    addLatitudeCurve(-earthtropics, color, size, false, TROPIC); // Capricorn
}
void Earth::removeTropics() {
    for (auto& po : m_polycache) {
        if (po.type == TROPIC) po.type = NONE;
    }
}
void Earth::addArcticCirles(float size, glm::vec4 color) {
    double eartharctics = 90.0 - m_scene->astro->TrueObliquityOfEcliptic(m_scene->astro->getJD_TT());
    addLatitudeCurve(eartharctics, color, size, false, ARCTIC);   // Arctic
    addLatitudeCurve(-eartharctics, color, size, false, ARCTIC);  // Antarctic
}
void Earth::removeArcticCircles() {
    for (auto& po : m_polycache) {
        if (po.type == ARCTIC) po.type = NONE;
    }
}
size_t Earth::addEquator(float size, glm::vec4 color) {
    return addLatitudeCurve(0.0f, color, size, false, EQUATOR);
}
void Earth::removeEquator() {
    for (auto& po : m_polycache) {
        if (po.type == EQUATOR) po.type = NONE;
    }
}
size_t Earth::addPrimeMeridian(float size, glm::vec4 color) {
    return addLongitudeCurve(0.0, color, size, false, PRIME_MERIDIAN); // Technically 0 degrees = 0 radians, beware when copy/pasting
}
void Earth::removePrimeMeridian() {
    for (auto& po : m_polycache) {
        if (po.type == PRIME_MERIDIAN) po.type = NONE;
    }
}
size_t Earth::addLatitudeCurve(double lat, glm::vec4 color, float width, bool rad, unsigned int type) {
    if (!rad) lat *= deg2rad;
    PolyCurve* curve = m_scene->newPolyCurve(color, width);
    curve->addPoint(getLoc3D(lat, (-180.0+tiny) * deg2rad, 0.0));
    for (int lon = -179; lon <= 180; lon++) { // Note: we do not want -180, but we do want +180
        curve->addPoint(getLoc3D(lat, lon * deg2rad, 0.0));
    }
    curve->generate();
    m_polycache.push_back({ curve, nullptr, nullptr, width, type, color, LLD({ lat,0.0 ,0.0 }), LLD({ 0.0 ,0.0 ,0.0 }) });
    return m_polycache.size() - 1;
}
void Earth::changeLatitudeCurve(size_t index, double lat, glm::vec4 color, float width, bool rad) {
    // lat outside -360 to 360 range is assumed to be NO_LAT.
    if (!rad) lat *= deg2rad;
    if (m_polycache[index].type == LATITUDE) {
        if (color != NO_COLOR) m_polycache[index].color = color;
        if (abs(lat) <= tau) m_polycache[index].lld1.lat = lat;
        if (width > 0.0f) m_polycache[index].width = width;
    }
}
void Earth::updateLatitudeCurve(polycache& p) {
    p.path->clearPoints();
    p.path->addPoint(getLoc3D(p.lld1.lat, (-180.0 + tiny) * deg2rad, 0.0));
    for (int lon = -179; lon <= 180; lon++) { // Note: we do not want -180, but we do want +180
        p.path->addPoint(getLoc3D(p.lld1.lat, lon * deg2rad, 0.0));
    }
    p.path->generate();
}
size_t Earth::addLongitudeCurve(double lon, glm::vec4 color, float width, bool rad, unsigned int type) {
    //bool debug = false;
    //if (lon == -45.0) {
    //    debug = true;
    //}
    if (!rad) lon *= deg2rad;
    PolyCurve* curve = m_scene->newPolyCurve(color, width);
    for (int lat = -90; lat <= 90; lat++) {
        curve->addPoint(getLoc3D(lat * deg2rad, lon, 0.0));
    }
    curve->generate();
    m_polycache.push_back({ curve, nullptr, nullptr, width, type, color, {0.0, lon, 0.0}, {0.0, 0.0, 0.0} });
    return m_polycache.size() - 1;
}
void Earth::changeLongitudeCurve(size_t index, double lon, glm::vec4 color, float width, bool rad) {
    // lat outside -360 to 360 range is assumed to be NO_LAT.
    if (!rad) lon *= deg2rad;
    if (m_polycache[index].type == LONGITUDE) {
        if (color != NO_COLOR) m_polycache[index].color = color;
        if (abs(lon) <= tau) m_polycache[index].lld1.lon = lon;
        if (width > 0.0f) m_polycache[index].width = width;
    }
    updateLongitudeCurve(m_polycache[index]);
    // Not sure how to update color and width of the PolyCurve path
}
void Earth::updateLongitudeCurve(polycache& p) {
    p.path->clearPoints();
    for (int lat = -90; lat <= 90; lat++) {
        p.path->addPoint(getLoc3D(lat * deg2rad, p.lld1.lon, 0.0));
    }
    p.path->generate();
}
void Earth::addEcliptic() {
    //NOTE: Assumes that Ecliptic curve is an exact great circle, because:
    // 1) The ecliptic plane slices through Earth without curvature
    // 2) The center of the Earth always lies in the ecliptic plane by definition
    double obliquity = m_scene->astro->TrueObliquityOfEcliptic(NO_DOUBLE, true);
    m_ecliptic = addTissotIndicatrix({ pi2 - obliquity,
                                       ACoord::rangempi2pi(-m_scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true) - pi2),
                                       0.0 },
                                       pi2, true, GREEN, 0.005f);  // Should really have a more generically named primitive
}
void Earth::updateEcliptic() {
    if (m_ecliptic == NO_UINT) return;
    double obliquity = m_scene->astro->TrueObliquityOfEcliptic(NO_DOUBLE, true);
    TissotCache* ecliptic = &tissotcache[m_ecliptic];
    ecliptic->lat = pi2 - obliquity;
    double agsid = -m_scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true) - pi2;
    ecliptic->lon = ACoord::rangempi2pi(agsid);
    //std::cout << "agsid=" << agsid << ", ecliptic.lon=" << ecliptic->lon << "\n";
    updateTissotIndicatrix(*ecliptic);
}
size_t Earth::addLerpArc(LLD lld1, LLD lld2, glm::vec4 color, float width, bool rad) {
    if (lld1.lon < lld2.lon) { // Do we still strictly need to go from high to low longitude? I guess not, so will comment out for now !!!
        // Added swap back in as Pole crossing in updateCompositePath() seems to need a bit of tweaking !!!
        LLD lldtmp = lld1;
        lld1 = lld2;
        lld2 = lldtmp;
    }
    return addArc(lld1, lld2, color, width, rad, &Earth::calcLerpArc, LERPARC);
}
size_t Earth::addGreatArc(LLD lld1, LLD lld2, glm::vec4 color, float width, bool rad) {
    if (lld1.lon < lld2.lon) { // Do we still strictly need to go from high to low longitude? I guess not, so will comment out for now !!!
        // Added swap back in as Pole crossing in updateCompositePath() seems to need a bit of tweaking !!!
        LLD lldtmp = lld1;
        lld1 = lld2;
        lld2 = lldtmp;
    }
    return addArc(lld1, lld2, color, width, rad, &Earth::calcGreatArc, GREATARC);
}
void Earth::removeGreatArc(size_t index) {
    deleteArc(index);
}
void Earth::changeGreatArc(size_t index, LLD lld1, LLD lld2, bool rad) {
    if (!rad) {
        lld1.lat *= deg2rad;
        lld1.lon *= deg2rad;
        lld2.lat *= deg2rad;
        lld2.lon *= deg2rad;
    }
    m_polycache[index].lld1 = lld1;
    m_polycache[index].lld2 = lld2;
    m_polycache[index].gpath->clearPoints();
    updateCompositePath2(m_polycache[index]);
}
void Earth::updateGreatArc(polycache& p) {
    p.gpath->clearPoints();
    updateCompositePath2(p);
}
size_t Earth::addFlatArc(LLD lld1, LLD lld2, glm::vec4 color, float width, bool rad) {
    // The shortest path between two points on the AE map
    // Completely different structure than addArc() et al, so no need to try to share code
    // Also does not have singularities, because LERP is done in Cartesian coordinates.
    float mindist = 0.01f; // Desired step size
    if (!rad) {
        lld1.lat *= deg2rad;
        lld1.lon *= deg2rad;
        lld2.lat *= deg2rad;
        lld2.lon *= deg2rad;
    }
    PolyCurve* curve = m_scene->newPolyCurve(color, width);
    LLD lld;
    glm::vec3 pos2;
    // Get Cartesian endpoints and lerp in xyz space
    glm::vec3 beg = getLoc3D_AE((float)lld1.lat, (float)lld1.lon, (float)lld1.dst);
    glm::vec3 fin = getLoc3D_AE((float)lld2.lat, (float)lld2.lon, (float)lld2.dst);
    float plen = glm::length(fin - beg);
    unsigned int steps = std::max(1, (int)(plen / mindist));
    Lerper<float> lx = Lerper<float>(beg.x, fin.x, steps, false);
    Lerper<float> ly = Lerper<float>(beg.y, fin.y, steps, false);
    Lerper<float> lz = Lerper<float>(beg.z, fin.z, steps, false);
    for (unsigned int i = 0; i < steps; i++) {
        // Convert to AE lat,lon
        lld = getXYZtoLLD_AE(glm::vec3(lx.getNext(), ly.getNext(), lz.getNext()));
        // Convert to Earth shape in use
        pos2 = getLoc3D(lld.lat, lld.lon, lld.dst);
        curve->addPoint(pos2);
    }
    curve->generate();
    m_polycache.push_back({ curve, nullptr, nullptr, width, FLATARC, color, lld1,lld2 });
    return m_polycache.size() - 1;
}
void Earth::updateLerpArc(polycache& p) {
    p.path->clearPoints();
    p.path2->clearPoints();
    updateCompositePath(p);
}
void Earth::updateFlatArc(polycache p) {
    // The shortest path between two points on the AE map
    float mindist = 0.01f; // Desired step size
    PolyCurve* curve = p.path;
    curve->clearPoints();
    LLD lld;
    glm::vec3 pos2;
    // Get Cartesian endpoints and lerp in xyz space
    glm::vec3 beg = getLoc3D_AE(p.lld1.lat, p.lld1.lon, p.lld1.dst);
    glm::vec3 fin = getLoc3D_AE(p.lld2.lat, p.lld2.lon, p.lld2.dst);
    float plen = glm::length(fin - beg);
    unsigned int steps = std::max(1, (int)(plen / mindist));
    Lerper<float> lx = Lerper<float>(beg.x, fin.x, steps, false);
    Lerper<float> ly = Lerper<float>(beg.y, fin.y, steps, false);
    Lerper<float> lz = Lerper<float>(beg.z, fin.z, steps, false);
    for (unsigned int i = 0;i < steps;i++) {
        // Convert to AE lat,lon
        lld = getXYZtoLLD_AE(glm::vec3(lx.getNext(), ly.getNext(), lz.getNext()));
        // Convert to Earth shape in use
        pos2 = getLoc3D(lld.lat, lld.lon, lld.dst);
        curve->addPoint(pos2);
    }
    curve->generate();
}
void Earth::addTerminatorTrueMoon(glm::vec4 color, float width) {
    //PolyCurve* curve1 = m_scene->newPolyCurve(LIGHT_GREEN, width);  // Show paths in two different colors to troubleshoot
    //PolyCurve* curve2 = m_scene->newPolyCurve(LIGHT_RED, width);
    PolyCurve* curve1 = m_scene->newPolyCurve(color, width);
    PolyCurve* curve2 = m_scene->newPolyCurve(color, width);
    m_polycache.push_back({ curve1, curve2, nullptr,
                                width, MOONTERMINATOR, color,
                                {m_moonDec, m_moonHour, 0.0}, {0.0, 0.0, 0.0},
                                w_refract ? w_mrefang : 0.0, tau, &Earth::calcTerminator, /*closed*/ true });
    updateCompositePath(m_polycache.back());
}
void Earth::deleteTerminatorTrueMoon() {
    for (auto& p : m_polycache) {
        if (p.type == MOONTERMINATOR) {
            p.type = NONE;         // Hide the path from updaters
            // clearPoints() also clears the geometric segments, so no need to call generate();
            p.path->clearPoints(); // Discard the point list
            p.path2->clearPoints(); // Discard the point list
            break; // Assumes there is only one MOONTERMINATOR
        }
    }
}
void Earth::updateTerminatorTrueMoon(polycache& p) {
    p.path->clearPoints();
    p.path2->clearPoints();
    p.refraction = w_refract ? w_mrefang : 0.0;
    p.lld1.lat = m_moonDec;
    p.lld1.lon = m_moonHour;
    p.fend = tau;
    updateCompositePath(p);
    return;
}
size_t Earth::addTerminatorTrueSun(glm::vec4 color, float width) {
    GenericPath* path = new GenericPath(m_scene, width, color);
    m_polycache.push_back({ nullptr, nullptr, path,
                            width, SUNTERMINATOR, color,
                            {subsolar.lat, subsolar.lon, 0.0}, {0.0, 0.0, 0.0},
                            w_refract ? w_srefang : 0.0, tau, &Earth::calcTerminator, /*closed*/ true });
    updateCompositePath2(m_polycache.back());
    return m_polycache.size() - 1;
}
void Earth::deleteTerminatorTrueSun() {
    for (auto& p : m_polycache) {
        if (p.type == SUNTERMINATOR) {
            p.type = NONE;         // Hide the path from updaters
            // clearPoints() also clears the geometric segments, so no need to call generate();
            p.gpath->clearPoints(); // Discard the point list
            break; // Assumes there is only one SUNTERMINATOR
        }
    }
}
void Earth::updateTerminatorTrueSun(polycache& p) {
    p.gpath->clearPoints();
    p.refraction = w_refract ? w_srefang : 0.0;
    p.lld1.lat = subsolar.lat;
    p.lld1.lon = subsolar.lon;
    p.fend = tau;
    updateCompositePath2(p);
    return;
}
void Earth::addSubsolarPath(double begin, double finish, unsigned int steps, bool fixed) {
    // Params are in fractional JDs
    // FIXME !!! Should use GenericPath instead of PolyCurve !!!
    if (begin == NO_DOUBLE) begin = -0.5;
    if (finish == NO_DOUBLE) finish = 0.5;
    if (steps == NO_UINT) steps = (unsigned int)((finish - begin) * 360.0); // default to 360 degrees per day
    sunpath = m_scene->astro->getCelestialPath(A_SUN, begin, finish, steps, ECGEO, fixed);
    suncurve = m_scene->newPolyCurve(SUNCOLOR, 0.005f, steps);
    updateSubsolarPath();
}
void Earth::updateSubsolarPath() {
    // FIXME !!! Does not cut path at seam !!! (see morph ER -> RC)
    suncurve->clearPoints();
    for (auto& sp : sunpath->entries) {
        suncurve->addPoint(getLoc3D(sp.geq.lat, -sp.geogha));   // Dec, GHA
    }
    suncurve->generate();
}

size_t Earth::addSumnerLine(LLD gp, double elevation, glm::vec4 color, float width, bool rad) {
    // For anecdote see: https://www.madinstro.net/sundry/navcel.html
    // A Sumner Line is a (small or, at the limit a great) circle centered on the GP of a celestial body
    // where an observer would measure the (corrected) elevation given.
    return addTissotIndicatrix(gp, rad ? pi2 - elevation : 90.0 - elevation, rad, color, width);
}
size_t Earth::addTissotIndicatrix(LLD location, double radius, bool rad, glm::vec4 color, float width) {
    // Note: radius is given in degrees/radians, not distance units
    // Can draw an indicatrix directly at a pole.
    // To validate, compare with:
    // https://map-projections.net/img/tissot-30-w/azimutal-equidistant-gpolar.jpg?ft=616b1080  radius = 5deg, grid = 10
    // https://map-projections.net/img/tissot-30/rectang-0.jpg?ft=54e394a0 radius 7.5, grid = 15
    // NOTE: This could be implemented using SumnerLine, as that is technically the same as is done here !!!
    if (!rad) {
        location.lat *= deg2rad;
        location.lon *= deg2rad;
        radius *= deg2rad;
    }
    unsigned int steps = 360;  // Might need to be a passed in parameter? !!!
    PolyCurve* curve = m_scene->newPolyCurve(color, width, steps);  // Obviously not scalable. Make into Arc or Composite path
    // Method: Draw a circe around the north pole, of the right size, in lat,lon coordinates.
    // Rotate it into place at provided location, converting to Cartesian world coordinates and plot using PolyCurve.
    double const zangle = location.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
    double const yangle = location.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
    double const cy = cos(yangle);
    double const sy = sin(yangle);
    double const cz = cos(zangle);
    double const sz = sin(zangle);
    double const lz = sin(pi2 - radius);
    double lat, lon = 0.0;
    double lx = 0.0, ly = 0.0, l2x = 0.0, l3x = 0.0, l3y = 0.0;
    for (double a = 0.0; a <= tau; a += tau / steps) {
        lx = cos(pi2 - radius) * cos(a);
        ly = cos(pi2 - radius) * sin(a);
        l2x = lx * cy + lz * sy;
        l3x = l2x * cz - ly * sz;
        l3y = l2x * sz + ly * cz;
        lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
        lon = atan2(l3y, l3x);
        curve->addPoint(getLoc3D(lat, lon));
    }
    curve->generate();
    return tissotcache.store({ location.lat, location.lon, radius, color, width, curve });
    // This could relatively easily be altered to be a calculation function for updateCompositePath(), at a slight overhead.
}
void Earth::removeTissotIndicatrix(size_t index) {
    if (tissotcache[index].curve != nullptr) m_scene->deletePolyCurve(tissotcache[index].curve);
    tissotcache.remove(index);
}
void Earth::updateTissotIndicatrix(TissotCache& tissot) {
    unsigned int steps = 360;
    tissot.curve->clearPoints();
    double const zangle = tissot.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
    double const yangle = tissot.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
    double const cy = cos(yangle);
    double const sy = sin(yangle);
    double const cz = cos(zangle);
    double const sz = sin(zangle);
    double const lz = sin(pi2 - tissot.radius);
    for (double a = 0.0; a <= tau; a += tau / steps) {
        double lx = cos(pi2 - tissot.radius) * cos(a);
        double ly = cos(pi2 - tissot.radius) * sin(a);
        double l2x = lx * cy + lz * sy;
        double l3x = l2x * cz - ly * sz;
        double l3y = l2x * sz + ly * cz;
        double lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
        double lon = atan2(l3y, l3x);
        tissot.curve->addPoint(getLoc3D(lat, lon));
    }
    tissot.curve->generate();
    // This could relatively easily be altered to be a calculation function for updateCompositePath(), at a slight overhead.
}
size_t Earth::addSemiTerminator(double radius, bool rad, glm::vec4 color, float width) {
    // Note: radius is given in degrees/radians, not distance units
    if (!rad) { radius *= deg2rad; }
    m_polycache.emplace_back(polycache());
    m_polycache.back().path = m_scene->newPolyCurve(color, width);
    m_polycache.back().path2 = m_scene->newPolyCurve(color, width);
    m_polycache.back().width = width;
    m_polycache.back().type = SEMITERMINATOR;
    m_polycache.back().color = color;
    m_polycache.back().lld1 = getSubsolar();
    m_polycache.back().lld2.dst = radius;
    m_polycache.back().fend = tau;
    m_polycache.back().ca = &Earth::calcSemiTerminator;
    m_polycache.back().closed = true;
    updateSemiTerminator(m_polycache.back());
    // Doesn't work well with updateCompositePath() because lld1 and lld2 are not actually start and end points of curve.
    // Well, closed paths do not use lld1 & lld2 for endpoints :) Still it gets very lo res in updateCompositePath(),
    // left it with a dedicated generator. But of course uses polycache for the data, so the path can be tracked.
    // UPD: Look into why it goes lo-res in updateCompositePath(), because it might be necessary to migrate !!!
    return m_polycache.size() - 1;
}
void Earth::updateSemiTerminator(polycache& tissot) { // Tested, works with radii larger than 90 degrees too.
    unsigned int steps = 360;
    tissot.lld1 = getSubsolar();
    tissot.path->clearPoints();
    //tissot.path2->clearPoints();
    //updateCompositePath(tissot);
    double const zangle = tissot.lld1.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
    double const yangle = tissot.lld1.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
    double const cy = cos(yangle);
    double const sy = sin(yangle);
    double const cz = cos(zangle);
    double const sz = sin(zangle);
    double const lz = sin(pi2 - tissot.lld2.dst);
    for (double a = 0.0; a <= tau; a += tau / steps) {  // creates steps+1 entries
        double lx = cos(pi2 - tissot.lld2.dst) * cos(a);
        double ly = cos(pi2 - tissot.lld2.dst) * sin(a);
        double l2x = lx * cy + lz * sy;
        double l3x = l2x * cz - ly * sz;
        double l3y = l2x * sz + ly * cz;
        double lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
        double lon = atan2(l3y, l3x);
        tissot.path->addPoint(getLoc3D(lat, lon));
    }
    tissot.path->generate();
    // This could relatively easily be altered to be a calculation function for updateCompositePath(), at a slight overhead.
}
LLD Earth::calcGreatArc(LLD lld1, LLD lld2, double f, double refraction, bool rad) {
    if (!rad) {
        lld1.lat *= deg2rad; // doubles
        lld1.lon *= deg2rad;
        lld2.lat *= deg2rad;
        lld2.lon *= deg2rad;
    }
    LLD ret;
    // Why calculate in cartesian? Apparently parametric equations in polar coordinates are hard to come by.
    double d = acos(sin(lld1.lat) * sin(lld2.lat) + cos(lld1.lat) * cos(lld2.lat) * cos(lld1.lon - lld2.lon));
    double A = sin((1 - f) * d) / sin(d);
    double B = sin(f * d) / sin(d);
    double x = A * cos(lld1.lat) * cos(lld1.lon) + B * cos(lld2.lat) * cos(lld2.lon);
    double y = A * cos(lld1.lat) * sin(lld1.lon) + B * cos(lld2.lat) * sin(lld2.lon);
    double z = A * sin(lld1.lat) + B * sin(lld2.lat);
    ret.lat = atan2(z, sqrt(x * x + y * y));
    ret.lon = atan2(y, x);
    if (abs(ret.lat) == pi2) ret.lon = 0.0; // On the poles longitude is undefined, setting to 0.0. Except, it ruins paths passing straight over the pole. !!!
    if (!rad) {
        ret.lat *= rad2deg;
        ret.lon *= rad2deg;
    }
    return ret;
}
LLD Earth::calcLerpArc(LLD lld1, LLD lld2, double f, double refraction, bool rad) {
    // refraction is not used here. It is included so calcLerpArc will match the callback function spec of updateCompositePath() et al.
    if (!rad) {
        lld1.lat *= deg2rad; // doubles
        lld1.lon *= deg2rad;
        lld2.lat *= deg2rad;
        lld2.lon *= deg2rad;
    }
    LLD ret;
    ret.lat = f * lld2.lat + (1.0 - f) * lld1.lat;
    ret.lon = f * lld2.lon + (1.0 - f) * lld1.lon;
    ret.dst = f * lld2.dst + (1.0 - f) * lld1.dst;
    if (!rad) {
        ret.lat *= rad2deg;
        ret.lon *= rad2deg;
    }
    return ret;
}
LLD Earth::calcTerminator(LLD lld1, LLD lld2, double param, double refang, bool rad) {
    if (!rad) {
        lld1.lat *= deg2rad;
        lld1.lon *= deg2rad;
        param *= deg2rad;
        refang *= deg2rad;
    }
    LLD res;
    double sinparm = sin(param);
    res.lat = asin(cos(lld1.lat) * sinparm);
    double x = -cos(lld1.lon) * sin(lld1.lat) * sinparm - sin(lld1.lon) * cos(param);
    double y = -sin(lld1.lon) * sin(lld1.lat) * sinparm + cos(lld1.lon) * cos(param);
    res.lon = atan2(y, x);
    // Refracted terminator
    if (refang != 0.0) {
        glm::vec3 loc = getLoc3D_NS(res.lat, res.lon);     // Calc as if on sphere
        glm::vec3 sun2 = getLoc3D_NS(lld1.lat, lld1.lon);
        glm::vec3 rloc = glm::rotate(loc, (float)-refang, glm::cross(loc, sun2));
        res = getXYZtoLLD_NS(rloc);
    }
    if (res.lon > pi) res.lon -= tau;
    if (res.lon < -pi) res.lon += tau;
    if (!rad) {
        res.lat *= rad2deg;
        res.lon *= rad2deg;
    }
    return res;
}
double Earth::calcArcDist(LLD lld1, LLD lld2, bool rad) {
    if (!rad) {
        lld1.lat *= deg2rad; // doubles
        lld1.lon *= deg2rad;
        lld2.lat *= deg2rad;
        lld2.lon *= deg2rad;
    }
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
    if (!rad) dist *= rad2deg;
    return dist;
}
LLD Earth::calcSemiTerminator(LLD lld1, LLD lld2, double param, double refang, bool rad) {
    if (!rad) {
        lld1.lat *= deg2rad;
        lld1.lon *= deg2rad;
        lld2.dst *= deg2rad;
    }
    // lld1.lat/lon = subsolar and lld2.dst = radius in radians
    LLD retval = { 0.0, 0.0, 0.0 };
    // const marked variables could be calculated once per curve instead of once per point,
    // but using updateCompositePath() may be worth it. Or cache them in a struct?
    double const zangle = lld1.lon - pi;   // Negative of angle to rotate around Z, to center above X axis
    double const yangle = lld1.lat - pi2;  // Really -(90-lat) Negative of angle to rotate around Y to center on north pole
    double const cy = cos(yangle);
    double const sy = sin(yangle);
    double const cz = cos(zangle);
    double const sz = sin(zangle);
    double const lz = sin(pi2 - lld2.dst);
    double lx = cos(pi2 - lld2.dst) * cos(param);
    double ly = cos(pi2 - lld2.dst) * sin(param);
    double l2x = lx * cy + lz * sy;
    double l3x = l2x * cz - ly * sz;
    double l3y = l2x * sz + ly * cz;
    retval.lat = atan2(-lx * sy + lz * cy, sqrt(l3x * l3x + l3y * l3y));
    retval.lon = atan2(l3y, l3x);
    if (!rad) {
        retval.lat *= rad2deg;
        retval.lon *= rad2deg;
    }
    return retval;
}
size_t Earth::addArc(LLD lld1, LLD lld2, glm::vec4 color, float width, bool rad, calcFunc ca, unsigned int type) {
    if (!rad) {
        lld1.lat *= deg2rad; // double
        lld2.lat *= deg2rad; // 
        lld1.lon *= deg2rad; // 
        lld2.lon *= deg2rad; // 
    }
    GenericPath* path = new GenericPath(m_scene, width, color);
    m_polycache.push_back({ nullptr, nullptr, path, width, type, color, lld1, lld2, /* refraction */ 0.0, /* fend */ 1.0, ca, /* closed */ false });
    updateCompositePath2(m_polycache.back());
    return m_polycache.size() - 1;
}
void Earth::changeArc(size_t index, glm::vec4 color, float width) {
    if (color != NO_COLOR) m_polycache[index].color = color;
    if (width != NO_FLOAT) m_polycache[index].width = width;
    m_polycache[index].path->changePolyCurve(m_polycache[index].color, m_polycache[index].width);
    if (m_polycache[index].path2 != nullptr) m_polycache[index].path2->changePolyCurve(m_polycache[index].color, m_polycache[index].width);
}
void Earth::deleteArc(size_t index) {
    m_polycache[index].type = NONE;
    if (m_polycache[index].path != nullptr) m_scene->deletePolyCurve(m_polycache[index].path);
    if (m_polycache[index].path2 != nullptr) m_scene->deletePolyCurve(m_polycache[index].path2);
    if (m_polycache[index].gpath != nullptr) delete m_polycache[index].gpath;
    m_polycache[index].path = nullptr;
    m_polycache[index].path2 = nullptr;
    m_polycache[index].gpath = nullptr;
    // Any PathTracker still around with reference to these curves will run into trouble after this !!!
}

void Earth::updateCompositePath2(polycache& p) {
    // NOTE: Height is awkward, not currently tracked/lerped !!!
    //       For Moon and Sun groundpath it is not needed. But for the Lerp and Great paths, it might be nice!
    //       Maybe Satellites and Planes will be implemented with a separate custom path anyway.
    //std::cout << "Path creation started.\n";
    glm::vec3 oldap = glm::vec3(1000.0);
    double histep = 1.3 * deg2rad;
    double lostep = 0.24 * deg2rad;
    double dflonflip = 270.0 * deg2rad; //(180.0 - tiny)* deg2rad;
    // Get useful old location and add first point to curve
    LLD first = (this->*p.ca)(p.lld1, p.lld2, 0.0, p.refraction, true);
    LLD lldf = first;
    glm::vec3 tp = getLoc3D(lldf.lat, lldf.lon, 0.0);
    p.gpath->addPoint(tp);
    oldap = tp;
    //addArcPoint2(tp, true, false, oldap, p.gpath); // First=true
    LLD oldf = lldf;
    // IMPORTANT: df must stay in range [0;p.fend] otherwise it makes no sense !!! Plz revise!
    //       UPD: Changed doubling limit to account for this. May cause an infinite loop though !!!
    double dist = calcArcDist(p.lld1, p.lld2, true);
    double df = 1.0 / (rad2deg * dist); // Reasonable initial stepsize of 1 step per degree? Seems reasonable during casual testing.
    //std::cout << "Initial df: " << df << "\n";
    if (p.type == MOONTERMINATOR) {
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
    while (f < p.fend) {
        f += df;
        half = 0;
        doub = 0;
        while (true) {
            // When crossing seam, lon goes from -180 to 180 or reverse; near 360 difference.
            // When crossing pole, lon goes from 0 to 180 or -180, or from -90 to 90, or from -170 to 10 or reverse; near 180 difference.
            // Generate next point
            lldf = (this->*p.ca)(p.lld1, p.lld2, f, p.refraction, true);
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
                if (df > p.fend) {
                    std::cout << "WARNING! Earth::updateCompositePath(): Stepsize (" << df << ") exceeds size of full range (" << p.fend << "), resetting to initial value (";
                    if (p.fend == 1.0) df = 1.0 / (rad2deg * dist);
                    if (p.fend == tau) df = deg2rad;
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
                    if (p.fend == 1.0) df = 1.0 / (rad2deg * dist);
                    if (p.fend == tau) df = deg2rad;
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
        tp = getLoc3D(lldf.lat, lldf.lon, 0.0f);

        // If a SEAM is crossed, split the path and insert intermediate points
        if (dflon > dflonflip) { // Longitude inversion at seam
            if (oldf.lon > lldf.lon) { // Passed counter clockwise (+ to -)
                ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - lldf.lat) / (lldf.lon + tau - oldf.lon);
                //std::cout << "Seam splitting counter cloclwise - End: " << ilat << "," << pi - tiny << " Start: " << ilat << "," << tiny - pi << "\n";
                ip1 = getLoc3D(ilat, pi - tiny);
                ip2 = getLoc3D(ilat, tiny - pi);
            }
            else { // Passed clockwise
                ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - lldf.lat) / (lldf.lon + tau - oldf.lon);
                //std::cout << "Seam splitting clockwise - End: " << ilat << "," << tiny - pi << " Start: " << ilat << "," << pi - tiny << "\n";
                ip2 = getLoc3D(ilat, pi - tiny);
                ip1 = getLoc3D(ilat, tiny - pi);
            }
            split = true;
        }

        // If a POLE is crossed, split the path and insert intermediate points
        if (dflon + tiny > pi && !split) { // Longitude inversion at pole, if not already split at seam
            if (oldf.lon > lldf.lon) { // Passed counter clockwise (+ to -) - Will never happen if addArc() is swapping directions to always be clockwise
                //std::cout << "Pole splitting counter cloclwise - Old: " << rad2deg * oldf.lat << "," << rad2deg * oldf.lon << " new: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                ip1 = getLoc3D(lldf.lat > 0 ? pi2 : -pi2, oldf.lon); // End path1
                ip2 = getLoc3D(lldf.lat > 0 ? pi2 : -pi2, lldf.lon); // Start path2
                //std::cout << " - End: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << rad2deg * lldf.lon << "," << " Start: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * oldf.lon << "\n";
            }
            else { // Passed clockwise
                //std::cout << "Pole splitting cloclwise - Old: " << rad2deg * oldf.lat << "," << rad2deg * oldf.lon << " new: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                ip1 = getLoc3D(lldf.lat > 0 ? pi2 : -pi2, oldf.lon); // End path1
                ip2 = getLoc3D(lldf.lat > 0 ? pi2 : -pi2, lldf.lon); // Start path2
                //std::cout << " - End: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * oldf.lon << " Start: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * lldf.lon << "\n";
            }
            split = true;
        }
        if (split) {
            // Now insert the extra points to end one path and begin the next
            p.gpath->addSplit(ip1, ip2);
            oldap = ip2;
            //addArcPoint2(ip1, false, true, oldap, p.gpath); // last=true so cap path and start new one
            //curve->generate();
            //curve = p.path2;
            //addArcPoint2(ip2, true, false, oldap, p.gpath);  // First point in new path
            split = false;
        }
        if (0.0 < f && f < p.fend && glm::length(tp - oldap) < 0.05f) {
            p.gpath->addPoint(tp);
            oldap = tp;
        }
        //if (0.0 < f && f < p.fend) addArcPoint2(tp, false, false, oldap, p.gpath);
        oldf = lldf;
    }
    if (!p.closed) p.gpath->addPoint(getLoc3D(p.lld2.lat, p.lld2.lon));
    if (p.closed) p.gpath->addPoint(getLoc3D(first.lat, first.lon));
    p.gpath->generate();
    return;
}

void Earth::addArcPoint(glm::vec3 ap, bool first, bool last, glm::vec3& oldap, PolyCurve* curve) {
    static const float mindiff = 0.05f;
    if (!last && !first && glm::length(ap - oldap) < mindiff) return; // Skip of new point is too close to previous one
    curve->addPoint(ap);
    oldap = ap; // Store, since last added point is not available directly from PolyCurve.
    return;
}
void Earth::updateCompositePath(polycache& p) {
    // NOTE: Height is awkward, not currently tracked/lerped !!!
    //       For Moon and Sun groundpath it is not needed. But for the Lerp and Great paths, it might be nice!
    //       Maybe Satellites and Planes will be implemented with a separate custom path anyway.
    //std::cout << "Path creation started.\n";
    glm::vec3 oldap = glm::vec3(1000.0);
    PolyCurve* curve = p.path;
    double histep = 1.3 * deg2rad;
    double lostep = 0.24 * deg2rad;
    double dflonflip = 270.0 * deg2rad; //(180.0 - tiny)* deg2rad;
    // Get useful old location and add first point to curve
    LLD first = (this->*p.ca)(p.lld1, p.lld2, 0.0, p.refraction, true);
    LLD lldf = first;
    glm::vec3 tp = getLoc3D(lldf.lat, lldf.lon, 0.0);
    addArcPoint(tp, true, false, oldap, curve); // First=true
    LLD oldf = lldf;
    // IMPORTANT: df must stay in range [0;p.fend] otherwise it makes no sense !!! Plz revise!
    //       UPD: Changed doubling limit to account for this. May cause an infinite loop though !!!
    double dist = calcArcDist(p.lld1, p.lld2, true);
    double df = 1.0 / (rad2deg * dist); // Reasonable initial stepsize of 1 step per degree? Seems reasonable during casual testing.
    //std::cout << "Initial df: " << df << "\n";
    if (p.type == MOONTERMINATOR) {
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
    while (f < p.fend) {
        f += df;
        half = 0;
        doub = 0;
        while (true) {
            // When crossing seam, lon goes from -180 to 180 or reverse; near 360 difference.
            // When crossing pole, lon goes from 0 to 180 or -180, or from -90 to 90, or from -170 to 10 or reverse; near 180 difference.
            // Generate next point
            lldf = (this->*p.ca)(p.lld1, p.lld2, f, p.refraction, true);
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
                if (df > p.fend) {
                    std::cout << "WARNING! Earth::updateCompositePath(): Stepsize (" << df << ") exceeds size of full range (" << p.fend << "), resetting to initial value (";
                    if (p.fend == 1.0) df = 1.0 / (rad2deg * dist);
                    if (p.fend == tau) df = deg2rad;
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
            if (dflat < histep && abs(dflon) + tiny > pi) {
                //std::cout << "Pole inversion detected\n";
                break;
            }
            // Halving / reducing
            if (dflat > histep || dflon > histep) { // (dflat > histep || (dflonflip > dflon && dflon > histep))
                //std::cout << "-df: " << df << " dflat,dflon: " << dflat << "," << dflon << " oldf: " << oldf.lat << "," << oldf.lon << " lldf: " << lldf.lat << "," << lldf.lon << "\n";
                if (half == 14) {
                    std::cout << "WARNING! Earth::updateCompositePath(): After 14 halvings, stepsize (" << df << ") is still too big, giving up!\n";
                    if (p.fend == 1.0) df = 1.0 / (rad2deg * dist);
                    if (p.fend == tau) df = deg2rad;
                    f += df;
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
        tp = getLoc3D(lldf.lat, lldf.lon, 0.0f);

        // If a SEAM is crossed, split the path and insert intermediate points
        if (abs(lldf.lon - oldf.lon) > dflonflip) { // Longitude inversion at seam
            if (oldf.lon > lldf.lon) { // Passed counter clockwise (+ to -)
                ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - lldf.lat) / (lldf.lon + tau - oldf.lon);
                //std::cout << "Seam splitting counter cloclwise - End: " << ilat << "," << pi - tiny << " Start: " << ilat << "," << tiny - pi << "\n";
                ip1 = getLoc3D(ilat, pi - tiny);
                ip2 = getLoc3D(ilat, tiny - pi);
            }
            else { // Passed clockwise
                ilat = oldf.lat - (pi - oldf.lon) * (oldf.lat - lldf.lat) / (lldf.lon + tau - oldf.lon);
                //std::cout << "Seam splitting clockwise - End: " << ilat << "," << tiny - pi << " Start: " << ilat << "," << pi - tiny << "\n";
                ip2 = getLoc3D(ilat, pi - tiny);
                ip1 = getLoc3D(ilat, tiny - pi);
            }
            split = true;
        }

        // If a POLE is crossed, split the path and insert intermediate points
        if (abs(dflon) + tiny > pi && !split) { // Longitude inversion at pole, if not already split at seam
            if (oldf.lon > lldf.lon) { // Passed counter clockwise (+ to -) - Will never happen if addArc() is swapping directions to always be clockwise
                //std::cout << "Pole splitting counter cloclwise - Old: " << rad2deg * oldf.lat << "," << rad2deg * oldf.lon << " new: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                ip1 = getLoc3D(lldf.lat > 0 ? pi2 : -pi2, oldf.lon); // End path1
                ip2 = getLoc3D(lldf.lat > 0 ? pi2 : -pi2, lldf.lon); // Start path2
                //std::cout << " - End: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << rad2deg * lldf.lon << "," << " Start: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * oldf.lon << "\n";
            }
            else { // Passed clockwise
                //std::cout << "Pole splitting cloclwise - Old: " << rad2deg * oldf.lat << "," << rad2deg * oldf.lon << " new: " << rad2deg * lldf.lat << "," << rad2deg * lldf.lon << "\n";
                ip1 = getLoc3D(lldf.lat > 0 ? pi2 : -pi2, oldf.lon); // End path1
                ip2 = getLoc3D(lldf.lat > 0 ? pi2 : -pi2, lldf.lon); // Start path2
                //std::cout << " - End: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * oldf.lon << " Start: " << rad2deg * (lldf.lat > 0 ? pi2 : -pi2) << "," << rad2deg * lldf.lon << "\n";
            }
            split = true;
        }
        if (split) {
            // Now insert the extra points to end one path and begin the next
            addArcPoint(ip1, false, true, oldap, curve); // last=true so cap path and start new one
            curve->generate();
            curve = p.path2;
            addArcPoint(ip2, true, false, oldap, curve);  // First point in new path
            split = false;
        }
        if (0.0 < f && f < p.fend) addArcPoint(tp, false, false, oldap, curve);
        oldf = lldf;
    }
    if (!p.closed) {
        addArcPoint(getLoc3D(p.lld2.lat, p.lld2.lon), false, true, oldap, curve); // End point of open path - last = true
        //std::cout << "Open curve last point: " << rad2deg * p.lld2.lat << "," << rad2deg * p.lld2.lon << "\n";
    }
    if (p.closed) {
        addArcPoint(getLoc3D(first.lat, first.lon), false, true, oldap, curve);
        //std::cout << "Closed curve last point: " << rad2deg * first.lat << "," << rad2deg * first.lon << "\n";
    } // End point of closed path - last = true
    curve->generate();
    //std::cout << "Curve generation completed.\n";
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
            m_verts.push_back({ loc, nms, nml, glm::vec2(u / (float)m_meshU + texture_x / 8192.0f, v / (float)m_meshV) + texture_y / 8192.0f, BLACK });
            if (u < m_meshU && v < m_meshV) {
                m_tris.push_back({ (m_meshU + 1) * v + u , (m_meshU + 1) * (v + 1) + u, (m_meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (m_meshU + 1) * (v + 1) + 1 + u, (m_meshU + 1) * v + u + 1, (m_meshU + 1) * v + u });
            }
        }
    }
}
void Earth::updGeom() { // Currently only used when shifting texture interactively (to tweak alignment with CountryBorders)
    double lat, lon;
    m_verts.clear();
    m_tris.clear();
    for (unsigned int v = 0; v <= m_meshV; v++) {  // -pi/2 to pi/2 => (v/m_meshV)*pi -pi/2
        lat = (pi * v / m_meshV) - pi2;
        for (unsigned int u = 0; u <= m_meshU; u++) {
            lon = (tau * u / m_meshU) - pi;
            glm::vec3 loc = getLoc3D(lat, lon);     // position of vertex
            glm::vec3 nms = getNml3D_NS(lat, lon);  // Insolation normal, always NS (or G8?)
            glm::vec3 nml = getNml3D(lat, lon);     // Geometry normal, for camera lighting
            m_verts.push_back({ loc, nms, nml, glm::vec2(u / (float)m_meshU + texture_x / 8192.0f, v / (float)m_meshV) + texture_y / 8192.0f, BLACK });
            if (u < m_meshU && v < m_meshV) {
                m_tris.push_back({ (m_meshU + 1) * v + u , (m_meshU + 1) * (v + 1) + u, (m_meshU + 1) * (v + 1) + 1 + u });
                m_tris.push_back({ (m_meshU + 1) * (v + 1) + 1 + u, (m_meshU + 1) * v + u + 1, (m_meshU + 1) * v + u });
            }
        }
    }
    vb->UpdateData(&m_verts[0], (unsigned int)m_verts.size() * sizeof(EarthV));
    gen_texture_x = texture_x;
    gen_texture_y = texture_y;
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
            if (tropicsoverlay && abs(lat) < 24.0 * deg2rad) m_verts[i].color = tropics;
            else if (arcticsoverlay && abs(lat) > (90.0 - 23.4) * deg2rad) m_verts[i].color = arctics;
            else m_verts[i].color = BLACK;
            i++;
        }
    }
    vb->LoadData(&m_verts[0], (unsigned int)m_verts.size() * sizeof(EarthV));
    return;
}
void Earth::updateSun() {
    // Get XYZ of subsolar point and flat sun (as per sun height set in GUI)
    subsolar = m_scene->astro->getDecGHA(A_SUN, NO_DOUBLE); // subsolar.lon is west of south, 0 to tau
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


// -------------------
//  Sub Stellar Point
// -------------------
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
void SubStellarPoint::Name::enable(Font* font, glm::vec4 color, float size) {
    m_font = font;
    if (color != NO_COLOR) m_color = color;
    if (size != NO_FLOAT) m_size = size;
    if (m_billboard == nullptr) m_billboard = new BillBoard(m_ssp->m_earth->m_scene, font, m_ssp->name, m_ssp->m_pos, m_color, m_size);
    m_enabled = true;
    update();
}
void SubStellarPoint::Name::update() {
    m_billboard->update(m_ssp->m_pos + 0.1f * m_ssp->m_earth->getNml3D(m_ssp->m_loc.lat, m_ssp->m_loc.lon));
}
SubStellarPoint::Name::Name(SubStellarPoint& ssp, std::string starname) {
    m_name = starname;
    m_ssp = &ssp;
    m_color = m_ssp->m_color;
}
SubStellarPoint::Name::~Name() {
    if (m_billboard != nullptr) delete m_billboard;
}
void SubStellarPoint::Dot::enable(glm::vec4 color, float size) {
    if (color != NO_COLOR) m_color = color;
    if (size != NO_FLOAT) m_size = size;
    update();
    enabled = true;
}
void SubStellarPoint::Dot::disable() { enabled = false; }
void SubStellarPoint::Dot::update() {
    m_dotsF->changeXYZ(dotindex, m_ssp->m_pos, m_color, m_size);
    //std::cout << "Dot position: " << m_ssp->m_pos.x << "," << m_ssp->m_pos.y << "," << m_ssp->m_pos.z << '\n';
}
SubStellarPoint::Dot::Dot(SubStellarPoint& ssp) {
    m_ssp = &ssp;
    m_color = m_ssp->m_color;
    m_dotsF = m_ssp->m_earth->m_scene->getDotsFactory();
    dotindex = m_dotsF->addXYZ(m_ssp->m_pos, glm::vec4(0.0f), m_size); // Color with alpha = 0.0f is not rendered (shader discards explicitly)
}
SubStellarPoint::Dot::~Dot() {
    if (m_dotsF != nullptr) m_dotsF->remove(dotindex);
}
void SubStellarPoint::SumnerLine::enable(glm::vec4 color, float width) {
    if (color != NO_COLOR) m_color = color;
    if (width != NO_FLOAT) m_width = width;
    update();
    enabled = true;
}
void SubStellarPoint::SumnerLine::disable() { enabled = false; }
void SubStellarPoint::SumnerLine::update() {
    //std::cout << "SubStellarPoint{" << m_ssp << "}::SumnerLine::update() called\n";
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
SubStellarPoint::SumnerLine::SumnerLine(SubStellarPoint& ssp) {
    m_ssp = &ssp;
    if (m_ssp->m_color != NO_COLOR) m_color = m_ssp->m_color;
    curve = m_ssp->m_earth->m_scene->newPolyCurve(m_color, m_width, steps);
    update();
}
SubStellarPoint::SumnerLine::~SumnerLine() {
    if (curve != nullptr) m_ssp->m_earth->m_scene->deletePolyCurve(curve);
}
void SubStellarPoint::Radius::enable() { enabled = true; }
void SubStellarPoint::Radius::changeAzimuth(double bearing, bool rad) {
    if (!rad) bearing *= deg2rad;
    m_bearing = tau - bearing;
    update();
}
void SubStellarPoint::Radius::changeAzimuthTo(LLD target, bool rad) {
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
void SubStellarPoint::Radius::update() {
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
SubStellarPoint::Radius::Radius(SubStellarPoint& ssp) : m_ssp(&ssp) {
}
SubStellarPoint::Radius::~Radius() {
    if (m_radius != NO_UINT) m_ssp->m_earth->removeGreatArc(m_radius);
}
SubStellarPoint::SubStellarPoint(Earth& earth, const std::string& starname, const bool lock, const double jd_tt, const glm::vec4 color) {
    m_earth = &earth;
    name = starname;
    locked = lock;
    if (jd_tt != NO_DOUBLE) m_jd = jd_tt;
    if (color == NO_COLOR) m_color = m_earth->m_scene->astro->getColorbyName(name);
    else m_color = color;
    m_decra = m_earth->m_scene->astro->getTrueDecRAbyName(name, m_jd, true);
    //m_decra = m_earth->m_scene->m_astro->getDecRAbyName(name, true);
    m_loc = m_earth->calcRADec2LatLon(m_decra, EDateTime::getJDTT2UTC(m_jd));
    m_loc.lat += m_dist_lat; // Is always 0.0 at construction time, is changed by shiftSpeedTime()
    //m_earth->addDot(m_decra.lat, m_decra.lon, 0.0, 0.02f, LIGHT_RED, true);
    m_pos = m_earth->getLoc3D(m_loc.lat, m_loc.lon);
    dot = new SubStellarPoint::Dot(*this);
    sumner = new SubStellarPoint::SumnerLine(*this);
    nametag = new SubStellarPoint::Name(*this, name);
    radius = new SubStellarPoint::Radius(*this);
}
SubStellarPoint::~SubStellarPoint() {
    if (dot != nullptr) delete dot;
    if (sumner != nullptr) delete sumner;
    if (nametag != nullptr) delete nametag;
    if (radius != nullptr) delete radius;
}
void SubStellarPoint::shiftSpeedTime(double bearing, double knots, double minutes) {
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
void SubStellarPoint::adjustElevation() {
    double dip = (m_observerHeight == NO_DOUBLE) ? 0.0 : ACoord::dms2rad(0.0, 1.76 * sqrt(m_observerHeight), 0.0);
    m_elevation = m_rawElevation - m_indexError - dip;
    double refraction = (m_temperature == NO_DOUBLE || m_pressure == NO_DOUBLE) ? 0.0 : 60 * ACoord::dms2rad(0.0, m_earth->calcRefractionBennett(rad2deg * m_elevation, m_temperature, m_pressure), 0.0);
    m_elevation -= refraction; // Reverse the predicted refraction to get from apparent to true elevation
    //std::cout << "SubStellarPoint {" << this << "} - " << name
    //    //<< ": Index Error = " << Astronomy::angle2DMstring(m_indexError, true)
    //    << " Dip = " << Astronomy::angle2DMstring(dip, true)
    //    << " Refraction = " << Astronomy::angle2DMSstring(refraction, true)
    //    << " True ele = " << Astronomy::angle2DMSstring(m_elevation, true)
    //    << "\n";
    // FIX: !!! Internally we apparently use co-latitude, check calculations in *PointSolver and in SubStellarPoint::SumnerLine !!!
    m_elevation = pi2 - m_elevation;
}
void SubStellarPoint::setElevation(double elevation, bool rad) {
    // Actually stores the radius of the SumnerLine, so maybe rename the variable, at best it is the co-elevation !!!
    m_rawElevation = rad ? elevation : elevation * deg2rad;
}
void SubStellarPoint::setIndexError(double indexError, bool rad) {
    m_indexError = rad ? indexError : deg2rad * indexError;
}
void SubStellarPoint::setObserverHeight(double observerHeight) {
    m_observerHeight = observerHeight;
}
void SubStellarPoint::setRefraction(double temperature, double pressure) {
    // If at least one argument is NO_DOUBLE, refraction calculation is not performed.
    m_temperature = temperature;
    m_pressure = pressure;
}
LLD SubStellarPoint::getDetails(bool rad) {
    adjustElevation();
    return rad ? LLD{ m_loc.lat, m_loc.lon, m_elevation } : LLD{ rad2deg * m_loc.lat, rad2deg * m_loc.lon, rad2deg * m_elevation };
}
void SubStellarPoint::update() {
    //std::cout << "SubStellarPoint::update()\n";
    if (!locked) { // Locked means don't update location with time
        m_loc = m_earth->calcRADec2LatLon(m_decra, true); // m_jd not passed, so Earth calculates new location based on current time
        m_loc.lat += m_dist_lat;
    }
    // position may still change with the same location if Earth morphs
    m_pos = m_earth->getLoc3D(m_loc.lat, m_loc.lon);
    // Still update to geometry and camera orientation (for the name billboard)
    adjustElevation();
    dot->update();
    sumner->update();
    radius->update();
    nametag->update();
}


// -----------------
//  SubPoint Solver
// -----------------
    // Solves a fix for 3 observations using SubPoint - should evolve into n-point solver
    // 1) add points, 2) pull solution
SubPointSolver::SubPointSolver(Earth* earth) : m_earth(earth) {

}
SubStellarPoint* SubPointSolver::addSubStellarPoint(const std::string starname, const double elevation, bool rad, const double jd_tt) {
    // Create a new SubStellarPoint and add it to the solver, returning a reference to the caller
    double myjd = jd_tt;
    if (myjd == NO_DOUBLE) myjd = m_earth->m_scene->astro->getJD_TT();
    m_ssps.push_back(new SubStellarPoint(*m_earth, starname, true, myjd, NO_COLOR));
    m_ssps.back()->setElevation(elevation, rad);
    return m_ssps.back();
}
void SubPointSolver::addSubStellarPoint(SubStellarPoint* ssp) {
    // Add an existing SubStellarPoint to the solver
    m_ssps.push_back(ssp);
}
// Perhaps add feature to show the star in the observable color, which is already available anyway !!!
// - Best achieved by making the star color the default color in SubStellarPoint
void SubPointSolver::showSumnerLines(glm::vec4 color, float width) {
    for (auto& s : m_ssps) {
        s->sumner->enable(color, width);
    }
}
void SubPointSolver::showNames(Font* font, glm::vec4 color, float size) {
    for (auto& s : m_ssps) {
        s->nametag->enable(font, color, size);
    }
}
void SubPointSolver::showDots(glm::vec4 color, float size) {
    for (auto& s : m_ssps) {
        s->dot->enable(color, size);
    }
}
void SubPointSolver::update() {
    for (auto& s : m_ssps) {
        s->update();
    }
}
LLD SubPointSolver::calcLocation(bool rad) { // Calculates intersections on a spherical Earth (NS). Possibly add AE, ER etc.
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
    solution = m_earth->getXYZtoLLD_NS(glm::normalize((p1 + p2 + p3) / 3.0f)); // Earth radius is 1.0f, otherwise multiply by earth_r

    //
    // centroid calculation with more points is slightly less straight forward, see:
    // https://gis.stackexchange.com/questions/164267/how-exactly-is-the-centroid-of-polygons-calculated#:~:text=The%20centroid%20(a.k.a.%20the%20center,%2B%20y2%20%2B%20y3)%2F3.
    // https://drive.google.com/file/d/0B6wCLzdYQE_gOUVTc0FuOVFZbHM/view?usp=sharing&resourcekey=0-4Vdlynw2E9fKwpHclOJxEQ
    // (last one from https://sites.google.com/site/navigationalalgorithms/Home/papersnavigation which has other great reads too)
    if (rad) return solution;
    else return { rad2deg * solution.lat, rad2deg * solution.lon, 0.0 };
}
Intersection SubPointSolver::calcSumnerIntersection(LLD lld1, LLD lld2, bool rad) {
    // Returns NO_DOUBLE for all values if there are no intersections (circles are concentric or antipodal)
    // Source: https://gis.stackexchange.com/questions/48937/calculating-intersection-of-two-circles
    if (!rad) {
        lld1.lat *= deg2rad; lld1.lon *= deg2rad; lld1.dst *= deg2rad;
        lld2.lat *= deg2rad; lld2.lon *= deg2rad; lld2.dst *= deg2rad;
    }
    // transform from spherical to cartesian coordinates using LLD to store coordinates as doubles: (lat,lon,dst) <- (x,y,z)
    LLD pos1{ cos(lld1.lon) * cos(lld1.lat), sin(lld1.lon) * cos(lld1.lat), sin(lld1.lat) };
    LLD pos2{ cos(lld2.lon) * cos(lld2.lat), sin(lld2.lon) * cos(lld2.lat), sin(lld2.lat) };
    // q equal to pos1 dot pos2
    double q = pos1.lat * pos2.lat + pos1.lon * pos2.lon + pos1.dst * pos2.dst;
    double q2 = q * q;
    // q2 == 1.0 gives DIV0 in the following, and indicates that the points coincide or are antipodal.
    if (abs(q2 - 1.0) < verytiny) {
        std::cout << "Earth::calcSumnerIntersection() the circles are not intersecting! (q*q is very close to 1.0)\n";
        return { {NO_DOUBLE, NO_DOUBLE, NO_DOUBLE},{NO_DOUBLE, NO_DOUBLE, NO_DOUBLE} };
    }
    // pos0 will be a unique point on the line of intersection of the two planes defined by the two distance circles
    double a = (cos(lld1.dst) - cos(lld2.dst) * q) / (1 - q2);
    double b = (cos(lld2.dst) - cos(lld1.dst) * q) / (1 - q2);
    // pos0 is a linear combination of pos1 and pos2 with the parameters a and b
    LLD pos0 = { a * pos1.lat + b * pos2.lat, a * pos1.lon + b * pos2.lon, a * pos1.dst + b * pos2.dst };
    // n equal to pos1 cross pos2, normal to both
    LLD n = { pos1.lon * pos2.dst - pos1.dst * pos2.lon, pos1.dst * pos2.lat - pos1.lat * pos2.dst, pos1.lat * pos2.lon - pos1.lon * pos2.lat };
    // t = sqrt((1.0 - dot(pos0, pos0)) / glm::dot(n, n)); (a vector dot itself is of course the square of its magnitude
    double t = sqrt((1.0 - (pos0.lat * pos0.lat + pos0.lon * pos0.lon + pos0.dst * pos0.dst)) / (n.lat * n.lat + n.lon * n.lon + n.dst * n.dst));
    //isect1 = pos0 + t * n and isect2 = pos0 - t * n, where t is a scalar
    LLD isect1 = { pos0.lat + t * n.lat, pos0.lon + t * n.lon, pos0.dst + t * n.dst };
    LLD isect2 = { pos0.lat - t * n.lat, pos0.lon - t * n.lon, pos0.dst - t * n.dst };
    // Transform back to spherical coordinates - Are isect1 & 2 always unit vectors? !!!
    LLD ll1 = { atan2(isect1.dst, sqrt(isect1.lat * isect1.lat + isect1.lon * isect1.lon)),
        atan2(isect1.lon, isect1.lat), sqrt(isect1.lat * isect1.lat + isect1.lon * isect1.lon + isect1.dst * isect1.dst) };
    LLD ll2 = { atan2(isect2.dst, sqrt(isect2.lat * isect2.lat + isect2.lon * isect2.lon)),
        atan2(isect2.lon, isect2.lat), sqrt(isect2.lat * isect2.lat + isect2.lon * isect2.lon + isect2.dst * isect2.dst) };
    // if degrees were passed in, return degrees rather than radians
    if (!rad) {
        ll1 = { ll1.lat * rad2deg, ll1.lon * rad2deg, ll1.dst * rad2deg };
        ll2 = { ll2.lat * rad2deg, ll2.lon * rad2deg, ll2.dst * rad2deg };
    }
    return { ll1, ll2 };
}
double SubPointSolver::calcArcDist(LLD lld1, LLD lld2, bool rad) {
    // Calculate great circle distance using Vincenty formula simplified for sphere rather than ellipsoid.
    // Source: https://en.wikipedia.org/wiki/Great-circle_distance#Computational_formulas
    // NOTE: The Vector version given in the same source is interesting.
    //       If locations are given by position normals rather than lat&lon, then:
    //       ArcDistance = atan2( |n1 x n2| / (n1 . n2) ) on a unit sphere.
    if (!rad) {
        lld1.lat *= deg2rad; // doubles
        lld1.lon *= deg2rad;
        lld2.lat *= deg2rad;
        lld2.lon *= deg2rad;
    }
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
    if (!rad) dist *= rad2deg;
    return dist;
}


// --------------------
//  Three Point Solver
// --------------------
    // Solves a fix for 3 observations using StellarSubPoint
    // 1) add points, 2) optionally set other parameters, 3) pull solution
ThreePointSolver::ThreePointSolver(Earth* earth) : m_earth(earth) {
}
SubStellarPoint* ThreePointSolver::addSubStellarPoint(const std::string starname, const double elevation, bool rad, const double jd_tt) {
    // Create a new SubStellarPoint and add it to the solver, returning a reference to the caller
    if (m_ssps.size() > 2) {
        std::cout << "WARNING: ThreePointSolver::addSubStellarPoint() There are already 3 stars defined. Ignoring star: " << starname << " and returning last added star.\n";
        return m_ssps.back();
    }
    double myjd = jd_tt;
    if (myjd == NO_DOUBLE) myjd = m_earth->m_scene->astro->getJD_TT();
    m_ssps.push_back(new SubStellarPoint(*m_earth, starname, true, myjd, NO_COLOR));
    m_ssps.back()->setElevation(elevation, rad);
    return m_ssps.back();
}
void ThreePointSolver::addSubStellarPoint(SubStellarPoint* ssp) {
    // Add an existing SubStellarPoint to the solver
    if (m_ssps.size() > 2) {
        std::cout << "WARNING: ThreePointSolver::addSubStellarPoint() There are already 3 stars defined. Ignoring!\n";
        return;
    }
    m_ssps.push_back(ssp);
}
void ThreePointSolver::setIndexError(double indexError, bool rad) {
    for (auto& s : m_ssps) {
        s->setIndexError(indexError, rad);
    }
}
void ThreePointSolver::setObserverHeight(double observerHeight) {
    //m_obsHeight = observerHeight;
    for (auto& s : m_ssps) {
        s->setObserverHeight(observerHeight);
    }
}
void ThreePointSolver::setRefraction(double temperature, double pressure) {
    //m_temperature = temperature;
    //m_pressure = pressure;
    for (auto& s : m_ssps) {
        s->setRefraction(temperature, pressure);
    }
}
void ThreePointSolver::showSumnerLines(glm::vec4 color, float width) {
    for (auto& s : m_ssps) {
        s->sumner->enable(color, width);
    }
}
void ThreePointSolver::showNames(Font* font, glm::vec4 color, float size) {
    for (auto& s : m_ssps) {
        s->nametag->enable(font, color, size);
    }
}
void ThreePointSolver::showDots(glm::vec4 color, float size) {
    for (auto& s : m_ssps) {
        s->dot->enable(color, size);
    }
}
void ThreePointSolver::update() {
    for (auto& s : m_ssps) {
        s->update();
    }
}
LLD ThreePointSolver::calcLocation(bool rad) { // Calculates intersections on a spherical Earth (NS). Possibly add AE, ER etc.
    // Should check if at least 3 points were supplied already !!!
    if (m_ssps.size() < 3) {
        std::cout << "WARNING: ThreePointSolver::calcLocation() There are less than 3 stars defined. Can't calculate, so returning invalid location!\n";
        return LLD{ NO_DOUBLE, NO_DOUBLE, NO_DOUBLE };
    }

    // adjust and refract in SSPs or here??? I want to see the difference in the SumnerLines, so in SSPs!


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
    solution = m_earth->getXYZtoLLD_NS(glm::normalize((p1 + p2 + p3) / 3.0f)); // Earth radius is 1.0f, otherwise multiply by earth_r

    //
    // centroid calculation with more points is slightly less straight forward, see:
    // https://gis.stackexchange.com/questions/164267/how-exactly-is-the-centroid-of-polygons-calculated#:~:text=The%20centroid%20(a.k.a.%20the%20center,%2B%20y2%20%2B%20y3)%2F3.
    // https://drive.google.com/file/d/0B6wCLzdYQE_gOUVTc0FuOVFZbHM/view?usp=sharing&resourcekey=0-4Vdlynw2E9fKwpHclOJxEQ
    // (last one from https://sites.google.com/site/navigationalalgorithms/Home/papersnavigation which has other great reads too)
    if (rad) return solution;
    else return { rad2deg * solution.lat, rad2deg * solution.lon, 0.0 };
}
Intersection ThreePointSolver::calcSumnerIntersection(LLD lld1, LLD lld2, bool rad) {
    // Returns NO_DOUBLE for all values if there are no intersections (circles are concentric or antipodal)
    // Source: https://gis.stackexchange.com/questions/48937/calculating-intersection-of-two-circles
    if (!rad) {
        lld1.lat *= deg2rad; lld1.lon *= deg2rad; lld1.dst *= deg2rad;
        lld2.lat *= deg2rad; lld2.lon *= deg2rad; lld2.dst *= deg2rad;
    }
    // transform from spherical to cartesian coordinates using LLD to store coordinates as doubles: (lat,lon,dst) <- (x,y,z)
    LLD pos1{ cos(lld1.lon) * cos(lld1.lat), sin(lld1.lon) * cos(lld1.lat), sin(lld1.lat) };
    LLD pos2{ cos(lld2.lon) * cos(lld2.lat), sin(lld2.lon) * cos(lld2.lat), sin(lld2.lat) };
    // q equal to pos1 dot pos2
    double q = pos1.lat * pos2.lat + pos1.lon * pos2.lon + pos1.dst * pos2.dst;
    double q2 = q * q;
    // q2 == 1.0 gives DIV0 in the following, and indicates that the points coincide or are antipodal.
    if (abs(q2 - 1.0) < verytiny) {
        std::cout << "Earth::calcSumnerIntersection() the circles are not intersecting! (q*q is very close to 1.0)\n";
        return { {NO_DOUBLE, NO_DOUBLE, NO_DOUBLE},{NO_DOUBLE, NO_DOUBLE, NO_DOUBLE} };
    }
    // pos0 will be a unique point on the line of intersection of the two planes defined by the two distance circles
    double a = (cos(lld1.dst) - cos(lld2.dst) * q) / (1 - q2);
    double b = (cos(lld2.dst) - cos(lld1.dst) * q) / (1 - q2);
    // pos0 is a linear combination of pos1 and pos2 with the parameters a and b
    LLD pos0 = { a * pos1.lat + b * pos2.lat, a * pos1.lon + b * pos2.lon, a * pos1.dst + b * pos2.dst };
    // n equal to pos1 cross pos2, normal to both
    LLD n = { pos1.lon * pos2.dst - pos1.dst * pos2.lon, pos1.dst * pos2.lat - pos1.lat * pos2.dst, pos1.lat * pos2.lon - pos1.lon * pos2.lat };
    // t = sqrt((1.0 - dot(pos0, pos0)) / glm::dot(n, n)); (a vector dot itself is of course the square of its magnitude
    double t = sqrt((1.0 - (pos0.lat * pos0.lat + pos0.lon * pos0.lon + pos0.dst * pos0.dst)) / (n.lat * n.lat + n.lon * n.lon + n.dst * n.dst));
    //isect1 = pos0 + t * n and isect2 = pos0 - t * n, where t is a scalar
    LLD isect1 = { pos0.lat + t * n.lat, pos0.lon + t * n.lon, pos0.dst + t * n.dst };
    LLD isect2 = { pos0.lat - t * n.lat, pos0.lon - t * n.lon, pos0.dst - t * n.dst };
    // Transform back to spherical coordinates - Are isect1 & 2 always unit vectors? !!!
    LLD ll1 = { atan2(isect1.dst, sqrt(isect1.lat * isect1.lat + isect1.lon * isect1.lon)),
        atan2(isect1.lon, isect1.lat), sqrt(isect1.lat * isect1.lat + isect1.lon * isect1.lon + isect1.dst * isect1.dst) };
    LLD ll2 = { atan2(isect2.dst, sqrt(isect2.lat * isect2.lat + isect2.lon * isect2.lon)),
        atan2(isect2.lon, isect2.lat), sqrt(isect2.lat * isect2.lat + isect2.lon * isect2.lon + isect2.dst * isect2.dst) };
    // if degrees were passed in, return degrees rather than radians
    if (!rad) {
        ll1 = { ll1.lat * rad2deg, ll1.lon * rad2deg, ll1.dst * rad2deg };
        ll2 = { ll2.lat * rad2deg, ll2.lon * rad2deg, ll2.dst * rad2deg };
    }
    return { ll1, ll2 };
}
double ThreePointSolver::calcArcDist(LLD lld1, LLD lld2, bool rad) {
    // Calculate great circle distance using Vincenty formula simplified for sphere rather than ellipsoid.
    // Source: https://en.wikipedia.org/wiki/Great-circle_distance#Computational_formulas
    // NOTE: The Vector version given in the same source is interesting.
    //       If locations are given by position normals rather than lat&lon, then:
    //       ArcDistance = atan2( |n1 x n2| / (n1 . n2) ) on a unit sphere.
    if (!rad) {
        lld1.lat *= deg2rad; // doubles
        lld1.lon *= deg2rad;
        lld2.lat *= deg2rad;
        lld2.lon *= deg2rad;
    }
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
    if (!rad) dist *= rad2deg;
    return dist;
}


// ----------
//  Location
// ----------
Location::Location(Earth* earth, double lat, double lon, bool radians, float rsky) {
    m_earth = earth;
    m_scene = m_earth->m_scene;  // Later, refactor this so the Location can be on other planetary bodies, like the Moon or a planet.
    m_radius = rsky;
    m_arrows = m_scene->getArrowsFactory();
    m_dots = m_scene->getDotsFactory();
    m_planes = m_scene->getPlanesFactory();
    m_cylinders = m_scene->getCylindersFactory();
    m_anglearcs = m_scene->getAngleArcsFactory();
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

    m_world2local = calcWorld2LocalMatrix();

    // The new way
    truesun = new Location::TrueSun(this); // Construct but don't enable any items within (arrows, paths, ...)
    flatsun = new Location::FlatSun(this);
    //azielegrid = new Location::EleAziGrid(this);
}
Location::~Location() {
    //std::cout << "~Loc (destructor) called.\n";
    Destroy();

    // New way
    delete truesun; // It is always constructed, so always destroy.
    delete flatsun;
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
void Location::Update(bool time, bool morph, bool sunflat) {
    // External callers: Earth::Update() only
    // If location has moved, pretend it was a morph, the same objects need updating.
    if (morph) {
        // Location geometry
        m_pos = m_earth->getLoc3D(m_lat, m_lon);
        m_east = m_earth->getEast3D(m_lat, m_lon);
        m_north = m_earth->getNorth3D(m_lat, m_lon);
        m_zenith = glm::cross(m_east, m_north);
        m_world2local = calcWorld2LocalMatrix();
    }
    // Update dots
    for (auto& d : m_dotcache) {
        if (d.type == LOC && morph) updateLocDot(d);
        if (d.type == LOCSKY && morph) updateLocSky(d);
        if (d.type == TRUEPLANET3D) updateTruePlanetDot(d);
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
        //if (a.type == FLATSUN3D && (time || morph || flatsun)) updateArrow3DFlatSun(a);
        if (a.type == TRUEMOON3D && (time || morph)) updateArrow3DTrueMoon(a);
        if (a.type == TRUEPLANET3D && (time || morph)) updateArrow3DTruePlanet(a);
        if (a.type == AZIELE3D && morph) updateArrow3DEleAzi(a);
        if (a.type == RADEC3D && (time || morph)) updateArrow3DDecRA(a);
    }
    // Update Lines
    for (auto& l : m_cylindercache) {
        if (l.type == FLATSUN3D) updateLine3DFlatSun(l);
    }
    // Update paths
    for (auto& pa : m_polycache.m_Elements) {
        if (pa.type == TRUESUN3D && (time || morph)) updatePlanetaryPath(pa);
        if (pa.type == TRUEANALEMMA3D && (time || morph)) updatePlanetaryPath(pa);
        //if (pa.type == FLATSUN3D && (time || morph || sunflat)) updatePath3DFlatSun(pa.path, pa.color, pa.width);
        if (pa.type == FLATSUN3D && (time || morph || sunflat)) flatsun->doPath24();
        if (pa.type == TRUEMOON3D && (time || morph)) updatePath3DTrueMoon(pa);
        if (pa.type == TRUELUNALEMMA3D && (time || morph)) updateTrueLunalemma(pa);
        if (pa.type == TRUEPLANET3D && (time || morph)) updatePlanetaryPath(pa);
        if (pa.type == SIDPLANET3D && (time || morph)) updatePlanetaryPath(pa);
        if (pa.type == RADEC3D && (time || morph)) updatePath3DRADec(pa);
        if (pa.type == ECGEO && (time || morph)) updatePlanetaryPath(pa);
        if (pa.type == HORIZON && morph) updatePathHorizon(pa);
        if (pa.type == CIRCUMPOLAR && morph) updatePathCircumPolar(pa);
    }
    truesun->update(time, morph);
    flatsun->update(time, morph);
    if (azielegrid) azielegrid->update(time, morph);
}
void Location::Draw(Camera* cam) {
    // Don't draw Primitives derived objects, as they are drawn from World.
    for (auto& pa : m_polycache.m_Elements) {
        pa.path->draw(cam);
    }
    truesun->draw();
    flatsun->draw();
}
glm::vec4 Location::getPlanetColor(size_t planet, glm::vec4 color) {
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
    std::cout << "WARNING! Location::getPlanetColor() called with unknown planet: " << planet << "\n";
    return WHITE;
}
void Location::moveLoc(double lat, double lon, bool rad) {
    if (!rad) {
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
glm::vec3 Location::getPosition() {
    return m_pos;
}
glm::vec3 Location::getZenith() { return m_zenith; }
glm::vec3 Location::getNorth() { return m_north; }

void Location::moveToXYZ(glm::vec3 pos) {
    //std::cout << "Location::moveToXYZ() was called.\n";
    LLD lld = m_earth->getXYZtoLLD_NS(pos);
    moveLoc(lld.lat, lld.lon);
    //std::cout << "Lat, lon: " << lld.lat << ", " << lld.lon << "\n";
}
void Location::setTimeZone(const std::string& timezonename) {
    // Check if the time zone is recognized
    m_timezone = timezonename;
}

// Calculations
glm::vec3 Location::calcEleAzi2Dir(LLD heading, bool rad) {
    if (!rad) {
        heading.lat *= deg2rad;
        heading.lon *= deg2rad;
    }
    if (abs(pi2 - heading.lat) < tiny) return m_zenith;
    glm::vec3 dir = -m_north; // South
    dir = glm::rotate(dir, (float)-heading.lat, m_east);    // Elevation + from horizonal
    dir = glm::rotate(dir, (float)-heading.lon, m_zenith);  // Azimuth W of S
    return glm::normalize(dir);
}
LLD Location::calcDir2EleAzi(glm::vec3 direction, bool rad) {
    // Direction in cartesian world coordinates
    glm::vec4 ldir = m_world2local * glm::vec4(direction, 1.0f);
    glm::vec3 localdir = glm::vec3(ldir.x, ldir.y, ldir.z);
    //std::cout << "localdir: "; VPRINT(localdir);
    LLD lld = { 0.0, 0.0, 0.0 };
    glm::vec3 zp = projectVector2Plane(localdir, glm::vec3(0.0f, 0.0f, 1.0f));
    lld.lon = atan2(zp.y, zp.x) - pi;
    if (lld.lon < 0.0) lld.lon += tau;
    if (lld.lon >= tau) lld.lon -= tau;
    lld.lat = pi2 - acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), localdir));
    return lld;
}
glm::vec3 Location::calcDecRA2Dir(double jd_utc, double dec, double ra, bool rad) {
    // Defaults to taking degrees, set rad = true otherwise. Pass jd = NO_DOUBLE to use current JD
    if (!rad) {
        dec *= deg2rad;
        ra *= deg2rad;
    }
    if (jd_utc == NO_DOUBLE) jd_utc = m_scene->astro->getJD_UTC();
    double gsidtime = m_scene->astro->ApparentGreenwichSiderealTime(jd_utc, true); // In radians
    double gha = gsidtime - ra; // calcGeo2Topo() needs GHA, not LHA
    LLD topocentric = m_scene->astro->calcGeo2Topo({ dec, gha, 0.0 }, { m_lat, m_lon, 0.0 });
    //std::cout << "Elevation, Azimuth: " << rad2deg * topocentric.lat << ", " << rad2deg * topocentric.lon << "\n";
    //std::cout << "Elevation with refraction: " << rad2deg * topocentric.lat - m_earth->calcRefractionBennett(rad2deg * topocentric.lat, 10.0, 1010.0) << '\n';
    glm::vec3 dir = calcEleAzi2Dir(topocentric, true);
    return dir;
}
glm::mat4 Location::calcWorld2LocalMatrix() {
    // Returns a matrix that takes cartesian world coordinates (X,Y,Z) into cartesian local coordinates (north,east,zenith)
    // Ref: http://www.codinglabs.net/article_world_view_projection_matrix.aspx
    glm::mat4 world2local = glm::mat4(1.0f); // Identity matrix, off-diag entries are 0.0f
    world2local[0].x = m_north.x;
    world2local[0].y = m_north.y;
    world2local[0].z = m_north.z;
    world2local[1].x = m_east.x;
    world2local[1].y = m_east.y;
    world2local[1].z = m_east.z;
    world2local[2].x = m_zenith.x;
    world2local[2].y = m_zenith.y;
    world2local[2].z = m_zenith.z;
    //world2local[3].x = m_pos.x; // Translation, in case I copy this for use elsewhere later.
    //world2local[3].y = m_pos.y;
    //world2local[3].z = m_pos.z;
    return world2local;
}
glm::vec3 Location::getTrueSunDir(double jd) {
    LLD sun = m_scene->astro->getDecGHA(A_SUN, jd);
    localsun = m_scene->astro->calcGeo2Topo(sun, { m_lat, m_lon, 0.0 }); // Sun Ele, Azi
    return calcEleAzi2Dir(localsun, true);
}
glm::vec3 Location::getFlatSunDir(double jd) {
    glm::vec3 sunloc = m_earth->getSubsolarXYZ(jd);
    return glm::normalize(sunloc - m_pos);
}
// ================================= The New Way ================================= //
// ------------------------------- True Sun object ------------------------------- //
Location::TrueSun::TrueSun(Location* location) : m_location(location) {
    update(true, true); // Ensure there are valid astronomical values
}
void Location::TrueSun::update(bool time, bool geometry) {
    sun = m_location->m_scene->astro->getDecGHA(A_SUN, NO_DOUBLE); // Current Sun GHA, Dec, no JD specified
    localsun = m_location->m_scene->astro->calcGeo2Topo(sun, { m_location->m_lat, m_location->m_lon, 0.0 }); // Sun Ele, Azi
    // AA+ gives Hour Angles westwards from south in range {pi;pi]. We store them like that, and convert to clockwise from North when displaying degrees.
    //std::cout << "Calculated Azi, Ele: " << rad2deg * localsun.lon << ", " << rad2deg * localsun.lat << "\n";
    sundir = m_location->calcEleAzi2Dir(localsun, true);

    if (m_dot != maxuint) m_location->m_scene->getDotsFactory()->changeXYZ(m_dot, m_location->m_pos + sundir * m_location->m_radius, NO_COLOR, NO_FLOAT);
    if (m_arrow != maxuint) m_location->m_scene->getArrowsFactory()->changeStartDirLen(m_arrow, m_location->m_pos, sundir, NO_FLOAT, NO_FLOAT, NO_COLOR);
    if (m_eleangle != maxuint) { // add checks for dirty flags
        glm::vec3 proj = sundir - m_location->m_zenith * glm::dot(sundir, m_location->m_zenith);
        m_location->m_anglearcs->update(m_eleangle, m_location->m_pos, proj, sundir, m_location->m_radius, defaultcolor, 0.002f);
    }
    if (m_aziangle != maxuint) { // add checks for dirty flags
        glm::vec3 proj = sundir - m_location->m_zenith * glm::dot(sundir, m_location->m_zenith);
        m_location->m_anglearcs->update(m_aziangle, m_location->m_pos, m_location->m_north, proj, m_location->m_radius, defaultcolor, 0.002f, true, -m_location->m_zenith);
    }
    if (m_line != maxuint) m_location->m_cylinders->changeStartEnd(m_line, m_location->m_pos, sundir * 100.0f + m_location->m_pos, 0.001f, defaultcolor);
    // No need to update paths, Location takes care of it!
}
void Location::TrueSun::draw() {
    if (m_eleangtext != nullptr) updateEleAngText();
    if (m_aziangtext != nullptr) updateAziAngText();
    // !!! This duplicates updates if both are enabled. There must be a better way !!!
    // Also, don't update in draw(), do that in update()
    // Also, also, Allocate the paths via Scene, they will then be drawn there.
    if (m_eleangle != maxuint) m_location->m_anglearcs->draw();
    if (m_aziangle != maxuint) m_location->m_anglearcs->draw();
}
void Location::TrueSun::enableArrow3D() { // Add params such as color, length, width (, refraction?)
    if (m_arrow != maxuint) return; // Already enabled
    // Move getTrueSunDir() into Location::TrueSun !!! - in progress, remove from Location once all items have been migrated
    update(true,true); // Calculate parameters 
    m_arrow = m_location->m_scene->getArrowsFactory()->addStartDirLen(m_location->m_pos, sundir, m_location->m_radius,0.003f, defaultcolor);
}
void Location::TrueSun::disableArrow3D() {} // Only add if needed
// updateArrow3D(), or just do it directly in update() ?
void Location::TrueSun::changeArrow3D(glm::vec4 color, float length, float width) {
    m_location->m_scene->getArrowsFactory()->changeArrow(m_arrow, color, length, width);
}
void Location::TrueSun::enableDot3D() {
    m_dot = m_location->m_dots->addXYZ(m_location->m_pos + sundir * m_location->m_radius, defaultcolor, locdotsize);
}
void Location::TrueSun::disableDot3D() {} // Only add if needed
void Location::TrueSun::changeDot3D(glm::vec4 color, float size) {
    m_location->m_scene->getDotsFactory()->changeDot(m_dot, color, size);
}
void Location::TrueSun::enableLine3D() {
    m_line = m_location->m_cylinders->addStartEnd(m_location->m_pos, sundir * 100.0f + m_location->m_pos, 0.001f, defaultcolor);
}
void Location::TrueSun::enableAzimuthAngle(glm::vec4 color, float width) {
    // Should draw north to azi circle arc in tangent plane
    if (color == NO_COLOR) color = defaultcolor;
    glm::vec3 proj = sundir - m_location->m_zenith * glm::dot(sundir, m_location->m_zenith);
    m_aziangle = m_location->m_anglearcs->add(m_location->m_pos, m_location->m_north, proj, m_location->m_radius, color, width, true, -m_location->m_zenith);
}
void Location::TrueSun::enableElevationAngle(glm::vec4 color, float width) { // add params for width & color !!!
    // As per https://www.maplesoft.com/support/help/maple/view.aspx?path=MathApps%2FProjectionOfVectorOntoPlane
    // the actual formula is dividing the dot product by the magnitude of m_zenith squared. However, m_zenith is a unit vector.
    // Also: DO NOT use glm::vec3.length(), it returns the NUMBER of elements, not the magnitude!!! use glm::length(glm::vec3)
    if (color == NO_COLOR) color = defaultcolor;
    glm::vec3 proj = sundir - m_location->m_zenith * glm::dot(sundir, m_location->m_zenith);
    m_eleangle = m_location->m_anglearcs->add(m_location->m_pos, proj, sundir, m_location->m_radius, color, width);
}
void Location::TrueSun::enableEleAngText(Font* font) {
    if (m_eleangtext != nullptr) {
        updateEleAngText();
        return;
    }
    glm::vec3 pos = m_location->m_pos + sundir * m_location->m_radius;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = m_location->m_scene->w_camera->position - pos;
    glm::vec3 dir = glm::cross(up, camdir);
    up = glm::cross(camdir, dir);
    char dstring[10];
    snprintf(dstring, sizeof(dstring), "%03.3f°", rad2deg * localsun.lat);
    std::string angle = dstring;
    m_eleangtext = m_location->m_scene->getTextFactory()->newText(font, angle, 0.08f, defaultcolor, pos, dir, up);
}
void Location::TrueSun::updateEleAngText() {
    glm::vec3 pos = m_location->m_pos + sundir * m_location->m_radius;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = m_location->m_scene->w_camera->position - pos;
    glm::vec3 dir = glm::cross(up, camdir);
    up = glm::cross(camdir, dir);
    m_eleangtext->updatePosDirUp(pos, dir, up);
    char dstring[10];
    snprintf(dstring, sizeof(dstring), "%03.3f°", rad2deg * localsun.lat);
    std::string angle = dstring;
    m_eleangtext->updateText(angle);
}
void Location::TrueSun::enableAziAngText(Font* font) {
    if (m_eleangtext != nullptr) {
        updateAziAngText();
        return;
    }
    glm::vec3 pos = m_location->m_pos + sundir * m_location->m_radius;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = m_location->m_scene->w_camera->position - pos;
    glm::vec3 dir = glm::cross(up, camdir);
    up = glm::cross(camdir, dir);
    char dstring[10];
    snprintf(dstring, sizeof(dstring), "%03.3f°", 180.0 + rad2deg * localsun.lon);
    std::string angle = dstring;
    m_aziangtext = m_location->m_scene->getTextFactory()->newText(font, angle, 0.06f, defaultcolor, pos, dir, up);
}
void Location::TrueSun::updateAziAngText() {
    glm::vec3 pos = m_location->m_pos + sundir * m_location->m_radius;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = m_location->m_scene->w_camera->position - pos;
    glm::vec3 dir = glm::cross(up, camdir);
    up = glm::cross(camdir, dir);
    m_aziangtext->updatePosDirUp(pos, dir, up);
    char dstring[10];
    snprintf(dstring, sizeof(dstring), "%03.3f°", 180.0 + rad2deg * localsun.lon);
    std::string angle = dstring;
    m_aziangtext->updateText(angle);
}void Location::TrueSun::enablePath24() {
    m_location->addPlanetaryPath(A_SUN, -0.5, 0.5, 100, TRUESUN3D, defaultcolor, 0.002f);
}
// No need for an update function, that is handled in Location
void Location::TrueSun::disablePath24() {
    m_location->deletePlanetaryPath(TRUESUN3D, A_SUN);
}
void Location::TrueSun::enableAnalemma() {
    m_location->addPlanetaryPath(A_SUN, -183.0, 183.0, 366, TRUEANALEMMA3D, defaultcolor, 0.002f);
}
void Location::TrueSun::disableAnalemma() {
    m_location->deletePlanetaryPath(TRUEANALEMMA3D, A_SUN);
}
double Location::TrueSun::getElevation(bool radians) {
    if (radians) return localsun.lat;
    return localsun.lat * rad2deg;
}
double Location::TrueSun::getAzimuth(bool radians) {
    if (radians) return localsun.lon;
    return localsun.lon * rad2deg;
}
double Location::TrueSun::getSolarNoonJD() {
    // FIX: !!! This will not actually work. Due to Earth orbit around the Sun, this will always be slightly off !!!
    //          Use interpolation methods instead.
    double diff = m_location->m_lon - m_location->m_earth->calcHADec2LatLon(sun).lon;
    if (diff < 0.0) return (diff + tau) / tau + m_location->m_scene->astro->getJD_UTC();
    return diff / tau + m_location->m_scene->astro->getJD_UTC();
}
// ------------------------------- Flat Sun object ------------------------------- //
Location::FlatSun::FlatSun(Location* location) : m_location(location) {
    update(true, true); // Ensure there are valid astronomical values
}
void Location::FlatSun::update(bool time, bool geometry) {
    sun = m_location->m_scene->astro->getDecGHA(A_SUN, NO_DOUBLE); // Current Sun GHA, Dec, no JD specified
    while (sun.lon < -pi) sun.lon += tau;
    while (sun.lon > pi) sun.lon -= tau;
    // Negate sun.lon to get east of south instead of west of south
    if (sun.lat > pi2 || sun.lat < -pi2) std::cout << "WARNING: Location::FlatSun::update(): sun.lat is out of range: " << sun.lat << "\n";
    if (sun.lon > pi || sun.lon < -pi) std::cout << "WARNING: Location::FlatSun::update(): sun.lon is out of range: " << sun.lon << "\n";
    glm::vec3 sun_xyz = getLoc3D_XX(sun.lat, sun.lon < pi ? -sun.lon : tau - sun.lon, m_location->m_earth->flatsunheight / (float)earthradius);
    sundir = glm::normalize(sun_xyz - m_location->m_pos);
    localsun = m_location->calcDir2EleAzi(sundir);
    //localsun = m_location->m_scene->m_astro->calcGeo2Topo(sun, { m_location->m_lat, m_location->m_lon, 0.0 }); // Sun Ele, Azi

    if (m_dot != maxuint) m_location->m_scene->getDotsFactory()->changeXYZ(m_dot, m_location->m_pos + sundir * m_location->m_radius, NO_COLOR, NO_FLOAT);
    if (m_arrow != maxuint) m_location->m_scene->getArrowsFactory()->changeStartDirLen(m_arrow, m_location->m_pos, sundir, NO_FLOAT, NO_FLOAT, NO_COLOR);
    if (m_eleangle != maxuint) { // add checks for dirty flags
        glm::vec3 proj = sundir - m_location->m_zenith * glm::dot(sundir, m_location->m_zenith);
        m_location->m_anglearcs->update(m_eleangle, m_location->m_pos, proj, sundir, m_location->m_radius, defaultcolor, 0.002f);
    }
    if (m_line != maxuint) m_location->m_cylinders->changeStartEnd(m_line, m_location->m_pos, sundir * 100.0f + m_location->m_pos, 0.001f, defaultcolor);
    // No need to update paths, Location takes care of it!
    if (m_path24 != nullptr) doPath24();
}
void Location::FlatSun::draw() {
    if (m_eleangtext != nullptr) updateEleAngText();
    if (m_aziangtext != nullptr) updateAziAngText();
    // !!! This duplicates updates if both are enabled. There must be a better way !!!
    if (m_eleangle != maxuint) m_location->m_anglearcs->draw();
    if (m_aziangle != maxuint) m_location->m_anglearcs->draw();
}
void Location::FlatSun::setGeometry(const std::string& geometry) {
    // Pass any single (2 letter) geometry recognized by Earth, or "CG" for current geometry (follows morph)
    m_Geometry = geometry;
}
glm::vec3 Location::FlatSun::getLoc3D_XX(double lat, double lon, float height) {
    if (m_Geometry == "CG") return m_location->m_earth->getLoc3D(lat, lon, height);
    return m_location->m_earth->getLoc3D_XX(m_Geometry, lat, lon, height);
}
void Location::FlatSun::enableArrow3D() { // Add params such as color, length, width (, refraction?)
    if (m_arrow != maxuint) return; // Already enabled
    update(true, true); // Calculate parameters 
    m_arrow = m_location->m_scene->getArrowsFactory()->addStartDirLen(m_location->m_pos, sundir, m_location->m_radius, 0.003f, defaultcolor);
}
void Location::FlatSun::disableArrow3D() {} // Only add if needed
// updateArrow3D(), or just do it directly in update() ?
void Location::FlatSun::changeArrow3D(glm::vec4 color, float length, float width) {
    m_location->m_scene->getArrowsFactory()->changeArrow(m_arrow, color, length, width);
}
void Location::FlatSun::enableDot3D() {
    m_dot = m_location->m_dots->addXYZ(m_location->m_pos + sundir * m_location->m_radius, defaultcolor, locdotsize);
}
void Location::FlatSun::disableDot3D() {} // Only add if needed
void Location::FlatSun::changeDot3D(glm::vec4 color, float size) {
    m_location->m_scene->getDotsFactory()->changeDot(m_dot, color, size);
}
void Location::FlatSun::enableLine3D() {
    m_line = m_location->m_cylinders->addStartEnd(m_location->m_pos, sundir * 100.0f + m_location->m_pos, 0.001f, defaultcolor);
}
void Location::FlatSun::enableAzimuthAngle() {

}
void Location::FlatSun::enableElevationAngle(glm::vec4 color, float width) { // add params for width color
    if (color == NO_COLOR) color = defaultcolor;
    // As per https://www.maplesoft.com/support/help/maple/view.aspx?path=MathApps%2FProjectionOfVectorOntoPlane
    // the actual formula is dividing the dot product by the magnitude of m_zenith squared. However, m_zenith is a unit vector.
    // Also: DO NOT use glm::vec3.length(), it returns the NUMBER of elements, not the magnitude!!! use glm::length(glm::vec3)
    glm::vec3 proj = sundir - m_location->m_zenith * glm::dot(sundir, m_location->m_zenith);
    m_eleangle = m_location->m_anglearcs->add(m_location->m_pos, proj, sundir, m_location->m_radius, color, width);
}
void Location::FlatSun::enableEleAngText(Font* font) {
    if (m_eleangtext != nullptr) {
        updateEleAngText();
        return;
    }
    glm::vec3 pos = m_location->m_pos + sundir * m_location->m_radius;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = m_location->m_scene->w_camera->position - pos;
    glm::vec3 dir = glm::cross(up, camdir);
    up = glm::cross(camdir, dir);
    char dstring[10];
    snprintf(dstring, sizeof(dstring), "%03.3f°", rad2deg * localsun.lat);
    std::string angle = dstring;
    m_eleangtext = m_location->m_scene->getTextFactory()->newText(font, angle, 0.08f, defaultcolor, pos, dir, up);
}
void Location::FlatSun::updateEleAngText() {
    glm::vec3 pos = m_location->m_pos + sundir * m_location->m_radius;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = m_location->m_scene->w_camera->position - pos;
    glm::vec3 dir = glm::cross(up, camdir);
    up = glm::cross(camdir, dir);
    m_eleangtext->updatePosDirUp(pos, dir, up);
    char dstring[10];
    snprintf(dstring, sizeof(dstring), "%03.3f°", rad2deg * localsun.lat);
    std::string angle = dstring;
    m_eleangtext->updateText(angle);
}
void Location::FlatSun::enableAziAngText(Font* font) {
    if (m_aziangtext != nullptr) {
        updateAziAngText();
        return;
    }
    glm::vec3 pos = m_location->m_pos + sundir * m_location->m_radius;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = m_location->m_scene->w_camera->position - pos;
    glm::vec3 dir = glm::cross(up, camdir);
    up = glm::cross(camdir, dir);
    char dstring[10];
    snprintf(dstring, sizeof(dstring), "%03.3f°", ACoord::rangezero2threesixty(rad2deg * localsun.lon - 180.0));
    std::string angle = dstring;
    m_aziangtext = m_location->m_scene->getTextFactory()->newText(font, angle, 0.08f, defaultcolor, pos, dir, up);
}
void Location::FlatSun::updateAziAngText() {
    glm::vec3 pos = m_location->m_pos + sundir * m_location->m_radius;
    glm::vec3 up = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = m_location->m_scene->w_camera->position - pos;
    glm::vec3 dir = glm::cross(up, camdir);
    up = glm::cross(camdir, dir);
    m_aziangtext->updatePosDirUp(pos, dir, up);
    char dstring[10];
    snprintf(dstring, sizeof(dstring), "%03.3f°", ACoord::rangezero2threesixty(rad2deg * localsun.lon - 180.0));
    std::string angle = dstring;
    m_aziangtext->updateText(angle);
}void Location::FlatSun::enablePath24() {
    float width = 0.002f;
    m_path24 = m_location->m_scene->newPolyCurve(defaultcolor, width);
    doPath24();
    m_location->m_polycache.store({ m_path24, nullptr, width, FLATSUN3D, defaultcolor, 0.0, 0.0, 0 });
}
// No need for an update function, that is handled in Location
void Location::FlatSun::disablePath24() {
    m_location->deletePlanetaryPath(TRUESUN3D, SUN);  // FIX !!!
}
void Location::FlatSun::enableAnalemma() {
    m_location->addPlanetaryPath(A_SUN, -183.0, 183.0, 366, TRUEANALEMMA3D, defaultcolor, 0.002f);
}
void Location::FlatSun::disableAnalemma() {
    m_location->deletePlanetaryPath(TRUEANALEMMA3D, SUN);
}
double Location::FlatSun::getElevation(bool radians) {
    if (radians) return localsun.lat;
    return localsun.lat * rad2deg;
}
double Location::FlatSun::getAzimuth(bool radians) {
    if (radians) return localsun.lon;
    return localsun.lon * rad2deg;
}
void Location::FlatSun::doPath24() {
    // Change to smarter date/time based start and end
    // For now, just sweep 1 JD centered on current time
    //float height = 0.0/earthradius; // Observer height in km to Earth radii 
    m_path24->clearPoints();
    double myJD = m_location->m_scene->astro->getJD_TT() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        //m_path24->addPoint(m_location->m_pos + m_location->getFlatSunDir(fday) * m_location->m_radius);
        sun = m_location->m_scene->astro->getDecGHA(A_SUN, fday); // Current Sun GHA, Dec, no JD specified
        while (sun.lon < -pi) sun.lon += tau;
        while (sun.lon > pi) sun.lon -= tau;
        if (sun.lat > pi2 || sun.lat < -pi2) std::cout << "WARNING: Location::FlatSun::update(): sun.lat is out of range: " << sun.lat << "\n";
        if (sun.lon > pi || sun.lon < -pi) std::cout << "WARNING: Location::FlatSun::update(): sun.lon is out of range: " << sun.lon << "\n";
        glm::vec3 sun_xyz = getLoc3D_XX(sun.lat, sun.lon < pi ? -sun.lon : tau - sun.lon, m_location->m_earth->flatsunheight / (float)earthradius);
        m_path24->addPoint(m_location->m_pos + glm::normalize(sun_xyz - m_location->m_pos) * m_location->m_radius);
    }
    m_path24->generate();
}

Location::EleAziGrid::EleAziGrid(Location* location, double stepsize, float width, glm::vec4 color) : m_location(location) {
    m_stepsize = stepsize; // always given in radians
    for (double angle = 0.0; angle < tau; angle += m_stepsize) {
        addAzimuthCircle(angle, width, color);
    }
    for (double angle = -pi2; angle < pi2; angle += m_stepsize) {
        addElevationCircle(angle, width, color);
    }
}
void Location::EleAziGrid::update(bool time, bool morph) {
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
void Location::EleAziGrid::addAzimuthCircle(double azi, float width, glm::vec4 color) {
    // Actually a half-circle
    azicircles.emplace_back(new GenericPath(m_location->m_scene, width, color));
    for (double ele = -pi2; ele <= pi2 + tiny; ele += deg2rad) {
        glm::vec3 point = m_location->getPosition() + m_location->m_radius * m_location->calcEleAzi2Dir({ ele,azi,0.0 });
        azicircles.back()->addPoint(point);
    }
    azicircles.back()->generate();
}
void Location::EleAziGrid::addElevationCircle(double ele, float width, glm::vec4 color) {
    elecircles.emplace_back(new GenericPath(m_location->m_scene, width, color));
    for (double azi = 0.0; azi <= tau; azi += deg2rad) {
        glm::vec3 point = m_location->getPosition() + m_location->m_radius * m_location->calcEleAzi2Dir({ ele,azi,0.0 });
        elecircles.back()->addPoint(point);
    }
    elecircles.back()->generate();
}





// Generics
void Location::addLocSky(float size, glm::vec4 color) {
    // !!! FIX: Should use SkySphere object instead !!!
    size = m_radius;
    size_t index = m_dots->addXYZ(m_pos, color, size);
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
void Location::addHorizon(glm::vec4 color, float width) {
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    doPathHorizon(path);
    m_polycache.store({ path, nullptr, width, HORIZON, color, 0.0, 0.0, 0 });
}
void Location::updatePathHorizon(polycache& pc) {
    pc.path->clearPoints();
    doPathHorizon(pc.path);
    return;
}
void Location::doPathHorizon(PolyCurve* path) {
    // !!! FIX: Should calculate visible horizon angle based on height for curved geometries !!!
    // !!! TODO: Might want to distinguish between geometric horizon and refracted horizon, but the effect is small !!!
    glm::vec3 dir;
    for (double azi = 0; azi <= tau; azi += deg2rad) {
        dir = calcEleAzi2Dir({ 0.0, azi, 0.0 }, true);
        path->addPoint(dir * m_radius + m_pos);
    }
    path->generate();
}

void Location::addCircumPolar(glm::vec4 color, float width) {
    //addPath3DRADec(9999, 0.0, 90.0 - rad2deg * m_lat, GREEN, 0.005f);
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    doPathCircumPolar(path);
    m_polycache.store({ path, nullptr, width, CIRCUMPOLAR, color, 0.0, 0.0, 0 });
}
void Location::updatePathCircumPolar(polycache& pc) {
    pc.path->clearPoints();
    doPathCircumPolar(pc.path);
    return;
}
void Location::doPathCircumPolar(PolyCurve* path) {
    glm::vec3 dir;
    for (double ra = 0; ra <= tau; ra += deg2rad) {
        dir = calcDecRA2Dir(NO_DOUBLE, (m_lat < 0.0) ? -pi2 - m_lat : pi2 - m_lat, ra, true);
        path->addPoint(dir * m_radius + m_pos);
    }
    path->generate();
}

void Location::addTangentPlane(glm::vec4 color, float alpha) {
    if (alpha == NO_FLOAT) alpha = 0.4f;
    color.a = alpha;
    const float radiusfactor = 2.5f;
    size_t index = m_planes->addStartUV(
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
    size_t index = m_planes->addStartUV(
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
    size_t index = m_arrows->addStartDirLen(m_pos, m_zenith, length, width, upcolor);
    m_arrowcache.push_back({ m_pos,m_zenith,upcolor,length,width,0.0f,0.0f,index,ZENITH, maxuint });
}
// ADD: change, delete
void Location::updateUpCoord(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, m_zenith, ar.length, ar.width, ar.color);
}
void Location::addEastCoord(float length, float width) {
    glm::vec4 eastcolor = glm::vec4(1.0f, 0.1f, 0.1f, 1.0f);
    size_t index = m_arrows->addStartDirLen(m_pos, m_east, length, width, eastcolor);
    m_arrowcache.push_back({ m_pos,m_east,eastcolor,length,width,0.0f,0.0f,index,EAST, maxuint });
}
void Location::updateEastCoord(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, m_east, ar.length, ar.width, ar.color);
}
void Location::addNorthCoord(float length, float width) {
    glm::vec4 northcolor = glm::vec4(0.1f, 1.0f, 0.1f, 1.0f);
    size_t index = m_arrows->addStartDirLen(m_pos, m_north, length, width, northcolor);
    m_arrowcache.push_back({ m_pos,m_north,northcolor,length,width,0.0f,0.0f,index,NORTH, maxuint });
}
void Location::updateNorthCoord(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, m_north, ar.length, ar.width, ar.color);
}
void Location::addNormal(float length, float width) {
    glm::vec4 normalcolor = LIGHT_BLUE; // glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec3 dir = m_earth->getNml3D(m_lat, m_lon);
    size_t index = m_arrows->addStartDirLen(m_pos, dir, length, width, normalcolor);
    m_arrowcache.push_back({ m_pos,dir,normalcolor,length,width,0.0f,0.0f,index,NORMAL, maxuint });
}
void Location::updateNormalCoord(arrowcache& ar) {
    m_arrows->changeStartDirLen(ar.index, m_pos, m_earth->getNml3D(m_lat, m_lon), ar.length, ar.width, ar.color);
}
void Location::addEleAziGrid(double degrees, bool radians, float width, glm::vec4 color) {
    radians ? azielegrid = new Location::EleAziGrid(this, degrees, width, color)           // true
            : azielegrid = new Location::EleAziGrid(this, degrees*deg2rad, width, color);  // false
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
    glm::vec3 dir = calcEleAzi2Dir({ ele, azi, 0.0 }, false);
    size_t index = m_arrows->addStartDirLen(m_pos, dir, length, width, color);
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
            glm::vec3 dir = calcEleAzi2Dir({ ele, azi, 0.0 }, false);
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
    glm::vec3 dir = calcEleAzi2Dir({ ar.elevation, ar.azimuth, 0.0 }, false);
    m_arrows->changeStartDirLen(ar.index, m_pos, dir, ar.length, ar.width, ar.color);
}
void Location::addArrow3DDecRA(unsigned int unique, double dec, double ra, glm::vec4 color, float width, float length) {
    // Takes RA & Dec in degrees
    // m_lat & m_lon are in rads. Consider using getLat(true) and getLon(true) instead
    double gsidtime = m_scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true); // In radians
    double gha = gsidtime - (deg2rad * ra); // calcGeo2Topo() needs GHA, not LHA
    LLD topocentric = m_scene->astro->calcGeo2Topo({ deg2rad * dec, gha, 0.0 }, { m_lat, m_lon, 0.0 });
    //std::cout << "Elevation, Azimuth: " << rad2deg * topocentric.lat << ", " << rad2deg * topocentric.lon << "\n";
    //std::cout << "Elevation with refraction: " << rad2deg * topocentric.lat - m_earth->calcRefractionBennett(rad2deg * topocentric.lat, 10.0, 1010.0) << '\n';
    glm::vec3 dir = calcEleAzi2Dir(topocentric, true);
    size_t index = m_arrows->addStartDirLen(m_pos, dir, length, width, color);
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
void Location::updateArrow3DDecRA(arrowcache& ar) {
    //NOTE: Stellarium gives Azimuth in clockwise from North, topocentric is clockwise from South
    double gsidtime = m_scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true); // In radians
    double gha = gsidtime - (deg2rad * ar.azimuth);
    LLD topocentric = m_scene->astro->calcGeo2Topo({ deg2rad * ar.elevation, gha, 0.0 }, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcEleAzi2Dir(topocentric, true);
    m_arrows->changeStartDirLen(ar.index, m_pos, dir, ar.length, ar.width, ar.color);
}

//-------------------------------------- Dots --------------------------------------//
void Location::addLocDot(float size, glm::vec4 color) {
    size_t index = m_dots->addXYZ(m_pos, color, size);
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
void Location::addTruePlanetDot(Planet planet, float size, glm::vec4 color, bool checkit) {
    if (checkit) { // Caller can specify to skip if this type of arrow is already present
        for (auto& dc : m_dotcache) {
            if (dc.unique == planet) return;
        }
    }
    color = getPlanetColor(planet, color);
    m_scene->astro->enablePlanet(planet);
    LLD pos = m_scene->astro->getDecGHA(planet, NO_DOUBLE);
    LLD topo = m_scene->astro->calcGeo2Topo(pos, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcEleAzi2Dir(topo, true);
    size_t index = m_dots->addXYZ(m_pos + dir * m_radius, color, size);
    m_dotcache.push_back({ m_pos + dir * m_radius, size, color, index, TRUEPLANET3D, (size_t)planet });
}
void Location::updateTruePlanetDot(dotcache& dc) {
    LLD pos = m_scene->astro->getDecGHA((Planet)dc.unique, NO_DOUBLE);
    LLD topo = m_scene->astro->calcGeo2Topo(pos, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcEleAzi2Dir(topo, true);
    m_dots->changeXYZ(dc.index, m_pos + dir * m_radius, dc.color, dc.size);
}
// ADD: change, delete

//-------------------------------------- Arrows --------------------------------------//
void Location::addArrow3DFlatSun(float length, float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present
    if (checkit) for (auto& ar : m_arrowcache) { if (ar.type == FLATSUN3D) return; }
    size_t index = m_arrows->addStartDirLen(m_pos, getFlatSunDir(), length, width, color);
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
    LLD moon = m_earth->getMoon();    // sun { rad sunDec, rad sunHour, AU sunDist }
    LLD localmoon = Spherical::Equatorial2Horizontal((-moon.lon + m_lon), moon.lat, m_lat);
    glm::vec3 dir = calcEleAzi2Dir(localmoon, true);
    size_t index = m_arrows->addStartDirLen(m_pos, dir, length, width, color);
    m_arrowcache.push_back({ m_pos,dir,color,length,width,localmoon.lat,localmoon.lon,index,TRUEMOON3D, maxuint });
}
void Location::updateArrow3DTrueMoon(arrowcache& ar) {
    LLD moon = m_earth->getMoon();    // sun { rad sunDec, rad sunHour, AU sunDist }
    LLD localmoon = Spherical::Equatorial2Horizontal((-moon.lon + m_lon), moon.lat, m_lat);
    glm::vec3 dir = calcEleAzi2Dir(localmoon, true);
    m_arrows->changeStartDirLen(ar.index, m_pos, dir, ar.length, 0.003f, ar.color);
}

void Location::updateArrow3DTruePlanet(arrowcache& ar) {
    LLD pos = m_scene->astro->getDecGHA((Planet)ar.unique, NO_DOUBLE);
    LLD topo = m_scene->astro->calcGeo2Topo(pos, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcEleAzi2Dir(topo, true);
    m_arrows->changeStartDirLen(ar.index, m_pos, dir, ar.length, 0.003f, ar.color);
}
// ADD: change, delete
void Location::addArrow3DTruePlanet(Planet planet, float length, glm::vec4 color, bool checkit) {
    if (checkit) { // Caller can specify to skip if this type of arrow is already present
        for (auto& ar : m_arrowcache) {
            if (ar.unique == planet) return;
        }
    }
    color = getPlanetColor(planet, color);
    m_scene->astro->enablePlanet(planet);
    LLD pos = m_scene->astro->getDecGHA(planet, NO_DOUBLE);
    LLD topo = m_scene->astro->calcGeo2Topo(pos, { m_lat, m_lon, 0.0 });
    glm::vec3 dir = calcEleAzi2Dir(topo, true);
    size_t index = m_arrows->addStartDirLen(m_pos, dir, length, 0.003f, color);
    m_arrowcache.push_back({ m_pos, dir, color, length, 0.003f, topo.lat, topo.lon, index, TRUEPLANET3D, (size_t)planet });
}
//-------------------------------------- Lines --------------------------------------//
void Location::addLine3DFlatSun(float width, glm::vec4 color, bool checkit) {
    // Caller can specify to skip if this type of arrow is already present
    if (checkit) for (auto& l : m_cylindercache) { if (l.type == FLATSUN3D) return; }
    glm::vec3 flatsun = m_earth->getSubsolarXYZ();
    size_t index = m_cylinders->addStartEnd(m_pos, flatsun, width, color);
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
void Location::addPlanetaryPath(Planet planet, double startoffset, double endoffset, unsigned int steps, unsigned int type, glm::vec4 color, float width) {
    CelestialPath* planetCP = m_scene->astro->getCelestialPath(planet, startoffset, endoffset, steps, ECGEO);
    color = getPlanetColor(planet, color);
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    size_t index = m_polycache.store({ path, planetCP, width, type, color, 0.0, 0.0, (size_t)planet });
    m_polycache[index].index = index;
    updatePlanetaryPath(m_polycache[index]);
}
// changePlanetaryPath() - What should it change exactly? primitive properties or celestial ones?
// deletePlanetaryPath() - Would need to specify (planet, start, end, steps), or addPlanetaryPath should return an index, or a second unique field is needed.
void Location::updatePlanetaryPath(polycache& pa) {
    pa.path->clearPoints();
    for (auto const& pt : pa.planetCP->entries) {
        LLD topo = m_scene->astro->calcGeo2Topo({ pt.geq.lat, pt.geogha, 0.0 }, { m_lat, m_lon, 0.0 });
        glm::vec3 dir = calcEleAzi2Dir(topo, true);
        pa.path->addPoint(m_pos + dir * m_radius);
    }
    pa.path->generate();
}
void Location::deletePlanetaryPath(unsigned int type, size_t unique) {
    polycache* path = nullptr;
    for (auto& p : m_polycache.m_Elements) {
        if (p.type == type && p.unique == unique) {
            path = &p;
            break;
        }
    }
    if (path != nullptr) {
        delete path->path; // PolyCurve
        m_scene->astro->removeCelestialPath(path->planetCP); // CelestialPath
        size_t index = path->index;
        m_polycache.remove(index); // Cache
    }
    else std::cout << "WARNING: Location::deletePlanetaryPath() called but no path was found with type = " << type << "!\n";
}
// ADD: change, delete
void Location::addPlanetTruePath24(Planet planet, glm::vec4 color, float width) {
    // Planetary path across sky in 24 hrs, roughly follows earth rotation of course
    addPlanetaryPath(planet, -0.5, 0.5, 100, TRUEPLANET3D, color, width);
}
// ADD: change(color, width)
void Location::deletePlanetTruePath24(Planet planet) {
    deletePlanetaryPath(TRUEPLANET3D, planet);
}
void Location::addPlanetTruePathSidYear(Planet planet, glm::vec4 color, float width) {
    // Planetary path across fixed star background, over approximately 1 year
    double bracket = sidyearparms[planet].bracket;
    double stepsize = sidyearparms[planet].stepsize;
    addPlanetaryPath(planet, -bracket * sidereald, bracket * sidereald, 2 * (unsigned int)(bracket / stepsize), SIDPLANET3D, color, width);
}
// ADD: change
void Location::deletePlanetTruePathSidYear(Planet planet) {
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
    double myJD = m_scene->astro->getJD_UTC() + 0.5;
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
    double myJD = m_scene->astro->getJD_UTC() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        LLD moon = m_earth->getMoon(fday);
        LLD localmoon = Spherical::Equatorial2Horizontal((-moon.lon + m_lon), moon.lat, m_lat);
        glm::vec3 dir = calcEleAzi2Dir(localmoon, true);
        path->addPoint(m_pos + dir * m_radius);
    }
    path->generate();
}
void Location::addTrueLunalemma(glm::vec4 color, float width, float size) {
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    doPath3DTrueMoon(path);
    m_polycache.store({ path, nullptr, width, TRUELUNALEMMA3D, color, 0.0, 0.0, 0 });
    m_lunalemmatrail = new ParticleTrail(m_scene, 29, color, size, 0, false);
}
void Location::updateTrueLunalemma(polycache p) {
    p.path->clearPoints();
    doTrueLunalemma(p.path);
    return;
}
void Location::doTrueLunalemma(PolyCurve* path) {
    double myJD = m_scene->astro->getJD_UTC();
    // Rough estimate by dividing sidereal month into 360 and compensating for 4 minutes per degree of celestial rotation: 1.036507 (24 hrs 52.57mins)
    // 1.035028 is from https://www.flickr.com/photos/vanamonde81/49859483963/in/dateposted/ and seems to work perfectly
    glm::vec3 dir = glm::vec3(0.0f);
    for (double fday = myJD; fday < myJD + 29.0; fday += 1.035028 + (m_scene->m_app->lunalemmaOffset/86400.0)) {
        LLD moon = m_earth->getMoon(fday);
        LLD localmoon = Spherical::Equatorial2Horizontal((-moon.lon + m_lon), moon.lat, m_lat);
        dir = calcEleAzi2Dir(localmoon, true);
        path->addPoint(m_pos + dir * m_radius);
        m_lunalemmatrail->push(m_pos + dir * m_radius);
    }
    path->generate();
}

// Stars are likely to be added in multitudes, implement a unique id passed by caller (unsigned int)
void Location::addPath3DDecRA(unsigned int unique, double dec, double ra, glm::vec4 color, float width) {
    PolyCurve* path = m_scene->newPolyCurve(color, width);
    doPath3DDecRA(dec, ra, path);
    m_polycache.store({ path, nullptr, width, RADEC3D, color, dec, ra, unique });
}
void Location::updatePath3DRADec(polycache& pa) {
    pa.path->clearPoints();
    doPath3DDecRA(pa.elevation, pa.azimuth, pa.path);
    return;
}
void Location::doPath3DDecRA(double dec, double ra, PolyCurve* path) {
    // Change to smarter date/time based start and end
    // For now, just sweep 1 JD centered on current epoch
    // !!! FIX: Allow radians via bool rad !!!
    double myJD = m_scene->astro->getJD_UTC() + 0.5;
    for (double fday = myJD - 1.0; fday < myJD; fday += 0.01) {
        glm::vec3 point = m_pos + calcDecRA2Dir(fday, dec, ra, false) * m_radius;
        path->addPoint(point);
    }
    path->generate();
}


// -------------
//  SolarSystem
// -------------
SolarSystem::SolarSystem(Scene* scene, bool geocentric) : m_scene(scene), m_geocentric(geocentric) {
    //std::cout << "SolarSystem::SolarSystem()\n";
    m_astro = m_scene->astro;
    m_jd = m_astro->getJD_TT();
    glm::vec3 earthpos = CalcEarth() + m_sunpos;
    m_sunpos = m_geocentric ? -earthpos : glm::vec3(0.0f);
    m_earthpos = m_geocentric ? glm::vec3(0.0f) : earthpos;

    // Calculate positions and create celestial objects
    SunPos(false);
    EarthPos(false);
    for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
        PlanetPos((Planet)p, false);
        PlanetOrbit((Planet)p);
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
    m_jd = m_astro->getJD_TT();
    SunPos(true);
    EarthPos(true);
    for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
        PlanetPos((Planet)p, true);
        PlanetOrbit((Planet)p);
    }
    //PlanetPos(EARTH, true);
    //if (m_geocentric) PlanetOrbit(SUN);
    //else PlanetOrbit(EARTH);
    UpdateDistLines();
}
void SolarSystem::Draw() {
    if (m_orbits) {
        for (auto& p : m_PlanetPath) {
            if (p != nullptr) p->draw(m_scene->w_camera);
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
    for (size_t p = A_MERCURY; p <= A_SUN; p++) {
        if (m_PlanetTrail[p] == nullptr) m_PlanetTrail[p] = new ParticleTrail(m_scene, m_traillen, m_planetinfos[p].color);
    }
    m_trails = true;
}
void SolarSystem::clearTrails() {
    for (size_t p = A_MERCURY; p <= A_SUN; p++) {
        if (m_PlanetTrail[p] != nullptr) m_PlanetTrail[p]->clear();
    }
}
glm::vec3 SolarSystem::CalcPlanet(Planet planet, double jd_tt) {
    if (jd_tt == 0.0) jd_tt = m_jd;
    const double lon = m_astro->getEcLon(planet, jd_tt);  // Radians
    const double lat = m_astro->getEcLat(planet, jd_tt);  // Radians
    const double dst = m_astro->getEcDst(planet, jd_tt);  // AU
    return Ecliptic2Cartesian(lat, lon, dst);
}
void SolarSystem::PlanetPos(Planet planet, bool update) {
    // NOTE: Optionally push to trail depending on distance from last frame. !!!
    glm::vec3 pos = CalcPlanet(planet) + m_sunpos;
    if (!update) m_PlanetDot[planet] = m_scene->getDotsFactory()->addXYZ(pos, m_planetinfos[planet].color, planetdot);
    if (update) m_scene->getDotsFactory()->changeXYZ(m_PlanetDot[planet], pos, m_planetinfos[planet].color, planetdot);
    if (m_PlanetTrail[planet] != nullptr && m_trails) m_PlanetTrail[planet]->push(pos);
}
void SolarSystem::PlanetOrbit(Planet planet) {
    if (m_PlanetPath[planet] == nullptr) {
        double siderealyear = m_planetinfos[planet].sidyear;
        glm::vec4 color = m_planetinfos[planet].color;
        color.a = 0.4f;
        m_PlanetPath[planet] = new PolyCurve(m_scene, color, solsyspathwidth);
        //m_PlanetPath[planet] = m_scene->newPolyCurve(color, solsyspathwidth); // Scene draws these automatically!!
        m_PlanetOrbit[planet] = m_scene->astro->getCelestialPath(planet, -0.5 * siderealyear, 0.5 * siderealyear, orbitsteps, EC);
    }
    m_PlanetPath[planet]->clearPoints(); // Even if it was just created, clearing an empty vector should be cheap
    for (auto const& pt : m_PlanetOrbit[planet]->entries) {
        if (planet == A_SUN) m_PlanetPath[planet]->addPoint(Ecliptic2Cartesian(pt.hec.lat, pt.hec.lon, pt.hec.dst)); // Heliocentric
        else  m_PlanetPath[planet]->addPoint(Ecliptic2Cartesian(pt.hec.lat, pt.hec.lon, pt.hec.dst) + m_sunpos);
    }
    m_PlanetPath[planet]->generate();
 }
void SolarSystem::SunPos(bool update) {
    m_sunpos = m_geocentric ? CalcSun() : glm::vec3(0.0f);
    if (!update) m_SunDot = m_scene->getDotsFactory()->addXYZ(m_sunpos, SUNCOLOR, planetdot);
    if (update) m_scene->getDotsFactory()->changeXYZ(m_SunDot, m_sunpos, SUNCOLOR, planetdot);
    if (m_PlanetTrail[A_SUN] != nullptr && m_trails) m_PlanetTrail[A_SUN]->push(m_sunpos);
}
void SolarSystem::SunOrbit(bool update) {
    m_SunPath = new PolyCurve(m_scene, SUNCOLOR, solsyspathwidth);
    double siderealyear = 366; // Sun days
    for (double jd = m_astro->getJD_TT() - 0.5 * siderealyear; jd < m_astro->getJD_TT() + 0.5 * siderealyear; jd += siderealyear / 360) {
        glm::vec3 pos = CalcSun(jd);
        m_SunPath->addPoint(pos);
    }
    m_SunPath->generate();
    m_SunPath->draw(m_scene->w_camera); // Default cam, NOT always correct !!!
}
glm::vec3 SolarSystem::CalcSun(double jd_tt) {
    if (jd_tt == 0.0) jd_tt = m_jd;
    const double lon = m_astro->getEcLon(A_EARTH, jd_tt);
    const double lat = m_astro->getEcLat(A_EARTH, jd_tt);
    const double dst = au2km * m_astro->getEcDst(A_EARTH, jd_tt);
    return -Ecliptic2Cartesian(lat,lon,dst);
}
void SolarSystem::EarthPos(bool update) {
    m_earthpos = m_geocentric ? glm::vec3(0.0f) : CalcEarth() + m_sunpos;
    if (!update) m_EarthDot = m_scene->getDotsFactory()->addXYZ(m_earthpos, EARTHCOLOR, planetdot);
    if (update) m_scene->getDotsFactory()->changeXYZ(m_EarthDot, m_earthpos, EARTHCOLOR, planetdot);
    //pos += m_sunpos;
    if (m_EarthTrail != nullptr && m_trails) m_EarthTrail->push(m_earthpos);
}
glm::vec3 SolarSystem::CalcEarth(double jd_tt) {
    if (jd_tt == 0.0) jd_tt = m_jd;
    const double lon = m_astro->getEcLon(A_EARTH, jd_tt);
    const double lat = m_astro->getEcLat(A_EARTH, jd_tt);
    const double dst = au2km * m_astro->getEcDst(A_EARTH, jd_tt);
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
glm::vec3 SolarSystem::GetPlanetPos(Planet planet) {  // Optional JD ?
    if (planet == A_SUN) return m_sunpos;
    else if (planet == A_EARTH) return m_earthpos;
    else if (planet > A_SUN && planet < A_EARTH) return CalcPlanet(planet);
    else std::cout << "SolarSystem::GetPlanetPos() called with invalid planet enum: " << planet << ", returning glm::vec3(0.0f)!\n";
    return glm::vec3(0.0f);
}
void SolarSystem::AddDistLine(Planet planet1, Planet planet2, glm::vec4 color, float width) {
    if (m_cylinders == nullptr) m_cylinders = m_scene->getCylindersFactory();
    size_t index = m_cylinders->addStartEnd(GetPlanetPos(planet1), GetPlanetPos(planet2), width, color);
    m_distlines.store({ index, planet1, planet2, color, width });
}
void SolarSystem::UpdateDistLines() {
    for (auto& dl : m_distlines.m_Elements) {
        glm::vec3 p1 = GetPlanetPos(dl.planet1);
        glm::vec3 p2 = GetPlanetPos(dl.planet2);
        if (dl.planet1 != A_SUN) p1 += m_sunpos;
        if (dl.planet2 != A_SUN) p2 += m_sunpos;
        m_cylinders->changeStartEnd(dl.index, p1, p2, dl.width, dl.color);
    }
}
void SolarSystem::changeCentricity() {
    m_geocentric = geocentric;
}
void SolarSystem::changeOrbits() {
    if (orbits && !m_orbits) { // Orbits being enabled
        for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
            PlanetOrbit((Planet)p); // Create or update
        }
        m_orbits = true;
    }
    if (!orbits && m_orbits) { // Orbits being disabled
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
        for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
            if (m_PlanetTrail[p] != nullptr) m_PlanetTrail[p]->trim(traillen);
        }
    }
    else {
        for (unsigned int p = A_MERCURY; p <= A_SUN; p++) {
            if (m_PlanetTrail[p] != nullptr) m_PlanetTrail[p]->expand(traillen);
        }
    }
    m_traillen = traillen;
}


// ----------
//  Flat Sky
// ----------
// This could be the sky above Flat Earth. Perhaps model as hemisphere?
// As per Bert Rickles, there are several ways to map the celestial objects, he uses the first two:
// 1) Place them above their GP as dome height or simply a fixed height above surface
// 2) Place them at azimuth/elevation and height above surface, as seen from a selected location.
// 3) Ball to Pancake unwrapping, same as for Earth? I guess this would just be 1) above.


// -----------
//  SkySphere
// -----------
// NOTE: To prevent drawing of the far side stars and grid lines, one could make a transparent sphere
//  that will set the depth buffer. Due to Primitive aplha = 0.0 filtering in the shader, set alpha to
//  slightly above zero. Also, this would only work for spherical skies, not planar ones,
//  but that might not matter. Currently, when Location sky and SkySphere have same radius and coincide,
//  the LocationSky can be used to accomplish this effect.
SkySphere::SkySphere(Scene* scene, unsigned int meshU, unsigned int meshV, bool texture)
    : m_scene(scene), m_meshU(meshU), m_meshV(meshV), m_texture(texture) {
    // Might this adapt to also become a Location Sky?
    // Well by default it is centered on origin and has radius of 1000 units.
    // If I simply scale stardots distances and radii by Location::m_radius/1000.0f and then offset by Location::m_pos it should work?
    // For rendering, it is better to disable backface culling and leave glFrontFace() alone

    // Note: Make radius configurable, and allow to resize.
    //  Requires rendering using the default Camera transform rather than the Sky one.
    m_verts.reserve(((size_t)m_meshU + 1) * ((size_t)m_meshV + 1));
    m_tris.reserve((size_t)(m_meshU * (size_t)m_meshV * (size_t)sizeof(Tri)));
    m_dots = m_scene->getSkyDotsFactory();
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
    shdr = m_scene->m_app->getShaderLib()->getShader(SKY_SPHERE_SHADER);
    // NOTE: Textures should be pre-flipped in Y, as SOIL2 soils itself when trying to flip large images
    // NOTE: Sky texture should be flipped in X, as it is viewed from the inside of the sphere !!!
    const std::string texfile = "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap-gimp_8k (8192x4096 celestial coords).png";
    tex = new Texture(texfile, GL_TEXTURE2);
    //m_world->SetSkySphereOb(this);
    //loadStars();
    UpdateTime(m_scene->astro->getJD_UTC());
}
SkySphere::~SkySphere() {
    delete tex;
    delete ib;
    delete vb;
    delete va;
    delete vbl;
}
void SkySphere::setTexture(bool texture) { 
    // Note: This does not set which texture to use. It is a boolean determining whether to render the texture or not.
    m_texture = texture;
}
void SkySphere::setMode(const std::string& mode, Earth* earth, Location* location) {
    // add validations
    if (earth) m_earth = earth;
    if (location) m_location = location;
    if (mode == "GP") { this->mode = FLATSKY_GP; return; }
    if (mode == "PE") { this->mode = FLATSKY_LC_PLANE; return; }
    if (mode == "SE") { this->mode = FLATSKY_LC_DOME; return; }
    if (mode == "SP") { this->mode = FLATSKY_SP; return; }
    if (mode == "HD") { this->mode = FLATSKY_HD; return; }
    std::cout << "SkySphere::setMode() was called with unknown mode: " << mode << ", using default GP!\n";
    this->mode = FLATSKY_GP;
    return;
}
void SkySphere::setMovable(bool movable) {
    m_movable = movable;
}
void SkySphere::setRadius(float radius) { m_Radius = radius; }
void SkySphere::draw() {
    glm::mat4 proj = glm::mat4(1.0f);
    if (m_movable) { proj = m_scene->w_camera->getProjMat(); }
    else { proj = glm::perspective(glm::radians(50.0f), m_scene->m_app->getAspect(), 1.0f, 1000.0f); }
    // Shader setup
    //UpdateTime(0.0);
    if (m_texture) {
        shdr->Bind();
        if (m_movable) shdr->SetUniformMatrix4f("view", m_scene->w_camera->getViewMat());
        else shdr->SetUniformMatrix4f("view", m_scene->w_camera->getSkyViewMat());
        shdr->SetUniformMatrix4f("projection", proj);
        shdr->SetUniform1i("skytexture", tex->GetTextureSlot());
        shdr->SetUniform1f("gsid", -m_gsid);
        va->Bind();
        ib->Bind();
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glFrontFace(GL_CCW);

        //glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE); // It is irritating that Earth disappears when seen from back-/in-side
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glEnable(GL_CULL_FACE);
        //glEnable(GL_DEPTH_TEST);

        glFrontFace(GL_CW);
        // Cleanup
        va->Unbind();
        ib->Unbind();
        shdr->Unbind();
    }
    return;
}
void SkySphere::UpdateTime(double jd_utc) {
    // HACK to keep steady background when displaying SolarSystem, check that it is orientated correctly !!!
    // Problem with this hack is, it will now create a SolarSystem and move on.
    //if (m_scene->getSolsysOb() != nullptr) return;
    m_gsid = (float)(m_scene->astro->ApparentGreenwichSiderealTime(NO_DOUBLE, true));
    // Update Dot cache with rotation
    //std::cout << "SkySphere::UpdateTime() : m_gsid = " << m_gsid << ", m_astro = " << m_scene->m_astro << '\n';
    for (auto& d : m_dotcache) {
        updateDotDecRA(d);
    }
}
//void SkySphere::addPlanetDot() {
//
//}
void SkySphere::addStars(double magnitude) {
    unsigned int i = 0;
    for (auto& s : Astronomy::stellarobjects) {
        if (s.vmag < magnitude) {
            //std::cout << "SkySphere::addStars(): size = " << size << " at Vmag = " << s.vmag << "\n";
            addDotDecRA(++i, s.dec, s.ra, glm::vec4((float)s.red, (float)s.green, (float)s.blue, 1.0f), getMagnitude2Radius(s.vmag), /*radians*/ false);
        }
    }
    std::cout << "Stars added: " << i << " of magnitude less than " << magnitude << '\n';
    curr_unique = i;
}
unsigned int SkySphere::addStarByName(const std::string name) {
    //LLD DecRA = Astronomy::getDecRAbyName(name, true);
    //glm::vec4 color = Astronomy::getColorbyName(name);
    Astronomy::stellarobject so = Astronomy::getSObyName(name);
    addDotDecRA(++curr_unique, so.dec, so.ra, glm::vec4((float)so.red, (float)so.green, (float)so.blue, 1.0f), getMagnitude2Radius(so.vmag), false);
    return curr_unique;
}
void SkySphere::addDotDecRA(unsigned int unique, double dec, double ra, glm::vec4 color, float size, bool radians) {
    // default is to deliver position in radians, else in degrees (thus, not hours for RA)
    //float dotsize = (float)size;
    //glm::vec4 dotcolor = glm::vec4((float)red, (float)green, (float)blue, 1.0f);
    //std::cout << "Star color: " << dotcolor.r << ", " << dotcolor.g << ", " << dotcolor.b << '\n';
    if (!radians) {
        dec *= deg2rad;
        ra *= deg2rad;
    }
    size_t index = m_dots->addXYZ(getDecRA2Pos3D(dec, ra), color, size);
    m_dotcache.push_back({ unique, color, dec, ra, 0.0f, size, index });
    //std::cout << "addDotDecRA(): " << dec << "," << ra << "\n";

    //updateDotDecRA(m_dotcache.back());
    return; // (unsigned int)m_dotcache.size() - 1;
}
void SkySphere::updateDotDecRA(dotcache& dot) {
    // FIX: !!! This only calculates the Precession !!!
    //      Consider which RA/Dec value to store and how much calculation we want for thouands of stars per frame
    //      RA/Dec only needs recalculation when the time changes, not when the observer location changes.
    //      So half of this function can be skipped if time is frozen.
    //      Might want to update when time since last update is more than 1 year.
    LLD mydecra = m_scene->astro->PrecessDecRA({ dot.lat,dot.lon }); // , m_scene->astro->getJD_UTC());
    glm::vec3 pos = getDecRA2Pos3D(mydecra.lat, mydecra.lon);
    m_dots->changeXYZ(dot.index, pos, dot.color, dot.size);
    //std::cout << "updateDotDecRA(): " << dot.lat << "," << dot.lon << "\n";
    //if (dot.index == 1) VPRINT(pos);
    return;
}
void SkySphere::addGrid(float width) {
    double gridstep = gridspacing;
    m_gridwidth = width;
    for (double dec = -90.0; dec <= 90.0; dec += gridstep) {
         pv.push_back(m_scene->newPolyCurve(LIGHT_GREY, m_gridwidth, 365));
        for (double ra = -180.0; ra <= 180.0; ra += 1.0) {
            pv.back()->addPoint(getDecRA2Pos3D(dec * deg2rad, ra * deg2rad));
        }
        pv.back()->generate();
    }
    for (double ra = -180.0; ra <= 180.0; ra += gridstep) {
        pv.push_back(m_scene->newPolyCurve(LIGHT_GREY, m_gridwidth, 365));
        for (double dec = -90.0; dec <= 90.0; dec += 1.0) {
            pv.back()->addPoint(getDecRA2Pos3D(dec * deg2rad, ra * deg2rad));
        }
        pv.back()->generate();
    }
    has_grid = true;
}
void SkySphere::updateGrid() {
    if (!has_grid) return;
    float width = m_Radius / 300.0f;   // In some modes it looks better to scale with radius
    if (mode == FLATSKY_LC_DOME) width = m_gridwidth;
    double gridstep = 15.0;
    for (auto& p : pv) {
        m_scene->deletePolyCurve(p);
    }
    pv.clear();
    for (double dec = -90.0; dec <= 90.0; dec += gridstep) {
        pv.push_back(m_scene->newPolyCurve(GREY, width, 365));
        for (double ra = -180.0; ra <= 180.0; ra += 1.0) {
            pv.back()->addPoint(getDecRA2Pos3D(dec * deg2rad, ra * deg2rad));
        }
        pv.back()->generate();
    }
    for (double ra = -180.0; ra <= 180.0; ra += gridstep) {
        pv.push_back(m_scene->newPolyCurve(GREY, width, 365));
        for (double dec = -90.0; dec <= 90.0; dec += 1.0) {
            pv.back()->addPoint(getDecRA2Pos3D(dec * deg2rad, ra * deg2rad));
        }
        pv.back()->generate();
    }
}
glm::vec3 SkySphere::getDecRA2Pos3D(double dec, double ra) {
    // Caution: May be called before a mode is set, so m_earth and m_location might be nullptr

    // Some of these are non-local, others are location dependent. Might move location dependent to Location? !!!

    if (mode == FLATSKY_HD) { // Half Dome across whole AE Earth or standalone
        return getLoc3D_NS((pi2 + dec) / 2.0, ra - m_gsid);
    }
    if (mode == FLATSKY_SP) { // Sphere around whole NS Earth or standalone
        return getLoc3D_NS(dec, ra - m_gsid);  // Obviously location Azi Ele arrows will not point to the stars here.
                                               // FIXME: However, they are not even pointing in the right general direction !!!
    }
    if (mode == FLATSKY_GP && m_earth) { // Maps all stars to GPs on Earth surface
        return m_earth->getLoc3D(dec, ra - m_gsid, 0.0f); // m_Radius);
        //m_earth->calcHADec2Dir({ dec, m_gsid - ra, 0.0f });
    }
    if (mode == FLATSKY_LC_PLANE && m_location) { // PE = Azi Ele mode Plane Sky ("dome")
        LLD topocentric = m_scene->astro->calcGeo2Topo({ dec, m_gsid - ra, 0.0 }, { m_location->getLat(), m_location->getLon(), 0.0 });
        // Now calculate where this Azi Ele intersects the sky dome
        glm::vec3 dir = m_location->calcEleAzi2Dir(topocentric, true);
        
        float dz = glm::dot(dir, m_location->getZenith()); 
        if (topocentric.lat >= 0.0f) { dir *= ((domeheight / (float)earthradius) / dz); }
        else { dir *= (-(domeheight / (float)earthradius) / dz); }
        return m_location->getPosition() + dir;
        //pos.z = 0.0f;
    }
    if (mode == FLATSKY_LC_DOME && m_location) { // SE = Azi Ele mode Spherical Sky (dome)
        // Sphere Line intersection from: https://www.youtube.com/watch?v=HFPlKQGChpE&ab_channel=TheArtofCode
        glm::vec3 ro = m_location->getPosition(); // Ray Origin
        LLD topocentric = m_scene->astro->calcGeo2Topo({ dec, m_gsid - ra, 0.0 }, { m_location->getLat(), m_location->getLon(), 0.0 });
        // Now calculate where this Azi Ele intersects the sky dome
        glm::vec3 dir = m_location->calcEleAzi2Dir(topocentric, true);
        glm::vec3 s = glm::vec3(0.0f);                 // Sphere origin (centered on north pole)
        if (m_movable) s = m_location->getPosition();  // Sphere origin (centered on location)
        //float r = 2.0f;                              // Sphere radius
        float t = glm::dot(s - ro, dir);
        glm::vec3 p = ro + dir * t;
        float y = glm::length(s - p);
        if (y > m_Radius) {
            std::cout << "ERROR: SkySphere::getDecRA2Pos3D(): No intersection in SE mode! Y: " << y << " Radius: " << m_Radius << "\n";
        }
        //float x = sqrt(r * r - y * y);
        //float t1 = t - x;
        //float t2 = t + x;
        return ro + dir * (t + sqrt(m_Radius * m_Radius - y * y));
    }
    //std::cout << "NO!!!";
    return getLoc3D_NS(dec, ra); // Unknown mode, default to sphere centered on origin.
}
glm::vec3 SkySphere::getLoc3D_NS(double lat, double lon) {
    double w = cos(lat) * m_Radius;
    float x = (float)(cos(lon) * w);
    float y = (float)(sin(lon) * w);
    float z = (float)(sin(lat) * m_Radius);
    return glm::vec3(x, y, z);
}
void SkySphere::addLineStarStar(const std::string star1, const std::string star2, glm::vec4 color, float width) {
    LLD s1 = m_scene->astro->getTrueDecRAbyName(star1, true);
    LLD s2 = m_scene->astro->getTrueDecRAbyName(star2, true);
    //LLD s1 = m_scene->m_astro->getDecRAbyName(star1, true);
    //LLD s2 = m_scene->m_astro->getDecRAbyName(star2, true);
    // Decide how to implement this in a way that shares as much as possible between Location, Earth and SkySphere, possibly via
    // a SceneObject::calcFunc() type, and SceneObject::position, SceneObject::rotation to account for Location being offset
    // Lines generically are arcs, with a parameter range, start and end points, calc function, ...
    // Maybe move a copy of Earth::updateCompositePath() to GenericPath::construct(), and cache values for use in GenericPath::update()
    // But since LLD to XYZ depends on the shape of the surface, this can get complicated.
    //
    // GenericPath will require a parent object, of type Earth, Location or SkySphere, so those should inherit from Geometrized.
    // Geometrized supplies getLoc3D_XX functions.
    // GenericPath will also require a calcFunc which calculates lat/lon based on the desired path (calcGreatArc, calcFlatArc, etc)
    // The calcFuncs can also be part of Geometrized.
    // Problem: Celestial DecRA and Earth LatLon coordinates rotate with respect to each other. This should be accounted for by callers.
    // Planetary paths and ground tracks: Calculate in SkySphere coordinates, and have 
    m_paths.emplace_back(new GenericPath(m_scene));
    //std::cout << "JD: " << m_scene->m_astro->getJD() << "\n";
    glm::vec3 pos1 = getDecRA2Pos3D(s1.lat, s1.lon);
    //VPRINT(pos1);
    glm::vec3 pos2 = getDecRA2Pos3D(s2.lat, s2.lon);
    //VPRINT(pos2);
    m_paths.back()->addPoint(pos1);
    m_paths.back()->addPoint(pos2);
    m_paths.back()->generate();
    //std::cout << "Line: Star 1 " << s1.lat << "," << s1.lon << "\n";
    //std::cout << "Line: Star 2 " << s2.lat << "," << s2.lon << "\n";
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
float SkySphere::getMagnitude2Radius(const double magnitude) const {
    // This hack seems to match the ESO full sky diameters pretty closely for some reason.
    // The scale is really non-linear so should probably involve a logarithm
    return (float)((4.0 - (magnitude + 1.5) / 2.0) / (200.0 / m_Radius));
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
    ShapeFile::parseFile(records, filebase + ".shp");
    // If a generic shapefile loader is made, pass std::vector<ShapeRecord*> records by reference.
    parseNames(filebase + ".csv");

}
void CountryBorders::addBorder(Earth& earth, const std::string countryname) {
    // Check CSV file for spelling of countries, this does NOT have fuzzy search.
    // Alternatively look up the index manually and pass that instead, see below definition
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


// ----------------
//  Constellations
// ----------------
// For implementation notes, see CountryBorders
Constellations2::Constellations2(Scene* scene, const std::string& filebase) : m_scene(scene) {
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
    ShapeFile::parseFile(records, filebase + ".shp");
    // If a generic shapefile loader is made, pass std::vector<ShapeRecord*> records by reference.
    parseNames(filebase + ".csv");

}
void Constellations2::addBorder(BodyGeometry& geom, const std::string constellationname) {
    // Check CSV file for spelling of countries, this does NOT have fuzzy search.
    // Alternatively look up the index manually and pass that instead (as unsigned int)
    size_t index = 0;
    for (auto& cn : constellationnames) { // Should probably use a std::find, this is the 21st century.
        if (constellationname == cn.searchname) index = cn.index; // If there are duplicates, this returns the last
    }
    if (index == 0) {
        std::cout << "WARNING: Constellations::addBorder(): No match found for constellation name: " << constellationname << ", defaulting to country_id = 1\n";
        index = 1;
    }
    addBorder(geom, index);
}
void Constellations2::addBorder(BodyGeometry& geom, size_t rindex) {
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
        constellationparts.push_back(new bordeconstellationpartcache());
        constellationparts.back()->country_id = rindex + 1;
        constellationparts.back()->part = &part;
        constellationparts.back()->polyline = m_scene->newPolyLine(LIGHT_RED, 0.001f, part.length);
        constellationparts.back()->geom = &geom;
        for (size_t i = part.startindex; i < (part.startindex + part.length); i++) {
            constellationparts.back()->polyline->addPoint(geom.getLoc3D({ deg2rad * records[rindex]->points[i].latitude, deg2rad * records[rindex]->points[i].longitude, surface_offset }, true));
        }
        constellationparts.back()->polyline->generate();
    }
}
void Constellations2::update() { // Updates all of the country border parts at once
    // This is even prepared to update "countries" on other planets, if they have a getLoc3D() function (or separate instances of Earth)
    // NOTE: Since this is plotted at 0.0f terrain height, it will intersect Earth tris !!!
    for (auto& pcache : constellationparts) {
        //std::cout << "CountryBorder::update(): country_id = " << pcache->country_id << ", polyline = " << pcache->polyline << ", earth = " << pcache->earth << '\n';
        pcache->polyline->clearPoints();
        //std::cout << "CountryBorder::update(): part: " << part.partnum << " start: " << part.startindex << " length: " << part.length << "\n";
        for (size_t i = pcache->part->startindex; i < (pcache->part->startindex + pcache->part->length); i++) {
            pcache->polyline->addPoint(pcache->geom->getLoc3D({ deg2rad * records[pcache->country_id - 1]->points[i].latitude,
                deg2rad * records[pcache->country_id - 1]->points[i].longitude, surface_offset }, true));
        }
        pcache->polyline->generate();
    }
}
void Constellations2::draw() {
    std::cout << "Constellations::draw(): no need to call draw(), PolyLines were allocated via Scene, so will be drawn there.\n";
    //for (auto& pcache : borderparts) {
    //    pcache->polyline->draw();
    //}
}
int Constellations2::parseNames(const std::string& namefile) {
    std::istringstream parse;
    std::string line;
    std::ifstream stream(namefile);
    getline(stream, line); // Skip headers
    unsigned int i = 0;
    while (getline(stream, line)) {
        i++;
        constellationnames.emplace_back(ConstellationName());
        constellationnames.back().index = i;
        parse.clear();
        parse.str(line);
        std::getline(parse, constellationnames.back().searchname, ',');
        std::getline(parse, constellationnames.back().displayname, ',');
    }
    return 0;
}
