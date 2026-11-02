#include "ui/ConsoleUI.h"

#include "domain/InputParser.h"
#include "domain/PlayerName.h"

#include <iostream>
#include <type_traits>
#include <variant>

namespace {

// 入力行の最大長。極端に長い入力を数値として扱わないための上限。
constexpr std::size_t kMaxLineLength = 256;

const char* errorMessage(InputError e) {
    switch (e) {
        case InputError::Empty:      return "Please enter a number.";
        case InputError::NotANumber: return "Invalid input. Digits only.";
        case InputError::OutOfRange: return "Out of range.";
    }
    return "Invalid input.";
}

const char* nameErrorMessage(NameError e) {
    switch (e) {
        case NameError::Empty:            return "Please enter a name.";
        case NameError::TooLong:          return "Name is too long.";
        case NameError::InvalidStart:     return "Name must start with a letter or digit.";
        case NameError::InvalidCharacter: return "Use only letters, digits, spaces, '_' or '-'.";
    }
    return "Invalid name.";
}

} // namespace

ConsoleUI::ConsoleUI() : ConsoleUI(std::cin, std::cout) {}

ConsoleUI::ConsoleUI(std::istream& in, std::ostream& out)
    : in_(in), out_(out) {}

std::string ConsoleUI::readLine() {
    std::string line;
    if (!std::getline(in_, line)) {
        throw InputClosedError{};
    }
    if (line.size() > kMaxLineLength) {
        line.clear();  // 長すぎる入力は破棄して「空入力」として扱う
    }
    return line;
}

std::string ConsoleUI::inputPlayerName() {
    while (true) {
        out_ << "Your name (1-" << kMaxPlayerNameLength << " chars): " << std::flush;
        const NameResult r = parsePlayerName(readLine());

        if (const std::string* name = std::get_if<std::string>(&r)) {
            return *name;
        }
        out_ << nameErrorMessage(std::get<NameError>(r)) << "\n";
    }
}

Difficulty ConsoleUI::selectDifficulty() {
    while (true) {
        out_ << "1: Easy  2: Normal  3: Hard\n> " << std::flush;
        if (auto diff = parseDifficulty(readLine())) {
            return *diff;
        }
        out_ << "Please choose 1, 2 or 3.\n";
    }
}

int ConsoleUI::inputGuess(const Difficulty& diff) {
    while (true) {
        out_ << "Guess (" << diff.min << "-" << diff.max << "): " << std::flush;
        const ParseResult r = parseGuess(readLine(), diff);

        if (const int* value = std::get_if<int>(&r)) {
            return *value;
        }
        out_ << errorMessage(std::get<InputError>(r)) << "\n";
    }
}

void ConsoleUI::handleEvent(const GameEvent& event) {
    std::visit([&](auto&& e) {
        using T = std::decay_t<decltype(e)>;

        if constexpr (std::is_same_v<T, TooSmall>) {
            out_ << "Too small\n";
        }
        else if constexpr (std::is_same_v<T, TooLarge>) {
            out_ << "Too large\n";
        }
        else if constexpr (std::is_same_v<T, Correct>) {
            out_ << "Correct! Attempts: " << e.attempts << "\n";
        }
    }, event);
}


void ConsoleUI::showBestScore(const ScoreRecord& best, bool isNewRecord) {
    if (isNewRecord) {
        out_ << "New record! ";
    }
    out_ << "Best for " << best.difficulty << ": "
         << best.attempts << " attempts by " << best.player << "\n";
}

void ConsoleUI::showWarning(const std::string& message) {
    out_ << "Warning: " << message << "\n";
}
