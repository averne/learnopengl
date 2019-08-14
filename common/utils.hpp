#pragma once

#include <type_traits>

#define _STRINGIFY(x)      #x
#define  STRINGIFY(x)      _STRINGIFY(x)
#define _CONCATENATE(x, y) x##y
#define  CONCATENATE(x, y) _CONCATENATE(x, y)

#define BIT(n) (1 << (n))

#define SIZEOF_ARRAY(arr) (sizeof(arr) / sizeof((arr)[0]))

#define ASSERT_SIZE(x, sz)        static_assert(sizeof(x) == sz, "Wrong size in " STRINGIFY(x))
#define ASSERT_STANDARD_LAYOUT(x) static_assert(std::is_standard_layout_v<x>, STRINGIFY(x) " is not standard layout")

template <typename T>
struct Position {
    constexpr Position() = default;
    constexpr Position(T x, T y): x(x), y(y) { }

    inline T get_x() const { return this->x; }
    inline T get_y() const { return this->y; }
    inline std::pair<T, T> get_pos() const { return {this->x, this->y}; }

    protected:
        T x = 0, y = 0;
};

template <typename T>
struct Area {
    constexpr Area() = default;
    constexpr Area(T w, T h): w(w), h(h) { }

    inline T get_w() const { return this->w; }
    inline T get_h() const { return this->h; }
    inline std::pair<T, T> get_dims() const { return {this->w, this->h}; }

    protected:
        T w = 0, h = 0;
};
