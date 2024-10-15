#include <GLFW/glfw3.h>
#ifndef __glad_h_
#include <glad/glad.h>  //! glad first
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform float xoffset; \n"
"out vec4 vertexColor; \n"
"out vec4 vrtxPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x + xoffset, aPos.y, aPos.z, 1.0);\n"
"   vertexColor = vec4(aColor, 1.0);\n"
"   vrtxPos = gl_Position;\n"
"}\0";

const char* fragmentShaderSource =
"#version 330 core\n"
"uniform vec4 customColor;\n"
"out vec4 FragColor;\n"
"in vec4 vertexColor;\n"
"in vec4 vrtxPos;\n"
"void main()\n"
"{\n"
"   FragColor = vrtxPos;\n"
"}\n\0";

void handleShaderError(unsigned int shader, char logBuffer[512]) {
    glGetShaderInfoLog(shader, 512, NULL, logBuffer);
    printf("shader error : \t\n %s ", logBuffer);
};
void handleProgramError(unsigned int program, char logBuffer[512]) {
    glGetProgramInfoLog(program, 512, NULL, logBuffer);
    printf("shader error : \t\n %s ", logBuffer);
};


int main(int argc, char const* argv[]) {



    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "ALL PRAISE DUE TO THE MOST HIGH ALLAH", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    gladLoadGL();
    glViewport(0, 0, 800, 800);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    printf(" max vertex attributes :  %d \n ", nrAttributes);

    int success;  // error handling
    char infolog[512];
    // making vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) handleShaderError(vertexShader, infolog);

    // Making the fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) handleShaderError(fragmentShader, infolog);

    // creating a program [vertex shader] --> [fragment shader] --> [screen / display]
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
    if (!success) handleProgramError(shaderProgram, infolog);
    glDeleteShader(vertexShader);  // cleanup
    glDeleteShader(fragmentShader);

    float vertices[] = {
        // positions         // colors
         0.5f, -0.5f, 0.0f,  1.0, 0.2, 1.0,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.2, 0.0, 0.8,   // bottom left
         0.0f,  0.5f, 0.0f,  0.0, 0.0, 0.9    // top 
    };
    float vertices0[] = {
        // positions         // colors
         0.5f, -0.2f, 0.2f,  1.0, 0.2, 1.0,   // bottom right
        -0.3f, -0.1f, 0.0f,  0.2, 0.0, 0.8,   // bottom left
         0.2f,  0.6f, 0.0f,  0.0, 0.0, 0.9    // top 
    };
    unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

    //* VBO is the buffer resides on the server side(the gpu) for undescribed data, VAO is the description of the VBO in the cpu
    unsigned int VBO[2], VAO, EBO;

    // generating
    glGenBuffers(2, VBO);  //? vertex buffer object to send to the gpu
    glGenVertexArrays(2, &VAO);  //? array of VBOs
    glGenBuffers(1, &EBO);


    // stating that we're going to use them
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // adding our data
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices0), vertices0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // fill or outline mode
    glClearColor(0.2f, 0.1f, 0.5f, 0.5f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        processInput(window);

        float timeValue = glfwGetTime();
        float channelValue = (sin(timeValue) / 2.0f) + 0.5f; //channel r/g/b

        glUseProgram(shaderProgram);
        int vertexColorLocation = glGetUniformLocation(shaderProgram, "customColor");
        glUniform4f(vertexColorLocation, channelValue, 0.05, 1 - channelValue, 1.0);
        int xoffsetLocation = glGetUniformLocation(shaderProgram, "xoffset");

        glUniform1f(xoffsetLocation, channelValue);
        glBindVertexArray(VAO );

        glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, &VAO);
    glDeleteBuffers(2, VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
