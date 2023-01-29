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
      data_sets_.erase(it);
      return true;
    }
  }
  return false;
}
