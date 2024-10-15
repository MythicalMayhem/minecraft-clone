#include <iostream>
#include <fstream> 
#include <string.h>

#ifndef __glad_h
#include <glad/glad.h>
#endif

using namespace std;

class shader
{
private:
  unsigned int  _shaderid;
  void handleShaderError(unsigned int shader) {
    char logBuffer[512];
    glGetShaderInfoLog(shader, 512, NULL, logBuffer);
    printf("shader error : \t\n %s ", logBuffer);
    exit(-2);
  };
  char* getFileContent(char* FileLocation) {
    ifstream file(FileLocation);
    if (!file.is_open()) {
      std::cout << "file doesn't exist" << std::endl;
      exit(-2);
    }
    string text;
    char ch;
    while (file.get(ch)) text += ch;
    file.close();
    char* p = new char[text.length() + 1];
    strcpy(p, text.c_str());

    return p;
  };
public:
  void Init(int type, char* sourceFilePath);
  void Destroy();
  unsigned int getId();
};

