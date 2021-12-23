#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{   
    // Translate and project vertex
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoords = aTexCoords;
};

#shader fragment
#version 460 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    // Sample texture here
    FragColor = texture(tex, TexCoords);
    //FragColor = vec4(TexCoords.x, TexCoords.y, 0.0, 1.0);
    //FragColor = vec4(1.0, 1.0, 1.0, 1.0);
};
