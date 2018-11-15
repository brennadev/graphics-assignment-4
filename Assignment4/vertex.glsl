#version 150 core

in vec3 positionIn;
in vec3 inColor;
in vec3 inNormal;

const vec3 inLightDirection = normalize(vec3(1, 0, 0));

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Color;
out vec3 normal;
out vec3 positionOut;
out vec3 eyePosition;
out vec3 lightDirection;


void main() {
    Color = inColor;
    vec4 position4 = view * model * vec4(positionIn, 1.0);
    positionOut = position4.xyz / position4.w;
    vec4 normal4 = transpose(inverse(view * model)) * vec4(inNormal, 0.0);
    normal = normal4.xyz;
    lightDirection = (view * vec4(inLightDirection, 0)).xyz;
    gl_Position = projection * position4;
}
