
#include <fstream>
#include <iostream>
#include <sstream>

#include <glm/gtc/type_ptr.hpp>          // Additional glm types

#include "OpenGL.h"
//#include "Primitives.h"

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
unsigned int VertexBuffer::GetRenderID() const {
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
void IndexBuffer::Bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RenderID);
}
void IndexBuffer::Unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
unsigned int IndexBuffer::GetRenderID() const {
    return m_RenderID;
}
unsigned int IndexBuffer::GetCount() const {
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


// -----------------
//  Texture Library
// -----------------
Texture* TextureLibrary::getTexture(unsigned int texture) {
    if (textures[texture].tex == nullptr) textures[texture].tex = new Texture(textures[texture].file, textures[texture].slot);
    textures[texture].count++;
    return textures[texture].tex;
}

// ----------------
//  Texture Buffer
// ----------------
// Alternative constructor for textures rendered on GPU? (Insolation)
// (Those don't need loading, but a reference class might be handy.
// Revise this idea when making a material class.
// UPD: Turns out insolation can be rendered directly in the shader, so no need for texturing
//      However, there is a need for overlays of temperature maps etc.
Texture::Texture(const std::string& filepath, unsigned int texslot)  // Pass in force channels param for SOIL2? GL parameters for texture?
	: m_FilePath(filepath), m_RenderID(0), m_TextureSlot(texslot) {
	LoadTextureFile();
	glActiveTexture(m_TextureSlot);
	glGenTextures(1, &m_RenderID);
	glBindTexture(GL_TEXTURE_2D, m_RenderID);
    //std::cout << "Texture::Texture(): TextureSlot: " << m_TextureSlot - GL_TEXTURE0 << ", RenderID: " << m_RenderID << ".\n";
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
    : m_FilePath(filepath), m_RenderID(0) {
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
void Shader::SetUniform4f(const std::string& name, glm::vec4 v) {
    int location = Shader::GetUniformLocation(name);
    glUniform4f(location, v.x, v.y, v.z, v.w);
}
void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {
    int location = Shader::GetUniformLocation(name);
    glUniform3f(location, v0, v1, v2);
}
void Shader::SetUniform3f(const std::string& name, glm::vec3 v) {
    int location = Shader::GetUniformLocation(name);
    glUniform3f(location, v.x, v.y, v.z);
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
    int location = Shader::GetUniformLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
void Shader::SetUniformMatrix3f(const std::string& name, const glm::mat3 matrix) {
    int location = Shader::GetUniformLocation(name);
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
int Shader::GetUniformLocation(const std::string& name) {
    return glGetUniformLocation(m_RenderID, name.c_str());
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


