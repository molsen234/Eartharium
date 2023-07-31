#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal; // Spherical normal for insolation
layout(location = 2) in vec3 rNormal; // Real normal for lighting
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in vec4 aTint;
out vec2 TexCoord;
out vec4 FragCoord;
out vec3 sNormal;
out vec3 lNormal;
out vec4 bTint;
out vec3 lightDir;
uniform mat4 world;
uniform mat4 projview;
uniform vec3 lDir;

void main() {
    FragCoord = world * vec4(aPos, 1.0);
    gl_Position = projview * world * vec4(aPos, 1.0);
    TexCoord = aTexCoord;

    mat3 norm_matrix = transpose(inverse(mat3(world)));
    sNormal = normalize(norm_matrix * aNormal);
    lNormal = normalize(norm_matrix * rNormal);

    lightDir = lDir;
    bTint = aTint;
};

#shader fragment
#version 460 core

in vec2 TexCoord;
in vec4 FragCoord;
in vec3 sNormal; // Spherical normal, for insolation
in vec3 lNormal; // Real normal, for scene lighting
in vec4 bTint;
in vec3 lightDir;
out vec4 FragColor;
uniform sampler2D texture1; // Daylight texture

void main() {
    vec4 dCol = texture(texture1, TexCoord);
    float lfactor = clamp(dot(lightDir, lNormal),0.0,1.0); //+0.1;
    FragColor = vec4((dCol.rgb * lfactor),1.0);
    FragColor = dCol; // lfactor is 0.0 on inside due to clamping
};
