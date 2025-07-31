/**
 * @file test_skip_list.cpp
 * @brief Тесты для STL-контейнера "Список с пропусками"
 * @author Pan Vladimir
 * @version 1.0
 * @date 2025
 */

#include <gtest/gtest.h>
#include "../include/skip_list.hpp"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

using namespace stl;

class SkipListTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Настройка перед каждым тестом
    }
    
    void TearDown() override {
        // Очистка после каждого теста
    }
};

// Тесты конструкторов
TEST_F(SkipListTest, DefaultConstructor) {
    skip_list<int> sl;
    EXPECT_TRUE(sl.empty());
    EXPECT_EQ(sl.size(), 0);
}

TEST_F(SkipListTest, CopyConstructor) {
    skip_list<int> original;
    original.insert(1);
    original.insert(2);
    original.insert(3);
    
    skip_list<int> copy(original);
    EXPECT_EQ(copy.size(), original.size());
    EXPECT_EQ(copy.size(), 3);
    
    auto it1 = original.begin();
    auto it2 = copy.begin();
    while (it1 != original.end() && it2 != copy.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST_F(SkipListTest, MoveConstructor) {
    skip_list<int> original;
    original.insert(1);
    original.insert(2);
    
    skip_list<int> moved(std::move(original));
    EXPECT_EQ(moved.size(), 2);
    EXPECT_TRUE(original.empty());
}

TEST_F(SkipListTest, InitializerListConstructor) {
    skip_list<int> sl = {3, 1, 4, 1, 5};
    EXPECT_EQ(sl.size(), 4); // Дубликаты не добавляются
    EXPECT_FALSE(sl.empty());
}

// Тесты операторов присваивания
TEST_F(SkipListTest, CopyAssignment) {
    skip_list<int> original = {1, 2, 3};
    skip_list<int> assigned;
    assigned = original;
    
    EXPECT_EQ(assigned.size(), original.size());
    auto it1 = original.begin();
    auto it2 = assigned.begin();
    while (it1 != original.end() && it2 != assigned.end()) {
        EXPECT_EQ(*it1, *it2);
        ++it1;
        ++it2;
    }
}

TEST_F(SkipListTest, MoveAssignment) {
    skip_list<int> original = {1, 2, 3};
    skip_list<int> assigned;
    assigned = std::move(original);
    
    EXPECT_EQ(assigned.size(), 3);
    EXPECT_TRUE(original.empty());
}

TEST_F(SkipListTest, InitializerListAssignment) {
    skip_list<int> sl;
    sl = {1, 2, 3, 4, 5};
    EXPECT_EQ(sl.size(), 5);
}

// Тесты итераторов
TEST_F(SkipListTest, IteratorTraversal) {
    skip_list<int> sl = {1, 2, 3, 4, 5};
    std::vector<int> expected = {1, 2, 3, 4, 5};
    std::vector<int> actual;
    
    for (const auto& elem : sl) {
        actual.push_back(elem);
    }
    
    EXPECT_EQ(actual, expected);
}

TEST_F(SkipListTest, ConstIterator) {
    const skip_list<int> sl = {1, 2, 3};
    std::vector<int> actual;
    
    for (auto it = sl.cbegin(); it != sl.cend(); ++it) {
        actual.push_back(*it);
    }
    
    EXPECT_EQ(actual.size(), 3);
    EXPECT_EQ(actual[0], 1);
    EXPECT_EQ(actual[1], 2);
    EXPECT_EQ(actual[2], 3);
}

TEST_F(SkipListTest, IteratorEquality) {
    skip_list<int> sl = {1, 2, 3};
    auto it1 = sl.begin();
    auto it2 = sl.begin();
    
    EXPECT_EQ(it1, it2);
    ++it1;
    EXPECT_NE(it1, it2);
}

// Тесты вставки
TEST_F(SkipListTest, InsertSingleElement) {
    skip_list<int> sl;
    auto result = sl.insert(42);
    
    EXPECT_TRUE(result.second);
    EXPECT_EQ(*result.first, 42);
    EXPECT_EQ(sl.size(), 1);
}

TEST_F(SkipListTest, InsertDuplicate) {
    skip_list<int> sl;
    sl.insert(42);
    auto result = sl.insert(42);
    
    EXPECT_FALSE(result.second);
    EXPECT_EQ(*result.first, 42);
    EXPECT_EQ(sl.size(), 1);
}

TEST_F(SkipListTest, InsertMultipleElements) {
    skip_list<int> sl;
    std::vector<int> values = {5, 2, 8, 1, 9, 3};
    
    for (const auto& val : values) {
        sl.insert(val);
    }
    
    EXPECT_EQ(sl.size(), 6);
    std::vector<int> actual;
    for (const auto& elem : sl) {
        actual.push_back(elem);
    }
    
    std::sort(values.begin(), values.end());
    EXPECT_EQ(actual, values);
}

TEST_F(SkipListTest, Emplace) {
    skip_list<std::string> sl;
    auto result = sl.emplace("test");
    
    EXPECT_TRUE(result.second);
    EXPECT_EQ(*result.first, "test");
    EXPECT_EQ(sl.size(), 1);
}

// Тесты поиска
TEST_F(SkipListTest, FindExisting) {
    skip_list<int> sl = {1, 2, 3, 4, 5};
    auto it = sl.find(3);
    
    EXPECT_NE(it, sl.end());
    EXPECT_EQ(*it, 3);
}

TEST_F(SkipListTest, FindNonExisting) {
    skip_list<int> sl = {1, 2, 3, 4, 5};
    auto it = sl.find(42);
    
    EXPECT_EQ(it, sl.end());
}

TEST_F(SkipListTest, Count) {
    skip_list<int> sl = {1, 2, 2, 3, 2, 4};
    EXPECT_EQ(sl.count(1), 1);
    EXPECT_EQ(sl.count(2), 1); // Дубликаты не добавляются
    EXPECT_EQ(sl.count(5), 0);
}

// Тесты границ
TEST_F(SkipListTest, LowerBound) {
    skip_list<int> sl = {1, 3, 5, 7, 9};
    
    auto lb1 = sl.lower_bound(2);
    EXPECT_NE(lb1, sl.end());
    EXPECT_EQ(*lb1, 3);
    
    auto lb2 = sl.lower_bound(5);
    EXPECT_NE(lb2, sl.end());
    EXPECT_EQ(*lb2, 5);
    
    auto lb3 = sl.lower_bound(10);
    EXPECT_EQ(lb3, sl.end());
}

TEST_F(SkipListTest, UpperBound) {
    skip_list<int> sl = {1, 3, 5, 7, 9};
    
    auto ub1 = sl.upper_bound(2);
    EXPECT_NE(ub1, sl.end());
    EXPECT_EQ(*ub1, 3);
    
    auto ub2 = sl.upper_bound(5);
    EXPECT_NE(ub2, sl.end());
    EXPECT_EQ(*ub2, 7);
    
    auto ub3 = sl.upper_bound(9);
    EXPECT_EQ(ub3, sl.end());
}

TEST_F(SkipListTest, EqualRange) {
    skip_list<int> sl = {1, 3, 5, 7, 9};
    
    auto range = sl.equal_range(5);
    EXPECT_EQ(*range.first, 5);
    EXPECT_EQ(*range.second, 7);
    
    auto empty_range = sl.equal_range(6);
    EXPECT_EQ(*empty_range.first, 7);
    EXPECT_EQ(*empty_range.second, 7);
}

// Тесты емкости
TEST_F(SkipListTest, Empty) {
    skip_list<int> sl;
    EXPECT_TRUE(sl.empty());
    
    sl.insert(1);
    EXPECT_FALSE(sl.empty());
}

TEST_F(SkipListTest, Size) {
    skip_list<int> sl;
    EXPECT_EQ(sl.size(), 0);
    
    sl.insert(1);
    EXPECT_EQ(sl.size(), 1);
    
    sl.insert(2);
    EXPECT_EQ(sl.size(), 2);
    
    sl.insert(1); // Дубликат
    EXPECT_EQ(sl.size(), 2);
}

TEST_F(SkipListTest, MaxSize) {
    skip_list<int> sl;
    EXPECT_GT(sl.max_size(), 0);
}

// Тесты модификаторов
TEST_F(SkipListTest, Clear) {
    skip_list<int> sl = {1, 2, 3, 4, 5};
    EXPECT_EQ(sl.size(), 5);
    
    sl.clear();
    EXPECT_TRUE(sl.empty());
    EXPECT_EQ(sl.size(), 0);
}

TEST_F(SkipListTest, Swap) {
    skip_list<int> sl1 = {1, 2, 3};
    skip_list<int> sl2 = {4, 5, 6};
    
    size_t size1 = sl1.size();
    size_t size2 = sl2.size();
    
    sl1.swap(sl2);
    
    EXPECT_EQ(sl1.size(), size2);
    EXPECT_EQ(sl2.size(), size1);
}

// Тесты операторов сравнения
TEST_F(SkipListTest, EqualityOperator) {
    skip_list<int> sl1 = {1, 2, 3};
    skip_list<int> sl2 = {1, 2, 3};
    skip_list<int> sl3 = {1, 2, 4};
    
    EXPECT_EQ(sl1, sl2);
    EXPECT_NE(sl1, sl3);
}

TEST_F(SkipListTest, LessThanOperator) {
    skip_list<int> sl1 = {1, 2, 3};
    skip_list<int> sl2 = {1, 2, 4};
    skip_list<int> sl3 = {1, 2, 3};
    
    EXPECT_LT(sl1, sl2);
    EXPECT_FALSE(sl1 < sl3);
}

// Тесты производительности
TEST_F(SkipListTest, PerformanceInsert) {
    skip_list<int> sl;
    const int num_elements = 1000;
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_elements; ++i) {
        sl.insert(i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    EXPECT_EQ(sl.size(), num_elements);
    EXPECT_LT(duration.count(), 1000000); // Должно быть меньше 1 секунды
}

TEST_F(SkipListTest, PerformanceSearch) {
    skip_list<int> sl;
    const int num_elements = 1000;
    
    for (int i = 0; i < num_elements; ++i) {
        sl.insert(i);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_elements; i += 10) {
        auto it = sl.find(i);
        EXPECT_NE(it, sl.end());
        EXPECT_EQ(*it, i);
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    EXPECT_LT(duration.count(), 1000000); // Должно быть меньше 1 секунды
}

// Тесты обработки ошибок
TEST_F(SkipListTest, IteratorDereferenceError) {
    skip_list<int> sl;
    auto it = sl.end();
    
    EXPECT_THROW(*it, std::runtime_error);
}

// Тесты с пользовательскими типами
struct TestStruct {
    int value;
    std::string name;
    
    TestStruct() : value(0), name("") {} // Конструктор по умолчанию
    TestStruct(int v, std::string n) : value(v), name(std::move(n)) {}
    
    bool operator<(const TestStruct& other) const {
        return value < other.value;
    }
    
    bool operator==(const TestStruct& other) const {
        return value == other.value && name == other.name;
    }
};

TEST_F(SkipListTest, CustomType) {
    skip_list<TestStruct> sl;
    
    sl.insert(TestStruct(1, "one"));
    sl.insert(TestStruct(2, "two"));
    sl.insert(TestStruct(0, "zero"));
    
    EXPECT_EQ(sl.size(), 3);
    
    auto it = sl.find(TestStruct(1, "one"));
    EXPECT_NE(it, sl.end());
    EXPECT_EQ(it->value, 1);
    EXPECT_EQ(it->name, "one");
}

// Тесты с компаратором
struct CustomCompare {
    bool operator()(int a, int b) const {
        return a > b; // Обратный порядок
    }
};

TEST_F(SkipListTest, CustomComparator) {
    skip_list<int, CustomCompare> sl;
    
    sl.insert(1);
    sl.insert(2);
    sl.insert(3);
    
    std::vector<int> actual;
    for (const auto& elem : sl) {
        actual.push_back(elem);
    }
    
    std::vector<int> expected = {3, 2, 1};
    EXPECT_EQ(actual, expected);
}

// Тесты концептов C++20
TEST_F(SkipListTest, Concepts) {
    static_assert(std::totally_ordered<int>, "int должен быть totally_ordered");
    static_assert(std::copyable<int>, "int должен быть copyable");
    static_assert(std::movable<int>, "int должен быть movable");
    
    skip_list<int> sl;
    static_assert(std::forward_iterator<decltype(sl.begin())>, 
                  "Итератор должен быть forward_iterator");
    
    EXPECT_TRUE(true); // Тест проходит, если компилируется
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
} 