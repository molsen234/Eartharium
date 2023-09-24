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
out vec4 FragCoord;
uniform mat4 projview;
uniform mat4 world;
uniform mat3 worldnormal;
void main()
{
    FragCoord = world * vec4(aPos, 1.0);
    gl_Position = projview * world * vec4(aPos, 1.0); // projview = projection * view (from CPU)

    TexCoord = aTexCoord;

    // FIX: No need to calculate norm_matrix for every vertex!
    //mat3 norm_matrix = transpose(inverse(mat3(world))); //
    sNormal = normalize(worldnormal * aNormal);
    lNormal = normalize(worldnormal * rNormal);
    bTint = aTint;

    // from moon.glsl
    //sunDir = normalize(norm_matrix * sDir);
    //lightDir = lDir;

};

#shader fragment
#version 460 core
in vec2 TexCoord;
in vec3 sNormal; // Spherical normal, for insolation & inlunation
in vec3 lNormal; // Real mesh normal, for primitice lighting/shading
in vec4 bTint;
in vec4 FragCoord;
out vec4 FragColor;
uniform vec3 sunDir;
uniform vec3 moonDir;
uniform vec3 lightDir;
uniform float refraction;
uniform float alpha;
uniform float twilight;
uniform sampler2D texture1; // Used as dayside texture
uniform sampler2D texture2; // Used as nightside texture
uniform sampler2D texture3; // Used for height map
uniform float sunBumpScale;
uniform float lightBumpScale;
uniform vec4 tintarctics;
uniform vec4 tinttropics;
uniform float obliquity; // Obliquity of Ecliptic, i.e. Earth's axial tilt, used to calculate arctics and tropics

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
    float y_band = 2.0 / 180.0; // Triangles at edge are degenerate, so dFdx & dFdy give odd values
    vec3 my_sNormal;
    if (TexCoord.y < 1.0 - y_band && TexCoord.y > y_band) {
        vec2 s_dHdxy = dHdxy_fwd(TexCoord, sunBumpScale);
        my_sNormal = perturbNormalArb(sNormal, sNormal, s_dHdxy);
    } else {
        my_sNormal = normalize(sNormal);
    }
    //float dotsun = dot(sunDir, sNormal);
    float dotsun = dot(sunDir, my_sNormal);
    float dotmoon = dot(moonDir, sNormal);
    vec4 nCol = texture(texture2, TexCoord); // Night
    vec4 dCol = texture(texture1, TexCoord); // Day
    // -= Calculate insolation and inlunation =-
    FragColor = dCol; // By default, assume dayTexture. The below will modify that if certain conditions are met.
    // -0.01454... is the cosine of 90 + 50/60 degrees (i.e. refraction is 50 arc minutes).
    // Of course this only works for spherical normals to show true insolation.
    // But it can show defective insolation if needed, and works well on ellipsoids.
    // With a bit of work, this can also show feathered insolation which may be more pleasing.
    //float refraction = 1.0f;                      // 0.0f is off, 1.0f is bands, 2.0f could be smooth - can be passed in from CPU
    float refcos = refraction * -0.014543897652f;   // cosine of 90 degrees plus 50 arc minutes refraction
    // Simplest case, if no twilight bands should be rendered, and the location is outside the insolation
    if (dotsun < refcos && twilight == 0.0f) FragColor = nCol +0.05;
    // If twilight bands are enabled, full darkness is 18 degrees (so use cos(72)) from terminator
    else if (dotsun < -0.309016994375f) FragColor = nCol + 0.05;
    // If twilight bands are enabled, darkest twilight band is 12-18 degrees (so use cos(78) from terminator (astronomical twilight)
    else if (dotsun < -0.207911690818f && twilight == 1.0f) FragColor = 0.75f * (nCol + 0.1) + 0.25f * dCol;
    // If twilight bands are enabled, middle twilight band is 6-12 degrees (so use cos(84) from terminator (nautical twilight)
    else if (dotsun < -0.104528463268f && twilight == 1.0f) FragColor = 0.50f * (nCol + 0.1) + 0.50f * dCol;
    // If twilight bands are enabled, lightest twilight band is from refraction or 0 to 6 degrees.
    else if (dotsun < refcos && twilight == 1.0f) FragColor = 0.25f * (nCol + 0.1) + 0.75f * dCol;

    // Make unrefracted insolation greyscale to illustrate the sliver of difference between refracted and unrefracted
    // I would probably make it red or another unnatuaral color, greyscale is used for inlunation below.
    //if (dotprod > 0.0f) {
    //    float avg = FragColor.r * 0.2126 + FragColor.g * 0.7152 + FragColor.b * 0.0722;
    //    FragColor = vec4(avg, avg, avg, 1.0);
    //};

    // (Above greyscale also served as a test for lunar insolation considerations,
    // and dot with that instead of sunDir. Optionally, apply lunar "refraction" model
    // due to the moon being so close it is visible from less of the surface.
    // UPD: Should probably do a proper parallax calculation on CPU.
    if (dotmoon > -0.00539595f && dotmoon != 0.0f) { //Moon ang size varies from 29.4' to 33.5', avg is 31.45', so cos(90+((50-31.45)/60))
        float avg = FragColor.r * 0.2126 + FragColor.g * 0.7152 + FragColor.b * 0.0722;
        FragColor = vec4(avg, avg, avg, 1.0);
    }

    // -= Calculate shading, deliberately toonish =-
    // Super simple directional light - Like Sun in blender, not accounting for location of source, so AE & ER are shaded 100% even
    //FragColor = vec4(0.5, 0.5, 0.5, 1.0);  // Test with uniform grey sphere
    vec2 l_dHdxy = dHdxy_fwd(TexCoord, lightBumpScale);
    vec3 my_lNormal = perturbNormalArb(FragCoord.xyz, lNormal, l_dHdxy);

    //float lfactor = clamp(dot(lightDir, lNormal),0.0,1.0)+0.2;
    float lfactor = clamp(dot(lightDir, my_lNormal),0.0,1.0)+0.2;
    FragColor = vec4((FragColor.rgb * lfactor),1.0);


    // Calculate tinting of (ant)arctic and tropic regions
    if (obliquity != 0.0) {
        vec4 geographictint = vec4(0.0);
        if (TexCoord.y > 1.0 - obliquity) geographictint = tintarctics; 
        if (TexCoord.y < obliquity) geographictint = tintarctics;
        if (TexCoord.y < 0.5 + obliquity && TexCoord.y > 0.5 - obliquity) geographictint = tinttropics;
        vec4 base = FragColor;
        FragColor.r = (1.0 - ((1.0 - base.r) * (1.0 - geographictint.r)));
        FragColor.g = (1.0 - ((1.0 - base.g) * (1.0 - geographictint.g)));
        FragColor.b = (1.0 - ((1.0 - base.b) * (1.0 - geographictint.b)));
    }

    // -= Calculate tinting if tint color is speficied (from vertex shader, interpolated per fragment based on vertex colors)
    //vec4 bTint = vec4(0.211, 0.173, 0.0, 1.0); // Override vertex color for testing (using tropics color)
    vec4 base = FragColor;
    vec4 blend = bTint;
    // Blend mode Overlay commonly used in photo editing software
    //if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
    //    FragColor.r = (base.r < 0.5 ? (2.0 * base.r * blend.r) : (1.0 - 2.0 * (1.0 - base.r) * (1.0 - blend.r)));
    //    FragColor.g = (base.g < 0.5 ? (2.0 * base.g * blend.g) : (1.0 - 2.0 * (1.0 - base.g) * (1.0 - blend.g)));
    //    FragColor.b = (base.b < 0.5 ? (2.0 * base.b * blend.b) : (1.0 - 2.0 * (1.0 - base.b) * (1.0 - blend.b)));
    //}
    // Blend mode Screen commonly used in photo editing software
    if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
        FragColor.r = (1.0 - ((1.0 - base.r) * (1.0 - blend.r)));
        FragColor.g = (1.0 - ((1.0 - base.g) * (1.0 - blend.g)));
        FragColor.b = (1.0 - ((1.0 - base.b) * (1.0 - blend.b)));
    }
    // Blend mode Multiply commonly used in photo editing software
    //if (bTint != vec4(0.0f, 0.0f, 0.0f, 1.0f)) {
    //    FragColor.r = (base.r * blend.r);
    //    FragColor.g = (base.g * blend.g);
    //    FragColor.b = (base.b * blend.b);
    //}

    // After tinting, finally set requested alpha blending.
    // Note that this simply overrides any alpha value set in vertex color 4-tuple as well as any alpha in the texture(s)
    FragColor.a = alpha;

    // Temporarily display only night texture - while playing with Flat Earth skies
    //FragColor = texture(nightTexture, TexCoord); // This version may use alpha from texture

    // For troubleshooting, color code by normals instead of texture, and ignore alpha blending
    //FragColor = vec4(my_sNormal.xyz, 1.0);

    // For troubleshooting, set pure white to simply draw the fragment without relying on any of above parameters
    //FragColor = vec4(1.0,1.0,1.0, 1.0);
};