#include "tree/BinaryTree.h"
#include <algorithm>
#include <utility>

BinaryTree::BinaryTree(std::unique_ptr<Node> root) : root_(std::move(root)) {}

// работа с узлом, но не со всем деревом
int BinaryTree::countNodes(const Node* node) {
    if (node == nullptr) {
        return 0;
    }

    return 1 + countNodes(node->left.get()) + countNodes(node->right.get());
}

int BinaryTree::calcHeight(const Node* node) {
    if (node == nullptr) {
        return 0;
    }

    return 1 + std::max(calcHeight(node->left.get()), calcHeight(node->right.get()));
}

void BinaryTree::collectPreOrder(const Node* node, DynamicArray<int>& result) {
    if (node == nullptr) {
        return;
    }

    result.pushBack(node->value); // сначала сам узел
    collectPreOrder(node->left.get(), result);
    collectPreOrder(node->right.get(), result);
}

void BinaryTree::collectInOrder(const Node* node, DynamicArray<int>& result) {
    if (node == nullptr) {
        return;
    }

    collectInOrder(node->left.get(), result);
    result.pushBack(node->value); // узел между поддеревьями
    collectInOrder(node->right.get(), result);
}

// левое -> правое -> корень
void BinaryTree::collectPostOrder(const Node* node, DynamicArray<int>& result) {
    if (node == nullptr) {
        return;
    }

    collectPostOrder(node->left.get(), result);
    collectPostOrder(node->right.get(), result);
    result.pushBack(node->value); // узел после обоих поддеревьев
}

std::string BinaryTree::toBracket(const Node* node) {
    if (node == nullptr) {
        return "()";
    }

    std::string text = "(" + std::to_string(node->value);

    // если есть хоть один ребёнок, левого пишем всегда (пустой - как "()"),
    // иначе правого сына парсер примет за левого
    if (node->left != nullptr || node->right != nullptr) {
        text += " " + toBracket(node->left.get());
    }
    if (node->right != nullptr) {
        text += " " + toBracket(node->right.get());
    }

    return text + ")";
}

// публичные методы
bool BinaryTree::empty() const {return root_ == nullptr;}
int BinaryTree::size() const {return countNodes(root_.get());}
int BinaryTree::height() const {return calcHeight(root_.get());}

DynamicArray<int> BinaryTree::preOrder() const {
    DynamicArray<int> result;
    collectPreOrder(root_.get(), result);

    return result;
}

DynamicArray<int> BinaryTree::inOrder() const {
    DynamicArray<int> result;
    collectInOrder(root_.get(), result);

    return result;
}

DynamicArray<int> BinaryTree::postOrder() const {
    DynamicArray<int> result;
    collectPostOrder(root_.get(), result);

    return result;
}

std::string BinaryTree::toBracketString() const {
    return toBracket(root_.get());
}
