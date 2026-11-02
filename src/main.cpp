#include "app/GameController.h"
#include "app/ports/IGameUI.h"
#include "infra/RandomMT.h"
#include "infra/CsvScoreRepository.h"
#include "infra/ScoreFilePath.h"
#include "ui/ConsoleUI.h"

#include <cstdlib>
#include <exception>
#include <iostream>

// 例外境界: ここより外へ例外を漏らさず、必ず終了コードで結果を返す
int main() {
    try {
        RandomMT random;
        CsvScoreRepository repo(defaultScoreFilePath());
        ConsoleUI ui;

        std::cout << "Scores are saved to: " << repo.file().string() << "\n\n";

        GameController controller(random, repo, ui);
        controller.run();
        return EXIT_SUCCESS;
    }
    catch (const InputClosedError&) {
        // ユーザーが入力を終了した（正常な中断）
        std::cout << "\nInput closed. Bye.\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Fatal error: unknown exception\n";
        return EXIT_FAILURE;
    }
}
