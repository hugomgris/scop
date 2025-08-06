#shader vertex
#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}

#shader fragment
#version 330 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D u_texture;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform bool useTexture; // Add this to toggle texture on/off

void main()
{
    // Sample the texture
    vec4 texColor = texture(u_texture, TexCoord);
    
    // Your existing lighting calculations...
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 baseColor;
    if (useTexture) {
        baseColor = texColor.rgb;
    } else {
        baseColor = vec3(0.6, 0.8, 1.0); // Your default color
    }
    
    // Apply lighting to the textured color
    vec3 ambient = 0.3 * baseColor;
    vec3 diffuse = diff * lightColor * baseColor;
    
    vec3 result = ambient + diffuse;
    FragColor = vec4(result, texColor.a);
}