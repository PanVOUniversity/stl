/**
 * @file skip_list.cpp
 * @brief Примеры использования STL-контейнера "Список с пропусками"
 * @author Pan Vladimir
 * @version 1.0
 * @date 2025
 */

#include "../include/skip_list.hpp"
#include <iostream>
#include <string>
#include <chrono>

/**
 * @brief Демонстрация основных операций со списком с пропусками
 */
void demonstrate_basic_operations() {
    std::cout << "=== Демонстрация основных операций ===" << std::endl;
    
    stl::skip_list<int> sl;
    
    // Вставка элементов
    sl.insert(10);
    sl.insert(20);
    sl.insert(5);
    sl.insert(15);
    sl.insert(25);
    
    std::cout << "Размер списка: " << sl.size() << std::endl;
    std::cout << "Элементы: ";
    for (const auto& elem : sl) {
        std::cout << elem << " ";
    }
    std::cout << std::endl;
    
    // Поиск элементов
    auto it = sl.find(15);
    if (it != sl.end()) {
        std::cout << "Найден элемент: " << *it << std::endl;
    }
    
    // Поиск границ
    auto lb = sl.lower_bound(12);
    auto ub = sl.upper_bound(12);
    std::cout << "lower_bound(12): " << (lb != sl.end() ? std::to_string(*lb) : "end") << std::endl;
    std::cout << "upper_bound(12): " << (ub != sl.end() ? std::to_string(*ub) : "end") << std::endl;
}

/**
 * @brief Демонстрация работы с пользовательскими типами
 */
void demonstrate_custom_types() {
    std::cout << "\n=== Демонстрация пользовательских типов ===" << std::endl;
    
    stl::skip_list<std::string> string_list;
    
    string_list.insert("яблоко");
    string_list.insert("банан");
    string_list.insert("апельсин");
    string_list.insert("груша");
    
    std::cout << "Строки в алфавитном порядке: ";
    for (const auto& str : string_list) {
        std::cout << str << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief Демонстрация производительности
 */
void demonstrate_performance() {
    std::cout << "\n=== Демонстрация производительности ===" << std::endl;
    
    stl::skip_list<int> sl;
    const int num_elements = 10000;
    
    // Измерение времени вставки
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_elements; ++i) {
        sl.insert(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto insert_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время вставки " << num_elements << " элементов: " 
              << insert_time.count() << " мкс" << std::endl;
    
    // Измерение времени поиска
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_elements; i += 100) {
        sl.find(i);
    }
    end = std::chrono::high_resolution_clock::now();
    
    auto search_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Время поиска " << num_elements / 100 << " элементов: " 
              << search_time.count() << " мкс" << std::endl;
}

/**
 * @brief Демонстрация итераторов
 */
void demonstrate_iterators() {
    std::cout << "\n=== Демонстрация итераторов ===" << std::endl;
    
    stl::skip_list<int> sl = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    std::cout << "Прямой обход: ";
    for (auto it = sl.begin(); it != sl.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // Примечание: обратные итераторы не поддерживаются в skip list
    // из-за сложности эффективной реализации оператора декремента
    std::cout << "Обратный обход (через vector): ";
    std::vector<int> elements;
    for (const auto& elem : sl) {
        elements.push_back(elem);
    }
    for (auto it = elements.rbegin(); it != elements.rend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
    
    // Демонстрация const итераторов
    const stl::skip_list<int>& const_sl = sl;
    std::cout << "Const итератор: ";
    for (auto it = const_sl.cbegin(); it != const_sl.cend(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << std::endl;
}

/**
 * @brief Демонстрация обработки ошибок
 */
void demonstrate_error_handling() {
    std::cout << "\n=== Демонстрация обработки ошибок ===" << std::endl;
    
    stl::skip_list<int> sl;
    
    try {
        // Попытка разыменования end() итератора
        auto it = sl.end();
        std::cout << "Значение end() итератора: " << *it << std::endl;
    } catch (const std::runtime_error& e) {
        std::cout << "Поймана ошибка: " << e.what() << std::endl;
    }
    
    // Вставка элементов и безопасный поиск
    sl.insert(42);
    auto it = sl.find(42);
    if (it != sl.end()) {
        std::cout << "Безопасно найдено: " << *it << std::endl;
    }
    
    auto not_found = sl.find(999);
    if (not_found == sl.end()) {
        std::cout << "Элемент 999 не найден" << std::endl;
    }
}

/**
 * @brief Демонстрация концептов C++20
 */
void demonstrate_concepts() {
    std::cout << "\n=== Демонстрация концептов C++20 ===" << std::endl;
    
    // Проверка концептов для различных типов
    static_assert(std::totally_ordered<int>, "int должен быть totally_ordered");
    static_assert(std::copyable<int>, "int должен быть copyable");
    static_assert(std::movable<int>, "int должен быть movable");
    
    stl::skip_list<int> int_list;
    stl::skip_list<std::string> string_list;
    
    std::cout << "Концепты проверены успешно!" << std::endl;
}

/**
 * @brief Главная функция
 */
int main() {
    std::cout << "STL Skip List Container Demo" << std::endl;
    std::cout << "=============================" << std::endl;
    
    try {
        demonstrate_basic_operations();
        demonstrate_custom_types();
        demonstrate_performance();
        demonstrate_iterators();
        demonstrate_error_handling();
        demonstrate_concepts();
        
        std::cout << "\nВсе демонстрации завершены успешно!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
} 