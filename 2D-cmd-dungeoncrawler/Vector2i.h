#pragma once

#include <utility>
#include <iostream>

struct Vector2i
{
	Vector2i( );
	Vector2i( int col, int row );

	int col;
	int row;

	friend std::ostream& operator<<( std::ostream& stream, const Vector2i& position );
};

std::ostream& operator<<( std::ostream& stream, const Vector2i& position );

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