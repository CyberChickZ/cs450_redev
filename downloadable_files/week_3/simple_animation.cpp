#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>  // Basic include for GLM
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SOIL2/SOIL2.h>  // For SOIL2
#include <iostream>
#include <string>
#include <fstream>

#define numVAOs 1
#define numVBOs 2

float vertices[] = {
      -0.5f, -0.5f, 0.0f,
       0.5f, -0.5f, 0.0f,
       0.0f,  0.5f, 0.0f
};

float colors[] = {
      1.0f, 0.0f, 0.0f,
      0.0f, 1.0f, 0.0f,
      0.0f, 0.0f, 1.0f,
};

GLuint renderingProgram;

GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLint mvpLoc = -1;
GLint posColorLoc = -1;

std::string loadShaderSource(const char* filePath) {
    std::string source;
    std::ifstream fileStream(filePath, std::ios::in);

    if (!fileStream.is_open()) {
        std::cerr << "Error: Could not open file " << filePath << std::endl;
        return "";
    }
    else {
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
    const char* vertShaderSrc = vertShaderStr.c_str();
    const char* fragShaderSrc = fragShaderStr.c_str();

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

    //Get uniform location
    mvpLoc = glGetUniformLocation(renderingProgram, "mvp");
	posColorLoc = glGetUniformLocation(renderingProgram, "posColor");

    // Generate VAOs and VBOs
    glGenVertexArrays(numVAOs, vao);
    glGenBuffers(numVBOs, vbo);

    // Bind VAO and Position VBO
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);

    // Fill Position VBO with vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    // Bind Color VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);

    // Fill Color VBO with vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

    // Set Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(1);

    // Unbind VAO and VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(renderingProgram);

    // This is not a true mvp matrix,
    // we will learn how to do it correctly soon
    float t = static_cast<float>(currentTime);
	float cycle = fmod(t, 2.0f);
	//float yOffset = 0.5f - abs(cycle - 1.0f);
    float yOffset = 0.5f * sin(t * glm::pi<float>()); // Oscillate between -0.5 and 0.5
	float xOffset = 0.5f * cos(t * glm::pi<float>()); // Oscillate between -0.5 and 0.5
    glm::mat4 mvp = glm::mat4(1.0f);
	float yOffsetNormal = (yOffset + 0.5f) / 1.0f; // Normalize yOffset to range [0, 1]
	float xOffsetNormal = (xOffset + 0.5f) / 1.0f; // Normalize xOffset to range [0, 1]
    glm::vec3 posColor = glm::vec3(yOffsetNormal, 0.0f, 1.0f-yOffsetNormal);
    mvp = glm::translate(mvp, glm::vec3(xOffset, yOffset, 0.0f));
    

    // Set Uniforms
    glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glUniform3fv(posColorLoc, 1, glm::value_ptr(posColor));

    // Bind VAO
    glBindVertexArray(vao[0]);

    // Draw triangle
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // Unbind VAO
    glBindVertexArray(0);
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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple Animation", NULL, NULL);
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