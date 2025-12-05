#version 330 core

in vec3 vColor;          // interpolated from vertex shader (for points same value)
out vec4 FragColor;      // final pixel color

void main() {
    // gl_PointCoord gives 2D coords in [0,1] inside the point sprite
    vec2 p = gl_PointCoord - vec2(0.5);
    float r = length(p);

    // Discard pixels outside a circle (ensures round points)
    if (r > 0.5) discard;

    // Gaussian falloff for a glow look
    // Map r in [0, 0.5] to intensity with a smooth center
    float sigma = 0.20; // controls softness
    float intensity = exp(-(r*r) / (2.0 * sigma * sigma));

    // Slight color boost toward the center
    vec3 col = vColor * (0.6 + 0.4 * intensity);

    // Use intensity as alpha weight; with blending (SRC_ALPHA, ONE) this looks glowy
    FragColor = vec4(col * intensity, intensity);
}