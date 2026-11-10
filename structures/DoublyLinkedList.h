#pragma once
#include <stdexcept>
using namespace std;

// список двусвязный, чтобы popBack() работало за O(1)
template <typename T>
class DoublyLinkedList {
private:
    struct Node {
        T value;
        Node* prev;
        Node* next;

        Node(T nodeValue) : value(nodeValue), prev(nullptr), next(nullptr) {}
    };

    Node* head_;
    Node* tail_;
    int count_;

public:
    // запрет на копирование
    // потому что используется как внутренняя память стека и очереди
    DoublyLinkedList(const DoublyLinkedList&) = delete;
    DoublyLinkedList& operator=(const DoublyLinkedList&) = delete;

    DoublyLinkedList() {
        head_ = nullptr;
        tail_ = nullptr;
        count_ = 0;
    }

    ~DoublyLinkedList() {
        clear();
    }

    int size() const {return count_;}

    bool isEmpty() const {return count_ == 0;}

    void clear() {
        while (!isEmpty()) {
            popBack();
        }
    }

    T getFirst() const {
        if (isEmpty()) {
            throw std::runtime_error("DoublyLinkedList/getFirst -> List is empty");
        }

        return head_->value;
    }

    T getLast() const {
        if (isEmpty()) {
            throw std::runtime_error("DoublyLinkedList/getLast -> List is empty");
        }

        return tail_->value;
    }

    void pushBack(T value) {
        Node* node = new Node(value);

        if (isEmpty()) {
            head_ = node;
            tail_ = node;
        } else {
            node->prev = tail_;
            tail_->next = node;
            tail_ = node;
        }

        count_++;
    }

    void pushFront(T value) {
        Node* node = new Node(value);

        if (isEmpty()) {
            head_ = node;
            tail_ = node;
        } else {
            node->next = head_;
            head_->prev = node;
            head_ = node;
        }

        count_++;
    }

    void popBack() {
        if (isEmpty()) {
            throw std::runtime_error("DoublyLinkedList/popBack -> List is empty");
        }

        Node* node = tail_;
        tail_ = tail_->prev;

        if (tail_ == nullptr) {
            head_ = nullptr; // это был последний узел
        } else {
            tail_->next = nullptr;
        }

        delete node;
        count_--;
    }

    void popFront() {
        if (isEmpty()) {
            throw std::runtime_error("DoublyLinkedList/popFront -> List is empty");
        }

        Node* node = head_;
        head_ = head_->next;

        if (head_ == nullptr) {
            tail_ = nullptr; // это был последний узел
        } else {
            head_->prev = nullptr;
        }

        delete node;
        count_--;
    }
};
