#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool isPointMode;
uniform float pointSize;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
    
    // Set point size for GL_POINTS rendering
    if (isPointMode) {
        gl_PointSize = pointSize;
    }
}

#shader fragment
#version 330 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec3 lineColor;
uniform vec3 pointColor;  // New uniform for point color
uniform bool isLineMode;
uniform bool isPointMode; // New uniform for point rendering

void main()
{
    if (isPointMode) {
        // Create circular points using gl_PointCoord
        vec2 coord = gl_PointCoord - vec2(0.5);
        float distance = dot(coord, coord);
        
        // Discard pixels outside the circle
        if (distance > 0.25) {
            discard;
        }
        
        // Optional: Add some shading to make points look more 3D
        float intensity = 1.0 - distance * 4.0;
        FragColor = vec4(pointColor * intensity, 1.0);
    } else if (isLineMode) {
        // Simple line rendering with uniform color
        FragColor = vec4(lineColor, 1.0);
    } else {
        // Existing Phong lighting for filled triangles
        vec3 color = vec3(0.6, 0.8, 1.0); // Base color
        
        // Ambient
        float ambientStrength = 0.3;
        vec3 ambient = ambientStrength * lightColor;
        
        // Diffuse
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        
        // Specular
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        vec3 specular = specularStrength * spec * lightColor;
        
        vec3 result = (ambient + diffuse + specular) * color;
        FragColor = vec4(result, 1.0);
    }
}
