class Vector2i
{
	public:
		Vector2i( );
		Vector2i( const int& xNew, const int& yNew );

		int x;
		int y;

		void operator=( const Vector2i& assign );
		bool operator==( const Vector2i& compare ) const;
		bool operator<=( const Vector2i& compare ) const;
		bool operator>=( const Vector2i& compare ) const;
		Vector2i& operator+( const Vector2i& add );
		Vector2i& operator+( const int& add );
		Vector2i& operator-( const Vector2i& subtract );
		Vector2i& operator-( const int& subtract );
};