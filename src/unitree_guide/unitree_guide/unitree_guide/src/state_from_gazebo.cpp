#include "gazebo_msgs/LinkStates.h"
#include "gazebo_msgs/ModelStates.h"
#include "geometry_msgs/TransformStamped.h"
#include "ros/ros.h"
#include "tf2/LinearMath/Quaternion.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.h"
#include "tf2_ros/buffer.h"
#include "tf2_ros/transform_broadcaster.h"
// #include "tf2_ros/transform_listener.h"
#include <tf/message_filter.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_listener.h>
#include <nav_msgs/Odometry.h>
#include <boost/bind.hpp>   // 将tf监听绑定到ROS回调函数


using namespace std;
ros::Publisher robotVelocity_BASE_frame_pub;
string robot_name = "a1";
nav_msgs::Odometry Odom;
double x=0, y=0, z=0, roll=0, pitch=0, yaw=0;


void callback_BASE(const gazebo_msgs::LinkStates::ConstPtr &msg) {
    int index = 0;
    for (auto &linkName : msg->name) {
        if (linkName == robot_name+"_gazebo::base")
            break;
        ++index;
    }
    ros::Rate rate(500);//延迟至100hz发布，避免重复发布
    // tf::StampedTransform transform_odom2map;
    //等待监听odom坐标系与map坐标系的关系，注意转换的时间戳！！！！
    // tf_listener_ptr->waitForTransform("odom", "map", ros::Time(0), ros::Duration(0.01));
    // tf_listener_ptr->lookupTransform("odom", "map",ros::Time(0), transform_odom2map);

    // tf::Point pt_map(msg->pose[index].position.x,msg->pose[index].position.y,msg->pose[index].position.z);
    // tf::Point pt_odom = transform_odom2map * pt_map;

    static tf::TransformBroadcaster bf2;
    tf::Transform transform_odom2map;
    tf2::Quaternion qtn;
    qtn.setRPY(roll, pitch, yaw);
    
    transform_odom2map.setRotation(tf::Quaternion(qtn.x(),
                                         qtn.y(),
                                         qtn.z(),
                                         qtn.w()));
    transform_odom2map.setOrigin(tf::Vector3(x,
                                    y,
                                    z));

    bf2.sendTransform(tf::StampedTransform(transform_odom2map, ros::Time::now(), "map", "odom"));
    
    //求变化矩阵的逆解，用于推算map到odom的关系，以便能得到base到map的关系
    tf::Transform transform_map2odom = transform_odom2map.inverse();

    tf::Point pt_map(msg->pose[index].position.x,msg->pose[index].position.y,msg->pose[index].position.z);
    tf::Point pt_odom = transform_map2odom * pt_map;
    
    tf::Quaternion q_map(msg->pose[index].orientation.x,
                        msg->pose[index].orientation.y,
                        msg->pose[index].orientation.z,
                        msg->pose[index].orientation.w);
    tf::Quaternion q_odom = transform_map2odom.getRotation() * q_map;

    static tf::TransformBroadcaster bf;
    tf::Transform transform_odom2base;
    transform_odom2base.setRotation(q_odom);
    transform_odom2base.setOrigin(pt_odom);

    bf.sendTransform(tf::StampedTransform(transform_odom2base, ros::Time::now(), "odom", "base"));

    Odom.header.stamp = ros::Time::now();
    Odom.header.frame_id = "odom";
    Odom.child_frame_id = "base";

    // set the position
    Odom.pose.pose.position.x = msg->pose[index].position.x;
    Odom.pose.pose.position.y = msg->pose[index].position.y;
    Odom.pose.pose.position.z = msg->pose[index].position.z;

    Odom.pose.pose.orientation.w = msg->pose[index].orientation.w ;
    Odom.pose.pose.orientation.x = msg->pose[index].orientation.x ;
    Odom.pose.pose.orientation.y = msg->pose[index].orientation.y ;
    Odom.pose.pose.orientation.z = msg->pose[index].orientation.z ;

    // set the velocity
    Odom.twist.twist.linear.x = msg->twist[index].linear.x;
    Odom.twist.twist.linear.y = msg->twist[index].linear.y;
    Odom.twist.twist.linear.z = msg->twist[index].linear.z;

    Odom.twist.twist.angular.x =msg->twist[index].angular.x;
    Odom.twist.twist.angular.y =msg->twist[index].angular.y;
    Odom.twist.twist.angular.z =msg->twist[index].angular.z;

    robotVelocity_BASE_frame_pub.publish(Odom);
    rate.sleep();
}


int main(int argc, char **argv) {
    ros::init(argc, argv, "state_from_gazebo");
    ros::NodeHandle nh("~");
    ros::NodeHandle node;
    ros::Subscriber tfState_BASE_sub;

    // tf::TransformListener tf_listener_;

    if (argc != 7)   // x y z qx qy qz qw 
    {
        ROS_ERROR("Usage: static_transform_publisher x y z yaw pitch roll");
        return -1;
    }

    x = atof(argv[1]);
    y = atof(argv[2]);
    z = atof(argv[3]);

    double yaw   = atof(argv[4]);
    double pitch = atof(argv[5]);
    double roll  = atof(argv[6]);
  
    nh.param<std::string>("robot_name", robot_name, string("a1"));
    tfState_BASE_sub = node.subscribe<gazebo_msgs::LinkStates>("/gazebo/link_states", 10, callback_BASE);
    robotVelocity_BASE_frame_pub = node.advertise<nav_msgs::Odometry>("/Odometry_gazebo", 1);

    ros::spin();
    return 0;
}



