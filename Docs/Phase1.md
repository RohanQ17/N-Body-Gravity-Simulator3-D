┌─────────────────────────────────────────────────────────────┐
│                         MAIN FLOW                            │
└─────────────────────────────────────────────────────────────┘

1. INITIALIZATION
   ├─ Initialize GLFW (window system)
   ├─ Create window with OpenGL context
   ├─ Initialize GLAD (load OpenGL functions)
   ├─ Configure OpenGL (depth test, point size)
   └─ Set up callbacks (mouse, keyboard, window resize)

2. SHADER SETUP
   ├─ Load vertex shader source from file
   ├─ Load fragment shader source from file
   ├─ Compile both shaders
   ├─ Link into shader program
   └─ Error checking at each step

3. DATA GENERATION
   ├─ Create particle structure (position + color)
   ├─ Generate 100 random particles
   │  ├─ Random positions in 3D space (-20 to +20)
   │  └─ Random bright colors
   └─ Store in std::vector

4. GPU SETUP
   ├─ Create VAO (Vertex Array Object)
   ├─ Create VBO (Vertex Buffer Object)
   ├─ Bind VAO
   ├─ Upload particle data to GPU
   ├─ Configure vertex attributes
   │  ├─ Attribute 0: Position (vec3)
   │  └─ Attribute 1: Color (vec3)
   └─ Unbind VAO

5. RENDER LOOP (runs every frame)
   ├─ Calculate delta time (for smooth movement)
   ├─ Process input (WASD, mouse, Space, Shift, ESC)
   ├─ Clear screen (color + depth buffer)
   ├─ Activate shader program
   ├─ Calculate matrices
   │  ├─ Projection (perspective camera)
   │  ├─ View (camera position/rotation)
   │  └─ Model (identity - no transformation)
   ├─ Send matrices to shader as uniforms
   ├─ Bind VAO
   ├─ Draw particles (GL_POINTS)
   ├─ Swap buffers (display to screen)
   └─ Poll events (handle input)

6. CLEANUP
   ├─ Delete VAO, VBO
   ├─ Delete shader program
   └─ Terminate GLFW

   CPU Side                     GPU Side
────────                     ────────

Particle Struct
  ├─ position (vec3)
  └─ color (vec3)
       │
       │ glBufferData()
       ▼
   VBO (GPU memory)
       │
       │ glVertexAttribPointer()
       ▼
   VAO (describes layout)
       │
       │ glDrawArrays(GL_POINTS)
       ▼
   Vertex Shader
       │ transforms position
       │ passes color through
       ▼
   Rasterizer (creates point sprites)
       ▼
   Fragment Shader
       │ makes points circular
       │ applies color
       ▼
   Framebuffer → Screen