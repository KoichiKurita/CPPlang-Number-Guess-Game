#pragma once

#include "app/GameSession.h"
#include "domain/Difficulty.h"

#include <QString>
#include <QWidget>

#include <vector>

class QComboBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;

// Qt 版のメイン画面。ボタンや Enter キーのイベントごとに GameSession を1手ずつ進める。
// 入力の検証は Console 版と同じ domain の関数（parsePlayerName / parseGuess）を使う。
class GameWindow : public QWidget {
    Q_OBJECT

public:
    explicit GameWindow(GameSession& session, const QString& scoreFileText = QString(),
                        QWidget* parent = nullptr);

private:
    void startGame();
    void submitGuess();
    void showOutcome(int value, const GuessOutcome& outcome);
    void showReport(const ScoreReport& report);
    void setPlaying(bool playing);
    void setMessage(const QString& text, bool isError = false);

    GameSession& session_;
    std::vector<Difficulty> difficulties_;

    QLineEdit* nameEdit_ = nullptr;
    QComboBox* difficultyBox_ = nullptr;
    QPushButton* startButton_ = nullptr;
    QLineEdit* guessEdit_ = nullptr;
    QPushButton* guessButton_ = nullptr;
    QLabel* messageLabel_ = nullptr;
    QListWidget* historyList_ = nullptr;
    QLabel* bestLabel_ = nullptr;
};
