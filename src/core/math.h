#ifndef CORE_MATH_H
#define CORE_MATH_H

typedef float scalar;

struct v2
{
    scalar X;
    scalar Y;
};

union rect2
{
    struct
    {
        scalar MinX;
        scalar MinY;
        scalar MaxX;
        scalar MaxY;
    };

    struct
    {
        v2 Min;
        v2 Max;
    };
};

inline rect2
Rect2MinMax(scalar MinX, scalar MinY, scalar MaxX, scalar MaxY)
{
    rect2 Result = {};
    Result.MinX = MinX;
    Result.MinY = MinY;
    Result.MaxX = MaxX;
    Result.MaxY = MaxY;
    return Result;
}

#endif // CORE_MATH_H