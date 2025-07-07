#include "SmartRobot.h"
#include "ArduinoJson-v6.11.1.h"

using ARDUINOJSON_NAMESPACE::StaticJsonDocument;

static String message;
static float distanceLeft = 0;
static float startingDistance = 0;
static float currentVelocity = 0;
static float currentDistanceMoved = 0;
unsigned long lastDistanceInterval = 0;

SmartRobot::SmartRobot(void) : _mode(SmartRobotMode::Standby), _currentHeading(0) {
}


void sendStandbyMode(void){
    delay(150);
    Serial.println(F("{Smart Robot,Standby}\x1b"));
    delay(50);
}

void sendReceived(void){
  delay(150);
  Serial.println(F("\x1b"));
  delay(50);
}

void SmartRobot::SmartRobotInit(){
  Serial.begin(9600);
  delay(500);
  _voltageControl.VoltageInit();
  _motorControl.MotorInit();
  _keyControl.KeyInit();

  bool error = _mpu6050.MPU6050_dveInit();
  calibrateGyro();

  //sendStandbyMode();
}

void SmartRobot::getSerialData(){ 
  if(_mode == SmartRobotMode::Unknown || _mode == SmartRobotMode::Estop) return;
  static String buffer = "";

  char c = "";
  int x = Serial.available();
  while(x){
    c = Serial.read();
    if(c != '\n' && c != ' ' && c != '\t' && c != '\r')
      buffer += c;
    --x;
    if(c == '}') break;
  }
  
  if(c == '}' && buffer[0] == '{'){
    StaticJsonDocument<200> doc;
    deserializeJson(doc, buffer);
    buffer = "";

    float velocity = doc["v"];
    float distance = doc["d"];
    float heading = doc["h"];

    doc.clear();

    startingDistance = fabs(distance);
    distanceLeft = fabs(distance);
    currentVelocity = fabs(velocity);

    _mode = SmartRobotMode::Moving;

    sendReceived();

    updateAngle(heading);
    isMoving = true;
    lastDistanceInterval = millis();
    if(distance > 0)
      updateMotion(velocity, distance);
  }
  else if(buffer.length() && buffer[0] != '{') buffer = "";

  //if(_mode == SmartRobotMode::Standby) sendStandbyMode();
}

unsigned long currentInterval;
unsigned long timeDiff;
constexpr uint8_t minTimeDiff = 20;

void SmartRobot::updateDistanceData(){
  currentInterval = millis();
  timeDiff = currentInterval - lastDistanceInterval;

  currentDistanceMoved = currentVelocity * (float)(timeDiff / 1000.0) * 10;

  updateDistanceLeft();
  lastDistanceInterval = currentInterval;

  if(isMoving){
    sendDistanceMoved();
  }
  else{
    stopRobot();
    delay(10);
    stopRobot(); //make sure it is stopped

    _mode = SmartRobotMode::Standby;
    sendDistanceMoved(true);
    delay(50);
    sendStandbyMode();
    
    distanceLeft = 0;
    currentDistanceMoved = 0;
  }
}

unsigned long lastDistanceMovedInterval = 0;
unsigned long distanceMovedInterval = 200;
float distanceMovedSinceLastSend = 0;

void SmartRobot::sendDistanceMoved(bool forced){
  distanceMovedSinceLastSend += currentDistanceMoved;

  if(millis() - lastDistanceMovedInterval < distanceMovedInterval && !forced) return;

  message = "{Smart Robot,Distance}";
  message.concat(distanceMovedSinceLastSend);
  message.concat('\x1b');
  Serial.println(message);

  message = "";
  lastDistanceMovedInterval = millis();
  distanceMovedSinceLastSend = 0;
}

void SmartRobot::updateDistanceLeft(){
  distanceLeft -= currentDistanceMoved;
  //Serial.println(distanceLeft);

  if(distanceLeft <= 0)
    isMoving = false;
}

bool getRotationDir(float currentHeading, float newHeading){ //True is counter-clockwise
  if(newHeading < 0 && currentHeading > 0)
    return newHeading < (3.14 - currentHeading);

  if(newHeading > 0 && currentHeading < 0)
    return newHeading > (3.14 + currentHeading);
  
  return newHeading > currentHeading;

}

void SmartRobot::updateAngle(float newHeading){
  if(_currentHeading == newHeading) return;

  if(getRotationDir(_currentHeading, newHeading)) { //Go clock-wise
    updateRobotAngle(false, true, newHeading * 180 / 3.14);
  }
  else{
    updateRobotAngle(true, false, newHeading * 180 / 3.14);
  }

  _currentHeading = newHeading;
}

void SmartRobot::updateMotion(float speed, float distance){
  uint16_t speedUnits = speed * 50;
  //Serial.println(speedUnits);
  if(speedUnits > 255) speedUnits = 255;

  moveRobot(speedUnits);
}

void SmartRobot::updateRobotAngle(bool leftDirection, bool rightDirection, float headingToFace){
    _motorControl.setMotorControl(leftDirection, 100, rightDirection, 100);

    while(!(getHeading() - angleEpsilon < headingToFace && getHeading() + angleEpsilon > headingToFace)) {}

    _motorControl.setMotorControl(leftDirection, 0, rightDirection, 0);
}

void SmartRobot::calibrateGyro(){
  _mpu6050.MPU6050_calibration();
}

float SmartRobot::getHeading(){
  float yaw;

  _mpu6050.MPU6050_dveGetEulerAngles(&yaw);

  return yaw;
}

void SmartRobot::moveRobot(uint8_t speed, bool forward){
  _motorControl.setMotorControl(forward, speed, forward, speed);
}

void SmartRobot::stopRobot(){
  _motorControl.setMotorControl(false, 0, false, 0);
}

void SmartRobot::setActualHeading(float heading){
  _currentHeading = heading;
}

void SmartRobot::setActualPos(Point3 newPos){
  _currentPos = newPos;
}

