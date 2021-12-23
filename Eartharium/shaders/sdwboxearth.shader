#shader vertex
#version 460 core
//Casts shades on Earth from ShadowMap - use sdwboxearth.shader instead if you cast shadows from ShadowBox
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal; // Spherical normal for insolation
layout(location = 2) in vec3 rNormal; // Real normal for lighting
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in vec4 aTint;

out vec3 sNormal;
out vec4 bTint;
out vec3 FragPos;
out vec3 lNormal;
out vec2 TexCoord;
//out vec4 FragPosLightSpace;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = aPos;     // Position in world
    TexCoord = aTexCoord;
    lNormal = normalize(rNormal);
    gl_Position = projection * view * vec4(aPos, 1.0);
    sNormal = normalize(aNormal);
    bTint = aTint;
};

#shader fragment
#version 460 core
in vec4 bTint;
in vec3 FragPos;
in vec3 lNormal;
in vec3 sNormal;
in vec2 TexCoord;
//in vec4 FragPosLightSpace;

out vec4 FragColor;

uniform vec3 sunDir;
uniform vec3 moonDir;
uniform vec3 lightDir;
uniform float refraction;
uniform float alpha;

uniform sampler2D dayTexture;
uniform sampler2D nightTexture;
uniform samplerCube depthTexture;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
    float dotsun = dot(sunDir, sNormal);
    float dotmoon = dot(moonDir, sNormal);
    float refcos = refraction * -0.014543897652f;
    vec4 nCol = texture(nightTexture, TexCoord);
    vec4 dCol = texture(dayTexture, TexCoord);
    if (dotsun < -0.309016994375f) FragColor = nCol + 0.05;
    else if (dotsun < -0.207911690818f) FragColor = 0.75f * (nCol + 0.1) + 0.25f * dCol;
    else if (dotsun < -0.104528463268f) FragColor = 0.50f * (nCol + 0.1) + 0.50f * dCol;
    else if (dotsun < refcos) FragColor = 0.25f * (nCol + 0.1) + 0.75f * dCol;
    else FragColor = dCol;
    if (dotmoon > -0.00539595f && dotmoon != 0.0f) {
        float avg = FragColor.r * 0.2126 + FragColor.g * 0.7152 + FragColor.b * 0.0722;
        FragColor = vec4(avg, avg, avg, 1.0);
    }
    float lfactor = clamp(dot(lightDir, lNormal), 0.0, 1.0) + 0.2;
    //FragColor = vec4((FragColor.rgb * lfactor), 1.0);

    vec4 base = FragColor;
    vec4 blend = bTint;
    if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
        // Blend mode Screen
        FragColor.r = (1.0 - ((1.0 - base.r) * (1.0 - blend.r)));
        FragColor.g = (1.0 - ((1.0 - base.g) * (1.0 - blend.g)));
        FragColor.b = (1.0 - ((1.0 - base.b) * (1.0 - blend.b)));
    }
    //FragColor = vec4((dCol.rgb), 1.0);

    vec3 lightToFrag = FragPos - lightPos;
    float closestDepth = texture(depthTexture, lightToFrag).r;

    // it is currently in linear range between [0,1]. Re-transform back to original value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and l    xight position
    float currentDepth = length(lightToFrag);
    // now test for shadows
    float bias = 0.0;
    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
    if (dot(lNormal,-lightToFrag)>0) FragColor = vec4((1.0 - (shadow) * 0.6f) * FragColor.rgb * lfactor, 1.0f);
    else FragColor = vec4(FragColor.rgb * lfactor, 1.0f);

    FragColor.a = alpha;
    //FragColor = vec4((1.0 - (shadow) * 0.6f) * FragColor.rgb * lfactor, 1.0f);
    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);
};
