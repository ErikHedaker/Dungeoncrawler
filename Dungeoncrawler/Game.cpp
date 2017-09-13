#include "Game.h"
#include "Functions.h"
#include <iostream>
#include <map>
#include <cctype>
#include <memory>
#include <chrono>
#include <deque>

class Stopwatch
{
    public:
        Stopwatch( ) :
            _current( 0 )
        { }

        void Start( )
        {
            _start = std::chrono::high_resolution_clock::now( );
        }
        void Stop( )
        {
            _stop = std::chrono::high_resolution_clock::now( );
            _current = std::chrono::duration_cast< std::chrono::microseconds >( _stop - _start ).count( );
            _average.push_back( _current );

            if( _average.size( ) > 100 )
            {
                _average.pop_front( );
            }
        }
        long long int MicrosecondsCurrent( ) const
        {
            return _current;
        }
        long long int MicrosecondsAverage( ) const
        {
            long long int total = 0;

            for( const auto& value : _average )
            {
                total += value;
            }

            return total / ( _average.size( ) ? _average.size( ) : 1 );
        }

    private:
        long long int _current;
        std::deque<long long int> _average;
        std::chrono::time_point<std::chrono::steady_clock> _start;
        std::chrono::time_point<std::chrono::steady_clock> _stop;
} stopwatch;

Game::Game( ) :
    _player( LoadPlayerDefault( LoadAbilities( ) ) )
{ }

void Game::Menu( )
{
    char input;

    while( true )
    {
        stopwatch = Stopwatch( );
        ClearScreen( );
        std::cout << "[1] Continue current game\n";
        std::cout << "[2] Load game from file\n";
        std::cout << "[3] Build new game (Randomization)\n";
        std::cout << "[4] Build new game (Configuration)\n";
        std::cout << "[5] Exit\n\n";
        input = GetChar( "Enter choice: ", { '1', '2', '3', '4', '5' } );

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
                ClearScreen( );
                std::cout << "Loading, please wait.\n";

                try
                {
                    _config = LoadGameConfig( );
                    _dungeons = LoadGameDungeons( _player, _entityFactory, _index );
                }
                catch( const std::exception& error )
                {
                    std::cout << "\nERROR " << error.what( );
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
            case '4':
            {
                _config = input == '3' ? DungeonConfiguration( ) : GetDungeonConfiguration( );
                ClearScreen( );
                std::cout << "Loading, please wait.\n";
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

bool Game::PlayerTurn( )
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
    char input;

    while( true )
    {
        ClearScreen( );
        PrintDungeon( _dungeons[_index], _player.real->visionReach, _player.real->position, { 30, 15 } );
        std::cout << "Sampled logic current microseconds: " << stopwatch.MicrosecondsCurrent( ) << "\n";
        std::cout << "Sampled logic average microseconds: " << stopwatch.MicrosecondsAverage( ) << "\n\n";
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
        input = GetChar( "Enter choice: ", { 'W', 'A', 'S', 'D', 'E', 'R', 'F', 'G', 'H' }, std::toupper );

        switch( input )
        {
            case 'W':
            case 'A':
            case 'S':
            case 'D':
            {
                stopwatch.Start( );
                _dungeons[_index].MovementPlayer( directions.at( input ) );
                stopwatch.Stop( );

                return true;
            }
            case 'E':
            case 'R':
            {
                if( input == 'E' )
                {
                    SaveGameConfig( _config );
                    SaveGameDungeons( _dungeons, _index );
                }

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
    _player.Reset( LoadPlayerDefault( LoadAbilities( ) ) );
    _dungeons.clear( );
    _dungeons.emplace_back( _player, _entityFactory, _config );
    _index = 0;
    DungeonLink( 0 );
    _dungeons[0].PlayerPlace( _dungeons[0].GetSize( ) / 2 );
}
void Game::Start( )
{
    while( _player.real->health > 0 &&
           PlayerTurn( ) )
    {
        _dungeons[_index].MovementRandom( );
        _dungeons[_index].Events( );
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
            const int entrance  = Quadrant( _dungeons[indexPrev].links[i].entrance, _dungeons[indexPrev].GetSize( ) );
            const int exit      = Quadrant( _dungeons[indexPrev].links[i].exit,     _dungeons[indexNext].GetSize( ) );
            const int align     = ( ( ( entrance - exit ) + 3 ) % 4 ) - 1;

            DungeonLink( indexNext );
            _index = indexNext;
            _dungeons[indexNext].PlayerPlace( _dungeons[indexPrev].links[i].exit );
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
        partner.exit     = PositionRotate( partner.exit,     sizePrev, orientation );
    }

    _dungeons[indexDungeon].Rotate( orientation );
}