#pragma once

#include <memory>
#include <vector>
#include <string>

class file_dialog
{
public:
  static std::unique_ptr<file_dialog> create_data_load_dialog();

  file_dialog() = default;

  file_dialog(const file_dialog&) = delete;

  file_dialog(file_dialog&&) = delete;

  file_dialog& operator=(const file_dialog&) = delete;

  file_dialog& operator=(file_dialog&&) = delete;

  virtual ~file_dialog() = default;

  virtual bool ready() const = 0;

  virtual std::vector<std::string> results() = 0;
};
