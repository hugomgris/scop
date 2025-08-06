#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec3 v_Normal;

void main()
{
    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    v_TexCoord = a_TexCoord;
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    
    gl_Position = u_Projection * u_View * vec4(v_Position, 1.0);
}

#shader fragment
#version 330 core

in vec3 v_Position;
in vec2 v_TexCoord;
in vec3 v_Normal;

out vec4 FragColor;

void main()
{
    // Simple color based on position for FDF maps
    vec3 color = vec3(0.8, 0.2, 0.5); // Pink/purple color
    
    // Add some basic lighting based on normal (if available)
    if (length(v_Normal) > 0.0) {
        vec3 lightDir = normalize(vec3(0.5, 0.5, 1.0));
        float diff = max(dot(normalize(v_Normal), lightDir), 0.3);
        color *= diff;
    }
    
    FragColor = vec4(color, 1.0);
}