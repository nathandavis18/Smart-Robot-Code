#include "MessageSerializer.h"
#include "Split.h"
#include "MyString.h"

namespace sr{
  MyString header, other;
  MyArray<MyString, 2> headerParts;
  MyArray<MyString, 6> waypointParts;
  Waypoint wp;

  void deserializeRobotMessage(const MyString& msg, RobotMessageData& outObj){
    header = msg.substring(1, msg.indexOf('}') - 1);

    split<2>(header, headerParts);
    if(headerParts[1] == "Standby"){
      outObj.type = MsgFromRobotType::Standby;
    }
    else if(headerParts[1] == "Distance"){
      outObj.type = MsgFromRobotType::Distance;
      other = msg.substring(msg.indexOf('}') + 1);
      outObj.distance = other.toDouble();
    }
    else{
      outObj.type == MsgFromRobotType::None;
    }
    
    headerParts.clear();
    header.clear();
    other.clear();
  }

  void deserializePathAssignment(const MyString& msg, PathAssignment& pa){
    pa.pathID = msg.substring(0, msg.indexOf(',')).toInt();

    unsigned int currentIndex = msg.indexOf(',') + 1;
    while(currentIndex < msg.length()){
      if(msg.charAt(currentIndex) == '['){
        split<6>(msg.substring(currentIndex, msg.indexOf(']', currentIndex) - currentIndex), waypointParts);

        wp.pointID = waypointParts[0].toInt();
        wp.desiredVelocity = waypointParts[1].toDouble();
        wp.point.x = waypointParts[2].toDouble();
        wp.point.y = waypointParts[3].toDouble();
        wp.point.z = waypointParts[4].toDouble();
        wp.heading = waypointParts[5].toDouble();

        pa.waypoints.insert(wp);

        currentIndex = msg.indexOf(']', currentIndex);
        waypointParts.clear();
      }
      ++currentIndex;
    }
    waypointParts.clear();
  }

  void deserialize(const MyString& msg, MyVariant& outObj){
    header = msg.substring(1, msg.indexOf('}') - 1);
    
    split<2>(header, headerParts);

    if(headerParts[1] == "PathAssignment"){
      other = msg.substring(msg.indexOf('}') + 1);

      outObj.alternative = MyVariant::alternative_t::pathassignment;
      outObj.pa.waypoints.clear();
      deserializePathAssignment(other, outObj.pa);
    }
    else{
      outObj.alternative = MyVariant::alternative_t::none;
    }
    headerParts.clear();
    header.clear();
    other.clear();
  }

  void serializeWaypoints(const PathAssignment& obj, MyString& outMsg){
      const Waypoint* wp = nullptr;
      outMsg = "{Smart Robot,PathAssignment}";
      outMsg += (int)obj.pathID;
      for (unsigned int i = 0; i < obj.waypoints.items(); ++i) {
		  wp = &obj.waypoints.at(i);

          outMsg += ",[";
          outMsg += (int)wp->pointID;
          outMsg += ",";
          outMsg += wp->desiredVelocity;
          outMsg += ",";
          outMsg += wp->point.x;
          outMsg += ",";
          outMsg += wp->point.y;
          outMsg += ",";
          outMsg += wp->point.z;
          outMsg += ",";
          outMsg += wp->heading;
          outMsg += "]";
      }
  }

  void serializeAsset(const SmartRobotAsset& obj, MyString& outMsg){
      outMsg = "{Smart Robot,SmartRobotAsset}";
	  outMsg += obj.name;
	  outMsg += ",";
      outMsg += obj.front;
      outMsg += ",";
      outMsg += obj.back;
      outMsg += ",";
      outMsg += obj.left;
      outMsg += ",";
      outMsg += obj.right;
      outMsg += ",";
      outMsg += obj.height;
  }

  void serializeVelocity(const Velocity& obj, MyString& outMsg){
      outMsg = "{Smart Robot,Velocity}";
      outMsg += obj.x;
      outMsg += ",";
      outMsg += obj.y;
      outMsg += ",";
      outMsg += obj.z;
      outMsg += ",";
      outMsg += obj.rollRate;
      outMsg += ",";
      outMsg += obj.pitchRate;
      outMsg += ",";
      outMsg += obj.yawRate;
  }

  void serializePosition(const Position& obj, MyString& outMsg){
      outMsg = "{Smart Robot,Position}";
      outMsg += obj.x;
      outMsg += ",";
      outMsg += obj.y;
      outMsg += ",";
      outMsg += obj.z;
  }

  void serializeAttitude(const Attitude& obj, MyString& outMsg){
      outMsg = "{Smart Robot,Attitude}";
      outMsg += obj.roll;
      outMsg += ",";
      outMsg += obj.pitch;
      outMsg += ",";
      outMsg += obj.yaw;
  }

  void serializeCurrentAssignment(const CurrentAssignment& obj, MyString& outMsg){
    outMsg = "{Smart Robot,CurrentAssignment}";
    outMsg += (int)obj.pathID;
  }

  void serializeCurrentSegment(const CurrentSegment& obj, MyString& outMsg){
    outMsg = "{Smart Robot,CurrentSegment}";
    outMsg += (int)obj.pointID;
  }
}