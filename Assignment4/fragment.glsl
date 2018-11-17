#version 150 core

in vec3 Color;
in vec3 normal;
in vec3 pos;
in vec3 lightDir;
out vec4 outColor;

const float ambient = 0;
void main() {
    vec3 diffuseC = Color*max(dot(-lightDir,normal),0.0);
    vec3 ambC = Color*ambient;
    vec3 viewDir = normalize(-pos); //We know the eye is at (0,0)
    vec3 reflectDir = reflect(viewDir,normal);
    float spec = max(dot(reflectDir,lightDir),0.0);
    if (dot(-lightDir,normal) <= 0.0)spec = 0;
    vec3 specC = .8*vec3(1.0,1.0,1.0)*pow(spec,4);
    vec3 oColor = ambC+diffuseC+specC;
    outColor = vec4(oColor,1);
}



/*#version 150 core

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
    vec3 reflectDirection = reflect(-lightDirection, N);
    vec3 viewDirection = normalize(-position);
    float specular = max(dot(reflectDirection, viewDirection), 0.0);
    
    if(dot(lightDirection, N) <= 0.0) {
        specular = 0;
    }
    
    vec3 specularC = vec3(.8, .8, .8) * pow(specular, 4);
    
    outColor = vec4(ambientC + diffuseC + specularC, 1.0);
}
*/
