#pragma once
#include <vector>
#include <cstddef>

#include <iostream>

/* Sparse Vector
 * A structure-less container with O(1) insert, delete, and Random-Access
 * Inserted elements get a unique index consistent throughout its lifetime
 *
 * Requirements for T:
 *   T instance has two "modes".
 *     In normal mode its accessible as a normal element
 *     In index mode it stores a size_t value, used by the container
 *         for extra internal functionality. The index does not need to be
 *         stored if not in index mode.
 *
 *   Required member functions:
 *     bool is_index() const - returns whether instance is in index mode
 *     size_t get_index() const - if instance is in index mode, returns index
 *     T& set_index(size_t)  - sets instance to index mode and returns it
 *     T::make_index(size_t) - Returns an instance of T in index mode
 */

template <typename T>
class SparseVector {
    size_t size_m;
    size_t stack_index;
    std::vector<T> data;
public:
    SparseVector() : size_m(0), stack_index(0), data(0) {}
    explicit SparseVector(size_t capacity) : size_m(0), stack_index(0), data(0) {
        for (size_t i = 0; i < capacity; ++i) {
            data.push_back(T::make_index(i+1));
        }
    }
    size_t size()                 const { return size_m; }
    size_t capacity()             const { return data.size(); }
    bool   is_taken(size_t index) const { return !data[index].is_index(); }

    T&       operator[](size_t index)       { return data[index]; }
    const T& operator[](size_t index) const { return data[index]; }
    T&       at(size_t index)       { return data[index]; }
    const T& at(size_t index) const { return data[index]; }

    size_t push(const T &value) {
        if (stack_index == data.size()) {
            size_t to_add = size_m * 3 / 2 + 1;
            for (size_t i = size_m; i < to_add; ++i)
                data.push_back(T::make_index(i+1));
        }
        ++size_m;
        size_t old_index = stack_index;
        size_t new_index = data[old_index].get_index();
        stack_index = new_index;
        data[old_index] = value;

        return old_index;
    }
    template <typename... Args>
    size_t emplace(Args&&... args) {
        if (stack_index == data.size()) {
            size_t to_add = size_m * 3 / 2 + 1;
            for (size_t i = size_m; i < to_add; ++i)
                data.push_back(T::make_index(i+1));
        }
        ++size_m;
        size_t old_index = stack_index;
        size_t new_index = data[old_index].get_index();
        stack_index = new_index;
        data[old_index] = T(std::forward<Args>(args)...);
        return old_index;
    }
    T pop(size_t index) {
        --size_m;
        T value = data[index];
        data[index].set_index(stack_index);
        stack_index = index;
        return value;
    }

    class const_iterator {
        size_t index_;
        const std::vector<T> &data_;

        void skip_nulls() {
            while (index_ < data_.size() and data_[index_].is_index())
                ++index_;
        }
    public:
        const_iterator(size_t start, const std::vector<T> &data)
            : index_(start), data_(data) {
            skip_nulls();
        }
        size_t operator*() const {
            return index_;
        }
        const_iterator& operator++() {
            ++index_;
            skip_nulls();
            return *this;
        }
        bool operator!=(const const_iterator &other) const {
            return index_ != other.index_;
        }
    };

    class iterator {
        size_t index_;
        std::vector<T> &data_;

        void skip_nulls() {
            while (index_ < data_.size() and data_[index_].is_index())
                ++index_;
        }
    public:
        iterator(size_t start, std::vector<T> &data)
            : index_(start), data_(data) {
            skip_nulls();
        }
        size_t operator*() {
            return index_;
        }
        iterator& operator++() {
            ++index_;
            skip_nulls();
            return *this;
        }
        bool operator!=(const iterator &other) const {
            return index_ != other.index_;
        }
    };
    const_iterator begin() const {
        return const_iterator(0, data);
    }
    const_iterator end() const {
        return const_iterator(data.size(), data);
    }
    iterator begin() {
        return iterator(0, data);
    }
    iterator end() {
        return iterator(data.size(), data);
    }
};
