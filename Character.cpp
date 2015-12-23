#include "Character.h"

extern int RandomNumberGenerator( int min, int max );

Character::Character( const Vector2i& position, char portrait ) :
	Entity( position, portrait )
{ }

void Character::Move( Orientation orientation )
{
	Vector2i position = GetPosition( );
	_positionPrev = GetPosition( );

	switch( orientation )
	{
		case Orientation::North:
		{
			position.row = position.row - 1;
			break;
		}
		case Orientation::South:
		{
			position.row = position.row + 1;
			break;
		}
		case Orientation::East:
		{
			position.col = position.col + 1;
			break;
		}
		case Orientation::West:
		{
			position.col = position.col - 1;
			break;
		}
	}

	SetPosition( position );
}
void Character::MoveTowards( const Vector2i& position )
{

}
void Character::MoveProbability( int north, int east, int south, int west, int still )
{
	int sumProbability = north + east + south + west + still;
	int random = RandomNumberGenerator( 1, sumProbability );

	if( random <= north )
	{
		Move( Orientation::North );
	}
	else if( random <= north + east )
	{
		Move( Orientation::East );
	}
	else if( random <= north + east + south )
	{
		Move( Orientation::South );
	}
	else if( random <= north + east + south + west )
	{
		Move( Orientation::West );
	}
}
void Character::RevertPosition( )
{
	Vector2i positionPrev = GetPosition( );
	SetPosition( _positionPrev );
	_positionPrev = positionPrev;
}