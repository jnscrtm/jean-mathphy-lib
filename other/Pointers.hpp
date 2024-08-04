#pragma once

#include <cstddef>
#include <utility>

template <class T>
class SharedPtr {
public:
    constexpr SharedPtr() : PtrObj(nullptr), PtrCounter(nullptr) {}
    constexpr SharedPtr(T* ptr) : PtrObj(ptr) {
        PtrCounter = new size_t(0);
    }

    constexpr SharedPtr(const SharedPtr& other) noexcept : PtrObj(other.PtrObj), PtrCounter(other.PtrCounter) {
        if(PtrCounter != nullptr)
            (*PtrCounter)++;
    }

    constexpr SharedPtr(SharedPtr&& other) noexcept : PtrObj(std::exchange(other.PtrObj, nullptr)), PtrCounter(std::exchange(other.PtrCounter, nullptr)) {}

    constexpr bool IsNull() noexcept { 
        return PtrCounter == nullptr; 
    }

    constexpr SharedPtr<T>& operator=(const SharedPtr<T>& other) noexcept {
        PtrObj = other.PtrObj; 
        PtrCounter = other.PtrCounter;
        if(PtrCounter != nullptr)
            (*PtrCounter)++;

        return *this;
    }

    constexpr SharedPtr<T>& operator=(SharedPtr<T>&& other) noexcept {
        PtrObj = std::exchange(other.PtrObj, nullptr); 
        PtrCounter = std::exchange(other.PtrCounter, nullptr);

        return *this;
    }

    constexpr T& operator*() {
        return *PtrObj;
    }

    constexpr const T& operator*() const {
        return *PtrObj;
    }

    constexpr T* operator->() {
        return PtrObj;
    }

    constexpr const T* operator->() const {
        return PtrObj;
    }

    constexpr ~SharedPtr() { 
        if(PtrCounter) { 
            if(*PtrCounter == 0) { delete PtrObj; delete PtrCounter; } 
            else (*PtrCounter)--; 
        } 
    }

    T* PtrObj;
    size_t* PtrCounter;
};