#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

ros::ServiceClient client;

void drive_robot(float lin_x, float ang_z)
{
	ball_chaser::DriveToTarget srv;
	
	srv.request.linear_x = lin_x;
	srv.request.angular_z = ang_z;

	if(!client.call(srv))
		{
			ROS_ERROR("Service Failed");		
		}
}

void process_image_callback(const sensor_msgs::Image img)
{
	int white_pixel=255;
	int step = -1;
	bool is_ball_there = false;
	float lin_x=0.0, ang_z=0.0;

	for(int i = 0; i< img.height*img.step; i+=3)
		{
			if(img.data[i]==white_pixel && img.data[i+1]==white_pixel && img.data[i+2]==white_pixel)
				{
					is_ball_there = true;
					step = i%img.step;
					ROS_INFO_STREAM("Ball Detected");
					break;					
				}
		}
	if(step>0 && step<img.step/3) 
		{
			drive_robot(0.0,0.5);
		}

	else if(step>((2*img.step)/3))
		{
			drive_robot(0.0,-0.5);				
		}
	else if(step!=-1)
		{
			drive_robot(0.5,0.0);
		}

	else if(is_ball_there == false)
		{
			drive_robot(0.0,0.0);
			ROS_INFO_STREAM("Ball Not Found");
		}

}

int main(int argc, char** argv)
{
	ros::init(argc,argv,"process_image");
	ros::NodeHandle n;

	client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

	ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

	ros::spin();

	return 0;
}
