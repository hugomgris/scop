#shader vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}

#shader fragment
#version 330 core

out vec4 FragColor;

void main() {
    FragColor = vec4(1.0, 0.4, 0.7, 1.0);
}