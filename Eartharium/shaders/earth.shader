#shader vertex
#version 460 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal; // Spherical normal for insolation
layout(location = 2) in vec3 rNormal; // Real normal for lighting
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in vec4 aTint;
out vec2 TexCoord;
out vec3 sNormal;
out vec3 lNormal;
out vec4 bTint;
//uniform mat4 model;
//uniform mat4 view;
//uniform mat4 projection;
uniform mat4 projview;
void main()
{
    //gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
    // note that we read the multiplication from right to left
    //gl_Position = projection * view * vec4(aPos, 1.0);
    gl_Position = projview * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
    sNormal = normalize(aNormal);
    lNormal = normalize(rNormal);
    bTint = aTint;
};

#shader fragment
#version 460 core
in vec2 TexCoord;
in vec3 sNormal; // Spherical normal, for insolation
in vec3 lNormal; // Real normal, for lighting
in vec4 bTint;
out vec4 FragColor;
uniform vec3 sunDir;
uniform vec3 moonDir;
uniform vec3 lightDir;
uniform float refraction;
uniform float alpha;
uniform float twilight;
uniform sampler2D dayTexture;
uniform sampler2D nightTexture;
//uniform sampler2D normalMap;
void main()
{   
    // -0.01454... is the cosine of 90 + 50/60 degrees.
    // Of course this only works for spherical normals to show true insolation.
    // But it can show defective insolation if needed, and works well on ellipsoids.
    // With a bit of work, this can also show feathered insolation which may be more pleasing.
    float dotsun = dot(sunDir, sNormal);
    float dotmoon = dot(moonDir, sNormal);
    //FragColor = vec4(texture(dayTexture, TexCoord).rgb,1.0f);
    //float refraction = 1.0f;                      // 0.0f is off, 1.0f is on - can be passed in from CPU
    float refcos = refraction * -0.014543897652f; // cosine of 90 degres plus 50 arc minutes refraction
    vec4 nCol = texture(nightTexture, TexCoord);
    vec4 dCol = texture(dayTexture, TexCoord);
    FragColor = dCol;
    if (dotsun < refcos && twilight == 0.0f) FragColor = nCol + 0.05;
    else if (dotsun < -0.309016994375f) FragColor = nCol + 0.05;
    else if (dotsun < -0.207911690818f && twilight == 1.0f) FragColor = 0.75f * (nCol + 0.1) + 0.25f * dCol;
    else if (dotsun < -0.104528463268f && twilight == 1.0f) FragColor = 0.50f * (nCol + 0.1) + 0.50f * dCol;
    else if (dotsun < refcos && twilight == 1.0f) FragColor = 0.25f * (nCol + 0.1) + 0.75f * dCol;
    // Make unrefracted insolation greyscale to illustrate the sliver of difference between refracted and unrefracted
    //if (dotprod > 0.0f) {
    //    float avg = FragColor.r * 0.2126 + FragColor.g * 0.7152 + FragColor.b * 0.0722;
    //    FragColor = vec4(avg, avg, avg, 1.0);
    //};
    // (Above greyscale also served as a test for lunar insolation considerations,
    // and dot with that instead of sunDir. Optionally, apply lunar "refraction" model
    // due to the moon being so close it is visible from less of the surface.
    if (dotmoon > -0.00539595f && dotmoon != 0.0f) { //Moon ang size varies from 29.4' to 33.5', avg is 31.45', so cos(90+((50-31.45)/60))
        float avg = FragColor.r * 0.2126 + FragColor.g * 0.7152 + FragColor.b * 0.0722;
        FragColor = vec4(avg, avg, avg, 1.0);
    }
    // Super simple directional light - Like Sun in blender, not accounting for location of source, so AE is shaded 100% even
    //FragColor = vec4(0.5, 0.5, 0.5, 1.0);  // Test with uniform grey sphere
    float lfactor = clamp(dot(lightDir, lNormal),0.0,1.0)+0.2;
    FragColor = vec4((FragColor.rgb * lfactor),1.0);
    vec4 base = FragColor;
    vec4 blend = bTint;
    //if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
    //    // Blend mode Overlay
    //    FragColor.r = (base.r < 0.5 ? (2.0 * base.r * blend.r) : (1.0 - 2.0 * (1.0 - base.r) * (1.0 - blend.r)));
    //    FragColor.g = (base.g < 0.5 ? (2.0 * base.g * blend.g) : (1.0 - 2.0 * (1.0 - base.g) * (1.0 - blend.g)));
    //    FragColor.b = (base.b < 0.5 ? (2.0 * base.b * blend.b) : (1.0 - 2.0 * (1.0 - base.b) * (1.0 - blend.b)));
    //}
    if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
        // Blend mode Screen
        FragColor.r = (1.0 - ((1.0 - base.r) * (1.0 - blend.r)));
        FragColor.g = (1.0 - ((1.0 - base.g) * (1.0 - blend.g)));
        FragColor.b = (1.0 - ((1.0 - base.b) * (1.0 - blend.b)));
    }
    FragColor.a = alpha;

    //if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
    //    // Blend mode Multiply
    //    FragColor.r = (base.r * blend.r);
    //    FragColor.g = (base.g * blend.g);
    //    FragColor.b = (base.b * blend.b);
    //}
    //FragColor = vec4((dCol.rbg), 1.0);
};