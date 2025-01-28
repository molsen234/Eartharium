#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <array>
#include <string>
#include <vector>

// OpenGL specific library includes
#include <glad/glad.h>                   // Platform specific OpenGL call encapsulation
#include <GLFW/glfw3.h>                  // Platform abstraction for OpenGL viewports and windowing

#include <SOIL2/SOIL2.h>                 // Image library for loading textures from file and saving screenshots

#include "config.h"

// Protos
GLFWwindow* setupEnv(unsigned int width, unsigned int height, GLint major, GLint minor, bool fullscreen, bool debug_info = false);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);
void saveImage(std::string& filepath, GLFWwindow* w, unsigned int framebuffer = 0, int width = 0, int height = 0);

// Actually still in Eartharium.cpp because it uses global vars (the callback cannot be in a class, it is C and not C++)
void keyboard_callback(GLFWwindow*, int key, int scancode, int action, int mods);
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);


// TEMPORARY Protos
class Scene;
class World;
class Camera;
class Texture;


// ----------------------
//  Vertex Buffer Layout
// ----------------------
struct VertexBufferElement {
	unsigned int type;
	unsigned int count;
	unsigned char normalized;
	static unsigned int GetSizeOfType(unsigned int type) {
		switch (type) {
		case GL_FLOAT: return sizeof(GLfloat);
		case GL_UNSIGNED_INT: return sizeof(GLuint);
		case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
		}
		return 0;
	}
};
class VertexBufferLayout {
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;
public:
	VertexBufferLayout()
		: m_Stride(0)
	{}
	template<typename T>
	void Push(unsigned int count) {
		//static_assert(false);
	}
	template<>
	void Push<float>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
	}
	template<>
	void Push<unsigned int>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
	}
	template<>
	void Push<unsigned char>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
	}
	inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
	inline unsigned int GetStride() const { return m_Stride; }
};


// ---------------
//  Vertex Buffer
// ---------------
class VertexBuffer {
private:
	unsigned int m_RenderID;
public:
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();
	void LoadData(const void* data, unsigned int size);
	void UpdateData(const void* data, unsigned int size);
	unsigned int GetRenderID() const;
	void Bind() const;
	void Unbind() const;
	// Might want to have Lock()/Unlock() so you can modify the data without colliding with rendering
	// (in a game engine for example). Here we probably build and then render the objects in serial.
};


// --------------
//  Index Buffer
// --------------
class IndexBuffer {
private:
	unsigned int m_RenderID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int size);
	~IndexBuffer();
	void Bind() const;
	void Unbind() const;
	unsigned int GetRenderID() const;
	unsigned int GetCount() const;
	// Might want to have Lock()/Unlock() so you can modify the data without colliding with rendering
	// (in a game engine for example). Here we probably build and then render the objects in serial.
};


// --------------
//  Vertex Array 
// --------------
class VertexArray {
private:
	unsigned int m_RenderID;
	unsigned int m_Elems;
public:
	VertexArray();
	~VertexArray();
	void Bind() const;
	void Unbind() const;
	unsigned int GetRenderID();
	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, bool clear);
};



// -----------------
//  Texture Library
// -----------------
// Texture Slots - Just a note to remember which texture slots are in use on the GPU
// Shadows = 1, SkyBox = 2, Earth = 3 + 4, Sundot = 5, Moondot = 6, OutputRender = 7, Font = 8, DetailedMoon = 9.
// There is probably  no real reason why the textures can't share a texture slot, look into that. I think the max is 32.

enum TextureType {
	DUMMY = 0,
	SHADOWS,
	SKYBOX,
	EARTH_DAY,
	EARTH_NIGHT,
	SUNDOT,
	MOONDOT,
	OUTPUT_RENDER,
	FONT_MAP,
	MOON_FULL,
	MOON_BUMP,
	EARTH_BUMP,
	SKY_FULL,
	PLANETOID_ATLAS
};
class TextureLibrary {
	struct TextureEntry {
		unsigned int Type = NO_UINT;
		unsigned int RenderID = NO_UINT;
		GLint slot = 0;
		Texture* tex = nullptr;
		unsigned int count = 0;
		std::string file;
	};
	std::array<TextureEntry, 14> textures = { {
		{ DUMMY, NO_UINT, DUMMY, nullptr, 0, "" },       // Dummy entry until I decide how to handle GL_TEXTUREx allocation
		{ SHADOWS, NO_UINT, GL_TEXTURE1, nullptr, 0, "" },       // Not loaded from file
		{ SKYBOX, NO_UINT, GL_TEXTURE2, nullptr, 0, "" },        // Not loaded from file
		{ EARTH_DAY, NO_UINT, GL_TEXTURE3, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\large map 8192x4096.png" },
		//{ EARTH_DAY, NO_UINT, GL_TEXTURE3, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\land_shallow_topo_8192_mdo.png" },
		{ EARTH_NIGHT, NO_UINT, GL_TEXTURE4, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\BlackMarble_2012_8192x4096_ice.png" },
		{ SUNDOT, NO_UINT, GL_TEXTURE5, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_sun.png" },
		{ MOONDOT, NO_UINT, GL_TEXTURE6, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_moon.png" },
		{ OUTPUT_RENDER, NO_UINT, GL_TEXTURE7, nullptr, 0, "" }, // Not loaded from file
		{ FONT_MAP, NO_UINT, GL_TEXTURE8, nullptr, 0, "" },      // Not loaded from file
		{ MOON_FULL, NO_UINT, GL_TEXTURE9, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\moon 8192x4096.png" },
		{ MOON_BUMP, NO_UINT, GL_TEXTURE10, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\moon height 8192x4096.png" },
		// EGM-96 geoid from: https://www.agisoft.com/downloads/geoids/
		//{ EARTH_BUMP, NO_UINT, GL_TEXTURE11, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\us_nga_egm96_15arcmin.png" }
		{ EARTH_BUMP, NO_UINT, GL_TEXTURE11, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\earth height 8192x4096.png" },
		{ SKY_FULL, NO_UINT, GL_TEXTURE12, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap-gimp_8k (8192x4096 celestial coords).png" },
		{ PLANETOID_ATLAS, NO_UINT, GL_TEXTURE13, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\planetoid-atlas2 4096x2048.png" }
	} };
public:
	Texture* getTexture(unsigned int texture);
};


// ----------------
//  Texture Buffer
// ----------------
class Texture {
public:
	// Currently none
private:
	unsigned int m_RenderID;
	unsigned int m_TextureSlot;
	std::string m_FilePath;
	unsigned char* i_image;
	int i_width, i_height, i_channels;
public:
	Texture(const std::string& filepath, unsigned int texslot);
	~Texture();
	void Bind();
	void Unbind();
	unsigned int GetRenderID();
	unsigned int GetTextureSlot();
private:
	void LoadTextureFile();
};


// --------
//  Shader
// --------
struct ShaderProgramSource {
	std::string VertexSource;
	std::string GeometrySource;
	std::string FragmentSource;
};
class Shader {
private:
	unsigned int m_RenderID;
	std::string m_FilePath;
public:
	Shader(const std::string& filepath);
	~Shader();
	void Bind();
	void Unbind();
	unsigned int GetRenderID();
	void SetUniform1i(const std::string& name, unsigned int v0);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform4f(const std::string& name, glm::vec4 v);
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform3f(const std::string& name, glm::vec3 v);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform1f(const std::string& name, float v0);
	void SetUniformMatrix4f(const std::string& name, const glm::mat4 matrix);
	void SetUniformMatrix3f(const std::string& name, const glm::mat3 matrix);
private:
	ShaderProgramSource ParseShader();
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
};


// ----------------
//  Shader Library
// ----------------
enum ShaderType {
	EARTH_SHADER = 0,
	EARTH_SHADOW_MAP_SHADER,
	EARTH_SHADOW_BOX_SHADER,
	PRIMITIVE_SHADER,
	PRIMITIVESO_SHADER,
	LINE_SHADER,
	PRIMITIVE_SHADOW_MAP_SHADER,
	PRIMITIVE_SHADOW_BOX_SHADER,
	BLIT_SHADER,
	GLYPH_SHADER,
	PLANETOID_SHADER,
	SKY_BOX_SHADER,
	SKY_SPHERE_SHADER,
	MOON_SHADER,
	SKY_SHADER
};
class ShaderLibrary {
	//World* m_world = nullptr;
	struct ShaderEntry {
		unsigned int Type = NO_UINT;
		unsigned int RenderID = NO_UINT;
		Shader* shdr = nullptr;
		unsigned int count = 0;
		std::string file;
	};
	std::array<ShaderEntry, 15> shaders = { {
		{ EARTH_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\earth.glsl" },
		{ EARTH_SHADOW_MAP_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitiveshadow.glsl" },
		{ EARTH_SHADOW_BOX_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitivesdwbox.glsl" },
		{ PRIMITIVE_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitive.glsl" },
		{ PRIMITIVESO_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitiveSO.glsl" },
		{ LINE_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\line.glsl" },
		{ PRIMITIVE_SHADOW_MAP_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitiveshadow.glsl" },
		{ PRIMITIVE_SHADOW_BOX_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitivesdwbox.glsl" },
		{ BLIT_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\blit.glsl" },
		{ GLYPH_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\glyph.glsl" },
		{ PLANETOID_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\planetoid.glsl" },
		{ SKY_BOX_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\skybox.glsl" },
		{ SKY_SPHERE_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\skysphere.glsl" },
		{ MOON_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\moon.glsl" },
		{ SKY_SHADER, NO_UINT, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\sky.glsl" }
	} };
public:
	//ShaderLibrary() = default;
	//~ShaderLibrary() = default;
	Shader* getShader(unsigned int shader);
};


