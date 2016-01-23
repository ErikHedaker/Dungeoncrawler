#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>

Game::Game( ) :
	existingGame( false ),
	_dungeonCurrent( nullptr ),
	_status( GameStatus::Neutral )
{ }

void Game::SetDungeonConfiguration( )
{
	/* Disregard unholy mess of code */
	_config.SetFixedSeed( );
	if( _config.fixedSeed )
		_config.SetSeed( );
	_config.SetFixedDungeonSize( );
	if( _config.fixedDungeonSize )
		_config.SetDungeonSize( );
	_config.SetGenerateDoors( );
	if( _config.generateDoors )
		_config.SetAmountDoors( );
	_config.SetGenerateOuterObstacles( );
	_config.SetGeneratePath( );
	_config.SetGenerateSourceObstacles( );
	if( _config.generateSourceObstacles )
		_config.SetAmountSourceObstacles( );
	_config.SetGenerateExtensionObstacles( );
	if( _config.generateExtensionObstacles )
		_config.SetAmountExtensionObstacles( );
	_config.SetGenerateFillerObstacles( );
	if( _config.generateFillerObstacles )
		_config.SetAmountFillerObstacleCycles( );
	_config.SetGenerateMonsters( );
	if( _config.generateMonsters )
		_config.SetAmountMonsters( );
}
void Game::NewGame( const DungeonType& type )
{
	_status = GameStatus::Neutral;
	_player.reset( new Player( Vector2i( ) ) );
	_links.clear( );
	_dungeons.clear( );
	_type = type;
	existingGame = true;

	_dungeons.emplace_back( );
	_dungeonCurrent = &_dungeons.back( );
	_dungeonCurrent->GenerateDungeon( _config );
	FullLinkDungeon( _dungeonCurrent );
	_dungeonCurrent->PlayerAdd( _player.get( ), _dungeonCurrent->GetSize( ) / 2 );
}
void Game::GameLoop( )
{
	while( _status == GameStatus::Neutral )
	{
		_dungeonCurrent->UpdatePlayerVision( );

		OutputClearScreen( );
		OutputDungeonCentered( *_dungeonCurrent, _player->GetPosition( ) );
		OutputCharacterStatus( *_player );
		OutputTurnOptions( );

		PlayerTurn( *_dungeonCurrent );

		_dungeonCurrent->MonsterMovement( );
		_dungeonCurrent->HandleEvents( _status );
		_dungeonCurrent->RemoveDeadCharacters( _status );

		if( _status == GameStatus::Next )
		{
			Dungeon* dungeon;
			Entity* door;
			Entity* doorCurrent;

			for( const auto& door : _dungeonCurrent->GetDoors( ) )
			{
				if( _player->GetPosition( ) == door->GetPosition( ) )
				{
					doorCurrent = door;

					break;
				}
			}

			dungeon = _links[doorCurrent].first;
			door = _links[doorCurrent].second;
			FullLinkDungeon( dungeon );
			dungeon->PlayerAdd( _player.get( ), door );
			_dungeonCurrent = dungeon;
			_status = GameStatus::Neutral;
		}
	}
}

bool Game::DungeonFullyLinked( Dungeon* dungeon ) const
{
	const auto& doors = dungeon->GetDoors( );
	unsigned int doorsLinked = 0;

	for( const auto& door : doors )
	{
		if( _links.find( door ) != _links.end( ) )
		{
			std::cout << "\nLink exists";

			doorsLinked++;
		}
	}

	return doorsLinked >= dungeon->GetDoors( ).size( );
}
void Game::AddAndLinkDungeon( Dungeon* dungeonParent, Entity* doorParent )
{
	std::cout << "\nAddAndLinkDungeon";

	std::pair<Dungeon*, Entity*> parent;
	std::pair<Dungeon*, Entity*> child;
	Dungeon* dungeonChild;
	Entity* doorChild;

	_dungeons.emplace_back( );
	dungeonChild = &_dungeons.back( );
	dungeonChild->GenerateDungeon( _config );

	const auto& doors = dungeonChild->GetDoors( );

	for( const auto& door : doors )
	{
		if( _links.find( door ) == _links.end( ) )
		{
			doorChild = door;
			parent = std::make_pair( dungeonParent, doorParent );
			child = std::make_pair( dungeonChild, doorChild );
			_links[doorParent] = child;
			_links[doorChild] = parent;

			std::cout << "\nAdded link";

			break;
		}
	}
}
void Game::FullLinkDungeon( Dungeon* dungeon )
{
	while( !DungeonFullyLinked( dungeon ) )
	{
		const auto& doors = dungeon->GetDoors( );

		for( const auto& door : doors )
		{
			if( _links.find( door ) == _links.end( ) )
			{
				AddAndLinkDungeon( dungeon, door );
			}
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