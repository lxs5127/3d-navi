import sys
import argparse
import threading
import numpy as np

import rospy
from nav_msgs.msg import Path

from utils import *
from planner_wrapper import TomogramPlanner
from visualization_msgs.msg import Marker,MarkerArray


sys.path.append('../')
from config import Config

parser = argparse.ArgumentParser()
parser.add_argument('--scene', type=str, default='Spiral', help='Name of the scene. Available: [\'Spiral\', \'Building\', \'Plaza\']')
args = parser.parse_args()

cfg = Config()

cluster_pub= rospy.Publisher('/path_markers', MarkerArray, queue_size=10)  # 新增发布器

if args.scene == 'Building':
    tomo_file = 'building2_9'
    start_pos = np.array([-5.5, 6, 0.5], dtype=np.float32)
    end_pos = np.array([5, 0, 7], dtype=np.float32)


path_pub = rospy.Publisher("/pct_path", Path, latch=True, queue_size=1)

planner = TomogramPlanner(cfg)


def publish_culster_marker():
        """发布多个聚类中心的MarkerArray"""
        rate = rospy.Rate(5)
        def gsl_thread():
             while not rospy.is_shutdown():
                # 先发布一个 DELETEALL 动作清除所有旧 Marker
                marker_array = MarkerArray()
                clear_marker = Marker()
                clear_marker.header.frame_id = "map"
                clear_marker.header.stamp = rospy.Time.now()
                clear_marker.ns = "path_markers"
                clear_marker.id = 0  # ID 在 DELETEALL 中不重要
                clear_marker.action = Marker.DELETEALL
                marker_array.markers.append(clear_marker)
                marker_array = MarkerArray()  # 创建 MarkerArray
                for i, pos in enumerate([start_pos,end_pos]):
                    marker = Marker()
                    marker.header.frame_id = "map"
                    marker.header.stamp = rospy.Time.now()
                    marker.ns = "culster"
                    marker.id = i  # 为每个 Marker 设置唯一 ID
                    marker.type = Marker.SPHERE
                    marker.action = Marker.ADD
                    marker.scale.x = 0.2
                    marker.scale.y = 0.2
                    marker.scale.z = 0.2
                    marker.color.r = 1
                    marker.color.g = 0
                    marker.color.b = 0.0
                    marker.color.a = 1.0
                    marker.lifetime = rospy.Duration(0)
                    # 将聚类中心从栅格坐标转换为世界坐标
                    marker.pose.position.x = pos[0]
                    marker.pose.position.y = pos[1]
                    marker.pose.position.z = 0.5
                    if i==1:
                        marker.pose.position.z = end_pos[2]
                    #将最近的kmeans-cluster点发送
                    marker_array.markers.append(marker)  # 添加到 MarkerArray
                cluster_pub.publish(marker_array)  # 发布 MarkerArray
                rate.sleep()
        gsl_pf=threading.Thread(target=gsl_thread)
        gsl_pf.start()

       

def pct_plan():
    planner.loadTomogram(tomo_file)
    publish_culster_marker()

    traj_3d = planner.plan(start_pos, end_pos)
    if traj_3d is not None:
        path_pub.publish(traj2ros(traj_3d))
        print("Trajectory published")



if __name__ == '__main__':
    rospy.init_node("pct_planner", anonymous=True)
    pct_plan()
    rospy.spin()
# export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/cjh/PCT_planner/planner/lib/3rdparty/gtsam-4.1.1/install/lib