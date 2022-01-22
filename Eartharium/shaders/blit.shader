#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    TexCoords = aTexCoords;
}
#shader fragment
#version 460 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;
//uniform sampler2DMS screenTexture;

//const int texSamples = 4;

// From: https://stackoverflow.com/questions/42878216/opengl-how-to-draw-to-a-multisample-framebuffer-and-then-use-the-result-as-a-n
//vec4 textureMultisample(sampler2DMS sampler, ivec2 coord) {
//    vec4 color = vec4(0.0);
//    for (int i = 0; i < texSamples; i++)
//        color += texelFetch(sampler, coord, i);
//    color /= float(texSamples);
//    return color;
//}

void main() {
    //vec2 uv = vec2(0.5, 0.5); // normalized coordinates
    //ivec2 texSize = textureSize(screenTexture);
    //ivec2 texCoord = ivec2(uv * texSize);
    //FragColor = textureMultisample(screenTexture, texCoord);

    FragColor = texture(screenTexture, TexCoords);
}