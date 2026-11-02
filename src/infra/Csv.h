#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

// RFC 4180 形式の CSV の書き出し・読み込み（I/O を持たない純粋関数）。
namespace csv {

// 1フィールドをエスケープする。
//  - 数式インジェクション対策: = + - @ タブ CR で始まる値の先頭に ' を付ける
//  - カンマ・ダブルクォート・改行を含む値はダブルクォートで囲み、" は "" にする
std::string escapeField(std::string_view field);

// フィールドの並びを1行の CSV にする（末尾の改行は含まない）
std::string formatLine(const std::vector<std::string>& fields);

// 1行の CSV をフィールドに分解する。escapeField で付けた先頭の ' は取り除く。
// 閉じていないダブルクォートなど、形式が壊れていれば std::nullopt を返す。
std::optional<std::vector<std::string>> parseLine(std::string_view line);

} // namespace csv
