struct Vector3
{
  float x;
  float y;
  float z;
};

class vertex
{
  static int vcount;

private:
  float _x, _y, _z;
  float _r, _g, _b;
  float _s, _t;
public:
  void fillRandom();
  void colorRandom();
  void posRandom();

  void setPosition(float x, float y, float z, float s, float t);
  void setColor(float r, float g, float b);
  void pushElementsToBuffer(float arr[], int from); 
  Vector3 getPosition();
};