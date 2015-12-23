#pragma once

#include "Vector2i.h"

class Entity
{
	public:
		Entity( const Vector2i& position, char portrait );

		const char portrait;

		void SetPosition( const Vector2i& position );
		const Vector2i GetPosition( ) const;

	private:
		Vector2i _position;
};