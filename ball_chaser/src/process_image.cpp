#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include "ball_chaser/DriveToTarget.h"

/* Define a global client that can request services */
ros::ServiceClient srvClient;

/* This function calls the command_robot service to drive the robot in the specified direction */
void drive_robot(float lin_x, float ang_z)
{
  
  ball_chaser::DriveToTarget srv;
  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;
  
  
  if (!srvClient.call(srv)) {
    ROS_ERROR("Failed");
  }
  
}



void process_image_callback(const sensor_msgs::Image img)
{
 
    int i;
    
    int bp;
    int bpc;
    int bps = 0;
    int wpn = 0;
    
    

    for (i = 0; i + 2 < img.data.size(); i = i + 3)
    {
        if ((img.data[i] == 255) && (img.data[i+1] == 255) && (img.data[i+2] == 255))
        {
            bp = (i % (img.width * 3)) / 3;
            bps += bp;
            wpn++;
        }
    }
    
    if (wpn == 0)
    {
        drive_robot(0, 0);
    }
    else
    {
        bpc = bps / wpn;
        
        if(bpc < img.width / 3)
        {
            drive_robot(0.4, 0.6);
        }
        else if(bpc > img.width * 2 / 3)
        {
            drive_robot(0.4, -0.6);
        }
        else
        {
            drive_robot(0.4, 0);
        }
    }


}

int main(int argc, char** argv)
{
  // Initialize the process_image node and create a NodeHandle to it
  ros::init(argc, argv, "process_image");
  ros::NodeHandle nh;

  // Define a service client capabale of requesting services from "/ball_chaser/command_robot"
  srvClient = nh.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

  // Subscribe to /camera/rgb/image_raw topic to read the image data inside
  ros::Subscriber sub = nh.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

  // Handle ROS communication events
  ros::spin();

  return 0;

}
