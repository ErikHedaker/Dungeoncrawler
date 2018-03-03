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

struct Orientation
{
    enum Enum
    {
        North =  0,
        East  =  1,
        South =  2,
        West  = -1
    };
};