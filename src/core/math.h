#ifndef CORE_MATH_H
#define CORE_MATH_H

typedef float scalar;

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

#endif // CORE_MATH_H