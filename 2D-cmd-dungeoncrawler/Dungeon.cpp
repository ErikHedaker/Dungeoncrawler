#include "Dungeon.h"
#include "Portrait.h"
#include "IO.h"
#include <math.h>
#include <algorithm>
#include <iterator>

extern std::vector<Vector2i> AStarAlgorithm( const Vector2i& positionStart, const Vector2i& positionFinish, const Vector2i& mapSize, const std::vector<Vector2i>& obstacles );
extern int RandomNumberGenerator( int min, int max );
extern int RandomPositiveNegativeGenerator( );

void Dungeon::Build( char gameType )
{
	SetDungeonSize( gameType );
	SetDungeonLineOfSight( gameType );
	SetDungeonMonsterAmount( gameType );

	BuildHiddenData( );
	BuildEntityData( );
	BuildVisionData( );

	SetPlayer( );
	SetExits( );
	SetOuterWalls( );
	IO::OutputString( "\nLoading path.\n" );
	SetHiddenPath( );
	IO::OutputString( "\nLoading source walls.\n" );
	SetRandomSourceWalls( );
	IO::OutputString( "\nLoading extension walls.\n" );
	SetRandomExtensionWalls( );
	IO::OutputString( "\nLoading filler walls.\n" );
	SetFillerWalls( );
	IO::OutputString( "\nLoading monsters.\n" );
	SetRandomMonsterPositions( );
}
void Dungeon::GameLoop( )
{
	while( true )
	{
		UpdateVisionData( );
		IO::OutputClearScreen( );
		//IO::OutputHiddenDungeon( *this, _size );
		IO::OutputDungeonCentered( *this, _size, _player->GetPosition( ) );
		IO::OutputTurnOptions( );
		PlayerTurn( IO::InputValidChar( "\nYour choice: ", std::vector<char>{ 'W', 'w', 'A', 'a', 'S', 's', 'D', 'd', 'Q', 'q', 'E', 'e' } ) );
		RandomMonsterMovement( );

		if( CheckGameState( ) )
		{
			break;
		}
	}
}

void Dungeon::UpdateEntityDataAt( const Vector2i& position, Entity* entity )
{
	_entityData[( position.row * _size.col ) + position.col] = entity;
}
void Dungeon::UpdateHiddenDataAt( const Vector2i& position, Entity* entity )
{
	_hiddenData[( position.row * _size.col ) + position.col] = entity;
}
void Dungeon::UpdateVisionData( )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _size.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _size.col; iterator.col++ )
		{
			if( iterator >= _player->GetPosition( ) - _lineOfSight &&
				iterator <= _player->GetPosition( ) + _lineOfSight )
			{
				_visionData[( iterator.row * _size.col ) + iterator.col] = true;
			}
		}
	}
}

const Entity* const Dungeon::GetEntityDataAt( const Vector2i& position ) const
{
	return _entityData[( position.row * _size.col ) + position.col];
}
const Entity* const Dungeon::GetHiddenDataAt( const Vector2i& position ) const
{
	return _hiddenData[( position.row * _size.col ) + position.col];
}
bool Dungeon::GetVisionDataAt( const Vector2i& position ) const
{
	return _visionData[( position.row * _size.col ) + position.col];
}

void Dungeon::SetDungeonSize( char gameType )
{
	if( gameType == '1' ) // Configuration
	{
		RETRY:
		_size.col = IO::InputPositiveInteger( "\nEnter the Dungeon width: " );
		_size.row = IO::InputPositiveInteger( "Enter the Dungeon heigth: " );

		if( _size.col < 3 ||
			_size.row < 3 )
		{
			IO::OutputString( "\nDungeon size is too small, try again.\n" );
			goto RETRY;
		}
	}
	else // Randomization
	{
		_size.col = RandomNumberGenerator( 50, 100 );
		_size.row = RandomNumberGenerator( 15, 30 );
	}
}
void Dungeon::SetDungeonLineOfSight( char gameType )
{
	if( gameType == '1' ) // Configuration
	{
		_lineOfSight = IO::InputPositiveInteger( "Enter the line of sight range: " );
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
		int maxMonsters = static_cast<int>( sqrt( _size.col * _size.row ) * 11.0 - 100.0 );

		while( true )
		{
			amountMonsters = IO::InputPositiveInteger( "Enter the amount of monsters: " );

			if( amountMonsters <= maxMonsters )
			{
				_monsters.resize( amountMonsters, Monster( Vector2i( -1, -1 ), Portrait::Monster ) );
				break;
			}
			else
			{
				IO::OutputString( "\nToo many monsters, try again.\n\n" );
			}
		}
	}
	else // Randomization
	{
		double high = sqrt( _size.col * _size.row ) / 1.5;
		double low = sqrt( _size.col * _size.row ) / 3;
		amountMonsters = RandomNumberGenerator( static_cast<int>( low ), static_cast<int>( high ) );
		_monsters.resize( amountMonsters, Monster( Vector2i( -1, -1 ), Portrait::Monster ) );
	}

	IO::OutputString( "\n" );
}

void Dungeon::BuildEntityData( )
{
	_entityData.clear( );
	_entityData.resize( _size.col * _size.row, nullptr );
}
void Dungeon::BuildHiddenData( )
{
	_hiddenData.clear( );
	_hiddenData.resize( _size.col * _size.row, nullptr );
}
void Dungeon::BuildVisionData( )
{
	_visionData.clear( );
	_visionData.resize( _size.col * _size.row, false );
}

void Dungeon::SetPlayer( )
{
	Vector2i position;

	position.col = RandomNumberGenerator( 1, _size.col - 2 );
	position.row = RandomNumberGenerator( 1, _size.row - 2 );

	_player.reset( new Player( position, Portrait::Player ) );
	UpdateEntityDataAt( position, _player.get( ) );
}
void Dungeon::SetExits( )
{
	Vector2i iterator;
	std::vector<Vector2i> positionValid;
	int exitAmount = 3;
	int randomIndex;

	for( iterator.row = 0; iterator.row < _size.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _size.col; iterator.col++ )
		{
			if( iterator.col == 0 ||
				iterator.row == 0 ||
				iterator.col == _size.col - 1 ||
				iterator.row == _size.row - 1 )
			{
				positionValid.push_back( iterator );
			}
		}
	}

	for( int i = 0; i < exitAmount; i++ )
	{
		randomIndex = RandomNumberGenerator( 0, positionValid.size( ) );
		_exits.emplace_back( positionValid[randomIndex], Portrait::Exit );
		UpdateEntityDataAt( positionValid[randomIndex], &_exits.back( ) );
		positionValid.erase( positionValid.begin( ) + randomIndex );
	}
}
void Dungeon::SetOuterWalls( )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _size.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _size.col; iterator.col++ )
		{
			if( GetEntityDataAt( iterator ) == nullptr )
			{
				if( iterator.col == 0 ||
					iterator.row == 0 ||
					iterator.col == _size.col - 1 ||
					iterator.row == _size.row - 1 )
				{
					_walls.emplace_back( iterator, Portrait::Wall );
					UpdateEntityDataAt( iterator, &_walls.back( ) );
				}
			}
		}
	}
}
void Dungeon::SetHiddenPath( )
{
	std::vector<Vector2i> positionWalls;

	for( const auto& wall : _walls )
	{
		positionWalls.push_back( wall.GetPosition( ) );
	}

	std::vector<Vector2i> positionPaths = AStarAlgorithm( _player->GetPosition( ), _exits.back( ).GetPosition( ), _size, positionWalls );
	
	for( const auto& path : positionPaths )
	{
		_paths.emplace_back( path, Portrait::Path );
		UpdateHiddenDataAt( path, &_paths.back( ) );
	}
}
void Dungeon::SetRandomSourceWalls( )
{
	Vector2i position;
	int sourceWallsLeft = ( _size.col * _size.row ) / 20;

	while( sourceWallsLeft > 0 )
	{
		position.col = RandomNumberGenerator( 1, _size.col - 2 );
		position.row = RandomNumberGenerator( 1, _size.row - 2 );

		if( GetEntityDataAt( position ) == nullptr &&
			GetHiddenDataAt( position ) == nullptr )
		{
			_walls.emplace_back( position, Portrait::Wall );
			UpdateEntityDataAt( position, &_walls.back( ) );
			sourceWallsLeft--;
		}
	}
}
void Dungeon::SetRandomExtensionWalls( )
{
	Vector2i position;
	int extensionWallsLeft = ( _size.col * _size.row ) / 4;

	while( extensionWallsLeft > 0 )
	{
		for( auto& wall : _walls )
		{
			position = wall.GetPosition( );

			if( position.col == 0 ||
				position.row == 0 ||
				position.col == _size.col - 1 ||
				position.row == _size.row - 1 )
			{
				continue;
			}

			position.col += RandomNumberGenerator( 0, 1 ) * RandomPositiveNegativeGenerator( );
			position.row += RandomNumberGenerator( 0, 1 ) * RandomPositiveNegativeGenerator( );

			if( GetEntityDataAt( position ) == nullptr &&
				GetHiddenDataAt( position ) == nullptr )
			{
				_walls.emplace_back( position, Portrait::Wall );
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

	for( iterator.row = 1; iterator.row < _size.row - 1; iterator.row++ )
	{
		for( iterator.col = 1; iterator.col < _size.col - 1; iterator.col++ )
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
					_walls.emplace_back( iterator, Portrait::Wall );
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
			position.col = RandomNumberGenerator( 1, _size.col - 2 );
			position.row = RandomNumberGenerator( 1, _size.row - 2 );

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
			IO::OutputClearScreen( );
			_state = GameState::Menu;
		}
	}
}
void Dungeon::PlayerMovement( Orientation orientation )
{
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
		_state = GameState::Lost;
	}

	UpdateEntityDataAt( _player->GetPosition( ), _player.get( ) );
}
void Dungeon::RandomMonsterMovement( )
{
	for( auto& monster : _monsters )
	{
		UpdateEntityDataAt( monster.GetPosition( ), nullptr );

		monster.MoveProbability( 1, 1, 1, 1, 12 ); // 25% to move, 75% to stand still.

		if( GetEntityDataAt( monster.GetPosition( ) ) != nullptr &&
			GetEntityDataAt( monster.GetPosition( ) ) != _player.get( ) )
		{
			monster.RevertPosition( );
		}
		else if( GetEntityDataAt( monster.GetPosition( ) ) != nullptr &&
				 GetEntityDataAt( monster.GetPosition( ) ) == _player.get( ) )
		{
			_state = GameState::Lost;
		}

		UpdateEntityDataAt( monster.GetPosition( ), &monster );
	}
}
bool Dungeon::CheckGameState( ) const
{

	if( _state == GameState::Won ||
		_state == GameState::Lost )
	{
		IO::OutputGameState( _state );
		IO::InputEnter( );
		
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