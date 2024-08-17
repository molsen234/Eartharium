
#include <list>
#include <queue>

#include <glm/gtc/matrix_transform.hpp>  // OpenGL projection and view matrices
#include <glm/gtx/rotate_vector.hpp>     // Rotation matrices for glm

#include "Primitives.h"
#include "Earth.h"      // !!! FIX: For shadow mapping. Refactor it. !!!


// --------
//  Lerper
// --------
// See Primitives.h = class templates like to live in *.h files rather than *.cpp
// !!! FIX: Move to somewhere else, it is not a primitive !!!

// -----------
//  ShadowBox
// -----------
ShadowBox::ShadowBox(Scene* scene, unsigned int w, unsigned int h) : m_scene(scene) {
    width = w;
    height = h;
    shadowTransforms.reserve(6);
    glActiveTexture(GL_TEXTURE1);
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0);
}
ShadowBox::~ShadowBox() { }
void ShadowBox::Render(Camera* cam, glm::vec3 lightPos) {  // pass far plane?
    // Render the shadow casting objects to depth map
    glActiveTexture(GL_TEXTURE1);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    glViewport(0, 0, width, height);
    glClear(GL_DEPTH_BUFFER_BIT);

    // ConfigureShaderAndMatrices();
    float aspect = (float)width / (float)height;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, far);
    shadowTransforms.clear();
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));
    // Render objects that are allowed to cast shadows
    //glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    // (so exclude SkyBox !!)
    //world->GetSphereUVFactoryb()->Draw(SHADOW_BOX);
    m_scene->getCylindersFactory()->draw(cam, SHADOW_BOX);
    //world->GetViewConesFactory()->Draw(SHADOW_BOX);
    //world->GetPlanesFactory()->Draw(SHADOW_BOX);
    m_scene->getConesFactory()->draw(cam, SHADOW_BOX);
    //world->GetDotsFactory()->Draw(SHADOW_BOX);


    // Cleanup
    //GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    //glWaitSync(fence, 0, GL_TIMEOUT_IGNORED);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_scene->m_app->w_width, m_scene->m_app->w_height);
    //glActiveTexture(GL_TEXTURE0);

}


// -----------
//  ShadowMap
// -----------
ShadowMap::ShadowMap(Scene* scene, unsigned int w, unsigned int h) : m_scene(scene) {
    width = w;
    height = h;

    glActiveTexture(GL_TEXTURE1);
    glGenFramebuffers(1, &m_depthmapFBO);
    glGenTextures(1, &depthmap);
    glBindTexture(GL_TEXTURE_2D, depthmap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float bordercolor[] = { 1.0,1.0,1.0,1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordercolor);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthmapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthmap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::cout << "ShadowMap::ShadowMap(): Created TextureSlot " << 1 << ", RenderID " << depthmap << ".\n";

    //glActiveTexture(GL_TEXTURE0);
}
ShadowMap::~ShadowMap() { }
void ShadowMap::Bind() {
    //glViewport(0, 0, width, height);
    //glBindFramebuffer(GL_FRAMEBUFFER, m_depthmapFBO);
    //glClear(GL_DEPTH_BUFFER_BIT);
    // From here go on and render the scene from the light's perspective, call Unbind() when done.
}
void ShadowMap::Unbind() {
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // NOTE: The following should be at the top of a normal Scene Render, but we don't have one yet:
    //glViewport(0, 0, world->w_width, world->w_height);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void ShadowMap::Render(Camera* cam) {
    glActiveTexture(GL_TEXTURE1);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthmapFBO);
    glViewport(0, 0, width, height);
    glBindTexture(GL_TEXTURE_2D, depthmap);
    glClear(GL_DEPTH_BUFFER_BIT);
    float near_plane = 1.0f, far_plane = 7.5f;
    glm::mat4 lightProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, near_plane, far_plane);
    glm::vec3 lightPos = m_scene->w_camera->CamLightDir * 5.0f;  //lightdir is normalized in Camera
    glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    lightSpaceMatrix = lightProjection * lightView;
    // Render objects that are allowed to cast shadows
    // (so exclude SkyBox !!)
    //world->GetSphereUVOb()->Draw(SHADOW_MAP);
    m_scene->getCylindersFactory()->draw(cam, SHADOW_MAP);
    //world->GetViewConesOb()->Draw(SHADOW_MAP);
    //world->GetPlanesOb()->Draw(SHADOW_MAP);
    m_scene->getConesFactory()->draw(cam, SHADOW_MAP);
    //world->GetDotsOb()->Draw(SHADOW_MAP);
    // Ensure frame is completely rendered before returning to scene render
    //GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    //glWaitSync(fence, 0, GL_TIMEOUT_IGNORED);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_scene->m_app->w_width, m_scene->m_app->w_height);
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
//  Bill Board
// ------------
// Builds a TextString and keeps it oriented towards the camera.
BillBoard::BillBoard(Scene* scene, Font* font, std::string text, glm::vec3 position, glm::vec4 color, float size) : m_scene(scene), m_font(font), m_text(text), m_pos(position) {
    glm::vec3 up = m_scene->w_camera->getUp(); // glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = glm::normalize(m_scene->w_camera->position - m_pos);
    glm::vec3 dir = glm::cross(up, camdir);
    m_textstring = m_scene->getTextFactory()->newText(m_font, "Sample Text", size, color, m_pos, dir, up);
    m_textstring->updatePosDirUp(m_pos, dir, up);
    m_textstring->updateText(m_text);
}
void BillBoard::update(glm::vec3 position) {
    // position defaults to previous position if unspecified
    if (position != NO_VEC3) m_pos = position;
    glm::vec3 up = m_scene->w_camera->getUp(); // glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 camdir = glm::normalize(m_scene->w_camera->position - m_pos);
    glm::vec3 dir = glm::cross(up, camdir);
    //m_textstring = m_scene->getTextFactory()->newText(m_font, "Sample Text", 0.08f, YELLOW, m_pos, dir, up);
    m_textstring->updatePosDirUp(m_pos, dir, up);
}
void BillBoard::changeColorSize(glm::vec4 color, float size) {
    if (color != NO_COLOR) m_textstring->m_color = color;
    if (size == NO_FLOAT) m_textstring->m_size = size;
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
// Also, this slowly fades while the animation is on pause. Maybe fade on new additions only.
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
    if (m_queue.size() > 0 && m_queue.begin()->position == pos) return; // Don't add duplicates. This also prevents the trail from fading while paused.
    float size = m_size; // Size of Dot
    if (m_taper) {
        // Reduce size and opacity of trail
        //glm::mat3 id = glm::mat3(1.0f);
        for (auto& p : m_queue) {  // Here whole trail could be faded out when desired
            p.size *= (float)m_sizefactor;
            //p.position = id * p.position;  // test in preparation for making this a SceneObject. It causes no perceptible delay?
            //                                  It will not work to repeatedly apply the world matrix. Current worldmatrix must be applied
            //                                  to the original point stored every time. So, could add p.originalpos to the structure.
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
    //if (limit) std::cout << "WARNING PolyLine (" << this << ") adding beyond capacity, resizing!\n"; // only triggers if coming back after setting limit flag
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
}


// -----------
//  PolyCurve
// -----------
// ToDo: !!! Make a PolyCurve factory that can render all the PolyCurves in one draw call, it should massively improve performance !!!
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


// --------------
//  Generic Path
// --------------
GenericPath::GenericPath(Scene* scene, float width, glm::vec4 color) { // : SceneObject(scene) {
    if (scene == nullptr) return;
    if (color == NO_COLOR) color = GREEN;
    if (width == NO_FLOAT) width = 0.005f;
    m_scene = scene;
    m_color = color;
    m_width = width;
    m_curves.emplace_back(m_scene->newPolyCurve(m_color, m_width, NO_UINT));
    m_curve = 0;
}
GenericPath::~GenericPath() {
    for (auto& c : m_curves) {
        m_scene->deletePolyCurve(c);
    }
    m_curves.clear();
}
void GenericPath::setColor(glm::vec4 color) {
    m_color = color;
    for (auto p : m_curves) {
        p->changePolyCurve(m_color);
    }
}
void GenericPath::setWidth(float width) {
    m_width = width;
    for (auto p : m_curves) {
        p->changePolyCurve(NO_COLOR, m_width);
    }
}
void GenericPath::addPoint(glm::vec3 point) {
    m_curves[m_curve]->addPoint(point);
}
void GenericPath::addSplit(glm::vec3 point1, glm::vec3 point2) {
    // point1 is last point in current PolyCurve, point2 is first point in next PolyCurve
    m_curves[m_curve]->addPoint(point1);
    if (m_curves.size() - 1 <= m_curve) m_curves.emplace_back(m_scene->newPolyCurve(m_color, m_width, NO_UINT));
    m_curve++;
    m_curves[m_curve]->addPoint(point2);
}
void GenericPath::clearPoints() {
    // Don't delete PolyCurve objects, reuse them as needed. Empty PolyCurve will be skipped in PolyCurve::draw()
    for (auto& c : m_curves) {
        c->clearPoints();
    }
    m_curve = 0;
}
void GenericPath::generate() {
    for (auto& c : m_curves) {
        c->generate();
    }
}
void GenericPath::draw(Camera* cam) {} // SceneObject children need a draw() but here the PolyCurve takes care of drawing independently


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
    //std::cout << "Prmitives::draw() " << m_Primitives.size() << " elements.\n";
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
    // !!! ERROR: m_Primitives[0] is NOT the first entry in the underlying m_Elements vector of tightvec !!!
    //vb2 = new VertexBuffer(&m_Primitives[0], (unsigned int)m_Primitives.size() * sizeof(Primitive3D));
    // Either of these will work:
    //vb2 = new VertexBuffer(&m_Primitives[m_Primitives.physFirst()], (unsigned int)m_Primitives.size() * sizeof(Primitive3D));
    vb2 = new VertexBuffer(&m_Primitives.m_Elements[0], (unsigned int)m_Primitives.size() * sizeof(Primitive3D));
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

    shdr->Bind();
    // NOTE: Consider passing in 1 multiplied matrix instead of these:
    //shdr->SetUniformMatrix4f("view", cam->getSkyViewMat());
    shdr->SetUniformMatrix4f("view", cam->getViewMat());
    shdr->SetUniformMatrix4f("projection", cam->getProjMat());
    shdr->SetUniform3f("lightDir", cam->CamLightDir);

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
    m_Primitives.reserve(DOTS_RESERVE);
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
