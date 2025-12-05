# Development Log

## November 8, 2025

### Project Setup & Configuration

**Environment Setup:**
- Installed vcpkg package manager at `C:\Users\ROHAN\vcpkg`
- Installed dependencies via vcpkg:
  - `glfw3:x64-windows` - Window management
  - `glew:x64-windows` - OpenGL extension loader
  - `glm:x64-windows` - Math library for graphics
- Ran `vcpkg integrate install` for system-wide integration

**Compiler Configuration:**
- Identified existing compilers: MSYS2 (ucrt64) and old MinGW
- Decided to use MSVC (Microsoft Visual C++) for better Windows/vcpkg compatibility
- Installed Visual Studio Build Tools 2022 with "Desktop development with C++"
- Configured VS Code to use MSVC compiler via CMake Tools extension

**VS Code Setup:**
- Installed required extensions:
  - CMake Tools (ms-vscode.cmake-tools)
  - CMake (twxs.cmake)
  - C/C++ (ms-vscode.cpptools)
- Created `.vscode/settings.json` with vcpkg toolchain configuration

**Project Structure Created:**
```
N body/
├── src/
│   └── main.cpp
├── shaders/
│   ├── particle.vert
│   └── particle.frag
├── Docs/
│   └── logs.md
├── .vscode/
│   └── settings.json
├── CMakeLists.txt
├── .gitignore
└── README.md
```

**Initial Code:**
- Created test program to verify OpenGL, GLFW, GLEW, and GLM integration
- Successfully compiled and ran test (OpenGL 3.3.0, NVIDIA 581.29 driver)
- Confirmed all dependencies working correctly

**Git Setup:**
- Initialized Git repository
- Created comprehensive `.gitignore` for C++/CMake projects
- Created `README.md` with project description
- Removed old remote and prepared for GitHub push

### Phase 1 Progress: Foundation ✅

**Completed:**
- ✅ Project structure setup
- ✅ CMake configuration
- ✅ Dependency installation (OpenGL, GLFW, GLEW, GLM)
- ✅ Window creation with OpenGL context
- ✅ Basic shader file structure
- ✅ Camera movement system (mouse look + WASD)
- ✅ Particle data structure
- ✅ Random particle generation (100 particles)
- ✅ VAO/VBO setup for rendering
- ✅ Basic vertex and fragment shaders

**Next Steps:**
- Build and test particle rendering system
- Verify camera controls work properly
- Begin Phase 1: Foundation

---

## December 5, 2025

### Dependency Migration & Build System Fixes

**Problem Identification:**
- Had mixed local GLAD files (include/glad, include/KHR, src/glad.c) and vcpkg packages
- Build errors due to missing `glad.c` file
- CMake couldn't find packages when running from terminal (toolchain file issue)

**GLAD Migration to vcpkg:**
- Installed GLAD via vcpkg: `glad:x64-windows`
- Removed local GLAD files:
  - Deleted `include/glad/` folder
  - Deleted `include/KHR/` folder
  - Removed `src/glad.c` from project
- Updated CMakeLists.txt to use `find_package(glad CONFIG REQUIRED)`
- Added `glad::glad` to `target_link_libraries`

**vcpkg Package Verification:**
- Confirmed installed packages via `vcpkg list`:
  - `glad:x64-windows` ✅
  - `glfw3:x64-windows` ✅
  - `glm:x64-windows` ✅
  - `opengl:x64-windows` ✅

**CMake Configuration Understanding:**
- Learned `.vscode/settings.json` already configured with:
  ```json
  "cmake.configureSettings": {
      "CMAKE_TOOLCHAIN_FILE": "C:/Users/ROHAN/vcpkg/scripts/buildsystems/vcpkg.cmake"
  }
  ```
- Understood `CMAKE_TOOLCHAIN_FILE` purpose:
  - Points to vcpkg's auto-generated configuration script
  - Tells CMake where to find vcpkg packages
  - Sets up include paths, library paths, and search locations
  - Not just a directory path, but a complete build environment configuration
  - vcpkg.cmake is auto-generated during vcpkg bootstrap (not manually written)

**Shader Path Resolution:**
- Issue: Shaders not found when running executable
- Problem: Executable looks for shaders relative to its location (build/Debug/)
- Solution: Updated CMakeLists.txt with `add_custom_command`:
  ```cmake
  add_custom_command(TARGET NBodySimulation POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_directory
          ${CMAKE_SOURCE_DIR}/shaders
          $<TARGET_FILE_DIR:NBodySimulation>/shaders
  )
  ```
- This automatically copies shaders to executable directory during build

**Testing & Verification:**
- Created `src/self.cpp` (working galaxy simulation code)
- Tested build system by temporarily switching to `self.cpp` in CMakeLists.txt
- Successfully built and ran - confirmed all dependencies working:
  - Window creation ✅
  - OpenGL rendering ✅
  - GLFW input ✅
  - GLM math ✅
  - GLAD loader ✅
  - Shader loading ✅
- Switched back to `src/main.cpp` for Phase 1 implementation

**Build Commands Used:**
```bash
# Using VS Code CMake Tools (recommended):
Ctrl+Shift+P → "CMake: Delete Cache and Reconfigure"
Ctrl+Shift+P → "CMake: Build"
Shift+F5 → Run without debugging

# Or terminal with toolchain file:
cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/Users/ROHAN/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build .
```

**Final Working Project Structure:**
```
N body/
├── src/
│   ├── main.cpp (Phase 1 tutorial code - to be implemented)
│   └── self.cpp (working galaxy simulation)
├── shaders/
│   ├── particle.vert (updated with distance-based point size)
│   └── particle.frag (Gaussian glow shader)
├── include/ (empty - vcpkg handles all headers)
├── .vscode/
│   └── settings.json (vcpkg toolchain configured)
├── CMakeLists.txt (clean, vcpkg-only dependencies)
└── build/ (auto-generated, in .gitignore)
```

**Key Learnings:**
- vcpkg handles all header and library files - no manual copying needed
- Always use CMake Tools in VS Code or specify toolchain file in terminal
- Shader files need explicit copy command in CMakeLists.txt
- Can test build system with different source files by changing `add_executable()`
- `CMAKE_TOOLCHAIN_FILE` is a configuration script, not just a path

**vcpkg Setup for Other PCs:**
1. Clone and bootstrap vcpkg
2. Install packages: `vcpkg install glfw3:x64-windows glm:x64-windows glad:x64-windows`
3. Run `vcpkg integrate install`
4. Update `.vscode/settings.json` with correct vcpkg path
5. Project builds immediately with no additional setup

**Status:**
- ✅ All dependencies installed via vcpkg
- ✅ Build system fully functional
- ✅ Shader loading working
- ✅ Test simulation runs successfully
- 🎯 Ready to implement Phase 1: 100 particle foundation with camera controls

**Next Steps:**
- Implement `src/main.cpp` with Phase 1 code (100 particles, basic rendering, camera)
- Test all controls (WASD movement, mouse look, Space/Shift vertical)
- Verify particle rendering and colors
- Move to Phase 2: Basic physics simulation


