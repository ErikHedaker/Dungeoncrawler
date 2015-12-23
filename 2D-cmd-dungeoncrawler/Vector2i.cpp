#include "Vector2i.h"

Vector2i::Vector2i( ) :
	col( 0 ),
	row( 0 )
{ }
Vector2i::Vector2i( int col, int row ) :
	col( col ),
	row( row )
{ }

bool Vector2i::operator==( const Vector2i& compare ) const
{
	return col == compare.col && row == compare.row;
}
bool Vector2i::operator!=( const Vector2i& compare ) const
{
	return col != compare.col || row != compare.row;
}
bool Vector2i::operator<=( const Vector2i& compare ) const
{
	return col <= compare.col && row <= compare.row;
}
bool Vector2i::operator>=( const Vector2i& compare ) const
{
	return col >= compare.col && row >= compare.row;
}
void Vector2i::operator=( const Vector2i& assign )
{
	col = assign.col;
	row = assign.row;
}
const Vector2i& Vector2i::operator+( const Vector2i& add )
{
	col += add.col;
	row += add.row;
	return *this;
}
const Vector2i& Vector2i::operator+( int add )
{
	col += add;
	row += add;
	return *this;
}
const Vector2i& Vector2i::operator-( const Vector2i& subtract )
{
	col -= subtract.col;
	row -= subtract.row;
	return *this;
}
const Vector2i& Vector2i::operator-( int subtract )
{
	col -= subtract;
	row -= subtract;
	return *this;
}