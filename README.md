## 注意事项
#注意：这是本人用来学习的仓库，在原作者项目进行了修改
本项目基于 (https://gitee.com/fdsf3e2342/3d-navi),[PCT-planner](https://github.com/byangw/PCT_planner.git)、[ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git) 和 [unitree_guide](https://gitee.com/chy2948331536/unitree_guide.git) 构建，仅限学习使用，禁止用于商业用途。

# 3D 导航仿真项目

---

## 下载与依赖

### 依赖项安装
1. **libtorch**：下载 C++ 版本的 [libtorch](https://pytorch.org/)。
2. **PCT-planner**：将其放置在 `src` 文件夹外单独编译。
3. **ego-planner**：请参考其官方文档进行配置：[ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git)
4. **Fast-lio**：`.auto.sh` 仿真脚本自动发布 `livox/lidar` 和 `livox/imu` 话题，无需额外配置，但需手动设置 `tf` 坐标关系。
5. **More PCD files**: [https://pan.baidu.com/s/1DnUMtvqcTSCsWxjJAQFnhQ?pwd=cjjj](https://pan.baidu.com/s/1DnUMtvqcTSCsWxjJAQFnhQ?pwd=cjjj)

---

## 安装步骤

### 1. 配置 libtorch 和 CUDA 路径

修改 `src/unitree_guide/unitree_guide/unitree_guide/CMakeLists.txt` 中的 `libtorch` 路径和 `CMAKE_CUDA_COMPILER` 路径。

### 2. 安装 ego-planner

进入 ROS 工作空间并执行以下命令：
```bash
sudo apt-get install libarmadillo-dev
catkin_make
```

### 3. 安装 PCT-planner

#### Environment

- Ubuntu >= 20.04
- ROS >= Noetic with ros-desktop-full installation
- CUDA >= 11.7

#### Python（建议使用虚拟环境）

- Python >= 3.8
- [CuPy](https://docs.cupy.dev/en/stable/install.html) with CUDA >= 11.7
- Open3d

####  Build & Install

```bash
cd PCT_planner/planner/
./build_thirdparty.sh
./build.sh
```

---

## 使用说明

### 1. 启动 RL 控制器
由于 RL 控制器需要手柄，因此需先启动虚拟手柄（注意这个不是控制器！！！）：
```bash
sudo -s
source ./devel/setup.bash
rosrun unitree_guide virtual_joy.py
```

然后启动 Gazebo 仿真环境并运行控制器：
```bash
. auto.sh  # 等待 Unitree A1 机器人展开
./devel/lib/unitree_guide/junior_ctrl
```

在控制器中：
- 按键 **2**：站立
- 按键 **6**：切换为 RL 模式（此时接收 `cmd_vel` 消息）
- 再次按键 **2**：会闪退，需重新启动控制器

### 2. 启动 ego-planner
```bash
source ./devel/setup.bash
roslaunch ego_planner run_in_sim.launch  # 局部导航模块
roslaunch ego_planner rviz.launch  # RVIZ 可视化
```

修改 `run_in_sim.launch` 文件中的 `flight_type` 参数可切换导航模式：
```xml
<!-- 1: 使用 2D Nav Goal 设置目标 -->
<arg name="flight_type" value="1" />
<!-- 3: 使用 move_base 的路径 -->
<arg name="flight_type" value="3" />
```

### 3. 启动 PCT-planner
进入 `PCT-planner` 文件夹并运行以下命令：
```bash
# 将分层地图可视化在RVIZ中
cd tomography/scripts/ 
python3 tomography.py --scene Building

# 发布plan任务，启用interactive_marker_server
cd planner/scripts/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:home/YOUR-NAME/3d-navi/PCT_planner/planner/lib/3rdparty/gtsam-4.1.1/install/lib
python3 plan.py --scene Building
```
### 4. 启动 FAST_LIO_LOCALIZATION_HUMANOID（修改适配A1）
改进适配的仓库:https://github.com/lxs5127/FAST_LIO_LOCALIZATION_HUMANOID
启动雷达进行重定位和发布里程计:
```bash
roslaunch open3d_loc localization_3d_g1.launch
```
> ⚠️ **注意**：如果地图配置更改，需重新生成地图，路径规划器才会重新规划路径。

---
