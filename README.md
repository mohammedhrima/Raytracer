
# Raytracing Project

This project is a simple raytracer that uses SDL2 for rendering. It displays a scene in a window where you can interact with the view using the mouse and keyboard. The scene can be rotated, zoomed in and out, and navigated using the arrow keys.

## Features

- **Rotate the scene** using the mouse.
- **Zoom in and zoom out** using the mouse scroll wheel.
- **Navigate the scene** using the arrow keys (up, down, left, right).

## Requirements

To build and run this project, you need to have the following installed:

- **c++** (C++ compiler)
- **SDL2** (for rendering the scene)

## Installation & Usage

### 1. Clone the Repository

+ First, clone this repository to your local machine:

```bash
    git clone https://github.com/mohammedhrima/Raytracer.git
```
```bash
    cd Raytracer
```

2. Build the Project
+ Run the following command to compile the project:
```bash
    make
```
+ This will compile the source code, generate object files, and link them to create the executable exec.

3. Run the Program
+ After the build process is complete, run the executable:

```bash
    ./exec
```
+ This will open a window displaying the raytraced scene.

4. Interact with the Scene
+ Mouse: Left-click and drag to rotate the scene.
+ Scroll wheel: Zoom in and out of the scene.
+ Arrow keys: Navigate the scene.
+ Up: Move the scene upward.
+ Down: Move the scene downward.
+ Left: Move the scene left.
+ Right: Move the scene right.
+ ESC: to exit

5. Clean Up
+ To clean the build (remove object files and the executable), run:
```bash
    make clean
```
+ This will remove all compiled object files and the exec executable.
