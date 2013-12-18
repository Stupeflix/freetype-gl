
#pragma once

#include <cstddef>

template<typename Type>
struct Vector4 {
  union {
    struct { Type x, y, z, w; };
    struct { Type _1, _2, width, height; };
  };
};

typedef Vector4<float> Rectf;
typedef Vector4<size_t> Rect;
typedef Vector4<int> Recti;

typedef Vector4<float> Vector4f;
typedef Vector4<size_t> Vector4s;
typedef Vector4<int> Vector4i;