#pragma once
#ifndef _MDO_OPENGL_H
#define _MDO_OPENGL_H

#define _CRT_SECURE_NO_WARNINGS

// std library includes
#include <vector>
#include <list>
#include <deque>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <array>
#include <chrono>
#include <ctime>
#include <cstddef> // for AAMoon.cpp
#include <algorithm>
#include <set>

// OpenGL specific library includes
#include <glad/glad.h>                   // Platform specific OpenGL call encapsulation
#include <GLFW/glfw3.h>                  // Platform abstraction for OpenGL viewports and windowing
#include <glm/glm.hpp>                   // OpenGL friendly linear algebra library
#include <glm/gtc/type_ptr.hpp>          // Additional glm types
#include <glm/gtc/matrix_transform.hpp>  // OpenGL projection and view matrices
#include <glm/gtx/rotate_vector.hpp>     // Rotation matrices for glm
#include <glm/gtx/string_cast.hpp>
#include "ImGUI/imgui.h" // ImGUI user interface - Moved to ImGUI folder in project, as that makes impl files happy
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"
//#include "ImGUI/imstb_truetype.h"
#include "ImGUI/implot.h"
#include <SOIL2/SOIL2.h>                 // Image library for loading textures from file and saving screenshots
// If using stb_image instead of SOIL2 (lighter, but less capable)
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>


// !!! FIX: DON'T DO THESE HERE !!!
// Task specific includes for astronomy calculations
//#define AAPLUS_VSOP87_NO_HIGH_PRECISION
#include "AAplus/AADate.h"
#include "AAplus/AACoordinateTransformation.h"
#include "AAplus/AASun.h"
#include "AAplus/AAMercury.h"
#include "AAplus/AAVenus.h"
#include "AAplus/AAEarth.h"
#include "AAplus/AAMars.h"
#include "AAplus/AAJupiter.h"
#include "AAplus/AASaturn.h"
#include "AAplus/AAUranus.h"
#include "AAplus/AANeptune.h"
#include "AAplus/AASidereal.h"
#include "AAplus/AANutation.h"
#include "AAplus/AAElliptical.h"
#include "AAplus/AAAberration.h"
#include "AAplus/AAFK5.h"
#include "AAplus/AAMoon.h"
#include "AAplus/AAEquationOfTime.h"
#include "AAplus/AAPlanetaryPhenomena.h"
#include "AAplus/AAPlanetaryPhenomena2.h"
#include "AAplus/AADiameters.h"
#include "AAplus/AARefraction.h"
#include "AAplus/AADynamicalTime.h"
#include "AAplus/AAPrecession.h"
#include "AAplus/AAPhysicalMoon.h"

// !!! FIX: DON'T DO THESE HERE !!!
// My includes
#include "config.h"

// Protos
GLFWwindow* setupEnv(unsigned int width, unsigned int height, GLint major, GLint minor, bool fullscreen);
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
	unsigned int GetRenderID();
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
	void Bind();
	void Unbind();
	unsigned int GetRenderID();
	unsigned int GetCount();
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
		unsigned int Type = maxuint;
		unsigned int RenderID = maxuint;
		GLint slot = 0;
		Texture* tex = nullptr;
		unsigned int count = 0;
		std::string file;
	};
	std::array<TextureEntry, 14> textures = { {
		{ DUMMY, maxuint, DUMMY, nullptr, 0, "" },       // Dummy entry until I decide how to handle GL_TEXTUREx allocation
		{ SHADOWS, maxuint, GL_TEXTURE1, nullptr, 0, "" },       // Not loaded from file
		{ SKYBOX, maxuint, GL_TEXTURE2, nullptr, 0, "" },        // Not loaded from file
		{ EARTH_DAY, maxuint, GL_TEXTURE3, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\large map 8192x4096.png" },
		{ EARTH_NIGHT, maxuint, GL_TEXTURE4, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\BlackMarble_2012_8192x4096_ice.png" },
		{ SUNDOT, maxuint, GL_TEXTURE5, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_sun.png" },
		{ MOONDOT, maxuint, GL_TEXTURE6, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\2k_moon.png" },
		{ OUTPUT_RENDER, maxuint, GL_TEXTURE7, nullptr, 0, "" }, // Not loaded from file
		{ FONT_MAP, maxuint, GL_TEXTURE8, nullptr, 0, "" },      // Not loaded from file
		{ MOON_FULL, maxuint, GL_TEXTURE9, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\moon 8192x4096.png" },
		{ MOON_BUMP, maxuint, GL_TEXTURE10, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\moon height 8192x4096.png" },
		// EGM-96 geoid from: https://www.agisoft.com/downloads/geoids/
		//{ EARTH_BUMP, maxuint, GL_TEXTURE11, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\us_nga_egm96_15arcmin.png" }
		{ EARTH_BUMP, maxuint, GL_TEXTURE11, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\earth height 8192x4096.png" },
		{ SKY_FULL, maxuint, GL_TEXTURE12, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\starmap-gimp_8k (8192x4096 celestial coords).png" },
		{ PLANETOID_ATLAS, maxuint, GL_TEXTURE13, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\textures\\planetoid-atlas2 4096x2048.png" }
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
		unsigned int Type = maxuint;
		unsigned int RenderID = maxuint;
		Shader* shdr = nullptr;
		unsigned int count = 0;
		std::string file;
	};
	std::array<ShaderEntry, 14> shaders = { {
		{ EARTH_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\earth.glsl" },
		{ EARTH_SHADOW_MAP_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitiveshadow.glsl" },
		{ EARTH_SHADOW_BOX_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitivesdwbox.glsl" },
		{ PRIMITIVE_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitive.glsl" },
		{ LINE_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\line.glsl" },
		{ PRIMITIVE_SHADOW_MAP_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitiveshadow.glsl" },
		{ PRIMITIVE_SHADOW_BOX_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitivesdwbox.glsl" },
		{ BLIT_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\blit.glsl" },
		{ GLYPH_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\glyph.glsl" },
		{ PLANETOID_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\planetoid.glsl" },
		{ SKY_BOX_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\skybox.glsl" },
		{ SKY_SPHERE_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\skysphere.glsl" },
		{ MOON_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\moon.glsl" },
		{ SKY_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\sky.glsl" }
	} };
public:
	//ShaderLibrary() = default;
	//~ShaderLibrary() = default;
	Shader* getShader(unsigned int shader);
};


// ------------
//  ShadowBox
// ------------
class ShadowBox {
private:
	Scene* m_scene;
	unsigned int depthMapFBO = maxuint;
	unsigned int width = 1024;
	unsigned int height = 1024;
public:
	unsigned int depthCubemap = maxuint;
	std::vector<glm::mat4> shadowTransforms;
	float near = 0.01f;
	float far = 20.0f;
	ShadowBox(Scene* scene, unsigned int w, unsigned int h);
	~ShadowBox();
	void Render(Camera* cam, glm::vec3 lightPos);
};


// -----------
//  ShadowMap
// -----------
class ShadowMap {
private:
	Scene* m_scene = nullptr;
	unsigned int m_depthmapFBO;
	unsigned int width;
	unsigned int height;
public:
	Shader* shdr = nullptr;
	unsigned int depthmap;
	glm::mat4 lightSpaceMatrix = glm::mat4(1.0f);
	ShadowMap(Scene* scene, unsigned int w, unsigned int h);
	~ShadowMap();
	void Bind();
	void Unbind();
	void Render(Camera* cam);
};

#endif // _MDO_OPENGL_H