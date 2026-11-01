#pragma once
#include "../domain/Difficulty.h"
#include "../domain/GameEvent.h"

#include <stdexcept>

// 入力ストリームが閉じられた（EOF: Ctrl+Z / Ctrl+D、パイプ終端など）ことを表す例外。
// UI はこれ以上入力を受け取れないため、ゲームを中断して main まで伝える。
class InputClosedError : public std::runtime_error {
public:
    InputClosedError() : std::runtime_error("input stream closed") {}
};

class IGameUI {
public:
    virtual ~IGameUI() = default;

    // 有効な難易度が選ばれるまで返らない。入力が閉じられたら InputClosedError を投げる。
    virtual Difficulty selectDifficulty() = 0;

    // diff の範囲内の値が入力されるまで返らない。入力が閉じられたら InputClosedError を投げる。
    virtual int inputGuess(const Difficulty& diff) = 0;

    virtual void handleEvent(const GameEvent& event) = 0;
};
