#include <classes/vertex.h>
#include <math.h>
#include <time.h>
#include <cstdlib>
#include <stdio.h>

void vertex::setPosition(float x, float y, float z, float s, float t) {
  this->_x = x;
  this->_y = y;
  this->_z = z;
  this->_s = s;
  this->_t = t;
};
void vertex::setColor(float r, float g, float b) {
  this->_r = r;
  this->_g = g;
  this->_b = b;
};
Vector3 vertex::getPosition() {
  Vector3 res = { this->_x, this->_y, this->_z , };
  return res;
};
void vertex::pushElementsToBuffer(float arr[], int from) {
  arr[from + 0] = this->_x;
  arr[from + 1] = this->_y;
  arr[from + 2] = this->_z;
  arr[from + 3] = this->_r;
  arr[from + 4] = this->_g;
  arr[from + 5] = this->_b;
  arr[from + 6] = this->_s;
  arr[from + 7] = this->_t;
  printf("[%d] = ", from);
  for (int i = 0; i < 8; i++) {
    printf("%.2f, ", arr[(from)+i]);
  }
  printf("\n");


};
void vertex::colorRandom() {
  this->_s = 0.5 + sin(rand()) / 2.0;
  this->_t = 0.5 + sin(rand()) / 2.0;
  this->_r = 0.5 + sin(rand()) / 2.0;
  this->_g = 0.5 + sin(rand()) / 2.0;
  this->_b = 0.5 + sin(rand()) / 2.0;
};
void vertex::posRandom() {
  this->_x = sin(rand());
  this->_y = sin(rand());
  this->_z = 0.0;
  // this->_z = sin(rand());
};

void vertex::fillRandom() {
  this->colorRandom();
  this->posRandom();


}