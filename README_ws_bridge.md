# WebSocket 外发、宿主 Supervisor 与失败上报说明

## 背景

这次改动的目标分成两层：

- 给现有主工程补一条不依赖 ROS2 DDS 外部发现的纯 WebSocket JSON 外发链路
- 在宿主机侧补一层 supervisor，让系统支持开机自启、依赖检测、失败上报和进程保活

现有工程里，Foxglove 本来就已经能从容器外访问，这说明：

- 容器到宿主机的网络本身是通的
- 端口映射能力是通的
- 外部前端访问能力也是通的

所以外部收不到 `ros2_ws`，更像是 ROS2 DDS 在容器 NAT 环境下的发现和传输机制问题，而不是基础网络不通。

为了解耦这件事，本次新增了：

- 业务侧纯 WebSocket JSON 通道
- 宿主侧启动管控与 UDP 故障通道

## 当前运行结构

现在的运行结构分成两条链路：

### 1. 正常业务链路

```text
systemd
-> 宿主 supervisor
-> Docker 容器
-> 主程序
-> 主程序内 ROS2 线程
-> 主程序内嵌 WS bridge
-> 外部 WS 客户端 / 前端
```

这里有一个关键点：

- WS bridge 不是独立守护进程
- 它仍然在主程序进程内
- 当主程序第一次镜像业务消息时，WS 服务会懒启动

也就是说：

- 正常情况下不需要单独再起一个 WS 服务
- 跑主工程即可带起 ROS2 线程和 WS bridge

### 2. 故障上报链路

```text
systemd
-> 宿主 supervisor
-> UDP JSON
-> 外部接收端
```

这条链路完全不依赖主程序存活，也不依赖容器里的 WS 服务。

所以即使：

- 主程序没起来
- 容器没起来
- WS 没起来

外部仍然可以收到一个最小故障包，知道当前为什么启动失败。

## 本次新增内容

### 1. 纯 WebSocket JSON Bridge

新增了一个独立的 WebSocket 服务模块，底层复用了工程里已有的 `websocketpp + asio`，但不走 Foxglove 协议层。

这个模块负责：

- 监听单个 TCP 端口
- 接收外部 WebSocket 连接
- 广播 JSON 消息
- 不替换现有 ROS2 内部通信

相关文件：

- [io/ws/ws_bridge.hpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ws/ws_bridge.hpp)
- [io/ws/ws_bridge.cpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ws/ws_bridge.cpp)
- [io/ws/sp_msg_json.hpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ws/sp_msg_json.hpp)

### 2. ROS2 到 WS 的镜像外发

现有 ROS2 内部消息流没有替换，只是在旁边增加了镜像到 WS 的旁路。

当前支持：

- `enemy_status`
- `autoaim_target`
- `dm02_serial_copy`

相关接入点：

- [io/ros2/publish2nav.cpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ros2/publish2nav.cpp)
- [io/ros2/subscribe2nav.cpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ros2/subscribe2nav.cpp)

### 3. 宿主 Supervisor

新增了宿主侧启动管控脚本：

- [spv_supervisor.sh](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/spv_supervisor.sh)

它负责：

- 清理旧容器
- 检测镜像、二进制、ROS2 setup
- 检测相机和串口
- 自动选择串口候选
- 拉起容器并运行主目标
- 主程序退出后自动重启
- 启动失败时发送 UDP JSON 故障包

### 4. systemd 开机自启模板

新增：

- [spv-supervisor.service.template](/home/gzu-printk/sp_vision/sp_vision_25-main-main/systemd/spv-supervisor.service.template)
- [spv-supervisor.env.example](/home/gzu-printk/sp_vision/sp_vision_25-main-main/systemd/spv-supervisor.env.example)
- [install_spv_supervisor_service.sh](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/install_spv_supervisor_service.sh)

这套文件用于把 supervisor 安装成宿主机 `systemd` 服务。

### 5. 外部接收脚本

新增两个最小接收脚本：

- WS 接收脚本：
  [ws_receive.py](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/ws_receive.py)
- UDP 故障包接收脚本：
  [udp_receive.py](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/udp_receive.py)

### 6. 虚拟串口联调

为了在没有真实串口时验证链路，仍然保留了虚拟串口测试方案：

- [virtual_serial_pair.py](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/virtual_serial_pair.py)
- [dm_virtual_sender_test.cpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tests/dm_virtual_sender_test.cpp)
- [dm_ws_bridge_test.cpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tests/dm_ws_bridge_test.cpp)
- [run_dm_virtual_ws_demo.sh](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/run_dm_virtual_ws_demo.sh)

## 运行方式

### 1. 主工程运行时会不会自动带起 WS

会，但不是独立守护方式。

当前逻辑是：

- 主程序启动
- 主程序内部创建 ROS2 相关线程
- 当业务消息被镜像到 WS 时，进程内的 WS bridge 第一次懒启动

因此：

- 生产模式下，通常只需要启动主工程
- 不需要额外先起一个独立 WS 进程

### 2. 什么时候要单独运行测试程序

只有在调试时才需要单独运行测试目标，例如：

- `ws_bridge_test`
- `dm_ws_bridge_test`
- `dm_virtual_sender_test`

这些主要用于：

- 验证 WS 服务是否能对外收发
- 验证虚拟串口链路
- 验证 JSON 格式

生产使用时，推荐由主目标例如 `auto_aim_test` 来承载 WS 外发。

### 3. 手动运行宿主 Supervisor

可以直接在宿主机运行：

```bash
make -C /home/gzu-printk/sp_vision/sp_vision_25-main-main spv-supervisor-run \
  SPV_RUN_TARGET=auto_aim_test \
  SPV_RUN_ARGS="--source=camera --config-path=configs/sentry.yaml --dm-send=true --allow-shoot=false --bullet-speed=27"
```

### 4. 开机自启

安装 `systemd` 服务：

```bash
make -C /home/gzu-printk/sp_vision/sp_vision_25-main-main spv-supervisor-install
```

查看状态：

```bash
make -C /home/gzu-printk/sp_vision/sp_vision_25-main-main spv-supervisor-status
```

跟踪日志：

```bash
make -C /home/gzu-printk/sp_vision/sp_vision_25-main-main spv-supervisor-logs
```

手工版 `systemd` 操作也可以直接用：

```bash
sudo systemctl daemon-reload
sudo systemctl enable --now spv-supervisor.service
systemctl status spv-supervisor.service
journalctl -u spv-supervisor.service -f
```

### 5. 配置文件位置

`systemd` 安装后默认读取：

```text
/etc/default/spv-supervisor
```

可参考模板：

- [spv-supervisor.env.example](/home/gzu-printk/sp_vision/sp_vision_25-main-main/systemd/spv-supervisor.env.example)

## 依赖检测与自动恢复

宿主 supervisor 第一版会处理这些事情：

- 检查 Docker 镜像是否存在
- 检查主目标二进制是否存在
- 检查 `ros2_ws/install/setup.bash` 是否存在
- 检查配置文件对应的相机 `vid_pid`
- 检查串口候选列表
- 在目标退出后自动重启

### 相机检测

默认会从当前配置文件里读取：

- `camera_name`
- `vid_pid`

然后在宿主机上用 `lsusb` 检查当前 USB 设备里是否存在对应 `VID:PID`。

当前对海康相机配置的典型值例如：

```text
vid_pid: "2bdf:0001"
```

### 串口检测与切换

默认串口候选列表：

```text
/dev/gimbal,/dev/ttyACM0,/dev/ttyACM1,/dev/ttyACM2,/dev/ttyUSB0,/dev/ttyUSB1,/dev/ttyUSB2
```

supervisor 会：

- 按顺序扫描
- 选中第一个存在的字符设备
- 把它映射到容器内固定路径
- 若主程序退出，下一轮启动会重新扫描，因此可切到别的接口

第一版不做“进程内热切换串口”，而是采用：

- 主程序退出
- supervisor 重新选串口
- 再次拉起

## 对外接口

### 1. 业务 WebSocket JSON

业务 WS 默认端口：

```text
ws://127.0.0.1:8878
```

统一 envelope：

```json
{
  "type": "enemy_status",
  "timestamp_ns": 1710000000000000000,
  "source": "sentry_debug",
  "payload": {
    "invincible_enemy_ids": [1, 2, 3],
    "game_stage": 2,
    "enemy_count": 4,
    "fire_allowed": true,
    "enemy_team": 1,
    "detected_enemy_ids": [1, 3]
  }
}
```

当前支持：

- `enemy_status`
- `autoaim_target`
- `dm02_serial_copy`

### 2. 启动失败 UDP JSON

默认故障包目标：

- `SPV_FAIL_NOTIFY_HOST=127.0.0.1`
- `SPV_FAIL_NOTIFY_PORT=18878`

UDP JSON v1 固定字段：

- `type`
- `service`
- `target`
- `reason_code`
- `reason`
- `timestamp_ns`
- `details`

示例：

```json
{
  "type": "startup_failed",
  "service": "spv_supervisor",
  "target": "auto_aim_test",
  "reason_code": "serial_not_found",
  "reason": "no serial device matched candidates",
  "timestamp_ns": 1710000000000000000,
  "details": {
    "serial_candidates": ["/dev/gimbal", "/dev/ttyACM0", "/dev/ttyACM1"]
  }
}
```

当前统一失败类型：

- `camera_not_found`
- `serial_not_found`
- `binary_not_found`
- `ros_setup_missing`
- `image_missing`
- `container_start_failed`
- `port_conflict`
- `dependency_timeout`

其中：

- `reason_code` 适合程序侧判断
- `reason` 适合给人看
- `details` 作为可选补充字段

## 接收示例

### 1. 接收业务 WS

```bash
/tmp/spv-ws-venv/bin/python /home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/ws_receive.py \
  --uri ws://127.0.0.1:8878 --type dm02_serial_copy
```

### 2. 接收 UDP 故障包

```bash
python3 /home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/udp_receive.py \
  --host 0.0.0.0 --port 18878 --type startup_failed
```

这里要注意：

- 业务数据走 WS
- 启动失败走 UDP

这两条通道不是一回事。

## 关键配置项

宿主 supervisor 相关：

- `SPV_AUTOSTART`
- `SPV_SUPERVISE`
- `SPV_REQUIRED_CAMERA`
- `SPV_REQUIRED_SERIAL`
- `SPV_SERIAL_CANDIDATES`
- `SPV_SERIAL_BAUD`
- `SPV_RESTART_DELAY_MS`
- `SPV_STARTUP_TIMEOUT_SEC`
- `SPV_SUPERVISOR_CONTAINER_NAME`

故障上报相关：

- `SPV_FAIL_NOTIFY_ENABLE`
- `SPV_FAIL_NOTIFY_HOST`
- `SPV_FAIL_NOTIFY_PORT`
- `SPV_FAIL_NOTIFY_SERVICE_NAME`

业务 WS 相关：

- `SPV_WS_ENABLE`
- `SPV_WS_PORT`
- `SPV_WS_BIND_HOST`
- `SPV_FOXGLOVE_PORT`

相关入口：

- [makefile](/home/gzu-printk/sp_vision/sp_vision_25-main-main/makefile)
- [spv_supervisor.sh](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/spv_supervisor.sh)

## 如果消息格式不对，要改哪里

### 1. 改业务 WS 的 envelope

改这里：

- [ws_bridge.cpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ws/ws_bridge.cpp)

主要是 `publish_message(...)` 里统一封装：

- `type`
- `timestamp_ns`
- `source`
- `payload`

### 2. 改某类业务消息的 payload

改这里：

- [sp_msg_json.hpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ws/sp_msg_json.hpp)

每一类消息对应一个 `to_json(...)`：

- `EnemyStatusMsg`
- `AutoaimTargetMsg`
- `Dm02SerialCopyMsg`

如果外部说字段名不对、少字段、类型不对，优先改这里。

### 3. 改什么时候镜像出去

改这里：

- [publish2nav.cpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ros2/publish2nav.cpp)
- [subscribe2nav.cpp](/home/gzu-printk/sp_vision/sp_vision_25-main-main/io/ros2/subscribe2nav.cpp)

如果要新增一种消息镜像出去，做法是：

- 在 `sp_msg_json.hpp` 里补 `to_json(...)`
- 再在对应业务接入点调用 `publish_xxx(...)`

### 4. 改启动失败 UDP JSON

改这里：

- [spv_supervisor.sh](/home/gzu-printk/sp_vision/sp_vision_25-main-main/tools/spv_supervisor.sh)

主要改动点：

- `reason_code`
- `reason`
- `details`

也就是说：

- 业务 JSON 协议改 `io/ws/...`
- 故障 JSON 协议改 `tools/spv_supervisor.sh`

不要把这两块混在一起改。

## 已完成验证

### 1. 业务 WS 验证

已经验证：

- 容器内虚拟串口
- DM02 协议发送
- DM02 协议接收与解析
- WebSocket JSON 外发
- 宿主机外部接收

说明：

- 业务数据生成正常
- DM02 协议链路正常
- WS 对外广播正常
- 宿主机接收正常

### 2. 网络侧判断

Foxglove 能通，纯 WS 也能通，说明：

- 基础网络不是根因
- 容器到宿主机的 TCP/WS 链路是正常的

因此更合理的判断仍然是：

- `ros2_ws` 外部收不到，更像 ROS2 DDS 在容器 NAT 环境下的机制问题

## 结论

当前工程已经同时具备：

- 主工程内嵌的业务 WS 外发
- 宿主机 supervisor 自动拉起
- `systemd` 开机自启
- 启动失败 UDP JSON 最小上报

因此现在的对外行为可以明确分成两类：

- 主程序成功启动后，业务数据从 WS 对外发
- 主程序启动失败时，故障信息从 UDP 对外发

这两条链路相互独立，所以即使主程序根本没跑起来，外部也能知道失败原因。
