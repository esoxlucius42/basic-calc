#include "calculator_engine.h"

#include <cstdlib>
#include <iostream>
#include <string_view>

namespace {

void require(bool condition, std::string_view message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
  }
}

void press(CalculatorEngine& engine, std::initializer_list<Button> buttons) {
  for (Button button : buttons) {
    engine.press(button);
  }
}

void require_display_fits(const CalculatorEngine& engine, std::string_view message) {
  require(engine.display_text().size() <= 12, message);
}

void test_basic_addition() {
  CalculatorEngine engine;
  press(engine, {Button::Digit2, Button::Add, Button::Digit3, Button::Equals});
  require(engine.display_text() == "5", "2 + 3 should equal 5");
}

void test_immediate_execution() {
  CalculatorEngine engine;
  press(engine, {Button::Digit2, Button::Add, Button::Digit3, Button::Multiply, Button::Digit4, Button::Equals});
  require(engine.display_text() == "20", "immediate execution should evaluate left to right");
}

void test_repeated_equals() {
  CalculatorEngine engine;
  press(engine, {Button::Digit2, Button::Add, Button::Digit3, Button::Equals, Button::Equals});
  require(engine.display_text() == "8", "repeated equals should repeat the last operation");
}

void test_square_root() {
  CalculatorEngine engine;
  press(engine, {Button::Digit8, Button::Digit1, Button::SquareRoot});
  require(engine.display_text() == "9", "sqrt(81) should equal 9");
}

void test_divide_by_zero_error() {
  CalculatorEngine engine;
  press(engine, {Button::Digit9, Button::Divide, Button::Digit0, Button::Equals});
  require(engine.is_error(), "division by zero should produce an error");
  require(engine.display_text() == "Error", "error display text should be shown");
}

void test_sign_toggle() {
  CalculatorEngine engine;
  press(engine, {Button::Digit5, Button::ToggleSign, Button::Multiply, Button::Digit2, Button::Equals});
  require(engine.display_text() == "-10", "sign toggle should negate the current value");
}

void test_rounds_fraction_to_fit_display() {
  CalculatorEngine engine;
  press(engine, {Button::Digit2, Button::Divide, Button::Digit3, Button::Equals});
  require(engine.display_text() == "0.666666667", "2 / 3 should be rounded to fit the display");
  require_display_fits(engine, "rounded fixed-point result should fit the display");
}

void test_negative_fixed_point_uses_full_display_budget() {
  CalculatorEngine engine;
  press(engine, {Button::Digit1, Button::Digit0, Button::Digit0, Button::Digit0, Button::Digit0,
                 Button::Digit0, Button::Digit0, Button::Digit0, Button::Digit0, Button::Digit0,
                 Button::Digit0, Button::ToggleSign});
  require(engine.display_text() == "-10000000000", "negative 11-digit integer should still fit on the display");
  require_display_fits(engine, "negative fixed-point result should fit the display");
}

void test_large_values_use_scientific_notation() {
  CalculatorEngine engine;
  press(engine, {Button::Digit9, Button::Digit9, Button::Digit9, Button::Digit9, Button::Digit9,
                 Button::Digit9, Button::Digit9, Button::Digit9, Button::Digit9, Button::Digit9,
                 Button::Digit9, Button::Add, Button::Digit1, Button::Equals});
  require(engine.display_text() == "1e11", "12-digit results should fall back to scientific notation");
  require_display_fits(engine, "scientific notation for large values should fit the display");
}

void test_small_values_use_scientific_notation() {
  CalculatorEngine engine;
  press(engine, {Button::Digit1, Button::Divide, Button::Digit1, Button::Digit0, Button::Digit0,
                 Button::Digit0, Button::Digit0, Button::Digit0, Button::Digit0, Button::Digit0,
                 Button::Digit0, Button::Digit0, Button::Digit0, Button::Equals});
  require(engine.display_text() == "1e-10", "very small results should fall back to scientific notation");
  require_display_fits(engine, "scientific notation for small values should fit the display");
}

void test_scientific_display_starts_new_manual_entry() {
  CalculatorEngine engine;
  press(engine, {Button::Digit1, Button::Divide, Button::Digit1, Button::Digit0, Button::Digit0,
                 Button::Digit0, Button::Digit0, Button::Digit0, Button::Digit0, Button::Digit0,
                 Button::Digit0, Button::Digit0, Button::Digit0, Button::Equals, Button::Digit2});
  require(engine.display_text() == "2", "typing after a scientific-notation result should start a new entry");
}

void test_reset_restores_zero_display() {
  CalculatorEngine engine;
  press(engine, {Button::Digit9, Button::Multiply, Button::Digit8, Button::Equals});
  engine.reset();
  require(engine.display_text() == "0", "reset should restore the display to 0");
  require(!engine.is_error(), "reset should clear any error state");
}

}  // namespace

int main() {
  test_basic_addition();
  test_immediate_execution();
  test_repeated_equals();
  test_square_root();
  test_divide_by_zero_error();
  test_sign_toggle();
  test_rounds_fraction_to_fit_display();
  test_negative_fixed_point_uses_full_display_budget();
  test_large_values_use_scientific_notation();
  test_small_values_use_scientific_notation();
  test_scientific_display_starts_new_manual_entry();
  test_reset_restores_zero_display();

  std::cout << "All calculator engine tests passed.\n";
  return 0;
}
