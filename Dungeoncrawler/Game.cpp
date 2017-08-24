#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <cctype>

Game::Game( ) :
    _status( GameStatus::Menu ),
    _player( LoadPlayerDefault( LoadAbilities( ) ) ),
    _entityFactory( _player )
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
                    _dungeonSystem = LoadDungeonSystem( _entityFactory );
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
                _dungeonSystem.config = GetDungeonConfiguration( GameConfig::Default );
                system( "CLS" );
                std::cout << "Loading, please wait.";
                Reset( );
                Start( );

                break;
            }
            case '4':
            {
                _dungeonSystem.config = GetDungeonConfiguration( GameConfig::Configure );
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
    return _dungeonSystem.dungeons.size( ) != 0;
}

void Game::Reset( )
{
    _player = LoadPlayerDefault( LoadAbilities( ) );
    _dungeonSystem.dungeons.clear( );
    _dungeonSystem.dungeons.emplace_back( _entityFactory, _dungeonSystem.config );
    _dungeonSystem.indexCurrent = 0;
    DungeonLink( 0 );
    _dungeonSystem.dungeons[0].PlayerAdd( _entityFactory, _dungeonSystem.dungeons[0].GetSize( ) / 2 );
}
void Game::Start( )
{
    while( _status == GameStatus::Playing &&
           _player.health > 0  )
    {
        TurnPlayer( _dungeonSystem.dungeons[_dungeonSystem.indexCurrent] );
        _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].MovementRandom( );
        _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].NextTurn( );
        _player.Update( );

        if( _player.states & States::Switch )
        {
            _player.states &= ~States::Switch;
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
        PrintDungeonCentered( dungeon, _player.visionReach, dungeon.GetPlayerPosition( ) );
        PrintHealth( _player );
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
                SaveDungeonSystem( _dungeonSystem );
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
                DungeonRotate( _dungeonSystem.indexCurrent, Orientation::East );

                break;
            }
            case 'G':
            {
                DungeonRotate( _dungeonSystem.indexCurrent, Orientation::South );

                break;
            }
            case 'H':
            {
                DungeonRotate( _dungeonSystem.indexCurrent, Orientation::West );

                break;
            }
        }
    }
}
void Game::DungeonLink( int indexCurrentDungeon )
{
    const int amount = _dungeonSystem.dungeons[indexCurrentDungeon].links.size( );

    for( int indexCurrentLink = 0; indexCurrentLink < amount; indexCurrentLink++ )
    {
        if( _dungeonSystem.dungeons[indexCurrentDungeon].links[indexCurrentLink].indexLink < 0 &&
            _dungeonSystem.dungeons[indexCurrentDungeon].links[indexCurrentLink].indexDungeon < 0 )
        {
            _dungeonSystem.dungeons.emplace_back( _entityFactory, _dungeonSystem.config );

            const int indexPartnerLink = 0;
            const int indexPartnerDungeon = _dungeonSystem.dungeons.size( ) - 1;
            auto& partner = _dungeonSystem.dungeons[indexPartnerDungeon].links[indexPartnerLink];
            auto& current = _dungeonSystem.dungeons[indexCurrentDungeon].links[indexCurrentLink];

            partner = { indexCurrentDungeon, indexCurrentLink, current.entry, partner.entry };
            current = { indexPartnerDungeon, indexPartnerLink, partner.entry, current.entry };
        }
    }
}
void Game::DungeonRotate( int indexDungeon, const Orientation::Enum& orientation )
{
    const Vector2<int> sizeOld = _dungeonSystem.dungeons[indexDungeon].GetSize( );

    _dungeonSystem.dungeons[indexDungeon].Rotate( orientation );

    for( auto& current : _dungeonSystem.dungeons[indexDungeon].links )
    {
        auto& partner = _dungeonSystem.dungeons[current.indexDungeon].links[current.indexLink];

        current.entry = PositionRotate( current.entry, sizeOld, orientation );
        partner.exit  = PositionRotate( partner.exit,  sizeOld, orientation );
    }
}
void Game::DungeonSwitch( )
{
    const int amount = _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].links.size( );

    for( int i = 0; i < amount; i++ )
    {
        if( _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].links[i].entry ==
            _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].GetPlayerPosition( ) )
        {
            const int indexOld = _dungeonSystem.indexCurrent;
            const int indexNew = _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].links[i].indexDungeon;

            DungeonLink( indexNew );
            _dungeonSystem.indexCurrent = indexNew;
            _dungeonSystem.dungeons[indexNew].PlayerAdd( _entityFactory, _dungeonSystem.dungeons[indexOld].links[i].exit );
            DungeonRotate( indexNew, static_cast<Orientation::Enum>( (
                _dungeonSystem.dungeons[indexOld].GetQuadrant( _dungeonSystem.dungeons[indexOld].GetPlayerPosition( ) ) -
                _dungeonSystem.dungeons[indexNew].GetQuadrant( _dungeonSystem.dungeons[indexNew].GetPlayerPosition( ) ) + 2 ) % 4 ) );

            break;
        }
    }
}