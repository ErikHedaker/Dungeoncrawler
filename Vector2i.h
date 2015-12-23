class Vector2i
{
	public:
		Vector2i( );
		Vector2i( int col, int row );

		int col;
		int row;

		bool operator==( const Vector2i& compare ) const;
		bool operator!=( const Vector2i& compare ) const;
		bool operator<=( const Vector2i& compare ) const;
		bool operator>=( const Vector2i& compare ) const;
		void operator=( const Vector2i& assign );
		const Vector2i& operator+( const Vector2i& add );
		const Vector2i& operator+( int add );
		const Vector2i& operator-( const Vector2i& subtract );
		const Vector2i& operator-( int subtract );
};