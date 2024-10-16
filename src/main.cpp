#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <classes/shader.h> 
#include <classes/vertex.h>
#include <iostream> 
#include <math.h>
#include <time.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std;
void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}
GLFWwindow* InitializeWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window = glfwCreateWindow(800, 800, "ALL PRAISE DUE TO THE MOST HIGH ALLAH", NULL, NULL);
  if (!window) {
    printf("Failed to create GLFW window");
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(window);
  gladLoadGL();
  glViewport(0, 0, 800, 800);
  return window;
}


void pushOneBlockToBuffer(float arr[], int from, float posx, float posy, float posz, float height) {
  vertex upLeft, downRight, downLeft, upRight;
  upLeft.fillRandom();
  upRight.fillRandom();
  downLeft.fillRandom();
  downRight.fillRandom();

  upLeft.setPosition(posx, posy, posz, 0.0, 0.0);
  upRight.setPosition(posx + height, posy, posz, 1.0, 0.0);
  downLeft.setPosition(posx, posy - height, posz, 0.0, 1.0);
  downRight.setPosition(posx + height, posy - height, posz, 1.0, 1.0);


  upLeft.pushElementsToBuffer(arr, from);
  upRight.pushElementsToBuffer(arr, from + 8);
  downLeft.pushElementsToBuffer(arr, from + 16);
  downRight.pushElementsToBuffer(arr, from + 24);


};

int main(int argc, char const* argv[])
{
  GLFWwindow* window = InitializeWindow();
  char vertexShaderFilePath[] = "src/shaders/vertex.glsl";
  char fragmentShaderFilePath[] = "src/shaders/fragment.glsl";
  char textureFilePath[] = "assets/blocks/grass.png";

  const unsigned short int numShaders = 10;
  shader vertexShader, fragmentShader;
  vertexShader.Init(GL_VERTEX_SHADER, &(*vertexShaderFilePath));
  fragmentShader.Init(GL_FRAGMENT_SHADER, &(*fragmentShaderFilePath));

  shader shaders[numShaders] = { vertexShader, fragmentShader };
  unsigned int shaderProgram = glCreateProgram();
  for (int i = 0; i < numShaders; i++) glAttachShader(shaderProgram, shaders[i].getId());
  glLinkProgram(shaderProgram);

  int status;
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
  if (!status) {
    char logBuffer[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, logBuffer);
    printf("%s ", logBuffer);
    exit(-2);
  }
  for (int i = 0; i < numShaders; i++) shaders[i].Destroy();

  const unsigned int numBlocks = 1;
  const unsigned short int vertecePerBlock = 4;
  const unsigned int numVertices = numBlocks * vertecePerBlock;

  vertex vertices[numVertices];
  float bufferedVertices[] = {
    -0.50, 0.00, 0.00, 0.22, 0.63, 0.88, 0.00, 0.00,
    -0.30, 0.00, 0.00, 0.88, 0.32, 0.65, 1.00, 0.00,
    -0.50, -0.20, 0.00, 0.96, 0.87, 0.94, 0.00, 1.00,
    -0.30, -0.20, 0.00, 0.10, 0.98, 0.98, 1.00, 1.00,
  };
  int indices[] = { 0, 1, 2, 2, 3, 1 };
  for (int i = 0; i < numBlocks; i++) {
    printf("block %d", i);
    pushOneBlockToBuffer(bufferedVertices, i * vertecePerBlock * 8, i * 0.2 - 0.5, 0.0, 0.0, 0.2);
  }

  //? VBO is the buffer resides on the server side(the gpu) for unknown data, VAO is the description of the VBO in the cpu
  unsigned int VBO, VAO, EBO;
  glGenBuffers(1, &VBO);  //? vertex buffer object to send to the gpu
  glGenBuffers(1, &EBO); //? element array buffer to create a map of how should open gl traverse the vertices
  glGenVertexArrays(1, &VAO);  //? buffer description on the cpu

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(bufferedVertices), bufferedVertices, GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0); glEnableVertexAttribArray(1); glEnableVertexAttribArray(2);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(0));
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  int width, height, nrChannels;
  unsigned int texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  // set the texture wrapping parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  unsigned char* data = stbi_load(textureFilePath, &width, &height, &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  }
  else {
    std::cout << "Failed to load texture" << std::endl;
    exit(-2);
  }
  stbi_image_free(data);




    glm::mat4 transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
    transform = glm::rotate(transform, (float)0.25, glm::vec3(0.0f, 0.0f, 1.0f));

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  while (!glfwWindowShouldClose(window)) {
    glClear(GL_COLOR_BUFFER_BIT);
    processInput(window);

    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
    glUseProgram(shaderProgram);

    // create transformations

    // get matrix's uniform location and set matrix
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
