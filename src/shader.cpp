#include <iostream>
#include <classes/shader.h>

#ifndef __glad_h
#include <glad/glad.h>
#endif
using namespace std;

void shader::Init(int type, char* sourceFile) {
  char* sourceCode = this->getFileContent(sourceFile);
  this->_shaderid = glCreateShader(type);
  glShaderSource(this->_shaderid, 1, &sourceCode, NULL);
  glCompileShader(this->_shaderid);
  
  int status;
  glGetShaderiv(this->_shaderid, GL_COMPILE_STATUS, &status);
  if (!(status == GL_TRUE)) this->handleShaderError(this->_shaderid);

};
void shader::Destroy() {
  glDeleteShader(this->_shaderid);
}
unsigned int shader::getId() {
  return this->_shaderid;
}
