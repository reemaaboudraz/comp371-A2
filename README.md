# COMP 371 - Computer Graphics
## Assignment 2 - Coloured Pyramid

### Team Members

| Name | Student ID |
| --- | --- |
| Aboudraz Reema | 40253549 |
| Wissem Oumsalem | 40291712 |

## Description

This program uses OpenGL to draw a coloured 3D pyramid.

The pyramid has five vertices and six triangles: four triangles for the sides and two triangles for the square base. Each vertex has a different shade of pink.

The pyramid can be moved, rotated, and scaled using the keyboard.

## Controls

| Key | Action |
| --- | --- |
| `W` | Move up |
| `S` | Move down |
| `A` | Move left |
| `D` | Move right |
| `Q` | Rotate 30° anticlockwise around the z-axis |
| `E` | Rotate 30° clockwise around the z-axis |
| `R` | Increase the scale along the z-axis |
| `F` | Decrease the scale along the z-axis |
| `ESC` | Close the window |

## Libraries

- **GLFW** for the window and keyboard input
- **GLEW** for OpenGL functions
- **GLM** for vectors, matrices, and transformations

## Running on macOS

1. Install the required libraries:

```bash
brew install glfw glew glm
```

2. From the project root folder, compile the program:

```bash
clang++ -std=c++17 -DGL_SILENCE_DEPRECATION src/main.cpp -o pyramid \
  -I/opt/homebrew/include -L/opt/homebrew/lib \
  -lglfw -lGLEW -framework OpenGL
```

3. Run the program:

```bash
./pyramid
```

For an Intel Mac, replace `/opt/homebrew` with `/usr/local`.

## Running on Windows with Visual Studio

1. Install Visual Studio 2022 with the **Desktop development with C++** workload.

2. Download and extract GLFW, GLEW, and GLM. Example folders:

```text
C:\GLFW
C:\GLEW
C:\GLM
```

3. Open the Visual Studio solution and set the configuration to:

```text
Debug
x64
```

4. Open:

```text
Project Properties → Configuration Properties
```

5. Under **VC++ Directories → Include Directories**, add:

```text
C:\GLFW\include
C:\GLEW\include
C:\GLM
```

6. Under **VC++ Directories → Library Directories**, add:

```text
C:\GLFW\lib-vc2022
C:\GLEW\lib\Release\x64
```

7. Under **Linker → Input → Additional Dependencies**, add:

```text
glfw3.lib
glew32.lib
opengl32.lib
```

8. Copy:

```text
C:\GLEW\bin\Release\x64\glew32.dll
```

into the folder containing the compiled `.exe`, usually:

```text
x64\Debug
```

9. Press `Ctrl + F5` to compile and run the program.