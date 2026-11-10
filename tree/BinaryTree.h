#pragma once
#include <memory>
#include <string>
#include "structures/DynamicArray.h"

class BinaryTree {
public:
    struct Node {
        int value;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Node(int nodeValue) : value(nodeValue) {}
    };

private:
    std::unique_ptr<Node> root_; // корень

    // работа с узлом, но не со всем деревом
    // static: поля деревьев не изменяют
    static int countNodes(const Node* node);
    static int calcHeight(const Node* node);
    static void collectPreOrder(const Node* node, DynamicArray<int>& result);
    static void collectInOrder(const Node* node, DynamicArray<int>& result);
    static void collectPostOrder(const Node* node, DynamicArray<int>& result);
    static std::string toBracket(const Node* node);

public:
    BinaryTree() = default; // пустое дерево

    // правило пяти из-за unique_ptr
    BinaryTree(const BinaryTree&) = delete;
    BinaryTree& operator=(const BinaryTree&) = delete;
    BinaryTree(BinaryTree&&) noexcept = default;
    BinaryTree& operator=(BinaryTree&&) noexcept = default;
    ~BinaryTree() = default;

    bool empty() const;
    int size() const;
    int height() const;

    // функции обходов
    DynamicArray<int> preOrder() const; // прямой обход
    DynamicArray<int> inOrder() const; // симметричный обход
    DynamicArray<int> postOrder() const; // обратный обход

    // обратно в скобочную запись
    std::string toBracketString() const;
};
