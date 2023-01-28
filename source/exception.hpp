#pragma once

#include <stdexcept>

class file_open_exception : public std::runtime_error
{
public:
  file_open_exception(const char* what, std::string path);

  const std::string& get_path() const { return path_; }

private:
  std::string path_;
};
