#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 rNormal; // Real normal for lighting
layout(location = 2) in vec2 aTexCoord;
out vec2 TexCoord;
out vec3 lNormal;
uniform mat4 projview;
uniform mat4 world;
uniform mat3 worldnormal;
void main()
{
    gl_Position = projview * world * vec4(aPos, 1.0);

    TexCoord = aTexCoord;
    lNormal = normalize(worldnormal * rNormal);

    // Must also take sun direction and apply shade accordin
};

#shader fragment
#version 460 core
in vec2 TexCoord;
in vec3 lNormal;
out vec4 FragColor;
uniform vec3 lightDir;
uniform sampler2D tex;
// Will take 3 texture atlas indices: daytexture, ringtexture, nighttexture
// Wait no. That is not how it works, think of the rings, they are a different object.
// The uv texture coordinates will automatically point to the right region of the atlas, and maybe ignore nighttexture for now.
void main()
{   
    FragColor = texture(tex, TexCoord);
    float lfactor = clamp(dot(lightDir, lNormal) + 0.1, 0.0, 1.0);
    FragColor = vec4((FragColor.rgb * lfactor),FragColor.a);
    //FragColor = vec4((FragColor.rgb * lfactor),1.0);
    //FragColor = vec4(1.0);
};
