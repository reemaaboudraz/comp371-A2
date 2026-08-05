// COMP 371 - Computer Graphics
// Assignment 2 - Coloured Pyramid
// Summer 2026
//
// Group Members:
// Aboudraz Reema    40253549
// Wissem Oumsalem   40291712

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

const int WIDTH = 800;
const int HEIGHT = 600;

const float MOVE_DISTANCE = 0.2f;
const float ROTATION_ANGLE = 30.0f;
const float SCALE_FACTOR = 1.2f;

float positionX = 0.0f;
float positionY = 0.0f;
float rotationZ = 0.0f;
float scaleZ = 1.0f;


// Vertex shader
const char* vertexShaderSource = R"glsl(
    #version 330 core

    layout(location = 0) in vec3 aPos;
    layout(location = 1) in vec3 aColor;

    uniform mat4 transform;

    out vec3 vertexColor;

    void main() {
        gl_Position = transform * vec4(aPos, 1.0);
        vertexColor = aColor;
    }
)glsl";


// Fragment shader
const char* fragmentShaderSource = R"glsl(
    #version 330 core

    in vec3 vertexColor;
    out vec4 FragColor;

    void main() {
        FragColor = vec4(vertexColor, 1.0);
    }
)glsl";


// Compile a shader
unsigned int compileShader(GLenum type, const char* source) {
    unsigned int shader = glCreateShader(type);

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Shader compilation error:\n"
                  << infoLog << std::endl;
    }

    return shader;
}


// Keyboard input
void processInput(
    GLFWwindow* window,
    int key,
    int,
    int action,
    int
) {
    if (action != GLFW_PRESS) {
        return;
    }

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_W) {
        positionY += MOVE_DISTANCE;
    }

    if (key == GLFW_KEY_S) {
        positionY -= MOVE_DISTANCE;
    }

    if (key == GLFW_KEY_A) {
        positionX -= MOVE_DISTANCE;
    }

    if (key == GLFW_KEY_D) {
        positionX += MOVE_DISTANCE;
    }

    if (key == GLFW_KEY_Q) {
        rotationZ += ROTATION_ANGLE;
    }

    if (key == GLFW_KEY_E) {
        rotationZ -= ROTATION_ANGLE;
    }

    if (key == GLFW_KEY_R) {
        scaleZ *= SCALE_FACTOR;
    }

    if (key == GLFW_KEY_F) {
        scaleZ /= SCALE_FACTOR;
    }
}


int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

#ifdef __APPLE__
    glfwWindowHint(
        GLFW_OPENGL_FORWARD_COMPAT,
        GL_TRUE
    );
#endif

    // Create the window
    GLFWwindow* window = glfwCreateWindow(
        WIDTH,
        HEIGHT,
        "COMP 371 - Coloured Pyramid",
        nullptr,
        nullptr
    );

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, processInput);

    // Initialize GLEW
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable(GL_DEPTH_TEST);


    // Compile and link the shaders
    unsigned int vertexShader = compileShader(
        GL_VERTEX_SHADER,
        vertexShaderSource
    );

    unsigned int fragmentShader = compileShader(
        GL_FRAGMENT_SHADER,
        fragmentShaderSource
    );

    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // Pyramid vertices: position and colour
    float vertices[] = {
        // Position              // Colour
        -0.5f, -0.5f,  0.5f,    1.00f, 0.25f, 0.55f,
         0.5f, -0.5f,  0.5f,    0.85f, 0.10f, 0.45f,
         0.5f, -0.5f, -0.5f,    0.95f, 0.45f, 0.70f,
        -0.5f, -0.5f, -0.5f,    0.70f, 0.20f, 0.60f,
         0.0f,  0.5f,  0.0f,    1.00f, 0.75f, 0.85f
    };


    // Pyramid triangles
    unsigned int indices[] = {
        0, 1, 4,
        1, 2, 4,
        2, 3, 4,
        3, 0, 4,
        0, 1, 2,
        0, 2, 3
    };


    // Create the VAO, VBO and EBO
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(indices),
        indices,
        GL_STATIC_DRAW
    );


    // Vertex positions
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        nullptr
    );

    glEnableVertexAttribArray(0);


    // Vertex colours
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        reinterpret_cast<void*>(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);

    glBindVertexArray(0);


    int transformLocation = glGetUniformLocation(
        shaderProgram,
        "transform"
    );


    // Render loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(
            0.12f,
            0.08f,
            0.14f,
            1.0f
        );

        glClear(
            GL_COLOR_BUFFER_BIT |
            GL_DEPTH_BUFFER_BIT
        );


        // Model transformations
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(
            model,
            glm::vec3(positionX, positionY, 0.0f)
        );

        model = glm::rotate(
            model,
            glm::radians(rotationZ),
            glm::vec3(0.0f, 0.0f, 1.0f)
        );

        model = glm::scale(
            model,
            glm::vec3(1.0f, 1.0f, scaleZ)
        );


        // Camera
        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 1.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );


        // Perspective projection
        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(WIDTH) /
                static_cast<float>(HEIGHT),
            0.1f,
            100.0f
        );


        glm::mat4 transform =
            projection * view * model;


        // Draw the pyramid
        glUseProgram(shaderProgram);

        glUniformMatrix4fv(
            transformLocation,
            1,
            GL_FALSE,
            glm::value_ptr(transform)
        );

        glBindVertexArray(VAO);

        glDrawElements(
            GL_TRIANGLES,
            18,
            GL_UNSIGNED_INT,
            nullptr
        );


        glfwSwapBuffers(window);
        glfwPollEvents();
    }


    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}