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

// My includes
#include "config.h"

// Mathematical constants
const double pi = 3.14159265358979323846;
const double tau = 2.0 * pi;
const double pi2 = pi / 2.0;
const double deg2rad = tau / 360.0;
const double rad2deg = 360.0 / tau;
const float pif = 3.14159265358979323846f;
const float tauf = 2.0f * pif;
const float pi2f = pif / 2.0f;
const float deg2radf = tauf / 360.0f;
const float rad2degf = 360.0f / tauf;
const double hrs2dec = 15.0;
const double dec2hrs = 1.0 / 15.0;
const double rad2hrs = 24.0 / tau;
const double hrs2rad = tau / 24.0;
const double ninety = deg2rad * 89.99999;  // Used to avoid singularity at poles
const double tiny = 0.00001;               // Used to determine practically zero
const unsigned int maxuint = 4294967295;   // pow(2,32)-1 used to represent 'none' for various indices
const float maxfloat = FLT_MAX;

// Astronomical constants
const double earthradius = 6378.1370;  // kilometers
const double earthaxialtilt = 23.439281; // degrees (2007 wikipedia). Value changes over time, as Earth axis wobbles
const double earthtropics = 23.4365;   // degrees (DMS: 23 26 11.4). Value fixed by convention - No !!! Is defined as obliquity of ecliptic
const double eartharctics = 66.5635;   // NOTE: Actually changes over time, but set here to align with tropics
const double astronomicalunit = 149597870.7; // 1 AU in km, from https://en.wikipedia.org/wiki/Astronomical_unit
const double sunradius = 696340.0;     // Some uncertainty around this number: https://academic.oup.com/mnras/article/276/2/476/998827
const double moonradius = 1737.3;
const double sidereals = 86164.09053083288; // Seconds in 1 sidereal day
const double sidereald = sidereals / 86400; // Sidereal day as fraction of solar day
const double km2au = astronomicalunit;
const double au2km = 1 / astronomicalunit;

// Protos
GLFWwindow* setupEnv(unsigned int width, unsigned int height, GLint major, GLint minor, bool fullscreen);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void saveImage(std::string& filepath, GLFWwindow* w, unsigned int framebuffer = 0);

// Actually still in Application.cpp because it uses global vars (the callback cannot be in a class, it is C and not C++)
void keyboard_callback(GLFWwindow*, int key, int scancode, int action, int mods);
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);


// TEMPORARY Protos
class Scene;

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
		static_assert(false);
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
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform2f(const std::string& name, float v0, float v1);
	void SetUniform1f(const std::string& name, float v0);
	void SetUniformMatrix4f(const std::string& name, const glm::mat4 matrix);
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
	PRIMITIVE_SHADOW_MAP_SHADER,
	PRIMITIVE_SHADOW_BOX_SHADER,
	PIP_SHADER,
	PLANETOID_SHADER,
	SKY_BOX_SHADER,
	SKY_SPHERE_SHADER
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
	std::array<ShaderEntry, 10> shaders = { {
		{ EARTH_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\earth.shader" },
		{ EARTH_SHADOW_MAP_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitiveshadow.shader" },
		{ EARTH_SHADOW_BOX_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitivesdwbox.shader" },
		{ PRIMITIVE_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitive.shader" },
		{ PRIMITIVE_SHADOW_MAP_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitiveshadow.shader" },
		{ PRIMITIVE_SHADOW_BOX_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\primitivesdwbox.shader" },
		{ PIP_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\pip.shader" },
		{ PLANETOID_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\planetoid.shader" },
		{ SKY_BOX_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\skybox.shader" },
		{ SKY_SPHERE_SHADER, maxuint, nullptr, 0, "C:\\Coding\\Eartharium\\Eartharium\\shaders\\skysphere.shader" }
	} };
public:
	//ShaderLibrary() = default;
	//~ShaderLibrary() = default;
	Shader* getShader(unsigned int shader);
};


class World;
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
	void Render(glm::vec3 lightPos);
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
	void Render();
};

#endif // _MDO_OPENGL_H