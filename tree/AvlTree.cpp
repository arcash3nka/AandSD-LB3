#include "tree/AvlTree.h"
#include <algorithm>
#include <cstdlib>
#include <utility>
#include "structures/Queue.h"
#include "structures/Stack.h"

// ВСПОМОГАТЕЛЬНЫЕ
int AvlTree::heightOf(const Node* node) {
    return node == nullptr ? 0 : node->height;
}
int AvlTree::balanceOf(const Node* node) {
    return heightOf(node->left.get()) - heightOf(node->right.get());
}
void AvlTree::updateHeight(Node* node) {
    node->height = 1 + std::max(heightOf(node->left.get()), heightOf(node->right.get()));
}

// ПОВОРОТЫ И БАЛАНСЫ
std::unique_ptr<AvlTree::Node> AvlTree::rotateLeft(std::unique_ptr<Node> node) {
    std::unique_ptr<Node> newRoot = std::move(node->right);

    node->right = std::move(newRoot->left);
    updateHeight(node.get()); // сначала пересчитываем нижний узел (старый корень)

    newRoot->left = std::move(node);
    updateHeight(newRoot.get()); // потом верхний

    return newRoot;
}

// правый поворот: левый сын становится корнем поддерева
std::unique_ptr<AvlTree::Node> AvlTree::rotateRight(std::unique_ptr<Node> node) {
    std::unique_ptr<Node> newRoot = std::move(node->left);

    node->left = std::move(newRoot->right); // B переезжает к старому корню
    updateHeight(node.get());

    newRoot->right = std::move(node);
    updateHeight(newRoot.get());

    return newRoot;
}

// пересчитывает высоту узла и при необходимости чинит баланс
// вызывается на обратном ходе рекурсии после вставки и удаления
// 4 случая нарушения баланса (|баланс| = 2):
//   LL: слева тяжело и в левом сыне слева (или поровну) -> один правый поворот
//   LR: слева тяжело, но в левом сыне тяжелее справа    -> левый поворот сына, потом правый
//   RR и RL - зеркально
std::unique_ptr<AvlTree::Node> AvlTree::rebalance(std::unique_ptr<Node> node) {
    updateHeight(node.get());
    int balance = balanceOf(node.get());

    if (balance > 1) { // левое поддерево слишком высокое
        if (balanceOf(node->left.get()) < 0) { // случай LR
            node->left = rotateLeft(std::move(node->left));
        }
        return rotateRight(std::move(node));
    }

    if (balance < -1) { // правое поддерево слишком высокое
        if (balanceOf(node->right.get()) > 0) { // случай RL
            node->right = rotateRight(std::move(node->right));
        }
        return rotateLeft(std::move(node));
    }

    return node; // баланс в норме
}

// ДЛЯ РЕКУРСИИ
std::unique_ptr<AvlTree::Node> AvlTree::insertNode(std::unique_ptr<Node> node, int value) {
    if (node == nullptr) {
        return std::make_unique<Node>(value); // нашли свободное место
    }

    if (value < node->value) {
        node->left = insertNode(std::move(node->left), value);
    } else {
        node->right = insertNode(std::move(node->right), value);
    }

    return rebalance(std::move(node));
}

std::unique_ptr<AvlTree::Node> AvlTree::removeMin(std::unique_ptr<Node> node, int& minValue) {
    if (node->left == nullptr) {
        minValue = node->value;
        return std::move(node->right);
    }

    node->left = removeMin(std::move(node->left), minValue);
    return rebalance(std::move(node));
}

std::unique_ptr<AvlTree::Node> AvlTree::eraseNode(std::unique_ptr<Node> node, int value) {
    if (value < node->value) {
        node->left = eraseNode(std::move(node->left), value);
    } else if (value > node->value) {
        node->right = eraseNode(std::move(node->right), value);
    } else {
        if (node->left == nullptr) {
            return std::move(node->right);
        }
        if (node->right == nullptr) {
            return std::move(node->left);
        }

        int minValue = 0;
        node->right = removeMin(std::move(node->right), minValue);
        node->value = minValue;
    }

    return rebalance(std::move(node));
}

// возвращает высоту поддерева или -1, если в нём нарушен баланс или записана неверная height
int AvlTree::checkBalance(const Node* node) {
    if (node == nullptr) {
        return 0;
    }

    int leftHeight = checkBalance(node->left.get());
    int rightHeight = checkBalance(node->right.get());

    if (leftHeight == -1 || rightHeight == -1) {
        return -1; // проблема уже найдена ниже
    }
    if (std::abs(leftHeight - rightHeight) > 1) {
        return -1;
    }

    int height = 1 + std::max(leftHeight, rightHeight);
    if (height != node->height) {
        return -1;
    }

    return height;
}

std::string AvlTree::toBracket(const Node* node) {
    if (node == nullptr) {
        return "()";
    }

    std::string text = "(" + std::to_string(node->value);

    // та же запись, что у BinaryTree: правого сына без левого пишем как "() (...)"
    if (node->left != nullptr || node->right != nullptr) {
        text += " " + toBracket(node->left.get());
    }
    if (node->right != nullptr) {
        text += " " + toBracket(node->right.get());
    }

    return text + ")";
}

// ПРАВИЛО ПЯТИ
AvlTree::AvlTree() : root_(nullptr), size_(0) {}

AvlTree::AvlTree(AvlTree&& other) noexcept
    : root_(std::move(other.root_)), size_(other.size_) {
    other.size_ = 0; // root_ у источника после move уже nullptr
}

AvlTree& AvlTree::operator=(AvlTree&& other) noexcept {
    if (this != &other) {
        root_ = std::move(other.root_); // старое дерево *this удалится автоматически
        size_ = other.size_;
        other.size_ = 0;
    }
    return *this;
}

// ИЗМЕНЕНИЯ
bool AvlTree::insert(int value) {
    if (contains(value)) {
        return false; // повторы не храним
    }

    root_ = insertNode(std::move(root_), value);
    ++size_;

    return true;
}

bool AvlTree::erase(int value) {
    if (!contains(value)) {
        return false;
    }

    root_ = eraseNode(std::move(root_), value);
    --size_;

    return true;
}

// ГЕТТЕРЫ
// поиск без рекурсии: на каждом шаге отбрасываем половину дерева
bool AvlTree::contains(int value) const {
    const Node* current = root_.get();

    while (current != nullptr) {
        if (value == current->value) {
            return true;
        }
        current = (value < current->value) ? current->left.get() : current->right.get();
    }

    return false;
}

void AvlTree::clear() {
    root_.reset();
    size_ = 0;
}

int AvlTree::size() const {
    return size_;
}

bool AvlTree::empty() const {
    return size_ == 0;
}

int AvlTree::height() const {
    return heightOf(root_.get());
}

bool AvlTree::isBalanced() const {
    return checkBalance(root_.get()) != -1;
}

// ФУНКЦИИ ОБХОДА
DynamicArray<int> AvlTree::breadthFirst() const {
    DynamicArray<int> result;
    if (root_ == nullptr) {
        return result;
    }

    Queue<const Node*> queue;
    queue.push(root_.get());

    while (!queue.isEmpty()) {
        const Node* node = queue.front();
        queue.pop();

        result.pushBack(node->value);
        if (node->left != nullptr) {
            queue.push(node->left.get());
        }
        if (node->right != nullptr) {
            queue.push(node->right.get());
        }
    }

    return result;
}

DynamicArray<int> AvlTree::preOrder() const {
    DynamicArray<int> result;
    if (root_ == nullptr) {
        return result;
    }

    Stack<const Node*> stack;
    stack.push(root_.get());

    while (!stack.isEmpty()) {
        const Node* node = stack.top();
        stack.pop();

        result.pushBack(node->value);

        // правого кладём первым, чтобы левый вышел из стека раньше
        if (node->right != nullptr) {
            stack.push(node->right.get());
        }
        if (node->left != nullptr) {
            stack.push(node->left.get());
        }
    }

    return result;
}

DynamicArray<int> AvlTree::inOrder() const {
    DynamicArray<int> result;
    Stack<const Node*> stack;
    const Node* current = root_.get();

    while (current != nullptr || !stack.isEmpty()) {
        // уходим влево до конца, запоминая путь
        while (current != nullptr) {
            stack.push(current);
            current = current->left.get();
        }

        // слева больше нет: берём последний запомненный узел
        current = stack.top();
        stack.pop();
        result.pushBack(current->value);

        current = current->right.get(); // теперь его правое поддерево
    }

    return result;
}

DynamicArray<int> AvlTree::postOrder() const {
    DynamicArray<int> result;
    Stack<const Node*> stack;
    const Node* current = root_.get();
    const Node* lastVisited = nullptr; // последний записанный в результат узел

    while (current != nullptr || !stack.isEmpty()) {
        if (current != nullptr) {
            stack.push(current);
            current = current->left.get();
        } else {
            const Node* top = stack.top();

            if (top->right != nullptr && lastVisited != top->right.get()) {
                current = top->right.get(); // правое ещё не обошли
            } else {
                result.pushBack(top->value);
                lastVisited = top;
                stack.pop();
            }
        }
    }

    return result;
}

std::string AvlTree::toBracketString() const {
    return toBracket(root_.get());
}
