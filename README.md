Project Overview

This project is a real-time interactive 3D scene built with OpenGL 4.1, featuring multiple objects, dynamic lighting, shadows, fog, and basic animations. The goal is to demonstrate key graphics programming concepts such as camera control, lighting, shadow mapping, texture mapping, and procedural animation.
Features

Camera Movement:

W / S: Move forward/backward

A / D: Move left/right

Mouse: Look around (pitch/yaw)

Lighting:

Directional light (sun/moon) with shadow mapping

Spotlight attached to camera for a flashlight effect

Object Transformations:

Dog object can be translated, rotated, and scaled using keyboard controls

Animations:

Gull flies around the scene in a circular path

Cat has subtle rotation animation

Rendering Options:

Solid / Wireframe modes (1 / 2)

Smooth / Flat shading (3 / 4)

Fog toggle (5)

Rain toggle (6)

Depth map visualization for shadows (M)
Installation

Clone the repository:

git clone https://github.com/yourusername/OpenGL-Scene-Project.git
cd OpenGL-Scene-Project

Dependencies:

OpenGL 4.1+

GLFW

GLEW (or macOS OpenGL Core)

GLM

stb_image (for textures)

Build Instructions (Linux / macOS):

mkdir build
cd build
cmake ..
make
./OpenGLScene

Build Instructions (Windows with Visual Studio):

Open CMake GUI, configure and generate a Visual Studio solution

Build the solution in Release/Debug mode

Run the executable
Usage

Camera: Move with W, A, S, D; look around with the mouse.

Dog Object:

Press T for translation mode, R for rotation mode, C for scale mode

Use arrow keys or WASD for transformations

Lighting / Rendering:

1 / 2: Solid / Wireframe mode

3 / 4: Smooth / Flat shading

5: Toggle fog

M: Show depth map for shadows

Screenshots: 
<img width="996" height="751" alt="Screenshot 2026-01-19 203551" src="https://github.com/user-attachments/assets/9c95fa5b-dc14-42a2-afb6-38904c500285" />
<img width="1907" height="982" alt="scr" src="https://github.com/user-attachments/assets/c6451f58-0ae6-48a9-b0ba-5b97504ec355" />

Further Development

Add collision detection for objects and camera

Implement skeletal animation for characters

Add post-processing effects: bloom, HDR, SSAO

Introduce dynamic shadows with cascaded shadow maps
