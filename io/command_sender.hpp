#ifndef IO__COMMAND_SENDER_HPP
#define IO__COMMAND_SENDER_HPP

#include "io/command.hpp"

namespace io
{
class CommandSender
{
public:
  virtual ~CommandSender() = default;
  virtual void send(Command command) = 0;
};
}  // namespace io

#endif  // IO__COMMAND_SENDER_HPP

