#pragma once

#include <imgui.h>

class menu_bar final
{
public:
  enum class action
  {
    none,
    new_chart,
    load_csv,
    load_tsv,
    undo,
    redo,
    change_font_size,
    exit
  };

  action render();

  [[nodiscard]] bool side_panel_visible() const { return side_panel_visible_; }

  [[nodiscard]] ImVec2 size() const { return size_; }

  [[nodiscard]] float font_size() const { return font_size_; }

  [[nodiscard]] float margin() const { return margin_; }

  [[nodiscard]] float side_panel_size() const { return side_panel_visible_ ? (side_panel_size_ * font_size_) : 0.0f; }

private:
  bool side_panel_visible_{ true };

  float side_panel_size_{ 20 };

  ImVec2 size_{ 0, 0 };

  float font_size_{ 16 };

  float margin_{ 4 };
};
