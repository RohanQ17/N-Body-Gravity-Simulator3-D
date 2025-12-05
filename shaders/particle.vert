#version 330 core

// Vertex inputs
layout(location = 0) in vec3 aPos;    // particle position (world)
layout(location = 1) in vec3 aColor;  // particle color

// Uniforms
uniform mat4 uMVP;        // projection * view * model (model = identity here)
uniform vec3 uCamPos;     // camera position (world)
uniform float uPointSize; // base size in pixels

// Varyings
out vec3 vColor;

void main() {
    // Project to clip space
    gl_Position = uMVP * vec4(aPos, 1.0);

    // Approximate distance-based size attenuation in world space
    float dist = length(uCamPos - aPos);
    float size = uPointSize / (0.06 * dist + 1.0);
    gl_PointSize = clamp(size, 1.0, 12.0);

    vColor = aColor;
}