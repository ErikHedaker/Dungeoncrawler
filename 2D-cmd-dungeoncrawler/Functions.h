#pragma once

#include "Vector2i.h"
#include <vector>

template<class T>
std::vector<T> TransposeArray1D( const std::vector<T>& arrayOld, const Vector2i& sizeOld, const Vector2i& sizeNew )
{
	auto arrayNew = arrayOld;
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < sizeNew.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < sizeNew.col; iterator.col++ )
		{
			arrayNew[( iterator.row * sizeNew.col ) + iterator.col] = arrayOld[( iterator.col * sizeOld.col ) + iterator.row];
		}
	}

	return arrayNew;
}

template<class T>
std::vector<T> ReverseColoumsArray1D( const std::vector<T>& arrayOld, const Vector2i& size )
{
	auto arrayNew = arrayOld;
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < size.row; iterator.row++ )
	{
		auto beginCol = arrayNew.begin( ) + iterator.row * size.col;
		auto endCol = arrayNew.begin( ) + iterator.row * size.col + size.col;

		std::reverse( beginCol, endCol );
	}

	return arrayNew;
}

int RandomNumberGenerator( int min, int max );