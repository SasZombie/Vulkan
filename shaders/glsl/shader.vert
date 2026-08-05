#version 450

layout(push_constant) uniform PushConstants {
    mat4 mvp; 
} pc;

// Hardcoded triangle data (no vertex buffer needed!)
vec2 positions[3] = vec2[](
    vec2( 0.0, -0.5), // Top
    vec2( 0.5,  0.5), // Right
    vec2(-0.5,  0.5)  // Left
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0), // Red
    vec3(0.0, 1.0, 0.0), // Green
    vec3(0.0, 0.0, 1.0)  // Blue
);

// Output passed down to the fragment shader
layout(location = 0) out vec3 fragColor;

void main() {
    gl_Position = pc.mvp * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}