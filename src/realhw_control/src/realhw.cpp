#include <ros/package.h>
#include <angles/angles.h>
#include <realhw.h>
#include <iostream> // for debug
#include <math.h>

Assembly_copy::Assembly_copy()
{
  
  // connect and register the joint state interface
  hardware_interface::JointStateHandle state_handle_1("rev1", &pos_[0], &vel_[0], &eff_[0]);
  jnt_state_interface.registerHandle(state_handle_1);
  hardware_interface::JointStateHandle state_handle_2("rev2", &pos_[1], &vel_[1], &eff_[1]);
  jnt_state_interface.registerHandle(state_handle_2);
  hardware_interface::JointStateHandle state_handle_3("rev3", &pos_[2], &vel_[2], &eff_[2]);
  jnt_state_interface.registerHandle(state_handle_3);
  hardware_interface::JointStateHandle state_handle_4("rev4", &pos_[3], &vel_[3], &eff_[3]);
  jnt_state_interface.registerHandle(state_handle_4);
  hardware_interface::JointStateHandle state_handle_5("rev5", &pos_[4], &vel_[4], &eff_[4]);
  jnt_state_interface.registerHandle(state_handle_5);
  hardware_interface::JointStateHandle state_handle_6("rev6", &pos_[5], &vel_[5], &eff_[5]);
  jnt_state_interface.registerHandle(state_handle_6);

  registerInterface(&jnt_state_interface);

  // connect and register the joint position interface
  hardware_interface::JointHandle pos_handle_1(jnt_state_interface.getHandle("rev1"), &cmd_[0]);
  jnt_pos_interface.registerHandle(pos_handle_1);
  hardware_interface::JointHandle pos_handle_2(jnt_state_interface.getHandle("rev2"), &cmd_[1]);
  jnt_pos_interface.registerHandle(pos_handle_2);
  hardware_interface::JointHandle pos_handle_3(jnt_state_interface.getHandle("rev3"), &cmd_[2]);
  jnt_pos_interface.registerHandle(pos_handle_3);
  hardware_interface::JointHandle pos_handle_4(jnt_state_interface.getHandle("rev4"), &cmd_[3]);
  jnt_pos_interface.registerHandle(pos_handle_4);
  hardware_interface::JointHandle pos_handle_5(jnt_state_interface.getHandle("rev5"), &cmd_[4]);
  jnt_pos_interface.registerHandle(pos_handle_5);
  hardware_interface::JointHandle pos_handle_6(jnt_state_interface.getHandle("rev6"), &cmd_[5]);
  jnt_pos_interface.registerHandle(pos_handle_6);

  registerInterface(&jnt_pos_interface);
}

void Assembly_copy::timeCallback(const std_msgs::Float32MultiArray::ConstPtr& msg_sub)
{
  pos_[0]=msg_sub->data[0]; //set pos_[0] as real_hw date
  pos_[1]=msg_sub->data[1];
  pos_[2]=msg_sub->data[2];
  pos_[3]=msg_sub->data[3];
  pos_[4]=msg_sub->data[4];
  pos_[5]=msg_sub->data[5];

}

void Assembly_copy::read(ros::Time time, ros::Duration period)
{

  sub = nh.subscribe("response" ,100000,&Assembly_copy::timeCallback, this) ; //to use subscribe in class, use "this"

}

//void Assembly_copy::write(ros::Time time, ros::Duration period,std_msgs::Float32 msg_pub)
void Assembly_copy::write(ros::Time time, ros::Duration period)
{
  //msg_pub.data=0.01;
  std_msgs::Float32MultiArray msg_pub;
  msg_pub.data.resize(6);
  ROS_INFO("resized msg_pub");
  msg_pub.data[0]=cmd_[0];
  msg_pub.data[1]=cmd_[1];
  msg_pub.data[2]=cmd_[2];
  msg_pub.data[3]=cmd_[3];
  msg_pub.data[4]=cmd_[4];
  msg_pub.data[5]=cmd_[5];
  ROS_INFO("input value");
  pub = nh.advertise<std_msgs::Float32MultiArray>("request",100000);
  ROS_INFO("initialized publisher");
  pub.publish(msg_pub);
  ROS_INFO("published");
}