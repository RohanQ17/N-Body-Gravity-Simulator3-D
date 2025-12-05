#include <glad/glad.h>        // OpenGL function loading via GLAD
#include <GLFW/glfw3.h>       // Window + context + input
#include <glm/glm.hpp>        // Vector / matrix types
#include <glm/gtc/matrix_transform.hpp> // Matrix helpers (perspective, lookAt)
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <sstream>
#include <filesystem>  // C++17: for current_path()

// Simple particle structure: position + color + velocity (+ mass)
// Note: Only position and color are sent as vertex attributes; velocity/mass stay CPU-side but
// live in the same struct so we can update a single VBO each frame if desired.
struct Particle {
    glm::vec3 pos;    // world-space position
    glm::vec3 color;  // display color (sRGB-ish)
    glm::vec3 vel;    // world-space velocity (simulation)
    float      mass;  // mass (can keep uniform for all if desired)
};

// Read entire text file (shader source)
static std::string loadTextFile(const char* path) {
    std::ifstream f(path, std::ios::in);
    if (!f) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return {};
    }
    std::stringstream ss;
    ss << f.rdbuf();
    return ss.str();
}
   
// Compile a shader of given type; print log on error
static GLuint compile(GLenum type, const char* src) {
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok = 0;
    glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetShaderInfoLog(sh, len, nullptr, log.data());
        std::cerr << "Shader compile error:\n" << log << std::endl;
    }
    return sh;
}

// Link vertex + fragment program
static GLuint makeProgram(const char* vsPath, const char* fsPath) {
    std::string vs = loadTextFile(vsPath);
    std::string fs = loadTextFile(fsPath);

    // Diagnostics: print byte sizes to confirm loading worked
    std::cout << "Loaded " << vsPath << ": " << vs.size() << " bytes\n";
    std::cout << "Loaded " << fsPath << ": " << fs.size() << " bytes\n";

    // Fail fast if either is empty; this prevents confusing GL link errors
    if (vs.empty() || fs.empty()) {
        std::cerr << "Shader source empty. Check working directory and shader copy step." << std::endl;
        return 0;
    }
    GLuint v = compile(GL_VERTEX_SHADER, vs.c_str());
    GLuint f = compile(GL_FRAGMENT_SHADER, fs.c_str());
    GLuint prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        std::string log(len, '\0');
        glGetProgramInfoLog(prog, len, nullptr, log.data());
        std::cerr << "Program link error:\n" << log << std::endl;
    }
    glDeleteShader(v);
    glDeleteShader(f);
    return prog;
}

// Create N random particles in a cube; colors in a light range
// Initialize a simple disk galaxy: particles distributed in a thin disk with
// tangential (orbital) velocities around the origin. We also color by radius
// to get a pleasant gradient.
static std::vector<Particle> makeDiskGalaxy(size_t n) {
    std::vector<Particle> pts;
    pts.reserve(n);
    std::mt19937 rng(std::random_device{}());

    // Radial distribution: more stars toward the center using an exponential profile
    std::uniform_real_distribution<float> u01(0.f, 1.f);
    std::normal_distribution<float> zdist(0.f, 0.2f); // thin disk thickness

    const float Rmax = 8.0f; // disk radius
    const float vScale = 2.0f; // overall velocity scale

    for (size_t i = 0; i < n; ++i) {
        float u = u01(rng);
        // Invert an exponential cdf roughly: r ~ -Rmax * ln(1 - u)
        // Clamp to Rmax
        float r = glm::min(-Rmax * std::log(1.0f - glm::max(u, 1e-4f)), Rmax);
        float a = u01(rng) * 2.0f * 3.14159265f; // angle
        float x = r * std::cos(a);
        float y = r * std::sin(a);
        float z = zdist(rng);

        glm::vec3 pos(x, y, z);

        // Tangential unit vector (perpendicular to radial)
        glm::vec3 radial = glm::normalize(glm::vec3(x, y, 0.0f));
        glm::vec3 tangential = glm::vec3(-radial.y, radial.x, 0.0f);

        // Rough orbital speed that falls off with radius (softened)
        float vtheta = vScale / std::sqrt(r + 0.2f);
        glm::vec3 vel = vtheta * tangential;

        // Color gradient: inner stars bluish/magenta, outer more golden
        float t = glm::clamp(r / Rmax, 0.0f, 1.0f);
        glm::vec3 inner(0.8f, 0.6f, 1.0f); // magenta-ish
        glm::vec3 outer(1.0f, 0.8f, 0.2f); // golden
        glm::vec3 col = glm::mix(inner, outer, t);

        pts.push_back({pos, col, vel, 1.0f});
    }
    return pts;
}

// Simple central-gravity update with softening to keep things stable.
static void stepParticles(std::vector<Particle>& pts, float dt) {
    const float mu = 25.0f;      // G * Mcentral
    const float eps2 = 0.04f;    // softening^2
    const float damp = 0.0f;     // small damping if needed (e.g., 0.001f)
    for (auto& p : pts) {
        glm::vec3 r = p.pos; // from center to particle
        float r2 = glm::dot(r, r) + eps2;
        float r3 = std::pow(r2, 1.5f);
        glm::vec3 a = -(mu * r) / r3; // acceleration toward origin
        p.vel += a * dt;
        p.vel *= (1.0f - damp * dt);
        p.pos += p.vel * dt;
    }
}

int main() {
    // Print current working directory to help diagnose relative paths at runtime
    try {
        std::cout << "CWD: " << std::filesystem::current_path().string() << std::endl;
    } catch (...) {
        // ignore if filesystem throws
    }
    // 1. Initialize GLFW (creates OpenGL context later)
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return -1;
    }

    // Request OpenGL 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 2. Create a window + context
    GLFWwindow* win = glfwCreateWindow(1280, 720, "N-Body Baseline", nullptr, nullptr);
    if (!win) {
        std::cerr << "Window creation failed\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1); // vsync

    // 3. Initialize GLAD after context is current
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n";
        return -1;
    }

    // Basic GL state
    glEnable(GL_PROGRAM_POINT_SIZE); // allow setting gl_PointSize in shader
    glEnable(GL_BLEND);
    // Additive-like blending for glow; alpha is used as weight
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // deep space background

    // 4. Generate particle data (disk galaxy)
    auto particles = makeDiskGalaxy(3000);

    // 5. Create GPU buffers (VAO + VBO)
    GLuint vao = 0, vbo = 0;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Upload all particle structs in one contiguous block
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);

    // Vertex attribute 0: position (first 3 floats)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, pos));
    glEnableVertexAttribArray(0);

    // Vertex attribute 1: color (next 3 floats)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)offsetof(Particle, color));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0); // unbind VAO for safety

    // 6. Build shader program (vertex + fragment)
    GLuint prog = makeProgram("shaders/particle.vert", "shaders/particle.frag");
    if (prog == 0) {
        std::cerr << "Aborting: shader program not created." << std::endl;
        glfwDestroyWindow(win);
        glfwTerminate();
        return -1;
    }
    GLint uMVP = glGetUniformLocation(prog, "uMVP"); // uniform location lookup
    GLint uCamPos = glGetUniformLocation(prog, "uCamPos");
    GLint uPointSize = glGetUniformLocation(prog, "uPointSize");

    // 7. Prepare camera matrices (simple, fixed camera)
    glm::vec3 camPos(0.f, 0.f, 18.f);
    glm::mat4 proj = glm::perspective(glm::radians(45.f), 1280.f / 720.f, 0.1f, 100.f);

    // Animation timing
    double lastTime = glfwGetTime();

    // 8. Main loop
    while (!glfwWindowShouldClose(win)) {
        // Handle simple input: ESC to exit
        if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(win, 1);

    // Integrate physics (fixed-ish timestep clamped for stability)
    double now = glfwGetTime();
    float dt = static_cast<float>(glm::min(now - lastTime, 0.033)); // <= ~30 FPS max step
    lastTime = now;
    stepParticles(particles, dt);

    // Update GPU positions
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, particles.size() * sizeof(Particle), particles.data());

    // Clear frame
        glClear(GL_COLOR_BUFFER_BIT);

        // Build MVP (Model * View * Projection): here model = identity
        glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.f), glm::vec3(0, 1, 0));
        glm::mat4 model(1.f);
        glm::mat4 mvp = proj * view * model;

        // Use program and set uniform
        glUseProgram(prog);
    glUniformMatrix4fv(uMVP, 1, GL_FALSE, &mvp[0][0]);
    glUniform3fv(uCamPos, 1, &camPos[0]);
    glUniform1f(uPointSize, 6.0f); // base size in pixels

        // Bind VAO and draw points
        glBindVertexArray(vao);
        glDrawArrays(GL_POINTS, 0, (GLsizei)particles.size());

        // Present frame + process events
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    // 9. Cleanup GL objects
    glDeleteProgram(prog);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    // 10. Terminate GLFW
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}