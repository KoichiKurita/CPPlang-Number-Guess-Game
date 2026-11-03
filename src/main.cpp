#include "Version.h"
#include "app/GameController.h"
#include "app/ports/IGameUI.h"
#include "infra/CsvScoreRepository.h"
#include "infra/RandomMT.h"
#include "infra/ScoreFilePath.h"
#include "ui/ConsoleUI.h"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <string_view>

namespace {

constexpr const char* kUsage = "Usage: NumberGuessGame [--version | --help]\n";

// コマンドライン引数を処理する。処理した場合は終了コードを返す（ゲームは始めない）。
std::optional<int> handleArguments(int argc, char* argv[]) {
    if (argc <= 1) {
        return std::nullopt;
    }
    const std::string_view arg = argv[1];
    if (arg == "--version") {
        std::cout << kAppName << " " << kAppVersion << "\n";
        return EXIT_SUCCESS;
    }
    if (arg == "--help") {
        std::cout << kUsage;
        return EXIT_SUCCESS;
    }
    std::cerr << "Unknown option: " << arg << "\n" << kUsage;
    return 2;
}

} // namespace

// 例外境界: ここより外へ例外を漏らさず、必ず終了コードで結果を返す
int main(int argc, char* argv[]) {
    if (const auto exitCode = handleArguments(argc, argv)) {
        return *exitCode;
    }

    try {
        RandomMT random;
        CsvScoreRepository repo(defaultScoreFilePath());
        ConsoleUI ui;

        std::cout << kAppName << " v" << kAppVersion << "\n"
                  << "Scores are saved to: " << repo.file().string() << "\n\n";

        GameController controller(random, repo, ui);
        controller.run();
        return EXIT_SUCCESS;
    } catch (const InputClosedError&) {
        // ユーザーが入力を終了した（正常な中断）
        std::cout << "\nInput closed. Bye.\n";
        return EXIT_SUCCESS;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "Fatal error: unknown exception\n";
        return EXIT_FAILURE;
    }
}
