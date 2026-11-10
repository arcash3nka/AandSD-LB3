#pragma once
#include "structures/DoublyLinkedList.h"
using namespace std;

// вершина стека - tail
// добавление в вершину
// удаление с вершины
// top с вершины
template <typename T>
class Stack {
private:
    DoublyLinkedList<T> list;

public:
    int size() const { return list.size(); }
    bool isEmpty() const { return list.isEmpty(); }

    void push(T value) {
        list.pushBack(value);
    }

    void pop() {
        if (list.isEmpty()) {
            throw std::runtime_error("Stack/pop -> Stack is empty");
        }

        list.popBack();
    }

    T top() const {
        if (list.isEmpty()) {
            throw std::runtime_error("Stack/top -> Stack is empty");
        }

        return list.getLast();
    }
};
