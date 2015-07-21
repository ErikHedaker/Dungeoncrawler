#include "Vector2i.h"

class Entity
{
	public:
		Entity( );
		Entity( const int& x, const int &y );
		Entity( const Vector2i& position );

		void SetPosition( Vector2i position );
		Vector2i GetPosition( ) const;

	private:
		Vector2i _position;
};