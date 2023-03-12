#shader vertex
// Casts shadows from primitives for use with ShadowMap.
#version 460 core
layout(location = 0) in vec3 aPos;      // primitive geometry
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aColor;    // primitive instances
layout(location = 4) in vec3 aOffset;
layout(location = 5) in vec3 aDir;
layout(location = 6) in vec3 aScale;

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;

out vec4 bColor;
out vec3 bNormal;

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

    // Normal rotation
    vec3 b = normalize(aNormal);
    y = b.y * cos(ele) - b.z * sin(ele);       // Rot X
    bNormal.z = b.y * sin(ele) + b.z * cos(ele);
    bNormal.x = b.x * cos(azi) - y * sin(azi); // Rot Z
    bNormal.y = b.x * sin(azi) + y * cos(azi);

    // Translate and project vertex
    //Without shadow casting:
    //gl_Position = projection * view * vec4(a + aOffset, 1.0);
    //With shadow casting:
    gl_Position = lightSpaceMatrix * vec4(a + aOffset, 1.0);
    // Color passthrough
    bColor = aColor;
};

#shader fragment
#version 460 core
in vec4 bColor;
in vec3 bNormal;
out vec4 FragColor;
uniform vec3 lightDir;
void main()
{
    // For shadow depth buffer, no color information is needed !!!
    if (bColor.a == 0.0) discard; // Alpha 0 still renders the fragment and may overlap non-invisible objects - so discard them
    //float pwr = 50; // 1 = simple directional light, 50 = specular highlight
    //float factor = pow(clamp(dot(lightDir, bNormal), 0, 1), pwr);
    float factor = clamp(dot(lightDir, bNormal), 0, 1) + 0.1;// +0.25;
    FragColor = vec4(factor * bColor.r, factor * bColor.g, factor * bColor.b, bColor.a);
    //FragColor = bColor;
};
