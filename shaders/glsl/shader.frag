#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(set = 0, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    // Sample texture pixel at fragTexCoord
    // outColor = vec4(fragColor, 1.0);
    // outColor = vec4(1.f);
    outColor = texture(texSampler, fragTexCoord);
}