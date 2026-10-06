# Gravity-Simulation

A 2D physics simulation in C++ and OpenGL. It started with bouncing bodies
and now models real multi-body gravity, with a rocket flight simulator as
the long-term goal.

Accurate model of Mars, Phobos, and Deimos in simulation.
<img width="565" height="450" alt="giphy" src="https://github.com/user-attachments/assets/14966c8d-acca-4567-ac34-d1cbd99d1bf3" />

# Current features
- N-body Newtonian gravity: every pair of bodies pulls on each other - F = G·m₁·m₂ / r² and the pulls are summed on each body every frame
- Real SI units (kg, m, s) with two scale constants: pPM (pixels per meter) and timeScale (simulated seconds per real second)
- Bodies stored in a circle vector, so drawing, movement, and gravity are all loops and adding a body is just adding to the vector
- Semi-implicit Euler integration with delta time
- Double-precision physics, with values cast to float only when sent to OpenGL
- Circles built from a reusable struct, drawn from one shared vertex buffer and a small shader

## Also in the code (currently commented out)
- Wall collisions
- Circle-to-circle collision (detection, separation, velocity response)

## Roadmap
- 3D version (next)
- Substeps for accuracy, adjustable time speed and zoom
- Inner solar system
- Rocket flight simulator

(Built with: OpenGL 3.3, GLFW, and glad)
