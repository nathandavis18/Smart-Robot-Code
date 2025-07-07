#ifndef _POINT3
#define _POINT3

struct Point3{
  float x, y, z;
  Point3() : x(0), y(0), z(0) {}
  Point3(float newX, float newY, float newZ) : x(newX), y(newY), z(newZ) {}
  Point3& operator=(const Point3& other){
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;

    return *this;
  }
};

struct Attitude{
  int16_t roll, pitch, yaw;
  Attitude() : roll(0), pitch(0), yaw(0) {}
  Attitude(int16_t r, int16_t p, int16_t y) : roll(r), pitch(p), yaw(y) {}
  Attitude& operator=(const Attitude& other){
    this->roll = other.roll;
    this->pitch = other.pitch;
    this->yaw = other.yaw;

    return *this;
  }
};

#endif //_POINT3