#ifndef CORE_MATH_H
#define CORE_MATH_H

#include <math.h>

typedef float scalar;

inline scalar
Round(scalar A)
{
    scalar Result = roundf(A);
    return Result;
}

struct vec2
{
    scalar X;
    scalar Y;
};

inline vec2
Vec2(scalar X, scalar Y)
{
    vec2 Result = {
        .X = X,
        .Y = Y,
    };
    return Result;
}

inline vec2
operator-(vec2 A, vec2 B)
{
    vec2 Result = {
        .X = A.X - B.X,
        .Y = A.Y - B.Y,
    };
    return Result;
}

inline vec2
operator*(vec2 A, scalar B)
{
    vec2 Result = {
        .X = A.X * B,
        .Y = A.Y * B,
    };
    return Result;
}

inline vec2
operator*(scalar A, vec2 B)
{
    return B * A;
}

inline vec2
operator/(scalar A, vec2 B)
{
    vec2 Result = {
        .X = A / B.X,
        .Y = A / B.Y,
    };
    return Result;
}

inline vec2
Hadamard(vec2 A, vec2 B)
{
    vec2 Result = {
        .X = A.X * B.X,
        .Y = A.Y * B.Y
    };
    return Result;
}

struct rect2
{
    vec2 Min;
    vec2 Max;
};

inline rect2
Rectangle2MinMax(scalar MinX, scalar MinY, scalar MaxX, scalar MaxY)
{
    rect2 Result = {
        .Min.X = MinX,
        .Min.Y = MinY,
        .Max.X = MaxX,
        .Max.Y = MaxY,
    };
    return Result;
}

inline rect2
Rect2MinSize(scalar MinX, scalar MinY, scalar Width, scalar Height)
{
    rect2 Result = Rectangle2MinMax(MinX, MinY, MinX + Width, MinY + Height);
    return Result;
}

inline vec2
GetRect2Dim(rect2 Rectangle)
{
    vec2 Result = {
        .X = Rectangle.Max.X - Rectangle.Min.X,
        .Y = Rectangle.Max.Y - Rectangle.Min.Y,
    };
    return Result;
}

union vec3
{
    struct
    {
        scalar X;
        scalar Y;
        scalar Z;
    };
};

inline vec3
operator*(vec3 A, scalar B)
{
    vec3 Result = {
        .X = A.X * B,
        .Y = A.Y * B,
        .Z = A.Z * B,
    };
    return Result;
}

union vec4
{
    struct
    {
        scalar X;
        scalar Y;
        scalar Z;
        scalar W;
    };

    struct
    {
        scalar R;
        scalar G;
        scalar B;
        scalar A;
    };

    struct
    {
        vec3 XYZ;
        scalar XYZPad;
    };

    struct
    {
        vec3 RGB;
        scalar RGBPad;
    };
};

inline vec4
Vec4(scalar X, scalar Y, scalar Z, scalar W)
{
    vec4 Result = {
        .X = X,
        .Y = Y,
        .Z = Z,
        .W = W,
    };
    return Result;
}

inline vec4
operator*(vec4 A, scalar B)
{
    vec4 Result = {
        .X = A.X * B,
        .Y = A.Y * B,
        .Z = A.Z * B,
        .W = A.W * B,
    };
    return Result;
}

inline vec4
operator/(vec4 A, scalar B)
{
    vec4 Result = {
        .X = A.X / B,
        .Y = A.Y / B,
        .Z = A.Z / B,
        .W = A.W / B,
    };
    return Result;
}

#endif // CORE_MATH_H