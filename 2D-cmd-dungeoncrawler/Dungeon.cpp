#include "Dungeon.h"
#include "AStarAlgorithm.h"
#include <math.h>
#include <algorithm>
#include <iostream>

std::size_t Components::Add( )
{
	indexCount++;
	active.push_back( false );
	attributes.emplace_back( );
	position.emplace_back( );
	positionPrevious.emplace_back( );
	health.emplace_back( );
	damage.emplace_back( );
	visionReach.emplace_back( );
	iconPriority.emplace_back( );
	icon.emplace_back( );

	return indexCount - 1;
}

Dungeon::Dungeon( const DungeonConfiguration& config ) :
	_maxCol( config.fixedDungeonSize ? config.maxCol : RandomNumberGenerator( 50, 60 ) ),
	_maxRow( config.fixedDungeonSize ? config.maxRow : RandomNumberGenerator( 50, 60 ) ),
	_tileMap( _maxCol * _maxRow ),
	_visionMap( _maxCol * _maxRow, false )

{
	PlayerAdd( { _maxCol / 2, _maxRow / 2 } );
	GenerateDoors( config.generateDoors, config.amountDoors );
	GenerateOuterWalls( config.generateOuterWalls );
	GeneratePath( config.generatePath );
	GenerateSourceWalls( config.generateSourceWalls, config.amountSourceWalls );
	GenerateExtensionWalls( config.generateExtensionWalls, config.amountExtensionWalls );
	GenerateFillerWalls( config.generateFillerWalls, config.amountFillerWallsCycles );
	GenerateMonsters( config.generateMonsters, config.amountMonsters );
}
Dungeon::Dungeon( int maxCol, int maxRow, const std::vector<bool>& visionMap, const std::vector<char>& iconMap ) :
	_maxCol( maxCol ),
	_maxRow( maxRow ),
	_tileMap( _maxCol * _maxRow ),
	_visionMap( visionMap )
{
	Vector2i iterator;

	PlayerAdd( { -1, -1 } );

	for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
		{
			switch( iconMap[( iterator.row * _maxCol ) + iterator.col] )
			{
				case Icon::Player:
				{
					const std::size_t indexPlayer = 0;

					_components.position[indexPlayer] = iterator;
					_components.positionPrevious[indexPlayer] = iterator;
					UpdateVision( iterator, _components.visionReach[indexPlayer] );
					OccupantAdd( indexPlayer );

					break;
				}
				case Icon::Monster:
				{
					MonsterAdd( iterator );

					break;
				}
				case Icon::Door:
				{
					DoorAdd( iterator );

					break;
				}
				case Icon::Wall:
				{
					WallAdd( iterator );

					break;
				}
			}
		}
	}
}

void Dungeon::SetPositionPlayer( const Vector2i& position )
{
	const std::size_t indexPlayer = 0;

	OccupantRemove( indexPlayer );

	if( CheckTile( position, Attribute::WalkableOthers ) )
	{
		_components.position[indexPlayer] = position;
		_components.positionPrevious[indexPlayer] = position;
	}
	else
	{
		const std::array<Vector2i, 4> directions =
		{
			Vector2i(  0, -1 ),
			Vector2i(  1,  0 ),
			Vector2i(  0,  1 ),
			Vector2i( -1,  0 )
		};

		for( const auto& direction : directions )
		{
			Vector2i nearby = position + direction;

			if( InBounds( nearby ) &&
				CheckTile( nearby, Attribute::WalkablePlayer ) )
			{
				_components.position[indexPlayer] = nearby;
				_components.positionPrevious[indexPlayer] = nearby;

				break;
			}
		}
	}

	OccupantAdd( indexPlayer );
}
const Vector2i& Dungeon::GetPositionPlayer( ) const
{
	const std::size_t indexPlayer = 0;

	return _components.position[indexPlayer];
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
		auto tileColoumEnd = tileRotated.begin( ) + iterator.row * _maxCol + _maxCol;
		auto visionColoumBegin = visionRotated.begin( ) + iterator.row * _maxCol;
		auto visionColoumEnd = visionRotated.begin( ) + iterator.row * _maxCol + _maxCol;

		std::reverse( tileColoumBegin, tileColoumEnd );
		std::reverse( visionColoumBegin, visionColoumEnd );
	}

	_tileMap = tileRotated;
	_visionMap = visionRotated;

	for( std::size_t index = 0; index < _components.indexCount; index++ )
	{
		_components.position[index] = PositionRotateClockwise( _components.position[index], _maxCol );
		_components.positionPrevious[index] = PositionRotateClockwise( _components.position[index], _maxCol );
	}

	for( auto& link : links )
	{
		link.entry = PositionRotateClockwise( link.entry, _maxCol );
	}
}

void Dungeon::PrintDungeonCentered( const Vector2i& screenSize )
{	
	/*
	struct LineOfSight
	{

		const std::vector<char> blockers;
		const Dungeon* const dungeon;
		const float errorOffset;
		const int visionReach;
		const std::array<Vector2i, 8> directions;
		std::vector<std::vector<Vector2i>> inspect;

		LineOfSight( const std::vector<char>& blockers, Dungeon* dungeon, float errorOffset, int visionReach ) :
			blockers( blockers ),
			dungeon( dungeon ),
			errorOffset( errorOffset ),
			visionReach( visionReach ),
			directions
			( { {
					Vector2i(  0, -1 ),
					Vector2i(  1, -1 ),
					Vector2i(  1,  0 ),
					Vector2i(  1,  1 ),
					Vector2i(  0,  1 ),
					Vector2i( -1,  1 ),
					Vector2i( -1,  0 ),
					Vector2i( -1, -1 )
			} } )
		{ }
		bool operator( )( const Vector2i& position )
		{
			if( std::find( blockers.begin( ), blockers.end( ), dungeon->GetTile( position ).icon ) != blockers.end( ) )
			{
				Vector2i iterator;

				inspect.emplace_back( );

				//for( iterator.row = position.row - 1; iterator.row <= position.row + 1; iterator.row++ )
				//{
				//	for( iterator.col = position.col - 1; iterator.col <= position.col + 1; iterator.col++ )
				//	{
				//		if( dungeon->InBounds( iterator ) )
				//		{
				//			inspect.back( ).push_back( iterator );
				//		}
				//	}
				//}

				for( auto direction : directions )
				{
					Vector2i neighbor = position + direction;

					if( dungeon->InBounds( neighbor ) )
					{
						inspect.back( ).push_back( iterator );
					}
				}


			}
		}
	};
	*/
	const std::size_t indexPlayer = 0;
	const int visionReach = _components.visionReach[indexPlayer];
	const Vector2i center = _components.position[indexPlayer];
	const Vector2i cameraOrigo = center - screenSize / 2;
	const Vector2i iteratorBegin = cameraOrigo - 1;
	const Vector2i iteratorEnd = cameraOrigo + screenSize + 1;
	auto InsideVisionReach = [visionReach, center] ( const Vector2i& iterator ) -> bool
	{
		return
			iterator >= center - visionReach &&
			iterator <= center + visionReach;
	};
	Vector2i iterator;

	for( iterator.row = iteratorBegin.row; iterator.row <= iteratorEnd.row; iterator.row++ )
	{
		for( iterator.col = iteratorBegin.col; iterator.col <= iteratorEnd.col; iterator.col++ )
		{
			if( iterator.col == iteratorBegin.col ||
				iterator.row == iteratorBegin.row ||
				iterator.col == iteratorEnd.col ||
				iterator.row == iteratorEnd.row )
			{
				std::cout << '\\';
			}
			else if( InBounds( iterator ) &&
					 InsideVisionReach( iterator ) )
			{
				std::cout << GetTile( iterator ).icon;
			}
			else if( InBounds( iterator ) &&
					 GetVision( iterator ) )
			{
				std::cout << ':';
			}
			else
			{
				std::cout << ' ';
			}
		}

		std::cout << '\n';
	}

	std::cout << '\n';
}
void Dungeon::PlayerMovement( const Orientation& orientation )
{
	const std::size_t indexPlayer = 0;

	if( _components.active[indexPlayer] )
	{
		OccupantRemove( indexPlayer );
		_components.positionPrevious[indexPlayer] = _components.position[indexPlayer];
		_components.position[indexPlayer] = MoveEntity( _components.position[indexPlayer], orientation );
		OccupantAdd( indexPlayer );
	}
}
void Dungeon::MonsterMovement( )
{
	for( std::size_t index = 1; index < _components.indexCount; index++ )
	{
		if( _components.active[index] &&
			_components.attributes[index] & Attribute::MovementRandom )
		{
			OccupantRemove( index );
			_components.positionPrevious[index] = _components.position[index];
			_components.position[index] = MoveEntityProbability( _components.position[index], 1, 1, 1, 1, 12 );
			OccupantAdd( index );
		}
	}
}
void Dungeon::HandleEvents( GameStatus& status )
{
	const std::size_t indexPlayer = 0;
	auto& tilePlayer = _tileMap[( _components.position[indexPlayer].row * _maxCol ) + _components.position[indexPlayer].col];

	for( std::size_t index = 1; index < _components.indexCount; index++ )
	{
		if( _components.active[index] &&
			_components.attributes[index] & Attribute::MovementRandom &&
			!CheckTile( _components.position[index], Attribute::WalkableOthers ) )
		{
			OccupantRemove( index );
			std::swap( _components.position[index], _components.positionPrevious[index] );
			OccupantAdd( index );
		}
	}

	if( _components.active[indexPlayer] &&
		!CheckTile( _components.position[indexPlayer], Attribute::WalkablePlayer ) )
	{
		OccupantRemove( indexPlayer );
		std::swap( _components.position[indexPlayer], _components.positionPrevious[indexPlayer] );
		OccupantAdd( indexPlayer );
	}

	UpdateVision( _components.position[indexPlayer], _components.visionReach[indexPlayer] );
	
	for( auto index : tilePlayer.indexOccupants )
	{
		if( _components.active[index] &&
			_components.attributes[index] & Attribute::Aggressive )
		{
			Battle( &_components.health[index],
					&_components.damage[index],
					&_components.health[indexPlayer],
					&_components.damage[indexPlayer] );
		}
	}

	for( const auto& link : links )
	{
		if( _components.active[indexPlayer] &&
			_components.position[indexPlayer] == link.entry )
		{
			status = GameStatus::Next;
		}
	}

	for( std::size_t index = 0; index < _components.indexCount; index++ )
	{
		if( _components.active[index] &&
			_components.attributes[index] & Attribute::Mortal &&
			_components.health[index] <= 0 )
		{
			if( index == indexPlayer )
			{
				status = GameStatus::Menu;
			}

			_components.active[index] = false;
			OccupantRemove( index );
		}
	}
}

bool Dungeon::CheckTile( const Vector2i& position, int bitmask ) const
{
	const auto& indexes = GetTile( position ).indexOccupants;
	int count = 0;

	for( auto index : indexes )
	{
		if( _components.attributes[index] & bitmask )
		{
			count++;
		}
	}

	return count == indexes.size( );
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
	return _tileMap[( position.row * _maxCol ) + position.col].indexOccupants.empty( );
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
	Vector2i neighbor;

	for( const auto& direction : directions )
	{
		neighbor = position + direction;

		if( !Unoccupied( neighbor ) )
		{
			surroundingEntity++;
		}
	}

	return surroundingEntity >= threshold;
}

void Dungeon::UpdateVision( const Vector2i& position, int visionReach )
{
	const Vector2i iteratorBegin = position - visionReach;
	const Vector2i iteratorEnd = position + visionReach;
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
	char iconDisplay = '-';
	int iconPriorityCurrent = 0;

	for( auto index : tile.indexOccupants )
	{
		if( !( _components.attributes[index] & Attribute::Hidden ) &&
			_components.iconPriority[index] > iconPriorityCurrent )
		{
			iconPriorityCurrent = _components.iconPriority[index];
			iconDisplay = _components.icon[index];
		}
	}

	tile.icon = iconDisplay;
}
void Dungeon::OccupantAdd( std::size_t index )
{
	Vector2i position = _components.position[index];

	_tileMap[( position.row * _maxCol ) + position.col].indexOccupants.push_back( index );
	UpdateTile( position );
}
void Dungeon::OccupantRemove( std::size_t index )
{
	Vector2i position = _components.position[index];
	auto& indexes = _tileMap[( position.row * _maxCol ) + position.col].indexOccupants;

	indexes.erase( std::remove( indexes.begin( ), indexes.end( ), index ), indexes.end( ) );
	UpdateTile( position );
}

void Dungeon::PlayerAdd( const Vector2i& position )
{
	const std::size_t indexPlayer = _components.Add( );

	_components.active[indexPlayer] = true;
	_components.attributes[indexPlayer] = Attribute::Mortal | Attribute::WalkableOthers | Attribute::WalkablePlayer;
	_components.position[indexPlayer] = position;
	_components.positionPrevious[indexPlayer] = position;
	_components.health[indexPlayer] = 100;
	_components.damage[indexPlayer] = 100;
	_components.visionReach[indexPlayer] = 5;
	_components.iconPriority[indexPlayer] = 1;
	_components.icon[indexPlayer] = '@';
	UpdateVision( _components.position[indexPlayer], _components.visionReach[indexPlayer] );
	OccupantAdd( indexPlayer );
}
void Dungeon::WallAdd( const Vector2i& position )
{
	const std::size_t index = _components.Add( );

	_components.active[index] = true;
	_components.attributes[index] = 0;
	_components.position[index] = position;
	_components.iconPriority[index] = 1;
	_components.icon[index] = '#';
	OccupantAdd( index );
}
void Dungeon::DoorAdd( const Vector2i& position )
{
	const std::size_t index = _components.Add( );

	links.push_back( { false, 0, { -1, -1 }, position } );
	_components.active[index] = true;
	_components.attributes[index] = Attribute::WalkablePlayer;
	_components.position[index] = position;
	_components.iconPriority[index] = 10;
	_components.icon[index] = '+';
	OccupantAdd( index );
}
void Dungeon::StepAdd( const Vector2i& position )
{
	const std::size_t index = _components.Add( );

	_components.active[index] = true;
	_components.attributes[index] = Attribute::Hidden | Attribute::WalkableOthers | Attribute::WalkablePlayer;
	_components.position[index] = position;
	OccupantAdd( index );
}
void Dungeon::MonsterAdd( const Vector2i& position )
{
	const std::size_t index = _components.Add( );

	_components.active[index] = true;
	_components.attributes[index] = Attribute::Aggressive | Attribute::Mortal | Attribute::MovementRandom | Attribute::WalkablePlayer;
	_components.position[index] = position;
	_components.positionPrevious[index] = position;
	_components.health[index] = 100;
	_components.damage[index] = 10;
	_components.iconPriority[index] = 1;
	_components.icon[index] = 'M';
	OccupantAdd( index );
}

void Dungeon::GenerateDoors( bool generate, int amount )
{
	if( generate )
	{
		int amountDoors = amount ? amount : 3;
		std::vector<Vector2i> valid;
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
						valid.push_back( iterator );
					}
				}
			}
		}

		for( int i = 0; i < amountDoors; i++ )
		{
			int index = RandomNumberGenerator( 0, valid.size( ) - 1 );

			DoorAdd( valid[index] );
			valid.erase( valid.begin( ) + index );
		}
	}
}
void Dungeon::GenerateOuterWalls( bool generate )
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
						WallAdd( iterator );
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
		const std::size_t indexPlayer = 0;
		std::vector<Vector2i> path;
		std::vector<Vector2i> walls;

		std::cout << "Generating path\n";

		for( std::size_t index = 1; index < _components.indexCount; index++ )
		{
			if( _components.active[index] &&
				!( _components.attributes[index] & Attribute::WalkableOthers ) )
			{
				walls.push_back( _components.position[index] );
			}
		}

		for( const auto& link : links )
		{
			const Vector2i start = link.entry;
			const Vector2i goal = _components.position[indexPlayer];

			path = AStarAlgorithm( start, goal, _maxCol, _maxRow, walls );

			for( const auto& position : path )
			{
				if( Unoccupied( position ) )
				{
					StepAdd( position );
				}
			}
		}
	}
}
void Dungeon::GenerateSourceWalls( bool generate, int amount )
{
	if( generate )
	{
		int sourceWallsLeft = amount ? amount : ( _maxCol * _maxRow ) / 50;

		std::cout << "Generating source obstacles\n";

		while( sourceWallsLeft > 0 )
		{
			Vector2i position;

			position.col = RandomNumberGenerator( 1, _maxCol - 2 );
			position.row = RandomNumberGenerator( 1, _maxRow - 2 );

			if( Unoccupied( position ) )
			{
				WallAdd( position );
				sourceWallsLeft--;
			}
		}
	}
}
void Dungeon::GenerateExtensionWalls( bool generate, int amount )
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

		std::cout << "Generating extension obstacles\n";

		while( extensionWallsLeft > 0 )
		{
			for( std::size_t index = 1; index < _components.indexCount; index++ )
			{
				if( _components.active[index] &&
					!( _components.attributes[index] & Attribute::WalkablePlayer ) &&
					!( _components.attributes[index] & Attribute::WalkableOthers ) )
				{
					int indexDirection = RandomNumberGenerator( 0, 3 );
					Vector2i position = _components.position[index] + directions[indexDirection];

					if( InBounds( position ) &&
						Unoccupied( position ) )
					{
						WallAdd( position );
						extensionWallsLeft--;
					}
				}
			}
		}
	}
}
void Dungeon::GenerateFillerWalls( bool generate, int amount )
{
	if( generate )
	{
		const int amountWalls = amount ? amount : 5;
		Vector2i iterator;

		std::cout << "Generating filler obstacles\n";

		for( int i = 0; i < amountWalls; i++ )
		{
			for( iterator.row = 1; iterator.row < _maxRow - 1; iterator.row++ )
			{
				for( iterator.col = 1; iterator.col < _maxCol - 1; iterator.col++ )
				{
					if( Unoccupied( iterator ) &&
						Surrounded( iterator, 5 ) )
					{
						WallAdd( iterator );
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

		std::cout << "Generating monsters\n";

		for( int i = 0; i < amountMonsters; i++ )
		{
			while( true )
			{
				Vector2i position;

				position.col = RandomNumberGenerator( 1, _maxCol - 2 );
				position.row = RandomNumberGenerator( 1, _maxRow - 2 );

				if( Unoccupied( position ) )
				{
					MonsterAdd( position );

					break;
				}
			}
		}
	}
}