#include "mdoOpenGL.h"
#include "Primitives.h"


// ---------------
//  Vertex Buffer
// ---------------
VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &m_RenderID);
    LoadData(data, size);
}
VertexBuffer::~VertexBuffer()
{
    glDeleteBuffers(1, &m_RenderID);
}
void VertexBuffer::LoadData(const void* data, unsigned int size) {
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
}
void VertexBuffer::UpdateData(const void* data, unsigned int size) {
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
    glBufferSubData(GL_ARRAY_BUFFER,0, size, data);
}
void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_RenderID);
}
void VertexBuffer::Unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
unsigned int VertexBuffer::GetRenderID() {
    return m_RenderID;
}


// --------------
//  Index Buffer
// --------------
IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) {
    //std::cout << "IndexBuffer::IndexBuffer(" << data << ", " << count << ")\n";
    m_Count = count;
    glGenBuffers(1, &m_RenderID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Count * sizeof(unsigned int), data, GL_DYNAMIC_DRAW);
}
IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &m_RenderID);
}
void IndexBuffer::Bind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
}
void IndexBuffer::Unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
unsigned int IndexBuffer::GetRenderID() {
    return m_RenderID;
}
unsigned int IndexBuffer::GetCount() {
    return m_Count;
}


// --------------
//  Vertex Array 
// --------------
VertexArray::VertexArray() {
	m_Elems = 0;
	glGenVertexArrays(1, &m_RenderID);
	//std::cout << "Created VA: " << m_RenderID << std::endl;
	//glBindVertexArray(m_RenderID);  // No need to bind until we get a buffer and a layout
}
VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &m_RenderID);
}
void VertexArray::Bind() const {
	glBindVertexArray(m_RenderID);
	for (unsigned int i = 0; i < m_Elems; i++) {
		glEnableVertexAttribArray(i);
	}
}
void VertexArray::Unbind() const {
	glBindVertexArray(m_RenderID);
	for (unsigned int i = 0; i < m_Elems; i++) {
		glDisableVertexAttribArray(i);
	}
	glBindVertexArray(0);
}
unsigned int VertexArray::GetRenderID() {
	return m_RenderID; 
}
void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, bool clear) {
	Bind();
	vb.Bind();
	if (clear == true) m_Elems = 0;
	const auto& elements = layout.GetElements();
    __int64 offset = 0;
	unsigned int i;
	for (i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		glEnableVertexAttribArray(i + m_Elems);
		glVertexAttribPointer(i + m_Elems, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset);
		//std::cout << "VAA: " << i + m_Elems << " " << element.count << " " << offset << " " << layout.GetStride() << std::endl;
		offset += ((__int64)element.count * VertexBufferElement::GetSizeOfType(element.type));
	}
	m_Elems += i;
}


// ----------------
//  Texture Buffer
// ----------------
// Alternative constructor for textures rendered on GPU? (Insolation)
// (Those don't need loading, but a reference class might be handy.
// Revise this idea when making a material class.
Texture::Texture(const std::string& filepath, unsigned int texslot)  // Pass in force channels param for SOIL2? GL parameters for texture?
	: m_FilePath(filepath), m_RenderID(0), m_TextureSlot(texslot)
{
	LoadTextureFile();
	glActiveTexture(m_TextureSlot);
	glGenTextures(1, &m_RenderID);
	glBindTexture(GL_TEXTURE_2D, m_RenderID);
    //std::cout << "Texture::Texture(): TextureSlot: " << m_TextureSlot - GL_TEXTURE0 << ", RenderID: " << m_RenderID << ".\n";
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (glfwExtensionSupported("GL_ARB_texture_filter_anisotropic")) {
        GLfloat value, max_anisotropy = 4.0f; /* don't exceed this value...*/
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &value);
        value = (value > max_anisotropy) ? max_anisotropy : value;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, value);
    }
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, i_width, i_height, 0, GL_RGB, GL_UNSIGNED_BYTE, i_image);
	glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    SOIL_free_image_data(i_image);
	return;
}
Texture::~Texture() {
	Unbind();
    glActiveTexture(m_TextureSlot);
    glBindTexture(GL_TEXTURE_2D, 0);
    //std::cout << "\nDeleting texture: " << m_RenderID << std::endl;
	glDeleteTextures(1, &m_RenderID);
	return;
}
void Texture::LoadTextureFile() {
	// NOTE: SOIL2 chokes on large image files when asked to flip_y, so flip images before loading them.
	i_image = SOIL_load_image(m_FilePath.c_str(), &i_width, &i_height, &i_channels, 0);
	if (!i_image) { std::cout << "FAILED LOADING TEXTURE IMAGE FROM DISC! " << m_FilePath.c_str() << std::endl; }
	//else { std::cout << "Texture slot "<< m_TextureSlot <<  " loaded: " << m_FilePath.c_str() << ", " << &i_image << ", " << i_width << ", " << i_height << std::endl; }
	return;
}
void Texture::Bind() {
	glActiveTexture(m_TextureSlot);
	glBindTexture(GL_TEXTURE_2D, m_RenderID);
    //std::cout << "Texture::Bind(): TextureSlot: " << m_TextureSlot - GL_TEXTURE0 << ", RenderID: " << m_RenderID << ".\n";
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
}
void Texture::Unbind() {
    glActiveTexture(GL_TEXTURE0);
}
unsigned int Texture::GetRenderID() { return m_RenderID; }
unsigned int Texture::GetTextureSlot() { return m_TextureSlot-GL_TEXTURE0; }


// --------
//  Shader 
// --------
Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath), m_RenderID(0)
{
    ShaderProgramSource source = ParseShader();
    m_RenderID = CreateShader(source.VertexSource, source.GeometrySource, source.FragmentSource);
}
Shader::~Shader() {
    glDeleteProgram(m_RenderID);
}
void Shader::Bind() {
    glUseProgram(m_RenderID);
}
void Shader::Unbind() {
    glUseProgram(0);
}
unsigned int Shader::GetRenderID() { 
    return m_RenderID;
}
void Shader::SetUniform1i(const std::string& name, unsigned int v0) {
    int location = Shader::GetUniformLocation(name);
    //std::cout << "Setting uniform '" << name << "' to " << v0 << std::endl;
    glUniform1i(location, v0);
}
void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    int location = Shader::GetUniformLocation(name);
    glUniform4f(location, v0, v1, v2, v3);
}
void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {
    int location = Shader::GetUniformLocation(name);
    glUniform3f(location, v0, v1, v2);
}
void Shader::SetUniform2f(const std::string& name, float v0, float v1) {
    int location = Shader::GetUniformLocation(name);
    glUniform2f(location, v0, v1);
}
void Shader::SetUniform1f(const std::string& name, float v0) {
    int location = Shader::GetUniformLocation(name);
    glUniform1f(location, v0);
}
void Shader::SetUniformMatrix4f(const std::string& name, const glm::mat4 matrix) {
    //std::cout << "Shader::SetUniformMatrix4f{" << m_RenderID << "}";
    //std::cout << "(" << name << ", " << &matrix << ")\n";
    int location = Shader::GetUniformLocation(name); // This is causing the crash
    //std::cout << "location = " << location << "\n";
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
int Shader::GetUniformLocation(const std::string& name) {
    //std::cout << "Shader::GetUniformLocation{" << m_RenderID << "}(" << name << ")\n";
    int retval = glGetUniformLocation(m_RenderID, name.c_str());
    return retval;
}
ShaderProgramSource Shader::ParseShader() {
    enum class ShaderType {
        NONE = -1, VERTEX = 0, GEOMETRY = 1, FRAGMENT = 2
    };
    ShaderType type = ShaderType::NONE;
    std::string line;
    std::stringstream ss[3];
    std::ifstream stream(m_FilePath);
    //bool status = stream.good();
    //std::cout << "Shader file status: " << status << std::endl;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            if (line.find("geometry") != std::string::npos) {
                type = ShaderType::GEOMETRY;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << "\n";
        }
    }
    return { ss[0].str(), ss[1].str(), ss[2].str() };
};
unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, NULL);
    glCompileShader(id);
    // Get status of compilation    
    int  success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(id, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED: " << m_FilePath << "\n" << infoLog << std::endl;
        return 0;
    }
    return id;
}
unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader) {
    unsigned int shaderProgram = glCreateProgram();
    unsigned int vs = maxuint;
    unsigned int gs = maxuint;
    unsigned int fs = maxuint;
    if (vertexShader.size() > 7) vs = Shader::CompileShader(GL_VERTEX_SHADER, vertexShader);
    if (geometryShader.size() > 7) gs = Shader::CompileShader(GL_GEOMETRY_SHADER, geometryShader);
    if (fragmentShader.size() > 7) fs = Shader::CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    if (vs != maxuint) glAttachShader(shaderProgram, vs);
    if (gs != maxuint) glAttachShader(shaderProgram, gs);
    if (fs != maxuint) glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    // Get status of linking
    int  success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADERPROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glValidateProgram(shaderProgram);
    // Shaders are now in the shader program and can be deleted
    if (vs != maxuint) glDeleteShader(vs);
    if (gs != maxuint) glDeleteShader(gs);
    if (fs != maxuint) glDeleteShader(fs);
    return shaderProgram;
}


// ----------------
//  Shader Library
// ----------------
Shader* ShaderLibrary::getShader(unsigned int shader) {
    if (shaders[shader].shdr == nullptr) shaders[shader].shdr = new Shader(shaders[shader].file);
    shaders[shader].count++;
    return shaders[shader].shdr;
}


// ----------------------
// OpenGL Initialization
// ----------------------
GLFWwindow* setupEnv(unsigned int width, unsigned int height, GLint major, GLint minor, bool fullscreen) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(width, height, "Eartharium", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return NULL;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return NULL;
    }
    // Check if GLFW enabled multisamples
    GLint parm = 0;
    glGetNamedFramebufferParameteriv(0, GL_SAMPLES, &parm);
    std::cout << "default fbo GL_SAMPLES: " << parm << "\n";  // Note: Intel UHD 630 drivers give incorrect values!

    int texture_units;
    int tex_max_units;
    int linwidth[2];
    int vplims[2];
    float maxanis;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &tex_max_units);
    glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE, linwidth);
    glGetIntegerv(GL_MAX_VIEWPORT_DIMS, vplims);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxanis);
    std::cout << "OpenGL version:  " << glGetString(GL_VERSION) << "\n";
    std::cout << "OpenGL vendor:   " << glGetString(GL_VENDOR) << "\n";
    std::cout << "OpenGL renderer: " << glGetString(GL_RENDERER) << "\n";
    std::cout << "Available texture slots: " << texture_units << " per stage, " << tex_max_units << " total." << "\n";
    std::cout << "Available line width: " << linwidth[0] << " to " << linwidth[1] << "\n";
    std::cout.precision(15);
    std::cout << "Maximum Anisotropic Filtering Samples: " << maxanis << "\n";
    std::cout << "Maximum Viewport size (x,y): " << vplims[0] << ", " << vplims[1] << '\n';
    std::cout << "\n\n" << std::endl;
    // Register callback for when user resizes the window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyboard_callback);

    // Register debug callback to enable debug output
    //glEnable(GL_DEBUG_OUTPUT);
    //glDebugMessageCallback(MessageCallback, 0);

    glfwSwapInterval(0);

    // Set global openGL parameters
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    // Transparency / blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    return window;
}


// -----------------
//  Save Screenshot
// -----------------
void saveImage(std::string& filepath, GLFWwindow* w, unsigned int framebuffer /* = 0 */, int fb_width, int fb_height) {
    // Saving named framebuffer doesn't work right, it seems to save a low res image of the main window instead
    int width, height;
    if (framebuffer == 0) glfwGetFramebufferSize(w, &width, &height);
    else {
        width = fb_width;
        height = fb_height;
    }
    GLsizei nrChannels = 3;
    GLsizei stride = nrChannels * width;
    stride += (stride % 4) ? (4 - stride % 4) : 0;
    GLsizei bufferSize = stride * height;
    std::vector<char> buffer(bufferSize);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    if (framebuffer != 0) glNamedFramebufferReadBuffer(framebuffer, GL_FRONT);
    else glReadBuffer(GL_BACK);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
    // MDO - flip image (code from SOIL_save_screenshot()
    int i,j;
    for (j = 0; j * 2 < height; ++j)
    {
        int index1 = j * width * 3;
        int index2 = (height - 1 - j) * width * 3;
        for (i = width * 3; i > 0; --i)
        {
            unsigned char temp = buffer[index1];
            buffer[index1] = buffer[index2];
            buffer[index2] = temp;
            ++index1;
            ++index2;
        }
    }
    //stbi_flip_vertically_on_write(true);
    SOIL_save_image(filepath.c_str(), SOIL_SAVE_TYPE_PNG, width, height, nrChannels, (const unsigned char*)buffer.data());
}


// -----------
//  ShadowBox
// -----------
ShadowBox::ShadowBox(Scene* scene, unsigned int w,unsigned int h) : m_scene(scene) {
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
void ShadowBox::Render(glm::vec3 lightPos) {  // pass far plane?
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
    m_scene->getCylindersFactory()->draw(SHADOW_BOX);
    //world->GetViewConesFactory()->Draw(SHADOW_BOX);
    //world->GetPlanesFactory()->Draw(SHADOW_BOX);
    m_scene->getConesFactory()->draw(SHADOW_BOX);
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
void ShadowMap::Render() {
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
    m_scene->getCylindersFactory()->draw(SHADOW_MAP);
    //world->GetViewConesOb()->Draw(SHADOW_MAP);
    //world->GetPlanesOb()->Draw(SHADOW_MAP);
    m_scene->getConesFactory()->draw(SHADOW_MAP);
    //world->GetDotsOb()->Draw(SHADOW_MAP);
    // Ensure frame is completely rendered before returning to scene render
    //GLsync fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    //glWaitSync(fence, 0, GL_TIMEOUT_IGNORED);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_scene->m_app->w_width, m_scene->m_app->w_height);
}
