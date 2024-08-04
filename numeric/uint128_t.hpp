#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

constexpr bool __UseLittleEndian = []() {
    uint16_t _test = 1;
    auto x = std::bit_cast<std::array<uint8_t, 2>>(_test);
    return x[0] == 1;
}();

/**  @brief A generic implementation for 128-bit unsigned integer.
 *  @note Only accessible when `uint64_t` is available, but used only 
 * if compiler-provided implementation is unavailable.
 */
struct uint128_t {
public:
    constexpr uint128_t() noexcept { _Words[0] = 0, _Words[1] = 0; };
    constexpr uint128_t(uint64_t x) noexcept { _Hi() = 0, _Lo() = x; }

    friend constexpr uint128_t __umul128(uint64_t a, uint64_t b) noexcept;

    friend constexpr bool operator==(uint128_t a, uint128_t b) noexcept;
    friend constexpr auto operator<=>(uint128_t a, uint128_t b) noexcept;

    friend constexpr uint128_t& operator+=(uint128_t& a, uint128_t b) noexcept;
    friend constexpr uint128_t& operator-=(uint128_t& a, uint128_t b) noexcept;
    friend constexpr uint128_t& operator*=(uint128_t& a, uint128_t b) noexcept;
    friend constexpr uint128_t& operator/=(uint128_t& a, uint128_t b) noexcept;
    friend constexpr uint128_t& operator%=(uint128_t& a, uint128_t b) noexcept;

    friend constexpr uint128_t& operator&=(uint128_t& a, uint128_t b) noexcept;
    friend constexpr uint128_t& operator|=(uint128_t& a, uint128_t b) noexcept;
    friend constexpr uint128_t& operator^=(uint128_t& a, uint128_t b) noexcept;

    friend constexpr uint128_t& operator>>=(uint128_t& a, size_t n) noexcept;
    friend constexpr uint128_t& operator<<=(uint128_t& a, size_t n) noexcept;

private:
    uint64_t _Words[2];
    constexpr uint64_t& _Lo() noexcept { return __UseLittleEndian ? _Words[0] : _Words[1]; }
    constexpr uint64_t& _Hi() noexcept { return __UseLittleEndian ? _Words[1] : _Words[0]; }
};

constexpr uint128_t __umul128(uint64_t a, uint64_t b) noexcept {
    using u128 = uint128_t;

    uint64_t a_lo = a & 0x00000000ffffffff, a_hi = a >> 32;
    uint64_t b_lo = b & 0x00000000ffffffff, b_hi = b >> 32;

    // This whole process is to ensure that hi-by-hi is shifted by 64-bit at the end,
    // lo-by-hi and hi-by-lo added and shifted by 32-bit, and lo-by-lo added but unshifted 
    u128 res = u128(a_hi * b_hi);
    res <<= 32; 
    (res += a_lo * b_hi) += a_hi * b_lo;
    res <<= 32;
    res += a_lo * b_lo;

    return res;
}

constexpr bool operator==(uint128_t a, uint128_t b) noexcept {
    return a._Lo() == b._Lo() && a._Hi() == b._Hi();
}

constexpr auto operator<=>(uint128_t a, uint128_t b) noexcept {
    auto hi_ord = a._Hi() <=> b._Hi();
    return (hi_ord != 0) ? hi_ord : (a._Lo() <=> b._Lo());
}

constexpr uint128_t& operator+=(uint128_t& a, uint128_t b) noexcept {
    bool carry = (a._Lo() += b._Lo()) < b._Lo();
    a._Hi() += (b._Hi() + uint64_t(carry));
    return a;
}

constexpr uint128_t& operator-=(uint128_t& a, uint128_t b) noexcept {
    bool borrow = (a._Lo() -= b._Lo()) > b._Lo();
    a._Hi() -= (b._Hi() + uint64_t(borrow));
    return a;
}

constexpr uint128_t& operator*=(uint128_t& a, uint128_t b) noexcept {
    uint64_t a_lo = a._Lo();

    a._Hi() = a._Lo() * b._Hi() + a._Hi() * b._Lo();
    a._Lo() = 0;
    return a += __umul128(a_lo, b._Lo());
}

/// TODO: Implement 128-bit unsigned integer division
constexpr uint128_t& operator/=(uint128_t& a, uint128_t b) noexcept {
    return a;
}

/// TODO: Implement 128-bit unsigned integer division remainder
constexpr uint128_t& operator%=(uint128_t& a, uint128_t b) noexcept {
    return a;
}

constexpr uint128_t& operator&=(uint128_t& a, uint128_t b) noexcept {
    a._Hi() &= b._Hi(), a._Lo() &= b._Lo();
    return a;
}

constexpr uint128_t& operator|=(uint128_t& a, uint128_t b) noexcept {
    a._Hi() |= b._Hi(), a._Lo() |= b._Lo();
    return a;
}

constexpr uint128_t& operator^=(uint128_t& a, uint128_t b) noexcept {
    a._Hi() ^= b._Hi(), a._Lo() ^= b._Lo();
    return a;
}

constexpr uint128_t& operator>>=(uint128_t& a, size_t n) noexcept {
    if(n < 64) {
        a._Lo() = (a._Lo() >> n) | (a._Hi() << (64 - n));
        a._Hi() >>= n;
    }
    else {
        a._Lo() = a._Hi() >> (n - 64);
        a._Hi() = 0;
    } 

    return a;
}

constexpr uint128_t& operator<<=(uint128_t& a, size_t n) noexcept {
    if(n < 64) {
        a._Hi() = (a._Hi() << n) | (a._Lo() >> (64 - n));
        a._Lo() <<= n;
    }
    else {
        a._Hi() = a._Lo() << (n - 64);
        a._Lo() = 0;
    } 

    return a;
}

constexpr uint128_t operator+(uint128_t a, uint128_t b) noexcept {
    uint128_t res = a;
    return res += b;
}

constexpr uint128_t operator-(uint128_t a, uint128_t b) noexcept {
    uint128_t res = a;
    return res -= b;
}

constexpr uint128_t operator*(uint128_t a, uint128_t b) noexcept {
    uint128_t res = a;
    return res *= b;
}

constexpr uint128_t operator/(uint128_t a, uint128_t b) noexcept {
    uint128_t res = a;
    return res /= b;
}

constexpr uint128_t operator%(uint128_t a, uint128_t b) noexcept {
    uint128_t res = a;
    return res %= b;
}