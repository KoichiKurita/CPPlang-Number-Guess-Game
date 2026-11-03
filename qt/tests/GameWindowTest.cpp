#include "GameWindow.h"

#include "FakeScoreRepository.h"
#include "FixedClock.h"
#include "MockRandom.h"

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QtTest>

namespace {

// 画面の部品を objectName で探す。見つからなければテストを中断する
template <typename T> T* find(GameWindow& w, const char* name) {
    auto* widget = w.findChild<T*>(QString::fromLatin1(name));
    if (widget == nullptr) {
        qFatal("widget not found: %s", name);
    }
    return widget;
}

} // namespace

// 画面を実際に操作して（文字入力・クリック・Enter）振る舞いを確かめる
class GameWindowTest : public QObject {
    Q_OBJECT

private slots:
    void guessIsDisabledBeforeStart() {
        MockRandom random(5);
        FakeScoreRepository repo;
        FixedClock clock;
        GameSession session(random, repo, clock);
        GameWindow w(session);

        QVERIFY(!find<QLineEdit>(w, "guessEdit")->isEnabled());
        QVERIFY(!find<QPushButton>(w, "guessButton")->isEnabled());
    }

    void invalidNameShowsErrorAndDoesNotStart() {
        MockRandom random(5);
        FakeScoreRepository repo;
        FixedClock clock;
        GameSession session(random, repo, clock);
        GameWindow w(session);

        QTest::keyClicks(find<QLineEdit>(w, "nameEdit"), QStringLiteral("=1+1"));
        QTest::mouseClick(find<QPushButton>(w, "startButton"), Qt::LeftButton);

        QCOMPARE(find<QLabel>(w, "messageLabel")->text(),
                 QStringLiteral("Name must start with a letter or digit."));
        QVERIFY(!session.isPlaying());
    }

    void invalidGuessShowsErrorAndDoesNotCount() {
        MockRandom random(5);
        FakeScoreRepository repo;
        FixedClock clock;
        GameSession session(random, repo, clock);
        GameWindow w(session);

        QTest::keyClicks(find<QLineEdit>(w, "nameEdit"), QStringLiteral("Ko1"));
        QTest::mouseClick(find<QPushButton>(w, "startButton"), Qt::LeftButton);

        auto* guess = find<QLineEdit>(w, "guessEdit");
        QTest::keyClicks(guess, QStringLiteral("11")); // Easy の範囲外
        QTest::keyClick(guess, Qt::Key_Return);

        QCOMPARE(find<QLabel>(w, "messageLabel")->text(), QStringLiteral("Out of range."));
        QCOMPARE(session.attempts(), 0);
    }

    void playsToWinAndShowsBestScore() {
        MockRandom random(5);
        FakeScoreRepository repo;
        FixedClock clock;
        GameSession session(random, repo, clock);
        GameWindow w(session);

        find<QComboBox>(w, "difficultyBox")->setCurrentIndex(0); // Easy
        QTest::keyClicks(find<QLineEdit>(w, "nameEdit"), QStringLiteral("Ko1"));
        QTest::mouseClick(find<QPushButton>(w, "startButton"), Qt::LeftButton);
        QVERIFY(session.isPlaying());

        auto* guess = find<QLineEdit>(w, "guessEdit");
        QTest::keyClicks(guess, QStringLiteral("3"));
        QTest::keyClick(guess, Qt::Key_Return);
        QTest::keyClicks(guess, QStringLiteral("5"));
        QTest::mouseClick(find<QPushButton>(w, "guessButton"), Qt::LeftButton);

        QCOMPARE(find<QListWidget>(w, "historyList")->count(), 2);
        QCOMPARE(find<QLabel>(w, "messageLabel")->text(), QStringLiteral("Correct! Attempts: 2"));
        QCOMPARE(find<QLabel>(w, "bestLabel")->text(),
                 QStringLiteral("New record! Best for Easy: 2 attempts by Ko1"));
        QVERIFY(!guess->isEnabled());
        QCOMPARE(repo.records().size(), std::size_t{1});
    }

    void showsScoreFileLocationAndTooLargeHint() {
        MockRandom random(5);
        FakeScoreRepository repo;
        FixedClock clock;
        GameSession session(random, repo, clock);
        GameWindow w(session, QStringLiteral("C:/data/scores.csv"));

        QCOMPARE(find<QLabel>(w, "scoreFileLabel")->text(),
                 QStringLiteral("Scores: C:/data/scores.csv"));

        QTest::keyClicks(find<QLineEdit>(w, "nameEdit"), QStringLiteral("Ko1"));
        QTest::mouseClick(find<QPushButton>(w, "startButton"), Qt::LeftButton);
        auto* guess = find<QLineEdit>(w, "guessEdit");
        QTest::keyClicks(guess, QStringLiteral("8"));
        QTest::keyClick(guess, Qt::Key_Return);

        QCOMPARE(find<QLabel>(w, "messageLabel")->text(), QStringLiteral("Too large"));
    }

    void showsWarningWhenScoreCannotBeSaved() {
        MockRandom random(5);
        FakeScoreRepository repo;
        repo.failOnSave = true;
        FixedClock clock;
        GameSession session(random, repo, clock);
        GameWindow w(session);

        QTest::keyClicks(find<QLineEdit>(w, "nameEdit"), QStringLiteral("Ko1"));
        QTest::mouseClick(find<QPushButton>(w, "startButton"), Qt::LeftButton);
        auto* guess = find<QLineEdit>(w, "guessEdit");
        QTest::keyClicks(guess, QStringLiteral("5"));
        QTest::keyClick(guess, Qt::Key_Return);

        QCOMPARE(find<QLabel>(w, "bestLabel")->text(),
                 QStringLiteral("Warning: Could not save your score: disk full"));
    }
};

QTEST_MAIN(GameWindowTest)
#include "GameWindowTest.moc"
