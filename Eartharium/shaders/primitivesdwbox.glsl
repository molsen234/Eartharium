#shader vertex
// Casts shadows from primitives for use with ShadowBox - if using ShadowMap use primitiveshadow.shader instead
#version 460 core
layout(location = 0) in vec3 aPos;      // primitive geometry
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aColor;    // primitive instances
layout(location = 4) in vec3 aOffset;
layout(location = 5) in vec3 aDir;
layout(location = 6) in vec3 aScale;

void main()
{
    // Calculate rotation angles from direction vector
    //  This will take Y axis to aDir
    float azi = atan(-aDir.x, aDir.y);
    float ele = atan(aDir.z, sqrt(aDir.x * aDir.x + aDir.y * aDir.y));
    
    // Vertex scaling
    vec3 c = vec3(aPos.x * aScale.x, aPos.y * aScale.y, aPos.z * aScale.z);
    
    // Vertex rotation
    vec3 a;
    float y = c.y * cos(ele) - c.z * sin(ele); // Rot X
    a.z = c.y * sin(ele) + c.z * cos(ele);
    a.x = c.x * cos(azi) - y * sin(azi);       // Rot Z
    a.y = c.x * sin(azi) + y * cos(azi);

    //// Normal rotation
    //vec3 b = normalize(aNormal);
    //y = b.y * cos(ele) - b.z * sin(ele);       // Rot X
    //bNormal.z = b.y * sin(ele) + b.z * cos(ele);
    //bNormal.x = b.x * cos(azi) - y * sin(azi); // Rot Z
    //bNormal.y = b.x * sin(azi) + y * cos(azi);

    // Translate and project vertex
    //Without shadow casting:
    //gl_Position = projection * view * vec4(a + aOffset, 1.0);
    //With shadow casting:
    gl_Position = vec4(a + aOffset, 1.0);
};

#shader geometry
#version 460 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // FragPos from GS (output per emitvertex)
void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // built-in variable that specifies to which face we render.
        for (int i = 0; i < 3; ++i) // for each triangle vertex
        {
            FragPos = gl_in[i].gl_Position;
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }
        EndPrimitive();
    }
}

#shader fragment
#version 460 core

in vec4 FragPos;
uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    // For shadow depth buffer, no color information is needed !!!
    // Enable when you figure out how to pass it through the geometry shader
    //if (frag.bColor.a == 0.0) discard; // Do discard invisible objects, they should not cast shadows either.

    // get distance between fragment and light source
    float lightDistance = length(FragPos.xyz - lightPos);
    // map to [0;1] range by dividing by far_plane
    lightDistance = lightDistance / far_plane;
    // write this as modified depth
    gl_FragDepth = lightDistance;
};
