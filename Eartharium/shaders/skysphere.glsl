#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform float gsid; // Greenwich hour angle

void main()
{
    TexCoords = aUV;
    vec3 bPos;
    bPos.x = aPos.x * cos(gsid) - aPos.y * sin(gsid);
    bPos.y = aPos.x * sin(gsid) + aPos.y * cos(gsid);
    bPos.z = aPos.z;
    gl_Position = projection * view * vec4(bPos, 1.0);
};

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D skytexture;

void main()
{
    FragColor = texture(skytexture, TexCoords);
    FragColor.a = 0.8;
};
