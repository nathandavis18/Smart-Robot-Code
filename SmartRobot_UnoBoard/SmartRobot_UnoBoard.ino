#include <avr/wdt.h>
#include "SmartRobot.h"

SmartRobot robot;
void setup() {
  // put your setup code here, to run once:
  robot.SmartRobotInit();
  //wdt_enable(WDTO_2S);
}

void loop() {
  // put your main code here, to run repeatedly:
  //wdt_reset();
  if(!robot.isMoving)
    robot.getSerialData();
  else{
    robot.updateDistanceData();
  }
}
