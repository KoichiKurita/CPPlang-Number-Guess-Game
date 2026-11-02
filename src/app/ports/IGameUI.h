#pragma once
#include "domain/Difficulty.h"
#include "domain/GameEvent.h"
#include "domain/ScoreRecord.h"

#include <stdexcept>
#include <string>

// 入力ストリームが閉じられた（EOF: Ctrl+Z / Ctrl+D、パイプ終端など）ことを表す例外。
// UI はこれ以上入力を受け取れないため、ゲームを中断して main まで伝える。
class InputClosedError : public std::runtime_error {
public:
    InputClosedError() : std::runtime_error("input stream closed") {}
};

class IGameUI {
public:
    virtual ~IGameUI() = default;

    // 有効なプレイヤー名が入力されるまで返らない。入力が閉じられたら InputClosedError を投げる。
    virtual std::string inputPlayerName() = 0;

    // 有効な難易度が選ばれるまで返らない。入力が閉じられたら InputClosedError を投げる。
    virtual Difficulty selectDifficulty() = 0;

    // diff の範囲内の値が入力されるまで返らない。入力が閉じられたら InputClosedError を投げる。
    virtual int inputGuess(const Difficulty& diff) = 0;

    virtual void handleEvent(const GameEvent& event) = 0;

    // ゲーム終了後に、その難易度の最高記録を表示する
    virtual void showBestScore(const ScoreRecord& best, bool isNewRecord) = 0;

    // ゲームは続行できるが、利用者に知らせるべき問題（スコアの保存失敗など）
    virtual void showWarning(const std::string& message) = 0;
};
