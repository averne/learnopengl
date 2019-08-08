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
