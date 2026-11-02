## 注意事项
#注意：这是本人用来学习的仓库，在原作者项目进行了修改
本项目基于 (https://gitee.com/fdsf3e2342/3d-navi),[PCT-planner](https://github.com/byangw/PCT_planner.git)、[ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git) 和 [unitree_guide](https://gitee.com/chy2948331536/unitree_guide.git) 构建，仅限学习使用，禁止用于商业用途。

# 3D 导航仿真项目

## 规划器分支

本仓库通过两个分支分别维护局部规划器：

| 分支 | 源码目录 | ROS 包 | 启动文件 |
| --- | --- | --- | --- |
| `ego` | `src/planner` | `ego_planner` | `run_in_sim.launch` |
| `scan`（当前分支） | `src/scan_planner` | `scan_planner` | `run.launch` |

两套规划器的 `bspline_opt`、`path_searching`、`plan_env`、`traj_utils` 包重名，分别保存在各自分支中。

工作区修改提交或保存后，可以在仓库根目录切换：

```bash
git fetch origin
git switch ego   # 使用原来的 ego 版本
git switch scan  # 使用 scan 版本
```

同一目录切换规划器后，需要移走旧的 `build/`、`devel/` 并重新编译，避免复用另一套规划器的构建缓存。
也可以在两个独立工作目录中使用这两个分支，各自运行 `catkin_make`：

```bash
git worktree add ../3d-navi-ego ego
git worktree add ../3d-navi-scan scan
```

每个终端只加载对应工作目录的 `devel/setup.bash`。后续修改在哪个分支完成，就推送到对应分支：

```bash
git push origin ego
git push origin scan
```

`scan` 分支从 GitHub 原有版本整理当前源码和已跟踪的地图更新，不包含本地历史中的超大 `scan.pcd`、虚拟环境及本地运行缓存。其他地图下载方式见下文。

---

## 下载与依赖

### 依赖项安装
1. **libtorch**：下载 C++ 版本的 [libtorch](https://pytorch.org/)。
2. **PCT-planner**：将其放置在 `src` 文件夹外单独编译。
3. **局部规划器**：本分支已包含 `scan_planner`；原来的 ego 版本保存在 `ego` 分支。两者均基于 [ego-planner](https://github.com/ZJU-FAST-Lab/ego-planner.git)。
4. **Fast-lio**：`.auto.sh` 仿真脚本自动发布 `livox/lidar` 和 `livox/imu` 话题，无需额外配置，但需手动设置 `tf` 坐标关系。
5. **More PCD files**: [https://pan.baidu.com/s/1DnUMtvqcTSCsWxjJAQFnhQ?pwd=cjjj](https://pan.baidu.com/s/1DnUMtvqcTSCsWxjJAQFnhQ?pwd=cjjj)

---

## 安装步骤

### 1. 配置 libtorch 和 CUDA 路径

修改 `src/unitree_guide/unitree_guide/unitree_guide/CMakeLists.txt` 中的 `libtorch` 路径和 `CMAKE_CUDA_COMPILER` 路径。

### 2. 编译 scan-planner

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

### 2. 启动 scan-planner
```bash
source ./devel/setup.bash
roslaunch scan_planner run.launch is_real_world:=false  # Gazebo 仿真
roslaunch scan_planner rviz.launch frame_id:=world      # RVIZ 可视化
```

实机运行时使用以下命令（`run.launch` 默认 `is_real_world:=true`，坐标系为 `map`）：

```bash
roslaunch scan_planner run.launch
roslaunch scan_planner rviz.launch
```

在 `src/scan_planner/plan_manage/launch/run.launch` 中配置 `navi_mode`、传感器话题和控制模式。ego 的启动命令见 `ego` 分支 README。

### 3. 启动 PCT-planner
进入 `PCT-planner` 文件夹并运行以下命令：
```bash
# 将分层地图可视化在RVIZ中
cd tomography/scripts/ 
python3 tomography.py --scene Building

# 发布plan任务，启用interactive_marker_server
cd planner/scripts/
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/YOUR-NAME/3d-navi/PCT_planner/planner/lib/3rdparty/gtsam-4.1.1/install/lib
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
