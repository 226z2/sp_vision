#include <foxglove/websocket/base64.hpp>
#include <foxglove/websocket/server_factory.hpp>
#include <foxglove/websocket/websocket_notls.hpp>
#include <foxglove/websocket/websocket_server.hpp>

#include <nlohmann/json.hpp>

#include <atomic>
#include <chrono>
#include <cmath>
#include <csignal>
#include <iostream>
#include <memory>
#include <random>
#include <thread>
#include <vector>

std::atomic<bool> running = true;

static uint64_t nanosecondsSinceEpoch() {
  return uint64_t(std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count());
}

int main() {
  const auto logHandler = [](foxglove::WebSocketLogLevel, char const* msg) {
    std::cout << msg << std::endl;
  };
  
  foxglove::ServerOptions serverOptions;
  auto server = foxglove::ServerFactory::createServer<websocketpp::connection_hdl>(
    "Foxglove Multi-Data Demo Server", logHandler, serverOptions);

  foxglove::ServerHandlers<foxglove::ConnHandle> hdlrs;
  hdlrs.subscribeHandler = [&](foxglove::ChannelId chanId, foxglove::ConnHandle clientHandle) {
    const auto clientStr = server->remoteEndpointString(clientHandle);
    std::cout << "Client " << clientStr << " subscribed to channel " << chanId << std::endl;
  };
  hdlrs.unsubscribeHandler = [&](foxglove::ChannelId chanId, foxglove::ConnHandle clientHandle) {
    const auto clientStr = server->remoteEndpointString(clientHandle);
    std::cout << "Client " << clientStr << " unsubscribed from channel " << chanId << std::endl;
  };
  
  server->setHandlers(std::move(hdlrs));
  server->start("0.0.0.0", 8765);
  
  std::cout << "Foxglove WebSocket server started on ws://localhost:8765" << std::endl;
  std::cout << "Connect with Foxglove Studio to view the data!" << std::endl;

  // 定义多个有趣的数据通道
  const auto channelIds = server->addChannels({
    {
      .topic = "/robot/position",
      .encoding = "json",
      .schemaName = "geometry_msgs/Point",
      .schema = nlohmann::json{
        {"type", "object"},
        {"properties", {
          {"x", {{"type", "number"}, {"description", "X coordinate"}}},
          {"y", {{"type", "number"}, {"description", "Y coordinate"}}},
          {"z", {{"type", "number"}, {"description", "Z coordinate"}}}
        }}
      }.dump()
    },
    {
      .topic = "/robot/velocity",
      .encoding = "json", 
      .schemaName = "geometry_msgs/Twist",
      .schema = nlohmann::json{
        {"type", "object"},
        {"properties", {
          {"linear", {
            {"type", "object"},
            {"properties", {
              {"x", {{"type", "number"}}},
              {"y", {{"type", "number"}}},
              {"z", {{"type", "number"}}}
            }}
          }},
          {"angular", {
            {"type", "object"},
            {"properties", {
              {"x", {{"type", "number"}}},
              {"y", {{"type", "number"}}},
              {"z", {{"type", "number"}}}
            }}
          }}
        }}
      }.dump()
    },
    {
      .topic = "/sensors/temperature",
      .encoding = "json",
      .schemaName = "sensor_msgs/Temperature", 
      .schema = nlohmann::json{
        {"type", "object"},
        {"properties", {
          {"temperature", {{"type", "number"}, {"description", "Temperature in Celsius"}}},
          {"variance", {{"type", "number"}}},
          {"header", {
            {"type", "object"},
            {"properties", {
              {"stamp", {{"type", "number"}}},
              {"frame_id", {{"type", "string"}}}
            }}
          }}
        }}
      }.dump()
    },
    {
      .topic = "/sensors/imu",
      .encoding = "json",
      .schemaName = "sensor_msgs/Imu",
      .schema = nlohmann::json{
        {"type", "object"},
        {"properties", {
          {"orientation", {
            {"type", "object"}, 
            {"properties", {
              {"x", {{"type", "number"}}},
              {"y", {{"type", "number"}}},
              {"z", {{"type", "number"}}},
              {"w", {{"type", "number"}}}
            }}
          }},
          {"angular_velocity", {
            {"type", "object"},
            {"properties", {
              {"x", {{"type", "number"}}},
              {"y", {{"type", "number"}}},
              {"z", {{"type", "number"}}}
            }}
          }},
          {"linear_acceleration", {
            {"type", "object"},
            {"properties", {
              {"x", {{"type", "number"}}},
              {"y", {{"type", "number"}}},
              {"z", {{"type", "number"}}}
            }}
          }}
        }}
      }.dump()
    },
    {
      .topic = "/robot/status",
      .encoding = "json",
      .schemaName = "std_msgs/String",
      .schema = nlohmann::json{
        {"type", "object"},
        {"properties", {
          {"data", {{"type", "string"}, {"description", "Robot status message"}}}
        }}
      }.dump()
    },
    {
      .topic = "/diagnostics",
      .encoding = "json", 
      .schemaName = "diagnostic_msgs/DiagnosticArray",
      .schema = nlohmann::json{
        {"type", "object"},
        {"properties", {
          {"battery_voltage", {{"type", "number"}}},
          {"cpu_usage", {{"type", "number"}}},
          {"memory_usage", {{"type", "number"}}},
          {"disk_usage", {{"type", "number"}}}
        }}
      }.dump()
    }
  });

  std::signal(SIGINT, [](int sig) {
    std::cerr << "Received signal " << sig << ", shutting down..." << std::endl;
    running = false;
  });

  // 随机数生成器
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<double> noise(-0.1, 0.1);
  std::uniform_real_distribution<double> temp_noise(-2.0, 2.0);
  
  double t = 0.0;
  int seq = 0;
  
  while (running) {
    const auto now = nanosecondsSinceEpoch();
    t += 0.05; // 时间步长
    seq++;

    // 1. 机器人位置 - 8字形轨迹
    {
      const auto posMsg = nlohmann::json{
        {"x", 8.0 * std::sin(t * 0.5)},
        {"y", 4.0 * std::sin(t)}, 
        {"z", 1.0 + 0.5 * std::sin(t * 2.0)}
      };
      const auto serialized = posMsg.dump();
      server->broadcastMessage(channelIds[0], now, 
                               reinterpret_cast<const uint8_t*>(serialized.data()),
                               serialized.size());
    }

    // 2. 机器人速度
    {
      const auto velMsg = nlohmann::json{
        {"linear", {
          {"x", 4.0 * std::cos(t * 0.5) * 0.5},
          {"y", 4.0 * std::cos(t)},
          {"z", std::cos(t * 2.0)}
        }},
        {"angular", {
          {"x", 0.0},
          {"y", 0.0}, 
          {"z", 0.3 * std::sin(t * 0.8)}
        }}
      };
      const auto serialized = velMsg.dump();
      server->broadcastMessage(channelIds[1], now,
                               reinterpret_cast<const uint8_t*>(serialized.data()),
                               serialized.size());
    }

    // 3. 温度传感器
    {
      const auto tempMsg = nlohmann::json{
        {"temperature", 25.0 + 10.0 * std::sin(t * 0.1) + temp_noise(gen)},
        {"variance", 0.5},
        {"header", {
          {"stamp", now / 1000000000.0},
          {"frame_id", "temperature_sensor"}
        }}
      };
      const auto serialized = tempMsg.dump();
      server->broadcastMessage(channelIds[2], now,
                               reinterpret_cast<const uint8_t*>(serialized.data()),
                               serialized.size());
    }

    // 4. IMU 数据
    {
      const double roll = 0.1 * std::sin(t * 0.7);
      const double pitch = 0.05 * std::cos(t * 0.9);
      const double yaw = t * 0.1;
      
      // 简化的四元数计算
      const double qw = std::cos(yaw/2) * std::cos(pitch/2) * std::cos(roll/2) + 
                        std::sin(yaw/2) * std::sin(pitch/2) * std::sin(roll/2);
      const double qx = std::cos(yaw/2) * std::cos(pitch/2) * std::sin(roll/2) - 
                        std::sin(yaw/2) * std::sin(pitch/2) * std::cos(roll/2);
      const double qy = std::cos(yaw/2) * std::sin(pitch/2) * std::cos(roll/2) + 
                        std::sin(yaw/2) * std::cos(pitch/2) * std::sin(roll/2);
      const double qz = std::sin(yaw/2) * std::cos(pitch/2) * std::cos(roll/2) - 
                        std::cos(yaw/2) * std::sin(pitch/2) * std::sin(roll/2);
      
      const auto imuMsg = nlohmann::json{
        {"orientation", {{"x", qx}, {"y", qy}, {"z", qz}, {"w", qw}}},
        {"angular_velocity", {
          {"x", 0.7 * std::cos(t * 0.7) + noise(gen)},
          {"y", 0.9 * std::cos(t * 0.9) + noise(gen)},
          {"z", 0.1 + noise(gen)}
        }},
        {"linear_acceleration", {
          {"x", std::sin(t * 1.2) + noise(gen)},
          {"y", std::cos(t * 0.8) + noise(gen)},
          {"z", 9.81 + noise(gen)}
        }}
      };
      const auto serialized = imuMsg.dump();
      server->broadcastMessage(channelIds[3], now,
                               reinterpret_cast<const uint8_t*>(serialized.data()),
                               serialized.size());
    }

    // 5. 状态信息 (每2秒更新一次)
    if (seq % 40 == 0) {
      std::vector<std::string> statuses = {"RUNNING", "IDLE", "CHARGING", "ERROR", "STANDBY"};
      const auto statusMsg = nlohmann::json{
        {"data", statuses[(seq / 40) % statuses.size()]}
      };
      const auto serialized = statusMsg.dump();
      server->broadcastMessage(channelIds[4], now,
                               reinterpret_cast<const uint8_t*>(serialized.data()),
                               serialized.size());
    }

    // 6. 系统诊断 (每秒更新一次)
    if (seq % 20 == 0) {
      const auto diagMsg = nlohmann::json{
        {"battery_voltage", 12.0 + 0.5 * std::sin(t * 0.05) + noise(gen) * 0.1},
        {"cpu_usage", 30.0 + 20.0 * std::sin(t * 0.3) + 10.0},
        {"memory_usage", 45.0 + 15.0 * std::cos(t * 0.2)},
        {"disk_usage", 60.0 + 5.0 * std::sin(t * 0.1)}
      };
      const auto serialized = diagMsg.dump();
      server->broadcastMessage(channelIds[5], now,
                               reinterpret_cast<const uint8_t*>(serialized.data()),
                               serialized.size());
    }

    if (seq % 100 == 0) {
      std::cout << "Published " << seq << " message cycles..." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // 20Hz
  }

  std::cout << "Cleaning up..." << std::endl;
  server->removeChannels(channelIds);
  server->stop();

  return 0;
}