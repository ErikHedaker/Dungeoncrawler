#include "Game.h"
#include "Grid.h"
#include "Functions.h"
#include "Dungeon.h"
#include "BattleSystem.h"
#include "EntityFactory.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include <chrono>
#include <vector>
#include <map>
#include <deque>

Game::Game( bool clear, bool save, bool exit, bool config ) :
    _clearOutput( clear ),
    _saveToFile( save ),
    _exitable( exit ),
    _customConfig( config ),
    _battleSystem( clear ),
    _player( _entityFactory.PlayerDefault( ) ),
    _index( -1 )
{ }

bool Game::Exist( ) const
{
    return _dungeons.size( ) != 0;
}
void Game::Menu( )
{
    std::string output;
    char input;

    while( true )
    {
        ClearScreen( _clearOutput );
        output.clear( );
        output
            .append( std::string( "[1] Continue current game" ) + ( Exist( ) ? "\n" : "\t[Disabled, no current game]\n" ) )
            .append( std::string( "[2] Load game from file" ) + ( _saveToFile ? "\n" : "\t\t[Disabled by server]\n" ) )
            .append( "[3] Build new game (Random)\n" )
            .append( std::string( "[4] Build new game (Config)" ) + ( _customConfig ? "\n" : "\t[Disabled by server]\n" ) )
            .append( std::string( "[5] Exit" ) + ( _exitable ? "\n" : "\t\t\t[Disabled by server]\n" ) )
            .append( "Select option: \n" );
        std::cout << output;
        input = SelectChar( { Exist() ? '1' : '\0', _saveToFile ? '2' : '\0', '3', _customConfig ? '4' : '\0', _exitable ? '5' : '\0' } );

        switch( input )
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
                try
                {
                    Load( );
                }
                catch( const std::exception& error )
                {
                    std::cout << "\n\nERROR " << error.what( );
                    std::cout << "\n\nPress enter to continue: \n";
                    SelectEnter( );

                    break;
                }

                if( Exist( ) )
                {
                    Start( );
                }

                break;
            }
            case '3':
            {
                _config = DungeonConfiguration( );
                Reset( );
                Start( );

                break;
            }
            case '4':
            {
                _config = SelectDungeonConfiguration( );
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

bool Game::Turn( )
{
    static const std::map<char, Orientation> directions
    {
        { 'W', Orientation::North },
        { 'A', Orientation::West  },
        { 'S', Orientation::South },
        { 'D', Orientation::East  }
    };
    static const std::map<char, Orientation> rotations
    {
        { 'F', Orientation::East  },
        { 'G', Orientation::South },
        { 'H', Orientation::West  }
    };
    std::string output;
    char input;

    while( true )
    {
        ClearScreen( _clearOutput );
        output.clear( );
        output
            .append( GetStringDungeon( _dungeons[_index], _player.real->position, { 32, 16 } ) )
            .append( "Health: " )
            .append( GetStringHealth( _player.real->health ) )
            .append( "\n\n" )
            .append( "[W] Go North\n" )
            .append( "[A] Go West\n" )
            .append( "[S] Go South\n" )
            .append( "[D] Go East\n" )
            .append( "[E] Exit to meny\n" )
            .append( std::string( _saveToFile ? "[R] Exit to meny and save\n" : "" ) )
            .append( "[F] Rotate dungeon 90'\n" )
            .append( "[G] Rotate dungeon 180'\n" )
            .append( "[H] Rotate dungeon 270'\n" )
            .append( "Select action: \n" );
        std::cout << output;
        input = SelectChar( { 'W', 'A', 'S', 'D', 'E', _saveToFile ? 'R' : '\0', 'F', 'G', 'H' }, std::toupper );

        switch( input )
        {
            case 'W':
            case 'A':
            case 'S':
            case 'D':
            {
                _dungeons[_index].MovementPlayer( directions.at( input ) );

                return true;
            }
            case 'E':
            {
                return false;
            }
            case 'R':
            {
                if (_saveToFile)
                {
                    Save();
                }

                return false;
            }
            case 'F':
            case 'G':
            case 'H':
            {
                _dungeons[_index].Rotate( rotations.at( input ) );

                break;
            }
        }
    }
}
void Game::Reset( )
{
    _player.Reset( _entityFactory.PlayerDefault( ) );
    _dungeons.clear( );
    _dungeons.emplace_back( _player, _entityFactory, _config );
    _dungeons[0].PlayerSet( std::nullopt );
    _index = 0;
    DungeonConnect( 0 );
}
void Game::Start( )
{
    while( _player.real->active &&
           Turn( ) )
    {
        _dungeons[_index].MovementRandom( );
        _dungeons[_index].Events( _battleSystem );
        _player.real->Update( );

        if( _player.real->next )
        {
            DungeonAlign( *_player.real->next );
            DungeonConnect( _player.real->next->indexDungeon );
            _dungeons[_player.real->next->indexDungeon].PlayerSet( _player.real->next->indexDoor );
            _index = _player.real->next->indexDungeon;
            _player.real->next.reset( );
        }
    }
}
void Game::DungeonAlign( const Connector& connector )
{
    const auto doorNext = _dungeons[connector.indexDungeon].GetDoors( )[connector.indexDoor];
    const auto doorPrev = _dungeons[doorNext->connector->indexDungeon].GetDoors( )[doorNext->connector->indexDoor];
    const int sideNext = RectQuadrantArithmetic( RectQuadrant( doorNext->position, _dungeons[doorPrev->connector->indexDungeon].GetSize( ) ) );
    const int sidePrev = RectQuadrantArithmetic( RectQuadrant( doorPrev->position, _dungeons[doorNext->connector->indexDungeon].GetSize( ) ) );
    const int align = ( ( ( sidePrev - sideNext ) + 3 ) % 4 ) - 1;

    _dungeons[connector.indexDungeon].Rotate( RectQuadrantArithmetic( align ) );
}
void Game::DungeonConnect( int index )
{
    const auto doors = _dungeons[index].GetDoors( );
    const int size = doors.size( );

    for( int i = 0; i < size; i++ )
    {
        if( !doors[i]->connector )
        {
            const int other = _dungeons.size( );

            _dungeons.emplace_back( _player, _entityFactory, _config );
            _dungeons[index].Connect( { other, 0 }, i );
            _dungeons[other].Connect( { index, i }, 0 );
        }
    }
}
void Game::Save( )
{
    const std::string name = "Dungeoncrawler_Save.txt";
    std::ofstream oFile( name, std::ios::out | std::ios::trunc );

    if( !oFile.is_open( ) )
    {
        throw std::exception( std::string( "Unable to open file: " + name ).c_str( ) );
    }

    oFile << _config.size.determined << ',';
    oFile << _config.size.dungeon.x << ',';
    oFile << _config.size.dungeon.y << ',';
    oFile << _config.generate.doors << ',';
    oFile << _config.generate.wallsOuter << ',';
    oFile << _config.generate.hiddenPath << ',';
    oFile << _config.generate.wallsParents << ',';
    oFile << _config.generate.wallsChildren << ',';
    oFile << _config.generate.wallsFiller << ',';
    oFile << _config.generate.enemies << ',';
    oFile << _config.amount.doors << ',';
    oFile << _config.amount.wallsParents << ',';
    oFile << _config.amount.wallsChildren << ',';
    oFile << _config.amount.wallsFillerCycles << ',';
    oFile << _config.amount.enemies << '\n';
    oFile << _index << '\n';
    oFile << _dungeons.size( ) << '\n';

    for( int i = 0, limit = _dungeons.size( ); i < limit; i++ )
    {
        const auto doors = _dungeons[i].GetDoors( );
        const Vector2<int> size = _dungeons[i].GetSize( );
        Vector2<int> iterator;

        oFile << size.x << ',';
        oFile << size.y << '\n';

        for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
        {
            for( iterator.x = 0; iterator.x < size.x; iterator.x++ )
            {
                oFile << _dungeons[i].GetIcon( iterator );
            }

            oFile << '\n';
        }

        oFile << doors.size( ) << '\n';

        for( const auto& door : doors )
        {
            oFile << door->position.x << ',';
            oFile << door->position.y << ',';
            oFile << door->connector->indexDungeon << ',';
            oFile << door->connector->indexDoor << '\n';
        }
    }
}
void Game::Load( )
{
    const std::string name = "Dungeoncrawler_Save.txt";
    std::ifstream iFile( name, std::ios::in );
    int limit;
    auto GetString = [] ( std::ifstream& stream ) -> std::string
    {
        std::string line;

        std::getline( stream, line );

        return line;
    };
    auto GetConfig = [] ( const std::string& line ) -> DungeonConfiguration
    {
        std::stringstream sstream( line );
        std::string value;
        std::vector<std::string> values;

        while( std::getline( sstream, value, ',' ) )
        {
            values.push_back( value );
        }

        return DungeonConfiguration( values );
    };
    auto GetVector = [] ( const std::string& line ) -> std::vector<int>
    {
        std::stringstream sstream( line );
        std::string value;
        std::vector<int> values;

        while( std::getline( sstream, value, ',' ) )
        {
            values.push_back( std::stoi( value ) );
        }

        return values;
    };
    auto GetVector2int = [] ( const std::string& line ) -> Vector2<int>
    {
        std::stringstream sstream( line );
        std::string value;
        Vector2<int> values;

        std::getline( sstream, value, ',' );
        values.x = std::stoi( value );
        std::getline( sstream, value, ',' );
        values.y = std::stoi( value );

        return values;
    };

    if( !iFile.is_open( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    _dungeons.clear( );
    _config = GetConfig( GetString( iFile ) );
    _index = std::stoi( GetString( iFile ) );
    limit = std::stoi( GetString( iFile ) );

    for( int indexDungeon = 0; indexDungeon < limit; indexDungeon++ )
    {
        Grid<char> icons( GetVector2int( GetString( iFile ) ) );
        std::vector<Door> doors;
        Vector2<int> iterator;
        int size;

        for( iterator.y = 0; iterator.y < icons.Size( ).y; iterator.y++ )
        {
            const std::string line = GetString( iFile );

            for( iterator.x = 0; iterator.x < icons.Size( ).x; iterator.x++ )
            {
                icons[iterator] = line[iterator.x];
            }
        }

        size = std::stoi( GetString( iFile ) );

        for( int i = 0; i < size; i++ )
        {
            const std::vector<int> values = GetVector( GetString( iFile ) );

            doors.push_back( *dynamic_cast<Door*>( _entityFactory.Get( "Door" ).get( ) ) );
            doors[i].position  = { values[0], values[1] };
            doors[i].connector = { values[2], values[3] };
        }

        _dungeons.emplace_back( _player, _entityFactory, icons, doors );
    }
}