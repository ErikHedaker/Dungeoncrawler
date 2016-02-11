#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <fstream>
#include <string>

bool Game::ExistingGame( ) const
{
	return _dungeons.size( ) != 0;
}
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
			const std::vector<char> choices { 'Y', 'y', 'N', 'n' };
			auto GetBool = [] ( char input ) -> bool
			{
				return input == 'Y' || input == 'y';
			};

			std::cout << "\n";

			_config.fixedDungeonSize       = GetBool( GetValidChar( "Fixed dungeon size, [Y/N]: ",        choices ) );
			_config.generateDoors          = GetBool( GetValidChar( "Generate doors, [Y/N]: ",            choices ) );
			_config.generateOuterWalls     = GetBool( GetValidChar( "Generate outer obstacles, [Y/N]: ",  choices ) );
			_config.generatePath           = GetBool( GetValidChar( "Generate path, [Y/N]: ",             choices ) );
			_config.generateSourceWalls    = GetBool( GetValidChar( "Generate source obstacles, [Y/N]: ", choices ) );
			_config.generateExtensionWalls = GetBool( GetValidChar( "Generate extension, [Y/N]: ",        choices ) );
			_config.generateFillerWalls    = GetBool( GetValidChar( "Generate filler obstacles, [Y/N]: ", choices ) );
			_config.generateMonsters       = GetBool( GetValidChar( "Generate monsters, [Y/N]: ",         choices ) );

			std::cout << "\n";

			if( _config.fixedDungeonSize )
			{
				_config.maxCol = GetPositiveInteger( "Enter dungeon col size: " );
				_config.maxRow = GetPositiveInteger( "Enter dungeon row size: " );
			}
			if( _config.generateDoors )
				_config.amountDoors             = GetPositiveInteger( "Enter amount of doors: " );
			if( _config.generateSourceWalls )
				_config.amountSourceWalls       = GetPositiveInteger( "Enter amount of source obstacles: " );
			if( _config.generateExtensionWalls )
				_config.amountExtensionWalls    = GetPositiveInteger( "Enter amount of extension obstacles: " );
			if( _config.generateFillerWalls )
				_config.amountFillerWallsCycles = GetPositiveInteger( "Enter amount of filler obstacle cycles: " );
			if( _config.generateMonsters )
				_config.amountMonsters          = GetPositiveInteger( "Enter amount of monsters: " );

			break;
		}
	}
}
void Game::NewGame( )
{
	_dungeons.clear( );
	_dungeons.emplace_back( _config );
	_indexCurrent = _dungeons.size( ) - 1;
	FullLinkDungeon( _indexCurrent );
}
void Game::GameLoop( )
{
	_status = GameStatus::Neutral;

	while( _status == GameStatus::Neutral )
	{
		system( "CLS" );
		_dungeons[_indexCurrent].PrintDungeonCentered( );
		PlayerTurn( _dungeons[_indexCurrent] );
		_dungeons[_indexCurrent].MonsterMovement( );
		_dungeons[_indexCurrent].HandleEvents( _status );

		if( _status == GameStatus::Next )
		{
			_status = GameStatus::Neutral;
			SwitchDungeon( );
		}
	}
}
void Game::SaveDungeons( )
{
	/*
	const std::string nameFile = "2D-cmd-dungeoncrawler-save.txt";
	std::ofstream outFile;

	outFile.open( nameFile, std::ios::out | std::ios::trunc );
	
	if( !outFile.is_open( ) )
	{
		throw std::exception( std::string( "Could not open file " + nameFile ).c_str( ) );
	}

	outFile << _indexCurrent << '\n';
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

		outFile << _dungeonGraph.nodes[indexNode].indexEdgesIn.size( ) << '\n';

		for( auto indexEdgeIn : _dungeonGraph.nodes[indexNode].indexEdgesIn )
		{
			outFile << indexEdgeIn << '\n';
		}
		for( auto indexEdgeOut : _dungeonGraph.nodes[indexNode].indexEdgesOut )
		{
			outFile << indexEdgeOut << '\n';
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
	*/
}
void Game::LoadDungeons( )
{
	/*
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
		_indexCurrent = std::stoi( line );

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

			// Add something here

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
							case Icon::Wall:
							{
								iconMap[( iterator.row * maxCol ) + iterator.col] = Icon::Wall;

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

			_dungeons.emplace_back( maxCol, maxRow, visionMap, iconMap );
			_dungeonGraph.nodes.push_back( { &_dungeons.back( ), indexNodeEdgeNodes } );
		}
	}
	catch( ... )
	{
		throw std::exception( std::string( "Could not read integers" ).c_str( ) );
	}

	inFile.close( );
	*/
}

void Game::PlayerTurn( Dungeon& dungeon )
{
	std::cout << "[W] Go North\n";
	std::cout << "[A] Go West\n";
	std::cout << "[S] Go South\n";
	std::cout << "[D] Go East\n";
	std::cout << "[Q] Stand still\n";
	std::cout << "[E] Exit to meny\n";
	std::cout << "[F] Rotate dungeon 90 degrees clockwise\n";

	static const std::vector<char> choices
	{
		'W', 'w',
		'A', 'a',
		'S', 's',
		'D', 'd',
		'E', 'e',
		'F', 'f',
		'Q', 'q'
	};
	static const std::map<char, Orientation> direction
	{
		{ 'W', Orientation::North }, { 'w', Orientation::North },
		{ 'A', Orientation::West }, { 'a', Orientation::West },
		{ 'S', Orientation::South }, { 's', Orientation::South },
		{ 'D', Orientation::East }, { 'd', Orientation::East }
	};
	const char choice = GetValidChar( "\nYour choice: ", choices );

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
			LinkExitsRotateClockwise( _indexCurrent );

			break;
		}
		case 'Q': case 'q':
		{
			break;
		}
	}
}
void Game::FullLinkDungeon( std::size_t indexDungeon )
{
	for( std::size_t index = 0; index < _dungeons[indexDungeon].links.size( ); index++ )
	{
		if( !_dungeons[indexDungeon].links[index].set )
		{
			std::cout << "\nAdding link\n\n";

			_dungeons.emplace_back( _config );

			const std::size_t indexDungeonNeighbor = _dungeons.size( ) - 1;
			auto& linkNeighbor = _dungeons.back( ).links.back( );

			_dungeons[indexDungeon].links[index].indexDungeon = indexDungeonNeighbor;
			linkNeighbor.indexDungeon = indexDungeon;

			_dungeons[indexDungeon].links[index].exit = linkNeighbor.entry;
			linkNeighbor.exit = _dungeons[indexDungeon].links[index].entry;

			_dungeons[indexDungeon].links[index].set = true;
			linkNeighbor.set = true;
		}
	}
}
void Game::LinkExitsRotateClockwise( std::size_t index )
{
	for( const auto& linkCurrent : _dungeons[index].links )
	{
		for( auto& link : _dungeons[linkCurrent.indexDungeon].links )
		{
			if( link.indexDungeon == index )
			{
				link.exit = PositionRotateClockwise( link.exit, _dungeons[index].GetSize( ).first );
			}
		}
	}
}
void Game::SwitchDungeon( )
{
	for( const auto& link : _dungeons[_indexCurrent].links )
	{
		if( link.entry == _dungeons[_indexCurrent].GetPositionPlayer( ) )
		{
			_indexCurrent = link.indexDungeon;
			_dungeons[_indexCurrent].SetPositionPlayer( link.exit );
			FullLinkDungeon( _indexCurrent );

			break;
		}
	}
}