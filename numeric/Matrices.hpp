#pragma once

#include <array>
#include <immintrin.h>
#include "Misc.hpp"

template <class T, size_t N>
class NVector {
public:
    constexpr NVector() noexcept : _Elems({}) {}
    constexpr NVector(const std::array<T, N>& elems) : _Elems(elems) {}
    constexpr NVector(std::array<T, N>&& elems) : _Elems(std::move(elems)) {}

    constexpr NVector(std::initializer_list<T> lst) {
        if(lst.size() <= N)
            std::copy(lst.begin(), lst.end(), _Elems.begin());
        else
            throw std::logic_error("Attempted to assign more elements than designated.");
    }

    constexpr const T& operator[](size_t i) const { return _Elems[i]; }
    constexpr T& operator[](size_t i) { return _Elems[i]; }
private:
    std::array<T, N> _Elems;
};

template <class T, size_t N>
constexpr NVector<T, N> operator+(const NVector<T, N>& a, const NVector<T, N>& b) {
    NVector<T, N> res;
    FOLD(Ns, N, ((res[Ns] = a[Ns] + b[Ns]), ...));
    return res;
}

template <class T, size_t N>
constexpr NVector<T, N>& operator+=(NVector<T, N>& a, const NVector<T, N>& b) {
    FOLD(Ns, N, ((a[Ns] += b[Ns]), ...));
    return a;
}

template <class T, size_t N>
constexpr NVector<T, N> operator-(const NVector<T, N>& a, const NVector<T, N>& b) {
    NVector<T, N> res;
    FOLD(Ns, N, ((res[Ns] = a[Ns] - b[Ns]), ...));
    return res;
}

template <class T, size_t N>
constexpr NVector<T, N>& operator-=(NVector<T, N>& a, const NVector<T, N>& b) {
    FOLD(Ns, N, ((a[Ns] -= b[Ns]), ...));
    return a;
}

template <class T, size_t N>
constexpr NVector<T, N> operator*(const NVector<T, N>& a, const T& b) {
    NVector<T, N> res;
    FOLD(Ns, N, ((res[Ns] = a[Ns] * b), ...));
    return res;
}

template <class T, size_t N>
constexpr NVector<T, N> operator*(const T& a, const NVector<T, N>& b) {
    return b * a;
}

template <class T, size_t N>
constexpr NVector<T, N>& operator*=(NVector<T, N>& a, const T& b) {
    FOLD(Ns, N, ((a[Ns] *= b), ...));
    return a;
}

template <class T, size_t N>
constexpr NVector<T, N> operator/(const NVector<T, N>& a, const T& b) {
    NVector<T, N> res;
    FOLD(Ns, N, ((res[Ns] = a[Ns] / b), ...));
    return res;
}

template <class T, size_t N>
constexpr NVector<T, N>& operator/=(NVector<T, N>& a, const T& b) {
    FOLD(Ns, N, ((a[Ns] /= b), ...));
    return a;
}


////////////////////////////////// MATRIX ACTUAL DEF LOL /////////////////////////////////////

template <class T, size_t N, size_t M>
class Matrix {
public:
    constexpr Matrix() noexcept : _Elems({}) {}
    //constexpr Matrix(const std::array<std::array<T, M>, N>& elems) : _Elems(std::bit_cast<std::array<NVector<T, M>, N>>(elems)) {}

    constexpr Matrix(std::initializer_list<NVector<T, M>> lst) {
        if(lst.size() <= N)
            std::copy(lst.begin(), lst.end(), _Elems.begin());
        else
            throw std::logic_error("Attempted to assign more elements than designated.");
    }

    constexpr const NVector<T, M>& operator[](size_t i) const { return _Elems[i]; }
    constexpr NVector<T, M>& operator[](size_t i) { return _Elems[i]; }
protected:
    std::array<NVector<T, M>, N> _Elems;
};

template <class T, size_t N, size_t M, size_t P>
Matrix<T, N, P> MatrixMul(const Matrix<T, N, M>& A, const Matrix<T, M, P>& B) {
    Matrix<T, N, P> C;
    for(size_t i = 0; i < N; i++)
        C[i] = FOLD(Ns, N, (A[i][Ns] * B[Ns]) + ...);

    return C;
}

template <class T, size_t N>
constexpr T Det(const Matrix<T, N, N>& M) {
    Matrix<T, N, N> cp_M = M;
    std::array<NVector<T, N>*, N> pRows = FOLD(Ns, N, (std::array<NVector<T, N>*, N> { &cp_M[Ns]... }));
    T sgn = T(1.0);

    for(size_t j = 0; j < N - 1; j++) {
        for(size_t i = j; i < N; i++) {
            if((*pRows[i])[j] != T(0.0)) {
                std::swap(pRows[i], pRows[j]), sgn *= T(-1.0);
                break;
            }
        }

        if((*pRows[j])[j] == T(0.0))
            return T(0.0);

        for(size_t i = j + 1; i < N; i++)
            *pRows[i] -= (*pRows[j]) * ((*pRows[i])[j])/((*pRows[j])[j]);
    }

    return sgn * FOLD(Ns, N, (*pRows[Ns])[Ns] * ...);
}

template <class T, size_t N>
constexpr T Tr(const Matrix<T, N, N>& M) {
    return FOLD(Ns, N, M[Ns][Ns] + ...);
}

// Fold over (P[i][j] * Q[i][j]) + ...