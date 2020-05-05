# PhySim
Assigment projects for CSE 291H Physical Simulation SP20.

## Configuration & Building
Environment:
- C++ 17 (I use GCC 9.3 on Windows)
- CMake 3.14+
- Python (For CMake configuration)
- OpenGL 4.5+ (CMake will set up glad)
- [ImGui](https://github.com/ocornut/imgui) (CMake will set it up)
- GLFW3 (CMake will search for it locally)
- glm (CMake will search for it locally)

## Common Files (`/commons`)
- `Camera.cpp`: FPS camera
- `Axes.cpp`: An axis frame located at the origin

## Project 1: Solid Mechanics (`/proj1`)
Features:
- `Grid.cpp`: Tetrahedron FEM simulation
    - Mesh generation (`translation`, `rotation`, `cell size`, `grid size`, `density`)
    - Strain-stress relationship (`E`, `nu`)
    - Linear strain-rate damping (`eta`)
- `Particle.cpp`: Forward Euler to compute motion
    - Collision with the ground
        - Friction to avoid sliding
- `GridRendered.cpp`: Mesh rendering
    - Tetrahedra as wirefames plus segments showing velocity/force
    - Phong shading of surfaces
- `main.cpp`: GUI to dynamically change parameters

Show cases:
- [Start/pause/step](docs/proj1/start_pause_step.webm)
    - Simulation is decoupled with redenering
    - You can step one timestep to observe the simulation
- [Phong/wireframe](docs/proj1/wireframe.webm)
    - Red line: force
    - Green line: velocity
- [Mesh parameters](docs/proj1/mesh.webm)
    - Origin translation/rotation
        - It may be unstable when falling on the edge
    - Cell size: x/y/z size of each cell
    - Grid size: x/y/z number of cells
- [Physical parameters](docs/proj1/physics.webm)
    - Young's modulus changes its stiffness
    - Poisson's ratio changes how it expands horizontally when compressd vertically
        - Negative Poisson's ratio has special effect
    - Viscosity control its dissipates energy like fluid
- [Time step](docs/proj1/timestep.webm)
    - High Young's modulus/viscosity require smaller timestep, otherwise it will blow up
    - Insufficient timestep causes interesting oscillation effect

# Project 2: Fluid Dynamics (`/proj2`)
Features:
- `SPHSimulater.cpp`: SPH simulation with box elastic interaction
    - Given a shape indicator function (true when inside the function, false otherwise), sample particles with certain spacing
    - `SPHSimulator::InitializeMass` uses [Jacobi method](https://en.wikipedia.org/wiki/Jacobi_method) to solve initial mass based on spacing and density
- `Integrator.cpp`: Forward euler integration
- `NeighborSearch.cpp`: Spatial hash table
- `SPHRenderer.cpp`: Render box as wire frame and particles as points
Showcases:
- [Fluid](docs/proj2.webm)
    - Starts as a sphere and drops to the box
    - Due to viscosity, looks a bit like honey while interacting with the box
    - Splats at the four corners
    - Finally remain steady