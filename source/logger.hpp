#pragma once

#include <vector>
#include <sstream>

class logger
{
public:
  enum class level
  {
    info,
    error
  };

  class sink
  {
  public:
    sink() = default;

    sink(const sink&) = delete;

    sink(sink&&) = delete;

    sink& operator=(const sink&) = delete;

    sink& operator=(sink&&) = delete;

    virtual ~sink() = default;

    virtual void write(level l, const char* msg) = 0;
  };

  void add_sink(sink* s);

  template<typename... Args>
  void info(Args ... args)
  {
    std::ostringstream stream;
    format(stream, args...);
    auto msg = stream.str();
    publish_log_record(level::info, msg);
  }

  template<typename... Args>
  void error(Args ... args)
  {
    std::ostringstream stream;
    format(stream, args...);
    auto msg = stream.str();
    publish_log_record(level::info, msg);
  }

private:
  template<typename Arg, typename... Args>
  static void format(std::ostream& stream, Arg arg, Args ...args)
  {
    stream << arg;
    format(stream, args...);
  }

  static void format(std::ostream&)
  {
  }

  void publish_log_record(level l, const std::string& msg) const;

private:
  std::vector<sink*> sinks_;
};
