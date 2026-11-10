#include "parser/BracketParser.h"
#include <cctype>    // std::isdigit, std::isspace
#include <climits>   // INT_MAX
#include <stdexcept>
#include <utility>   // std::move

BracketParser::BracketParser(const std::string& text) : text_(text), pos_(0) {}

// ================= Вспомогательные =================
bool BracketParser::atEnd() const {
    return pos_ >= text_.size();
}

bool BracketParser::nextIs(char symbol) const {
    return !atEnd() && text_[pos_] == symbol;
}

void BracketParser::skipSpaces() {
    // static_cast<unsigned char>: isspace/isdigit принимают только неотрицательные значения,
    // а русские буквы в char могут быть отрицательными
    while (!atEnd() && std::isspace(static_cast<unsigned char>(text_[pos_]))) {
        ++pos_;
    }
}

void BracketParser::fail(const std::string& message) const {
    // pos_ + 1: людям привычнее считать символы с 1
    throw std::invalid_argument("BracketParser->parse: " + message +
                                " (symbol " + std::to_string(pos_ + 1) + ")");
}

void BracketParser::expect(char symbol) {
    if (atEnd()) {
        fail(std::string("unexpected end of input, expected '") + symbol + "'");
    }
    if (text_[pos_] != symbol) {
        fail(std::string("expected '") + symbol + "', found '" + text_[pos_] + "'");
    }
    ++pos_; // символ подошёл - идём дальше
}

int BracketParser::parseNumber() {
    bool negative = false;
    if (nextIs('-')) {
        negative = true;
        ++pos_;
    }

    if (atEnd()) {
        fail("unexpected end of input, expected a number");
    }
    if (!std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
        fail(std::string("expected a number, found '") + text_[pos_] + "'");
    }

    // собираем число по цифре: 128 -> 1, потом 1*10+2 = 12, потом 12*10+8 = 128
    // long long, чтобы проверить переполнение int до того, как оно случится
    long long number = 0;
    while (!atEnd() && std::isdigit(static_cast<unsigned char>(text_[pos_]))) {
        number = number * 10 + (text_[pos_] - '0');
        if (number > INT_MAX) {
            fail("number is too big");
        }
        ++pos_;
    }

    return negative ? -static_cast<int>(number) : static_cast<int>(number);
}

// ================= Разбор =================
// одна пара скобок: "()" или "(число [левый [правый]])"
// рекурсия: вложенные деревья разбирает тот же parseTree
std::unique_ptr<BinaryTree::Node> BracketParser::parseTree() {
    expect('(');
    skipSpaces();

    // "()" - пустое поддерево
    if (nextIs(')')) {
        ++pos_;
        return nullptr;
    }

    std::unique_ptr<BinaryTree::Node> node = std::make_unique<BinaryTree::Node>(parseNumber());
    skipSpaces();

    // первое вложенное дерево - левый сын
    if (nextIs('(')) {
        node->left = parseTree();
        skipSpaces();
    }

    // второе - правый сын
    if (nextIs('(')) {
        node->right = parseTree();
        skipSpaces();
    }

    // третье уже лишнее: это не двоичное дерево
    if (nextIs('(')) {
        fail("node has more than two children, it is not a binary tree");
    }

    expect(')'); // после числа и детей обязана быть закрывающая скобка
    return node;
}

BinaryTree BracketParser::run() {
    skipSpaces();
    if (atEnd()) {
        fail("empty input");
    }

    std::unique_ptr<BinaryTree::Node> root = parseTree();

    // после дерева ничего, кроме пробелов, быть не должно
    skipSpaces();
    if (!atEnd()) {
        fail("extra symbols after the end of the tree");
    }

    return BinaryTree(std::move(root));
}

BinaryTree BracketParser::parse(const std::string& text) {
    BracketParser parser(text); // конструктор private, но из метода этого же класса можно
    return parser.run();
}
