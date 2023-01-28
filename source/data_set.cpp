#include "data_set.hpp"

#include "csv.hpp"

csv_data_set::csv_data_set(const csv::file& file)
{
  std::vector<std::size_t> column_indices;

  for (size_t i = 0; i < file.columns.size(); i++) {
    const auto type = file.columns.at(i).guess_column_type();
    if (type == csv::unknown)
      continue;

    if (type == csv::numerical)
      column_indices.emplace_back(i);
  }

  data_.resize(file.row_count, column_indices.size());

  indices_.resize(file.row_count, 1);

  for (size_t i = 0; i < column_indices.size(); ++i)
    indices_.at(i, 0) = static_cast<int>(i);

  for (size_t i = 0; i < column_indices.size(); ++i) {
    column_names_.emplace_back(file.columns.at(column_indices.at(i)).name);

    std::vector<float> row;

    file.columns.at(column_indices[i]).transform(
      row,
      [](const std::string& str)-> float { return std::strtof(str.c_str(), nullptr); });

    for (std::size_t j = 0; j < row.size(); ++j)
      data_.at(j, i) = row.at(j);
  }
}
