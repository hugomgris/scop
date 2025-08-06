#shader fragment
#version 330 core

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 u_lightPos;
uniform vec3 u_lightColor;
uniform vec3 u_viewPos;
uniform vec3 u_lineColor;
uniform bool u_isLineMode;

void main()
{
    if (u_isLineMode) {
        FragColor = vec4(u_lineColor, 1.0);
    } else {
        // Base reflective color (slight tint)
        vec3 baseColor = vec3(0.9, 0.9, 1.0);
        
        // Minimal ambient - mirrors don't self-illuminate much
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * u_lightColor;

        // Reduce diffuse component significantly
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(u_lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = 0.2 * diff * u_lightColor; // Much lower diffuse

        // Maximize specular component
        float specularStrength = 1.0; // Maximum specular
        vec3 viewDir = normalize(u_viewPos - FragPos);
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128); // Higher shininess
        vec3 specular = specularStrength * spec * u_lightColor;
        
        // Specular dominates the final result
        vec3 result = ambient * baseColor + diffuse * baseColor + specular;
        FragColor = vec4(result, 1.0);
    }
}