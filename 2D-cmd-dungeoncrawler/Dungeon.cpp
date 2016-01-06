#include "Dungeon.h"
#include "RNG.h"
#include "AStarAlgorithm.h"
#include "IO.h"
#include <math.h>
#include <algorithm>
#include <iterator>

Dungeon::Dungeon( Player* player ) :
	_player( player )
{ }

void Dungeon::Build( const GameType& type )
{
	SetDungeonSize( type );
	SetPlayerLineOfSight( type );
	SetMonsterAmount(type );

	BuildEntityData( );
	BuildHiddenData( );
	BuildVisionData( );

	SetPlayer( );
	SetExits( );
	SetOuterWalls( );
	Output::String( "\nLoading path." );
	SetHiddenPath( );
	Output::String( "\nLoading source walls." );
	SetRandomSourceWalls( );
	Output::String( "\nLoading extension walls." );
	SetRandomExtensionWalls( );
	Output::String( "\nLoading filler walls." );
	SetFillerWalls( );
	Output::String( "\nLoading monster positions." );
	SetRandomMonsterPositions( );
}
void Dungeon::GameLoop( )
{
	const std::vector<char> playerTurnChoices { 'W', 'w', 'A', 'a', 'S', 's', 'D', 'd', 'Q', 'q', 'E', 'e' };

	while( true )
	{
		UpdateVisionDataAt( _player->GetPosition( ), _player->GetLineOfSight( ) );
		Output::ClearScreen( );
		Output::DungeonCentered( *this, _dungeonSize, _player->GetPosition( ) );
		//Output::DungeonFull( *this, _dungeonSize );
		//Output::DungeonFullHidden( *this, _dungeonSize );
		Output::PlayerStatus( *_player );
		Output::TurnOptions( );
		PlayerTurn( Input::ValidChar( "\nYour choice: ", playerTurnChoices ) );
		RandomMonsterMovement( );
		UpdateCharacters( );

		if( CheckGameStatus( ) )
		{
			break;
		}
	}
}

bool Dungeon::InBounds( const Vector2i& position ) const
{
	return
		position.col >= 0 &&
		position.row >= 0 &&
		position.col <= _dungeonSize.col - 1 &&
		position.row <= _dungeonSize.row - 1;
}
bool Dungeon::FloorSurroundedWalls( const Vector2i& position, int threshold ) const
{
	static const std::array<Vector2i, 8> directions =
	{
		Vector2i( 0, -1 ),
		Vector2i( 1, -1 ),
		Vector2i( 1, 0 ),
		Vector2i( 1, 1 ),
		Vector2i( 0, 1 ),
		Vector2i( -1, 1 ),
		Vector2i( -1, 0 ),
		Vector2i( -1, -1 )
	};
	int surroundingWalls = 0;
	Vector2i positionNeighbor;

	for( const auto& direction : directions )
	{
		positionNeighbor = position + direction;

		if( GetEntityDataAt( positionNeighbor ) != nullptr )
		{
			surroundingWalls++;
		}
	}

	return surroundingWalls > threshold;
}

void Dungeon::UpdateEntityDataAt( const Vector2i& position, Entity* entity )
{
	_entityData[( position.row * _dungeonSize.col ) + position.col] = entity;
}
void Dungeon::UpdateHiddenDataAt( const Vector2i& position, Entity* entity )
{
	_hiddenData[( position.row * _dungeonSize.col ) + position.col] = entity;
}
void Dungeon::UpdateVisionDataAt( const Vector2i& position, int lineOfSight )
{
	const Vector2i iteratorBegin = position - lineOfSight;
	const Vector2i iteratorEnd = position + lineOfSight;
	Vector2i iterator;

	for( iterator.row = iteratorBegin.row; iterator.row <= iteratorEnd.row; iterator.row++ )
	{
		for( iterator.col = iteratorBegin.col; iterator.col <= iteratorEnd.col; iterator.col++ )
		{
			if( InBounds( iterator ) )
			{
				_visionData[( iterator.row * _dungeonSize.col ) + iterator.col] = true;
			}
		}
	}
}

const Entity* const Dungeon::GetEntityDataAt( const Vector2i& position ) const
{
	return _entityData[( position.row * _dungeonSize.col ) + position.col];
}
const Entity* const Dungeon::GetHiddenDataAt( const Vector2i& position ) const
{
	return _hiddenData[( position.row * _dungeonSize.col ) + position.col];
}
bool Dungeon::GetVisionDataAt( const Vector2i& position ) const
{
	return _visionData[( position.row * _dungeonSize.col ) + position.col];
}

void Dungeon::SetDungeonSize( const GameType& type )
{
	switch( type )
	{
		case GameType::Randomized:
		{
			_dungeonSize.col = RandomNumberGenerator( 40, 80 );
			_dungeonSize.row = RandomNumberGenerator( 40, 80 );

			break;
		}
		case GameType::MinorConfiguration:
		{
			while( true )
			{
				_dungeonSize.col = Input::PositiveInteger( "\nEnter the Dungeon width: " );
				_dungeonSize.row = Input::PositiveInteger( "Enter the Dungeon heigth: " );

				if( _dungeonSize.col > 2 &&
					_dungeonSize.row > 2 )
				{
					break;
				}
				else
				{
					Output::String( "\nDungeon size is too small, try again.\n" );
				}
			}

			break;
		}
		default:
		{
			Output::String( "\nSomething went wrong." );
			Input::Enter( );

			break;
		}
	}
}
void Dungeon::SetPlayerLineOfSight( const GameType& type )
{
	switch( type )
	{
		case GameType::Randomized: /* Not really randomized */
		{
			_player->SetLineOfSight( 5 );

			break;
		}
		case GameType::MinorConfiguration:
		{
			_player->SetLineOfSight( Input::PositiveInteger( "Enter the line of sight range: " ) );

			break;
		}
		default:
		{
			Output::String( "\nSomething went wrong." );
			Input::Enter( );

			break;
		}
	}
}
void Dungeon::SetMonsterAmount( const GameType& type )
{
	int amountMonsters;

	switch( type )
	{
		case GameType::Randomized:
		{
			const double high = sqrt( _dungeonSize.col * _dungeonSize.row ) / 1.5;
			const double low = sqrt( _dungeonSize.col * _dungeonSize.row ) / 3;

			amountMonsters = RandomNumberGenerator( static_cast<int>( low ), static_cast<int>( high ) );
			_monsters.resize( amountMonsters, Monster( ) );

			break;
		}
		case GameType::MinorConfiguration:
		{
			const int maxMonsters = static_cast<int>( sqrt( _dungeonSize.col * _dungeonSize.row ) * 10.5 - 100.0 ); /* 5 monsters in 10 * 10 grid */

			while( true )
			{
				amountMonsters = Input::PositiveInteger( "Enter the amount of monsters: " );

				if( amountMonsters <= maxMonsters )
				{
					_monsters.resize( amountMonsters, Monster( ) );

					break;
				}
				else
				{
					Output::String( "\nToo many monsters, try again.\n\n" );
				}
			}

			break;
		}
		default:
		{
			Output::String( "\nSomething went wrong." );
			Input::Enter( );

			break;
		}
	}
}

void Dungeon::BuildEntityData( )
{
	_entityData.clear( );
	_entityData.resize( _dungeonSize.col * _dungeonSize.row, nullptr );
}
void Dungeon::BuildHiddenData( )
{
	_hiddenData.clear( );
	_hiddenData.resize( _dungeonSize.col * _dungeonSize.row, nullptr );
}
void Dungeon::BuildVisionData( )
{
	_visionData.clear( );
	_visionData.resize( _dungeonSize.col * _dungeonSize.row, false );
}

void Dungeon::SetPlayer( )
{
	Vector2i position;

	position.col = RandomNumberGenerator( 1, _dungeonSize.col - 2 );
	position.row = RandomNumberGenerator( 1, _dungeonSize.row - 2 );

	_player->SetPosition( position );
	UpdateEntityDataAt( position, _player );
}
void Dungeon::SetExits( )
{
	Vector2i iterator;
	std::vector<Vector2i> positionValid;
	const int exitAmount = 2;
	int index;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			if( iterator.col == 0 ||
				iterator.row == 0 ||
				iterator.col == _dungeonSize.col - 1 ||
				iterator.row == _dungeonSize.row - 1 )
			{
				/* Check if position is not in a corner */
				if( ( iterator.col != 0 || iterator.col != _dungeonSize.col - 1 ) &&
					( iterator.row != 0 || iterator.row != _dungeonSize.row - 1 ) )
				{
					positionValid.push_back( iterator );
				}
			}
		}
	}

	for( int i = 0; i < exitAmount; i++ )
	{
		index = RandomNumberGenerator( 0, positionValid.size( ) - 1 );
		_exits.emplace_back( positionValid[index] );
		UpdateEntityDataAt( positionValid[index], &_exits.back( ) );
		positionValid.erase( positionValid.begin( ) + index );
	}
}
void Dungeon::SetOuterWalls( )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			if( GetEntityDataAt( iterator ) == nullptr )
			{
				if( iterator.col == 0 ||
					iterator.row == 0 ||
					iterator.col == _dungeonSize.col - 1 ||
					iterator.row == _dungeonSize.row - 1 )
				{
					_walls.emplace_back( iterator );
					UpdateEntityDataAt( iterator, &_walls.back( ) );
				}
			}
		}
	}
}
void Dungeon::SetHiddenPath( )
{
	for( const auto& exit : _exits )
	{
		std::vector<Vector2i> positionWalls;
		std::vector<Vector2i> positionPaths;

		for( const auto& wall : _walls )
		{
			positionWalls.push_back( wall.GetPosition( ) );
		}

		positionPaths = AStarAlgorithm( _player->GetPosition( ), exit.GetPosition( ), _dungeonSize, positionWalls );

		for( const auto& path : positionPaths )
		{
			if( GetHiddenDataAt( path ) == nullptr )
			{
				_paths.emplace_back( path );
				UpdateHiddenDataAt( path, &_paths.back( ) );
			}
		}
	}
}
void Dungeon::SetRandomSourceWalls( )
{
	int sourceWallsLeft = ( _dungeonSize.col * _dungeonSize.row ) / 10;
	Vector2i position;

	while( sourceWallsLeft > 0 )
	{
		position.col = RandomNumberGenerator( 1, _dungeonSize.col - 2 );
		position.row = RandomNumberGenerator( 1, _dungeonSize.row - 2 );

		if( GetEntityDataAt( position ) == nullptr &&
			GetHiddenDataAt( position ) == nullptr )
		{
			_walls.emplace_back( position );
			UpdateEntityDataAt( position, &_walls.back( ) );
			sourceWallsLeft--;
		}
	}
}
void Dungeon::SetRandomExtensionWalls( )
{
	const std::array<Vector2i, 8> directions =
	{
		Vector2i( 0, -1 ),
		Vector2i( 1, -1 ),
		Vector2i( 1, 0 ),
		Vector2i( 1, 1 ),
		Vector2i( 0, 1 ),
		Vector2i( -1, 1 ),
		Vector2i( -1, 0 ),
		Vector2i( -1, -1 )
	};
	int extensionWallsLeft = ( _dungeonSize.col * _dungeonSize.row ) / 4;
	int index;
	Vector2i positionNeighbor;


	while( extensionWallsLeft > 0 )
	{
		for( auto& wall : _walls )
		{
			index = RandomNumberGenerator( 0, 7 );
			positionNeighbor = wall.GetPosition( ) + directions[index];

			if( InBounds( positionNeighbor ) &&
				GetEntityDataAt( positionNeighbor ) == nullptr &&
				GetHiddenDataAt( positionNeighbor ) == nullptr )
			{
				_walls.emplace_back( positionNeighbor );
				UpdateEntityDataAt( positionNeighbor, &_walls.back( ) );
				extensionWallsLeft--;
			}
		}
	}
}
void Dungeon::SetFillerWalls( )
{
	const int amount = 2;
	Vector2i iterator;
	
	for( int i = 0; i < amount; i++ )
	{
		for( iterator.row = 1; iterator.row < _dungeonSize.row - 1; iterator.row++ )
		{
			for( iterator.col = 1; iterator.col < _dungeonSize.col - 1; iterator.col++ )
			{
				if( GetEntityDataAt( iterator ) == nullptr &&
					GetHiddenDataAt( iterator ) == nullptr &&
					FloorSurroundedWalls( iterator, 4 ) )
				{
					_walls.emplace_back( iterator );
					UpdateEntityDataAt( iterator, &_walls.back( ) );
				}
			}
		}
	}
}
void Dungeon::SetRandomMonsterPositions( )
{
	Vector2i position;

	for( auto& monster : _monsters )
	{
		while( true )
		{
			position.col = RandomNumberGenerator( 1, _dungeonSize.col - 2 );
			position.row = RandomNumberGenerator( 1, _dungeonSize.row - 2 );

			if( GetEntityDataAt( position ) == nullptr )
			{
				monster.SetPosition( position );
				UpdateEntityDataAt( position, &monster );

				break;
			}
		}
	}
}

void Dungeon::PlayerTurn( char choice )
{
	switch( choice )
	{
		case 'W':
		case 'w':
		{
			PlayerMovement( Orientation::North );

			break;
		}

		case 'A':
		case 'a':
		{
			PlayerMovement( Orientation::West );

			break;
		}

		case 'S':
		case 's':
		{
			PlayerMovement( Orientation::South );

			break;
		}

		case 'D':
		case 'd':
		{
			PlayerMovement( Orientation::East );

			break;
		}

		case 'Q':
		case 'q':
		{
			break;
		}

		case 'E':
		case 'e':
		{
			_status = GameStatus::Menu;

			break;
		}
		default:
		{
			Output::String( "\nSomething went wrong." );
			Input::Enter( );

			break;
		}
	}
}
void Dungeon::PlayerMovement( const Orientation& orientation )
{
	/* If player is alive, it will be re-entered into _entityData in Dungeon::UpdateCharacters( ) */
	UpdateEntityDataAt( _player->GetPosition( ), nullptr );

	_player->Move( orientation );

	auto entityCached = GetEntityDataAt( _player->GetPosition( ) );

	if( entityCached != nullptr &&
		entityCached->portrait == Portrait::Wall )
	{
		_player->RevertPosition( );
	}
	else if( entityCached != nullptr &&
			 entityCached->portrait == Portrait::Exit )
	{
		_status = GameStatus::Won;
	}
	else if( entityCached != nullptr &&
			 entityCached->portrait == Portrait::Monster )
	{
		/* 
			const_cast is fine here because the object that the pointer points to is mutable (a monster in this case).
			While I try to be as const correct as possible, this is only one time I need to access and modify a monster directly from a pointer.
			My only other option would be to search all monster for the same position as the entity pointer, which would be inefficient.
		*/

		auto entity = const_cast<Entity*>( GetEntityDataAt( _player->GetPosition( ) ) );
		auto monster = static_cast<Monster*>( entity );
		_player->Attack( monster );
	}
}
void Dungeon::RandomMonsterMovement( )
{
	for( auto& monster : _monsters )
	{
		UpdateEntityDataAt( monster.GetPosition( ), nullptr );

		monster.MoveProbability( 1, 1, 1, 1, 12 ); /* 25% to move, 75% to stand still. */

		auto entityCached = GetEntityDataAt( monster.GetPosition( ) );

		if( entityCached != nullptr &&
			entityCached != _player )
		{
			monster.RevertPosition( );
		}
		else if( entityCached != nullptr &&
				 entityCached == _player )
		{
			monster.Attack( _player );
		}
		else
		{
			UpdateEntityDataAt( monster.GetPosition( ), &monster );
		}
	}
}
void Dungeon::UpdateCharacters( )
{
	for( auto it = _monsters.begin( ); it != _monsters.end( ); it++ )
	{
		if( it->GetAlive( ) == false )
		{
			_monsters.erase( it-- );
		}
		else
		{
			UpdateEntityDataAt( it->GetPosition( ), &( *it ) );
		}
	}

	if( _player->GetAlive( ) == false )
	{
		_status = GameStatus::Lost;
	}
	else
	{
		UpdateEntityDataAt( _player->GetPosition( ), _player );
	}
}
bool Dungeon::CheckGameStatus( ) const
{

	if( _status == GameStatus::Won ||
		_status == GameStatus::Lost )
	{
		Output::GameStatusEnd( _status );
		Input::Enter( );
		
		return true;
	}
	else if( _status == GameStatus::Menu )
	{
		return true;
	}
	else
	{
		return false;
	}
}