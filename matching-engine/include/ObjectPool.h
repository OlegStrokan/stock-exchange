#pragma once

#include <vector>
#include <cstddef>
#include <stdexcept>
#include <new>

namespace engine {
    template <typename T>
    class ObjectPool {
    public:
        explicit ObjectPool(size_t capacity)
            : capacity_(capacity)
            , size_(0) {

            storage_ = static_cast<T*>(std::malloc(capacity * sizeof(T)));
            if (!storage_) {
                throw std::bad_alloc();
            }

            freeSlots_.reserve(capacity);
            for (size_t i = capacity; i > 0; i--) {
                freeSlots_.push_back(i - 1);
            }
        }

        ~ObjectPool() {
                // we don't call destructor here because we manage that in release(), just free the memory
                std::free(storage_);
        }

        // no coping, there is only one pool
        ObjectPool(const ObjectPool&) = delete;
        ObjectPool& operator = (const ObjectPool&) = delete;

        template <typename... Args>
        T* acquire(Args&&... args) {
            if (freeSlots_.empty()) {
                throw std::runtime_error("Object pool exhausted");
            }

            size_t index = freeSlots_.back();
            freeSlots_.pop_back();
            size_++;

            // construct object in the pre-allocated memory
            // this is "placement new" - it doesn't allocate, just constructs
            T* slot = &storage_[index];
            new (slot) T(std::forward<Args>(args)...);

            return slot;
        }

        void release(T* ptr) {
            if (!ptr) return;

            // calculate which slot index this pointer corresponds to
            size_t index = static_cast<size_t>(ptr - storage_);

            // call the destructor
            ptr->~T();

            // return the slot to the free list
            freeSlots_.push_back(index);
            size_--;
        }

        size_t size() const { return size_; };
        size_t capacity() const { return capacity_; };
        size_t available() const { return freeSlots_.size(); }

    private:
        T* storage_;
        std::vector<size_t> freeSlots_;
        size_t capacity_;
        size_t size_;
    };
}