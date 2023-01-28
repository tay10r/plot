#include "menu_bar.hpp"

menu_bar::action
menu_bar::render()
{
  auto action = menu_bar::action::none;

  if (!ImGui::BeginMainMenuBar())
    return action;

  size_ = ImGui::GetWindowSize();

  if (ImGui::BeginMenu("File")) {

    if (ImGui::MenuItem("Load CSV"))
      action = menu_bar::action::load_csv;

    if (ImGui::MenuItem("Load TSV"))
      action = menu_bar::action::load_tsv;

    ImGui::Separator();

    if (ImGui::MenuItem("Exit"))
      action = menu_bar::action::exit;

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("View")) {

    ImGui::Checkbox("Side Panel", &side_panel_visible_);

    ImGui::SliderFloat("Side Panel Size", &side_panel_size_, 10, 40);

    if (ImGui::SliderFloat("Font Size", &font_size_, 8, 24))
      action = action::change_font_size;

    ImGui::SliderFloat("Margin", &margin_, 0, 32);

    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Help")) {
    ImGui::EndMenu();
  }

  ImGui::EndMainMenuBar();

  return action;
}
