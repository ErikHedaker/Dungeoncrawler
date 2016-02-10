#pragma once

#include "Vector2i.h"
#include "Enums.h"
#include <algorithm>
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
		auto coloumBegin = arrayNew.begin( ) + iterator.row * size.col;
		auto coloumEnd   = arrayNew.begin( ) + iterator.row * size.col + size.col;

		std::reverse( coloumBegin, coloumEnd );
	}

	return arrayNew;
}

void GetEnter( );
char GetValidChar( const std::string& context, const std::vector<char>& validChoices );
int GetPositiveInteger( const std::string& context );

int RandomNumberGenerator( int min, int max );

Vector2i RotatePositionClockwise( const Vector2i& position, int maxCol );
Vector2i MoveEntity( const Vector2i& position, const Orientation& orientation );
Vector2i MoveEntityProbability( const Vector2i& position, int north, int south, int west, int east, int still );
void Battle( int* attackerHealth, int* attackerDamage, int* defenderHealth, int* defenderDamage );

void AsciiArtSpider( );
void AsciiArtSwords( );