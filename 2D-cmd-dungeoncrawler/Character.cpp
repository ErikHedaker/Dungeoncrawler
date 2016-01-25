#include "Character.h"
#include "Functions.h"
#include "AStarAlgorithm.h"
#include <map>

Character::Character( const Vector2i& position, const EntityType& type, float speed, float armor, float damage, float health, float mana ) :
	Entity( position, type ),
	_positionPrev( position ),
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
	static const std::map<Orientation, Vector2i> directions =
	{
		{ Orientation::North, Vector2i(  0, -1 ) },
		{ Orientation::West,  Vector2i( -1,  0 ) },
		{ Orientation::South, Vector2i(  0,  1 ) },
		{ Orientation::East,  Vector2i(  1,  0 ) }
	};

	_positionPrev = GetPosition( );
	SetPosition( GetPosition( ) + directions.at( orientation ) );
}
void Character::MoveTowards( const Vector2i& position )
{
	int costBest = 999999999;
	Vector2i positionBest = Vector2i( 0, 0 );
	std::vector<Vector2i> directions =
	{
		Vector2i( 1, 0 ),
		Vector2i( 0, -1 ),
		Vector2i( -1, 0 ),
		Vector2i( 0, 1 )
	};

	_positionPrev = GetPosition( );

	for( const auto& direction : directions )
	{
		Vector2i positionNeighbor = GetPosition( ) + direction;
		int costCheck = Heuristic( positionNeighbor, position );

		if( costBest > costCheck )
		{
			costBest = costCheck;
			positionBest = positionNeighbor;
		}
	}

	SetPosition( positionBest );
}
void Character::MoveProbability( int north, int south, int west, int east, int still )
{
	const int sumProbability = north + south + west + east + still;
	const int random = RandomNumberGenerator( 0, sumProbability - 1 );

	if( random < north )
	{
		Move( Orientation::North );
	}
	else if( random < north + south )
	{
		Move( Orientation::South );
	}
	else if( random < north + south + west )
	{
		Move( Orientation::West );
	}
	else if( random < north + south + west + east )
	{
		Move( Orientation::East );
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

	OutputBattleScreenStart( *attacker, *defender );

	while( true )
	{
		damage = attacker->GetDamage( ) * defender->GetArmor( );
		health = defender->GetHealth( ) - damage;
		defender->SetHealth( health );

		if( defender->GetHealth( ) <= 0 )
		{
			OutputBattleScreenEnd( *attacker, *defender );
			defender->SetAlive( false );
			break;
		}
		else
		{
			std::swap( attacker, defender );
		}
	}
}