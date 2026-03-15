#include "calculator_engine.h"

#include <cmath>
#include <iomanip>
#include <sstream>

namespace {

constexpr double kEpsilon = 1e-12;
constexpr std::size_t kMaxDisplayChars = 12;
constexpr std::size_t kMaxMagnitudeChars = 11;

bool is_zero(double value) {
  return std::fabs(value) < kEpsilon;
}

bool uses_scientific_notation(const std::string& text) {
  return text.find_first_of("eE") != std::string::npos;
}

std::string trim_decimal_zeros(std::string text) {
  const std::size_t exponent_pos = text.find_first_of("eE");
  std::string mantissa = exponent_pos == std::string::npos ? text : text.substr(0, exponent_pos);

  const std::size_t decimal_pos = mantissa.find('.');
  if (decimal_pos != std::string::npos) {
    while (!mantissa.empty() && mantissa.back() == '0') {
      mantissa.pop_back();
    }
    if (!mantissa.empty() && mantissa.back() == '.') {
      mantissa.pop_back();
    }
  }

  if (mantissa == "-0") {
    mantissa = "0";
  }

  if (exponent_pos == std::string::npos) {
    return mantissa;
  }

  return mantissa + "e" + std::to_string(std::stoi(text.substr(exponent_pos + 1)));
}

std::size_t magnitude_length(const std::string& text) {
  return !text.empty() && text.front() == '-' ? text.size() - 1 : text.size();
}

bool fits_display_budget(const std::string& text) {
  return text.size() <= kMaxDisplayChars && magnitude_length(text) <= kMaxMagnitudeChars;
}

int integer_digits(double value) {
  const double abs_value = std::fabs(value);
  if (abs_value < 1.0) {
    return 1;
  }
  return static_cast<int>(std::floor(std::log10(abs_value))) + 1;
}

std::string format_fixed_candidate(double value, int fractional_digits) {
  std::ostringstream stream;
  stream << std::fixed << std::setprecision(fractional_digits) << value;
  return trim_decimal_zeros(stream.str());
}

std::string format_scientific_candidate(double value, int fractional_digits) {
  std::ostringstream stream;
  stream << std::scientific << std::setprecision(fractional_digits) << value;
  return trim_decimal_zeros(stream.str());
}

std::string fit_fixed_point(double value) {
  const int digits_before_decimal = integer_digits(value);
  if (digits_before_decimal > static_cast<int>(kMaxMagnitudeChars)) {
    return {};
  }

  const int max_fractional_digits =
      std::max(0, static_cast<int>(kMaxMagnitudeChars) - digits_before_decimal - 1);
  for (int fractional_digits = max_fractional_digits; fractional_digits >= 0; --fractional_digits) {
    const std::string candidate = format_fixed_candidate(value, fractional_digits);
    if (candidate != "0" && fits_display_budget(candidate)) {
      return candidate;
    }
  }

  return {};
}

std::string fit_scientific(double value) {
  for (int fractional_digits = 9; fractional_digits >= 0; --fractional_digits) {
    const std::string candidate = format_scientific_candidate(value, fractional_digits);
    if (fits_display_budget(candidate)) {
      return candidate;
    }
  }

  return value < 0.0 ? "-9.9e999" : "9.9e999";
}

}  // namespace

CalculatorEngine::CalculatorEngine() {
  reset();
}

void CalculatorEngine::press(Button button) {
  if (error_state_) {
    if ((button >= Button::Digit0 && button <= Button::Digit9) || button == Button::Decimal) {
      reset();
      press(button);
    }
    return;
  }

  switch (button) {
    case Button::ToggleSign:
      apply_sign_toggle();
      return;
    case Button::SquareRoot:
      apply_square_root();
      return;
    case Button::Digit0:
    case Button::Digit1:
    case Button::Digit2:
    case Button::Digit3:
    case Button::Digit4:
    case Button::Digit5:
    case Button::Digit6:
    case Button::Digit7:
    case Button::Digit8:
    case Button::Digit9: {
      const char digit = static_cast<char>('0' + (static_cast<int>(button) - static_cast<int>(Button::Digit0)));
      prepare_for_manual_entry();
      if (editing_entry_ && uses_scientific_notation(current_entry_)) {
        begin_entry_with_text(std::string(1, digit));
        return;
      }
      if (!editing_entry_) {
        begin_entry_with_text(std::string(1, digit));
        return;
      }
      if (current_entry_ == "0") {
        current_entry_ = std::string(1, digit);
      } else if (current_entry_ == "-0") {
        current_entry_ = "-" + std::string(1, digit);
      } else {
        current_entry_.push_back(digit);
      }
      display_ = current_entry_;
      return;
    }
    case Button::Decimal:
      prepare_for_manual_entry();
      if (editing_entry_ && uses_scientific_notation(current_entry_)) {
        begin_entry_with_text("0.");
        return;
      }
      if (!editing_entry_) {
        begin_entry_with_text("0.");
      } else if (current_entry_.find('.') == std::string::npos) {
        if (current_entry_.empty() || current_entry_ == "-") {
          current_entry_ += "0.";
        } else {
          current_entry_ += '.';
        }
        display_ = current_entry_;
      }
      return;
    case Button::Add:
      apply_binary_operator(BinaryOp::Add);
      return;
    case Button::Subtract:
      apply_binary_operator(BinaryOp::Subtract);
      return;
    case Button::Multiply:
      apply_binary_operator(BinaryOp::Multiply);
      return;
    case Button::Divide:
      apply_binary_operator(BinaryOp::Divide);
      return;
    case Button::Equals:
      apply_equals();
      return;
  }
}

std::string CalculatorEngine::display_text() const {
  return display_;
}

bool CalculatorEngine::is_error() const {
  return error_state_;
}

void CalculatorEngine::reset() {
  accumulator_ = 0.0;
  last_rhs_ = 0.0;
  pending_op_ = BinaryOp::None;
  last_op_ = BinaryOp::None;
  current_entry_.clear();
  display_ = "0";
  has_accumulator_ = false;
  editing_entry_ = false;
  error_state_ = false;
  just_evaluated_ = false;
}

void CalculatorEngine::set_error() {
  accumulator_ = 0.0;
  last_rhs_ = 0.0;
  pending_op_ = BinaryOp::None;
  last_op_ = BinaryOp::None;
  current_entry_.clear();
  display_ = "Error";
  has_accumulator_ = false;
  editing_entry_ = false;
  error_state_ = true;
  just_evaluated_ = false;
}

void CalculatorEngine::prepare_for_manual_entry() {
  if (just_evaluated_ && pending_op_ == BinaryOp::None) {
    accumulator_ = 0.0;
    has_accumulator_ = false;
    last_op_ = BinaryOp::None;
    last_rhs_ = 0.0;
  }
  just_evaluated_ = false;
}

void CalculatorEngine::begin_entry_with_text(const std::string& text) {
  current_entry_ = text;
  display_ = current_entry_;
  editing_entry_ = true;
}

void CalculatorEngine::set_result(double value) {
  if (is_zero(value)) {
    value = 0.0;
  }
  accumulator_ = value;
  has_accumulator_ = true;
  current_entry_.clear();
  display_ = format_value(value);
  editing_entry_ = false;
}

void CalculatorEngine::apply_binary_operator(BinaryOp op) {
  if (editing_entry_) {
    const double rhs = parse_entry();
    if (has_accumulator_ && pending_op_ != BinaryOp::None) {
      bool ok = true;
      const double result = execute(pending_op_, accumulator_, rhs, &ok);
      if (!ok) {
        set_error();
        return;
      }
      set_result(result);
    } else {
      set_result(rhs);
    }
  } else if (!has_accumulator_) {
    set_result(parse_text(display_));
  }

  pending_op_ = op;
  just_evaluated_ = false;
}

void CalculatorEngine::apply_equals() {
  if (pending_op_ != BinaryOp::None) {
    if (!has_accumulator_) {
      accumulator_ = 0.0;
      has_accumulator_ = true;
    }

    double rhs = 0.0;
    if (editing_entry_) {
      rhs = parse_entry();
    } else if (last_op_ == BinaryOp::None) {
      rhs = accumulator_;
    } else {
      rhs = last_rhs_;
    }

    bool ok = true;
    const double result = execute(pending_op_, accumulator_, rhs, &ok);
    if (!ok) {
      set_error();
      return;
    }

    last_rhs_ = rhs;
    last_op_ = pending_op_;
    pending_op_ = BinaryOp::None;
    set_result(result);
    just_evaluated_ = true;
    return;
  }

  if (last_op_ != BinaryOp::None && has_accumulator_) {
    bool ok = true;
    const double result = execute(last_op_, accumulator_, last_rhs_, &ok);
    if (!ok) {
      set_error();
      return;
    }
    set_result(result);
    just_evaluated_ = true;
    return;
  }

  if (editing_entry_) {
    set_result(parse_entry());
  }
  just_evaluated_ = true;
}

void CalculatorEngine::apply_square_root() {
  const double value = current_value();
  if (value < 0.0) {
    set_error();
    return;
  }

  const double result = std::sqrt(value);
  if (editing_entry_ || pending_op_ != BinaryOp::None) {
    begin_entry_with_text(format_value(result));
  } else {
    set_result(result);
    just_evaluated_ = true;
  }
}

void CalculatorEngine::apply_sign_toggle() {
  double value = current_value();
  value = -value;

  if (editing_entry_ || pending_op_ != BinaryOp::None) {
    begin_entry_with_text(format_value(value));
  } else {
    set_result(value);
    just_evaluated_ = true;
  }
}

double CalculatorEngine::current_value() const {
  if (editing_entry_) {
    return parse_entry();
  }
  if (has_accumulator_) {
    return accumulator_;
  }
  return parse_text(display_);
}

double CalculatorEngine::parse_entry() const {
  return parse_text(current_entry_.empty() ? display_ : current_entry_);
}

double CalculatorEngine::parse_text(const std::string& text) {
  return std::stod(text);
}

std::string CalculatorEngine::format_value(double value) {
  if (!std::isfinite(value)) {
    return "Error";
  }

  if (is_zero(value)) {
    return "0";
  }

  if (const std::string fixed = fit_fixed_point(value); !fixed.empty()) {
    return fixed;
  }

  return fit_scientific(value);
}

double CalculatorEngine::execute(BinaryOp op, double lhs, double rhs, bool* ok) {
  *ok = true;
  switch (op) {
    case BinaryOp::None:
      return rhs;
    case BinaryOp::Add:
      return lhs + rhs;
    case BinaryOp::Subtract:
      return lhs - rhs;
    case BinaryOp::Multiply:
      return lhs * rhs;
    case BinaryOp::Divide:
      if (is_zero(rhs)) {
        *ok = false;
        return 0.0;
      }
      return lhs / rhs;
  }

  *ok = false;
  return 0.0;
}
