#pragma once

#include <imgui.h>

class menu_bar final
{
public:
  enum class action
  {
    none,
    load_file,
    exit
  };

  static action render(ImVec2* size);
};
