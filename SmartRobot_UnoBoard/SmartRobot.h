#ifndef _SmartRobot
#define _SmartRobot

#include <Arduino.h>
#include "DeviceDrivers.h"
#include "MPU6050_getdata.h"
#include "Point3.h"

class SmartRobot{
public:
  SmartRobot(void);

  void SmartRobotInit(void);

  void updateMotion(float speed, float distance);
  void updateAngle(float newHeading);
  void calibrateGyro(void);
  float getHeading(void);
  void setActualHeading(float heading);
  void setActualPos(Point3 newPos);
  void getSerialData(void);
  void updateDistanceData(void);
  void updateDistanceLeft(void);
  void sendDistanceMoved(bool forced = false);

public:
  bool isMoving = false;

private:
  enum SmartRobotMode{
    Standby,
    Moving,
    Estop,
    Unknown
  };

private:
  void updateRobotAngle(bool leftDirection, bool rightDirection, float headingToFace);
  void moveRobot(uint8_t speed, bool forward = true);
  void stopRobot();

private:
  float _currentHeading;
  Point3 _currentPos;
  SmartRobotMode _mode;
  MPU6050_getdata _mpu6050;
  DeviceDriver_Motor _motorControl;
  DeviceDriver_Voltage _voltageControl;
  DeviceDriver_Key _keyControl;

  static constexpr float angleEpsilon = 0.5;
};

#endif //_SmartRobot