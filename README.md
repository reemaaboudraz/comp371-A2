# COMP 371 - Computer Graphics - Assignment 2

**Team members:**

| Name | Student ID |
| ---- | ---------- |
| Aboudraz, Reema   | 40253549 |
| Wissem Oumsalem     | 40291712 |

## What this program does

Draws a coloured pyramid with OpenGL and lets you transform it with the keyboard.

The pyramid is a square base with a tip above it: 5 vertices and 6 triangles
(4 sides + 2 for the base), sent to the GPU with a VAO, a VBO and an EBO.
Each vertex has its own colour, and OpenGL blends those colours across each face.

Every frame we build one matrix, `projection * view * model`, and send it to the
vertex shader as the `transform` uniform. The `model` part holds the translation,
rotation and scale controlled by the keys below.

## Controls

| Key | Action |
| --- | ------ |
| `W` | Translate up by `d` |
| `S` | Translate down by `d` |
| `A` | Translate left by `d` |
| `D` | Translate right by `d` |
| `Q` | Rotate 30° anticlockwise around the z axis |
| `E` | Rotate 30° clockwise around the z axis |
| `R` | Scale up along the z axis by factor `s` |
| `F` | Scale down along the z axis by factor `s` |
| `ESC` | Close the window |

`d`, the 30° step and `s` are hardcoded at the top of `src/main.cpp` as
`MOVE_DISTANCE`, `ROTATE_DEGREES` and `SCALE_FACTOR`.

One key press = one step. We use `glfwSetKeyCallback` instead of checking the
keys every frame, otherwise holding `Q` would rotate 30° per frame.

## Libraries

- **GLFW** - creates the window and reads the keyboard
- **GLEW** - loads the modern OpenGL functions
- **GLM** - matrix and vector math

## Running it on Windows (Visual Studio)

1. Install **Visual Studio 2022 Community** with the *Desktop development with C++* workload.
2. Download the precompiled binaries and extract them, e.g. to `C:\GLFW`, `C:\GLEW`, `C:\GLM`:
   - GLFW (Windows 64-bit binaries): https://www.glfw.org/
   - GLEW (Windows binaries): https://glew.sourceforge.net/install.html
   - GLM: https://github.com/g-truc/glm
3. Create a new **Console App** C++ project and replace its `.cpp` file with `src/main.cpp`.
4. Right click the project → **Properties** → *Configuration Properties*:
   - **VC++ Directories → Include Directories**: `C:\GLFW\include`, `C:\GLEW\include`, `C:\GLM`
   - **VC++ Directories → Library Directories**: `C:\GLFW\lib-vc2022`, `C:\GLEW\lib\Release\x64`
   - **Linker → Input → Additional Dependencies**: `glfw3.lib`, `glew32.lib`, `opengl32.lib`
5. Copy `glew32.dll` (the x64 one) next to the built `.exe`, usually in the `Debug` folder.
6. Make sure the build target is **x64**, then press **Ctrl+F5** to run.

## Running it on macOS

Install the libraries with [Homebrew](https://brew.sh):

```bash
brew install glfw glew glm
```

Compile and run:

```bash
clang++ -std=c++17 -DGL_SILENCE_DEPRECATION src/main.cpp -o pyramid \
  -I/opt/homebrew/include -L/opt/homebrew/lib \
  -lglfw -lGLEW -framework OpenGL

./pyramid
```

On an Intel Mac, Homebrew installs to `/usr/local` instead, so use
`-I/usr/local/include -L/usr/local/lib`.

macOS only gives OpenGL 3.3 through a forward compatible core profile, which is
why `main.cpp` sets `GLFW_OPENGL_FORWARD_COMPAT` inside an `#ifdef __APPLE__`.
