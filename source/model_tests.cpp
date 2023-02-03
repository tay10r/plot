#include <gtest/gtest.h>

#include "model.hpp"

TEST(model, load_yaml)
{
  constexpr char* yaml_src = R"(
  csv_data_sets:
  - path: "a.csv"
    id: 0
  - path: "b.csv"
    id: 1
  charts:
  - name: "View"
    line_plots:
    - data_set_id: 0
    - data_set_id: 1
  )";
}
