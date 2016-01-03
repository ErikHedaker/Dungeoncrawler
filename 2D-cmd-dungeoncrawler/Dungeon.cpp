#include "Dungeon.h"
#include "RNG.h"
#include "AStarAlgorithmNew.h"
#include "IO.h"
#include <math.h>
#include <algorithm>
#include <iterator>

Dungeon::Dungeon( Player* player ) :
	_player( player )
{ }

void Dungeon::Build( char gameType )
{
	SetDungeonSize( gameType );
	SetDungeonLineOfSight( gameType );
	SetDungeonMonsterAmount( gameType );

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
		UpdateVisionData( );
		Output::ClearScreen( );
		Output::DungeonCentered( *this, _dungeonSize, _player->GetPosition( ) );
		//Output::DungeonFull( *this, _dungeonSize );
		//Output::DungeonFullHidden( *this, _dungeonSize );
		Output::PlayerStatus( *_player );
		Output::TurnOptions( );
		PlayerTurn( Input::ValidChar( "\nYour choice: ", playerTurnChoices ) );
		RandomMonsterMovement( );
		UpdateCharacters( );

		if( CheckGameState( ) )
		{
			break;
		}
	}
}

void Dungeon::UpdateEntityDataAt( const Vector2i& position, Entity* entity )
{
	_entityData[( position.row * _dungeonSize.col ) + position.col] = entity;
}
void Dungeon::UpdateHiddenDataAt( const Vector2i& position, Entity* entity )
{
	_hiddenData[( position.row * _dungeonSize.col ) + position.col] = entity;
}
void Dungeon::UpdateVisionData( )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			if( iterator >= _player->GetPosition( ) - _lineOfSight &&
				iterator <= _player->GetPosition( ) + _lineOfSight )
			{
				_visionData[( iterator.row * _dungeonSize.col ) + iterator.col] = true;
			}
		}
	}
}

Entity* const Dungeon::GetEntityDataAt( const Vector2i& position ) const
{
	return _entityData[( position.row * _dungeonSize.col ) + position.col];
}
Entity* const Dungeon::GetHiddenDataAt( const Vector2i& position ) const
{
	return _hiddenData[( position.row * _dungeonSize.col ) + position.col];
}
bool Dungeon::GetVisionDataAt( const Vector2i& position ) const
{
	return _visionData[( position.row * _dungeonSize.col ) + position.col];
}

void Dungeon::SetDungeonSize( char gameType )
{
	if( gameType == '1' ) // Configuration
	{
		RETRY:
		_dungeonSize.col = Input::PositiveInteger( "\nEnter the Dungeon width: " );
		_dungeonSize.row = Input::PositiveInteger( "Enter the Dungeon heigth: " );

		if( _dungeonSize.col < 3 ||
			_dungeonSize.row < 3 )
		{
			Output::String( "\nDungeon size is too small, try again.\n" );
			goto RETRY;
		}
	}
	else // Randomization
	{
		_dungeonSize.col = RandomNumberGenerator( 40, 80 );
		_dungeonSize.row = RandomNumberGenerator( 40, 80 );
	}
}
void Dungeon::SetDungeonLineOfSight( char gameType )
{
	if( gameType == '1' ) // Configuration
	{
		_lineOfSight = Input::PositiveInteger( "Enter the line of sight range: " );
	}
	else // Randomization
	{
		_lineOfSight = 5;
	}
}
void Dungeon::SetDungeonMonsterAmount( char gameType )
{
	int amountMonsters;

	if( gameType == '1' ) // Configuration
	{
		const int maxMonsters = static_cast<int>( sqrt( _dungeonSize.col * _dungeonSize.row ) * 11.0 - 100.0 );

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
	}
	else // Randomization
	{
		const double high = sqrt( _dungeonSize.col * _dungeonSize.row ) / 1.5;
		const double low = sqrt( _dungeonSize.col * _dungeonSize.row ) / 3;
		amountMonsters = RandomNumberGenerator( static_cast<int>( low ), static_cast<int>( high ) );
		_monsters.resize( amountMonsters, Monster( ) );
	}

	Output::String( "\n" );
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
	const int exitAmount = 3;
	int randomIndex;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			if( iterator.col == 0 ||
				iterator.row == 0 ||
				iterator.col == _dungeonSize.col - 1 ||
				iterator.row == _dungeonSize.row - 1 )
			{
				positionValid.push_back( iterator );
			}
		}
	}

	for( int i = 0; i < exitAmount; i++ )
	{
		randomIndex = RandomNumberGenerator( 0, positionValid.size( ) - 1 );
		_exits.emplace_back( positionValid[randomIndex] );
		UpdateEntityDataAt( positionValid[randomIndex], &_exits.back( ) );
		positionValid.erase( positionValid.begin( ) + randomIndex );
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

		positionPaths = AStarAlgorithmNew( _player->GetPosition( ), exit.GetPosition( ), _dungeonSize, positionWalls );

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
	Vector2i position;
	int sourceWallsLeft = ( _dungeonSize.col * _dungeonSize.row ) / 30;

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
	Vector2i position;
	int extensionWallsLeft = ( _dungeonSize.col * _dungeonSize.row ) / 3;

	while( extensionWallsLeft > 0 )
	{
		for( auto& wall : _walls )
		{
			position = wall.GetPosition( );

			position.col += RandomNumberGenerator( 0, 1 ) * ( RandomNumberGenerator( 0, 1 ) ? 1 : -1 );
			position.row += RandomNumberGenerator( 0, 1 ) * ( RandomNumberGenerator( 0, 1 ) ? 1 : -1 );

			if( position >= Vector2i( 0, 0 ) &&
				position <= _dungeonSize - 1 &&
				GetEntityDataAt( position ) == nullptr &&
				GetHiddenDataAt( position ) == nullptr )
			{
				_walls.emplace_back( position );
				UpdateEntityDataAt( position, &_walls.back( ) );
				extensionWallsLeft--;
			}
		}
	}
}
void Dungeon::SetFillerWalls( )
{
	Vector2i iterator;
	Vector2i iteratorSurround;
	int surroundingWalls;

	for( iterator.row = 1; iterator.row < _dungeonSize.row - 1; iterator.row++ )
	{
		for( iterator.col = 1; iterator.col < _dungeonSize.col - 1; iterator.col++ )
		{
			if( GetEntityDataAt( iterator ) == nullptr &&
				GetHiddenDataAt( iterator ) == nullptr )
			{
				surroundingWalls = 0;

				for( iteratorSurround.row = iterator.row - 1; iteratorSurround.row <= iterator.row + 1; iteratorSurround.row++ )
				{
					for( iteratorSurround.col = iterator.col - 1; iteratorSurround.col <= iterator.col + 1; iteratorSurround.col++ )
					{
						if( GetEntityDataAt( iteratorSurround ) != nullptr &&
							GetEntityDataAt( iteratorSurround )->portrait == Portrait::Wall )
						{
							surroundingWalls++;
						}
					}
				}

				if( surroundingWalls > 4 )
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
			_state = GameState::Menu;
		}
	}
}
void Dungeon::PlayerMovement( const Orientation& orientation )
{
	/* If player is alive, it will be re-entered into _entityData in Dungeon::UpdateCharacters( ) */
	UpdateEntityDataAt( _player->GetPosition( ), nullptr );

	_player->Move( orientation );

	if( GetEntityDataAt( _player->GetPosition( ) ) != nullptr &&
		GetEntityDataAt( _player->GetPosition( ) )->portrait == Portrait::Wall )
	{
		_player->RevertPosition( );
	}
	else if( GetEntityDataAt( _player->GetPosition( ) ) != nullptr &&
			 GetEntityDataAt( _player->GetPosition( ) )->portrait == Portrait::Exit )
	{
		_state = GameState::Won;
	}
	else if( GetEntityDataAt( _player->GetPosition( ) ) != nullptr &&
			 GetEntityDataAt( _player->GetPosition( ) )->portrait == Portrait::Monster )
	{
		Monster* monster = static_cast<Monster*>( GetEntityDataAt( _player->GetPosition( ) ) );
		_player->Attack( monster );
	}
}
void Dungeon::RandomMonsterMovement( )
{
	for( auto& monster : _monsters )
	{
		/* If monster is alive, it will be re-entered into _entityData in Dungeon::UpdateCharacters( ) */
		UpdateEntityDataAt( monster.GetPosition( ), nullptr );

		monster.MoveProbability( 1, 1, 1, 1, 12 ); // 25% to move, 75% to stand still.

		if( GetEntityDataAt( monster.GetPosition( ) ) != nullptr &&
			GetEntityDataAt( monster.GetPosition( ) ) != _player )
		{
			monster.RevertPosition( );
		}
		else if( GetEntityDataAt( monster.GetPosition( ) ) != nullptr &&
				 GetEntityDataAt( monster.GetPosition( ) ) == _player )
		{
			monster.Attack( _player );
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
		_state = GameState::Lost;
	}
	else
	{
		UpdateEntityDataAt( _player->GetPosition( ), _player );
	}
}
bool Dungeon::CheckGameState( ) const
{

	if( _state == GameState::Won ||
		_state == GameState::Lost )
	{
		Output::GameStateEnd( _state );
		Input::Enter( );
		
		return true;
	}
	else if( _state == GameState::Menu )
	{
		return true;
	}
	else
	{
		return false;
	}
}