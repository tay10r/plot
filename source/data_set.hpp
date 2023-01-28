#pragma once

#include <mlpack/core.hpp>

#include <vector>
#include <string>

namespace csv {

struct file;

} // namespace csv

class csv_data_set;
class img_data_set;

class data_set_visitor
{
public:
  data_set_visitor() = default;

  data_set_visitor(const data_set_visitor&) = delete;

  data_set_visitor(data_set_visitor&&) = default;

  data_set_visitor& operator=(const data_set_visitor&) = delete;

  data_set_visitor& operator=(data_set_visitor&&) = default;

  virtual ~data_set_visitor() = default;

  virtual void visit(const csv_data_set&) = 0;

  virtual void visit(const img_data_set&) = 0;
};

class data_set
{
public:
  data_set() = default;

  data_set(const data_set&) = delete;

  data_set(data_set&&) = default;

  data_set& operator=(const data_set&) = delete;

  data_set& operator=(data_set&&) = default;

  virtual ~data_set() = default;

  virtual void accept(data_set_visitor&) const = 0;
};

class csv_data_set final : public data_set
{
public:
  explicit csv_data_set(const csv::file& file);

  void accept(data_set_visitor& visitor) const override
  {
    visitor.visit(*this);
  }

  [[nodiscard]] const char* get_column_name(const std::size_t index) const
  {
    return column_names_.at(index).c_str();
  }

  [[nodiscard]] std::size_t get_column_count() const { return column_names_.size(); }

  const arma::Mat<float>& get_data() const { return data_; }

  const arma::Mat<int>& get_indices() const { return indices_; }

private:
  std::vector<std::string> column_names_;

  arma::Mat<float> data_;

  arma::Mat<int> indices_;
};
