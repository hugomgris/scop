#shader vertex
#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

void main()
{
    TexCoord = aTexCoord;
    gl_Position = vec4(aPos, 0.0, 1.0);
}

#shader fragment
#version 330 core

in vec2 TexCoord;

uniform sampler2D u_screenTexture;
uniform float u_time;
uniform vec2 u_resolution;
uniform bool u_enableCRT;

out vec4 FragColor;

// CRT Effect Parameters
const float scanlineIntensity = 0.02;
const float vignetteStrength = 0.3;
const float curvature = 5;
const float aberrationStrength = 0.003;

vec2 curveRemapUV(vec2 uv) {
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs(uv.yx) / vec2(curvature, curvature);
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

vec3 scanlineEffect(vec3 color, vec2 uv) {
    float scanline = sin(uv.y * u_resolution.y * 0.75) * scanlineIntensity;
    return color - scanline;
}

vec3 vignetteEffect(vec3 color, vec2 uv) {
    float vignette = distance(uv, vec2(0.5));
    vignette = smoothstep(0.0, 0.7, vignette);
    return color * (1.0 - vignette * vignetteStrength);
}

vec3 chromaticAberration(sampler2D tex, vec2 uv) {
    vec2 center = vec2(0.5);
    vec2 direction = normalize(uv - center);
    vec2 offset = direction * aberrationStrength;
    
    float r = texture(tex, uv + offset).r;
    float g = texture(tex, uv).g;
    float b = texture(tex, uv - offset).b;
    
    return vec3(r, g, b);
}

void main()
{
    vec2 uv = TexCoord;
    vec3 color;
    
    if (u_enableCRT) {
        uv = curveRemapUV(uv);
        
        if (uv.x < 0.0 || uv.x > 1.0 || uv.y < 0.0 || uv.y > 1.0) {
            FragColor = vec4(0.0, 0.0, 0.0, 1.0);
            return;
        }
        
        color = chromaticAberration(u_screenTexture, uv);
        
        color = scanlineEffect(color, uv);
        
        color = vignetteEffect(color, uv);
        
        color += 0.05 * sin(u_time * 1.5) * 0.3;
        
        color.g *= 1.02;
        
    } else {
        color = texture(u_screenTexture, uv).rgb;
    }
    
    FragColor = vec4(color, 1.0);
}
