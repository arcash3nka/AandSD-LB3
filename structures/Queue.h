#pragma once
#include "structures/DoublyLinkedList.h"
using namespace std;

template <typename T>
class Queue {
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
            throw std::runtime_error("Queue/pop -> Queue is empty");
        }

        list.popFront();
    }

    T front() const {
        if (list.isEmpty()) {
            throw std::runtime_error("Queue/front -> Queue is empty");
        }

        return list.getFirst();
    }
};
