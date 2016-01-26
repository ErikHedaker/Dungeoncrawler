#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <fstream>
#include <string>

Game::Game( ) :
	existingGame( false ),
	_player( new Player( Vector2i( ) ) )
{ }

void Game::SetDungeonConfiguration( const ConfigType& type )
{
	switch( type )
	{
		case ConfigType::Default:
		{
			_config = DungeonConfiguration( );

			break;
		}
		case ConfigType::Configure:
		{
			const std::vector<char> YesNo = { 'Y', 'y', 'N', 'n' };
			auto GetBool = [] ( char input ) -> bool
			{
				return input == 'Y' || input == 'y';
			};

			std::cout << "\n";

			_config.fixedDungeonSize = GetBool( InputValidChar( "Fixed dungeon size, [Y/N]: ", YesNo ) );
			_config.generateDoors = GetBool( InputValidChar( "Generate doors, [Y/N]: ", YesNo ) );
			_config.generateOuterObstacles = GetBool( InputValidChar( "Generate outer obstacles, [Y/N]: ", YesNo ) );
			_config.generatePath = GetBool( InputValidChar( "Generate path, [Y/N]: ", YesNo ) );
			_config.generateSourceObstacles = GetBool( InputValidChar( "Generate source obstacles, [Y/N]: ", YesNo ) );
			_config.generateExtensionObstacles = GetBool( InputValidChar( "Generate extension, [Y/N]: ", YesNo ) );
			_config.generateFillerObstacles = GetBool( InputValidChar( "Generate filler obstacles, [Y/N]: ", YesNo ) );
			_config.generateMonsters = GetBool( InputValidChar( "Generate monsters, [Y/N]: ", YesNo ) );

			std::cout << "\n";

			if( _config.fixedDungeonSize )
			{
				_config.maxCol = InputPositiveInteger( "Enter dungeon col size: " );
				_config.maxRow = InputPositiveInteger( "Enter dungeon row size: " );
			}
			if( _config.generateDoors )
				_config.amountDoors = InputPositiveInteger( "Enter amount of doors: " );
			if( _config.generateSourceObstacles )
				_config.amountSourceObstacles = InputPositiveInteger( "Enter amount of source obstacles: " );
			if( _config.generateExtensionObstacles )
				_config.amountExtensionObstacles = InputPositiveInteger( "Enter amount of extension obstacles: " );
			if( _config.generateFillerObstacles )
				_config.amountFillerObstacleCycles = InputPositiveInteger( "Enter amount of filler obstacle cycles: " );
			if( _config.generateMonsters )
				_config.amountMonsters = InputPositiveInteger( "Enter amount of monsters: " );

			break;
		}
	}
}
void Game::NewGame( )
{
	Dungeon* dungeon;

	_player->SetHealth( 100 );
	_player->SetAlive( true );
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
	dungeon->PlayerInitialPlace( Vector2i( dungeon->GetSize( ).first / 2, dungeon->GetSize( ).second / 2 ) );
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
		OutputDungeonCentered( *dungeon, _player->GetPosition( ), _player->GetVisionReach( ) );
		OutputCharacterStatus( *_player );
		OutputTurnOptions( );

		PlayerTurn( *dungeon );

		dungeon->MonsterMovement( );
		dungeon->HandleEvents( _status );
		dungeon->RemoveDeadCharacters( );

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
void Game::SaveDungeons( )
{
	const std::string nameFile = "2D-cmd-dungeoncrawler-save.txt";
	std::ofstream outFile;

	outFile.open( nameFile, std::ios::out | std::ios::trunc );
	
	if( !outFile.is_open( ) )
	{
		throw std::exception( std::string( "Could not open file " + nameFile ).c_str( ) );
	}

	outFile << _indexNodeCurrent << '\n';
	outFile << _dungeonGraph.edges.size( ) << '\n';

	for( std::size_t indexEdge = 0; indexEdge < _dungeonGraph.edges.size( ); indexEdge++ )
	{
		outFile << _dungeonGraph.edges[indexEdge].indexNode << '\n';
		outFile << _dungeonGraph.edges[indexEdge].data.col << '\n';
		outFile << _dungeonGraph.edges[indexEdge].data.row << '\n';
	}

	outFile << _dungeonGraph.nodes.size( ) << '\n';

	for( std::size_t indexNode = 0; indexNode < _dungeonGraph.nodes.size( ); indexNode++ )
	{
		const auto& dungeon = *_dungeonGraph.nodes[indexNode].data;
		const int maxCol = dungeon.GetSize( ).first;
		const int maxRow = dungeon.GetSize( ).second;
		Vector2i iterator;

		outFile << _dungeonGraph.nodes[indexNode].indexEdges.size( ) << '\n';

		for( auto indexEdge : _dungeonGraph.nodes[indexNode].indexEdges )
		{
			outFile << indexEdge << '\n';
		}

		outFile << maxCol << '\n';
		outFile << maxRow << '\n';

		for( iterator.row = 0; iterator.row < maxRow; iterator.row++ )
		{
			for( iterator.col = 0; iterator.col < maxCol * 2; iterator.col++ )
			{
				if( iterator.col < maxCol )
				{
					outFile << dungeon.GetTile( iterator ).icon;
				}
				else
				{
					Vector2i position( iterator.col % 2, iterator.row );

					outFile << dungeon.GetVision( position );
				}
			}

			outFile << '\n';
		}
	}
	
	outFile.close( );
}
void Game::LoadDungeons( )
{
	const std::string nameFile = "2D-cmd-dungeoncrawler-save.txt";
	std::string line;
	std::ifstream inFile;
	std::size_t amountEdges;
	std::size_t amountNodes;

	inFile.open( nameFile, std::ios::in );

	if( !inFile.is_open( ) )
	{
		throw std::exception( std::string( "Could not open file " + nameFile ).c_str( ) );
	}

	try
	{
		std::getline( inFile, line );
		_indexNodeCurrent = std::stoi( line );

		std::getline( inFile, line );
		amountEdges = std::stoi( line );

		for( std::size_t indexEdge = 0; indexEdge < amountEdges; indexEdge++ )
		{
			std::size_t indexNode;
			int col;
			int row;

			std::getline( inFile, line );
			indexNode = std::stoi( line );

			std::getline( inFile, line );
			col = std::stoi( line );

			std::getline( inFile, line );
			row = std::stoi( line );

			_dungeonGraph.edges.push_back( { Vector2i( col, row ), indexNode } );
		}

		std::getline( inFile, line );
		amountNodes = std::stoi( line );

		for( std::size_t indexNode = 0; indexNode < amountNodes; indexNode++ )
		{
			std::vector<char> iconMap;
			std::vector<bool> visionMap;
			std::vector<std::size_t> indexNodeEdgeNodes;
			std::size_t amountNodeEdges;
			Vector2i iterator;
			int maxCol;
			int maxRow;

			std::getline( inFile, line );
			amountNodeEdges = std::stoi( line );

			for( std::size_t indexNodeEdge = 0; indexNodeEdge < amountNodeEdges; indexNodeEdge++ )
			{
				std::getline( inFile, line );
				indexNodeEdgeNodes.push_back( std::stoi( line ) );
			}

			std::getline( inFile, line );
			maxCol = std::stoi( line );

			std::getline( inFile, line );
			maxRow = std::stoi( line );

			iconMap.resize( maxCol * maxRow );
			visionMap.resize( maxCol * maxRow );

			for( iterator.row = 0; iterator.row < maxRow; iterator.row++ )
			{
				std::getline( inFile, line );

				for( iterator.col = 0; iterator.col < maxCol * 2; iterator.col++ )
				{
					if( iterator.col < maxCol )
					{
						switch( line[iterator.col] )
						{
							case Icon::Player:
							{
								iconMap[( iterator.row * maxCol ) + iterator.col] = Icon::Player;
								_player->SetPosition( iterator );

								break;
							}
							case Icon::Monster:
							{
								iconMap[( iterator.row * maxCol ) + iterator.col] = Icon::Monster;

								break;
							}
							case Icon::Door:
							{
								iconMap[( iterator.row * maxCol ) + iterator.col] = Icon::Door;

								break;
							}
							case Icon::Obstacle:
							{
								iconMap[( iterator.row * maxCol ) + iterator.col] = Icon::Obstacle;

								break;
							}
							case Icon::Ground:
							{
								iconMap[( iterator.row * maxCol ) + iterator.col] = Icon::Ground;

								break;
							}
							default:
							{
								throw std::exception( std::string( "Could not read tile icons" ).c_str( ) );
							}
						}
					}
					else
					{
						Vector2i position( iterator.col % 2, iterator.row );

						switch( line[iterator.col] )
						{
							case '1':
							{
								visionMap[( position.row * maxCol ) + position.col] = true;

								break;
							}
							case '0':
							{
								visionMap[( position.row * maxCol ) + position.col] = false;

								break;
							}
							default:
							{
								throw std::exception( std::string( "Could not read vision" ).c_str( ) );
							}
						}
					}
				}
			}

			_dungeons.emplace_back( _player.get( ), maxCol, maxRow, visionMap, iconMap );
			_dungeonGraph.nodes.push_back( { &_dungeons.back( ), indexNodeEdgeNodes } );
		}
	}
	catch( ... )
	{
		throw std::exception( std::string( "Could not read integers" ).c_str( ) );
	}

	inFile.close( );
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

			std::cout << "\nAdding dungeon link\n\n";

			_dungeons.emplace_back( _player.get( ), _config );
			dungeonChild = &_dungeons.back( );
			_dungeonGraph.edges.push_back( { door->GetPosition( ), indexNodeParent } );
			_dungeonGraph.nodes.push_back( { dungeonChild, std::vector<std::size_t>{ _dungeonGraph.edges.size( ) - 1 } } );
			_dungeonGraph.edges.push_back( { dungeonChild->GetDoors( )[0]->GetPosition( ), _dungeonGraph.nodes.size( ) - 1 } );
			_dungeonGraph.nodes[indexNodeParent].indexEdges.push_back( _dungeonGraph.edges.size( ) - 1 );
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

			//for( auto& indexEdge : _dungeonGraph.nodes[_indexNodeCurrent].indexEdges )
			//{
			//	Vector2i& position = _dungeonGraph.edges[indexEdge].data;

			//	position = RotatePositionClockwise( position, dungeon.GetSize( ).first );
			//}

			break;
		}
		case 'Q': case 'q':
		{
			break;
		}
	}
}