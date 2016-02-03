#include "Dungeon.h"
#include "AStarAlgorithm.h"
#include <math.h>
#include <algorithm>
#include <iostream>

Dungeon::Dungeon( Player* player, const DungeonConfiguration& config ) :
	_player( player ),
	_maxCol( config.fixedDungeonSize ? config.maxCol : RandomNumberGenerator( 50, 60 ) ),
	_maxRow( config.fixedDungeonSize ? config.maxRow : RandomNumberGenerator( 50, 60 ) ),
	_tileMap( _maxCol * _maxRow ),
	_visionMap( _maxCol * _maxRow, false )

{
	GenerateDoors( config.generateDoors, config.amountDoors );
	GenerateOuterObstacles( config.generateOuterObstacles );
	GeneratePath( config.generatePath );
	GenerateSourceObstacles( config.generateSourceObstacles, config.amountSourceObstacles );
	GenerateExtensionObstacles( config.generateExtensionObstacles, config.amountExtensionObstacles );
	GenerateFillerObstacles( config.generateFillerObstacles, config.amountFillerObstacleCycles );
	GenerateMonsters( config.generateMonsters, config.amountMonsters );
}
Dungeon::Dungeon( Player* player, int maxCol, int maxRow, const std::vector<bool>& visionMap, const std::vector<char>& iconMap ) :
	_player( player ),
	_maxCol( maxCol ),
	_maxRow( maxRow ),
	_tileMap( _maxCol * _maxRow ),
	_visionMap( visionMap )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
		{
			switch( iconMap[( iterator.row * _maxCol ) + iterator.col] )
			{
				case Icon::Player:
				{
					_player->SetPosition( iterator );
					OccupantAdd( _player );

					break;
				}
				case Icon::Monster:
				{
					_monsters.emplace_back( iterator );
					OccupantAdd( &_monsters.back( ) );

					break;
				}
				case Icon::Door:
				{
					_entities.emplace_back( iterator, EntityType::Door );
					OccupantAdd( &_entities.back( ) );

					break;
				}
				case Icon::Obstacle:
				{
					_entities.emplace_back( iterator, EntityType::Obstacle );
					OccupantAdd( &_entities.back( ) );

					break;
				}
			}
		}
	}
}

void Dungeon::PlayerInitialPlace( const Vector2i& position )
{
	_player->SetPosition( position );
	OccupantAdd( _player );
}
void Dungeon::PlayerInitialPlaceNearby( const Vector2i& position )
{
	const std::array<Vector2i, 4> directions =
	{
		Vector2i(  0, -1 ),
		Vector2i(  1,  0 ),
		Vector2i(  0,  1 ),
		Vector2i( -1,  0 ),
	};

	for( const auto& direction : directions )
	{
		Vector2i positionNearby = position + direction;

		if( InBounds( positionNearby ) &&
			Walkable( positionNearby ) )
		{
			_player->SetPosition( positionNearby );
			OccupantAdd( _player );

			break;
		}
	}
}

void Dungeon::RotateDungeonClockwise( )
{
	auto tileRotated = _tileMap;
	auto visionRotated = _visionMap;
	Vector2i iterator;

	std::swap( _maxCol, _maxRow );

	for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
		{
			tileRotated[( iterator.row * _maxCol ) + iterator.col] = _tileMap[( iterator.col * _maxRow ) + iterator.row];
			visionRotated[( iterator.row * _maxCol ) + iterator.col] = _visionMap[( iterator.col * _maxRow ) + iterator.row];
		}

		auto tileColoumBegin = tileRotated.begin( ) + iterator.row * _maxCol;
		auto tileColoumEnd   = tileRotated.begin( ) + iterator.row * _maxCol + _maxCol;
		auto visionColoumBegin = visionRotated.begin( ) + iterator.row * _maxCol;
		auto visionColoumEnd   = visionRotated.begin( ) + iterator.row * _maxCol + _maxCol;

		std::reverse( tileColoumBegin, tileColoumEnd );
		std::reverse( visionColoumBegin, visionColoumEnd );
	}

	_tileMap = tileRotated;
	_visionMap = visionRotated;
}
void Dungeon::UpdateEntityPositions( )
{
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
		{
			auto& occupants = _tileMap[( iterator.row * _maxCol ) + iterator.col].occupants;

			for( auto& occupant : occupants )
			{
				occupant->SetPosition( iterator );
			}
		}
	}
}

void Dungeon::UpdatePlayerVision( )
{
	UpdateVision( _player->GetPosition( ), _player->GetVisionReach( ) );
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
		monster.MoveProbability( 1, 1, 1, 1, 12 ); /* 25% to move, 75% to stand still. */
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

		auto& occupants = _tileMap[( monster.GetPosition( ).row * _maxCol ) + monster.GetPosition( ).col].occupants;

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

	auto& occupants = _tileMap[( _player->GetPosition( ).row * _maxCol ) + _player->GetPosition( ).col].occupants;

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
				OccupantRemove( _player );
				status = GameStatus::Next;

				break;
			}
			case EntityType::Monster:
			{
				auto monster = static_cast<Monster*>( occupant );

				_player->Attack( monster );

				if( !_player->GetAlive( ) )
				{
					status = GameStatus::Menu;
				}

				break;
			}
			default:
			{
				break;
			}
		}
	}
}
void Dungeon::RemoveDeadCharacters( bool safe )
{
	auto it = _monsters.begin( );

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
	}
}

const std::vector<const Entity*> Dungeon::GetDoors( ) const
{
	std::vector<const Entity*> doors;
	
	for( const auto& entity : _entities )
	{
		if( entity.type == EntityType::Door )
		{
			doors.push_back( &entity );
		}
	}

	return doors;
}
const std::pair<int, int> Dungeon::GetSize( ) const
{
	return std::make_pair( _maxCol, _maxRow );
}
const Tile& Dungeon::GetTile( const Vector2i& position ) const
{
	return _tileMap[( position.row * _maxCol ) + position.col];
}
bool Dungeon::GetVision( const Vector2i& position ) const
{
	return _visionMap[( position.row * _maxCol ) + position.col];
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
		position.col < _maxCol &&
		position.row < _maxRow;
}
bool Dungeon::IsCorner( const Vector2i& position ) const
{
	return
		( position.col == 0 || position.col == _maxCol - 1 ) &&
		( position.row == 0 || position.row == _maxRow - 1 );
}
bool Dungeon::Unoccupied( const Vector2i& position ) const
{
	return _tileMap[( position.row * _maxCol ) + position.col].occupants.empty( );
}
bool Dungeon::Surrounded( const Vector2i& position, int threshold ) const
{
	const std::array<Vector2i, 8> directions =
	{
		Vector2i(  0, -1 ),
		Vector2i(  1, -1 ),
		Vector2i(  1,  0 ),
		Vector2i(  1,  1 ),
		Vector2i(  0,  1 ),
		Vector2i( -1,  1 ),
		Vector2i( -1,  0 ),
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
				_visionMap[( iterator.row * _maxCol ) + iterator.col] = true;
			}
		}
	}
}
void Dungeon::UpdateTile( const Vector2i& position )
{
	auto& tile = _tileMap[( position.row * _maxCol ) + position.col];
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
	_tileMap[( entity->GetPosition( ).row * _maxCol ) + entity->GetPosition( ).col].occupants.push_back( entity );
	UpdateTile( entity->GetPosition( ) );
}
void Dungeon::OccupantRemove( Entity* entity )
{
	auto& occupants = _tileMap[( entity->GetPosition( ).row * _maxCol ) + entity->GetPosition( ).col].occupants;
	occupants.erase( std::remove( occupants.begin( ), occupants.end( ), entity ), occupants.end( ) );
	UpdateTile( entity->GetPosition( ) );
}

void Dungeon::GenerateDoors( bool generate, int amount )
{
	if( generate )
	{
		int amountDoors = amount ? amount : 3;
		std::vector<Vector2i> positionValid;
		Vector2i iterator;

		std::cout << "Generating doors\n";

		for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
		{
			for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
			{
				if( iterator.col == 0 ||
					iterator.row == 0 ||
					iterator.col == _maxCol - 1 ||
					iterator.row == _maxRow - 1 )
				{
					if( !IsCorner( iterator ) )
					{
						positionValid.push_back( iterator );
					}
				}
			}
		}

		for( int i = 0; i < amountDoors; i++ )
		{
			int index = RandomNumberGenerator( 0, positionValid.size( ) - 1 );
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

		std::cout << "Generating outer obstacles\n";

		for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
		{
			for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
			{
				if( Unoccupied( iterator ) )
				{
					if( iterator.col == 0 ||
						iterator.row == 0 ||
						iterator.col == _maxCol - 1 ||
						iterator.row == _maxRow - 1 )
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
		const Vector2i center = Vector2i( _maxCol / 2, _maxRow / 2 );
		std::vector<Vector2i> pathToDoor;
		std::vector<Vector2i> obstacles;

		std::cout << "Generating path\n";

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
				pathToDoor = AStarAlgorithm( entity.GetPosition( ), center, _maxCol, _maxRow, obstacles );

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
		int sourceWallsLeft = amount ? amount : ( _maxCol * _maxRow ) / 50;
		const Vector2i center = Vector2i( _maxCol / 2, _maxRow / 2 );

		std::cout << "Generating source obstacles\n";

		while( sourceWallsLeft > 0 )
		{
			Vector2i position;
			position.col = RandomNumberGenerator( 1, _maxCol - 2 );
			position.row = RandomNumberGenerator( 1, _maxRow - 2 );

			if( Unoccupied( position ) &&
				position != center )
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
	if( generate )
	{
		const std::array<Vector2i, 4> directions =
		{
			Vector2i(  0, -1 ),
			Vector2i(  1,  0 ),
			Vector2i(  0,  1 ),
			Vector2i( -1,  0 ),
		};
		int extensionWallsLeft = amount ? amount : ( _maxCol * _maxRow ) / 3;
		const Vector2i center = Vector2i( _maxCol / 2, _maxRow / 2 );

		std::cout << "Generating extension obstacles\n";

		while( extensionWallsLeft > 0 )
		{
			for( const auto& entity : _entities )
			{
				if( entity.type == EntityType::Obstacle )
				{
					int index = RandomNumberGenerator( 0, 3 );
					Vector2i position = entity.GetPosition( ) + directions[index];

					if( InBounds( position ) &&
						Unoccupied( position ) &&
						position != center )
					{
						_entities.emplace_back( position, EntityType::Obstacle );
						OccupantAdd( &_entities.back( ) );
						extensionWallsLeft--;
					}
				}
			}
		}
	}
}
void Dungeon::GenerateFillerObstacles( bool generate, int amount )
{
	if( generate )
	{
		const int amountWalls = amount ? amount : 5;
		const Vector2i center = Vector2i( _maxCol / 2, _maxRow / 2 );
		Vector2i iterator;

		std::cout << "Generating filler obstacles\n";

		for( int i = 0; i < amountWalls; i++ )
		{
			for( iterator.row = 1; iterator.row < _maxRow - 1; iterator.row++ )
			{
				for( iterator.col = 1; iterator.col < _maxCol - 1; iterator.col++ )
				{
					if( Unoccupied( iterator ) &&
						Surrounded( iterator, 5 ) &&
						iterator != center )
					{
						_entities.emplace_back( iterator, EntityType::Obstacle );
						OccupantAdd( &_entities.back( ) );
					}
				}
			}
		}
	}
}
void Dungeon::GenerateMonsters( bool generate, int amount )
{
	if( generate )
	{
		const int min = static_cast<int>( sqrt( _maxCol * _maxRow ) / 3.0 );
		const int max = static_cast<int>( sqrt( _maxCol * _maxRow ) / 1.5 );
		const int amountMonsters = amount ? amount : RandomNumberGenerator( min, max );
		const Vector2i center = Vector2i( _maxCol / 2, _maxRow / 2 );

		std::cout << "Generating monsters\n";

		for( int i = 0; i < amountMonsters; i++ )
		{
			while( true )
			{
				Vector2i position;

				position.col = RandomNumberGenerator( 1, _maxCol - 2 );
				position.row = RandomNumberGenerator( 1, _maxRow - 2 );

				if( Walkable( position ) &&
					position != center )
				{
					_monsters.emplace_back( position );
					OccupantAdd( &_monsters.back( ) );

					break;
				}
			}
		}
	}
}