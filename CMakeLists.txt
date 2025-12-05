cmake_minimum_required(VERSION 3.15)
project(NBodySimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find packages (vcpkg handles everything)
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)

# Add executable - Using self.cpp for testing
add_executable(NBodySimulation 
    src/main.cpp
)

# Link libraries (vcpkg provides everything automatically)
target_link_libraries(NBodySimulation PRIVATE
    OpenGL::GL
    glfw
    glm::glm
    glad::glad
)

# Copy shaders to the executable directory (handles Debug/Release)
add_custom_command(TARGET NBodySimulation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/shaders
        $<TARGET_FILE_DIR:NBodySimulation>/shaders
)
