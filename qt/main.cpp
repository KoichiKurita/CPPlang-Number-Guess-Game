#include "GameWindow.h"

#include "app/GameSession.h"
#include "infra/CsvScoreRepository.h"
#include "infra/RandomMT.h"
#include "infra/ScoreFilePath.h"

#include <QApplication>
#include <QMessageBox>

#include <cstdlib>
#include <exception>

// Qt 版の Composition Root: 具体クラスを組み立てて画面に渡す
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    try {
        RandomMT random;
        CsvScoreRepository repo(defaultScoreFilePath());
        GameSession session(random, repo);

        GameWindow window(session, QString::fromStdString(repo.file().string()));
        window.show();
        return app.exec();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, QStringLiteral("Fatal error"), QString::fromUtf8(e.what()));
        return EXIT_FAILURE;
    }
}
