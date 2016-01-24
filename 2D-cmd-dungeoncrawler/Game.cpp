#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>

Game::Game( ) :
	existingGame( false )
{ }

void Game::SetDungeonConfiguration( )
{
	const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
	auto GetBool = []( char input ) -> bool { return input == 'Y' || input == 'y'; };

	std::cout << "\n";
	
	_config.fixedSeed = GetBool( InputValidChar( "Fixed seed for RandomNumberGenerator, [Y/N]: ", YesNo ) );
	_config.fixedDungeonSize = GetBool( InputValidChar( "Fixed dungeon size, [Y/N]: ", YesNo ) );
	_config.generateDoors = GetBool( InputValidChar( "Generate doors, [Y/N]: ", YesNo ) );
	_config.generateOuterObstacles = GetBool( InputValidChar( "Generate outer obstacles, [Y/N]: ", YesNo ) );
	_config.generatePath = GetBool( InputValidChar( "Generate path, [Y/N]: ", YesNo ) );
	_config.generateSourceObstacles = GetBool( InputValidChar( "Generate source obstacles, [Y/N]: ", YesNo ) );
	_config.generateExtensionObstacles = GetBool( InputValidChar( "Generate extension, [Y/N]: ", YesNo ) );
	_config.generateFillerObstacles = GetBool( InputValidChar( "Generate filler obstacles, [Y/N]: ", YesNo ) );
	_config.generateMonsters = GetBool( InputValidChar( "Generate monsters, [Y/N]: ", YesNo ) );

	std::cout << "\n";

	if( _config.fixedSeed )
	{
		_config.seed = InputPositiveInteger( "Enter seed: " );
	}
	if( _config.fixedDungeonSize )
	{
		_config.dungeonSize.col = InputPositiveInteger( "Enter dungeon col size: " );
		_config.dungeonSize.row = InputPositiveInteger( "Enter dungeon col size: " );
	}
	if( _config.generateDoors )
	{
		_config.amountDoors = InputPositiveInteger( "Enter amount of doors: " );
	}
	if( _config.generateSourceObstacles )
	{
		_config.amountSourceObstacles = InputPositiveInteger( "Enter amount of source obstacles: " );
	}
	if( _config.generateExtensionObstacles )
	{
		_config.amountExtensionObstacles = InputPositiveInteger( "Enter amount of extension obstacles: " );
	}
	if( _config.generateFillerObstacles )
	{
		_config.amountFillerObstacleCycles = InputPositiveInteger( "Enter amount of filler obstacle cycles: " );
	}
	if( _config.generateMonsters )
	{
		_config.amountMonsters = InputPositiveInteger( "Enter amount of monsters: " );
	}
}
void Game::NewGame( )
{
	Dungeon* dungeon;

	_player.reset( new Player( Vector2i( ) ) );
	_dungeons.clear( );
	_dungeonGraph.nodes.clear( );
	_dungeonGraph.edges.clear( );
	_status = GameStatus::Neutral;
	existingGame = true;

	_dungeons.emplace_back( _player.get( ), _config );
	dungeon = &_dungeons.back( );
	_dungeonGraph.nodes.push_back( { &_dungeons.back( ) } );
	_indexNodeCurrent = _dungeonGraph.nodes.size( ) - 1;
	FullLinkDungeon( _indexNodeCurrent );
	dungeon->PlayerInitialPlace( dungeon->GetSize( ) / 2 );
}
void Game::GameLoop( )
{
	Dungeon* dungeon = _dungeonGraph.nodes[_indexNodeCurrent].data;

	_status = GameStatus::Neutral;

	while( _status == GameStatus::Neutral &&
		   _player->GetAlive( ) )
	{
		dungeon->UpdatePlayerVision( );

		OutputClearScreen( );
		OutputDungeonCentered( *dungeon, _player->GetPosition( ) );
		OutputCharacterStatus( *_player );
		OutputTurnOptions( );

		PlayerTurn( *dungeon );

		dungeon->MonsterMovement( );
		dungeon->HandleEvents( _status );
		dungeon->RemoveDeadCharacters( _status );

		if( _status == GameStatus::Next )
		{
			for( const auto& indexEdgeParent : _dungeonGraph.nodes[_indexNodeCurrent].indexEdges )
			{
				auto indexNodeChild = _dungeonGraph.edges[indexEdgeParent].indexNode;

				for( const auto& indexEdgeChild : _dungeonGraph.nodes[indexNodeChild].indexEdges )
				{
					if( _dungeonGraph.edges[indexEdgeChild].data == _player->GetPosition( ) )
					{
						_indexNodeCurrent = indexNodeChild;
						dungeon = _dungeonGraph.nodes[indexNodeChild].data;
						dungeon->PlayerInitialPlaceNearby( _dungeonGraph.edges[indexEdgeParent].data );
						FullLinkDungeon( indexNodeChild );

						goto END;
					}
				}
			}

			END:
			_status = GameStatus::Neutral;
		}
	}
}

void Game::FullLinkDungeon( std::size_t indexNodeParent )
{
	auto doors = _dungeonGraph.nodes[indexNodeParent].data->GetDoors( );
	auto required = doors.size( );

	while( _dungeonGraph.nodes[indexNodeParent].indexEdges.size( ) < required )
	{
		for( const auto& door : doors )
		{
			Dungeon* dungeonChild;

			_dungeons.emplace_back( _player.get( ), _config );
			dungeonChild = &_dungeons.back( );
			_dungeonGraph.edges.push_back( { door->GetPosition( ), indexNodeParent } );
			_dungeonGraph.nodes.push_back( { dungeonChild, std::vector<std::size_t>{ _dungeonGraph.edges.size( ) - 1 } } );
			_dungeonGraph.edges.push_back( { dungeonChild->GetDoors( ).back( )->GetPosition( ), _dungeonGraph.nodes.size( ) - 1 } );
			_dungeonGraph.nodes[indexNodeParent].indexEdges.push_back( _dungeonGraph.edges.size( ) - 1 );

			std::cout << "\nLinked added";
		}
	}
}
void Game::PlayerTurn( Dungeon& dungeon )
{
	static const std::vector<char> choices { 'W', 'w', 'A', 'a', 'S', 's', 'D', 'd', 'E', 'e', 'F', 'f', 'Q', 'q' };
	static const std::map<char, Orientation> direction
	{
		{ 'W', Orientation::North }, { 'w', Orientation::North },
		{ 'A', Orientation::West  }, { 'a', Orientation::West  },
		{ 'S', Orientation::South }, { 's', Orientation::South },
		{ 'D', Orientation::East  }, { 'd', Orientation::East  }
	};
	const char choice = InputValidChar( "\nYour choice: ", choices );

	switch( choice )
	{
		case 'W': case 'w':
		case 'A': case 'a':
		case 'S': case 's':
		case 'D': case 'd':
		{
			dungeon.PlayerMovement( direction.at( choice ) );

			break;
		}
		case 'E': case 'e':
		{
			_status = GameStatus::Menu;

			break;
		}
		case 'F': case 'f':
		{
			dungeon.RotateDungeonClockwise( );
			dungeon.UpdateEntityPositions( );

			break;
		}
		case 'Q': case 'q':
		{
			break;
		}
	}
}