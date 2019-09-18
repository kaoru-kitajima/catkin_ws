#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>

#include <moveit_msgs/DisplayRobotState.h>
#include <moveit_msgs/DisplayTrajectory.h>

#include <moveit_msgs/AttachedCollisionObject.h>
#include <moveit_msgs/CollisionObject.h>

#include <moveit_visual_tools/moveit_visual_tools.h>

#include <sensor_msgs/Image.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>

#define WIDTH   25
#define HEIGHT  25
 

  // BEGIN_TUTORIAL
  //
  // Setup
  // ^^^^^
  //
  // MoveIt operates on sets of joints called "planning groups" and stores them in an object called
  // the `JointModelGroup`. Throughout MoveIt the terms "planning group" and "joint model group"
  // are used interchangably.
  static const std::string PLANNING_GROUP = "arm1";

  // The :move_group_interface:`MoveGroupInterface` class can be easily
  // setup using just the name of the planning group you would like to control and plan for.
  moveit::planning_interface::MoveGroupInterface move_group(PLANNING_GROUP);

void depthImageCallback(const sensor_msgs::Image::ConstPtr& msg){
 
    int x1, x2, y1, y2;
    int i, j, k;
    int width = WIDTH;
    int height = HEIGHT;
    double sum = 0.0;
    double ave;
    cv_bridge::CvImagePtr cv_ptr;
 
    try{
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::TYPE_32FC1);
    }catch (cv_bridge::Exception& ex){
        ROS_ERROR("error");
        exit(-1);
    }
 
    cv::Mat depth(cv_ptr->image.rows, cv_ptr->image.cols, CV_32FC1);
    cv::Mat img(cv_ptr->image.rows, cv_ptr->image.cols, CV_8UC1);
 
    x1 = int(depth.cols / 2) - width;
    x2 = int(depth.cols / 2) + width;
    y1 = int(depth.rows / 2) - height;
    y2 = int(depth.rows / 2) + height;
 
    for(i = 0; i < cv_ptr->image.rows;i++){
        float* Dimage = cv_ptr->image.ptr<float>(i);
        float* Iimage = depth.ptr<float>(i);
        char* Ivimage = img.ptr<char>(i);
        for(j = 0 ; j < cv_ptr->image.cols; j++){
            if(Dimage[j] > 0.0){
                Iimage[j] = Dimage[j];
                Ivimage[j] = (char)(255*(Dimage[j]/5.5));
            }else{
            }
 
            if(i > y1 && i < y2){
                if(j > x1 && j < x2){
                    if(Dimage[j] > 0.0){
                        sum += Dimage[j];
                    }
                }
            }
        }
    }
 
    ave = sum / ((width * 2) * (height * 2));
    ROS_INFO("depth : %f [m]", ave);
 
    cv::imshow("DEPTH image", img);

  // Planning to a Pose goal
  // ^^^^^^^^^^^^^^^^^^^^^^^
  // We can plan a motion for this group to a desired pose for the
  // end-effector.
  geometry_msgs::Pose target_pose1;
  target_pose1.orientation.w = 1.0;
  target_pose1.position.x = ave/1000;
  target_pose1.position.y = -0.2;
  target_pose1.position.z = 0.5;
  move_group.setPoseTarget(target_pose1);

  // Now, we call the planner to compute the plan and visualize it.
  // Note that we are just planning, not asking move_group
  // to actually move the robot.
  moveit::planning_interface::MoveGroupInterface::Plan my_plan;

  bool success = (move_group.plan(my_plan) == moveit::planning_interface::MoveItErrorCode::SUCCESS);

  ROS_INFO_NAMED("tutorial", "Visualizing plan 1 (pose goal) %s", success ? "" : "FAILED");
    cv::waitKey(10);
}
int main(int argc, char** argv)
{
  ros::init(argc, argv, "haircut_head_traser");

  ros::NodeHandle node_handle;
  ros::Subscriber sub_depth;
  ros::AsyncSpinner spinner(1);
  spinner.start();


  // We will use the :planning_scene_interface:`PlanningSceneInterface`
  // class to add and remove collision objects in our "virtual world" scene
  moveit::planning_interface::PlanningSceneInterface planning_scene_interface;

  // Raw pointers are frequently used to refer to the planning group for improved performance.
  const robot_state::JointModelGroup* joint_model_group =
      move_group.getCurrentState()->getJointModelGroup(PLANNING_GROUP);
  // Visualization
  // ^^^^^^^^^^^^^
  //
  // The package MoveItVisualTools provides many capabilties for visualizing objects, robots,
  // and trajectories in RViz as well as debugging tools such as step-by-step introspection of a script
  namespace rvt = rviz_visual_tools;
  moveit_visual_tools::MoveItVisualTools visual_tools("base_link");
  visual_tools.deleteAllMarkers();

  // Remote control is an introspection tool that allows users to step through a high level script
  // via buttons and keyboard shortcuts in RViz
  visual_tools.loadRemoteControl();

  // RViz provides many types of markers, in this demo we will use text, cylinders, and spheres
  Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
  text_pose.translation().z() = 1.75;
  visual_tools.publishText(text_pose, "MoveGroupInterface Demo", rvt::WHITE, rvt::XLARGE);

  // Batch publishing is used to reduce the number of messages being sent to RViz for large visualizations
  visual_tools.trigger();

  // Getting Basic Information
  // ^^^^^^^^^^^^^^^^^^^^^^^^^
  //
  // We can print the name of the reference frame for this robot.
  ROS_INFO_NAMED("tutorial", "Planning frame: %s", move_group.getPlanningFrame().c_str());

  // We can also print the name of the end-effector link for this group.
  ROS_INFO_NAMED("tutorial", "End effector link: %s", move_group.getEndEffectorLink().c_str());

  // We can get a list of all the groups in the robot:
  ROS_INFO_NAMED("tutorial", "Available Planning Groups:");
  std::copy(move_group.getJointModelGroupNames().begin(), move_group.getJointModelGroupNames().end(),
            std::ostream_iterator<std::string>(std::cout, ", "));
  // Start the demo
  // ^^^^^^^^^^^^^^^^^^^^^^^^^
  visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to start the demo");

  

  sub_depth = node_handle.subscribe<sensor_msgs::Image>("/kinect2/sd/image_depth_rect", 100, &depthImageCallback);
}