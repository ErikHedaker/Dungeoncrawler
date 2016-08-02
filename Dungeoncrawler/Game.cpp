#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>

Game::Game( ) :
    _entityLibrary( ),
    _player( LoadPlayer( _entityLibrary.abilities ) )
{
    _entityLibrary.player.reset( new PlayerEntity( _player ) );
}

void Game::Menu( )
{
    const std::vector<char> choices =
    {
        '1',
        '2',
        '3',
        '4',
        '5'
    };
    char choice;

    while( true )
    {
        _status = GameStatus::Neutral;

        system( "CLS" );
        std::cout << "[1] Continue current game\n";
        std::cout << "[2] Load game from file\n";
        std::cout << "[3] Build new game (Randomization)\n";
        std::cout << "[4] Build new game (Configuration)\n";
        std::cout << "[5] Exit\n\n";
        choice = GetValidChar( "Enter choice: ", choices );

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
                LoadDungeonSystem( _entityLibrary );

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

            _dungeonSystem.config.sizeDungeonFixed       = GetBool( GetValidChar( "Fixed dungeon size, [Y/N]: ",       choices ) );
            _dungeonSystem.config.generateDoors          = GetBool( GetValidChar( "Generate doors, [Y/N]: ",           choices ) );
            _dungeonSystem.config.generateOuterWalls     = GetBool( GetValidChar( "Generate outer walls, [Y/N]: ",     choices ) );
            _dungeonSystem.config.generateHiddenPath     = GetBool( GetValidChar( "Generate hidden path, [Y/N]: ",     choices ) );
            _dungeonSystem.config.generateSourceWalls    = GetBool( GetValidChar( "Generate source walls, [Y/N]: ",    choices ) );
            _dungeonSystem.config.generateExtensionWalls = GetBool( GetValidChar( "Generate extension walls, [Y/N]: ", choices ) );
            _dungeonSystem.config.generateFillerWalls    = GetBool( GetValidChar( "Generate filler walls, [Y/N]: ",    choices ) );
            _dungeonSystem.config.generateMonsters       = GetBool( GetValidChar( "Generate monsters, [Y/N]: ",        choices ) );

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
    _player.states = 0;
    _player.health = _player.healthMax;
    _dungeonSystem.dungeons.clear( );
    _dungeonSystem.dungeons.emplace_back( _entityLibrary, _dungeonSystem.config );
    _dungeonSystem.indexCurrent = 0;
    DungeonLink( 0 );
    _dungeonSystem.dungeons[0].PlayerAdd( _dungeonSystem.dungeons[0].GetSize( ) / 2 );
}
void Game::Start( )
{
    while( _status == GameStatus::Neutral &&
           !( _player.states & States::Dead ) )
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
    static const std::vector<char> choices =
    {
        'W', 'w',
        'A', 'a',
        'S', 's',
        'D', 'd',
        'E', 'e',
        'R', 'r',
        'F', 'f'
    };
    static const std::map<char, Orientation> directions =
    {
        { 'W', Orientation::North }, { 'w', Orientation::North },
        { 'A', Orientation::West  }, { 'a', Orientation::West  },
        { 'S', Orientation::South }, { 's', Orientation::South },
        { 'D', Orientation::East  }, { 'd', Orientation::East  }
    };
    char choice;
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
        std::cout << "[F] Rotate dungeon 90 degrees clockwise\n\n";
        choice = GetValidChar( "Enter choice: ", choices );

        switch( choice )
        {
            case 'W': case 'w':
            case 'A': case 'a':
            case 'S': case 's':
            case 'D': case 'd':
            {
                done = true;
                dungeon.MovementPlayer( directions.at( choice ) );

                break;
            }
            case 'E': case 'e':
            {
                done = true;
                _status = GameStatus::Menu;
                SaveDungeonSystem( _dungeonSystem );

                break;
            }
            case 'R': case 'r':
            {
                done = true;
                _status = GameStatus::Menu;

                break;
            }
            case 'F': case 'f':
            {
                dungeon.RotateClockwise( );
                LinksRotateClockwise( _dungeonSystem.indexCurrent );

                break;
            }
        }
    }
}
void Game::DungeonSwitch( )
{
    for( const auto& link : _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].links )
    {
        if( link.entry == _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].GetPlayerPosition( ) )
        {
            _dungeonSystem.indexCurrent = link.indexDungeon;
            _dungeonSystem.dungeons[_dungeonSystem.indexCurrent].PlayerAdd( link.exit );
            DungeonLink( _dungeonSystem.indexCurrent );

            break;
        }
    }
}
void Game::DungeonLink( int indexCurrentDungeon )
{
    const int amountLinks = _dungeonSystem.dungeons[indexCurrentDungeon].links.size( );

    for( int indexCurrentLink = 0; indexCurrentLink < amountLinks; indexCurrentLink++ )
    {
        if( _dungeonSystem.dungeons[indexCurrentDungeon].links[indexCurrentLink].indexLink < 0 )
        {
            _dungeonSystem.dungeons.emplace_back( _entityLibrary, _dungeonSystem.config );

            const int indexPartnerDungeon = _dungeonSystem.dungeons.size( ) - 1;
            const int indexPartnerLink = 0;
            auto& current = _dungeonSystem.dungeons[indexCurrentDungeon].links[indexCurrentLink];
            auto& partner = _dungeonSystem.dungeons[indexPartnerDungeon].links[indexPartnerLink];

            current = { indexPartnerDungeon, indexPartnerLink, partner.entry, current.entry };
            partner = { indexCurrentDungeon, indexCurrentLink, current.entry, partner.entry };
        }
    }
}
void Game::LinksRotateClockwise( int indexDungeon )
{
    for( auto& current : _dungeonSystem.dungeons[indexDungeon].links )
    {
        auto& partner = _dungeonSystem.dungeons[current.indexDungeon].links[current.indexLink];

        current.entry = PositionRotateClockwise( current.entry, _dungeonSystem.dungeons[indexDungeon].GetSize( ) );
        partner.exit  = PositionRotateClockwise( partner.exit,  _dungeonSystem.dungeons[indexDungeon].GetSize( ) );
    }
}