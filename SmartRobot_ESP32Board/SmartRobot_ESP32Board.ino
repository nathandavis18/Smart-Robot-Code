#include <WiFi.h>
#include <cmath>

#include "MessageSerializer.h"
#include "SmartRobotDtos.h"
#include "MyString.h"

#define RXD2 3  //The receiving pin for communication between the ESP32 and Uno board
#define TXD2 40 //The transmission pin for communication between the ESP32 and Uno board

const char* ssid = "SR_Test"; // Change to your laptop's hotspot name
const char* pass = "srtest1234"; // Change to your hotspot's password
IPAddress serverIp(192, 168, 137, 1); // Change to your laptop's hotspot IPv4 address
const int serverPort = 1818; // The port you choose in Mobius (can leave unchanged)
const int localPort = 100; // The port you want this robot to operate on

WiFiUDP udp;
bool connected = false;

bool pathToSend = false;
bool canSendToRobot = true;
int currentPathIndex = 0;
bool doneWithSegment = false;
static long previousPathId = -1;
static unsigned int currentPointId = 0;

sr::Attitude attitude{};
sr::Position position{};
sr::Velocity velocity{};
sr::CurrentAssignment currentAssignment;
sr::CurrentSegment currentSegment;
sr::RobotMessageData robotMsgData;

sr::MyVariant obj;
MyString message;
MyString receiveBuff;
MyString msgFromRobot;

void connectToWifi(){
  //WiFi.config(staticIP);
  WiFi.begin(ssid, pass);

  delay(200);
  if(WiFi.status() != WL_CONNECTED){
    while (WiFi.status() != WL_CONNECTED); //Wait for it to connect before moving on
  }
  Serial.println(F("Wifi ready!"));
}

void startUdp(){

  udp.begin(localPort);

  delay(500);

  Serial.println(F("Establishing connection with server . . ."));

  delay(500);

  sr::SmartRobotAsset asset;
  sr::serialize<sr::SmartRobotAsset>(asset, message);
  
  while(!connected){
    Serial.println(message);
    udp.beginPacket(serverIp, serverPort);
    udp.print(message);
    udp.endPacket();

    //Serial.println("Packet sent to server! Awaiting response . . .");
    delay(500);

    udp.parsePacket();
    if(udp.available()){
      udp.readString();
      connected = true; //Connection with server made
      Serial.println(F("Connection with server established!"));
    }
    if(!connected){
      delay(100);
    }
  }
  message.clear();
}

void checkAndHandleNewAssignment(){
  if(obj.pa.pathID != previousPathId){
    message = "{Smart Robot,AssignmentReceived}";
    message += (int)obj.pa.pathID;

    udp.beginPacket(serverIp, serverPort);
    udp.print(message);
    udp.endPacket();

    delay(50);

    udp.beginPacket(serverIp, serverPort);
    udp.print(message);
    udp.endPacket();

    previousPathId = obj.pa.pathID;
    currentPointId = obj.pa.waypoints[0].pointID;

    message.clear();
  }
  doneWithSegment = false;
}

void handleIncomingPacket(){
  //Serial.println("Test");
  udp.parsePacket();
  int x = udp.available();
  while(x){
    receiveBuff += (char)udp.read();
    --x;
  }
  if(receiveBuff.length()){
    Serial.println(F("Handling packet"));
    Serial.println(receiveBuff);
    //Serial.println(receiveBuff);
    sr::deserialize(receiveBuff, obj);

    if(obj.alternative == sr::MyVariant::alternative_t::none){
      Serial.println(F("Null received"));
      //Serial.println(receiveBuff);
    }
    else if(obj.alternative == sr::MyVariant::alternative_t::pathassignment){
      Serial.println(F("PathAssignment received"));
      //Serial.println(receiveBuff); //Just print the data for now

      currentPathIndex = 1;
      checkAndHandleNewAssignment();
      pathToSend = true;
    }
    
    message.clear();
    receiveBuff.clear();
  }
}

unsigned long prevDtoInterval = 0;
unsigned long currentDtoInterval;
unsigned long sendDtoInterval = 200;
bool sendAttitude = true;
bool sendPosition = false;
bool sendVelocity = false;

void sendDtos(){

  message.clear();

  if(doneWithSegment && robotMsgData.type == sr::MsgFromRobotType::Standby){
    message = "{Smart Robot,StandbyMode}";
    delay(50);
    udp.beginPacket(serverIp, serverPort);
    udp.print(message);
    udp.endPacket();
    delay(50);
  }

  currentDtoInterval = millis();
  if(currentDtoInterval - prevDtoInterval < sendDtoInterval) return;

  if(sendAttitude){
    sr::serialize<sr::Attitude>(attitude, message);
    udp.beginPacket(serverIp, serverPort);
    udp.print(message);
    udp.endPacket();

    sendPosition = true;
    sendAttitude = false;
    
    goto clearandreturn;
  }
  
  if(sendPosition){
    sr::serialize<sr::Position>(position, message);
    udp.beginPacket(serverIp, serverPort);
    udp.print(message);
    udp.endPacket();
    
    sendPosition = false;
    sendVelocity = true;
    goto clearandreturn;
  }

  if(sendVelocity){
    sr::serialize<sr::Velocity>(velocity, message);
    udp.beginPacket(serverIp, serverPort);
    udp.print(message);
    udp.endPacket();

    sendVelocity = false;
    sendAttitude = true;
    goto clearandreturn;
  }

clearandreturn:
  message.clear();
  prevDtoInterval = millis();
  return;
}

void sendPathToRobot(){
  if(!canSendToRobot) return;
  if(currentPathIndex >= obj.pa.waypoints.items()) return;

  double vel = obj.pa.waypoints[currentPathIndex].desiredVelocity;

  static double xDiff = obj.pa.waypoints[currentPathIndex].point.x - position.x;
  static double yDiff = obj.pa.waypoints[currentPathIndex].point.y - position.y;

  double distance = sqrt((xDiff * xDiff) + (yDiff * yDiff));

  double heading = obj.pa.waypoints[currentPathIndex].heading;

  message = "{\"v\":";
  message.concat(vel);

  message += ",\"d\":";
  message.concat(distance);

  message += ",\"h\":";
  message.concat(heading);

  message += "}";
  delay(50);

  bool received = false;
  while(!received){
    Serial2.print(message);
    Serial.print("Sending message to robot: ");
    Serial.println(message);

    delay(100);
    received = Serial2.available() > 0;
    delay(50);
  }

  //Serial2.readStringUntil('\b');
  
  velocity.x = fabs(obj.pa.waypoints[currentPathIndex].desiredVelocity * cos(heading));
  velocity.y = fabs(obj.pa.waypoints[currentPathIndex].desiredVelocity * sin(heading));

  attitude.yaw = heading;

  message.clear();
  canSendToRobot = false;

  ++currentPathIndex;
}

void handleRobotMessage(){
  robotMsgData.type = sr::MsgFromRobotType::None;
  //Serial.println(msgFromRobot);
  if(msgFromRobot.length() && msgFromRobot[0] == '{'){
    Serial.println(msgFromRobot);
    sr::deserializeRobotMessage(msgFromRobot, robotMsgData);
    if(robotMsgData.type == sr::MsgFromRobotType::Standby){
      canSendToRobot = true;
      if(obj.pa.waypoints.items() > 0 && currentPathIndex >= obj.pa.waypoints.items()){
        position.x = obj.pa.waypoints[currentPathIndex - 1].point.x;
        position.y = obj.pa.waypoints[currentPathIndex - 1].point.y;

        pathToSend = false;
        doneWithSegment = true;
      }
    }
    else if(robotMsgData.type == sr::MsgFromRobotType::Distance){
      position.x += robotMsgData.distance * cos(attitude.yaw);
      position.y += robotMsgData.distance * sin(attitude.yaw);
    }
  }
  msgFromRobot.clear();
}

void receiveFromRobot(){
  //Serial.println(F("Listening for robot message"));
  int x = Serial2.available();
  static char c = '\0';
  while(x){
    //Serial.println("There is something here from the robot");
    --x;
    c = (char)Serial2.read();
    //Serial.print(c);
    if(c != '\n' && c != '\t' && c != '\r' && c != '\x1b')
      msgFromRobot += c;

    if(c == '\x1b') {
      //Serial.println(msgFromRobot);
      handleRobotMessage();
      continue;
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  delay(2000);

  connectToWifi();

  delay(100);

  startUdp();

  delay(100);
}

void loop() {
  if(WiFi.status() == WL_CONNECTED){
    if(connected){
      receiveFromRobot();
      handleIncomingPacket();
      sendDtos();
      if(pathToSend){
        sendPathToRobot();
      }
      else{
        velocity.x = 0;
        velocity.y = 0;
      }
    }
    else{
      Serial.println(F("Connection Lost . . ."));
      udp.stop();
      delay(100);
      startUdp();
    }
  }
  else{
    /*
    Send Stop Command
    */
    udp.stop();
    Serial.println(F("WiFi disconnected"));
    Serial.println(F("Trying to reconnect"));
    connectToWifi();
    delay(2000);
    if(WiFi.status() == WL_CONNECTED){
      startUdp();
    }
  }
}
