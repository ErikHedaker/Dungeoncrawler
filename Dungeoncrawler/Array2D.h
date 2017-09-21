#pragma once

#include "Vector2.h"
#include <vector>
#include <utility>

template<class T>
class Array2D
{
    public:
        Array2D( const Vector2<int>& size ) :
            _size( size ),
            _data( size.x * size.y )
        { }
        Array2D( Array2D<T>&& other ) = default;
        Array2D( const Array2D<T>& other ) = delete;

        Array2D<T>& operator=( const Array2D<T>& other ) = delete;
        Array2D<T>& operator=( Array2D<T>&& other )
        {
            _size = std::move( other._size );
            _data = std::move( other._data );

            return *this;
        }
        const T& operator[]( const Vector2<int>& position ) const
        {
            return _data[( position.y * _size.x ) + position.x];
        }
        const T& operator[]( std::size_t index ) const
        {
            return _data[index];
        }
        T& operator[]( const Vector2<int>& position )
        {
            return _data[( position.y * _size.x ) + position.x];
        }
        T& operator[]( std::size_t index )
        {
            return _data[index];
        }

    private:
        Vector2<int> _size;
        std::vector<T> _data;
};