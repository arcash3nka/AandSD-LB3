#pragma once
#include <memory>
#include <string>
#include "structures/DynamicArray.h"

// AvlTree - дерево поиска
// слева - меньще, справа - больше
class AvlTree {
private:
    struct Node {
        int value;
        int height; // высота поддерева с корнем в узле, лист=1
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Node(int nodeValue) : value(nodeValue), height(1) {}
    };

    std::unique_ptr<Node> root_; // корень
    int size_; // количество узлов

    // ВСПОМОГАТЕЛЬНЫЕ
    static int heightOf(const Node* node); // высота узда, у nullptr = 0, у root_ = 1
    // высота левого / высота правого
    // считает баланс одного узла
    static int balanceOf(const Node* node);
    static void updateHeight(Node* node); // пересчет высоты по детям

    // ПОВОРОТЫ И БАЛАНСЫ
    // узел передается по значению
    // поддерево забирается, возвращается новый корень
    static std::unique_ptr<Node> rotateLeft(std::unique_ptr<Node> node);
    static std::unique_ptr<Node> rotateRight(std::unique_ptr<Node> node);
    // починка баланса узла
    // balance = H(L) - H(R)
    // каждый узел на пути проверяется, поэтому дерево остается сбалансированным на всем пути
    static std::unique_ptr<Node> rebalance(std::unique_ptr<Node> node);

    // ДЛЯ РЕКУРСИИ
    static std::unique_ptr<Node> insertNode(std::unique_ptr<Node> node, int value); // вставка
    static std::unique_ptr<Node> eraseNode(std::unique_ptr<Node> node, int value); // удаление
    static std::unique_ptr<Node> removeMin(std::unique_ptr<Node> node, int& minValue); // удаление минимума, его значение в minValue
    // для isBalanced: высота или -1 при ошибке
    // считает баланс всего поддерева
    static int checkBalance(const Node* node);
    static std::string toBracket(const Node* node); // скобочная запись дерева

public:
    AvlTree();

    // ПРАВИЛО ПЯТИ
    // из-за uniue_ptr
    AvlTree(const AvlTree&) = delete;
    AvlTree& operator=(const AvlTree&) = delete;
    // пишется самостоятельно, чтобы обнулить size_ e bcnjxybrf
    // при перемещении нужно обнулить size_ при root_=nullptr
    AvlTree(AvlTree&& other) noexcept;
    AvlTree& operator=(AvlTree&& other) noexcept;
    ~AvlTree() = default;

    // ИЗМЕНЕНИЯ
    bool insert(int value); // false - значение уже есть
    bool erase(int value); // false - значения нет
    void clear();

    // ГЕТТЕРЫ
    bool contains(int value) const; // поиск, O(logn)
    int size() const;
    bool empty() const;
    int height() const;
    bool isBalanced() const; // баланс и height у всех узлов в норме

    // ФУНКЦИИ ОБХОДА (итеративно: очередь или стек)
    // очередь: FIFO
    // стек: LIFO
    DynamicArray<int> breadthFirst() const; // ОБХОД В ШИРИНУ: по уровням сверху вниз (очередь)
    DynamicArray<int> preOrder() const; // ПРЯМОЙ ОБХОД В ГЛУБИНУ: корень -> левое -> правое (стек)
    DynamicArray<int> inOrder() const; // СИММЕТРИЧНЫЙ ОБХОД В ГЛУБИНУ: левое -> корень -> правое (стек)
    DynamicArray<int> postOrder() const; // ОБРАТНЫЙ ОБХОД В ГЛУБИНУ: левое -> правое -> корень (стек)

    std::string toBracketString() const;
};
