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
            Grid<T> rotatedGrid( sizeNext );
            Vector2<int> iterator;

            for( iterator.y = 0; iterator.y < sizePrev.y; iterator.y++ )
            {
                for( iterator.x = 0; iterator.x < sizePrev.x; iterator.x++ )
                {
                    const Vector2<int> rotation = PositionRotate( iterator, sizePrev, orientation );
                    const int indexPrev = ( rotation.x * sizeNext.y ) + rotation.y;
                    const int indexNext = ( iterator.x * sizePrev.y ) + iterator.y;

                    rotatedGrid[indexNext] = (*this)[indexPrev];
                }
            }

            _size = std::move( rotatedGrid._size );
            _data = std::move( rotatedGrid._data );
        }

    private:
        Vector2<int> _size;
        std::vector<T> _data;
};