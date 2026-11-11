#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "reader/FileReader.h"
#include "structures/DynamicArray.h"
#include "parser/BracketParser.h"
#include "tree/AvlTree.h"
#include "tree/BinaryTree.h"

namespace {
    constexpr const char* kDataDir = "data/";

    constexpr const char* kExampleFile = "ok_example.txt";
    constexpr const char* kChainFile = "ok_chain.txt";
    constexpr const char* kMixedFile = "ok_mixed.txt";

    constexpr const char* kErrorFiles[] = {
        "err_symbols.txt",
        "err_brackets.txt",
        "err_extra_bracket.txt",
        "err_not_binary.txt",
        "err_no_value.txt",
        "err_empty.txt",
        "no_such_file.txt",
    };

    const std::string kWide(60, '=');
    const std::string kNarrow(60, '-');
}

// ===================== ВЫВОД =====================
// заголовок сценария
void printTitle(const std::string& title) {
    std::cout << "\n" << kWide << "\n"
              << "  " << title << "\n"
              << kWide << "\n";
}

// подраздел внутри сценария
void printSection(const std::string& title) {
    std::cout << "\n  " << title << "\n"
              << "  " << kNarrow << "\n";
}

// строка вида "    метка: значение"
void printField(const std::string& label, const std::string& value) {
    std::cout << "    " << label << ": " << value << "\n";
}

// строка обхода: значения через пробел
void printTraversal(const std::string& label, const DynamicArray<int>& values) {
    std::cout << "      " << label << ":";
    for (int i = 0; i < values.size(); ++i) {
        std::cout << " " << values.get(i);
    }
    std::cout << "\n";
}

std::string yesNo(bool value) {
    return value ? "да" : "нет";
}

// убирает пробелы и переводы строк в конце записи
std::string trimEnd(std::string text) {
    while (!text.empty() && (text.back() == '\n' || text.back() == '\r' || text.back() == ' ')) {
        text.pop_back();
    }
    return text;
}

// двоичное дерево: рекурсивные обходы
void printBinaryTree(const BinaryTree& tree) {
    printField("запись", tree.toBracketString());
    printField("узлов", std::to_string(tree.size()));
    printField("высота", std::to_string(tree.height()));

    std::cout << "    обходы в глубину:\n";
    printTraversal("прямой      (корень-левое-правое)", tree.preOrder());
    printTraversal("симметричный (левое-корень-правое)", tree.inOrder());
    printTraversal("обратный    (левое-правое-корень)", tree.postOrder());
}

// АВЛ-дерево: все обходы итеративные
void printAvlTree(const AvlTree& tree) {
    printField("запись", tree.toBracketString());
    printField("узлов", std::to_string(tree.size()));
    printField("высота", std::to_string(tree.height()));
    printField("сбалансировано", yesNo(tree.isBalanced()));

    std::cout << "    обходы:\n";
    printTraversal("в ширину", tree.breadthFirst());
    printTraversal("прямой      (корень-левое-правое)", tree.preOrder());
    printTraversal("симметричный (левое-корень-правое)", tree.inOrder());
    printTraversal("обратный    (левое-правое-корень)", tree.postOrder());
}

// ===================== ПОДГОТОВКА =====================
// вставляет значения двоичного дерева (прямой обход) в АВЛ; повторы пропускаются
AvlTree buildAvl(const BinaryTree& tree) {
    AvlTree avl;

    DynamicArray<int> values = tree.preOrder();
    for (int i = 0; i < values.size(); ++i) {
        avl.insert(values.get(i));
    }

    return avl;
}

// полная цепочка: чтение -> разбор -> АВЛ -> вывод; ошибки печатаются, программа работает дальше
void runPipeline(const std::string& path) {
    try {
        std::string text = readFile(path);
        printField("файл", path);

        BinaryTree tree = BracketParser::parse(text);
        printSection("Двоичное дерево");
        printBinaryTree(tree);

        AvlTree avl = buildAvl(tree);
        printSection("АВЛ дерево (повторов пропущено: " + std::to_string(tree.size() - avl.size()) + ")");
        printAvlTree(avl);
    } catch (const std::invalid_argument& e) {
        std::cout << "    [ОШИБКА В ЗАПИСИ] " << e.what() << "\n";
    } catch (const std::runtime_error& e) {
        std::cout << "    [ОШИБКА ФАЙЛА] " << e.what() << "\n";
    }
}

// ===================== СЦЕНАРИИ =====================
// 1. Чтение записи из файла + двоичное дерево
void scenReadFile() {
    printTitle("1. Чтение записи из файла + двоичное дерево");

    std::string path = std::string(kDataDir) + kExampleFile;
    std::string text = readFile(path);
    printField("файл", path);

    BinaryTree tree = BracketParser::parse(text);
    printSection("Двоичное дерево (рекурсивный обход в глубину)");
    printBinaryTree(tree);
}

// 2. Ошибки в записи
void scenErrors() {
    printTitle("2. Ошибки в записи");

    for (const char* name : kErrorFiles) {
        std::string path = std::string(kDataDir) + name;
        printSection("Файл " + path);

        try {
            std::string text = readFile(path);
            printField("запись", "\"" + trimEnd(text) + "\"");

            BinaryTree tree = BracketParser::parse(text);
            std::cout << "    [OK] дерево создано\n";
        } catch (const std::invalid_argument& e) {
            std::cout << "    [ОШИБКА В ЗАПИСИ] " << e.what() << "\n";
        } catch (const std::runtime_error& e) {
            std::cout << "    [ОШИБКА ФАЙЛА] " << e.what() << "\n";
        }
    }
}

// 3. Двоичное дерево -> АВЛ + 4 обхода
void scenBuildAvl() {
    printTitle("3. Двоичное дерево -> АВЛ дерево");

    printSection("Цепочка: у обычного дерева высота равна числу узлов");
    runPipeline(std::string(kDataDir) + kChainFile);

    printSection("Отрицательные числа и повторяющееся значение 3");
    runPipeline(std::string(kDataDir) + kMixedFile);
}

// 4. АВЛ: вставка, поиск, удаление
void scenAvlOperations() {
    printTitle("4. АВЛ дерево: вставка, поиск, удаление");

    AvlTree tree;

    printSection("Вставка 1..7 по возрастанию (в обычном дереве поиска вышла бы цепочка)");
    for (int value = 1; value <= 7; ++value) {
        tree.insert(value);
        std::cout << "    вставили " << value << " -> " << tree.toBracketString()
                  << "  (высота " << tree.height() << ")\n";
    }

    printSection("Поиск");
    printField("contains(5)", yesNo(tree.contains(5)));
    printField("contains(10)", yesNo(tree.contains(10)));

    printSection("Повторная вставка 4");
    printField("insert(4) вернул", yesNo(tree.insert(4)));
    printField("узлов", std::to_string(tree.size()));

    printSection("Удаление: лист, один ребёнок, два ребёнка с поворотом");
    const int toErase[] = {1, 2, 3, 6};
    for (int value : toErase) {
        tree.erase(value);
        std::cout << "    удалили " << value << " -> " << tree.toBracketString()
                  << "  (высота " << tree.height()
                  << ", сбалансировано: " << yesNo(tree.isBalanced()) << ")\n";
    }
    printField("erase(100) вернул", yesNo(tree.erase(100)) + " (такого значения нет)");

    printSection("Итог");
    printAvlTree(tree);
}

// 5. Полная цепочка для своего файла
void scenCustomFile() {
    printTitle("5. Полная цепочка для своего файла");

    std::cout << "    Путь к файлу со скобочной записью: ";
    std::string path;
    std::cin >> std::ws; // пропускаем перевод строки после выбора пункта меню
    std::getline(std::cin, path);

    runPipeline(path);
}

// ===================== МЕНЮ =====================
void clearScreen() {
    std::cout << "\033[2J\033[H"; // ANSI: очистить экран, курсор в начало
}

void printMenu() {
    std::cout << "\n" << kNarrow << "\n"
              << "  1 - чтение записи из файла + двоичное дерево\n"
              << "  2 - ошибки в записи\n"
              << "  3 - двоичное дерево -> АВЛ + 4 обхода\n"
              << "  4 - АВЛ: вставка, поиск, удаление\n"
              << "  5 - полная цепочка для своего файла\n"
              << "  0 - выход\n"
              << kNarrow << "\n"
              << "Выберите сценарий и нажмите Enter: ";
}

// ===================== MAIN =====================
int main() {
    clearScreen();

    while (true) {
        printMenu();

        int choice = 0;
        if (!(std::cin >> choice)) {
            if (std::cin.eof()) {
                return 0; // Ctrl+D
            }

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
            std::cout << "Ошибка: " << e.what() << "\n";
        }
    }
}
