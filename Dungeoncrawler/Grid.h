#pragma once

#include "Vector2.h"
#include "Functions.h"
#include <vector>
#include <utility>
#include <map>

template<class T>
class Grid
{
    public:
        Grid( const Vector2<int>& size ) :
            _size( size ),
            _data( size.x * size.y )
        { }
        Grid( Grid<T>&& other ) = default;
        Grid( const Grid<T>& other ) = delete;

        Grid<T>& operator=( const Grid<T>& other ) = delete;
        Grid<T>& operator=( Grid<T>&& other )
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

        const Vector2<int>& Size( ) const
        {
            return _size;
        }
        void Rotate( const Orientation::Enum& orientation )
        {
            const std::map<Orientation::Enum, Vector2<int>> sizeSet
            {
                { Orientation::North, { _size.x, _size.y } },
                { Orientation::East,  { _size.y, _size.x } },
                { Orientation::South, { _size.x, _size.y } },
                { Orientation::West,  { _size.y, _size.x } }
            };
            const Vector2<int> sizeNext = sizeSet.at( orientation );
            const Vector2<int> sizePrev = _size;
            Grid<T> rotate( sizeNext );
            Vector2<int> iterator;

            for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
            {
                for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
                {
                    const Vector2<int> rotation = PositionRotate( iterator, sizePrev, orientation );
                    const int indexPrev = ( rotation.x * sizeNext.y ) + rotation.y;
                    const int indexNext = ( iterator.x * sizePrev.y ) + iterator.y;

                    rotate[indexNext] = (*this)[indexPrev];
                }
            }

            _size = std::move( rotate._size );
            _data = std::move( rotate._data );
        }

    private:
        Vector2<int> _size;
        std::vector<T> _data;
};