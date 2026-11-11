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

    // ДЛЯ РЕКУРСИИ
    // static: поля деревьев не изменяют
    static int countNodes(const Node* node); // кол-во узлов в поддереве с корнем node
    static int calcHeight(const Node* node); // подсчет высоты поддерева с корнем node
    static void collectPreOrder(const Node* node, DynamicArray<int>& result); // ПРЯМОЙ ОБХОД В ГЛУБИНУ: записывает в порядке корень->левое->правое
    static void collectInOrder(const Node* node, DynamicArray<int>& result); // СИММЕТРИЧНЫЙ ОБХОД В ГЛУБИНУ: записывает в порядке левое->корень->правое
    static void collectPostOrder(const Node* node, DynamicArray<int>& result); // ОБРАТНЫЙ ОБХОД В ГЛУБИНУ: записывает в порядке левое->правое->корень
    static std::string toBracket(const Node* node); // строит скобочную запись поддерева с корнем node

public:
    explicit BinaryTree(std::unique_ptr<Node> root);
    BinaryTree() = default; // пустое дерево

    // ПРАВИЛО ПЯТИ
    // из-за unique_ptr
    BinaryTree(const BinaryTree&) = delete;
    BinaryTree& operator=(const BinaryTree&) = delete;
    BinaryTree(BinaryTree&&) noexcept = default;
    BinaryTree& operator=(BinaryTree&&) noexcept = default;
    ~BinaryTree() = default;


    // ГЕТТЕРЫ
    bool empty() const;
    int size() const;
    int height() const;

    // ФУНКЦИИ ОБХОДА
    DynamicArray<int> preOrder() const; // ПРЯМОЙ ОБХОД В ГЛУБИНУ
    DynamicArray<int> inOrder() const; // СИММЕТРИЧНЫЙ ОБХОД В ГЛУБИНУ
    DynamicArray<int> postOrder() const; // ОБРЫТНЫЙ ОБХОД В ГЛУБИНУ

    std::string toBracketString() const; // строит скобочную запись всего дерева
};
