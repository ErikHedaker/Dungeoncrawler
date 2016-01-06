#include "Character.h"
#include "RNG.h"
#include "IO.h"

Character::Character( const Vector2i& position, char portrait ) :
	Entity( position, portrait ),
	_baseSpeed( 10.0f ),
	_baseArmor( 0.10f ),
	_baseDamage( 10.0f ),
	_baseHealth( 100.0f ),
	_baseMana( 100.0f ),
	_alive( true ),
	_armor( nullptr ),
	_weapon( nullptr )
{ }
Character::Character( const Vector2i& position, char portrait, float speed, float armor, float damage, float health, float mana ) :
	Entity( position, portrait ),
	_baseSpeed( speed ),
	_baseArmor( armor ),
	_baseDamage( damage ),
	_baseHealth( health ),
	_baseMana( mana ),
	_alive( true ),
	_armor( nullptr ),
	_weapon( nullptr )
{ }

void Character::Move( const Orientation& orientation )
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
		default:
		{
			Output::String( "\nSomething went wrong." );
			Input::Enter( );

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
	const int sumProbability = north + east + south + west + still;
	const int random = RandomNumberGenerator( 1, sumProbability );

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

float Character::GetSpeed( ) const
{
	return _baseSpeed;
}
void Character::SetSpeed( float value )
{
	_baseSpeed = value;
}

float Character::GetArmor( ) const
{
	return _baseArmor;
}
void Character::SetArmor( float value )
{
	_baseArmor = value;
}

float Character::GetDamage( ) const
{
	return _baseDamage;
}
void Character::SetDamage( float value )
{
	_baseDamage = value;
}

float Character::GetHealth( ) const
{
	return _baseHealth;
}
void Character::SetHealth( float value )
{
	_baseHealth = value;
}

float Character::GetMana( ) const
{
	return _baseMana;
}
void Character::SetMana( float value )
{
	_baseMana = value;
}

bool Character::GetAlive( ) const
{
	return _alive;
}
void Character::SetAlive( bool value )
{
	_alive = value;
}

void Character::Attack( Character* character )
{
	Character* attacker = this;
	Character* defender = character;
	float health;
	float damage;

	Output::BattleScreenStart( *attacker, *defender );

	while( true )
	{
		damage = attacker->GetDamage( ) * defender->GetArmor( );
		health = defender->GetHealth( ) - damage;
		defender->SetHealth( health );

		if( defender->GetHealth( ) <= 0 )
		{
			Output::BattleScreenEnd( *attacker, *defender );
			defender->SetAlive( false );
			break;
		}
		else
		{
			std::swap( attacker, defender );
		}
	}
}