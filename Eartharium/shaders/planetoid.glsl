#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 rNormal; // Real normal for lighting
layout(location = 2) in vec2 aTexCoord;
out vec2 TexCoord;
out vec3 lNormal;
uniform vec3 position;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // note that we read the multiplication from right to left
    gl_Position = projection * view * vec4(aPos + position, 1.0);

    TexCoord = aTexCoord;
    lNormal = normalize(rNormal);
};

#shader fragment
#version 460 core
in vec2 TexCoord;
in vec3 lNormal;
out vec4 FragColor;
uniform vec3 lightDir;
uniform sampler2D tex;

void main()
{   
    FragColor = texture(tex, TexCoord);
    float lfactor = clamp(dot(lightDir, lNormal), 0.0, 1.0) + 0.3;
    FragColor = vec4((FragColor.rgb * lfactor),1.0);
};
