#include "Vector2i.h"

Vector2i::Vector2i( ) :
	col( -1 ),
	row( -1 )
{ }
Vector2i::Vector2i( int col, int row ) :
	col( col ),
	row( row )
{ }

std::ostream& operator<<( std::ostream& stream, const Vector2i& position )
{
	stream << "( " << position.col << ", " << position.row << " )";

	return stream;
}

bool operator==( const Vector2i& lhs, const Vector2i& rhs )
{
	return lhs.col == rhs.col && lhs.row == rhs.row;
}
bool operator!=( const Vector2i& lhs, const Vector2i& rhs )
{
	return lhs.col != rhs.col || lhs.row != rhs.row;
}
bool operator<=( const Vector2i& lhs, const Vector2i& rhs )
{
	return lhs.col <= rhs.col && lhs.row <= rhs.row;
}
bool operator>=( const Vector2i& lhs, const Vector2i& rhs )
{
	return lhs.col >= rhs.col && lhs.row >= rhs.row;
}
bool operator<( const Vector2i& lhs, const Vector2i& rhs )
{
	return lhs.col < rhs.col && lhs.row < rhs.row;
}
bool operator>( const Vector2i& lhs, const Vector2i& rhs )
{
	return lhs.col > rhs.col && lhs.row > rhs.row;
}
const Vector2i operator+( const Vector2i& lhs, const Vector2i& rhs )
{
	Vector2i result = Vector2i( 0, 0 );
	result.col = lhs.col + rhs.col;
	result.row = lhs.row + rhs.row;
	return result;
}
const Vector2i operator+( const Vector2i& lhs, int rhs )
{
	Vector2i result = Vector2i( 0, 0 );
	result.col = lhs.col + rhs;
	result.row = lhs.row + rhs;
	return result;
}
const Vector2i operator-( const Vector2i& lhs, const Vector2i& rhs )
{
	Vector2i result = Vector2i( 0, 0 );
	result.col = lhs.col - rhs.col;
	result.row = lhs.row - rhs.row;
	return result;
}
const Vector2i operator-( const Vector2i& lhs, int rhs )
{
	Vector2i result = Vector2i( 0, 0 );
	result.col = lhs.col - rhs;
	result.row = lhs.row - rhs;
	return result;
}
const Vector2i operator*( const Vector2i& lhs, const Vector2i& rhs )
{
	Vector2i result = Vector2i( 0, 0 );
	result.col = lhs.col * rhs.col;
	result.row = lhs.row * rhs.row;
	return result;
}
const Vector2i operator*( const Vector2i& lhs, int rhs )
{
	Vector2i result = Vector2i( 0, 0 );
	result.col = lhs.col * rhs;
	result.row = lhs.row * rhs;
	return result;
}
const Vector2i operator/( const Vector2i& lhs, const Vector2i& rhs )
{
	Vector2i result = Vector2i( 0, 0 );
	result.col = lhs.col / rhs.col;
	result.row = lhs.row / rhs.row;
	return result;
}
const Vector2i operator/( const Vector2i& lhs, int rhs )
{
	Vector2i result = Vector2i( 0, 0 );
	result.col = lhs.col / rhs;
	result.row = lhs.row / rhs;
	return result;
}

void Vector2iHasher::HashCombine( std::size_t& seed, int value )
{
	/*
	https://www.quora.com/How-can-I-declare-an-unordered-set-of-pair-of-int-int-in-C++11
	Function copied from source and then rewritten.
	*/

	std::hash<int> hasher;

	seed ^= hasher( value ) + 0x9e3779b9 + ( seed << 6 ) + ( seed >> 2 );
}
std::size_t Vector2iHasher::operator()( const Vector2i& key ) const
{
	size_t seed = 0;

	HashCombine( seed, key.col );
	HashCombine( seed, key.row );

	return seed;
}