#include "GameWindow.h"

#include "app/Messages.h"
#include "domain/InputParser.h"
#include "domain/PlayerName.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include <string>
#include <type_traits>
#include <variant>

namespace {

// 予想値の入力欄の最大文字数（不必要に長い入力を受け付けない）
constexpr int kMaxGuessLength = 10;

QString describe(const GameEvent& event) {
    return std::visit(
        [](auto&& e) -> QString {
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, TooSmall>) {
                return QStringLiteral("Too small");
            } else if constexpr (std::is_same_v<T, TooLarge>) {
                return QStringLiteral("Too large");
            } else {
                return QStringLiteral("Correct! Attempts: %1").arg(e.attempts);
            }
        },
        event);
}

} // namespace

GameWindow::GameWindow(GameSession& session, const QString& scoreFileText, QWidget* parent)
    : QWidget(parent), session_(session),
      difficulties_{Difficulty::Easy(), Difficulty::Normal(), Difficulty::Hard()} {

    setWindowTitle(QStringLiteral("Number Guess Game"));

    // ---- 設定（名前・難易度・開始） ----
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setObjectName(QStringLiteral("nameEdit"));
    nameEdit_->setMaxLength(static_cast<int>(kMaxPlayerNameLength));
    nameEdit_->setPlaceholderText(QStringLiteral("Letters, digits, space, _ or -"));

    difficultyBox_ = new QComboBox(this);
    difficultyBox_->setObjectName(QStringLiteral("difficultyBox"));
    for (const auto& d : difficulties_) {
        difficultyBox_->addItem(
            QStringLiteral("%1 (%2-%3)").arg(QString::fromStdString(d.name)).arg(d.min).arg(d.max));
    }

    startButton_ = new QPushButton(QStringLiteral("Start"), this);
    startButton_->setObjectName(QStringLiteral("startButton"));

    auto* settings = new QFormLayout;
    settings->addRow(QStringLiteral("Name"), nameEdit_);
    settings->addRow(QStringLiteral("Difficulty"), difficultyBox_);
    settings->addRow(QString(), startButton_);

    // ---- プレイ（予想の入力） ----
    guessEdit_ = new QLineEdit(this);
    guessEdit_->setObjectName(QStringLiteral("guessEdit"));
    guessEdit_->setMaxLength(kMaxGuessLength);

    guessButton_ = new QPushButton(QStringLiteral("Guess"), this);
    guessButton_->setObjectName(QStringLiteral("guessButton"));

    auto* guessRow = new QHBoxLayout;
    guessRow->addWidget(guessEdit_);
    guessRow->addWidget(guessButton_);

    // ---- 結果表示 ----
    messageLabel_ = new QLabel(QStringLiteral("Enter your name and press Start."), this);
    messageLabel_->setObjectName(QStringLiteral("messageLabel"));
    messageLabel_->setWordWrap(true);

    historyList_ = new QListWidget(this);
    historyList_->setObjectName(QStringLiteral("historyList"));

    bestLabel_ = new QLabel(this);
    bestLabel_->setObjectName(QStringLiteral("bestLabel"));
    bestLabel_->setWordWrap(true);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(settings);
    layout->addLayout(guessRow);
    layout->addWidget(messageLabel_);
    layout->addWidget(historyList_);
    layout->addWidget(bestLabel_);
    if (!scoreFileText.isEmpty()) {
        auto* fileLabel = new QLabel(QStringLiteral("Scores: %1").arg(scoreFileText), this);
        fileLabel->setWordWrap(true);
        fileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        layout->addWidget(fileLabel);
    }

    // ---- イベント ----
    connect(startButton_, &QPushButton::clicked, this, &GameWindow::startGame);
    connect(nameEdit_, &QLineEdit::returnPressed, this, &GameWindow::startGame);
    connect(guessButton_, &QPushButton::clicked, this, &GameWindow::submitGuess);
    connect(guessEdit_, &QLineEdit::returnPressed, this, &GameWindow::submitGuess);

    setPlaying(false);
    resize(380, 460);
}

void GameWindow::startGame() {
    const NameResult name = parsePlayerName(nameEdit_->text().toStdString());
    if (const auto* error = std::get_if<NameError>(&name)) {
        setMessage(QString::fromUtf8(toMessage(*error)), true);
        nameEdit_->setFocus();
        return;
    }

    const int index = difficultyBox_->currentIndex();
    const Difficulty& diff = difficulties_.at(static_cast<std::size_t>(index));
    session_.start(std::get<std::string>(name), diff);

    historyList_->clear();
    bestLabel_->clear();
    setPlaying(true);
    setMessage(QStringLiteral("Guess a number between %1 and %2.").arg(diff.min).arg(diff.max));
    guessEdit_->setFocus();
}

void GameWindow::submitGuess() {
    if (!session_.isPlaying()) {
        return;
    }

    const ParseResult r = parseGuess(guessEdit_->text().toStdString(), session_.difficulty());
    if (const auto* error = std::get_if<InputError>(&r)) {
        setMessage(QString::fromUtf8(toMessage(*error)), true);
        guessEdit_->selectAll();
        return;
    }

    const int value = std::get<int>(r);
    const GuessOutcome outcome = session_.submitGuess(value);
    guessEdit_->clear();
    showOutcome(value, outcome);
}

void GameWindow::showOutcome(int value, const GuessOutcome& outcome) {
    const QString text = describe(outcome.event);
    historyList_->addItem(
        QStringLiteral("#%1  %2  →  %3").arg(session_.attempts()).arg(value).arg(text));
    historyList_->scrollToBottom();
    setMessage(text);

    if (outcome.report) {
        setPlaying(false);
        showReport(*outcome.report);
        startButton_->setFocus();
    }
}

void GameWindow::showReport(const ScoreReport& report) {
    QStringList lines;
    if (report.best) {
        const auto& best = *report.best;
        lines << QStringLiteral("%1Best for %2: %3 attempts by %4")
                     .arg(report.isNewRecord ? QStringLiteral("New record! ") : QString())
                     .arg(QString::fromStdString(best.difficulty))
                     .arg(best.attempts)
                     .arg(QString::fromStdString(best.player));
    }
    for (const auto& warning : report.warnings) {
        lines << QStringLiteral("Warning: %1").arg(QString::fromStdString(warning));
    }
    bestLabel_->setText(lines.join(QLatin1Char('\n')));
}

void GameWindow::setPlaying(bool playing) {
    guessEdit_->setEnabled(playing);
    guessButton_->setEnabled(playing);
    startButton_->setText(playing ? QStringLiteral("Restart") : QStringLiteral("Start"));
}

void GameWindow::setMessage(const QString& text, bool isError) {
    messageLabel_->setText(text);
    messageLabel_->setStyleSheet(isError ? QStringLiteral("color: #c62828;") : QString());
}
