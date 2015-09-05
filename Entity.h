#include "Vector2i.h"

class Entity
{
	public:
		Entity( );
		Entity( const Vector2i& newPosition );

		void SetPosition( const Vector2i& newPosition );
		Vector2i GetPosition( ) const;

	private:
		Vector2i _position;
};