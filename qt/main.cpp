#include "GameWindow.h"

#include "app/GameSession.h"
#include "infra/CsvScoreRepository.h"
#include "infra/RandomMT.h"
#include "infra/ScoreFilePath.h"
#include "Version.h"

#include <QApplication>
#include <QMessageBox>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <string_view>

// Qt 版の Composition Root: 具体クラスを組み立てて画面に渡す
int main(int argc, char* argv[]) {
    // 画面を作らずにバージョンだけ表示する（配布物の動作確認にも使う）
    if (argc > 1 && std::string_view(argv[1]) == "--version") {
        std::cout << kAppName << " " << kAppVersion << "\n";
        return EXIT_SUCCESS;
    }

    QApplication app(argc, argv);
    QApplication::setApplicationName(QString::fromLatin1(kAppName));
    QApplication::setApplicationVersion(QString::fromLatin1(kAppVersion));

    try {
        RandomMT random;
        CsvScoreRepository repo(defaultScoreFilePath());
        GameSession session(random, repo);

        GameWindow window(session, QString::fromStdString(repo.file().string()));
        window.setWindowTitle(QStringLiteral("%1 v%2")
            .arg(QString::fromLatin1(kAppName), QString::fromLatin1(kAppVersion)));
        window.show();
        return app.exec();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, QStringLiteral("Fatal error"), QString::fromUtf8(e.what()));
        return EXIT_FAILURE;
    }
}
