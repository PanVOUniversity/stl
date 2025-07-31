/**
 * @file skip_list.hpp
 * @brief Реализация STL-контейнера "Список с пропусками" (Skip List)
 * @author STL Container Implementation
 * @version 1.0
 * @date 2024
 */

#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <memory>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <concepts>
#include <iterator>
#include <algorithm>
#include <initializer_list>
#include <vector>

namespace stl {

constexpr size_t MAX_LEVEL = 32;
constexpr double P = 0.25;

template<typename T>
struct SkipListNode {
    T value;
    std::vector<std::shared_ptr<SkipListNode>> forward;
    size_t level;

    explicit SkipListNode(const T& val, size_t lvl = 0) 
        : value(val), level(lvl) {
        forward.resize(lvl + 1);
    }

    explicit SkipListNode(T&& val, size_t lvl = 0) 
        : value(std::move(val)), level(lvl) {
        forward.resize(lvl + 1);
    }
};

template<typename T, bool IsConst = false>
class SkipListIterator {
public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;

private:
    using NodePtr = std::conditional_t<IsConst, 
        std::shared_ptr<const SkipListNode<T>>, 
        std::shared_ptr<SkipListNode<T>>>;
    
    NodePtr current_;

public:
    SkipListIterator() = default;
    explicit SkipListIterator(NodePtr node) : current_(node) {}
    SkipListIterator(const SkipListIterator& other) = default;
    SkipListIterator& operator=(const SkipListIterator& other) = default;

    reference operator*() const {
        if (!current_) {
            throw std::runtime_error("Dereferencing null iterator");
        }
        return current_->value;
    }

    pointer operator->() const {
        if (!current_) {
            throw std::runtime_error("Accessing null iterator");
        }
        return &(current_->value);
    }

    SkipListIterator& operator++() {
        if (current_ && !current_->forward.empty()) {
            current_ = current_->forward[0];
        } else {
            current_ = nullptr;
        }
        return *this;
    }

    SkipListIterator operator++(int) {
        SkipListIterator temp = *this;
        ++(*this);
        return temp;
    }

    bool operator==(const SkipListIterator& other) const {
        return current_ == other.current_;
    }

    bool operator!=(const SkipListIterator& other) const {
        return !(*this == other);
    }

    NodePtr get_node() const { return current_; }
};

template<typename T, 
         typename Compare = std::less<T>,
         typename Allocator = std::allocator<T>>
class skip_list {
    static_assert(std::is_default_constructible_v<Compare>, 
                  "Compare must be default constructible");
    static_assert(std::is_default_constructible_v<Allocator>, 
                  "Allocator must be default constructible");

public:
    using value_type = T;
    using allocator_type = Allocator;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using value_compare = Compare;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = typename std::allocator_traits<Allocator>::pointer;
    using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
    using iterator = SkipListIterator<T, false>;
    using const_iterator = SkipListIterator<T, true>;

private:
    using Node = SkipListNode<T>;
    using NodePtr = std::shared_ptr<Node>;
    using ConstNodePtr = std::shared_ptr<const Node>;

    NodePtr head_;
    size_type size_;
    size_type max_level_;
    value_compare comp_;
    allocator_type alloc_;
    std::mt19937 gen_;
    std::uniform_real_distribution<double> dist_;

public:
    skip_list() : skip_list(Compare(), Allocator()) {}

    explicit skip_list(const Compare& comp, const Allocator& alloc = Allocator())
        : size_(0), max_level_(0), comp_(comp), alloc_(alloc), 
          gen_(std::random_device{}()), dist_(0.0, 1.0) {
        head_ = std::make_shared<Node>(T{}, MAX_LEVEL);
    }

    explicit skip_list(const Allocator& alloc)
        : skip_list(Compare(), alloc) {}

    skip_list(const skip_list& other)
        : skip_list(other.comp_, other.alloc_) {
        for (const auto& value : other) {
            insert(value);
        }
    }

    skip_list(skip_list&& other) noexcept
        : head_(std::move(other.head_)), size_(other.size_), 
          max_level_(other.max_level_), comp_(std::move(other.comp_)),
          alloc_(std::move(other.alloc_)), gen_(std::move(other.gen_)),
          dist_(std::move(other.dist_)) {
        other.size_ = 0;
        other.max_level_ = 0;
    }

    skip_list(std::initializer_list<value_type> init,
              const Compare& comp = Compare(),
              const Allocator& alloc = Allocator())
        : skip_list(comp, alloc) {
        for (const auto& value : init) {
            insert(value);
        }
    }

    ~skip_list() = default;

    skip_list& operator=(const skip_list& other) {
        if (this != &other) {
            skip_list temp(other);
            swap(temp);
        }
        return *this;
    }

    skip_list& operator=(skip_list&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = std::move(other.head_);
            size_ = other.size_;
            max_level_ = other.max_level_;
            comp_ = std::move(other.comp_);
            alloc_ = std::move(other.alloc_);
            gen_ = std::move(other.gen_);
            dist_ = std::move(other.dist_);
            other.size_ = 0;
            other.max_level_ = 0;
        }
        return *this;
    }

    allocator_type get_allocator() const noexcept {
        return alloc_;
    }

    // Итераторы
    iterator begin() noexcept {
        return iterator(head_->forward[0]);
    }

    const_iterator begin() const noexcept {
        return const_iterator(head_->forward[0]);
    }

    const_iterator cbegin() const noexcept {
        return const_iterator(head_->forward[0]);
    }

    iterator end() noexcept {
        return iterator(nullptr);
    }

    const_iterator end() const noexcept {
        return const_iterator(nullptr);
    }

    const_iterator cend() const noexcept {
        return const_iterator(nullptr);
    }

    // Емкость
    [[nodiscard]] bool empty() const noexcept {
        return size_ == 0;
    }

    size_type size() const noexcept {
        return size_;
    }

    size_type max_size() const noexcept {
        return std::allocator_traits<Allocator>::max_size(alloc_);
    }

    // Модификаторы
    void clear() noexcept {
        head_->forward.clear();
        head_->forward.resize(MAX_LEVEL);
        size_ = 0;
        max_level_ = 0;
    }

    std::pair<iterator, bool> insert(const value_type& value) {
        return insert_impl(value);
    }

    std::pair<iterator, bool> insert(value_type&& value) {
        return insert_impl(std::move(value));
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        return insert(value_type(std::forward<Args>(args)...));
    }

    void swap(skip_list& other) noexcept(
        std::allocator_traits<Allocator>::is_always_equal::value &&
        std::is_nothrow_swappable_v<Compare>) {
        std::swap(head_, other.head_);
        std::swap(size_, other.size_);
        std::swap(max_level_, other.max_level_);
        std::swap(comp_, other.comp_);
        std::swap(alloc_, other.alloc_);
        std::swap(gen_, other.gen_);
        std::swap(dist_, other.dist_);
    }

    // Поиск
    iterator find(const value_type& key) {
        return find_impl(key);
    }

    const_iterator find(const value_type& key) const {
        NodePtr current = head_;

        for (int i = max_level_; i >= 0; --i) {
            while (current->forward[i] && comp_(current->forward[i]->value, key)) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];

        if (current && !comp_(key, current->value) && !comp_(current->value, key)) {
            return const_iterator(current);
        }

        return const_iterator(nullptr);
    }

    size_type count(const value_type& key) const {
        return find(key) != end() ? 1 : 0;
    }

    iterator lower_bound(const value_type& key) {
        return lower_bound_impl(key);
    }

    const_iterator lower_bound(const value_type& key) const {
        NodePtr current = head_;

        for (int i = max_level_; i >= 0; --i) {
            while (current->forward[i] && comp_(current->forward[i]->value, key)) {
                current = current->forward[i];
            }
        }

        return const_iterator(current->forward[0]);
    }

    iterator upper_bound(const value_type& key) {
        return upper_bound_impl(key);
    }

    const_iterator upper_bound(const value_type& key) const {
        NodePtr current = head_;

        for (int i = max_level_; i >= 0; --i) {
            while (current->forward[i] && !comp_(key, current->forward[i]->value)) {
                current = current->forward[i];
            }
        }

        return const_iterator(current->forward[0]);
    }

    std::pair<iterator, iterator> equal_range(const value_type& key) {
        return {lower_bound(key), upper_bound(key)};
    }

    std::pair<const_iterator, const_iterator> equal_range(const value_type& key) const {
        return {lower_bound(key), upper_bound(key)};
    }

    // Наблюдатели
    value_compare value_comp() const {
        return comp_;
    }

private:
    size_type random_level() {
        size_type level = 0;
        while (dist_(gen_) < P && level < MAX_LEVEL - 1) {
            ++level;
        }
        return level;
    }

    template<typename U>
    std::pair<iterator, bool> insert_impl(U&& value) {
        std::vector<NodePtr> update(MAX_LEVEL, head_);
        NodePtr current = head_;

        for (int i = max_level_; i >= 0; --i) {
            while (current->forward[i] && comp_(current->forward[i]->value, value)) {
                current = current->forward[i];
            }
            update[i] = current;
        }

        current = current->forward[0];

        if (current && !comp_(value, current->value) && !comp_(current->value, value)) {
            return {iterator(current), false};
        }

        size_type new_level = random_level();
        if (new_level > max_level_) {
            for (size_type i = max_level_ + 1; i <= new_level; ++i) {
                update[i] = head_;
            }
            max_level_ = new_level;
        }

        auto new_node = std::make_shared<Node>(std::forward<U>(value), new_level);

        for (size_type i = 0; i <= new_level; ++i) {
            new_node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = new_node;
        }

        ++size_;
        return {iterator(new_node), true};
    }

    iterator find_impl(const value_type& key) const {
        NodePtr current = head_;

        for (int i = max_level_; i >= 0; --i) {
            while (current->forward[i] && comp_(current->forward[i]->value, key)) {
                current = current->forward[i];
            }
        }

        current = current->forward[0];

        if (current && !comp_(key, current->value) && !comp_(current->value, key)) {
            return iterator(current);
        }

        return iterator(nullptr);
    }

    iterator lower_bound_impl(const value_type& key) const {
        NodePtr current = head_;

        for (int i = max_level_; i >= 0; --i) {
            while (current->forward[i] && comp_(current->forward[i]->value, key)) {
                current = current->forward[i];
            }
        }

        return iterator(current->forward[0]);
    }

    iterator upper_bound_impl(const value_type& key) const {
        NodePtr current = head_;

        for (int i = max_level_; i >= 0; --i) {
            while (current->forward[i] && !comp_(key, current->forward[i]->value)) {
                current = current->forward[i];
            }
        }

        return iterator(current->forward[0]);
    }
};

// Операторы сравнения
template<typename T, typename Compare, typename Allocator>
bool operator==(const skip_list<T, Compare, Allocator>& lhs,
                const skip_list<T, Compare, Allocator>& rhs) {
    if (lhs.size() != rhs.size()) {
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename T, typename Compare, typename Allocator>
bool operator!=(const skip_list<T, Compare, Allocator>& lhs,
                const skip_list<T, Compare, Allocator>& rhs) {
    return !(lhs == rhs);
}

template<typename T, typename Compare, typename Allocator>
bool operator<(const skip_list<T, Compare, Allocator>& lhs,
               const skip_list<T, Compare, Allocator>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename T, typename Compare, typename Allocator>
bool operator<=(const skip_list<T, Compare, Allocator>& lhs,
                const skip_list<T, Compare, Allocator>& rhs) {
    return !(rhs < lhs);
}

template<typename T, typename Compare, typename Allocator>
bool operator>(const skip_list<T, Compare, Allocator>& lhs,
               const skip_list<T, Compare, Allocator>& rhs) {
    return rhs < lhs;
}

template<typename T, typename Compare, typename Allocator>
bool operator>=(const skip_list<T, Compare, Allocator>& lhs,
                const skip_list<T, Compare, Allocator>& rhs) {
    return !(lhs < rhs);
}

template<typename T, typename Compare, typename Allocator>
void swap(skip_list<T, Compare, Allocator>& lhs,
          skip_list<T, Compare, Allocator>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

} // namespace stl

#endif // SKIP_LIST_HPP 