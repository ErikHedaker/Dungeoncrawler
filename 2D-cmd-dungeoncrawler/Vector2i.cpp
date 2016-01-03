#include "Vector2i.h"

Vector2i::Vector2i( ) :
	col( 0 ),
	row( 0 )
{ }
Vector2i::Vector2i( int col, int row ) :
	col( col ),
	row( row )
{ }

void Vector2i::operator=( const Vector2i& rhs )
{
	col = rhs.col;
	row = rhs.row;
}
void Vector2i::operator=( int rhs )
{
	col = rhs;
	row = rhs;
}
void Vector2i::operator+=( const Vector2i& rhs )
{
	col = col + rhs.col;
	row = row + rhs.row;
}
void Vector2i::operator+=( int rhs )
{
	col = col + rhs;
	row = row + rhs;
}
void Vector2i::operator-=( const Vector2i& rhs )
{
	col = col - rhs.col;
	row = row - rhs.row;
}
void Vector2i::operator-=( int rhs )
{
	col = col - rhs;
	row = row - rhs;
}
void Vector2i::operator*=( const Vector2i& rhs )
{
	col = col * rhs.col;
	row = row * rhs.row;
}
void Vector2i::operator*=( int rhs )
{
	col = col * rhs;
	row = row * rhs;
}
void Vector2i::operator/=( const Vector2i& rhs )
{
	col = col / rhs.col;
	row = row / rhs.row;
}
void Vector2i::operator/=( int rhs )
{
	col = col / rhs;
	row = row / rhs;
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
	Vector2i result;
	result.col = lhs.col + rhs.col;
	result.row = lhs.row + rhs.row;
	return result;
}
const Vector2i operator+( const Vector2i& lhs, int rhs )
{
	Vector2i result;
	result.col = lhs.col + rhs;
	result.row = lhs.row + rhs;
	return result;
}
const Vector2i operator-( const Vector2i& lhs, const Vector2i& rhs )
{
	Vector2i result;
	result.col = lhs.col - rhs.col;
	result.row = lhs.row - rhs.row;
	return result;
}
const Vector2i operator-( const Vector2i& lhs, int rhs )
{
	Vector2i result;
	result.col = lhs.col - rhs;
	result.row = lhs.row - rhs;
	return result;
}
const Vector2i operator*( const Vector2i& lhs, const Vector2i& rhs )
{
	Vector2i result;
	result.col = lhs.col * rhs.col;
	result.row = lhs.row * rhs.row;
	return result;
}
const Vector2i operator*( const Vector2i& lhs, int rhs )
{
	Vector2i result;
	result.col = lhs.col * rhs;
	result.row = lhs.row * rhs;
	return result;
}
const Vector2i operator/( const Vector2i& lhs, const Vector2i& rhs )
{
	Vector2i result;
	result.col = lhs.col / rhs.col;
	result.row = lhs.row / rhs.row;
	return result;
}
const Vector2i operator/( const Vector2i& lhs, int rhs )
{
	Vector2i result;
	result.col = lhs.col / rhs;
	result.row = lhs.row / rhs;
	return result;
}