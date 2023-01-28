#pragma once

#include <memory>

#include <GLFW/glfw3.h>

class application
{
public:
  static std::unique_ptr<application> create(GLFWwindow* window);

  application() = default;

  application(const application&) = delete;

  application(application&&) = default;

  application& operator=(const application&) = delete;

  application& operator=(application&&) = delete;

  virtual ~application() = default;

  virtual bool run() = 0;
};
