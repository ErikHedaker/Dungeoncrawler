#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>

Game::Game( ) :
    _player( 200, 70, 1, 50, Spells::TouchOfDeath | Spells::Fireball | Spells::Iceblast )
{ }

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

    while( true )
    {
        system( "CLS" );
        std::cout << "[1] Continue current game\n";
        std::cout << "[2] Load game from file\n";
        std::cout << "[3] Build new game (Randomization)\n";
        std::cout << "[4] Build new game (Configuration)\n";
        std::cout << "[5] Exit\n\n";

        const char choice = GetValidChar( "Enter choice: ", choices );

        _status = GameStatus::Neutral;

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

                if( Load( ) )
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

            _config.sizeDungeonFixed       = GetBool( GetValidChar( "Fixed dungeon size, [Y/N]: ",        choices ) );
            _config.generateDoors          = GetBool( GetValidChar( "Generate doors, [Y/N]: ",            choices ) );
            _config.generateOuterWalls     = GetBool( GetValidChar( "Generate outer obstacles, [Y/N]: ",  choices ) );
            _config.generatePath           = GetBool( GetValidChar( "Generate path, [Y/N]: ",             choices ) );
            _config.generateSourceWalls    = GetBool( GetValidChar( "Generate source obstacles, [Y/N]: ", choices ) );
            _config.generateExtensionWalls = GetBool( GetValidChar( "Generate extension, [Y/N]: ",        choices ) );
            _config.generateFillerWalls    = GetBool( GetValidChar( "Generate filler obstacles, [Y/N]: ", choices ) );
            _config.generateMonsters       = GetBool( GetValidChar( "Generate monsters, [Y/N]: ",         choices ) );

            std::cout << "\n";

            if( _config.sizeDungeonFixed )
            {
                _config.sizeDungeon.x           = GetPositiveInteger( "Enter dungeon width: " );
                _config.sizeDungeon.y           = GetPositiveInteger( "Enter dungeon height: " );
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
void Game::Reset( )
{
    _player.status = PlayerStatus::Wandering;
    _player.health = _player.healthMax;
    _dungeons.clear( );
    _dungeons.emplace_back( _config );
    _indexCurrent = _dungeons.size( ) - 1;
    FullLinkDungeon( _indexCurrent );
    _dungeons[_indexCurrent].CreatePlayerLocal( _dungeons[_indexCurrent].GetSize( ) / 2, _player );
}
void Game::Start( )
{
    while( _status == GameStatus::Neutral &&
           _player.status != PlayerStatus::Dead )
    {
        PlayerTurn( _dungeons[_indexCurrent] );
        _dungeons[_indexCurrent].MovementRandom( );
        _dungeons[_indexCurrent].CheckEvents( _player );
        CheckEventsPlayer( );
    }
}

void Game::Save( )
{
    const std::string fileName = "DungeoncrawlerSave.txt";
    std::ofstream outFile( fileName, std::ios::out | std::ios::trunc );

    if( !outFile.is_open( ) )
    {
        return;
    }

    outFile << _config.sizeDungeonFixed << '\t';
    outFile << _config.sizeDungeon.x << '\t';
    outFile << _config.sizeDungeon.x << '\t';
    outFile << _config.generateDoors << '\t';
    outFile << _config.generateOuterWalls << '\t';
    outFile << _config.generatePath << '\t';
    outFile << _config.generateSourceWalls << '\t';
    outFile << _config.generateExtensionWalls << '\t';
    outFile << _config.generateFillerWalls << '\t';
    outFile << _config.generateMonsters << '\t';
    outFile << _config.amountDoors << '\t';
    outFile << _config.amountSourceWalls << '\t';
    outFile << _config.amountExtensionWalls << '\t';
    outFile << _config.amountFillerWallsCycles << '\t';
    outFile << _config.amountMonsters << '\n';

    outFile << _indexCurrent << '\n';
    outFile << _dungeons.size( ) << '\n';

    for( const auto& dungeon : _dungeons )
    {
        const Vector2<int> sizeDungeon = dungeon.GetSize( );
        Vector2<int> iterator;

        outFile << sizeDungeon.x << '\t';
        outFile << sizeDungeon.y << '\n';

        for( iterator.y = 0; iterator.y < sizeDungeon.y; iterator.y++ )
        {
            for( iterator.x = 0; iterator.x < sizeDungeon.x * 2; iterator.x++ )
            {
                if( iterator.x < sizeDungeon.x )
                {
                    outFile << dungeon.GetTile( iterator ).icon;
                }
                else
                {
                    outFile << dungeon.GetVision( { iterator.x % sizeDungeon.x, iterator.y } );
                }
            }

            outFile << '\n';
        }

        outFile << dungeon.links.size( ) << '\n';

        for( const auto& link : dungeon.links )
        {
            outFile << link.indexDungeon << '\t';
            outFile << link.indexLink << '\t';
            outFile << link.exit.x << '\t';
            outFile << link.exit.y << '\t';
            outFile << link.entry.x << '\t';
            outFile << link.entry.y << '\n';
        }
    }
}
bool Game::Load( )
{
    try
    {
        const std::string fileName = "DungeoncrawlerSave.txt";
        std::ifstream inFile( fileName, std::ios::in );
        std::string line;

        if( !inFile.is_open( ) )
        {
            throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
        }

        _dungeons.clear( );

        std::getline( inFile, line );
        std::vector<int> configArgs( ( std::istream_iterator<int>( std::stringstream( line ) ) ), std::istream_iterator<int>( ) );

        _config.sizeDungeonFixed        = configArgs[0] != 0;
        _config.sizeDungeon.x           = configArgs[1];
        _config.sizeDungeon.y           = configArgs[2];
        _config.generateDoors           = configArgs[3] != 0;
        _config.generateOuterWalls      = configArgs[4] != 0;
        _config.generatePath            = configArgs[5] != 0;
        _config.generateSourceWalls     = configArgs[6] != 0;
        _config.generateExtensionWalls  = configArgs[7] != 0;
        _config.generateFillerWalls     = configArgs[8] != 0;
        _config.generateMonsters        = configArgs[9] != 0;
        _config.amountDoors             = configArgs[10];
        _config.amountSourceWalls       = configArgs[11];
        _config.amountExtensionWalls    = configArgs[12];
        _config.amountFillerWallsCycles = configArgs[13];
        _config.amountMonsters          = configArgs[14];

        std::getline( inFile, line );
        _indexCurrent = std::stoi( line );

        std::getline( inFile, line );
        const int dungeonCount = std::stoi( line );

        for( int index = 0; index < dungeonCount; index++ )
        {
            Vector2<int> sizeDungeon;
            Vector2<int> iterator;

            std::getline( inFile, line, '\t' );
            sizeDungeon.x = std::stoi( line );

            std::getline( inFile, line );
            sizeDungeon.y = std::stoi( line );

            std::vector<char> iconMap( sizeDungeon.x * sizeDungeon.y );
            std::vector<bool> visionMap( sizeDungeon.x * sizeDungeon.y );

            for( iterator.y = 0; iterator.y < sizeDungeon.y; iterator.y++ )
            {
                std::getline( inFile, line );

                for( iterator.x = 0; iterator.x < sizeDungeon.x * 2; iterator.x++ )
                {
                    if( iterator.x < sizeDungeon.x )
                    {
                        switch( line[iterator.x] )
                        {
                            case Icon::Player:
                            {
                                iconMap[( iterator.y * sizeDungeon.x ) + iterator.x] = Icon::Player;

                                break;
                            }
                            case Icon::Monster:
                            {
                                iconMap[( iterator.y * sizeDungeon.x ) + iterator.x] = Icon::Monster;

                                break;
                            }
                            case Icon::Door:
                            {
                                iconMap[( iterator.y * sizeDungeon.x ) + iterator.x] = Icon::Door;

                                break;
                            }
                            case Icon::Wall:
                            {
                                iconMap[( iterator.y * sizeDungeon.x ) + iterator.x] = Icon::Wall;

                                break;
                            }
                            case Icon::Ground:
                            {
                                iconMap[( iterator.y * sizeDungeon.x ) + iterator.x] = Icon::Ground;

                                break;
                            }
                            default:
                            {
                                throw std::exception( std::string( "Could not read tile" ).c_str( ) );
                            }
                        }
                    }
                    else
                    {
                        const Vector2<int> position = { iterator.x % sizeDungeon.x, iterator.y };

                        switch( line[iterator.x] )
                        {
                            case '1':
                            {
                                visionMap[( position.y * sizeDungeon.x ) + position.x] = true;

                                break;
                            }
                            case '0':
                            {
                                visionMap[( position.y * sizeDungeon.x ) + position.x] = false;

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

            _dungeons.emplace_back( sizeDungeon, visionMap, iconMap, _player );

            std::getline( inFile, line );
            const int linkCount = std::stoi( line );

            for( int indexLink = 0; indexLink < linkCount; indexLink++ )
            {
                std::getline( inFile, line );
                std::vector<int> linkArgs( ( std::istream_iterator<int>( std::stringstream( line ) ) ), std::istream_iterator<int>( ) );
                _dungeons.back( ).links.push_back( { linkArgs[0], linkArgs[1], { linkArgs[2], linkArgs[3] }, { linkArgs[4], linkArgs[5] } } );
            }
        }
    }
    catch( const std::exception& e )
    {
        std::cout << "\n\nException: " << e.what( );
        std::cout << "\n\nPress enter to continue: ";
        GetEnter( );

        return false;
    }

    return true;
}

void Game::PlayerTurn( Dungeon& dungeon )
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
    static const std::map<char, Orientation> direction =
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
        PrintDungeonCentered( dungeon, _player.visionReach, _player.position );
        PrintCombatantInformation( _player );
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
                dungeon.MovementPlayer( direction.at( choice ) );

                break;
            }
            case 'E': case 'e':
            {
                done = true;
                _status = GameStatus::Menu;
                Save( );

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
                LinksRotateClockwise( _indexCurrent );
                _player.position = PositionRotateClockwise( _player.position, dungeon.GetSize( ) );

                break;
            }
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
        _player.status = ( _player.health <= 0 ? PlayerStatus::Dead : PlayerStatus::Wandering );
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
void Game::FullLinkDungeon( int indexCurrentDungeon )
{
    const int amountLinks = _dungeons[indexCurrentDungeon].links.size( );

    for( int indexCurrentLink = 0; indexCurrentLink < amountLinks; indexCurrentLink++ )
    {
        if( _dungeons[indexCurrentDungeon].links[indexCurrentLink].indexLink < 0 )
        {
            _dungeons.emplace_back( _config );

            const int indexPartnerDungeon = _dungeons.size( ) - 1;
            const int indexPartnerLink = 0;
            auto& current = _dungeons[indexCurrentDungeon].links[indexCurrentLink];
            auto& partner = _dungeons[indexPartnerDungeon].links[indexPartnerLink];

            current = { indexPartnerDungeon, indexPartnerLink, partner.entry, current.entry };
            partner = { indexCurrentDungeon, indexCurrentLink, current.entry, partner.entry };
        }
    }
}
void Game::LinksRotateClockwise( int indexDungeon )
{
    for( auto& current : _dungeons[indexDungeon].links )
    {
        auto& partner = _dungeons[current.indexDungeon].links[current.indexLink];

        current.entry = PositionRotateClockwise( current.entry, _dungeons[indexDungeon].GetSize( ) );
        partner.exit  = PositionRotateClockwise( partner.exit,  _dungeons[indexDungeon].GetSize( ) );
    }
}