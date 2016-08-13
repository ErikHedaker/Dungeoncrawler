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
    while( true )
    {
        system( "CLS" );
        std::cout << "[1] Continue current game\n";
        std::cout << "[2] Load game from file\n";
        std::cout << "[3] Build new game (Randomization)\n";
        std::cout << "[4] Build new game (Configuration)\n";
        std::cout << "[5] Exit\n\n";
        const char choice = GetChar( "Enter choice: ", { '1', '2', '3', '4', '5' } );
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
                SetDungeonConfiguration( GameConfig::Default );
                system( "CLS" );
                std::cout << "Loading, please wait.";
                Reset( );
                Start( );

                break;
            }
            case '4':
            {
                SetDungeonConfiguration( GameConfig::Configure );
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

void Game::SetDungeonConfiguration( const GameConfig& type )
{
    switch( type )
    {
        case GameConfig::Default:
        {
            _dungeonSystem.config = DungeonConfiguration( );

            break;
        }
        case GameConfig::Configure:
        {
            auto ToBool = [] ( char input ) -> bool
            {
                return 
                    input == 'Y' ||
                    input == 'y';
            };

            std::cout << "\n";
            _dungeonSystem.config.sizeDungeonFixed       = ToBool( GetChar( "Fixed dungeon size, [Y/N]: ",       { 'Y', 'N' }, std::toupper ) );
            _dungeonSystem.config.generateDoors          = ToBool( GetChar( "Generate doors, [Y/N]: ",           { 'Y', 'N' }, std::toupper ) );
            _dungeonSystem.config.generateOuterWalls     = ToBool( GetChar( "Generate outer walls, [Y/N]: ",     { 'Y', 'N' }, std::toupper ) );
            _dungeonSystem.config.generateHiddenPath     = ToBool( GetChar( "Generate hidden path, [Y/N]: ",     { 'Y', 'N' }, std::toupper ) );
            _dungeonSystem.config.generateSourceWalls    = ToBool( GetChar( "Generate source walls, [Y/N]: ",    { 'Y', 'N' }, std::toupper ) );
            _dungeonSystem.config.generateExtensionWalls = ToBool( GetChar( "Generate extension walls, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
            _dungeonSystem.config.generateFillerWalls    = ToBool( GetChar( "Generate filler walls, [Y/N]: ",    { 'Y', 'N' }, std::toupper ) );
            _dungeonSystem.config.generateMonsters       = ToBool( GetChar( "Generate monsters, [Y/N]: ",        { 'Y', 'N' }, std::toupper ) );
            std::cout << "\n";

            if( _dungeonSystem.config.sizeDungeonFixed )
            {
                _dungeonSystem.config.sizeDungeon.x           = GetPositiveInteger( "Enter dungeon width: " );
                _dungeonSystem.config.sizeDungeon.y           = GetPositiveInteger( "Enter dungeon height: " );
            }
            if( _dungeonSystem.config.generateDoors )
                _dungeonSystem.config.amountDoors             = GetPositiveInteger( "Enter amount of doors: " );
            if( _dungeonSystem.config.generateSourceWalls )
                _dungeonSystem.config.amountSourceWalls       = GetPositiveInteger( "Enter amount of source walls: " );
            if( _dungeonSystem.config.generateExtensionWalls )
                _dungeonSystem.config.amountExtensionWalls    = GetPositiveInteger( "Enter amount of extension walls: " );
            if( _dungeonSystem.config.generateFillerWalls )
                _dungeonSystem.config.amountFillerWallsCycles = GetPositiveInteger( "Enter amount of filler wall cycles: " );
            if( _dungeonSystem.config.generateMonsters )
                _dungeonSystem.config.amountMonsters          = GetPositiveInteger( "Enter amount of monsters: " );

            break;
        }
    }
}
void Game::Reset( )
{
    _player = LoadPlayerDefault( LoadAbilities( ) );
    _dungeonSystem.dungeons.clear( );
    _dungeonSystem.dungeons.emplace_back( _entityFactory, _dungeonSystem.config );
    _dungeonSystem.indexCurrent = 0;
    DungeonLink( 0 );
    _dungeonSystem.dungeons[0].PlayerAdd( _dungeonSystem.dungeons[0].GetSize( ) / 2 );
}
void Game::Start( )
{
    while( _status == GameStatus::Playing &&
           _player.health > 0  )
    {
        TurnPlayer( _dungeonSystem.dungeons[_dungeonSystem.indexCurrent] );
        _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].MovementRandom( );
        _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].Events( );
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
    static const std::map<char, Orientation::OrientationType> directions =
    {
        { 'W', Orientation::North },
        { 'A', Orientation::West  },
        { 'S', Orientation::South },
        { 'D', Orientation::East  },
    };
    bool done = false;

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
        std::cout << "[E] Save and exit to meny\n";
        std::cout << "[R] Exit to meny\n";
        std::cout << "[F] Rotate dungeon\n";
        const char choice = GetChar( "Enter choice: ", { 'W', 'A', 'S', 'D', 'E', 'R', 'F' }, std::toupper );

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
                _status = GameStatus::Menu;
                SaveDungeonSystem( _dungeonSystem );
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
void Game::DungeonRotate( int indexDungeon, const Orientation::OrientationType& orientation )
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
            _dungeonSystem.dungeons[indexNew].PlayerAdd( _dungeonSystem.dungeons[indexOld].links[i].exit );
            DungeonRotate( indexNew, static_cast<Orientation::OrientationType>( (
                _dungeonSystem.dungeons[indexOld].GetQuadrant( _dungeonSystem.dungeons[indexOld].GetPlayerPosition( ) ) -
                _dungeonSystem.dungeons[indexNew].GetQuadrant( _dungeonSystem.dungeons[indexNew].GetPlayerPosition( ) ) + 2 ) % 4 ) );

            break;
        }
    }
}