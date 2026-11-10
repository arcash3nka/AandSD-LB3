#pragma once
#include <stdexcept>
#include <utility>
using namespace std;

template <typename T>
class DynamicArray {
private:
    T* data;
    int count_;
    int capacity_;

    void ensureCapacity() {
        int newCapacity = (capacity_ == 0) ? 4 : capacity_ * 2;
        T* newData = new T[newCapacity];

        for (int i = 0 ; i < count_; i++) {
            newData[i] = data[i];
        }

        delete[] data;
        data = newData;
        capacity_ = newCapacity;
    }

public:
    DynamicArray() {
        capacity_ = 4;
        count_ = 0;
        data = new T[capacity_];
    }


    // Правило пяти: в классе сырой указатель, поэтому копирование нужно писать самому
    // иначе два массива будут владеть одной памятью и удалят её дважды
    // 1. Деструктор
    ~DynamicArray() {
        delete[] data;
    }

    // 2. Глубокое копирование
    DynamicArray(const DynamicArray& other) {
        capacity_ = other.capacity_;
        count_ = other.count_;
        data = new T[capacity_];

        for (int i = 0; i < count_; i++) {
            data[i] = other.data[i];
        }
    }

    // 3. Оператор присваивания копирования
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            DynamicArray temp(other); // копия во временный объект
            *this = std::move(temp); // перекладываем дату
        }

        return *this;
    }

    // 4. Перемещение
    // noexcept: гарантия отсутствия исключений
    DynamicArray(DynamicArray&& other) noexcept {
        data = other.data;
        count_ = other.count_;
        capacity_ = other.capacity_;

        other.data = nullptr;
        other.count_ = 0;
        other.capacity_ = 0;
    }

    // 5. Присваивание перемещения
    // noexcept: гарантия отсутствия исключений
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] data;

            data = other.data;
            count_ = other.count_;
            capacity_ = other.capacity_;

            other.data = nullptr;
            other.count_ = 0;
            other.capacity_ = 0;
        }

        return *this;
    }

    // Технические функции
    int size() const { return count_; }
    bool isEmpty() const { return count_ == 0; }
    void clear() { count_ = 0; }

    // Получить элемент с проверкой выхода за границы
    T get(int index) const {
        if (index < 0 || index >= count_) {
            throw std::runtime_error("DynamicArray/get -> Index out of range");
        }
        return data[index];
    }

    // Добавление в конец: амортизированное O(1)
    void pushBack(T value) {
        if (count_ == capacity_) {
            ensureCapacity();
        }
        data[count_] = value;
        count_++;
    }

    // Удаление с конца: O(1)
    void popBack() {
        if (count_ == 0) {
            throw std::runtime_error("DynamicArray/popBack -> Array is empty");
        }
        count_--;
    }

    // Вставка по произвольному индексу: O(n)
    void insertAt(int index, T value) {
        if (index < 0 || index > count_) {
            throw std::runtime_error("DynamicArray/insertAt -> Index out of range");
        }
        if (count_ == capacity_) {
            ensureCapacity();
        }
        // сдвиг элементов вправо, начиная с конца
        for (int i = count_; i > index; i--) {
            data[i] = data[i - 1];
        }
        data[index] = value;
        count_++;
    }

    // Удаление по индексу: O(n)
    void removeAt(int index) {
        if (index < 0 || index >= count_) {
            throw std::runtime_error("DynamicArray/removeAt -> Index out of range");
        }
        // сдвиг элементов влево, затирая удаляемый
        for (int i = index; i < count_ - 1; i++) {
            data[i] = data[i + 1];
        }
        count_--;
    }
};
