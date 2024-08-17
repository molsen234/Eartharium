#pragma once

#include "Renderer.h"
#include "Primitives.h"

//// -----------
////  PolyLine SceneObject aware
//// -----------
//class PolyLineSO : public SceneObject {
//private:
//    //Scene* m_scene = nullptr;
//    glm::vec4 m_color = NO_COLOR;
//    float m_width = 0.0f;
//    unsigned int facets = 8;
//    std::vector<Vertex> m_verts;
//    std::vector<Tri> m_tris;
//    std::vector<Primitive3D> m_segments;
//    Shader* shdr = nullptr;
//    VertexArray* va = nullptr;
//    VertexBuffer* vb1 = nullptr;
//    VertexBuffer* vb2 = nullptr;
//    VertexBufferLayout* vbl1 = nullptr;
//    VertexBufferLayout* vbl2 = nullptr;
//    IndexBuffer* ib = nullptr;
//    bool dirty = false;  // If changes have been made which require regenerating m_segments since last update() call
//    //bool limit = false; // For debugging reserve() issues 
//public:
//    std::vector<glm::vec3> m_points; // Public so we can use it as track for objects (PathTracker in Earth.h)
//    PolyLineSO(Scene* scene, SceneObject* parent, glm::vec4 color, float width, size_t reserve = NO_UINT);
//    ~PolyLineSO();
//    void setColor(glm::vec4 color);
//    void setWidth(float width);
//    void changePolyCurve(glm::vec4 color = NO_COLOR, float width = NO_FLOAT);
//    void addPoint(glm::vec3 point);
//    void clearPoints();
//    void generate();
//    bool update() override;
//    void draw(Camera* cam);
//    void genGeom();
//};


