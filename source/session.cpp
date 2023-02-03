#include "session.hpp"

namespace {

class operation_interpreter final : public session::operation_visitor
{
public:
  explicit operation_interpreter(session* s)
    : session_(s)
  {
  }

  void visit(const session::close_data_set_operation& op) override
  {
    const data_set* data_set_to_delete = op.get_data_set();

    session_->close_data_set(data_set_to_delete);
  }

private:
  session* session_{ nullptr };
};

} // namespace

void
session::add_data_set(std::unique_ptr<data_set> ds)
{
  data_sets_.emplace_back(std::move(ds));
}

void
session::apply_operation(const operation& op)
{
  operation_interpreter interpreter(this);

  op.accept(interpreter);
}

bool
session::close_data_set(const data_set* ds)
{
  for (auto it = data_sets_.begin(); it != data_sets_.end(); it++) {
    if (it->get() == ds) {
      it->reset();
      data_sets_.erase(it);
      return true;
    }
  }
  return false;
}

void
session::add_logger_sink(logger::sink* s)
{
  logger_.add_sink(s);
}

logger*
session::get_logger()
{
  return &logger_;
}

void
session::new_chart()
{
  auto* m = edit_model();

  const auto id = m->generate_id();

  std::string chart_name{ "Untitled Chart" };

  size_t counter = 1;

  while (true) {

    auto is_valid_name{ true };

    for (const auto& c : m->charts) {
      if (c.second.name == chart_name) {
        std::ostringstream new_chart_name_stream;
        new_chart_name_stream << "Untitled Chart ";
        new_chart_name_stream << counter;
        chart_name = new_chart_name_stream.str();
        counter++;
        is_valid_name = false;
        break;
      }
    }

    if (!is_valid_name)
      continue;

    m->charts.emplace(id, model::chart{ chart_name });

    break;
  }
}

model*
session::edit_model()
{
  model_history_.resize(model_index_ + 2);

  model_history_[model_history_.size() - 1] = model_history_[model_history_.size() - 2];

  model_index_ = model_history_.size() - 1;

  return &model_history_.at(model_index_);
}

const model*
session::get_model() const
{
  return &model_history_.at(model_index_);
}

void
session::undo()
{
  if (model_index_ > 0)
    --model_index_;
}

void
session::redo()
{
  if ((model_index_ + 1) < model_history_.size())
    ++model_index_;
}
