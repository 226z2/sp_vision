#ifndef IO__PBLISH2NAV_HPP
#define IO__PBLISH2NAV_HPP

#include <Eigen/Dense>  // For Eigen::Vector3d
#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "sp_msgs/msg/dm02_serial_copy_msg.hpp"
#include "std_msgs/msg/string.hpp"

namespace io
{
class Publish2Nav : public rclcpp::Node
{
public:
  Publish2Nav();

  ~Publish2Nav();

  void start();

  void send_data(const Eigen::Vector4d & data);
  void send_serial_copy(const sp_msgs::msg::Dm02SerialCopyMsg & msg);

private:
  // ROS2 发布者
  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::Publisher<sp_msgs::msg::Dm02SerialCopyMsg>::SharedPtr serial_copy_publisher_;
};

}  // namespace io

#endif  // Publish2Nav_HPP_
