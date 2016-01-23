#include "Dungeon.h"
#include "AStarAlgorithm.h"
#include "Functions.h"
#include <math.h>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <string>
#include <map>
#include <iostream>

//void Dungeon::SaveDungeon( const std::string& fileName ) const
//{
//	std::ofstream outFile;
//
//	outFile.open( fileName, std::ios::out | std::ios::trunc );
//
//	if( !outFile.is_open( ) )
//	{
//		throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
//	}
//
//	WriteDungeonSize( outFile );
//
//	Output::String( "\nWriting tile icons." );
//	WriteTileIcons( outFile );
//	Output::String( "\nWriting vision." );
//	WriteVision( outFile );
//
//	outFile.close( );
//}
//void Dungeon::LoadDungeon( const std::string& fileName )
//{
//	std::ifstream inFile;
//
//	inFile.open( fileName, std::ios::in );
//
//	if( !inFile.is_open( ) )
//	{
//		throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
//	}
//
//	ReadDungeonSize( inFile );
//
//	ResizeTileMap( );
//	ResizeVisionMap( );
//
//	Output::String( "\nReading tile icons." );
//	ReadTileIcons( inFile );
//	Output::String( "\nReading vision." );
//	ReadVision( inFile );
//
//	inFile.close( );
//}

void Dungeon::GenerateDungeon( )
{
	_fixed = false;
	_seed = 0;

	SetDungeonSize( );
	SetDungeonContainers( );
	GenerateDoors( );
	GenerateOuterObstacles( );
	GeneratePath( );
	GenerateSourceObstacles( );
	GenerateExtensionObstacles( );
	GenerateFillerObstacles( );
	GenerateMonsters( );
}
void Dungeon::GenerateDungeon( const DungeonConfiguration& config )
{
	_fixed = config.fixedSeed;
	_seed = config.seed;

	SetDungeonSize( config.fixedDungeonSize, config.dungeonSize );
	SetDungeonContainers( );
	GenerateDoors( config.generateDoors, config.amountDoors );
	GenerateOuterObstacles( config.generateOuterObstacles );
	GeneratePath( config.generatePath );
	GenerateSourceObstacles( config.generateSourceObstacles, config.amountSourceObstacles );
	GenerateExtensionObstacles( config.generateExtensionObstacles, config.amountExtensionObstacles );
	GenerateFillerObstacles( config.generateFillerObstacles, config.amountFillerObstacleCycles );
	GenerateMonsters( config.generateMonsters, config.amountMonsters );
}

void Dungeon::PlayerAdd( Player* player, const Vector2i& position )
{
	_player = player;
	_player->SetPosition( position );
	OccupantAdd( _player );
}
void Dungeon::PlayerAdd( Player* player, Entity* door )
{
	static const std::array<Vector2i, 4> directions =
	{
		Vector2i( 0, -1 ),
		Vector2i( 1, 0 ),
		Vector2i( 0, 1 ),
		Vector2i( -1, 0 ),
	};

	for( const auto& direction : directions )
	{
		Vector2i position = door->GetPosition( ) + direction;

		if( InBounds( position ) &&
			Walkable( position ) )
		{
			_player = player;
			_player->SetPosition( position );
			OccupantAdd( _player );

			break;
		}
	}
}

void Dungeon::RotateDungeonClockwise( )
{
	auto tileRotated = _tileMap;
	auto visionRotated = _visionMap;
	Vector2i sizeOld;
	Vector2i sizeNew;
	Vector2i iterator;

	sizeOld = _dungeonSize;
	std::swap( _dungeonSize.col, _dungeonSize.row );
	sizeNew = _dungeonSize;

	for( iterator.row = 0; iterator.row < sizeNew.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < sizeNew.col; iterator.col++ )
		{
			tileRotated[( iterator.row * sizeNew.col ) + iterator.col] = _tileMap[( iterator.col * sizeOld.col ) + iterator.row];
			visionRotated[( iterator.row * sizeNew.col ) + iterator.col] = _visionMap[( iterator.col * sizeOld.col ) + iterator.row];
		}

		auto tileColoumBegin = tileRotated.begin( ) + iterator.row * sizeNew.col;
		auto tileColoumEnd   = tileRotated.begin( ) + iterator.row * sizeNew.col + sizeNew.col;
		auto visionColoumBegin = visionRotated.begin( ) + iterator.row * sizeNew.col;
		auto visionColoumEnd   = visionRotated.begin( ) + iterator.row * sizeNew.col + sizeNew.col;

		std::reverse( tileColoumBegin, tileColoumEnd );
		std::reverse( visionColoumBegin, visionColoumEnd );
	}

	_tileMap = tileRotated;
	_visionMap = visionRotated;
}
void Dungeon::RotateEntityClockwise( Entity* entity )
{
	Vector2i position;

	position.col = _dungeonSize.col - entity->GetPosition( ).row - 1;
	position.row = entity->GetPosition( ).col;

	if( InBounds( entity->GetPosition( ) ) )
	{
		OccupantRemove( entity );
	}

	entity->SetPosition( position );
	OccupantAdd( entity );
}
void Dungeon::UpdateEntityPositions( )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
		{
			auto& occupants = _tileMap[( iterator.row * _dungeonSize.col ) + iterator.col].occupants;

			for( auto& occupant : occupants )
			{
				occupant->SetPosition( iterator );
			}
		}
	}
}

void Dungeon::UpdatePlayerVision( )
{
	const Vector2i iteratorBegin = _player->GetPosition( ) - _player->GetLineOfSight( );
	const Vector2i iteratorEnd   = _player->GetPosition( ) + _player->GetLineOfSight( );
	Vector2i iterator;

	for( iterator.row = iteratorBegin.row; iterator.row <= iteratorEnd.row; iterator.row++ )
	{
		for( iterator.col = iteratorBegin.col; iterator.col <= iteratorEnd.col; iterator.col++ )
		{
			if( InBounds( iterator ) )
			{
				_visionMap[( iterator.row * _dungeonSize.col ) + iterator.col] = true;
			}
		}
	}
}
void Dungeon::PlayerMovement( const Orientation& orientation )
{
	OccupantRemove( _player );
	_player->Move( orientation );
	OccupantAdd( _player );
}
void Dungeon::MonsterMovement( )
{
	for( auto& monster : _monsters )
	{
		OccupantRemove( &monster );
		monster.MoveProbability( 1, 1, 1, 1, 12, _fixed, _seed ); /* 25% to move, 75% to stand still. */
		OccupantAdd( &monster );
	}
}
void Dungeon::HandleEvents( GameStatus& status )
{
	for( auto& monster : _monsters )
	{
		if( !InBounds( monster.GetPosition( ) ) )
		{
			monster.RevertPosition( );

			continue;
		}

		auto& occupants = _tileMap[( monster.GetPosition( ).row * _dungeonSize.col ) + monster.GetPosition( ).col].occupants;

		for( const auto& occupant : occupants )
		{
			switch( occupant->type )
			{
				case EntityType::Obstacle:
				case EntityType::Door:
				{
					OccupantRemove( &monster );
					monster.RevertPosition( );
					OccupantAdd( &monster );

					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	if( !InBounds( _player->GetPosition( ) ) )
	{
		_player->RevertPosition( );

		return;
	}

	auto& occupants = _tileMap[( _player->GetPosition( ).row * _dungeonSize.col ) + _player->GetPosition( ).col].occupants;

	for( auto& occupant : occupants )
	{
		switch( occupant->type )
		{
			case EntityType::Player:
			{
				break;
			}
			case EntityType::Obstacle:
			{
				OccupantRemove( _player );
				_player->RevertPosition( );
				OccupantAdd( _player );

				break;
			}
			case EntityType::Door:
			{
				status = GameStatus::Next;

				break;
			}
			case EntityType::Monster:
			{
				auto monster = static_cast<Monster*>( occupant );

				_player->Attack( monster );

				break;
			}
			default:
			{
				break;
			}
		}
	}
}
void Dungeon::RemoveDeadCharacters( GameStatus& status, bool safe )
{
	auto& it = _monsters.begin( );

	while( it != _monsters.end( ) )
	{
		if( it->GetAlive( ) == false )
		{
			if( safe )
			{
				OccupantRemove( &( *it ) );
			}

			it = _monsters.erase( it );
		}
		else
		{
			it++;
		}
	}

	if( _player->GetAlive( ) == false )
	{
		if( safe )
		{
			OccupantRemove( _player );
		}

		status = GameStatus::Dead;
	}
}

const std::vector<Entity*> Dungeon::GetDoors( ) const
{
	std::vector<Entity*> doors;
	
	for( auto entity : _entities )
	{
		if( entity.type == EntityType::Door )
		{
			doors.push_back( &entity );
		}
	}

	return doors;
}
const Vector2i& Dungeon::GetSize( ) const
{
	return _dungeonSize;
}
const Tile& Dungeon::GetTile( const Vector2i& position ) const
{
	return _tileMap[( position.row * _dungeonSize.col ) + position.col];
}
bool Dungeon::GetVision( const Vector2i& position ) const
{
	return _visionMap[( position.row * _dungeonSize.col ) + position.col];
}

bool Dungeon::Contains( const Vector2i& position, const EntityType& type ) const
{
	for( const auto& entity : GetTile( position ).occupants )
	{
		if( entity->type == type )
		{
			return true;
		}
	}

	return false;
}
bool Dungeon::Walkable( const Vector2i& position ) const
{
	return !( Contains( position, EntityType::Obstacle ) );
}
bool Dungeon::InBounds( const Vector2i& position ) const
{
	return
		position.col >= 0 &&
		position.row >= 0 &&
		position.col < _dungeonSize.col &&
		position.row < _dungeonSize.row;
}
bool Dungeon::Unoccupied( const Vector2i& position ) const
{
	return _tileMap[( position.row * _dungeonSize.col ) + position.col].occupants.empty( );
}
bool Dungeon::Surrounded( const Vector2i& position, int threshold ) const
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

		if( Contains( positionNeighbor, EntityType::Obstacle ) )
		{
			surroundingEntity++;
		}
	}

	return surroundingEntity >= threshold;
}

void Dungeon::UpdateVision( const Vector2i& position, int lineOfSight )
{
	const Vector2i iteratorBegin = position - lineOfSight;
	const Vector2i iteratorEnd   = position + lineOfSight;
	Vector2i iterator;

	for( iterator.row = iteratorBegin.row; iterator.row <= iteratorEnd.row; iterator.row++ )
	{
		for( iterator.col = iteratorBegin.col; iterator.col <= iteratorEnd.col; iterator.col++ )
		{
			if( InBounds( iterator ) )
			{
				_visionMap[( iterator.row * _dungeonSize.col ) + iterator.col] = true;
			}
		}
	}
}
void Dungeon::UpdateTile( const Vector2i& position )
{
	auto& tile = _tileMap[( position.row * _dungeonSize.col ) + position.col];
	char iconPriority = '-';

	for( const auto& occupant : tile.occupants )
	{
		switch( occupant->type )
		{
			case EntityType::Player:
			{
				iconPriority = Icon::Player;

				break;
			}
			case EntityType::Monster:
			{
				iconPriority = Icon::Monster;

				break;
			}
			case EntityType::Door:
			{
				iconPriority = Icon::Door;

				break;
			}
			case EntityType::Obstacle:
			{
				iconPriority = Icon::Obstacle;

				break;
			}
			default:
			{
				iconPriority = Icon::Ground;

				break;
			}
		}
	}

	tile.icon = iconPriority;
}
void Dungeon::OccupantAdd( Entity* entity )
{
	_tileMap[( entity->GetPosition( ).row * _dungeonSize.col ) + entity->GetPosition( ).col].occupants.push_back( entity );
	UpdateTile( entity->GetPosition( ) );
}
void Dungeon::OccupantRemove( Entity* entity )
{
	auto& occupants = _tileMap[( entity->GetPosition( ).row * _dungeonSize.col ) + entity->GetPosition( ).col].occupants;
	occupants.erase( std::remove( occupants.begin( ), occupants.end( ), entity ), occupants.end( ) );
	UpdateTile( entity->GetPosition( ) );
}

void Dungeon::SetDungeonSize( bool set, const Vector2i& size )
{
	_dungeonSize.col = set ? size.col : RandomNumberGenerator( 40, 80, _fixed, _seed );
	_dungeonSize.row = set ? size.row : RandomNumberGenerator( 40, 80, _fixed, _seed );
}
void Dungeon::SetDungeonContainers( )
{
	_monsters.clear( );
	_entities.clear( );

	_tileMap.clear( );
	_visionMap.clear( );

	_tileMap.resize( _dungeonSize.col * _dungeonSize.row );
	_visionMap.resize( _dungeonSize.col * _dungeonSize.row, false );
}
void Dungeon::GenerateDoors( bool generate, int amount )
{
	if( generate )
	{
		int amountDoors = amount ? amount : 2;
		Vector2i iterator;
		std::vector<Vector2i> positionValid;
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

		for( int i = 0; i < amountDoors; i++ )
		{
			index = RandomNumberGenerator( 0, positionValid.size( ) - 1, _fixed, _seed );
			_entities.emplace_back( positionValid[index], EntityType::Door );
			OccupantAdd( &_entities.back( ) );
			positionValid.erase( positionValid.begin( ) + index );
		}
	}
}
void Dungeon::GenerateOuterObstacles( bool generate )
{
	if( generate )
	{
		Vector2i iterator;

		for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
		{
			for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
			{
				if( Unoccupied( iterator ) )
				{
					if( iterator.col == 0 ||
						iterator.row == 0 ||
						iterator.col == _dungeonSize.col - 1 ||
						iterator.row == _dungeonSize.row - 1 )
					{
						_entities.emplace_back( iterator, EntityType::Obstacle );
						OccupantAdd( &_entities.back( ) );
					}
				}
			}
		}
	}
}
void Dungeon::GeneratePath( bool generate )
{
	if( generate )
	{
		const Vector2i positionCenter = _dungeonSize / 2;
		std::vector<Vector2i> pathToDoor;
		std::vector<Vector2i> obstacles;

		for( const auto& entity : _entities )
		{
			if( entity.type == EntityType::Obstacle )
			{
				obstacles.push_back( entity.GetPosition( ) );
			}
		}

		for( const auto& entity : _entities )
		{
			if( entity.type == EntityType::Door )
			{
				pathToDoor = AStarAlgorithm( entity.GetPosition( ), positionCenter, _dungeonSize, obstacles );

				for( const auto& position : pathToDoor )
				{
					if( Unoccupied( position ) )
					{
						_entities.emplace_back( position, EntityType::Path );
						OccupantAdd( &_entities.back( ) );
					}
				}
			}
		}
	}
}
void Dungeon::GenerateSourceObstacles( bool generate, int amount )
{
	if( generate )
	{
		int sourceWallsLeft = amount ? amount : ( _dungeonSize.col * _dungeonSize.row ) / 20;
		Vector2i position;

		while( sourceWallsLeft > 0 )
		{
			position.col = RandomNumberGenerator( 1, _dungeonSize.col - 2, _fixed, _seed );
			position.row = RandomNumberGenerator( 1, _dungeonSize.row - 2, _fixed, _seed );

			if( Unoccupied( position ) &&
				position != _dungeonSize / 2 )
			{
				_entities.emplace_back( position, EntityType::Obstacle );
				OccupantAdd( &_entities.back( ) );
				sourceWallsLeft--;
			}
		}
	}
}
void Dungeon::GenerateExtensionObstacles( bool generate, int amount )
{
	static const std::array<Vector2i, 4> directions =
	{
		Vector2i( 0, -1 ),
		Vector2i( 1, 0 ),
		Vector2i( 0, 1 ),
		Vector2i( -1, 0 ),
	};

	if( generate )
	{
		int extensionWallsLeft = amount ? amount : ( _dungeonSize.col * _dungeonSize.row ) / 4;
		int index;
		Vector2i position;

		while( extensionWallsLeft > 0 )
		{
			for( const auto& entity : _entities )
			{
				index = RandomNumberGenerator( 0, 3, _fixed, _seed );
				position = entity.GetPosition( ) + directions[index];

				if( InBounds( position ) &&
					Unoccupied( position ) &&
					position != _dungeonSize / 2 )
				{
					_entities.emplace_back( position, EntityType::Obstacle );
					OccupantAdd( &_entities.back( ) );
					extensionWallsLeft--;
				}
			}
		}
	}
}
void Dungeon::GenerateFillerObstacles( bool generate, int amount )
{
	const int amountWalls = amount ? amount : 5;
	Vector2i iterator;
	
	for( int i = 0; i < amountWalls; i++ )
	{
		for( iterator.row = 1; iterator.row < _dungeonSize.row - 1; iterator.row++ )
		{
			for( iterator.col = 1; iterator.col < _dungeonSize.col - 1; iterator.col++ )
			{
				if( Unoccupied( iterator ) &&
					Surrounded( iterator, 5 ) &&
					iterator != _dungeonSize / 2 )
				{
					_entities.emplace_back( iterator, EntityType::Obstacle );
					OccupantAdd( &_entities.back( ) );
				}
			}
		}
	}
}
void Dungeon::GenerateMonsters( bool generate, int amount )
{
	if( generate )
	{
		const int low = static_cast<int>( sqrt( _dungeonSize.col * _dungeonSize.row ) / 3.0 );
		const int high = static_cast<int>( sqrt( _dungeonSize.col * _dungeonSize.row ) / 1.5 );
		const int amountMonsters = amount ? amount : RandomNumberGenerator( low, high, _fixed, _seed );
		Vector2i position;

		for( int i = 0; i < amountMonsters; i++ )
		{
			while( true )
			{
				position.col = RandomNumberGenerator( 1, _dungeonSize.col - 2, _fixed, _seed );
				position.row = RandomNumberGenerator( 1, _dungeonSize.row - 2, _fixed, _seed );

				if( Walkable( position ) &&
					position != _dungeonSize / 2 )
				{
					_monsters.emplace_back( position );
					OccupantAdd( &_monsters.back( ) );

					break;
				}
			}
		}
	}
}

//void Dungeon::WriteDungeonSize( std::ofstream& stream ) const
//{
//	stream << _dungeonSize.col << '\n';
//	stream << _dungeonSize.row << '\n';
//}
//void Dungeon::WriteTileIcons( std::ofstream& stream ) const
//{
//	Vector2i iterator;
//
//	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
//	{
//		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
//		{
//			if( iterator == _player->GetPosition( ) )
//			{
//				stream << Icon::Player;
//			}
//			else if( Contains( iterator, EntityType::Path ) )
//			{
//				stream << Icon::Path;
//			}
//			else
//			{
//				stream << GetTile( iterator ).icon;
//			}
//		}
//
//		stream << '\n';
//	}
//}
//void Dungeon::WriteVision( std::ofstream& stream ) const
//{
//	Vector2i iterator;
//
//	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
//	{
//		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
//		{
//			stream << GetVision( iterator );
//		}
//
//		stream << '\n';
//	}
//}
//
//void Dungeon::ReadDungeonSize( std::ifstream& stream )
//{
//	std::string line;
//
//	try
//	{
//		std::getline( stream, line );
//		_dungeonSize.col = std::stoi( line );
//
//		std::getline( stream, line );
//		_dungeonSize.row = std::stoi( line );
//	}
//	catch( ... )
//	{
//		throw std::exception( std::string( "Could not read dungeon size" ).c_str( ) );
//	}
//}
//void Dungeon::ReadTileIcons( std::ifstream& stream )
//{
//	std::string line;
//	Vector2i iterator;
//
//	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
//	{
//		std::getline( stream, line );
//
//		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
//		{
//			switch( line[iterator.col] )
//			{
//				case Icon::Player:
//				{
//					_player.reset( new Player( iterator, 100.0f, 0.10f, 50.0f, 100.0f, 100.0f, 3 ) );
//					OccupantAdd( iterator, _player.get( ) );
//
//					break;
//				}
//				case Icon::Monster:
//				{
//					_monsters.emplace_back( iterator );
//					OccupantAdd( iterator, &_monsters.back( ) );
//
//					break;
//				}
//				case Icon::Door:
//				{
//					doors.emplace_back( iterator );
//					OccupantAdd( iterator, &doors.back( ) );
//
//					break;
//				}
//				case Icon::Path:
//				{
//					_path.emplace_back( iterator, EntityType::Path );
//					OccupantAdd( iterator, &_path.back( ) );
//
//					break;
//				}
//				case Icon::Obstacle:
//				{
//					_obstacles.emplace_back( iterator, EntityType::Obstacle );
//					OccupantAdd( iterator, &_obstacles.back( ) );
//
//					break;
//				}
//				case Icon::Ground:
//				{
//					break;
//				}
//				default:
//				{
//					throw std::exception( std::string( "Could not read tile icons" ).c_str( ) );
//				}
//			}
//		}
//	}
//}
//void Dungeon::ReadVision( std::ifstream& stream )
//{
//	std::string line;
//	Vector2i iterator;
//
//	for( iterator.row = 0; iterator.row < _dungeonSize.row; iterator.row++ )
//	{
//		std::getline( stream, line );
//
//		for( iterator.col = 0; iterator.col < _dungeonSize.col; iterator.col++ )
//		{
//			switch( line[iterator.col] )
//			{
//				case '1':
//				{
//					SetVision( iterator, true );
//
//					break;
//				}
//				case '0':
//				{
//					SetVision( iterator, false );
//
//					break;
//				}
//				default:
//				{
//					throw std::exception( std::string( "Could not read vision" ).c_str( ) );
//				}
//			}
//		}
//	}
//}