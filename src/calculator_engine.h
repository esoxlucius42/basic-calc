#pragma once

#include <string>

enum class Button {
  ToggleSign,
  SquareRoot,
  Digit0,
  Digit1,
  Digit2,
  Digit3,
  Digit4,
  Digit5,
  Digit6,
  Digit7,
  Digit8,
  Digit9,
  Decimal,
  Add,
  Subtract,
  Multiply,
  Divide,
  Equals
};

class CalculatorEngine {
 public:
  CalculatorEngine();

  void press(Button button);
  void reset();

  [[nodiscard]] std::string display_text() const;
  [[nodiscard]] bool is_error() const;

 private:
  enum class BinaryOp {
    None,
    Add,
    Subtract,
    Multiply,
    Divide,
  };

  void set_error();
  void prepare_for_manual_entry();
  void begin_entry_with_text(const std::string& text);
  void set_result(double value);
  void apply_binary_operator(BinaryOp op);
  void apply_equals();
  void apply_square_root();
  void apply_sign_toggle();

  [[nodiscard]] double current_value() const;
  [[nodiscard]] double parse_entry() const;
  [[nodiscard]] static double parse_text(const std::string& text);
  [[nodiscard]] static std::string format_value(double value);
  [[nodiscard]] static double execute(BinaryOp op, double lhs, double rhs, bool* ok);

  double accumulator_ = 0.0;
  double last_rhs_ = 0.0;
  BinaryOp pending_op_ = BinaryOp::None;
  BinaryOp last_op_ = BinaryOp::None;
  std::string current_entry_;
  std::string display_ = "0";
  bool has_accumulator_ = false;
  bool editing_entry_ = false;
  bool error_state_ = false;
  bool just_evaluated_ = false;
};
