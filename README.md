# 3d_nav

### 介绍
本项目为gazebo仿真认证[PCT-planner](https://github.com/byangw/PCT_planner.git) & [ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git)，使用robot unitree a1机器人，以及强化学习控制器[chy2948331536/unitree_guide](https://gitee.com/chy2948331536/unitree_guide)，控制器和PCT-planner需要CUDA


<div align="center">
  <img src="/src/image/878355907.gif" width="800"/>
</div>


### 下载
1.  将代码clone到ros工作目录src下
2.  网盘下载文件，其中**unitree_ros**为**unitree a1**的描述文件，**PCT-planner**为三维地图生成工具
3.  **PCT-planner**放在src文件夹外，单独编译

### 安装
1.  修改ros工作目录**src/unitree_guide/unitree_guide/unitree_guide/src/FSM/State_RL_test.cpp**中的**model_path**为**unitree_guide/logs**中的模型绝对路径
2.  修改ros工作目录**src/unitree_guide/unitree_guide/unitree_guide/CMakeLists.txt**中的libtorch路径以及**CMAKE_CUDA_COMPILER**路径
3. 进入工作空间进行**catkin_make**
4. 配置PCT-planner，请参考官方教程[PCT_planner](https://github.com/byangw/PCT_planner.git)
5. 配置ego-planner，请参考官方教程[ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git)


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

4.  打开ego-planner，[3d_nav_goal](https://github.com/LiiXZ/rviz-3d-nav-goal-tool)需要先在rviz添加这个插件
```
source ./devel/setup.bash
roslaunch ego_planner run_in_sim.launch #局部导航模块
roslaunch ego_planner ego_rviz.launch #rviz
```
5. ego-planner默认使用nav-goal发布信息，修改run_in_sim.launch中的flight_type
```
<!-- 1: use 2D Nav Goal to select goal  -->
<!-- 2: use global waypoints below  -->
<!-- 3: use move_base path waypoints below  -->
    <arg name="flight_type" value="3" />
```
6.  打开PCT-planner，请参考PCT-planner官方教程，这里只做简单介绍（规划失败，会闪退plan.py，还没来得及改）
```
##修改地图配置，需要重新生成地图，path planner才会重新规划
cd tomography/scripts/ 
python3 tomography.py --scene Building
###################################
cd planner/scripts/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:planner/lib/3rdparty/gtsam-4.1.1/install/lib
python3 plan.py --scene Building
```
### 注意
1. 时间比较紧，代码写的比较乱，请见谅
2. 有问题请尽快提出issue，便于我修改，如有更好建议也请联系我
3. 本项目基于[PCT-planner](https://github.com/byangw/PCT_planner.git) & [ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git) & [unitree_guide](https://gitee.com/chy2948331536/unitree_guide.git)，仅供学习，切勿商用


### Communication
如需帮助，若本项目对你有帮助，请给我点个star，谢谢！
bilibil: https://space.bilibili.com/29152879
email: 1906570332@qq.com