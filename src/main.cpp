#include <iostream> 
#include <unordered_map>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <string> 
#include <vector>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>


#include <stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <classes/shader.h> 
#include <classes/vertex.h>
#include <SimplexNoise.h>



using namespace std;
// settings
const unsigned short int SCR_WIDTH = 1000;
const unsigned short int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(-20.0f, 0.0f, 10.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(20.0f, 0.0f, 10.0f) - cameraPos);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);


float mouseX = SCR_WIDTH / 2.0f, mouseY = SCR_HEIGHT / 2.0f;
float yaw = -0.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
  if (firstMouse) {
    mouseX = (float)xpos;
    mouseY = (float)ypos;
    firstMouse = false;
    return;
  }

  float xoffset = (float)xpos - mouseX;
  float yoffset = mouseY - (float)ypos;
  mouseX = (float)xpos;
  mouseY = (float)ypos;

  const float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;
  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)  pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  glm::vec3 direction;
  direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  direction.y = sin(glm::radians(pitch));
  direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
  cameraFront = glm::normalize(direction);
};
GLFWwindow* InitializeWindow() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ALL PRAISE DUE TO THE MOST HIGH ALLAH", NULL, NULL);
  if (!window) {
    printf("Failed to create GLFW window");
    glfwTerminate();
    exit(-1);
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);
  gladLoadGL();
  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

  return window;
}
void processInput(GLFWwindow* window, float dt) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);

  float cameraSpeed = 10.0f * dt;
  if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) cameraPos -= glm::vec3(0.0f, cameraSpeed, 0.0f);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) cameraPos += glm::vec3(0.0f, cameraSpeed, 0.0f);
}

int main(int argc, char const* argv[]) {
  GLFWwindow* window = InitializeWindow();
  char vertexShaderFilePath[] = "src/shaders/vertex.vs";
  char fragmentShaderFilePath[] = "src/shaders/fragment.fs";
  char textureFilePath[] = "assets/blocks/grass.png";

  glEnable(GL_DEPTH_TEST);
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
  unsigned int  indices[] = {
    0, 1, 2, 2, 3, 0,//front
    0, 4, 1, 1, 5, 4,//left
    4, 6, 5, 6, 7, 4,//back 
    3, 6, 7, 6, 2, 3,//right
    1, 2, 5, 5, 6, 2, //bottom
    0, 4, 7, 7, 0, 3  //top
  };
  float bufferedVertices[] = {
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, // front upright
     0.5f, -0.5f,  0.5f, 1.0f, 0.0f, // front downright
    -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, // front downleft
    -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, // front upleft
     0.5f,  0.5f, -0.5f, 0.0f, 1.0f, // right up right
     0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // right down right
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, // back downleft right
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, // back upleft right
  };


  //? VBO is the buffer resides on the server side(the gpu) for unknown data, VAO is the description of the VBO in the cpu
  unsigned int VBO, VAO, EBO;
  glGenBuffers(1, &VBO);  //? vertex buffer object to send to the gpu
  glGenBuffers(1, &EBO); //? element array buffer to create a map of how should open gl traverse the vertices
  glGenVertexArrays(1, &VAO);  //? buffer description on the cpu

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
  glBufferData(GL_ARRAY_BUFFER, sizeof(bufferedVertices), bufferedVertices, GL_STATIC_DRAW);


  glEnableVertexAttribArray(0); glEnableVertexAttribArray(1);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0));
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));


  int width, height, nrChannels;
  unsigned int texture;
  stbi_set_flip_vertically_on_load(true);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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
  glUseProgram(shaderProgram);

  glm::mat4 projection = glm::mat4(1.0f);
  projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
  unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
  glfwSetTime(0.0f);
  glBindTexture(GL_TEXTURE_2D, texture);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  // Initialize ImGU
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");


  int maxHeight = 10;
  const int renderdist = 1;
  SimplexNoise noise(0.01, 1, 2.0, 0.5);

  struct block {
    glm::mat4 model;
    int y = 0;
    bool exists = false;
  };

  float dt = 0.0f;
  while (!glfwWindowShouldClose(window)) {
    glfwSetTime(0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Begin("ImGUI window");
    processInput(window, dt);

    glUseProgram(shaderProgram);

    glm::mat4 view = glm::mat4(1.0f);
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));


    int startx = cameraPos.x / 16;
    int startz = cameraPos.z / 16;

    if (startx <= 0) startx--; else startx++ ;
    if (startz <= 0) startz--; else startz++ ;

    block world[renderdist * 3 * 16][renderdist * 3 * 16];

 
    block previousBlock;
    for (float cx = startx - renderdist; cx < startx + renderdist + 1; cx++ ) {
      for (int cz = startz - renderdist; cz < startz + renderdist + 1; cz++ ) // -1 ??
        for (int i = 0; i < 16; i++)
          for (int j = 0; j < 16; j++) {
            int x = i + cx * 16;
            int z = j + cz * 16;
            int y = noise.fractal(5, (float)x, (float)z) * maxHeight;

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
            unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));


            int iterx = i + (cx - (startx - renderdist)) * 16;
            int iterz = j + (cz - (startz - renderdist)) * 16;

            world[iterx][iterz].y = y;
            world[iterx][iterz].model = model;
            world[iterx][iterz].exists = true;
            
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(30 * sizeof(unsigned int))); //top
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(24 * sizeof(unsigned int))); //bottom

            if (!(world[iterx][iterz - 1].exists && (world[iterx][iterz - 1].y == world[iterx][iterz].y))) {
              glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(12 * sizeof(unsigned int)));
              glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(world[iterx][iterz - 1].model));
              glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
              glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(world[iterx][iterz].model));
            }

            if (!(world[iterx - 1][iterz].exists && (world[iterx - 1][iterz].y == world[iterx][iterz].y))) {
              glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(18 * sizeof(unsigned int)));
              glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(world[iterx - 1][iterz].model));
              glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(6 * sizeof(unsigned int)));
            }

          }
    }


    ImGui::Text(to_string(1 / (dt + 0.0001)).c_str());
    ImGui::Text(to_string(cameraPos.x).c_str());
    ImGui::Text(to_string(cameraPos.z).c_str());
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();

    dt = glfwGetTime();
    glfwSetTime(0);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glfwDestroyWindow(window);
  glfwTerminate();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  return 0;
}
