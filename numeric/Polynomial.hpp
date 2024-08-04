#pragma once

#include <algorithm>
#include <array>
#include <utility>
#include <vector>

template <class T> class Polynomial;
template <class T> constexpr Polynomial<T>& __polynomial_add(Polynomial<T>&, const Polynomial<T>&);
template <class T> constexpr Polynomial<T>& __polynomial_add(Polynomial<T>&, const T&);
template <class T> constexpr Polynomial<T>& __polynomial_sub(Polynomial<T>&, const Polynomial<T>&);
template <class T> constexpr Polynomial<T>& __polynomial_sub(Polynomial<T>&, const T&);
template <class T> constexpr Polynomial<T>& __polynomial_mul(Polynomial<T>&, const Polynomial<T>&);
template <class T> constexpr Polynomial<T>& __polynomial_mul(Polynomial<T>&, const T&);
template <class T> constexpr std::pair<Polynomial<T>, Polynomial<T>> __polynomial_div(const Polynomial<T>&, const Polynomial<T>&);


/** 
 * @brief A template class for univariate polynomial over the given type.
 * @tparam T the type of coefficients 
 */
template <class T>
class Polynomial {
public:
    constexpr Polynomial() noexcept = default;
    constexpr Polynomial(const T& val) : Coefficients({ val }) {}
    constexpr Polynomial(std::initializer_list<T> lst) {
        Coefficients = std::vector<T>(lst);
        _normalize();
    }

    constexpr Polynomial<T> GetFormalDerivative() const {
        if(Coefficients.size() <= 1)
            return {T(0)};
        
        Polynomial<T> res;
        res.Coefficients = std::vector<T>(Coefficients.size() - 1);
        for(size_t i = 0; i < Coefficients.size() - 1; i++)
            res.Coefficients[i] = Coefficients[i+1] * (i+1);
        
        return res;
    }

    /// @brief Evaluate the polynomial by substituting the argument into the indeterminate.
    template <class V>
    constexpr auto operator()(const V& x) const -> decltype(std::declval<T>() * std::declval<V>()) {
        using ret_type = decltype(std::declval<T>() * std::declval<V>());

        if(x == V(0.0))
            return Coefficients[0] * V(1.0);

        V xpow = V(1.0);
        ret_type res = ret_type(0.0);
        for(auto& a : Coefficients) {
            res += a * xpow;
            xpow *= x;
        }

        return res;
    }

    constexpr size_t Degree() const {
        return Coefficients.size() > 0 ? Coefficients.size() - 1 : 0;
    }

    std::vector<T> Coefficients;

    friend constexpr Polynomial<T>& __polynomial_add<>(Polynomial<T>&, const Polynomial<T>&);
    friend constexpr Polynomial<T>& __polynomial_add<>(Polynomial<T>&, const T&);
    friend constexpr Polynomial<T>& __polynomial_sub<>(Polynomial<T>&, const Polynomial<T>&);
    friend constexpr Polynomial<T>& __polynomial_sub<>(Polynomial<T>&, const T&);
    friend constexpr Polynomial<T>& __polynomial_mul<>(Polynomial<T>&, const Polynomial<T>&);
    friend constexpr Polynomial<T>& __polynomial_mul<>(Polynomial<T>&, const T&);
    friend constexpr std::pair<Polynomial<T>, Polynomial<T>> __polynomial_div<>(const Polynomial<T>&, const Polynomial<T>&);

private:
    constexpr void _normalize() {
        Coefficients.erase(
            std::find_if_not(Coefficients.rbegin(), Coefficients.rend(), [](const T& a){ return a == 0; }).base(),
            Coefficients.end()
        );
    }
};


template <class T, size_t N, T..._args>
constexpr bool __pack_contains_trailing_zero() noexcept {
    std::array<T, N + 1> arr = { _args... };
    return std::find_if(arr.crbegin(), arr.crend(), [](const T& x) { return x == T(0); }) == arr.crbegin();
};

// ############################################### OPERATORS FOR Polynomial #########################################

template <class T>
constexpr bool operator==(const Polynomial<T>& a, const Polynomial<T>& b) {
    return a.Coefficients == b.Coefficients;
}

template <class T>
constexpr Polynomial<T> operator+(const Polynomial<T>& a) {
    return a;
}

template <class T>
constexpr Polynomial<T> operator-(const Polynomial<T>& a) {
    Polynomial<T> res = a;
    for(auto& x : res.Coefficients)
        x *= T(-1.0);
    return res;
}

template <class T>
constexpr Polynomial<T> operator+(const Polynomial<T>& a, const Polynomial<T>& b) {
    Polynomial<T> res = a;
    return __polynomial_add(res, b);
}

template <class T>
constexpr Polynomial<T>& operator+=(Polynomial<T>& a, const Polynomial<T>& b) {
    return __polynomial_add(a, b);
}

template <class T>
constexpr Polynomial<T> operator+(const Polynomial<T>& a, const T& b) {
    Polynomial<T> res = a;
    return __polynomial_add(a, b);
}

template <class T>
constexpr Polynomial<T>& operator+=(Polynomial<T>& a, const T& b) {
    return __polynomial_add(a, b);
}

template <class T>
constexpr Polynomial<T> operator-(const Polynomial<T>& a, const Polynomial<T>& b) {
    Polynomial<T> res = a;
    return __polynomial_sub(a, b);
}

template <class T>
constexpr Polynomial<T>& operator-=(Polynomial<T>& a, const Polynomial<T>& b) {
    return __polynomial_sub(a, b);
}

template <class T>
constexpr Polynomial<T> operator-(const Polynomial<T>& a, const T& b) {
    Polynomial<T> res = a;
    return __polynomial_sub(a, b);
}

template <class T>
constexpr Polynomial<T>& operator-=(Polynomial<T>& a, const T& b) {
    return __polynomial_sub(a, b);
}

template <class T>
constexpr Polynomial<T> operator*(const Polynomial<T>& a, const Polynomial<T>& b) {
    Polynomial<T> res = a;
    return __polynomial_mul(res, b);
}

template <class T>
constexpr Polynomial<T>& operator*=(Polynomial<T>& a, const Polynomial<T>& b) {
    return __polynomial_mul(a, b);
}

template <class T>
constexpr Polynomial<T> operator*(const Polynomial<T>& a, const T& b) {
    Polynomial<T> res = a;
    return __polynomial_mul(res, b);
}

template <class T>
constexpr Polynomial<T>& operator*=(Polynomial<T>& a, const T& b) {
    return __polynomial_mul(a, b);
}

template <class T>
constexpr Polynomial<T> operator/(const Polynomial<T>& a, const T& b) {
    Polynomial<T> res = a;
    for(auto& x : res.Coefficients)
        x /= b;
    return res;
}

template <class T>
constexpr Polynomial<T>& operator/=(Polynomial<T>& a, const T& b) {
    for(auto& x : a.Coefficients)
        x /= b;
    return a;
}

template <class T>
constexpr Polynomial<T> operator/(const Polynomial<T>& a, const Polynomial<T>& b) {
    return divide_and_remainder(a, b).first;
}

template <class T>
constexpr Polynomial<T>& operator/=(Polynomial<T>& a, const Polynomial<T>& b) {
    return a = a / b;
}

template <class T>
constexpr Polynomial<T> operator%(const Polynomial<T>& a, const Polynomial<T>& b) {
    return divide_and_remainder(a, b).second;
}

template <class T>
constexpr Polynomial<T>& operator%=(Polynomial<T>& a, const Polynomial<T>& b) {
    return a = a % b;
}

template <class T>
constexpr Polynomial<T> operator*(double c, const Polynomial<T>& p) {
    return p * c;
}

template <class T> 
constexpr Polynomial<T>& __polynomial_add(Polynomial<T>& a, const Polynomial<T>& b) {
const size_t n = std::max(a.Coefficients.size(), b.Coefficients.size());
    if(a.Coefficients.size() < n) 
        a.Coefficients.resize(n);
    
    for(size_t i = 0; i < n; i++)
        a.Coefficients[i] += b.Coefficients[i];
    
    return a;
}

template <class T>
constexpr Polynomial<T> __polynomial_add(const Polynomial<T>& a, const T& b) {
    Polynomial<T> res = a;
    if(a.Coefficients.size() > 0) 
        res.Coefficients[0] += b;
    else 
        res.Coefficients.push_back(b);

    res._normalize();
    return res;
}

template <class T> 
constexpr Polynomial<T>& __polynomial_sub(Polynomial<T>& a, const Polynomial<T>& b) {
    const size_t n = std::max(a.Coefficients.size(), b.Coefficients.size());

    if(a.Coefficients.size() < n) 
        a.Coefficients.resize(n);

    for(size_t i = 0; i < n; i++)
        a.Coefficients[i] -= b.Coefficients[i];

    a._normalize();
    return a;
}

template <class T> 
constexpr Polynomial<T>& __polynomial_mul(Polynomial<T>& a, const Polynomial<T>& b) {
    const size_t deg = a.Degree(), deg_p = b.Degree();
    a.Coefficients.resize(deg + deg_p + 1);

    for(size_t i = 0; i <= deg; i++) {
        T base = a.Coefficients[deg - i]; 
        a.Coefficients[deg - i] = T(0.0);
        for(size_t j = 0; j <= deg_p; j++) {
            a.Coefficients[deg - i + deg_p - j] += base * b.Coefficients[deg_p - j];
        }
    }

    a._normalize();
    return a;
}

template <class T>
constexpr Polynomial<T>& __polynomial_mul(Polynomial<T>& a, const T& b) {
    if(b == T(0)) 
        a.Coefficients.clear();
    else for(auto& x : a.Coefficients)
        x *= b;

    return a;
}

template <class T> 
constexpr std::pair<Polynomial<T>, Polynomial<T>> __polynomial_div(const Polynomial<T>& a, const Polynomial<T>& b) {
    if(a.Degree() < b.Degree())
        return std::make_pair(Polynomial<T>(), a);

    const size_t deg_a = a.Degree(), deg_b = b.Degree(), deg_q = a.Degree() - b.Degree();
    std::pair<Polynomial<T>, Polynomial<T>> res;
    auto&[q, r] = res;

    r = a;
    q.Coefficients.resize(deg_q + 1);

    for(size_t i = 0; i <= deg_q; i++) {
        if(r.Coefficients[deg_a - i] == 0)
            continue;

        T coef = r.Coefficients[deg_a - i] / b.Coefficients.back();
        q.Coefficients[deg_q - i] = coef;

        for(size_t j = 0; j <= deg_b; j++)
            r.Coefficients[deg_a - i - j] -= coef * b.Coefficients[deg_b - j]; 
    }

    q._normalize();
    r._normalize();
    return res;
}