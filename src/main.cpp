#include <glad/glad.h> // opengl function loader
#include <GLFW/glfw3.h> // used for window creation and input handling
#include <glm/glm.hpp> // OpenGL Mathematics library
#include <glm/gtc/matrix_transform.hpp> // transformations
#include <glm/gtc/type_ptr.hpp> // access to the value_ptr function(converts glm types to arrays for opengl)

#include <iostream>
#include <fstream> // will read shader files
#include <sstream> // string stream for file reading
#include <vector>
#include <random>

// CONSTANTS
const unsigned int WINDOW_WIDHT = 1280;
const unsigned int WINDOW_HEIGHT = 720;
const int PARTICLE_COUNT = 100;
//.............................................................

// ═══════════════════════════════════════════════════════════
// CAMERA STATE (Global variables for camera system)
// ═══════════════════════════════════════════════════════════

//camera position in 3D space
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 50.0f); // starting 50 units back on z axis

//Direction the camera is looking at(normalized)
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // looking towards negative z axis

//Up direction of the camera (usually positive y axis)
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// ═══════════════════════════════════════════════════════════
// MOUSE INPUT STATE
// ═══════════════════════════════════════════════════════════

// so notice like in valorant, we start a mouse at side edge doesnt cause our screen to immediately jump so we assume that mouse starts at center everytime.
float lastX = WINDOW_WIDHT / 2.0f; // center of the window
float lastY = WINDOW_HEIGHT / 2.0f; // center of the window
float yaw = -90.0f; // rotation around y axis ,yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f; // rotation around x axis(looking up or down)
bool firstMouse = true; // initially set to true since we havent moved the mouse yet


// ═══════════════════════════════════════════════════════════
// TIMING (for frame-rate independent movement)
// ═══════════════════════════════════════════════════════════
float deltaTime = 0.0f; // time passed between current frame and last frame
float lastFrame = 0.0f; // time of last frame

struct particle{
    glm::vec3 position; // xyz float coordinates
    glm::vec3 color; // RGB color 
    // Note: This struct is tightly packed and matches the layout
    // we'll tell OpenGL about in glVertexAttribPointer()
    // 
    // Memory layout:
    // [position.x][position.y][position.z][color.r][color.g][color.b]
    //  <- 12 bytes (3 floats) -> <- 12 bytes (3 floats) ->
    //  Total: 24 bytes per particle
};
// ═══════════════════════════════════════════════════════════
// FUNCTION PROTOTYPES (declarations - implementations below)
// ═══════════════════════════════════════════════════════════

void framebuffer_size_callback(GLFWwindow* window, int width, int height); // callback (called when window is resized)
void mouse_callback(GLFWwindow* window, double xpos, double ypos); // callback (called when mouse is moved)
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset); // optional callback if we decide to implement scrollwheel
void processInput(GLFWwindow *window); // process keyboard input

std::string loadShaderSource(const char* filePath); // load shader source code from file

GLuint compileShader(const char* source, GLenum shaderType); // compile a shader of given type from source code

GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource); // create shader program from vertex and fragment shader source code

std:: vector<particle> generateParticles(int count); // generate 'count' number of particles with random positions and colors


int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Create a window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello, World!", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return -1;
    }
    
    // Make the window the current context
    glfwMakeContextCurrent(window);
    
    // Initialize GLAD
    if (!gladLoadGL()) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // ... rest of the code ...
}

