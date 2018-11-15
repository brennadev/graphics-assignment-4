#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 position;
in vec3 eyePosition;
in vec3 lightDirection;

out vec4 outColor;

const float ambient = 0.3;

void main() {
    vec3 N = normalize(normal);
    vec3 diffuseC = Color * max(dot(lightDirection, N), 0.0);
    vec3 ambientC = Color * ambient;
    vec3 reflectDirection = reflect(-lightDir, N);
    vec3 viewDirection = normalize(-position);
    float specular = max(dot(reflectDirection, viewDirection), 0.0);
    
    if(dot(lightDirection, N) <= 0.0) {
        specular = 0;
    }
    
    vec3 specularC = vec3(.8, .8, .8) * pow(specular, 4);
    
    outColor = vec4(ambientC + diffuseC + specularC, 1.0);
}
