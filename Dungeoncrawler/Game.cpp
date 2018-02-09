#include "Game.h"
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

class Stopwatch
{
    public:
        Stopwatch& Start( )
        {
            _start = std::chrono::high_resolution_clock::now( );

            return *this;
        }
        Stopwatch& Stop( )
        {
            _stop = std::chrono::high_resolution_clock::now( );
            _current = std::chrono::duration_cast<std::chrono::microseconds>( _stop - _start );

            return *this;
        }
        std::string FPS( ) const
        {
            return std::to_string( 1.0 / ( static_cast<double>( _current.count( ) ) / 1000000.0 ) );
        }
        std::string Microseconds( ) const
        {
            return std::to_string( _current.count( ) );
        }

    private:
        std::chrono::microseconds _current;
        std::chrono::time_point<std::chrono::steady_clock> _start;
        std::chrono::time_point<std::chrono::steady_clock> _stop;
};

Stopwatch stopwatchTotal;
Stopwatch stopwatchLogic;

Game::Game( ) :
    _player( _entityFactory.PlayerDefault( ) )
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
        ClearScreen( );
        output.clear( );
        output
            .append( "[1] Continue current game\n" )
            .append( "[2] Load game from file\n" )
            .append( "[3] Build new game (Randomization)\n" )
            .append( "[4] Build new game (Configuration)\n" )
            .append( "[5] Exit\n" )
            .append( "Select option: " );
        std::cout << output;
        input = SelectChar( { '1', '2', '3', '4', '5' } );
        std::cout << "\nLoading, please wait.";

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
                    std::cout << "\n\nPress enter to continue: ";
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

bool Game::TurnPlayer( )
{
    static const std::map<char, Orientation::Enum> directions
    {
        { 'W', Orientation::North },
        { 'A', Orientation::West  },
        { 'S', Orientation::South },
        { 'D', Orientation::East  }
    };
    static const std::map<char, Orientation::Enum> rotations
    {
        { 'F', Orientation::East  },
        { 'G', Orientation::South },
        { 'H', Orientation::West  }
    };
    std::string output;
    char input;

    while( true )
    {
        ClearScreen( );
        output.clear( );
        output
            .append( GetStringDungeon( _dungeons[_index], _player.real->position, { 32, 16 } ) )
            .append( "\nTotal time between frames:  " )
            .append( stopwatchTotal.Stop( ).Microseconds( ) )
            .append( "\nSampled logic microseconds: " )
            .append( stopwatchLogic.Microseconds( ) )
            .append( "\n\nHealth: " )
            .append( GetStringHealth( _player.real->health ) )
            .append( "\n\n" )
            .append( "[W] Go North\n" )
            .append( "[A] Go West\n" )
            .append( "[S] Go South\n" )
            .append( "[D] Go East\n" )
            .append( "[E] Exit to meny while saving\n" )
            .append( "[R] Exit to meny without saving\n" )
            .append( "[F] Rotate dungeon 90'\n" )
            .append( "[G] Rotate dungeon 180'\n" )
            .append( "[H] Rotate dungeon 270'\n" )
            .append( "Select action: " );
        std::cout << output;
        input = SelectChar( { 'W', 'A', 'S', 'D', 'E', 'R', 'F', 'G', 'H' }, std::toupper );
        output
            .append( std::string( 1, input ) )
            .append( "\n" );
        stopwatchTotal.Start( );

        switch( input )
        {
            case 'W':
            case 'A':
            case 'S':
            case 'D':
            {
                stopwatchLogic.Start( );
                _dungeons[_index].MovementPlayer( directions.at( input ) );
                stopwatchLogic.Stop( );

                return true;
            }
            case 'E':
            {
                Save( );

                return false;
            }
            case 'R':
            {
                return false;
            }
            case 'F':
            case 'G':
            case 'H':
            {
                DungeonRotate( _index, rotations.at( input ) );

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
    _index = 0;
    DungeonLink( 0 );
    _dungeons[0].PlayerSet( _dungeons[0].GetSize( ) / 2 );
}
void Game::Start( )
{
    while( _player.real->active &&
           TurnPlayer( ) )
    {
        _dungeons[_index].MovementRandom( );
        _dungeons[_index].Events( _battleSystem );
        _player.real->Update( );

        if( _player.real->states & States::Swapping )
        {
            _player.real->states &= ~States::Swapping;
            DungeonSwap( );
        }
    }
}
void Game::DungeonSwap( )
{
    const int limit = _dungeons[_index].links.size( );

    for( int i = 0; i < limit; i++ )
    {
        if( _dungeons[_index].links[i].entrance == _player.real->position )
        {
            const int indexPrev = _index;
            const int indexNext = _dungeons[indexPrev].links[i].indexDungeon;
            const int entrance  = RectQuadrant( _dungeons[indexPrev].links[i].entrance, _dungeons[indexPrev].GetSize( ) );
            const int exit      = RectQuadrant( _dungeons[indexPrev].links[i].exit,     _dungeons[indexNext].GetSize( ) );
            const int align     = ( ( ( entrance - exit ) + 3 ) % 4 ) - 1;

            DungeonLink( indexNext );
            _index = indexNext;
            _dungeons[indexNext].PlayerSet( _dungeons[indexPrev].links[i].exit );
            DungeonRotate( indexNext, static_cast<Orientation::Enum>( align ) );

            break;
        }
    }
}
void Game::DungeonLink( int indexCurrentDungeon )
{
    const int limit = _dungeons[indexCurrentDungeon].links.size( );

    for( int indexCurrentLink = 0; indexCurrentLink < limit; indexCurrentLink++ )
    {
        if( _dungeons[indexCurrentDungeon].links[indexCurrentLink].indexLink < 0 &&
            _dungeons[indexCurrentDungeon].links[indexCurrentLink].indexDungeon < 0 )
        {
            _dungeons.emplace_back( _player, _entityFactory, _config );

            const int indexPartnerLink = 0;
            const int indexPartnerDungeon = _dungeons.size( ) - 1;
            Link& partner = _dungeons[indexPartnerDungeon].links[indexPartnerLink];
            Link& current = _dungeons[indexCurrentDungeon].links[indexCurrentLink];

            partner = { indexCurrentDungeon, indexCurrentLink, partner.entrance, current.entrance };
            current = { indexPartnerDungeon, indexPartnerLink, current.entrance, partner.entrance };
        }
    }
}
void Game::DungeonRotate( int indexDungeon, const Orientation::Enum& orientation )
{
    const Vector2<int> sizePrev = _dungeons[indexDungeon].GetSize( );

    for( auto& current : _dungeons[indexDungeon].links )
    {
        Link& partner = _dungeons[current.indexDungeon].links[current.indexLink];

        current.entrance = PositionRotate( current.entrance, sizePrev, orientation );
        partner.exit = PositionRotate( partner.exit, sizePrev, orientation );
    }

    _dungeons[indexDungeon].Rotate( orientation );
}
void Game::Save( )
{
    const std::string name = "Dungeoncrawler_Save.txt";
    std::ofstream fileOut( name, std::ios::out | std::ios::trunc );

    if( !fileOut.is_open( ) )
    {
        throw std::exception( std::string( "Unable to open file: " + name ).c_str( ) );
    }

    fileOut << _config.size.determined << ',';
    fileOut << _config.size.dungeon.x << ',';
    fileOut << _config.size.dungeon.y << ',';
    fileOut << _config.generate.doors << ',';
    fileOut << _config.generate.wallsOuter << ',';
    fileOut << _config.generate.hiddenPath << ',';
    fileOut << _config.generate.wallsParents << ',';
    fileOut << _config.generate.wallsChildren << ',';
    fileOut << _config.generate.wallsFiller << ',';
    fileOut << _config.generate.enemies << ',';
    fileOut << _config.amount.doors << ',';
    fileOut << _config.amount.wallsParents << ',';
    fileOut << _config.amount.wallsChildren << ',';
    fileOut << _config.amount.wallsFillerCycles << ',';
    fileOut << _config.amount.enemies << '\n';
    fileOut << _index << '\n';
    fileOut << _dungeons.size( ) << '\n';

    for( int i = 0, limit = _dungeons.size( ); i < limit; i++ )
    {
        const Vector2<int> sizeDungeon = _dungeons[i].GetSize( );
        Vector2<int> iterator;

        fileOut << sizeDungeon.x << ',';
        fileOut << sizeDungeon.y << '\n';

        for( iterator.y = 0; iterator.y < sizeDungeon.y; iterator.y++ )
        {
            for( iterator.x = 0; iterator.x < sizeDungeon.x; iterator.x++ )
            {
                fileOut << _dungeons[i].GetIcon( iterator );
            }

            fileOut << '\n';
        }

        fileOut << _dungeons[i].links.size( ) << '\n';

        for( const auto& link : _dungeons[i].links )
        {
            fileOut << link.indexDungeon << ',';
            fileOut << link.indexLink << ',';
            fileOut << link.entrance.x << ',';
            fileOut << link.entrance.y << ',';
            fileOut << link.exit.x << ',';
            fileOut << link.exit.y << '\n';
        }
    }
}
void Game::Load( )
{
    const std::string name = "Dungeoncrawler_Save.txt";
    std::ifstream fileIn( name, std::ios::in );
    std::string line;
    int amountDungeon;
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
    auto GetLink = [] ( const std::string& line ) -> Link
    {
        std::stringstream sstream( line );
        std::string value;
        std::vector<int> values;

        while( std::getline( sstream, value, ',' ) )
        {
            values.push_back( std::stoi( value ) );
        }

        return Link
        {
            values[0],
            values[1],
            { values[2], values[3] },
            { values[4], values[5] }
        };
    };

    if( !fileIn.is_open( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    _dungeons.clear( );

    std::getline( fileIn, line );
    _config = GetConfig( line );

    std::getline( fileIn, line );
    _index = std::stoi( line );

    std::getline( fileIn, line );
    amountDungeon = std::stoi( line );

    for( int indexDungeon = 0; indexDungeon < amountDungeon; indexDungeon++ )
    {
        std::vector<char> icons;
        Vector2<int> size;
        Vector2<int> iterator;
        int amountLink;

        std::getline( fileIn, line );
        size = GetVector2int( line );
        icons.resize( size.x * size.y );

        for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
        {
            std::getline( fileIn, line );

            for( iterator.x = 0; iterator.x < size.x; iterator.x++ )
            {
                icons[( iterator.y * size.x ) + iterator.x] = line[iterator.x];
            }
        }

        _dungeons.emplace_back( _player, _entityFactory, size, icons );

        std::getline( fileIn, line );
        amountLink = std::stoi( line );

        for( int indexLink = 0; indexLink < amountLink; indexLink++ )
        {
            std::getline( fileIn, line );
            _dungeons.back( ).links.push_back( GetLink( line ) );
        }
    }
}