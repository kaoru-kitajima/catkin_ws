#include <ros/ros.h>
#include <controller_manager/controller_manager.h>
#include <realhw.h>

int main(int argc, char *argv[])
{
  ros::init(argc, argv, "assembly_copy");

  Assembly_copy assembly_copy;
  controller_manager::ControllerManager cm(&assembly_copy, assembly_copy.nh);

  ros::Rate rate(1 / assembly_copy.getPeriod().toSec());
  ros::AsyncSpinner spinner(1);
  spinner.start();

  while(ros::ok())
  {
    ros::Time now = assembly_copy.getTime();
    ros::Duration dt = assembly_copy.getPeriod();

    assembly_copy.read(now, dt);
    cm.update(now, dt);
    assembly_copy.write(now, dt);
    rate.sleep();
  }
  spinner.stop();

  return 0;
}