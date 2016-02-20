#pragma once

#include <utility>

struct Vector2i
{
	Vector2i( );
	Vector2i( int col, int row );

	int col;
	int row;
};

bool operator==( const Vector2i& lhs, const Vector2i& rhs );
bool operator!=( const Vector2i& lhs, const Vector2i& rhs );
bool operator<=( const Vector2i& lhs, const Vector2i& rhs );
bool operator>=( const Vector2i& lhs, const Vector2i& rhs );
bool operator<( const Vector2i& lhs, const Vector2i& rhs );
bool operator>( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator+( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator+( const Vector2i& lhs, int rhs );
const Vector2i operator-( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator-( const Vector2i& lhs, int rhs );
const Vector2i operator*( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator*( const Vector2i& lhs, int rhs );
const Vector2i operator/( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator/( const Vector2i& lhs, int rhs );

struct Vector2iHasher
{
	static void HashCombine( std::size_t& seed, int value );
	std::size_t operator( )( const Vector2i& key ) const;
};