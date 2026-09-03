#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>  // Basic include for GLM
#include <SOIL2/SOIL2.h>  // For SOIL2
#include <iostream>
#include <string>
#include <fstream>

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

std::string loadShaderSource(const char *filePath) {
    std::string source;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return "";
    } else {
        std::cout << "Loading shader: " << filePath << std::endl;
    }

    std::string line;
    while (std::getline(fileStream, line)) {
        source.append(line + "\n");
    }

    fileStream.close();
    return source;
}

GLuint buildShaderProgram() {
    // We need create our shader programs and save the ID number for each
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vertShaderStr = loadShaderSource("shader.vert");
    std::string fragShaderStr = loadShaderSource("shader.frag");

    // We need to convert our strings into c-strings
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    // We need to load the shader source code into the shader program itself
    glShaderSource(vShader, 1, &vertShaderSrc, NULL);
    glShaderSource(fShader, 1, &fragShaderSrc, NULL);

    // We need to compile the shader source code
    glCompileShader(vShader);
    glCompileShader(fShader);

    // We need to create our program and attach our shader
    GLuint vfProgram = glCreateProgram();
    glAttachShader(vfProgram, vShader);
    glAttachShader(vfProgram, fShader);
    
    // We need to link the compiled shaders into a single program
    glLinkProgram(vfProgram);

    return vfProgram;
}

void init(GLFWwindow* window) {
    renderingProgram = buildShaderProgram();
    glGenVertexArrays(numVAOs, vao);
    glBindVertexArray(vao[0]);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(renderingProgram);
    //glPointSize(30.0f);
    glDrawArrays(GL_POINTS, 0, 1);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //Needed for MacOS
    GLFWwindow* window = glfwCreateWindow(800, 600, "Get to the Point", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return -1;
    }

    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;  // Ensures all extensions are loaded including Core Profiles
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    glfwSwapInterval(1);  // enables Vsync

    init(window);

    while (!glfwWindowShouldClose(window)) {
        display(window, glfwGetTime());
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}