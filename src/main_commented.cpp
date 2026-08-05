// COMP 371 - Computer Graphics
// Assignment 2 - Coloured Pyramid
// Summer 2026
//
// Group Members:
// Aboudraz Reema    40253549
// Wissem Oumsalem   40291712
//
// Commented version of main.cpp. Same code, one comment per line.

#include <GL/glew.h>        // loads modern OpenGL functions, must come before GLFW
#include <GLFW/glfw3.h>     // creates the window and gives us keyboard input

#include <glm/glm.hpp>                      // math types: vec3, mat4
#include <glm/gtc/matrix_transform.hpp>     // translate, rotate, scale, lookAt, perspective
#include <glm/gtc/type_ptr.hpp>             // value_ptr, converts a mat4 to a float*

#include <iostream>         // std::cout, used only for error messages

const int WIDTH = 800;      // window width in pixels
const int HEIGHT = 600;     // window height in pixels

const float MOVE_DISTANCE = 0.2f;    // world units moved per W/A/S/D press
const float ROTATION_ANGLE = 30.0f;  // degrees turned per Q/E press
const float SCALE_FACTOR = 1.2f;     // multiplier per R/F press, so R then F undoes itself

float positionX = 0.0f;     // current left/right offset, global so the key callback can reach it
float positionY = 0.0f;     // current up/down offset
float rotationZ = 0.0f;     // current rotation around z in degrees
float scaleZ = 1.0f;        // current depth stretch factor


//vertex shader
const char* vertexShaderSource = R"glsl(     // raw string literal, holds GLSL source compiled at runtime
    #version 330 core                        // use OpenGL 3.3 core GLSL

    layout(location = 0) in vec3 aPos;       // input attribute 0: this vertex's position
    layout(location = 1) in vec3 aColor;     // input attribute 1: this vertex's colour

    uniform mat4 transform;                  // matrix sent from the CPU, same for every vertex

    out vec3 vertexColor;                    // passed to the fragment shader, interpolated on the way

    void main() {                                        // runs once per vertex
        gl_Position = transform * vec4(aPos, 1.0);       // final position, the 1.0 makes translation apply
        vertexColor = aColor;                            // forward the colour unchanged
    }
)glsl";


//fragment shader
const char* fragmentShaderSource = R"glsl(   // second shader stage, also compiled at runtime
    #version 330 core                        // use OpenGL 3.3 core GLSL

    in vec3 vertexColor;                     // interpolated colour, name must match the vertex shader's out
    out vec4 FragColor;                      // the pixel colour we output

    void main() {                                // runs once per pixel covered by a triangle
        FragColor = vec4(vertexColor, 1.0);      // use that colour, alpha 1.0 = fully opaque
    }
)glsl";


//compile a shader
unsigned int compileShader(GLenum type, const char* source) {   // turns one GLSL string into a shader object
    unsigned int shader = glCreateShader(type);                 // make an empty shader, get back a handle

    glShaderSource(shader, 1, &source, nullptr);   // attach 1 source string, nullptr = it is null-terminated
    glCompileShader(shader);                       // compile it inside the graphics driver

    int success;            // will hold the compile result
    char infoLog[512];      // buffer for the driver's error message

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);   // ask if it compiled, OpenGL never throws

    if (!success) {                                            // compile failed
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);     // copy the driver's message into infoLog
        std::cout << "Shader compilation error:\n"             // print it, otherwise we just get a blank window
                  << infoLog << std::endl;
    }

    return shader;      // hand back the handle either way
}


//keyboard input
void processInput(          // GLFW calls this for us on every key event
    GLFWwindow* window,     // which window got the event
    int key,                // which key, as a GLFW_KEY_* constant
    int,                    // unnamed: hardware scancode, unused
    int action,             // GLFW_PRESS, GLFW_RELEASE or GLFW_REPEAT
    int                     // unnamed: modifier keys, unused
) {
    if (action != GLFW_PRESS) {   // ignore releases and auto-repeat
        return;                   // so one tap moves the pyramid once
    }

    if (key == GLFW_KEY_ESCAPE) {                    // ESC pressed
        glfwSetWindowShouldClose(window, true);      // flag the loop to exit so cleanup still runs
    }

    if (key == GLFW_KEY_W) {          // W
        positionY += MOVE_DISTANCE;   // move up
    }

    if (key == GLFW_KEY_S) {          // S
        positionY -= MOVE_DISTANCE;   // move down
    }

    if (key == GLFW_KEY_A) {          // A
        positionX -= MOVE_DISTANCE;   // move left
    }

    if (key == GLFW_KEY_D) {          // D
        positionX += MOVE_DISTANCE;   // move right
    }

    if (key == GLFW_KEY_Q) {            // Q
        rotationZ += ROTATION_ANGLE;    // turn anticlockwise
    }

    if (key == GLFW_KEY_E) {            // E
        rotationZ -= ROTATION_ANGLE;    // turn clockwise
    }

    if (key == GLFW_KEY_R) {          // R
        scaleZ *= SCALE_FACTOR;       // stretch along z
    }

    if (key == GLFW_KEY_F) {          // F
        scaleZ /= SCALE_FACTOR;       // squash along z
    }
}


int main() {
    //initialize GLFW
    if (!glfwInit()) {                                           // start GLFW, returns 0 on failure
        std::cout << "Failed to initialize GLFW." << std::endl;  // report it
        return -1;                                               // quit, nothing to clean up yet
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);   // ask for OpenGL 3.x
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);   // specifically 3.3, the minimum for shaders and VAOs
    glfwWindowHint(                                  // ask for the core profile
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE                     // modern shader-only OpenGL, no fixed-function pipeline
    );


// for my mac (wis)
#ifdef __APPLE__                       // compiled only on macOS, invisible to the Windows build
    glfwWindowHint(                    // macOS gives OpenGL 2.1 without this and every shader call fails
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );
#endif

    //create window
    GLFWwindow* window = glfwCreateWindow(   // makes the window and its OpenGL context together
        WIDTH,                               // width
        HEIGHT,                              // height
        "COMP 371 - Coloured Pyramid",       // title bar text
        nullptr,                             // no monitor = windowed, not fullscreen
        nullptr                              // no other context to share resources with
    );

    if (window == nullptr) {                                        // failed, machine cannot do 3.3 core
        std::cout << "Failed to create GLFW window." << std::endl;  // report it
        glfwTerminate();                                            // GLFW did start, so shut it down
        return -1;                                                  // quit
    }

    glfwMakeContextCurrent(window);              // every later gl* call targets this context
    glfwSetKeyCallback(window, processInput);    // register our handler, we pass it, we don't call it

    //initialize GLEW
    glewExperimental = GL_TRUE;   // needed for core profiles or core function pointers stay null

    if (glewInit() != GLEW_OK) {                                 // load the function pointers, after the context exists
        std::cout << "Failed to initialize GLEW." << std::endl;  // report it
        glfwTerminate();                                         // clean up GLFW
        return -1;                                               // quit
    }

    glViewport(0, 0, WIDTH, HEIGHT);   // render into the whole window, origin at bottom left
    glEnable(GL_DEPTH_TEST);           // only draw a pixel if it is nearer, so back faces stay behind


    //compile + link the shaders
    unsigned int vertexShader = compileShader(    // compile the vertex stage
        GL_VERTEX_SHADER,
        vertexShaderSource
    );

    unsigned int fragmentShader = compileShader(  // compile the fragment stage
        GL_FRAGMENT_SHADER,
        fragmentShaderSource
    );

    unsigned int shaderProgram = glCreateProgram();   // a program is the two stages linked together

    glAttachShader(shaderProgram, vertexShader);      // attach the vertex stage
    glAttachShader(shaderProgram, fragmentShader);    // attach the fragment stage
    glLinkProgram(shaderProgram);                     // link them, matching vertexColor out to in

    glDeleteShader(vertexShader);      // the program has its own copy now, free this one
    glDeleteShader(fragmentShader);    // same here


    // pyramid vertices
    float vertices[] = {      // 5 corners, 6 floats each: x, y, z then r, g, b, interleaved in one array
        //position and colors
        -0.5f, -0.5f,  0.5f,    1.00f, 0.25f, 0.55f,   // 0: base, front-left
         0.5f, -0.5f,  0.5f,    0.85f, 0.10f, 0.45f,   // 1: base, front-right
         0.5f, -0.5f, -0.5f,    0.95f, 0.45f, 0.70f,   // 2: base, back-right
        -0.5f, -0.5f, -0.5f,    0.70f, 0.20f, 0.60f,   // 3: base, back-left
         0.0f,  0.5f,  0.0f,    1.00f, 0.75f, 0.85f    // 4: apex
    };


    //pyramid triangles
    unsigned int indices[] = {   // each row is one triangle, given as 3 indices into vertices above
        0, 1, 4,   // front side
        1, 2, 4,   // right side
        2, 3, 4,   // back side
        3, 0, 4,   // left side
        0, 1, 2,   // base half 1, a square needs two triangles
        0, 2, 3    // base half 2
    };


    //create the VAO, VBO and EBO
    unsigned int VAO;   // remembers the vertex layout so we rebind one object at draw time
    unsigned int VBO;   // GPU buffer holding the vertices array
    unsigned int EBO;   // GPU buffer holding the indices array

    glGenVertexArrays(1, &VAO);   // generate 1 VAO handle, written back into VAO
    glGenBuffers(1, &VBO);        // generate 1 buffer handle for the vertices
    glGenBuffers(1, &EBO);        // generate 1 buffer handle for the indices

    glBindVertexArray(VAO);       // bind it first, everything below gets recorded into it

    glBindBuffer(GL_ARRAY_BUFFER, VBO);   // the array buffer I mean from now on is the VBO
    glBufferData(                         // upload the vertices from RAM to the GPU
        GL_ARRAY_BUFFER,
        sizeof(vertices),                 // total bytes, works because vertices is a real array here
        vertices,                         // the data
        GL_STATIC_DRAW                    // hint: set once, drawn many times
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);   // this is the slot glDrawElements reads indices from
    glBufferData(                                 // upload the indices the same way
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),                          // total bytes
        indices,                                  // the data
        GL_STATIC_DRAW                            // same hint
    );


    //vertex positions
    glVertexAttribPointer(     // describe how to read attribute 0 out of the interleaved buffer
        0,                     // attribute location, matches layout(location = 0)
        3,                     // 3 components: x, y, z
        GL_FLOAT,              // each one is a float
        GL_FALSE,              // do not normalize the values
        6 * sizeof(float),     // stride, 6 not 3 because the colour sits between vertices
        nullptr                // offset, position starts at byte 0 of each vertex
    );

    glEnableVertexAttribArray(0);   // attributes are off by default, switch 0 on


    //vertex colours
    glVertexAttribPointer(                            // same for attribute 1
        1,                                            // matches layout(location = 1)
        3,                                            // 3 components: r, g, b
        GL_FLOAT,                                     // floats
        GL_FALSE,                                     // no normalizing
        6 * sizeof(float),                            // same stride
        reinterpret_cast<void*>(3 * sizeof(float))    // offset, colour starts 3 floats in, cast is required by the old signature
    );

    glEnableVertexAttribArray(1);   // switch attribute 1 on

    glBindVertexArray(0);           // unbind so stray calls cannot change what we just set up


    int transformLocation = glGetUniformLocation(   // find where the transform uniform lives in the program
        shaderProgram,
        "transform"                                 // looked up once here, not 60 times a second
    );


    //render loop
    while (!glfwWindowShouldClose(window)) {   // one pass per frame, until ESC or the close button
        glClearColor(                          // the colour the screen is wiped to
            0.12f,                             // red
            0.08f,                             // green
            0.14f,                             // blue, together a dark purple
            1.0f                               // alpha
        );

        glClear(                        // actually wipe
            GL_COLOR_BUFFER_BIT |       // the image
            GL_DEPTH_BUFFER_BIT         // and the stored depths, or last frame's depths block this one
        );


        //model transformations
        glm::mat4 model = glm::mat4(1.0f);   // identity, meaning no transformation yet, rebuilt each frame

        model = glm::translate(                        // move it
            model,
            glm::vec3(positionX, positionY, 0.0f)      // by the W/A/S/D offsets, z stays 0
        );

        model = glm::rotate(                        // spin it
            model,
            glm::radians(rotationZ),                // GLM wants radians, we store degrees
            glm::vec3(0.0f, 0.0f, 1.0f)             // around the z-axis
        );

        model = glm::scale(                       // stretch it
            model,
            glm::vec3(1.0f, 1.0f, scaleZ)         // z only, x and y stay at 1
        );


        //camera
        glm::mat4 view = glm::lookAt(              // the camera
            glm::vec3(0.0f, 1.0f, 3.0f),           // eye: up a bit and 3 units back, gives a slight downward angle
            glm::vec3(0.0f, 0.0f, 0.0f),           // aimed at the origin where the pyramid is
            glm::vec3(0.0f, 1.0f, 0.0f)            // up is +y, so the image is not rolled
        );


        //perspective projection
        glm::mat4 projection = glm::perspective(   // makes distant things smaller
            glm::radians(45.0f),                   // vertical field of view
            static_cast<float>(WIDTH) /            // aspect ratio, the cast matters or int division gives 1
                static_cast<float>(HEIGHT),
            0.1f,                                  // near clip plane, nearer is not drawn
            100.0f                                 // far clip plane, further is not drawn
        );


        glm::mat4 transform =            // combine all three into what the shader uses
            projection * view * model;   // applied right to left: model, then view, then projection


        //draw pyramid
        glUseProgram(shaderProgram);   // activate our shaders

        glUniformMatrix4fv(                  // send the matrix to the shader, must be after glUseProgram
            transformLocation,               // where to write it
            1,                               // 1 matrix
            GL_FALSE,                        // no transpose, GLM already matches OpenGL's layout
            glm::value_ptr(transform)        // the raw 16 floats
        );

        glBindVertexArray(VAO);   // restore the buffers and attribute layout in one call

        glDrawElements(          // draw using the index buffer
            GL_TRIANGLES,        // read indices 3 at a time as triangles
            18,                  // 6 triangles x 3 indices
            GL_UNSIGNED_INT,     // the indices array's type
            nullptr              // start at offset 0 in the bound EBO
        );


        glfwSwapBuffers(window);   // show the finished frame, we drew into a hidden buffer to avoid flicker
        glfwPollEvents();          // handle window and key events, this is what calls processInput
    }


    // cleanup
    glDeleteVertexArrays(1, &VAO);      // free the VAO
    glDeleteBuffers(1, &VBO);           // free the vertex buffer
    glDeleteBuffers(1, &EBO);           // free the index buffer
    glDeleteProgram(shaderProgram);     // free the shader program

    glfwDestroyWindow(window);   // close the window and destroy its context
    glfwTerminate();             // shut GLFW down completely

    return 0;   // 0 tells the OS the program finished successfully
}
