#include "Dungeon.h"
#include "AStarAlgorithm.h"
#include "IO.h"
#include "Functions.h"
#include <math.h>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>

Dungeon::Dungeon( ) :
	_status( GameStatus::Neutral )
{ }

void Dungeon::BuildDungeon( const GameType& type )
{
	SetDungeonSize( type );
	SetSpawnMonsters( type );

	ResizeEntityData( );
	ResizeHiddenData( );
	ResizeVisionData( );

	SetPlayer( );
	SetRandomDoors( );
	SetOuterWalls( );
	Output::String( "\nLoading path." );
	SetHiddenSteps( );
	Output::String( "\nLoading source walls." );
	SetRandomSourceWalls( );
	Output::String( "\nLoading extension walls." );
	SetRandomExtensionWalls( );
	Output::String( "\nLoading filler walls." );
	SetFillerWalls( );
	Output::String( "\nLoading monster positions." );
	SetRandomMonsters( );
}
void Dungeon::SaveDungeon( const std::string& fileName ) const
{
	std::ofstream outFile;

	outFile.open( fileName, std::ios::out | std::ios::trunc );

	if( !outFile.is_open( ) )
	{
		throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
	}

	WriteDungeonSize( outFile );

	Output::String( "\nWriting entity data." );
	WriteEntityData( outFile );
	Output::String( "\nWriting hidden data." );
	WriteHiddenData( outFile );
	Output::String( "\nWriting vision data." );
	WriteVisionData( outFile );

	outFile.close( );
}
void Dungeon::LoadDungeon( const std::string& fileName )
{
	std::ifstream inFile;

	inFile.open( fileName, std::ios::in );

	if( !inFile.is_open( ) )
	{
		throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
	}

	ReadDungeonSize( inFile );

	ResizeEntityData( );
	ResizeHiddenData( );
	ResizeVisionData( );

	Output::String( "\nReading entity data." );
	ReadEntityData( inFile );
	Output::String( "\nReading hidden data." );
	ReadHiddenData( inFile );
	Output::String( "\nReading vision data." );
	ReadVisionData( inFile );

	inFile.close( );
}
void Dungeon::GameLoop( )
{
	while( true )
	{
		RandomMonsterMovement( );
		UpdateCharacters( );

		if( !CheckGameStatus( ) )
		{
			break;
		}

		UpdateVisionDataAt( _player->GetPosition( ), _player->GetLineOfSight( ) );
		Output::ClearScreen( );
		Output::DungeonCentered( *this, _dungeonSize, _player->GetPosition( ) );
		Output::PlayerStatus( *_player );
		Output::TurnOptions( );
		PlayerTurn( );
		UpdateCharacters( );
	}

	Output::String( "\n\nDungeon destructor might take some time." );
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

bool Dungeon::InBounds( const Vector2i& position ) const
{
	return
		position.col >= 0 &&
		position.row >= 0 &&
		position.col < _dungeonSize.col &&
		position.row < _dungeonSize.row;
}
bool Dungeon::PositionSurrounded( const Vector2i& position, int threshold ) const
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
	int surroundingEntity = 0;
	Vector2i positionNeighbor;

	for( const auto& direction : directions )
	{
		positionNeighbor = position + direction;

		if( GetEntityDataAt( positionNeighbor ) != nullptr )
		{
			surroundingEntity++;
		}
	}

	return surroundingEntity >= threshold;
}

void Dungeon::SetEntityDataAt( const Vector2i& position, Entity* entity )
{
	_entityData[( position.row * _dungeonSize.col ) + position.col] = entity;
}
void Dungeon::SetHiddenDataAt( const Vector2i& position, Entity* entity )
{
	_hiddenData[( position.row * _dungeonSize.col ) + position.col] = entity;
}
void Dungeon::SetVisionDataAt( const Vector2i& position, bool vision )
{
	_visionData[( position.row * _dungeonSize.col ) + position.col] = vision;
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

void Dungeon::UpdatePositions( std::vector<Entity*>& arrayCurrent )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			if( arrayCurrent[( iterator.row * _dungeonSize.col ) + iterator.col] != nullptr )
			{
				arrayCurrent[( iterator.row * _dungeonSize.col ) + iterator.col]->SetPosition( iterator );
			}
		}
	}
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
			Output::String( "\n\nSomething went wrong in Dungeon::SetDungeonSize." );
			Output::String( "\nPress enter to continue: " );
			Input::Enter( );

			break;
		}
	}
}
void Dungeon::SetSpawnMonsters( const GameType& type )
{
	switch( type )
	{
		case GameType::Randomized:
		{
			_spawnMonsters = true;

			break;
		}
		case GameType::MinorConfiguration:
		{
			const std::vector<char> choices { 'Y', 'y', 'N', 'n' };
			const char choice = Input::ValidChar( "Spawn monsters [Y/N]: ", choices );

			switch( choice )
			{
				case 'Y':
				case 'y':
				{
					_spawnMonsters = true;

					break;
				}
				case 'N':
				case 'n':
				{
					_spawnMonsters = false;

					break;
				}
				default:
				{
					Output::String( "\n\nSomething went wrong in Dungeon::SetSpawnMonsters." );
					Output::String( "\nPress enter to continue: " );
					Input::Enter( );

					break;
				}
			}

			break;
		}
		default:
		{
			Output::String( "\n\nSomething went wrong in Dungeon::SetSpawnMonsters." );
			Output::String( "\nPress enter to continue: " );
			Input::Enter( );

			break;
		}
	}
}

void Dungeon::ResizeEntityData( )
{
	_entityData.clear( );
	_entityData.resize( _dungeonSize.col * _dungeonSize.row, nullptr );
}
void Dungeon::ResizeHiddenData( )
{
	_hiddenData.clear( );
	_hiddenData.resize( _dungeonSize.col * _dungeonSize.row, nullptr );
}
void Dungeon::ResizeVisionData( )
{
	_visionData.clear( );
	_visionData.resize( _dungeonSize.col * _dungeonSize.row, false );
}

void Dungeon::SetPlayer( )
{
	Vector2i position;

	position.col = RandomNumberGenerator( 1, _dungeonSize.col - 2 );
	position.row = RandomNumberGenerator( 1, _dungeonSize.row - 2 );

	_player.reset( new Player( position, 100.0f, 0.10f, 50.0f, 100.0f, 100.0f, 3 ) );
	SetEntityDataAt( position, _player.get( ) );
}
void Dungeon::SetRandomDoors( )
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
		_doors.emplace_back( positionValid[index] );
		SetEntityDataAt( positionValid[index], &_doors.back( ) );
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
					SetEntityDataAt( iterator, &_walls.back( ) );
				}
			}
		}
	}
}
void Dungeon::SetHiddenSteps( )
{
	const Vector2i positionCenter = _dungeonSize / 2;
	std::vector<Vector2i> positionWalls;
	std::vector<Vector2i> pathToCenter;

	std::for_each( _walls.begin( ), _walls.end( ), [&positionWalls]( const Wall& wall ){ positionWalls.push_back( wall.GetPosition( ) ); } );

	pathToCenter = AStarAlgorithm( _player->GetPosition( ), positionCenter, _dungeonSize, positionWalls );

	for( const auto& position : pathToCenter )
	{
		if( GetHiddenDataAt( position ) == nullptr )
		{
			_steps.emplace_back( position );
			SetHiddenDataAt( position, &_steps.back( ) );
		}
	}

	for( const auto& door : _doors )
	{
		std::vector<Vector2i> pathToDoor( AStarAlgorithm( positionCenter, door.GetPosition( ), _dungeonSize, positionWalls ) );

		for( const auto& position : pathToDoor )
		{
			if( GetHiddenDataAt( position ) == nullptr )
			{
				_steps.emplace_back( position );
				SetHiddenDataAt( position, &_steps.back( ) );
			}
		}
	}
}
void Dungeon::SetRandomSourceWalls( )
{
	int sourceWallsLeft = ( _dungeonSize.col * _dungeonSize.row ) / 20;
	Vector2i position;

	while( sourceWallsLeft > 0 )
	{
		position.col = RandomNumberGenerator( 1, _dungeonSize.col - 2 );
		position.row = RandomNumberGenerator( 1, _dungeonSize.row - 2 );

		if( GetEntityDataAt( position ) == nullptr &&
			GetHiddenDataAt( position ) == nullptr )
		{
			_walls.emplace_back( position );
			SetEntityDataAt( position, &_walls.back( ) );
			sourceWallsLeft--;
		}
	}
}
void Dungeon::SetRandomExtensionWalls( )
{
	const std::array<Vector2i, 4> directions =
	{
		Vector2i( 0, -1 ),
		Vector2i( 1, 0 ),
		Vector2i( 0, 1 ),
		Vector2i( -1, 0 ),
	};
	int extensionWallsLeft = ( _dungeonSize.col * _dungeonSize.row ) / 4;
	int index;
	Vector2i positionNeighbor;

	while( extensionWallsLeft > 0 )
	{
		for( auto& wall : _walls )
		{
			index = RandomNumberGenerator( 0, 3 );
			positionNeighbor = wall.GetPosition( ) + directions[index];

			if( InBounds( positionNeighbor ) &&
				GetEntityDataAt( positionNeighbor ) == nullptr &&
				GetHiddenDataAt( positionNeighbor ) == nullptr )
			{
				_walls.emplace_back( positionNeighbor );
				SetEntityDataAt( positionNeighbor, &_walls.back( ) );
				extensionWallsLeft--;
			}
		}
	}
}
void Dungeon::SetFillerWalls( )
{
	const int amount = 5;
	Vector2i iterator;
	
	for( int i = 0; i < amount; i++ )
	{
		for( iterator.row = 1; iterator.row < _dungeonSize.row - 1; iterator.row++ )
		{
			for( iterator.col = 1; iterator.col < _dungeonSize.col - 1; iterator.col++ )
			{
				if( GetEntityDataAt( iterator ) == nullptr &&
					GetHiddenDataAt( iterator ) == nullptr &&
					PositionSurrounded( iterator, 5 ) )
				{
					_walls.emplace_back( iterator );
					SetEntityDataAt( iterator, &_walls.back( ) );
				}
			}
		}
	}
}
void Dungeon::SetRandomMonsters( )
{
	if( _spawnMonsters )
	{
		const int low = static_cast<int>( sqrt( _dungeonSize.col * _dungeonSize.row ) / 3.0 );
		const int high = static_cast<int>( sqrt( _dungeonSize.col * _dungeonSize.row ) / 1.5 );
		const int amountMonsters = RandomNumberGenerator( low, high );
		Vector2i position;

		for( int i = 0; i < amountMonsters; i++ )
		{
			while( true )
			{
				position.col = RandomNumberGenerator( 1, _dungeonSize.col - 2 );
				position.row = RandomNumberGenerator( 1, _dungeonSize.row - 2 );

				if( GetEntityDataAt( position ) == nullptr )
				{
					_monsters.emplace_back( position );
					SetEntityDataAt( position, &_monsters.back( ) );

					break;
				}
			}
		}
	}
}

void Dungeon::WriteDungeonSize( std::ofstream& stream ) const
{
	stream << _dungeonSize.col << '\n';
	stream << _dungeonSize.row << '\n';
}
void Dungeon::WriteEntityData( std::ofstream& stream ) const
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			if( GetEntityDataAt( iterator ) != nullptr )
			{
				stream << GetEntityDataAt( iterator )->portrait;
			}
			else
			{
				stream << Portrait::Ground;
			}
		}

		stream << '\n';
	}
}
void Dungeon::WriteHiddenData( std::ofstream& stream ) const
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			if( GetHiddenDataAt( iterator ) != nullptr )
			{
				stream << GetHiddenDataAt( iterator )->portrait;
			}
			else
			{
				stream << Portrait::Ground;
			}
		}

		stream << '\n';
	}
}
void Dungeon::WriteVisionData( std::ofstream& stream ) const
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			stream << GetVisionDataAt( iterator );
		}

		stream << '\n';
	}
}

void Dungeon::ReadDungeonSize( std::ifstream& stream )
{
	std::string line;

	try
	{
		std::getline( stream, line );
		_dungeonSize.col = std::stoi( line );

		std::getline( stream, line );
		_dungeonSize.row = std::stoi( line );
	}
	catch( ... )
	{
		throw std::exception( std::string( "Could not read dungeon size" ).c_str( ) );
	}
}
void Dungeon::ReadEntityData( std::ifstream& stream )
{
	std::string line;
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		std::getline( stream, line );

		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			switch( line[iterator.col] )
			{
				case Portrait::Wall:
				{
					_walls.emplace_back( iterator );
					SetEntityDataAt( iterator, &_walls.back( ) );

					break;
				}
				case Portrait::Door:
				{
					_doors.emplace_back( iterator );
					SetEntityDataAt( iterator, &_doors.back( ) );

					break;
				}
				case Portrait::Monster:
				{
					_monsters.emplace_back( iterator );
					SetEntityDataAt( iterator, &_monsters.back( ) );

					break;
				}
				case Portrait::Player:
				{
					_player.reset( new Player( iterator, 100.0f, 0.10f, 50.0f, 100.0f, 100.0f, 3 ) );
					SetEntityDataAt( iterator, _player.get( ) );

					break;
				}
				case Portrait::Ground:
				{
					SetEntityDataAt( iterator, nullptr );

					break;
				}
				default:
				{
					throw std::exception( std::string( "Could not read entity data" ).c_str( ) );
				}
			}
		}
	}
}
void Dungeon::ReadHiddenData( std::ifstream& stream )
{
	std::string line;
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		std::getline( stream, line );

		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			switch( line[iterator.col] )
			{
				case Portrait::Step:
				{
					_steps.emplace_back( iterator );
					SetHiddenDataAt( iterator, &_steps.back( ) );

					break;
				}
				case Portrait::Ground:
				{
					SetHiddenDataAt( iterator, nullptr );

					break;
				}
				default:
				{
					throw std::exception( std::string( "Could not read hidden data" ).c_str( ) );
				}	
			}
		}
	}
}
void Dungeon::ReadVisionData( std::ifstream& stream )
{
	std::string line;
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		std::getline( stream, line );

		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			switch( line[iterator.col] )
			{
				case '1':
				{
					SetVisionDataAt( iterator, true );

					break;
				}
				case '0':
				{
					SetVisionDataAt( iterator, false );

					break;
				}
				default:
				{
					throw std::exception( std::string( "Could not read vision data" ).c_str( ) );
				}
			}
		}
	}
}

void Dungeon::PlayerTurn( )
{
	const std::vector<char> playerTurnChoices { 'W', 'w', 'A', 'a', 'S', 's', 'D', 'd', 'Q', 'q', 'E', 'e', 'F', 'f' };
	const char choice = Input::ValidChar( "\nYour choice: ", playerTurnChoices );

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
			try
			{
				SaveDungeon( "dungeonSave.txt" );
			}
			catch( const std::exception& e )
			{
				Output::String( "\n\nSomething went wrong in Dungeon::PlayerTurn." );
				Output::String( "\nReason: " );
				Output::String( e.what( ) );
				Output::String( "\nPress enter to continue: " );
				Input::Enter( );
			}

			_status = GameStatus::Menu;

			break;
		}
		case 'F':
		case 'f':
		{
			Vector2i dungeonSizeOld = _dungeonSize;

			std::swap( _dungeonSize.col, _dungeonSize.row );
			
			_entityData = TransposeArray1D( _entityData, dungeonSizeOld, _dungeonSize );
			_hiddenData = TransposeArray1D( _hiddenData, dungeonSizeOld, _dungeonSize );
			_visionData = TransposeArray1D( _visionData, dungeonSizeOld, _dungeonSize );

			_entityData = ReverseColoumsArray1D( _entityData, _dungeonSize );
			_hiddenData = ReverseColoumsArray1D( _hiddenData, _dungeonSize );
			_visionData = ReverseColoumsArray1D( _visionData, _dungeonSize );

			UpdatePositions( _entityData );
			UpdatePositions( _hiddenData );

			break;
		}
		default:
		{
			Output::String( "\n\nSomething went wrong in Dungeon::PlayerTurn." );
			Output::String( "\nPress enter to continue: " );
			Input::Enter( );

			break;
		}
	}
}
void Dungeon::PlayerMovement( const Orientation& orientation )
{
	/* If player is alive, it will be re-entered into _entityData in Dungeon::UpdateCharacters( ) */
	SetEntityDataAt( _player->GetPosition( ), nullptr );

	_player->Move( orientation );

	auto entityCached = GetEntityDataAt( _player->GetPosition( ) );

	if( entityCached != nullptr )
	{
		switch( entityCached->portrait )
		{
			case Portrait::Wall:
			{
				_player->RevertPosition( );

				break;
			}
			case Portrait::Door:
			{
				_status = GameStatus::Won;
				
				break;
			}
			case Portrait::Monster:
			{
				/*
					const_cast is fine here because the object that the pointer points to is mutable (a monster in this case).
					While I try to be as const correct as possible, this is only one time I need to access and modify a monster directly from a const pointer.
					My only other option would be to search all monsters for the same position as the entity pointer, which would be inefficient.
				*/

				auto entity = const_cast<Entity*>( GetEntityDataAt( _player->GetPosition( ) ) );
				auto monster = static_cast<Monster*>( entity );
				_player->Attack( monster );

				break;
			}
			default:
			{
				break;
			}
		}
	}

	SetEntityDataAt( _player->GetPosition( ), _player.get( ) );
}
void Dungeon::RandomMonsterMovement( )
{
	for( auto& monster : _monsters )
	{
		SetEntityDataAt( monster.GetPosition( ), nullptr );

		if( Heuristic( monster.GetPosition( ), _player->GetPosition( ) ) < 3 )
		{
			monster.MoveTowards( _player->GetPosition( ) );
		}
		else
		{
			monster.MoveProbability( 1, 1, 1, 1, 12 ); /* 25% to move, 75% to stand still. */
		}

		auto entityCached = GetEntityDataAt( monster.GetPosition( ) );

		if( entityCached != nullptr &&
			entityCached != _player.get( ) )
		{
			monster.RevertPosition( );
		}
		else if( entityCached != nullptr &&
				 entityCached == _player.get( ) )
		{
			monster.Attack( _player.get( ) );
		}

		SetEntityDataAt( monster.GetPosition( ), &monster );
	}
}
void Dungeon::UpdateCharacters( )
{
	auto it = _monsters.begin( );

	while( it != _monsters.end( ) )
	{
		if( it->GetAlive( ) == false )
		{
			it = _monsters.erase( it );
		}
		else
		{
			SetEntityDataAt( it->GetPosition( ), &( *it ) );
			it++;
		}
	}

	if( _player->GetAlive( ) == false )
	{
		_status = GameStatus::Lost;
	}
	else
	{
		SetEntityDataAt( _player->GetPosition( ), _player.get( ) );
	}
}
bool Dungeon::CheckGameStatus( ) const
{

	if( _status == GameStatus::Won ||
		_status == GameStatus::Lost )
	{
		Output::ClearScreen( );
		Output::GameStatusEnd( _status );
		Input::Enter( );
		
		return false;
	}
	else if( _status == GameStatus::Menu )
	{
		return false;
	}
	else
	{
		return true;
	}
}