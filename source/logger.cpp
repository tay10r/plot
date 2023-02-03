#include "logger.hpp"

void
logger::add_sink(sink* s)
{
  sinks_.emplace_back(s);
}

void
logger::publish_log_record(level l, const std::string& msg) const
{
  for (auto* s : sinks_)
    s->write(l, msg.c_str());
}
