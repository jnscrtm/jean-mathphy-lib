#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <stdexcept>

#define GL_FOLD(Ns, N, expr) []<size_t...Ns>(std::index_sequence<Ns...>){ return (expr); }(std::make_index_sequence<N>{})
#define FOLD(Ns, N, expr) [&]<size_t...Ns>(std::index_sequence<Ns...>){ return (expr); }(std::make_index_sequence<N>{})

#define NFOLD(Cap, Ns, N, expr) [&, _ = Cap]<size_t...Ns>(std::index_sequence<Ns...>){ return (expr); }(std::make_index_sequence<N>{})

#define CONTRACT(I, N, expr) [&]<size_t..._##I>(std::index_sequence<_##I...>){ \
    return (... + ([&]() { \
        constexpr size_t I = _##I; \
        return (expr); \
    }())); \
}(std::make_index_sequence<N>{})


template <class T, class V>
concept DecayedSameAs = std::same_as<std::decay_t<T>, std::decay_t<V>>;

template <class T>
concept Indexable = requires(T a) { a[0]; };

template <class T, class V, size_t N>
constexpr bool __has_tupled_indices_of() noexcept {
    return FOLD(Ns, N, requires(T a, V b) { b = (V) a[{Ns...}]; });
}

template <class T, class V, size_t N>
struct __s_has_split_indices_of {
    static constexpr bool __value() noexcept {
        return false;
    }
};

template <Indexable T, class V, size_t N>
struct __s_has_split_indices_of<T, V, N> {
    static constexpr bool __value() noexcept {
        return __s_has_split_indices_of<std::decay_t<decltype(std::declval<T>()[0])>, V, N-1>::__value();
    }
};

template <class T, class V>
struct __s_has_split_indices_of<T, V, 0> {
    static constexpr bool __value() noexcept {
        return DecayedSameAs<T, V>;
    }
};

template <class T, size_t N>
struct __s_is_split_multiindexable {
    static constexpr bool __value() noexcept {
        return false;
    }
};

template <Indexable T, size_t N>
struct __s_is_split_multiindexable<T, N> {
    static constexpr bool __value() noexcept {
        return __s_is_split_multiindexable<std::decay_t<decltype(std::declval<T>()[0])>, N-1>::__value();
    }
};

template <class T>
struct __s_is_split_multiindexable<T, 0> {
    static constexpr bool __value() noexcept {
        return true;
    }
};

template <class T, class V, size_t N>
concept TupledIndicesOf = __has_tupled_indices_of<T, V, N>();

template <class T, class V, size_t N>
concept SplitIndicesOf = __s_has_split_indices_of<T, V, N>::__value();

template <class T, size_t N>
concept TupledMultiIndexable = GL_FOLD(Ns, N, requires(T a) { a[{Ns...}]; });

template <class T, size_t N>
concept SplitMultiIndexable = __s_is_split_multiindexable<T, N>::__value();

template <class T, class V>
concept MatrixLikeOf = TupledIndicesOf<T, V, 2> || SplitIndicesOf<T, V, 2>;

template <class T>
using GetScalarType = double;

template <class T>
constexpr T Abs(T x) noexcept {
    return x >= 0 ? x : -x;
}

template <class T>
constexpr T Pow(T x, size_t n) noexcept {
    T res = 1;
    for(size_t i = 0; i < n; i++)
        res = res * x;

    return res;
}

template <class T>
constexpr T Sqrt(T a) noexcept {
    if(a == 0) return T(0);

    T multiplier = 1.0;

    while(a > 65536.0)
        a /= 65536.0, multiplier *= 256.0;
    while(a > 256.0)
        a /= 256.0, multiplier *= 16.0;
    while(a > 16.0)
        a /= 16.0, multiplier *= 4.0;
    while(a > 4.0)
        a /= 4.0, multiplier *= 2.0;

    T x = 1.0;
    T old_x = 1.0;
    do {
        old_x = x;
        x = T(0.5) * (x + a/x);
    } while(Abs(x - old_x) > 0.0);
    return x * multiplier;
}

template <class T>
constexpr T sum(const T& a, const T& b, bool accumulate_error = false, T* err_accumulator = nullptr) {
    if(accumulate_error && err_accumulator == nullptr)
        throw std::logic_error("Attempted to access data from null pointer.");
    
    T S = a + b;
    T _a_ = S - b;
    T _b_ = S - _a_;
    T da = a - _a_;
    T db = b - _b_;

    if(accumulate_error)
        *err_accumulator += da + db;

    return S;
}

template<class Lambda, int=(Lambda{}(), 0)>
constexpr bool is_constexpr(Lambda) { return true; }
constexpr bool is_constexpr(...) { return false; }

// A helper for index-based right-folding from 0 to N-1
template <class T>
struct ROIndexSplitter {
    constexpr ROIndexSplitter(const T& obj) noexcept : _Obj(obj) {}

    constexpr const T& operator[](const size_t i) const { return _Obj[i]; }
    constexpr auto operator%(const size_t i) const { return ROIndexSplitter<std::decay_t<decltype(_Obj[i])>>(_Obj[i]); }

    constexpr const T& operator()() const { return _Obj; }

    const T& _Obj;
};

template <size_t N, SplitMultiIndexable<N> T>
constexpr auto TupleToSplitIndices(const T& Obj, const std::array<size_t, N>& I) {
    return FOLD(Ns, N, (ROIndexSplitter(Obj) % ... % I[Ns]))();
}
