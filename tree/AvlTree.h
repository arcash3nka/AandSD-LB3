#pragma once
#include <memory>
#include <string>
#include "structures/DynamicArray.h"

// АВЛ-дерево: дерево поиска, в котором у КАЖДОГО узла высоты левого и правого поддеревьев
// отличаются не больше чем на 1. Из-за этого высота всегда порядка log(n),
// и вставка, удаление и поиск работают за O(log n)
// значения не повторяются: вставка уже существующего значения ничего не меняет
class AvlTree {
private:
    struct Node {
        int value;
        int height;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;

        explicit Node(int nodeValue) : value(nodeValue), height(1) {}
    };

    std::unique_ptr<Node> root_;
    int size_;

    static int heightOf(const Node* node); // высота узда
    static int balanceOf(const Node* node); // высота левого / высота правого
    static void updateHeight(Node* node); // пересчет высоты по детям

    static std::unique_ptr<Node> roteteLeft(std::unique_ptr<Node> node);
    static std::unique_ptr<Node> roteteRight(std::unique_ptr<Node> node);
    static std::unique_ptr<Node> rebalance(std::unique_ptr<Node> node);

    static std::unique_ptr<Node> insertNode(std::unique_ptr<Node> node, int value);
    static std::unique_ptr<Node> eraseNode(std::unique_ptr<Node> node, int value);
    // удаление самого левого (минимального) узла поддерева, его значение кладется в minValue
    static std::unique_ptr<Node> removeMin(std::unique_ptr<Node> node, int& minValue);

    static int checkBalance(const Node* node); // для isBalanced
    static std::string toBracket(const Node* node);

public:
    AvlTree();

    // копирование запрещено (unique_ptr + глубокое копирование здесь не нужно)
    AvlTree(const AvlTree&) = delete;
    AvlTree& operator=(const AvlTree&) = delete;
    AvlTree(AvlTree&& other) noexcept;
    AvlTree& operator=(AvlTree&& other) noexcept;
    ~AvlTree() = default;

    bool insert(int value); // false - такое значение уже есть
    bool erase(int value); // false - такого значения нет
    bool contains(int value) const; // поиск
    void clear();

    int size() const;
    bool empty() const;
    int height() const;
    bool isBalanced() const; // проверка: у всех узлов баланс -1, 0 или 1 и height записана верно

    // обход ИТЕРАТИВНО
    DynamicArray<int> breadthFirst() const; // в ширину: по уровням сверху вниз (очередь)
    DynamicArray<int> preOrder() const; // в глубину: корень -> левое -> правое (стек)
    DynamicArray<int> inOrder() const; // в глубину: левое -> корень -> правое (стек)
    DynamicArray<int> postOrder() const; // в глубину: левое -> правое -> корень (стек)

    std::string toBracketString() const;
};
