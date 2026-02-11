#pragma once
#include <variant>

struct TooSmall {};
struct TooLarge {};
struct Correct { int attempts; };

using GameEvent = std::variant<TooSmall, TooLarge, Correct>;
