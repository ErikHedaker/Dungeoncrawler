#include "Vector2i.h"

Vector2i::Vector2i( )
{
	x = 0;
	y = 0;
}
Vector2i::Vector2i( const int& xNew, const int& yNew )
{
	x = xNew;
	y = yNew;
}

void Vector2i::operator=( const Vector2i& assign )
{
	x = assign.x;
	y = assign.y;
}
bool Vector2i::operator==( const Vector2i& compare ) const
{
	return x == compare.x && y == compare.y;
}
bool Vector2i::operator<=( const Vector2i& compare ) const
{
	return x <= compare.x && y <= compare.y;
}
bool Vector2i::operator>=( const Vector2i& compare ) const
{
	return x >= compare.x && y >= compare.y;
}
Vector2i& Vector2i::operator+( const Vector2i& add )
{
	x += add.x;
	y += add.y;
	return *this;
}
Vector2i& Vector2i::operator+( const int& add )
{
	x += add;
	y += add;
	return *this;
}
Vector2i& Vector2i::operator-( const Vector2i& subtract )
{
	x -= subtract.x;
	y -= subtract.y;
	return *this;
}
Vector2i& Vector2i::operator-( const int& subtract )
{
	x -= subtract;
	y -= subtract;
	return *this;
}