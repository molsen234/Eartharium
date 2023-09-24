#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;      // primitive geometry

//uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//out vec4 bColor;

void main()
{
    // Translate and project vertex
    gl_Position = projection * view * vec4(aPos, 1.0);

    // Color passthrough
    //bColor = aColor;
};

#shader fragment
#version 460 core
//in vec4 bColor;
out vec4 FragColor;
void main()
{
    //FragColor = bColor;
    FragColor = vec4(1.0, 0.5, 0.5, 1.0);
};
