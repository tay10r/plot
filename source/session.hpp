#pragma once

#include "data_set.hpp"

struct session final
{
  std::vector<std::unique_ptr<data_set>> data_sets_;
};

