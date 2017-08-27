#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <cctype>
#include <memory>

Game::Game( ) :
    _player( LoadPlayerDefault( LoadAbilities( ) ) ),
    _status( GameStatus::Menu ),
    _entityFactory( )
{ }

void Game::Menu( )
{
    char choice;

    while( true )
    {
        system( "CLS" );
        std::cout << "[1] Continue current game\n";
        std::cout << "[2] Load game from file\n";
        std::cout << "[3] Build new game (Randomization)\n";
        std::cout << "[4] Build new game (Configuration)\n";
        std::cout << "[5] Exit\n\n";
        choice = GetChar( "Enter choice: ", { '1', '2', '3', '4', '5' } );
        _status = GameStatus::Playing;

        switch( choice )
        {
            case '1':
            {
                if( Exist( ) )
                {
                    Start( );
                }

                break;
            }
            case '2':
            {
                system( "CLS" );
                std::cout << "Loading, please wait.";

                try
                {
                    _config = LoadGameConfig( );
                    _dungeons = LoadGameDungeons( _player, _entityFactory, _index );
                }
                catch( const std::exception& e )
                {
                    std::cout << "\nError: " << e.what( );
                    std::cout << "\n\nPress enter to continue: ";
                    GetEnter( );
                }

                if( Exist( ) )
                {
                    Start( );
                }

                break;
            }
            case '3':
            {
                _config = GetDungeonConfiguration( GameConfig::Default );
                system( "CLS" );
                std::cout << "Loading, please wait.";
                Reset( );
                Start( );

                break;
            }
            case '4':
            {
                _config = GetDungeonConfiguration( GameConfig::Configure );
                system( "CLS" );
                std::cout << "Loading, please wait.";
                Reset( );
                Start( );

                break;
            }
            case '5':
            {
                return;
            }
        }
    }
}
bool Game::Exist( ) const
{
    return _dungeons.size( ) != 0;
}

void Game::Reset( )
{
    *_player.real = LoadPlayerDefault( LoadAbilities( ) );
    _dungeons.clear( );
    _dungeons.emplace_back( _player, _entityFactory, _config );
    _index = 0;
    DungeonLink( 0 );
    _dungeons[0].PlayerPlace( _dungeons[0].GetSize( ) / 2 );
}
void Game::Start( )
{
    while( _status == GameStatus::Playing &&
           _player.real->health > 0  )
    {
        TurnPlayer( _dungeons[_index] );
        _dungeons[_index].MovementRandom( );
        _dungeons[_index].NextTurn( );
        _player.real->Update( );

        if( _player.real->states & States::Switch )
        {
            _player.real->states &= ~States::Switch;
            DungeonSwitch( );
        }
    }
}
void Game::TurnPlayer( Dungeon& dungeon )
{
    static const std::map<char, Orientation::Enum> directions =
    {
        { 'W', Orientation::North },
        { 'A', Orientation::West  },
        { 'S', Orientation::South },
        { 'D', Orientation::East  },
    };
    bool done = false;
    char choice;

    while( !done )
    {
        system( "CLS" );
        PrintDungeonCentered( dungeon, _player.real->visionReach, _player.real->position );
        PrintHealth( *_player.real );
        std::cout << "\n";
        std::cout << "[W] Go North\n";
        std::cout << "[A] Go West\n";
        std::cout << "[S] Go South\n";
        std::cout << "[D] Go East\n";
        std::cout << "[E] Exit to meny while saving\n";
        std::cout << "[R] Exit to meny without saving\n";
        std::cout << "[F] Rotate dungeon 90'\n";
        std::cout << "[G] Rotate dungeon 180'\n";
        std::cout << "[H] Rotate dungeon 270'\n\n";
        choice = GetChar( "Enter choice: ", { 'W', 'A', 'S', 'D', 'E', 'R', 'F', 'G', 'H' }, std::toupper );

        switch( choice )
        {
            case 'W':
            case 'A':
            case 'S':
            case 'D':
            {
                dungeon.MovementPlayer( directions.at( choice ) );
                done = true;

                break;
            }
            case 'E':
            {
                SaveGameConfig( _config );
                SaveGameDungeons( _dungeons, _index );
                _status = GameStatus::Menu;
                done = true;

                break;
            }
            case 'R':
            {
                _status = GameStatus::Menu;
                done = true;

                break;
            }
            case 'F':
            {
                DungeonRotate( _index, Orientation::East );

                break;
            }
            case 'G':
            {
                DungeonRotate( _index, Orientation::South );

                break;
            }
            case 'H':
            {
                DungeonRotate( _index, Orientation::West );

                break;
            }
        }
    }
}
void Game::DungeonLink( int indexCurrentDungeon )
{
    const int amount = _dungeons[indexCurrentDungeon].links.size( );

    for( int indexCurrentLink = 0; indexCurrentLink < amount; indexCurrentLink++ )
    {
        if( _dungeons[indexCurrentDungeon].links[indexCurrentLink].indexLink < 0 &&
            _dungeons[indexCurrentDungeon].links[indexCurrentLink].indexDungeon < 0 )
        {
            _dungeons.emplace_back( _player, _entityFactory, _config );

            const int indexPartnerLink = 0;
            const int indexPartnerDungeon = _dungeons.size( ) - 1;
            auto& partner = _dungeons[indexPartnerDungeon].links[indexPartnerLink];
            auto& current = _dungeons[indexCurrentDungeon].links[indexCurrentLink];

            partner = { indexCurrentDungeon, indexCurrentLink, current.entry, partner.entry };
            current = { indexPartnerDungeon, indexPartnerLink, partner.entry, current.entry };
        }
    }
}
void Game::DungeonRotate( int indexDungeon, const Orientation::Enum& orientation )
{
    const Vector2<int> sizeOld = _dungeons[indexDungeon].GetSize( );

    _dungeons[indexDungeon].Rotate( orientation );

    for( auto& current : _dungeons[indexDungeon].links )
    {
        auto& partner = _dungeons[current.indexDungeon].links[current.indexLink];

        current.entry = PositionRotate( current.entry, sizeOld, orientation );
        partner.exit  = PositionRotate( partner.exit,  sizeOld, orientation );
    }
}
void Game::DungeonSwitch( )
{
    const int amount = _dungeons[_index].links.size( );

    for( int i = 0; i < amount; i++ )
    {
        if( _dungeons[_index].links[i].entry == _player.real->position )
        {
            const int indexOld = _index;
            const int indexNew = _dungeons[_index].links[i].indexDungeon;

            DungeonLink( indexNew );
            _index = indexNew;
            _dungeons[indexNew].PlayerPlace( _dungeons[indexOld].links[i].exit );
            DungeonRotate( indexNew, static_cast<Orientation::Enum>( (
                _dungeons[indexOld].GetQuadrant( _dungeons[indexOld].links[i].entry ) -
                _dungeons[indexNew].GetQuadrant( _dungeons[indexOld].links[i].exit ) + 2 ) % 4 ) );

            break;
        }
    }
}