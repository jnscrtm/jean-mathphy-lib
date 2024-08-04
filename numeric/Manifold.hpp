#pragma once

#include <cstddef>
#include <cstdint>

#include "Matrices.hpp"

template <class M>
class ManifoldBase {
public:
    constexpr size_t Dim() noexcept { return static_cast<M*>(this)->Dim(); }
};

class R3 : public ManifoldBase<R3> {
public:
    constexpr size_t Dim() noexcept { return 3; }
};

// Euclidean : public Manifold<Euclidean<T, double>>