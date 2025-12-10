# 3d_navi

### 介绍
本项目为gazebo仿真验证[PCT-planner](https://github.com/byangw/PCT_planner.git) & [ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git)，使用robot unitree a1机器人，以及强化学习控制器[chy2948331536/unitree_guide](https://gitee.com/chy2948331536/unitree_guide)，控制器和PCT-planner需要CUDA


<div align="center">
  <img src="/src/image/878355907.gif" width="800"/>
</div>


### 下载
1.  将代码clone到ros工作目录src下
2.  下载c++版本的[libtorch](https://pytorch.org/)
3.  其中**unitree_guide**为**unitree a1**的描述文件以及控制器，**planner**为**ego-planner**规划器，**PCT-planner**为三维地图规划器，**Mid360_imu_sim**为**livox_msg_mid360**的仿真包
4.  **PCT-planner**放在src文件夹外，单独编译
5.  本项目支持使用Fast-lio，**.auto.sh**自动发布**livox/lidar**与**livox/imu**话题，**tf关系**需要自己添加

### 安装
1.  修改ros工作目录**src/unitree_guide/unitree_guide/unitree_guide/CMakeLists.txt**中的libtorch路径以及**CMAKE_CUDA_COMPILER**路径
2.  安装**ego-planner**相关依赖，请参考官方教程[ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git)
3. 进入工作空间进行**catkin_make**
   
4. 安装**PCT-planner**相关依赖，请参考官方教程[PCT_planner](https://github.com/byangw/PCT_planner.git)
```
## 配置PCT-planner
cd planner/
./build_thirdparty.sh
./build.sh
```

### 使用说明

1.  由于RL控制器需要手柄，因此需要开启一个虚拟控制器
```
sudo -s
source ./devel/setup.bash
```
2.  打开仿真环境
```
. auto.sh  #等待unitree a1展开，启动控制器
./devel/lib/unitree_guide/junior_ctrl
```
3.  控制器，按键2站立后，按键6切换为RL模式，此时接收cmd_vel消息，再次2后会闪退需要重新打开控制器

4.  打开**ego-planner**
```
source ./devel/setup.bash
roslaunch ego_planner run_in_sim.launch #局部导航模块
roslaunch ego_planner ego_rviz.launch #rviz
```
1. **ego-planner**默认使用nav-goal发布信息，修改**run_in_sim.launch**中的**flight_type**
```
<!-- 1: use 2D Nav Goal to select goal  -->
<!-- 2: use global waypoints below  -->
<!-- 3: use move_base path waypoints below  -->
    <arg name="flight_type" value="3" />
```
1.  打开PCT-planner，请参考PCT-planner官方教程，这里只做简单介绍（规划失败，会core dumped）
```
##修改地图配置，需要重新生成地图，path planner才会重新规划
cd tomography/scripts/ 
python3 tomography.py --scene Building
###################################
cd planner/scripts/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/YOUR-NAME/3d-navi/PCT_planner/planner/lib/3rdparty/gtsam-4.1.1/install/lib
python3 plan.py --scene Building
```
### 注意
1. 时间比较紧，代码写的比较乱，请见谅
2. 有问题请尽快提出issue，便于我修改，如有更好建议也请联系我
3. 本项目基于[PCT-planner](https://github.com/byangw/PCT_planner.git) & [ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git) & [unitree_guide](https://gitee.com/chy2948331536/unitree_guide.git)，仅供学习，切勿商用


### Communication
若本项目对你有帮助，不妨给我点个star，谢谢！
bilibil: https://space.bilibili.com/29152879
email: 1906570332@qq.com

