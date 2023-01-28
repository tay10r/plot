#pragma once

#include <vector>
#include <string>
#include <algorithm>

namespace csv {

enum column_type
{
  numerical,
  categorical,
  date_time,
  unknown
};

struct column final
{
  std::string name;

  std::vector<std::string> data;

  [[nodiscard]] column_type guess_column_type() const;

  template<typename T, typename Func>
  void transform(std::vector<T>& output, Func func) const
  {
    output.resize(data.size());

    std::transform(data.begin(), data.end(), output.begin(), func);
  }
};

struct file final
{
  std::vector<column> columns;

  std::size_t row_count{ 0 };

  void load(const char* path, char separator = ',');
};

} // namespace csv
