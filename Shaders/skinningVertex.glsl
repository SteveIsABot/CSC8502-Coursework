#version 400 core

uniform mat4 modelMatrix;
uniform mat4 projMatrix;
uniform mat4 viewMatrix;

in vec3 position;
in vec2 texCoord;
in vec4 jointWeights;
in ivec4 jointIndices;

uniform mat4 joints[128];

out Vertex {
    vec2 texCoord;
} OUT;

void main(void) {

    vec4 localPos = vec4(position, 1.0f);
    vec4 skelPos = vec4(0, 0, 0, 0);

    for(int i = 0; i < 4; i++) {
        int jointIndex = jointIndices[i];
        float jointWeight = jointWeights[i];

        skelPos += joints[jointIndex] * localPos * jointWeight;
    }

    mat4 mvp = projMatrix * viewMatrix * modelMatrix;
    gl_Position = mvp * vec4(skelPos.xyz, 1.0);
    OUT.texCoord = texCoord;
}