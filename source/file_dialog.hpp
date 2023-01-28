#pragma once

#include <memory>
#include <vector>
#include <string>

class file_dialog
{
public:
  using callback = void(*)(void* caller, const std::vector<std::string>& paths);

  static std::unique_ptr<file_dialog> create_data_load_dialog(const char* title,
                                                              const char* ext,
                                                              void* caller,
                                                              const callback cb);

  file_dialog() = default;

  file_dialog(const file_dialog&) = delete;

  file_dialog(file_dialog&&) = delete;

  file_dialog& operator=(const file_dialog&) = delete;

  file_dialog& operator=(file_dialog&&) = delete;

  virtual ~file_dialog() = default;

  virtual bool poll() = 0;
};
