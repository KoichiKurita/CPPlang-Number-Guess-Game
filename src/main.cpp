#include "app/GameController.h"
#include "infra/RandomMT.h"
#include "infra/CsvScoreRepository.h"
#include "ui/ConsoleUI.h"

int main() {
    RandomMT random;
    CsvScoreRepository repo;
    ConsoleUI ui;

    GameController controller(random, repo, ui);
    controller.run();
}
