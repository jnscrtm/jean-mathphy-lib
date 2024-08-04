#pragma once

#include <concepts>
#include <cmath>
#include <functional>
#include <stdexcept>

template <std::convertible_to<double> T>
constexpr T left_lim(const std::function<T(T)>& f, T a);

template <std::convertible_to<double> T>
constexpr T right_lim(const std::function<T(T)>& f, T a);

// Evaluate lim_(x->a-) f(x)
template <std::convertible_to<double> T>
constexpr T left_lim(const std::function<T(T)>& f, T a) {
    // Transforming left-limit tending to +inf into right-limit tending to 0
    if(std::isinf((double) a)) {
        if(a < T(0.0))
            throw std::logic_error("Please use right-limit for negative infinity!");
        
        return right_lim<T>([=](T a_) {
            return f(T(1.0)/a_);
        }, T(0.0));
    }

    using Point = std::pair<T, T>;

    T h = T(1.0/64.0);
    std::vector<Point> points = std::vector<Point>(6);
    for(size_t i = 0; i < points.size(); i++) {
        points[i].first = a - h;
        points[i].second = f(points[i].first);
        h /= 8.0;
    }
    auto res = CreateLagrangeInterpolator(points)(a);
    return std::isnan((double) res) ? points.back().second : res;
}

// Evaluate lim_(x->a+) f(x)
template <std::convertible_to<double> T>
constexpr T right_lim(const std::function<T(T)>& f, T a) {
    // Transforming right-limit tending to -inf into left-limit tending to 0
    if(std::isinf((double) a)) {
        if(a > T(0.0))
            throw std::logic_error("Please use left-limit for positive infinity!");
        
        return left_lim<T>([=](T a_) {
            return f(-T(1.0)/a_);
        }, T(0.0));
    }

    using Point = std::pair<T, T>;

    T h = T(1.0/64.0);
    std::vector<Point> points = std::vector<Point>(6);
    for(size_t i = 0; i < points.size(); i++) {
        points[i].first = a + h;
        points[i].second = f(points[i].first);
        h /= 2.0;
    }
    auto res = CreateLagrangeInterpolator(points)(a);
    return std::isnan((double) res) ? points.back().second : res;
}

// Approximate a two-sided limit
template <std::convertible_to<double> T>
constexpr T lim(const std::function<T(T)>& f, T a) {
    constexpr auto sqrt_eps = Sqrt(std::numeric_limits<T>::epsilon());

    if(std::isinf((double) a))
        throw std::logic_error("Attempted to evaluate a two-sided limit at infinity");

    T ll = left_lim(f, a);
    T rl = right_lim(f, a);

    if(Abs(rl - ll) < Abs(rl + ll) * T(0.5) * sqrt_eps)
        return (rl + ll) * 0.5;
    else
        return std::numeric_limits<T>::quiet_NaN();
}
