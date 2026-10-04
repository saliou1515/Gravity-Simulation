# Gravity-Simulation

A 2D physics simulation in C++ and OpenGL, starting with bouncing bodies
and growing toward multi-body gravity and, eventually, a rocket flight
simulator.

## Current features
- Multiple circle objects built from a reusable circle struct
- Constant gravity and acceleration, integrated each frame with delta time
- Wall collisions with energy loss on the floor

### In progress
- Circle-to-circle collision (detection, separation, velocity response)
- Mass-based interaction between bodies

(Built with: OpenGL 3.3, GLFW, and glad)
