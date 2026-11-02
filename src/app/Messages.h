#pragma once

#include "domain/InputParser.h"
#include "domain/PlayerName.h"

// 入力エラーを利用者向けの文言に変換する。Console 版と Qt 版で同じ文言を使う。
inline const char* toMessage(InputError e) {
    switch (e) {
        case InputError::Empty:      return "Please enter a number.";
        case InputError::NotANumber: return "Invalid input. Digits only.";
        case InputError::OutOfRange: return "Out of range.";
    }
    return "Invalid input.";
}

inline const char* toMessage(NameError e) {
    switch (e) {
        case NameError::Empty:            return "Please enter a name.";
        case NameError::TooLong:          return "Name is too long.";
        case NameError::InvalidStart:     return "Name must start with a letter or digit.";
        case NameError::InvalidCharacter: return "Use only letters, digits, spaces, '_' or '-'.";
    }
    return "Invalid name.";
}
