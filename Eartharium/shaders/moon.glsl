#shader vertex
#version 460 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal; // Spherical normal for insolation
layout(location = 2) in vec3 rNormal; // Real normal for lighting
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in vec4 aTint;
out vec2 TexCoord;
out vec4 FragCoord;
out vec3 sNormal;
out vec3 lNormal;
out vec4 bTint;
out vec3 sunDir;
out vec3 lightDir;
uniform mat4 world;
uniform mat4 projview;
uniform vec3 sDir;
uniform vec3 lDir;

void main() {
    FragCoord = world * vec4(aPos, 1.0);
    gl_Position = projview * world * vec4(aPos, 1.0);
    TexCoord = aTexCoord;

    mat3 norm_matrix = transpose(inverse(mat3(world)));
    sNormal = normalize(norm_matrix * aNormal);
    lNormal = normalize(norm_matrix * rNormal);
    sunDir = normalize(norm_matrix * sDir);

    lightDir = lDir;
    bTint = aTint;
};

#shader fragment
#version 460 core

in vec2 TexCoord;
in vec4 FragCoord;
in vec3 sNormal; // Spherical normal, for insolation
in vec3 lNormal; // Real normal, for scene lighting
in vec4 bTint;
in vec3 sunDir;
in vec3 lightDir;
out vec4 FragColor;
uniform sampler2D texture1; // Daylight texture
//uniform sampler2D texture2;
uniform sampler2D texture3; // Height map for bump-mapping
uniform float sunBumpScale;
uniform float lightBumpScale;

float map(float value, float min1, float max1, float min2, float max2) {
  return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

vec2 dHdxy_fwd(vec2 vUv, float bumpScale) {
	vec2 dSTdx = dFdx( vUv );
	vec2 dSTdy = dFdy( vUv );
	float Hll = bumpScale * texture2D( texture3, vUv ).x;
	float dBx = bumpScale * texture2D( texture3, vUv + dSTdx ).x - Hll;
	float dBy = bumpScale * texture2D( texture3, vUv + dSTdy ).x - Hll;
return vec2( dBx, dBy );
}

vec3 perturbNormalArb( vec3 surf_pos, vec3 surf_norm, vec2 dHdxy ) {
	vec3 vSigmaX = dFdx( surf_pos );
	vec3 vSigmaY = dFdy( surf_pos );
	vec3 vN = surf_norm;		// normalized
	vec3 R1 = cross( vSigmaY, vN );
	vec3 R2 = cross( vN, vSigmaX );
	float fDet = dot( vSigmaX, R1 );
	vec3 vGrad = sign( fDet ) * ( dHdxy.x * R1 + dHdxy.y * R2 );
	return normalize( abs( fDet ) * surf_norm - vGrad );
}

void main() {
    vec2 s_dHdxy = dHdxy_fwd(TexCoord, sunBumpScale);
    vec3 my_sNormal = perturbNormalArb(FragCoord.xyz, sNormal, s_dHdxy);
    //float dotsun = dot(sunDir, sNormal);
    float dotsun = dot(sunDir, my_sNormal);
    //vec4 dCol = texture(texture1, TexCoord);
    vec4 dCol = texture(texture1, TexCoord);
    FragColor = dCol;
    // Slightly feather the terminator for a more visually pleasing result
    float terminator_threshold = 0.003;
    // Instead of a night side texture, just multiply dark side with this factor
    float darkside_factor = 0.25;
    if (dotsun < 0.0) FragColor = darkside_factor * dCol; // Dark side is slightly illuminated from earthshine
    else if (dotsun > 0.0 && dotsun <= terminator_threshold) FragColor = map(dotsun, 0.0,terminator_threshold, darkside_factor,1.0) * dCol;

    vec2 l_dHdxy = dHdxy_fwd(TexCoord, lightBumpScale);
    vec3 my_lNormal = perturbNormalArb(FragCoord.xyz, lNormal, l_dHdxy);

    //FragColor = vec4(1.0);
    //float lfactor = clamp(dot(lightDir, lNormal),0.0,1.0); //+0.1;
    float lfactor = clamp(dot(lightDir, my_lNormal),0.0,1.0); //+0.1;
    FragColor = vec4((FragColor.rgb * lfactor),1.0);

    // For troubleshooting, quickly view the perturbed lighting normals
    //FragColor = vec4(my_lNormal.xyz, 1.0);

    //vec4 base = FragColor;
    //vec4 blend = bTint;
    //if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
    //    // Blend mode Overlay
    //    FragColor.r = (base.r < 0.5 ? (2.0 * base.r * blend.r) : (1.0 - 2.0 * (1.0 - base.r) * (1.0 - blend.r)));
    //    FragColor.g = (base.g < 0.5 ? (2.0 * base.g * blend.g) : (1.0 - 2.0 * (1.0 - base.g) * (1.0 - blend.g)));
    //    FragColor.b = (base.b < 0.5 ? (2.0 * base.b * blend.b) : (1.0 - 2.0 * (1.0 - base.b) * (1.0 - blend.b)));
    //}

    //if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) { // This is the one used in earth.glsl
    //    // Blend mode Screen
    //    FragColor.r = (1.0 - ((1.0 - base.r) * (1.0 - blend.r)));
    //    FragColor.g = (1.0 - ((1.0 - base.g) * (1.0 - blend.g)));
    //    FragColor.b = (1.0 - ((1.0 - base.b) * (1.0 - blend.b)));
    //}

    //if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
    //    // Blend mode Multiply
    //    FragColor.r = (base.r * blend.r);
    //    FragColor.g = (base.g * blend.g);
    //    FragColor.b = (base.b * blend.b);
    //}
};
