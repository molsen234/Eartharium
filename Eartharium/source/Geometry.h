#pragma once

#include "Primitives.h"
#include "Renderer.h"  // -> SceneObjects



// Rectangular items



// Spherical items

////SceneObject template
//class CoordinateSphere : public SceneObject {
//public:
//	CoordinateSphere(Scene* scene, SceneObject* parent) : SceneObject(scene, parent) {
//	}
//	~CoordinateSphere() {
//	}
//	void setColor(glm::vec4 color) {
//	}
//	void setRadius(float width) {
//	}
//	void generate() {
//	}
//	bool update() override { return false; }
//	void draw(Camera* cam) override { return; }
//private:
//	// private parts here
//};



#define DEFAULT_SEMIMAJOR 1.0
#define DEFAULT_ECCENTRICITY 0.5
class Ellipse : public SceneObject {
public:
	Ellipse(Scene* scene, SceneObject* parent) : SceneObject(scene, parent) {
		m_path = new GenericPath(scene);  // Can specify width and color if desired, might want to add optionals to Ellipse ctor
		setSemiMajorEccentricity(DEFAULT_SEMIMAJOR, DEFAULT_ECCENTRICITY);  // Sets default parameters and generates the path
	}
	~Ellipse() {
		if (m_path) delete m_path;
	}
	void setColor(glm::vec4 color) {
		m_path->setColor(color);
	}
	void setWidth(float width) {
		m_path->setWidth(width);
	}
	void setSemiMajorMinor(double semimajor, double semiminor) {
		m_semimajor_axis = semimajor;
		m_semiminor_axis = semiminor;
		m_flattening = 1.0 - (m_semiminor_axis / m_semimajor_axis);
		m_eccentricity = sqrt(m_flattening * (2.0 - m_flattening));
		generate();
	}
	void setSemiMajorEccentricity(double semimajor, double eccentricity) {
		m_semimajor_axis = semimajor;
		m_eccentricity = eccentricity;
		m_flattening = 1.0 - sqrt(1.0 - eccentricity * eccentricity);
		m_semiminor_axis = semimajor - (semimajor * m_flattening);
		generate();
	}
	void generate() {
		m_path->clearPoints();
		for (double t{ 0.0 }; t < tau; t += deg2rad) {
			m_path->addPoint({ 0.0, m_semimajor_axis * sin(t), m_semiminor_axis * cos(t) });
		}
		m_path->generate();
		std::cout << m_semimajor_axis << ", " << m_eccentricity << '\n';
	}
	bool update() override { return false; }
	void draw(Camera* cam) override { return; }
private:
	GenericPath* m_path{ nullptr };
	double m_semimajor_axis{ 0.0 };
	double m_semiminor_axis{ 0.0 };
	double m_eccentricity{ 0.0 };
	double m_flattening{ 0.0 };
};





//class CoordinateSphere : public SceneObject {
//public:
//	CoordinateSphere(Scene* scene, SceneObject* parent) : SceneObject(scene, parent) {
//	}
//	~CoordinateSphere() {
//	}
//	void setColor(glm::vec4 color) {
//	}
//	void setRadius(float width) {
//	}
//	void generate() {
//	}
//	bool update() override { return false; }
//	void draw(Camera* cam) override { return; }
//private:
//
//};



