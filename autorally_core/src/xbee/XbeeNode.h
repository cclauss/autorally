/**********************************************
 * @file XbeeNode.h
 * @author Brian Goldfain <bgoldfai@gmail.com>
 * @date May 17, 2013
 * @copyright 2012 Georgia Institute of Technology
 * @brief
 *
 * @details This file contains the XbeeNode class definition
 ***********************************************/

#include "XbeeInterface.h"

#include <boost/lexical_cast.hpp>
#include <std_msgs/ByteMultiArray.h>
#include <nav_msgs/Odometry.h>
#include <autorally_msgs/safeSpeed.h>

/**
 *  @class XbeeNode XbeeNode.h
 *  "xbee/XbeeNode.h"
 *  @brief Publishes ROS messages from a Coordinator xbee into system
 *
 *  XbeeNode is designed to allow an alternate wireless communication channel than wifi,
 *  and receive messages from a central coordinator that can send important information
 *  to multiple robots at the same time. The current data sent over Xbee is a safeSpeed
 *  message from the runStop as well as RTK correction data from the gps base station.
 *  Also, each node commicates it's current status to coordinator.
 *
 *  @todo Make return statuses and such more visible outside of this class
 *  @todo Maybe add more information to message to coordinator?
 *        (current speed, heading, gps location)
 */
class XbeeNode
{
 public:

  XbeeNode(ros::NodeHandle &nh, const std::string& port);
  ~XbeeNode();

 private:
  XbeeInterface m_xbee; ///< Xbee object, handles communication with device
  ros::NodeHandle m_nh; ///< local copy
  ros::Publisher m_safeSpeedPublisher; ///< Subscriber for safeSpeed
  ros::Publisher m_gpsRTCM3Publisher; ///< Publisher for RTK correction data from xbee
  ros::Subscriber m_poseSubscriber; ///<
  ///< publishers for odomoetry msgs received over xbee
  std::map<std::string, ros::Publisher> m_recOdomPublishers; 
  ros::Timer m_hiTimer; ///< Startup timer to register with coordinator
  ros::Timer m_stateTimer; ///< timer to send heartbeat to xbee coordinator
  ros::Timer m_xbeeHeartbeatTimer; ///< timer to publish safeSpeed into system
  ros::Timer m_transmitPositionTimer; ///< timer to publish safeSpeed into system
  ros::Time m_lastSafeSpeed; ///< Last receive time of safeSpeed over xbee
  ros::Time m_lastTargetedSafeSpeed; ///< last targeted safeSPeed from coordinator
  ros::Time m_lastXbeeOdomTransmit; ///< last targeted safeSPeed from coordinator


  std::string m_coordinatorAddress; ///< Xbee addess of coordinator
  autorally_msgs::safeSpeed m_safeSpeed; ///< SafeSpeed message to send
  std::string m_msgLabel; ///< RTCM3 message header from xbee
  std::string m_gpsString; ///< buffer to store incoming message data as packets arrive
  int m_prevGpsMsgNum; ///< sequence number for received message
  nav_msgs::Odometry m_odometry;
  /**
   * @brief Send startup information to xbee coordinator and wait for confirmation
   * @param time timer information
   *
   */
  void sendHi(const ros::TimerEvent& time);
  
  /**
   * @brief Periodically send state information to xbee coordinator
   * @param time timer information
   *
   */
  void sendState(const ros::TimerEvent& time);
  
  /**
   * @brief Update ROS system on state of incoming data from xbee
   * @param time timer information
   *
   * Sends error safeSpeed message if no data is being received from xbee
   */
  void xbeeHeartbeatState(const ros::TimerEvent& time);
  

  /**
   * @brief Parse a message received over USB from xbee
   * @param sender identifier for xbee that sent message
   * @param networkAddress address of the originating xbee network
   * @param data message payload
   * @param broadcast if the message was targeted or broadcast
   *
   */
  void processXbeeMessage(const std::string &sender,
                          const std::string &networkAddress,
                          const std::string &data,
                          const bool broadcast);

  void odomCallback(const nav_msgs::OdometryConstPtr& odom);
  void transmitPosition(const ros::TimerEvent& time);
  int scaleAndClip(double number, double range, double resolution);
  double unscaleAndClip(int number, double range, double resolution);
  void processXbeeOdom(const std::string& message,const std::string& sender);
};
