// COMP 371 - Computer Graphics
// Assignment 2 - Coloured Pyramid
// Summer 2026
//
// Group Members:
// Aboudraz Reema    40253549
// Wissem Oumsalem   40291712


// GLEW gives us access to modern OpenGL functions
#include <GL/glew.h>

// GLFW creates the window and reads keyboard input
#include <GLFW/glfw3.h>

// GLM gives us vectors and matrices
#include <glm/glm.hpp>

// This gives us translate, rotate, scale, perspective and lookAt
#include <glm/gtc/matrix_transform.hpp>

// This helps us send a GLM matrix to OpenGL
#include <glm/gtc/type_ptr.hpp>

// Used to print error messages
#include <iostream>


// ------------------------------------------------------------
// Window settings
// ------------------------------------------------------------

// constexpr means these values cannot change
constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

// ------------------------------------------------------------
// Transformation settings
// ------------------------------------------------------------

// Distance used when moving the pyramid
constexpr float MOVE_DISTANCE = 0.2f;

// Rotation used for each Q or E key press
constexpr float ROTATE_DEGREES = 30.0f;

// Number used to make the pyramid bigger or smaller in z
constexpr float SCALE_FACTOR = 1.2f;

// ------------------------------------------------------------
// Current pyramid values
// ------------------------------------------------------------

// Left and right position
float positionX = 0.0f;

// Up and down position
float positionY = 0.0f;

// Rotation around the z-axis
float rotationZ = 0.0f;

// Size in the z direction
float scaleZ = 1.0f;

// ------------------------------------------------------------
// Vertex shader
// ------------------------------------------------------------

// The vertex shader changes the position of every vertex
const char* vertexShaderSource = R"glsl(
    #version 330 core

    // Position comes from location 0 in the VBO
    layout (location = 0) in vec3 aPos;

    // Colour comes from location 1 in the VBO
    layout (location = 1) in vec3 aColor;

    // This matrix moves, rotates and scales the pyramid
    uniform mat4 transform;

    // Send the colour to the fragment shader
    out vec3 vertexColor;

    void main() {
        // Change the position of the vertex
        gl_Position = transform * vec4(aPos, 1.0);

        // Pass the vertex colour to the fragment shader
        vertexColor = aColor;
    }
)glsl";

// ------------------------------------------------------------
// Fragment shader
// ------------------------------------------------------------

// The fragment shader chooses the colour of each pixel
const char* fragmentShaderSource = R"glsl(
    #version 330 core

    // Colour received from the vertex shader
    in vec3 vertexColor;

    // Final pixel colour
    out vec4 FragColor;

    void main() {
        // Use the received colour
        // The last value is opacity, where 1.0 means fully visible
        FragColor = vec4(vertexColor, 1.0);
    }
)glsl";

// ------------------------------------------------------------
// Compile one shader
// ------------------------------------------------------------

// shaderType tells us if this is a vertex shader or fragment shader
unsigned int compileShader(
    GLenum shaderType,
    const char* shaderSource
) {
    // Create an empty shader
    unsigned int shader = glCreateShader(shaderType);

    // Give the shader its GLSL code
    glShaderSource(shader, 1, &shaderSource, nullptr);

    // Compile the shader code
    glCompileShader(shader);

    // success tells us if the shader compiled
    int success;

    // infoLog stores an error message
    char infoLog[512];

    // Check if compilation worked
    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    // Print the error if compilation failed
    if (!success) {
        glGetShaderInfoLog(
            shader,
            512,
            nullptr,
            infoLog
        );

        std::cerr
            << "Shader compilation failed:\n"
            << infoLog
            << std::endl;

        // Delete the broken shader
        glDeleteShader(shader);

        // Zero means failure
        return 0;
    }

    // Return the finished shader
    return shader;
}

// ------------------------------------------------------------
// Create the complete shader program
// ------------------------------------------------------------

unsigned int createShaderProgram() {
    // Compile the vertex shader
    unsigned int vertexShader = compileShader(
        GL_VERTEX_SHADER,
        vertexShaderSource
    );

    // Compile the fragment shader
    unsigned int fragmentShader = compileShader(
        GL_FRAGMENT_SHADER,
        fragmentShaderSource
    );

    // Stop if one shader failed
    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return 0;
    }

    // Create an empty shader program
    unsigned int shaderProgram = glCreateProgram();

    // Add both shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link the shaders together
    glLinkProgram(shaderProgram);

    // success tells us if linking worked
    int success;

    // Stores a linking error message
    char infoLog[512];

    // Check if linking worked
    glGetProgramiv(
        shaderProgram,
        GL_LINK_STATUS,
        &success
    );

    // Print the error if linking failed
    if (!success) {
        glGetProgramInfoLog(
            shaderProgram,
            512,
            nullptr,
            infoLog
        );

        std::cerr
            << "Shader program linking failed:\n"
            << infoLog
            << std::endl;

        // Delete the broken program
        glDeleteProgram(shaderProgram);

        shaderProgram = 0;
    }

    // The program now contains the shaders
    // We do not need the separate shaders anymore
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Return the complete shader program
    return shaderProgram;
}

// ------------------------------------------------------------
// Keyboard input
// ------------------------------------------------------------

// GLFW calls this function when a key is pressed
void processInput(
    GLFWwindow* window,
    int key,
    int,
    int action,
    int
) {
    // Only react when the key is first pressed
    // Ignore key release and key repeat
    if (action != GLFW_PRESS) {
        return;
    }

    // Check which key was pressed
    switch (key) {
        // Close the window
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, true);
            break;

        // Move up
        case GLFW_KEY_W:
            positionY += MOVE_DISTANCE;
            break;

        // Move down
        case GLFW_KEY_S:
            positionY -= MOVE_DISTANCE;
            break;

        // Move left
        case GLFW_KEY_A:
            positionX -= MOVE_DISTANCE;
            break;

        // Move right
        case GLFW_KEY_D:
            positionX += MOVE_DISTANCE;
            break;

        // Rotate anticlockwise by 30 degrees
        case GLFW_KEY_Q:
            rotationZ += ROTATE_DEGREES;
            break;

        // Rotate clockwise by 30 degrees
        case GLFW_KEY_E:
            rotationZ -= ROTATE_DEGREES;
            break;

        // Make the pyramid larger in the z direction
        case GLFW_KEY_R:
            scaleZ *= SCALE_FACTOR;
            break;

        // Make the pyramid smaller in the z direction
        case GLFW_KEY_F:
            scaleZ /= SCALE_FACTOR;
            break;

        // Do nothing for other keys
        default:
            break;
    }
}

// ------------------------------------------------------------
// Main program
// ------------------------------------------------------------

int main() {
    // --------------------------------------------------------
    // Start GLFW
    // --------------------------------------------------------

    // GLFW is used to create the window
    if (!glfwInit()) {
        std::cerr
            << "Failed to initialize GLFW."
            << std::endl;

        return -1;
    }

    // Ask for OpenGL version 3.3
    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    // Use modern OpenGL
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

#ifdef __APPLE__
    // This is needed for OpenGL on macOS
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );
#endif

    // --------------------------------------------------------
    // Create the window
    // --------------------------------------------------------

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "COMP 371 - Assignment 2 - Pyramid",
        nullptr,
        nullptr
    );

    // Stop if the window was not created
    if (window == nullptr) {
        std::cerr
            << "Failed to create the GLFW window."
            << std::endl;

        glfwTerminate();

        return -1;
    }

    // Tell OpenGL to use this window
    glfwMakeContextCurrent(window);

    // Connect keyboard input to our processInput function
    glfwSetKeyCallback(window, processInput);

    // --------------------------------------------------------
    // Start GLEW
    // --------------------------------------------------------

    // This lets GLEW load modern OpenGL functions
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cerr
            << "Failed to initialize GLEW."
            << std::endl;

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    // --------------------------------------------------------
    // Enable depth testing
    // --------------------------------------------------------

    // Depth testing shows the closest triangle
    // Without it, triangles may appear in the wrong order
    glEnable(GL_DEPTH_TEST);

    // --------------------------------------------------------
    // Create the shader program
    // --------------------------------------------------------

    unsigned int shaderProgram = createShaderProgram();

    // Stop if the shader program failed
    if (shaderProgram == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    // --------------------------------------------------------
    // Pyramid vertices
    // --------------------------------------------------------

    // A vertex is one point of the pyramid
    // Each vertex has:
    // x, y, z position
    // red, green, blue colour
    float vertices[] = {
        // Position              // Colour

        // Vertex 0: front-left corner
        -0.5f, -0.5f,  0.5f,    1.00f, 0.25f, 0.55f,

        // Vertex 1: front-right corner
         0.5f, -0.5f,  0.5f,    0.85f, 0.10f, 0.45f,

        // Vertex 2: back-right corner
         0.5f, -0.5f, -0.5f,    0.95f, 0.45f, 0.70f,

        // Vertex 3: back-left corner
        -0.5f, -0.5f, -0.5f,    0.70f, 0.20f, 0.60f,

        // Vertex 4: top of the pyramid
         0.0f,  0.5f,  0.0f,    1.00f, 0.75f, 0.85f
    };

    // --------------------------------------------------------
    // Pyramid indices
    // --------------------------------------------------------

    // Every three numbers create one triangle
    // The numbers point to vertices from the array above
    unsigned int indices[] = {
        // Front triangle
        0, 1, 4,

        // Right triangle
        1, 2, 4,

        // Back triangle
        2, 3, 4,

        // Left triangle
        3, 0, 4,

        // First half of the square bottom
        0, 1, 2,

        // Second half of the square bottom
        0, 2, 3
    };

    // Count how many index numbers we have
    // There are 18 numbers in this array
    const GLsizei indexCount = static_cast<GLsizei>(
        sizeof(indices) / sizeof(indices[0])
    );

    // --------------------------------------------------------
    // VAO, VBO and EBO
    // --------------------------------------------------------

    // VAO remembers how the vertex data is organized
    unsigned int VAO;

    // VBO stores the vertex data
    unsigned int VBO;

    // EBO stores the index data
    unsigned int EBO;

    // Create one VAO
    glGenVertexArrays(1, &VAO);

    // Create one VBO
    glGenBuffers(1, &VBO);

    // Create one EBO
    glGenBuffers(1, &EBO);

    // Bind the VAO
    // The next settings will be saved inside it
    glBindVertexArray(VAO);

    // --------------------------------------------------------
    // Send vertices to the VBO
    // --------------------------------------------------------

    // Select the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy the vertices into the VBO
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    // GL_STATIC_DRAW means the data will not change often

    // --------------------------------------------------------
    // Send indices to the EBO
    // --------------------------------------------------------

    // Select the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    // Copy the indices into the EBO
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );

    // --------------------------------------------------------
    // Explain the vertex positions
    // --------------------------------------------------------

    // Location 0 is the position
    glVertexAttribPointer(
        0,                  // Shader location
        3,                  // Three values: x, y and z
        GL_FLOAT,           // The values are floats
        GL_FALSE,           // Do not change the values
        6 * sizeof(float),  // Each vertex has six floats
        nullptr             // Position starts at the first float
    );

    // Turn on location 0
    glEnableVertexAttribArray(0);

    // --------------------------------------------------------
    // Explain the vertex colours
    // --------------------------------------------------------

    // Location 1 is the colour
    glVertexAttribPointer(
        1,                  // Shader location
        3,                  // Three values: red, green and blue
        GL_FLOAT,           // The values are floats
        GL_FALSE,           // Do not change the values
        6 * sizeof(float),  // Each vertex has six floats
        reinterpret_cast<void*>(3 * sizeof(float))
                            // Colour starts after x, y and z
    );

    // Turn on location 1
    glEnableVertexAttribArray(1);

    // Stop using the VAO for now
    glBindVertexArray(0);

    // --------------------------------------------------------
    // Find the transform uniform
    // --------------------------------------------------------

    // Get the location of "transform" inside the vertex shader
    int transformLocation = glGetUniformLocation(
        shaderProgram,
        "transform"
    );

    // -1 means OpenGL could not find the uniform
    if (transformLocation == -1) {
        std::cerr
            << "Could not find the transform uniform."
            << std::endl;

        // Delete OpenGL objects before stopping
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteProgram(shaderProgram);

        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    // --------------------------------------------------------
    // Render loop
    // --------------------------------------------------------

    // This loop keeps running until the window closes
    while (!glfwWindowShouldClose(window)) {
        // Store the current window size
        int width;
        int height;

        // Get the real drawing area size
        glfwGetFramebufferSize(
            window,
            &width,
            &height
        );

        // Avoid using zero as the width
        if (width <= 0) {
            width = 1;
        }

        // Avoid dividing by zero
        if (height <= 0) {
            height = 1;
        }

        // Tell OpenGL where to draw
        glViewport(
            0,
            0,
            width,
            height
        );

        // ----------------------------------------------------
        // Clear the old frame
        // ----------------------------------------------------

        // Set a dark purple background colour
        glClearColor(
            0.12f,
            0.08f,
            0.14f,
            1.0f
        );

        // Clear the old colours and old depth information
        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );

        // ----------------------------------------------------
        // Model matrix
        // ----------------------------------------------------

        // The model matrix changes the pyramid itself
        // 1.0 creates an identity matrix
        glm::mat4 model(1.0f);

        // Move the pyramid
        model = glm::translate(
            model,
            glm::vec3(
                positionX,
                positionY,
                0.0f
            )
        );

        // Rotate around the z-axis
        model = glm::rotate(
            model,

            // GLM needs radians, not degrees
            glm::radians(rotationZ),

            // This vector chooses the z-axis
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        // Scale only in the z direction
        model = glm::scale(
            model,
            glm::vec3(
                1.0f,
                1.0f,
                scaleZ
            )
        );

        // ----------------------------------------------------
        // View matrix
        // ----------------------------------------------------

        // The view matrix acts like a camera
        glm::mat4 view = glm::lookAt(
            // Camera position
            glm::vec3(0.0f, 1.0f, 3.0f),

            // Point the camera looks at
            glm::vec3(0.0f, 0.0f, 0.0f),

            // The camera's up direction
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // ----------------------------------------------------
        // Projection matrix
        // ----------------------------------------------------

        // The aspect ratio keeps the pyramid from stretching
        float aspectRatio =
            static_cast<float>(width) /
            static_cast<float>(height);

        // Perspective makes closer objects look bigger
        glm::mat4 projection = glm::perspective(
            // Camera viewing angle
            glm::radians(45.0f),

            // Window width divided by height
            aspectRatio,

            // Closest visible distance
            0.1f,

            // Farthest visible distance
            100.0f
        );

        // ----------------------------------------------------
        // Final transformation
        // ----------------------------------------------------

        // Combine projection, camera and model matrices
        // Matrix order matters
        glm::mat4 transform =
            projection * view * model;

        // ----------------------------------------------------
        // Draw the pyramid
        // ----------------------------------------------------

        // Use our shader program
        glUseProgram(shaderProgram);

        // Send the transform matrix to the vertex shader
        glUniformMatrix4fv(
            transformLocation,

            // We are sending one matrix
            1,

            // Do not transpose the matrix
            GL_FALSE,

            // Get the raw values from the GLM matrix
            glm::value_ptr(transform)
        );

        // Use the pyramid's VAO
        glBindVertexArray(VAO);

        // Draw triangles using the indices from the EBO
        glDrawElements(
            GL_TRIANGLES,
            indexCount,
            GL_UNSIGNED_INT,
            nullptr
        );

        // Show the finished frame on the screen
        glfwSwapBuffers(window);

        // Check for keyboard and window events
        glfwPollEvents();
    }

    // --------------------------------------------------------
    // Cleanup
    // --------------------------------------------------------

    // Delete the VAO
    glDeleteVertexArrays(1, &VAO);

    // Delete the VBO
    glDeleteBuffers(1, &VBO);

    // Delete the EBO
    glDeleteBuffers(1, &EBO);

    // Delete the shader program
    glDeleteProgram(shaderProgram);

    // Delete the window
    glfwDestroyWindow(window);

    // Stop GLFW
    glfwTerminate();

    return 0;
}