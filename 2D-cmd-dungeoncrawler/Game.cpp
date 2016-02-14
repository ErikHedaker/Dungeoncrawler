#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <fstream>
#include <string>

bool Game::ExistingGame( ) const
{
	return _dungeons.size( ) != 0 && _player.status != PlayerStatus::Dead;
}
void Game::SetDungeonConfiguration( const GameConfig& type )
{
	switch( type )
	{
		case GameConfig::Default:
		{
			_config = DungeonConfiguration( );

			break;
		}
		case GameConfig::Configure:
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

	auto maxCol = _dungeons[_indexCurrent].GetSize( ).first;
	auto maxRow = _dungeons[_indexCurrent].GetSize( ).second;
	_dungeons[_indexCurrent].CreatePlayerLocal( { maxCol / 2, maxRow / 2 }, _player );
}
void Game::GameLoop( )
{
	_player.status = PlayerStatus::Wandering;

	while( _player.status == PlayerStatus::Wandering )
	{
		system( "CLS" );
		PrintDungeonCentered( _dungeons[_indexCurrent], _player.visionReach, _player.position );
		PlayerTurn( _dungeons[_indexCurrent] );
		_dungeons[_indexCurrent].RandomMovement( );
		_dungeons[_indexCurrent].HandleEvents( _player );

		if( _player.status == PlayerStatus::Traveling )
		{
			SwitchDungeon( );
			_player.status = PlayerStatus::Wandering;
		}
	}
}
void Game::SaveDungeons( )
{
	const std::string fileName = "2D-cmd-dungeoncrawler-save.txt";
	std::ofstream outFile;

	outFile.open( fileName, std::ios::out | std::ios::trunc );

	if( !outFile.is_open( ) )
	{
		throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
	}

	outFile << _indexCurrent << '\n';
	outFile << _dungeons.size( ) << '\n';

	for( const auto& dungeon : _dungeons )
	{
		const auto maxCol = dungeon.GetSize( ).first;
		const auto maxRow = dungeon.GetSize( ).second;
		Vector2i iterator;

		outFile << maxCol << '\n';
		outFile << maxRow << '\n';
		outFile << dungeon.links.size( ) << '\n';
		
		for( const auto& link : dungeon.links )
		{
			outFile << link.set << '\n';
			outFile << link.indexDungeon << '\n';
			outFile << link.exit.col << '\n';
			outFile << link.exit.row << '\n';
			outFile << link.entry.col << '\n';
			outFile << link.entry.row << '\n';
		}

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
					outFile << dungeon.GetVision( { iterator.col % maxCol, iterator.row } );
				}
			}

			outFile << '\n';
		}
	}
}
void Game::LoadDungeons( )
{
	const std::string fileName = "2D-cmd-dungeoncrawler-save.txt";
	std::string line;
	std::ifstream inFile;
	std::size_t dungeonCount;

	inFile.open( fileName, std::ios::in );

	if( !inFile.is_open( ) )
	{
		throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
	}

	_dungeons.clear( );

	try
	{
		std::getline( inFile, line );
		_indexCurrent = std::stoi( line );
	}
	catch( ... )
	{
		throw std::exception( std::string( "Could not read integers" ).c_str( ) );
	}

	std::getline( inFile, line );
	dungeonCount = std::stoi( line );

	for( std::size_t index = 0; index < dungeonCount; index++ )
	{
		std::vector<char> iconMap;
		std::vector<bool> visionMap;
		int maxCol;
		int maxRow;
		std::size_t linkCount;
		std::vector<Link> links;
		bool set;
		std::size_t indexDungeon;
		Vector2i exit;
		Vector2i entry;
		Vector2i iterator;

		try
		{
			std::getline( inFile, line );
			maxCol = std::stoi( line );

			std::getline( inFile, line );
			maxRow = std::stoi( line );

			std::getline( inFile, line );
			linkCount = std::stoi( line );

			for( std::size_t indexLink = 0; indexLink < linkCount; indexLink++ )
			{
				std::getline( inFile, line );
				set = line != "0";

				std::getline( inFile, line );
				indexDungeon = std::stoi( line );

				std::getline( inFile, line );
				exit.col = std::stoi( line );

				std::getline( inFile, line );
				exit.row = std::stoi( line );

				std::getline( inFile, line );
				entry.col = std::stoi( line );

				std::getline( inFile, line );
				entry.row = std::stoi( line );

				links.push_back( { set, indexDungeon, exit, entry } );
			}
		}
		catch( ... )
		{
			throw std::exception( std::string( "Could not read integers" ).c_str( ) );
		}

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
					Vector2i position( iterator.col % maxCol, iterator.row );

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

		_dungeons.emplace_back( maxCol, maxRow, visionMap, iconMap, _player );
		_dungeons.back( ).links = links;
	}
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

	static const std::vector<char> choices =
	{
		'W', 'w',
		'A', 'a',
		'S', 's',
		'D', 'd',
		'Q', 'q',
		'E', 'e',
		'F', 'f'
	};
	static const std::map<char, Orientation> direction =
	{
		{ 'W', Orientation::North }, { 'w', Orientation::North },
		{ 'A', Orientation::West  }, { 'a', Orientation::West  },
		{ 'S', Orientation::South }, { 's', Orientation::South },
		{ 'D', Orientation::East  }, { 'd', Orientation::East  }
	};
	const char choice = GetValidChar( "\nEnter choice: ", choices );

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
		case 'Q': case 'q':
		{
			break;
		}
		case 'E': case 'e':
		{
			_player.status = PlayerStatus::Inactive;

			break;
		}
		case 'F': case 'f':
		{
			dungeon.RotateDungeonClockwise( );
			LinkExitsRotateClockwise( _indexCurrent );

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
void Game::SwitchDungeon( )
{
	for( const auto& link : _dungeons[_indexCurrent].links )
	{
		if( link.entry == _player.position )
		{
			_indexCurrent = link.indexDungeon;
			_dungeons[_indexCurrent].CreatePlayerLocal( link.exit, _player );
			FullLinkDungeon( _indexCurrent );

			break;
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