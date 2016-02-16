#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>

Game::Game( ) :
	_player( 100, 100, 1, 50, Spells::Fireball | Spells::Iceblast )
{ }

bool Game::ExistingGame( ) const
{
	return _dungeons.size( ) != 0;
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
			const std::vector<char> choices =
			{ 
				'Y', 'y',
				'N', 'n'
			};
			auto GetBool = [] ( char input ) -> bool
			{
				return 
					input == 'Y' ||
					input == 'y';
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
	_player.status = PlayerStatus::Wandering;
	_player.health = 100;
	_dungeons.clear( );
	_dungeons.emplace_back( _config );
	_indexCurrent = _dungeons.size( ) - 1;
	FullLinkDungeon( _indexCurrent );

	const int maxCol = _dungeons[_indexCurrent].GetSize( ).first;
	const int maxRow = _dungeons[_indexCurrent].GetSize( ).second;
	const Vector2i center = { maxCol / 2, maxRow / 2 };

	_dungeons[_indexCurrent].CreatePlayerLocal( center, _player );
}
void Game::GameLoop( )
{
	_status = GameStatus::Gameloop;

	while( _status == GameStatus::Gameloop &&
		   _player.status != PlayerStatus::Dead )
	{
		system( "CLS" );
		PrintDungeonCentered( _dungeons[_indexCurrent], _player.visionReach, _player.position );
		PrintCombatantInformation( _player );
		PlayerTurn( _dungeons[_indexCurrent] );
		_dungeons[_indexCurrent].RandomMovement( );
		_dungeons[_indexCurrent].CheckEvents( _player );
		CheckEventsPlayer( );
	}
}
void Game::SaveDungeons( )
{
	const std::string fileName = "2D-cmd-dungeoncrawler-save.txt";
	std::ofstream outFile( fileName, std::ios::out | std::ios::trunc );

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

		outFile << maxCol << '\t';
		outFile << maxRow << '\n';
		outFile << dungeon.links.size( ) << '\n';
		
		for( const auto& link : dungeon.links )
		{
			outFile << link.indexDungeon << '\t';
			outFile << link.exit.col << '\t';
			outFile << link.exit.row << '\t';
			outFile << link.entry.col << '\t';
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
	std::ifstream inFile( fileName, std::ios::in );
	std::string line;
	std::size_t dungeonCount;

	if( !inFile.is_open( ) )
	{
		throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
	}

	_dungeons.clear( );

	try
	{
		std::getline( inFile, line );
		_indexCurrent = std::stoi( line );

		std::getline( inFile, line );
		dungeonCount = std::stoi( line );
	}
	catch( ... )
	{
		throw std::exception( std::string( "Could not read integers" ).c_str( ) );
	}

	for( std::size_t index = 0; index < dungeonCount; index++ )
	{
		std::vector<char> iconMap;
		std::vector<bool> visionMap;
		int maxCol;
		int maxRow;
		std::size_t linkCount;
		std::vector<Link> links;
		Vector2i iterator;

		try
		{
			std::getline( inFile, line, '\t' );
			maxCol = std::stoi( line );

			std::getline( inFile, line );
			maxRow = std::stoi( line );

			std::getline( inFile, line );
			linkCount = std::stoi( line );

			for( std::size_t indexLink = 0; indexLink < linkCount; indexLink++ )
			{
				std::getline( inFile, line );
				std::stringstream lineStream( line );
				std::vector<int> numbers( ( std::istream_iterator<int>( lineStream ) ), std::istream_iterator<int>( ) );
				links.push_back( { numbers[0], { numbers[1], numbers[2] }, { numbers[3], numbers[4] } } );
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
	std::cout << "\n";
	std::cout << "[W] Go North\n";
	std::cout << "[A] Go West\n";
	std::cout << "[S] Go South\n";
	std::cout << "[D] Go East\n";
	std::cout << "[Q] Stand still\n";
	std::cout << "[E] Exit to meny\n";
	std::cout << "[F] Rotate dungeon 90 degrees clockwise\n\n";

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
	const char choice = GetValidChar( "Enter choice: ", choices );

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
			_status = GameStatus::Menu;

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
void Game::CheckEventsPlayer( )
{
	_player.Update( );

	if( _player.status == PlayerStatus::Traveling )
	{
		SwitchDungeon( );
		_player.status = PlayerStatus::Wandering;
	}

	if( _player.status == PlayerStatus::Combat )
	{
		_battleSystem.EngageRandomMonster( _player );

		if( _player.health <= 0 )
		{
			_player.status = PlayerStatus::Dead;
		}
		else
		{
			_player.status = PlayerStatus::Wandering;
		}
	}
}
void Game::FullLinkDungeon( std::size_t indexDungeon )
{
	const Vector2i notSet = { -1, -1 };

	for( std::size_t index = 0; index < _dungeons[indexDungeon].links.size( ); index++ )
	{
		if( _dungeons[indexDungeon].links[index].exit == notSet )
		{
			std::cout << "\nAdding link\n\n";

			_dungeons.emplace_back( _config );

			const std::size_t indexDungeonNeighbor = _dungeons.size( ) - 1;
			auto& linkNeighbor = _dungeons.back( ).links.back( );

			_dungeons[indexDungeon].links[index].indexDungeon = indexDungeonNeighbor;
			linkNeighbor.indexDungeon = indexDungeon;

			_dungeons[indexDungeon].links[index].exit = linkNeighbor.entry;
			linkNeighbor.exit = _dungeons[indexDungeon].links[index].entry;
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
	const int maxCol = _dungeons[index].GetSize( ).first;

	for( const auto& linkCurrent : _dungeons[index].links )
	{
		for( auto& link : _dungeons[linkCurrent.indexDungeon].links )
		{
			if( link.indexDungeon == index )
			{
				link.exit = PositionRotateClockwise( link.exit, maxCol );
			}
		}
	}
}