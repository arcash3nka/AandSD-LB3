#pragma once
#include <cstddef> // std::size_t
#include <memory>
#include <string>
#include "tree/BinaryTree.h"

class BracketParser {
private:
    std::string text_; // разбираемая запись
    std::size_t pos_; // индекс текущего символа

    explicit BracketParser(const std::string& text);

    bool atEnd() const; // дошли до конца записи
    bool nextIs(char symbol) const; // текущий символ равен symbol (на конце записи - false)
    void skipSpaces(); // пропустить пробелы, табы и переводы строк
    void expect(char symbol); // текущий символ обязан быть symbol, иначе ошибка
    int parseNumber(); // прочитать целое число со знаком или бросить ошибку
    std::unique_ptr<BinaryTree::Node> parseTree(); // одна пара скобок, nullptr для "()"
    BinaryTree run(); // разбор всей записи

    // [[noreturn]]: функция всегда бросает исключение и никогда не возвращается
    [[noreturn]] void fail(const std::string& message) const;

public:
    // разбирает запись и возвращает дерево или бросает std::invalid_argument
    static BinaryTree parse(const std::string& text);
};
