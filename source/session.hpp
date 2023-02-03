#pragma once

#include "data_set.hpp"
#include "logger.hpp"
#include "model.hpp"

class session final
{
public:
  //===================//
  // Operation Classes //
  //===================//

  class close_data_set_operation;

  class operation_visitor
  {
  public:
    operation_visitor() = default;

    operation_visitor(const operation_visitor&) = default;

    operation_visitor(operation_visitor&&) = default;

    operation_visitor& operator=(const operation_visitor&) = default;

    operation_visitor& operator=(operation_visitor&&) = default;

    virtual ~operation_visitor() = default;

    virtual void visit(const close_data_set_operation&) = 0;
  };

  class operation
  {
  public:
    operation() = default;

    operation(const operation&) = default;

    operation(operation&&) = default;

    operation& operator=(const operation&) = default;

    operation& operator=(operation&&) = default;

    virtual ~operation() = default;

    virtual void accept(operation_visitor&) const = 0;
  };

  class close_data_set_operation final : public operation
  {
  public:
    explicit close_data_set_operation(const data_set* ds)
      : data_set_(ds)
    {
    }

    void accept(operation_visitor& visitor) const override
    {
      visitor.visit(*this);
    }

    [[nodiscard]] const data_set* get_data_set() const { return data_set_; }

  private:
    const data_set* data_set_{ nullptr };
  };

  //==================//
  // Member Functions //
  //==================//

  void add_data_set(std::unique_ptr<data_set> ds);

  void apply_operation(const operation& op);

  void accept_data_set_visitor(data_set_visitor& visitor) const
  {
    for (const auto& ds : data_sets_)
      ds->accept(visitor);
  }

  [[nodiscard]] std::size_t get_data_set_count() const
  {
    return data_sets_.size();
  }

  /// @return True if the data set was found and closed, false otherwise.
  bool close_data_set(const data_set* ds);

  [[nodiscard]] const data_set* get_data_set(const std::size_t index) const
  {
    return data_sets_.at(index).get();
  }

  void add_logger_sink(logger::sink* s);

  [[nodiscard]] logger* get_logger();

  /// Gets a read-only pointer to the current model.
  ///
  /// @return A pointer to the current model.
  [[nodiscard]] const model* get_model() const;

  /// Creates a new chart.
  void new_chart();

  /// Used to edit the model.
  ///
  /// Will make a new copy of the model and modify the edit history buffer.
  ///
  /// @return A pointer to the model to edit.
  model* edit_model();

  /// If possible, go back into the model history before a change was made.
  void undo();

  /// If possible, re-apply a previously reverted model change.
  void redo();

private:
  /// The edit history for the project model.
  std::vector<model> model_history_{ model() };

  /// The index of the current model.
  std::size_t model_index_{ 0 };

  std::vector<std::unique_ptr<data_set>> data_sets_;

  logger logger_;
};
