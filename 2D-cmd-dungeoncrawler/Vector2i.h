#pragma once

class Vector2i
{
	public:
		Vector2i( );
		Vector2i( int col, int row );

		int col;
		int row;

		void operator=( const Vector2i& rhs );
};

bool operator==( const Vector2i& lhs, const Vector2i& rhs );
bool operator!=( const Vector2i& lhs, const Vector2i& rhs );
bool operator<=( const Vector2i& lhs, const Vector2i& rhs );
bool operator>=( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator+( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator+( const Vector2i& lhs, int rhs );
const Vector2i operator-( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator-( const Vector2i& lhs, int rhs );
const Vector2i operator*( const Vector2i& lhs, const Vector2i& rhs );
const Vector2i operator*( const Vector2i& lhs, int rhs );