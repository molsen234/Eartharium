#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;      // primitive geometry
layout(location = 1) in vec2 aUV;

uniform mat4 projview;
uniform mat4 world;
out vec2 TexCoord;

void main()
{   
    gl_Position = projview * world * vec4(aPos, 1.0); // projview = projection * view (from CPU)
    //gl_Position = projview * vec4(aPos, 1.0); // projview = projection * view (from CPU)
    TexCoord = aUV;
};


#shader fragment
#version 460 core

in vec2 TexCoord;

uniform sampler2D tex;
uniform float alpha;

out vec4 FragColor;

void main()
{
    //FragColor = vec4(1.0,1.0,1.0, 1.0);
    vec3 tx = texture(tex, TexCoord).rgb;
    FragColor = vec4(tx.rgb, alpha);
    //FragColor = vec4(1.0-tx.r, 1.0-tx.g, 1.0-tx.b, alpha);
};
