#include <iostream>
#include <limits>    // std::numeric_limits — для очистки неправильного ввода
#include <stdexcept>
#include <string>

#include "reader/FileReader.h"
#include "structures/DynamicArray.h"
#include "parser/BracketParser.h"
#include "tree/AvlTree.h"
#include "tree/BinaryTree.h"

namespace {
    // constexpr: константа, есть тип, известна при компиляции
    // namespace { } без имени: видно только внутри этого файла
    constexpr const char* kDataDir = "data/"; // программа запускается из корня проекта

    constexpr const char* kExampleFile = "ok_example.txt"; // пример из ТЗ
    constexpr const char* kChainFile = "ok_chain.txt"; // цепочка: для обычного дерева худший случай
    constexpr const char* kMixedFile = "ok_mixed.txt"; // отрицательные числа и повтор значения

    // файлы с ошибками в записи + файл, которого нет
    constexpr const char* kErrorFiles[] = {
        "err_symbols.txt",
        "err_brackets.txt",
        "err_extra_bracket.txt",
        "err_not_binary.txt",
        "err_no_value.txt",
        "err_empty.txt",
        "no_such_file.txt",
    };
}

/*
По ТЗ необходимо реализовать деревья и продемонстрировать:
1. Чтение скобочной записи (путем чтения из файла)
2. Шаг 1 или создает двоичное дерево, или сообщает об ошибке
3. Обойти полученное дерево и создать АВЛ дерево
4. Вывод всех узлов 4 способами: в ширину и 3 в глубину

Сценарии:
1. Чтение записи из файла + двоичное дерево (рекурсивный обход в глубину)
2. Ошибки в записи
3. Двоичное дерево -> АВЛ дерево + 4 обхода (итеративные)
4. АВЛ дерево: вставка, поиск, удаление
5. Полная цепочка для своего файла (путь вводится с клавиатуры)

Каждый сценарий работает со своими деревьями, поэтому их можно запускать
в любом порядке и сколько угодно раз через меню
*/

// ===================== Вывод на экран =====================
void printTitle(const char* title) {
    std::cout << "\n========== " << title << " ==========\n";
}

// значения обхода в одну строку
void printValues(const std::string& title, const DynamicArray<int>& values) {
    std::cout << "  " << title << ":";
    for (int i = 0; i < values.size(); ++i) {
        std::cout << " " << values.get(i);
    }
    std::cout << "\n";
}

// убирает пробелы и переводы строк в конце, чтобы запись из файла красиво вывести в одну строку
std::string trimEnd(std::string text) {
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r' || text.back() == ' ')) {
        text.pop_back();
    }
    return text;
}

// обычное двоичное дерево: рекурсивный обход в глубину
void printBinaryTree(const BinaryTree& tree) {
    std::cout << "  запись: " << tree.toBracketString() << "\n";
    std::cout << "  узлов: " << tree.size() << ", высота: " << tree.height() << "\n";
    printValues("в глубину, прямой  (корень-левое-правое)", tree.preOrder());
    printValues("в глубину, симметричный (левое-корень-правое)", tree.inOrder());
    printValues("в глубину, обратный (левое-правое-корень)", tree.postOrder());
}

// АВЛ-дерево: все 4 обхода итеративные
void printAvlTree(const AvlTree& tree) {
    std::cout << "  запись: " << tree.toBracketString() << "\n";
    std::cout << "  узлов: " << tree.size() << ", высота: " << tree.height()
              << ", сбалансировано: " << std::boolalpha << tree.isBalanced() << "\n";
    printValues("в ширину", tree.breadthFirst());
    printValues("в глубину, прямой  (корень-левое-правое)", tree.preOrder());
    printValues("в глубину, симметричный (левое-корень-правое)", tree.inOrder());
    printValues("в глубину, обратный (левое-правое-корень)", tree.postOrder());
}

// ===================== Подготовка =====================
// обходит двоичное дерево (в глубину, рекурсия) и вставляет каждое значение в АВЛ
// повторы АВЛ не хранит, поэтому узлов в нём может быть меньше
AvlTree buildAvl(const BinaryTree& tree) {
    AvlTree avl;

    DynamicArray<int> values = tree.preOrder();
    for (int i = 0; i < values.size(); ++i) {
        avl.insert(values.get(i));
    }

    return avl;
}

// вся цепочка из ТЗ для одного файла: чтение -> разбор -> АВЛ -> вывод 4 способами
// любая ошибка выводится на экран, программа продолжает работать
void runPipeline(const std::string& path) {
    try {
        std::string text = readFile(path); // 1. чтение из файла
        std::cout << "Файл " << path << "\nЗапись: " << trimEnd(text) << "\n";

        BinaryTree tree = BracketParser::parse(text); // 2. дерево или исключение
        std::cout << "\nДвоичное дерево создано:\n";
        printBinaryTree(tree);

        AvlTree avl = buildAvl(tree); // 3. обход + АВЛ
        std::cout << "\nАВЛ дерево (из значений двоичного, повторов пропущено: "
                  << tree.size() - avl.size() << "):\n";
        printAvlTree(avl); // 4. вывод 4 способами
    } catch (const std::invalid_argument& e) {
        std::cout << "Ошибка в записи: " << e.what() << "\n";
    } catch (const std::runtime_error& e) {
        std::cout << "Ошибка файла: " << e.what() << "\n";
    }
}

// ===================== Сценарии =====================
// 1. Чтение записи из файла + двоичное дерево
void scenReadFile() {
    printTitle("1. Чтение записи из файла + двоичное дерево");

    std::string path = std::string(kDataDir) + kExampleFile;
    std::string text = readFile(path);
    std::cout << "Файл " << path << "\nЗапись: " << trimEnd(text) << "\n\n";

    BinaryTree tree = BracketParser::parse(text);
    std::cout << "Двоичное дерево создано (рекурсивный обход в глубину):\n";
    printBinaryTree(tree);
}

// 2. Ошибки в записи
void scenErrors() {
    printTitle("2. Ошибки в записи");

    for (const char* name : kErrorFiles) {
        std::string path = std::string(kDataDir) + name;
        std::cout << "\nФайл " << path << "\n";

        try {
            std::string text = readFile(path);
            std::cout << "  запись: \"" << trimEnd(text) << "\"\n";

            BinaryTree tree = BracketParser::parse(text);
            std::cout << "  дерево создано\n";
        } catch (const std::invalid_argument& e) {
            std::cout << "  ошибка в записи: " << e.what() << "\n";
        } catch (const std::runtime_error& e) {
            std::cout << "  ошибка файла: " << e.what() << "\n";
        }
    }
}

// 3. Двоичное дерево -> АВЛ + 4 обхода
void scenBuildAvl() {
    printTitle("3. Двоичное дерево -> АВЛ дерево");

    std::cout << "\n--- цепочка: у обычного дерева высота равна числу узлов ---\n";
    runPipeline(std::string(kDataDir) + kChainFile);

    std::cout << "\n--- отрицательные числа и повторяющееся значение 3 ---\n";
    runPipeline(std::string(kDataDir) + kMixedFile);
}

// 4. АВЛ: вставка, поиск, удаление
void scenAvlOperations() {
    printTitle("4. АВЛ дерево: вставка, поиск, удаление");

    AvlTree tree;
    std::cout << "Вставка 1..7 по возрастанию (в обычном дереве поиска получилась бы цепочка):\n";
    for (int value = 1; value <= 7; ++value) {
        tree.insert(value);
        std::cout << "  вставили " << value << " -> " << tree.toBracketString()
                  << "  (высота " << tree.height() << ")\n";
    }

    std::cout << "\nПоиск:\n";
    std::cout << "  contains(5)  = " << std::boolalpha << tree.contains(5) << "\n";
    std::cout << "  contains(10) = " << tree.contains(10) << "\n";

    std::cout << "\nПовторная вставка 4 -> insert вернул " << tree.insert(4)
              << ", узлов: " << tree.size() << "\n";

    std::cout << "\nУдаление:\n";
    // лист, узел с одним ребёнком, узел с одним ребёнком + поворот,
    // корень с двумя детьми + двойной поворот
    const int toErase[] = {1, 2, 3, 6};
    for (int value : toErase) {
        tree.erase(value);
        std::cout << "  удалили " << value << " -> " << tree.toBracketString()
                  << "  (высота " << tree.height()
                  << ", сбалансировано: " << tree.isBalanced() << ")\n";
    }
    std::cout << "  erase(100) вернул " << tree.erase(100) << " (такого значения нет)\n";

    std::cout << "\nИтог:\n";
    printAvlTree(tree);
}

// 5. Полная цепочка для своего файла
void scenCustomFile() {
    printTitle("5. Полная цепочка для своего файла");

    std::cout << "Путь к файлу со скобочной записью: ";
    std::string path;
    std::cin >> std::ws; // пропустить перевод строки, оставшийся после выбора пункта меню
    std::getline(std::cin, path);

    std::cout << "\n";
    runPipeline(path);
}

// ===================== Меню =====================
// очистка экрана: ANSI-команда терминалу
// 2J — очистить экран, H — курсор в начало
void clearScreen() {
    std::cout << "\033[2J\033[H";
}

void printMenu() {
    std::cout << "\n--------------------------------------------\n"
              << "1 - чтение записи из файла + двоичное дерево\n"
              << "2 - ошибки в записи\n"
              << "3 - двоичное дерево -> АВЛ + 4 обхода\n"
              << "4 - АВЛ: вставка, поиск, удаление\n"
              << "5 - полная цепочка для своего файла\n"
              << "0 - выход\n"
              << "Выберите сценарий и нажмите Enter: ";
}

// ===================== main =====================
int main() {
    clearScreen();

    // бесконечный цикл меню, выход только по 0
    while (true) {
        printMenu();

        int choice = 0;
        if (!(std::cin >> choice)) {
            // ввод закончился (Ctrl+D)
            if (std::cin.eof()) {
                return 0;
            }

            // ввели не число: сбрасываем ошибку потока и выкидываем строку
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            clearScreen();
            std::cout << "Нужно ввести число от 0 до 5\n";
            continue;
        }

        clearScreen();
        try {
            switch (choice) {
                case 1: scenReadFile(); break;
                case 2: scenErrors(); break;
                case 3: scenBuildAvl(); break;
                case 4: scenAvlOperations(); break;
                case 5: scenCustomFile(); break;
                case 0: return 0;
                default: std::cout << "Нет сценария " << choice << "\n"; break;
            }
        } catch (const std::exception& e) {
            // сценарий 1 читает файл без своего try: если файла нет, программа не падает
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }
}
