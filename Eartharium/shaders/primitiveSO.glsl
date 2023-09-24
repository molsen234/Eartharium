#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;      // primitive geometry
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

layout(location = 3) in vec4 aColor;    // primitive instances
layout(location = 4) in vec3 aOffset;
layout(location = 5) in vec3 aDir;
layout(location = 6) in vec3 aScale;
layout(location = 7) in float aRot;

//uniform mat4 model;
uniform mat4 projview;
uniform mat4 world;
uniform vec3 lightDir;

out vec4 bColor;
out vec3 bNormal;
out vec3 lDir;

void main()
{   
    // Calculate rotation angles from direction vector
    //  This will take Y axis to aDir
    float azi = atan(-aDir.x, aDir.y);
    float ele = atan(aDir.z, sqrt(aDir.x * aDir.x + aDir.y * aDir.y));

    // Vertex scaling
    vec3 c = vec3(aPos.x * aScale.x, aPos.y * aScale.y, aPos.z * aScale.z);

    // Vertex rotation
    // Axis rotation
    float rx = c.x * cos(aRot) - c.z * sin(aRot);
    float rz = c.x * sin(aRot) + c.z * cos(aRot);
    // Orientation rotation
    vec3 a;
    float y = c.y * cos(ele) - rz * sin(ele); // Rot X
    a.z = c.y * sin(ele) + rz * cos(ele);
    a.x = rx * cos(azi) - y * sin(azi);       // Rot Z
    a.y = rx * sin(azi) + y * cos(azi);

    // Normal rotation
    vec3 b = normalize(aNormal);
    // Axis rotation
    float sx = b.x * cos(aRot) - b.z * sin(aRot);
    float sz = b.x * sin(aRot) + b.z * cos(aRot);
    // Orientation rotation
    y = b.y * cos(ele) - sz * sin(ele);       // Rot X
    bNormal.z = b.y * sin(ele) + sz * cos(ele);
    bNormal.x = sx * cos(azi) - y * sin(azi); // Rot Z
    bNormal.y = sx * sin(azi) + y * cos(azi);

    lDir = lightDir;
    //lDir = transpose(inverse(mat3(world))) * lightDir;

    // Translate and project vertex
    gl_Position = projview * world * vec4(a + aOffset, 1.0);
    bNormal = transpose(inverse(mat3(world))) * bNormal;

    // Color passthrough
    bColor = aColor;
    //bColor = vec4(bNormal, 1.0); //aColor;
};

#shader fragment
#version 460 core
in vec4 bColor;
in vec3 bNormal;
in vec3 lDir;
out vec4 FragColor;
//uniform vec3 lightDir;
void main()
{
    if (bColor.a == 0.0) discard; // Alpha 0 still renders the fragment and may overlap non-invisible objects - so discard them
    //float pwr = 50; // 1 = simple directional light, 50 = specular highlight
    //float factor = pow(clamp(dot(lightDir, bNormal), 0, 1), pwr);
    float factor = clamp(dot(lDir, bNormal), 0, 1); //+0.1; 
    FragColor = vec4(factor * bColor.r, factor * bColor.g, factor * bColor.b, bColor.a);
    //FragColor = bColor;
};
