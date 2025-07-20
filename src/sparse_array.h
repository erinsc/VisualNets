#pragma once
#include <vector>
#include <cstddef>

/* Sparse Array
 * A structure-less container with O(1) insert, delete, and Random-Access
 * Inserted elements get a unique index consistent throughout its lifetime
 *
 * Requirements for T:
 *   Element has two "modes".
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
 * */

template <typename T>
class SparseArray {
    size_t size_;
    size_t stack_index_;
    std::vector<T> data_;
public:
    SparseArray(size_t capacity) : size_(0), stack_index_(0), data_(capacity) {
        for (size_t i = 0; i < capacity; ++i) {
            data_[i].set_index(i+1);
        }
    }
    size_t size()                 const { return size_; }
    size_t capacity()             const { return data_.size(); }
    bool   is_taken(size_t index) const { return !data_[index].is_index(); }

    T&       operator[](size_t index)       { return data_[index]; }
    const T& operator[](size_t index) const { return data_[index]; }

    size_t push(const T &value) {
        ++size_;
        size_t old_index = stack_index_;
        size_t new_index = data_[old_index].get_index();
        stack_index_ = new_index;
        data_[old_index] = value;
        return old_index;
    }
    template <typename... Args>
    size_t emplace(Args&&... args) {
        ++size_;
        size_t old_index = stack_index_;
        size_t new_index = data_[old_index].get_index();
        stack_index_ = new_index;
        data_[old_index] = T(std::forward<Args>(args)...);
        return old_index;
    }
    T pop(size_t index) {
        --size_;
        T value = data_[index];
        data_[index].set_index(stack_index_);
        stack_index_ = index;
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
        std::pair<size_t, const T&> operator*() const {
            return {index_, data_[index_]};
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
        std::pair<size_t, T&> operator*() {
            return {index_, data_[index_]};
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
        return const_iterator(0, data_);
    }
    const_iterator end() const {
        return const_iterator(data_.size(), data_);
    }
    iterator begin() {
        return iterator(0, data_);
    }
    iterator end() {
        return iterator(data_.size(), data_);
    }
};
