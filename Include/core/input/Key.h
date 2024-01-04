#pragma once

namespace Core {
enum class Key : unsigned char {
    // clang-format off
  A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
  Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
  Escape, LControl, LShift, LAlt, LSystem, RControl, RShift, RAlt, RSystem,
  Menu, LBracket, RBracket, Semicolon, Comma, Period, Quote, Slash, Backslash,
  Tilde, Equal, Hyphen, Space, Enter, Backspace, Tab, PageUp, PageDown, End, Home,
  Insert, Delete, Add, Subtract, Multiply, Divide,
  Left, Right, Up, Down,
  Kp0, Kp1, Kp2, Kp3, Kp4, Kp5, Kp6, Kp7, Kp8, Kp9,
  KpDivide, KpMultiply, KpSubtract, KpAdd, KpDecimal, KpEnter,
  Unknown
    // clang-format on
};
} // namespace Core