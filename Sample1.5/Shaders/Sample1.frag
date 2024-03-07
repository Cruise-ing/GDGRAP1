#version 330 core

uniform sampler2D tex0;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float ambientStr;
uniform vec3 ambientColor;
uniform vec3 cameraPos;
uniform float specStr;
uniform float specPhong;
uniform float brightness;

in vec2 texCoord;
in vec3 normCoord;
in vec3 fragPos;

out vec4 FragColor;

void main () {
    vec3 normal = normalize(normCoord);
    vec3 lightDir = normalize(lightPos - fragPos);

    // Calculate distance from fragment to the light
    float distance = length(lightPos - fragPos);

    // Attenuation to simulate light falloff
    float attenuation = 1.0 / (1.0 + 0.1 * distance + 0.01 * distance * distance);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * attenuation * brightness; // Apply brightness and attenuation

    vec3 ambientCol = ambientColor * ambientStr * attenuation * brightness; // Add brightness and attenuation

    vec3 viewDir = normalize(cameraPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(reflectDir, viewDir), 0.1), specPhong);
    vec3 specColor = spec * specStr * lightColor * attenuation * brightness; // Add brightness and attenuation

    FragColor = vec4(diffuse + ambientCol + specColor, 1.0) * texture(tex0, texCoord);
}
