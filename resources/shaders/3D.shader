#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool u_isVertexMode;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
    
    // Set point size for vertex visualization
    if (u_isVertexMode) {
        gl_PointSize = 8.0;
    }
}

#shader fragment
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

uniform vec3 u_color;
uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_viewPos;
uniform vec3 u_lineColor;
uniform vec3 u_vertexColor;
uniform bool u_isLineMode;
uniform bool u_isVertexMode;
uniform bool useTexture;
uniform sampler2D u_texture;

out vec4 FragColor;

void main()
{
    vec4 texColor = texture(u_texture, TexCoord);
    
    if (u_isVertexMode) {
        FragColor = vec4(u_vertexColor, 1.0);
        return;
    }
    
    if (u_isLineMode) {
        FragColor = vec4(u_lineColor, 1.0);
        return;
    }
    
    vec3 ambient = 0.2 * u_lightColor;

    vec3 baseColor;
    if (useTexture) {
        baseColor = texColor.rgb;
    }
    
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(u_lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_lightColor;
    
    vec3 viewDir = normalize(u_viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    float specStrength = 0.2f;
    vec3 specular = spec * u_lightColor * specStrength;
    
    vec3 result = (ambient + diffuse + specular) * (useTexture ? baseColor : u_color);
    FragColor = vec4(result, 1.0);
}