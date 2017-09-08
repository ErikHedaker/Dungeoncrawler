#pragma once

#include <utility>

template<class T> struct Vector2
{
    T x;
    T y;

    Vector2& operator+=( const Vector2& rhs )
    {
        x = x + rhs.x;
        y = y + rhs.y;

        return *this;
    }
    Vector2& operator-=( const Vector2& rhs )
    {
        x = x - rhs.x;
        y = y - rhs.y;

        return *this;
    }
    template<class Y> operator Vector2<Y>( ) const
    {
        return { static_cast<Y>( x ), static_cast<Y>( y ) };
    }
};

template<class T> bool operator==( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}
template<class T> bool operator!=( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}
template<class T> bool operator<=( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return lhs.x <= rhs.x && lhs.y <= rhs.y;
}
template<class T> bool operator>=( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return lhs.x >= rhs.x && lhs.y >= rhs.y;
}
template<class T> bool operator<( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return lhs.x < rhs.x && lhs.y < rhs.y;
}
template<class T> bool operator>( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return lhs.x > rhs.x && lhs.y > rhs.y;
}
template<class T> const Vector2<T> operator+( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}
template<class T> const Vector2<T> operator+( const Vector2<T>& lhs, T rhs )
{
    return { lhs.x + rhs, lhs.y + rhs };
}
template<class T> const Vector2<T> operator-( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return { lhs.x - rhs.x, lhs.y - rhs.y };
}
template<class T> const Vector2<T> operator-( const Vector2<T>& lhs, T rhs )
{
    return { lhs.x - rhs, lhs.y - rhs };
}
template<class T> const Vector2<T> operator*( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return { lhs.x * rhs.x, lhs.y * rhs.y };
}
template<class T> const Vector2<T> operator*( const Vector2<T>& lhs, T rhs )
{
    return { lhs.x * rhs, lhs.y * rhs };
}
template<class T> const Vector2<T> operator/( const Vector2<T>& lhs, const Vector2<T>& rhs )
{
    return { lhs.x / rhs.x, lhs.y / rhs.y };
}
template<class T> const Vector2<T> operator/( const Vector2<T>& lhs, T rhs )
{
    return { lhs.x / rhs, lhs.y / rhs };
}

template<class T> struct HasherVector2
{
    static void HashCombine( std::size_t& seed, T value )
    {
        /*
            https://www.quora.com/How-can-I-declare-an-unordered-set-of-pair-of-int-int-in-C++11
            Function implementation is based on the source
        */

        std::hash<T> hasher;

        seed ^= hasher( value ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
    };
    std::size_t operator()( const Vector2<int>& key ) const
    {
        size_t seed = 0;

        HashCombine( seed, key.x );
        HashCombine( seed, key.y );

        return seed;
    };
};