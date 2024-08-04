#pragma once

#include <cmath>
#include <numbers>
#include <stdexcept>

#include "Polynomial.hpp"

namespace Interpolator {
    template <class T, std::invocable<T> F>
    std::vector<std::pair<T, T>> CreateChebyshevNodes(F f, const T& start_point, const T& end_point, size_t n) {
        T centre_point = (start_point + end_point) * 0.5;
        T interval = end_point - centre_point;
        std::vector<std::pair<T, T>> res(n);
        using namespace std::numbers;

        for(size_t k = 0; k < n; k++) {
            res[k].first = centre_point + interval * cos(k/(n - 1.0) * pi);
            res[k].second = f(res[k].first);
        }

        return res;
    }

    template <class T, std::invocable<T> F>
    constexpr std::vector<std::pair<T, T>> CreateEquidistantNodes(F f, const T& start_point, const T& end_point, size_t n) {
        if(n < 2) throw std::logic_error("Attempted to create less than two nodes on an interval");

        std::vector<std::pair<T, T>> res(n);
        T interval_between = (end_point - start_point) / (n - 1);

        for(size_t k = 0; k < n; k++) {
            res[k].first = start_point + k * interval_between;
            res[k].second = f(res[k].first);
        }

        return res;
    }
};

namespace Interpolator::Lagrange {
    template <class T>
    constexpr Polynomial<T> ComputePolynomial(const std::vector<std::pair<T, T>>& points) {
        Polynomial<T> res;
        Polynomial<T> base = {0.0};
        Polynomial<T> mult = {0.0, 0.0};

        res.Coefficients = std::vector<T>(points.size());
        res.Coefficients.clear();

        // Iterate through all Lagrange bases
        for(size_t i = 0; i < points.size(); i++) {
            base.Coefficients = {1.0};
            auto& [xi, yi] = points[i];
            
            for(size_t j = 0; j < points.size(); j++) {
                auto& [xj, yj] = points[j];

                if(i == j) { 
                    continue;
                }
                else {
                    mult.Coefficients = {-xj, T(1.0)};
                    base *= mult;
                    base /= (xi - xj);
                }
            }
            res += base * yi;
        }

        return res;
    }

    template <class T>
    constexpr auto CreateBarycentricInterpolator(const std::vector<std::pair<T, T>>& points) {
        return [=, weights = std::vector<T>(points.size()), cp_points = points, precomputed_weights=false](T x) mutable -> T {
            if(!precomputed_weights) {
                std::fill(weights.begin(), weights.end(), T(1.0));
                for(size_t i = 0; i < cp_points.size(); i++)
                    for(size_t j = 0; j < cp_points.size(); j++)
                        if(i == j) continue;
                        else weights[i] /= cp_points[i].first - cp_points[j].first;
                
                precomputed_weights = true;
            }

            T numerator = T(0.0);
            T denominator = T(0.0);
            T holder = T(0.0);
            for(size_t i = 0; i < cp_points.size(); i++) {
                holder = weights[i] / (x - cp_points[i].first);
                numerator += holder * cp_points[i].second;
                denominator += holder;
            }

            return numerator / denominator;
        };
    }
};

namespace Interpolator::Trigonometric {

};