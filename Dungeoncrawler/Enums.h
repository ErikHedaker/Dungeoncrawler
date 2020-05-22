#pragma once

struct Attributes
{
    enum Enum
    {
        Obstacle    = 1 << 0,
        Movement    = 1 << 1,
        Hostile     = 1 << 2
    };
};

enum class Orientation
{
    North,
    East,
    South,
    West
};