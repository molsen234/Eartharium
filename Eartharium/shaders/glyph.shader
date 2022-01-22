#shader vertex
#version 460 core
//layout(location = 0) in vec3 aPos;      // primitive geometry
//layout(location = 1) in vec3 aNormal;
//layout(location = 2) in vec2 aUV;
//
//layout(location = 3) in vec4 aColor;    // primitive instances
//layout(location = 4) in vec3 aOffset;
//layout(location = 5) in vec3 aDir;
//layout(location = 6) in vec3 aScale;
//layout(location = 7) in float aRot;

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUV;
layout(location = 2) in vec4 aColor;

uniform mat4 view;
uniform mat4 projection;

out vec2 bUV;
out vec4 bColor;
//out vec3 bNormal;

void main()
{
//    // Calculate rotation angles from direction vector
//    //  This will take Y axis to aDir
//    float azi = atan(-aDir.x, aDir.y);
//    float ele = atan(aDir.z, sqrt(aDir.x * aDir.x + aDir.y * aDir.y));
//
//    // Vertex scaling
//    vec3 c = vec3(aPos.x * aScale.x, aPos.y * aScale.y, aPos.z * aScale.z);
//
//    // Vertex rotation
//    // Axis rotation
//    float rx = c.x * cos(aRot) - c.z * sin(aRot);
//    float rz = c.x * sin(aRot) + c.z * cos(aRot);
//    // Orientation rotation
//    vec3 a;
//    float y = c.y * cos(ele) - rz * sin(ele); // Rot X
//    a.z = c.y * sin(ele) + rz * cos(ele);
//    a.x = rx * cos(azi) - y * sin(azi);       // Rot Z
//    a.y = rx * sin(azi) + y * cos(azi);
//
//    // Normal rotation
//    vec3 b = normalize(aNormal);
//    // Axis rotation
//    float sx = b.x * cos(aRot) - b.z * sin(aRot);
//    float sz = b.x * sin(aRot) + b.z * cos(aRot);
//    // Orientation rotation
//    y = b.y * cos(ele) - sz * sin(ele);       // Rot X
//    bNormal.z = b.y * sin(ele) + sz * cos(ele);
//    bNormal.x = sx * cos(azi) - y * sin(azi); // Rot Z
//    bNormal.y = sx * sin(azi) + y * cos(azi);
//
//    // Translate and project vertex
//    gl_Position = projection * view * vec4(a + aOffset, 1.0);
    gl_Position = projection * view * vec4(aPos, 1.0);

    // Color & UV passthrough
    bColor = aColor;
    bUV = aUV;
};

#shader fragment
#version 460 core
in vec4 bColor;
//in vec3 bNormal;
in vec2 bUV;
out vec4 FragColor;
uniform vec3 lightDir;
uniform sampler2D tex;

// Computation of the median value using minima and maxima
float median(float a, float b, float c) {
    return max(min(a, b), min(max(a, b), c));
}

void main() {
    //if (bColor.a == 0.0) discard
    vec4 bgColor = vec4(0.0, 0.0, 0.0, 0.0);
    vec4 fgColor = bColor; // vec4(1.0, 0.4, 0.4, 1.0);
    // Bilinear sampling of the distance field
    vec3 s = texture2D(tex, bUV).rgb;

    // The anti-aliased measure of how "inside" the fragment lies - from msdf thesis
    //float d = median(s.r, s.g, s.b) - 0.5;
    //float w = clamp(d / fwidth(d) + 0.5, 0.0, 1.0);
    //FragColor = mix(bgColor, fgColor, w);

    // With outline, from https://stackoverflow.com/questions/26155614/outlining-a-font-with-a-shader-and-using-distance-field
    const float smoothing = 1.0 / 16.0;
    const float outlineWidth = 6.0 / 16.0;
    const float outerEdgeCenter = 0.5 - outlineWidth;
    float d = median(s.r, s.g, s.b); // -0.5; outlined version does not need offset
    float alpha = smoothstep(outerEdgeCenter - smoothing, outerEdgeCenter + smoothing, d);
    float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, d);
    FragColor = vec4(mix(bgColor.rgb, fgColor.rgb, border), alpha);
}


//float pxRange = 200;
//
//float median(float r, float g, float b) {
//    return max(min(r, g), min(max(r, g), b));
//}
//
//float screenPxRange() {
//    vec2 unitRange = vec2(pxRange) / vec2(212,212);
//    vec2 screenTexSize = vec2(1.0) / fwidth(bUV);
//    return max(0.5 * dot(unitRange, screenTexSize), 1.0);
//}
//
//void main()
//{
//    if (bColor.a == 0.0) discard; // Alpha 0 still renders the fragment and may overlap non-invisible objects - so discard them
//    //float pwr = 50; // 1 = simple directional light, 50 = specular highlight
//    //float factor = pow(clamp(dot(lightDir, bNormal), 0, 1), pwr);
//    vec4 col = texture(tex, bUV);
//    float factor = clamp(dot(lightDir, bNormal), 0, 1) + 0.1;// +0.25;
//    //FragColor = vec4(factor * bColor.r, factor * bColor.g, factor * bColor.b, bColor.a);
//    //FragColor = col;
//    vec4 bgColor = vec4(0.0, 0.0, 0.0, 0.0);
//    vec4 fgColor = vec4(1.0, 0.4, 0.4, 1.0);
//    vec3 msd = texture(tex, bUV).rgb;
//    float sd = median(msd.r, msd.g, msd.b);
//    float screenPxDistance = screenPxRange() * (sd - 0.5);
//    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
//    FragColor = mix(bgColor, fgColor, opacity);
//};
