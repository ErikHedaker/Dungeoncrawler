#include "Functions.h"
#include "BattleSystem.h"
#include "Game.h"
#include "Enums.h"
#include <iostream>
#include <random>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cctype>
#include <limits>
#include <cstdlib>
#include <string_view>

double RandomNumberGenerator( double min, double max )
{
    static std::random_device rd;
    static std::mt19937 generator( rd( ) );
    std::uniform_real_distribution<double> random( min, max );

    return random( generator );
}
int RandomNumberGenerator( int min, int max )
{
    static std::random_device rd;
    static std::mt19937 generator( rd( ) );
    std::uniform_int_distribution<int> random( min, max );

    return random( generator );
}
int GetPowerDiceRoll( const Power& power )
{
    int accumulate = 0;

    for( int i = 0; i < power.rolls; i++ )
    {
        accumulate += RandomNumberGenerator( 1, power.sides );
    }

    return accumulate + power.modifier;
}
int GetBitmask( const std::string& line )
{
    std::stringstream sstream( line );
    std::string value;
    int values = 0;

    while( std::getline( sstream, value, ',' ) )
    {
        values |= 1 << std::stoi( value );
    }

    return values;
};
std::optional<Power> GetOptionalPower( const std::string& line )
{
    std::stringstream sstream( line );
    std::string value;
    std::vector<int> values;

    if( line.size( ) == 0 )
    {
        return std::optional<Power>( );
    }

    while( std::getline( sstream, value, ',' ) )
    {
        values.push_back( std::stoi( value ) );
    }

    return
    { {
        values[0] != 0,
        values[1],
        values[2],
        values[3]
    } };
}
std::string GetStringPower( const Power& power )
{
    return std::string( std::to_string( power.rolls ) + "d" + std::to_string( power.sides ) + " + " + std::to_string( power.modifier ) );
}
std::string GetStringHealth( const Health& health )
{
    std::string output;

    output += std::to_string( health.current );
    output += " (";

    if( health.current == health.max )
    {
        output += "max";
    }
    else
    {
        if( health.regeneration > 0 )
        {
            output += "+";
        }

        output += std::to_string( health.regeneration );
    }

    output += ")";

    return output;
}
std::string GetStringEffects( const std::vector<Effect>& effects )
{
    std::string output;

    for( int i = 0, limit = effects.size( ); i < limit; i++ )
    {
        output.append( effects[i].name );

        if( i != limit - 1 )
        {
            if( i == limit - 2 )
            {
                output.append( " and " );
            }
            else
            {
                output.append( ", " );
            }
        }
    }

    return output;
}
std::string GetStringDungeon( const Dungeon& dungeon, const Vector2<int>& center, const Vector2<int>& sizeScreen )
{
    const Vector2<int> origoCamera   = center - sizeScreen / 2;
    const Vector2<int> iteratorBegin = origoCamera - 1;
    const Vector2<int> iteratorEnd   = origoCamera + 2 + sizeScreen;
    Vector2<int> iterator;
    std::string output;

    for( iterator.y = iteratorBegin.y; iterator.y < iteratorEnd.y; iterator.y++ )
    {
        for( iterator.x = iteratorBegin.x; iterator.x < iteratorEnd.x; iterator.x++ )
        {
            if( OnBorder( iterator, iteratorEnd, iteratorBegin ) )
            {
                output += '/';
            }
            else if( InBounds( iterator, dungeon.GetSize( ) ) &&
                     dungeon.Visible( iterator ) )
            {
                output += dungeon.GetIcon( iterator );
            }
            else
            {
                output += ' ';
            }
        }

        output += '\n';
    }

    return output;
}
Vector2<int> PositionRotate( const Vector2<int>& position, const Vector2<int>& size, const Orientation::Enum& rotation )
{
    switch( rotation )
    {
        case Orientation::North:
        {
            return position;
        }
        case Orientation::East:
        {
            return { size.y - position.y - 1, position.x };
        }
        case Orientation::South:
        {
            return { size.x - position.x - 1, size.y - position.y - 1 };
        }
        case Orientation::West:
        {
            return { position.y, size.x - position.x - 1 };
        }
    }

    return position;
}
Vector2<int> PositionMove( const Vector2<int>& position, const Orientation::Enum& orientation )
{
    static const std::map<Orientation::Enum, Vector2<int>> directions
    {
        { Orientation::North, {  0, -1 } },
        { Orientation::East,  {  1,  0 } },
        { Orientation::South, {  0,  1 } },
        { Orientation::West,  { -1,  0 } }
    };

    return position + directions.at( orientation );
}
Vector2<int> PositionMoveProbability( const Vector2<int>& position, int north, int west, int south, int east, int still )
{
    const int random = RandomNumberGenerator( 0, north + east + south + west + still - 1 );

    if( random < north )
    {
        return PositionMove( position, Orientation::North );
    }

    if( random < north + east )
    {
        return PositionMove( position, Orientation::East );
    }

    if( random < north + east + south )
    {
        return PositionMove( position, Orientation::South );
    }

    if( random < north + east + south + west )
    {
        return PositionMove( position, Orientation::West );
    }

    return position;
}
std::vector<Vector2<int>> BresenhamCircle( const Vector2<int>& center, int radius )
{
    std::vector<Vector2<int>> result;
    Vector2<int> current = { radius * ( -1 ), 0 };
    int error = 2 - 2 * radius;

    while( current.x < 0 )
    {
        const int temp = error;

        result.push_back( { center.x - current.x, center.y + current.y } );
        result.push_back( { center.x - current.y, center.y - current.x } );
        result.push_back( { center.x + current.x, center.y - current.y } );
        result.push_back( { center.x + current.y, center.y + current.x } );

        if( temp <= current.y )
        {
            current.y++;
            error += current.y * 2 + 1;
        }

        if( temp > current.x ||
            error > current.y )
        {
            current.x++;
            error += current.x * 2 + 1;
        }
    }

    return result;
}
std::vector<Vector2<int>> BresenhamLine( const Vector2<int>& start, const Vector2<int>& end )
{
    const Vector2<int> delta
    {
        std::abs( end.x - start.x ),
        std::abs( end.y - start.y ) * -1
    };
    const Vector2<int> offset
    {
        start.x < end.x ? 1 : -1,
        start.y < end.y ? 1 : -1
    };
    std::vector<Vector2<int>> result;
    Vector2<int> current = start;
    int error = delta.x + delta.y;

    while( true )
    {
        const int temp = error * 2;

        result.push_back( current );

        if( current == end )
        {
            break;
        }

        if( temp >= delta.y )
        {
            error += delta.y;
            current.x += offset.x;
        }

        if( temp <= delta.x )
        {
            error += delta.x;
            current.y += offset.y;
        }
    }

    return result;
}
bool OnBorder( const Vector2<int>& position, const Vector2<int>& size, const Vector2<int>& origo, int minLayer, int maxLayer )
{
    return
        position.x + minLayer <= origo.x + maxLayer ||
        position.y + minLayer <= origo.y + maxLayer ||
        position.x - minLayer >= size.x  - maxLayer - 1 ||
        position.y - minLayer >= size.y  - maxLayer - 1;
}
bool InCorner( const Vector2<int>& position, const Vector2<int>& size, int sensitivity )
{
    return
        ( position.x <= sensitivity || position.x >= size.x - sensitivity - 1 ) &&
        ( position.y <= sensitivity || position.y >= size.y - sensitivity - 1 );
}
bool InBounds( const Vector2<int>& position, const Vector2<int>& size )
{
    return
        position.x >= 0 &&
        position.y >= 0 &&
        position.x < size.x &&
        position.y < size.y;
}
Orientation::Enum RectQuadrant( const Vector2<int>& position, const Vector2<int>& size )
{
    static const std::map<std::pair<bool, bool>, Orientation::Enum> quadrants
    {
        { { true,  false }, Orientation::North },
        { { true,  true  }, Orientation::East  },
        { { false, true  }, Orientation::South },
        { { false, false }, Orientation::West  }
    };
    const Vector2<float> positionf = position;
    const Vector2<float> sizef     = size;
    const Vector2<float> ratiof    = sizef / sizef.y;
    const bool rightOfMainDiagonal = positionf.x > ( positionf.y * ratiof.x );
    const bool rightOfAntiDiagonal = positionf.x > ( sizef.x - positionf.y * ratiof.x - 1 );

    return quadrants.at( { rightOfMainDiagonal, rightOfAntiDiagonal } );
}
void ClearScreen( )
{
    #ifdef _WIN32
        std::system( "CLS" );
    #else
        std::system( "clear" );
    #endif
}
DungeonConfiguration InputDungeonConfiguration( )
{
    DungeonConfiguration config;
    auto ToBool = [] ( char input ) -> bool
    {
        return
            input == 'Y' ||
            input == 'y';
    };

    std::cout << "\n";
    config.size.determined = ToBool( InputChar( "Fixed dungeon size, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
    config.generate.doors = ToBool( InputChar( "Generate doors, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
    config.generate.wallsOuter = ToBool( InputChar( "Generate outer walls, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
    config.generate.hiddenPath = ToBool( InputChar( "Generate hidden path, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
    config.generate.wallsParents = ToBool( InputChar( "Generate parent walls, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
    config.generate.wallsChildren = ToBool( InputChar( "Generate children walls, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
    config.generate.wallsFiller = ToBool( InputChar( "Generate filler walls, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
    config.generate.enemies = ToBool( InputChar( "Generate monsters, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
    std::cout << "\n";

    if( config.size.determined )
    {
        config.size.dungeon.x = InputPositiveInteger( "Enter dungeon width: " );
        config.size.dungeon.y = InputPositiveInteger( "Enter dungeon height: " );
    }
    if( config.generate.doors )         config.amount.doors = InputPositiveInteger( "Enter amount of doors: " );
    if( config.generate.wallsParents )  config.amount.wallsParents = InputPositiveInteger( "Enter amount of parent walls: " );
    if( config.generate.wallsChildren ) config.amount.wallsChildren = InputPositiveInteger( "Enter amount of children walls: " );
    if( config.generate.wallsFiller )   config.amount.wallsFillerCycles = InputPositiveInteger( "Enter amount of filler wall cycles: " );
    if( config.generate.enemies )       config.amount.enemies = InputPositiveInteger( "Enter amount of enemies: " );

    return config;
}
void InputEnter( )
{
    std::cin.ignore( std::numeric_limits<std::streamsize>::max( ), '\n' );
    std::cin.get( );
}
char InputChar( std::string_view context, const std::vector<char>& valid, std::function<int( int )> modifier )
{
    while( true )
    {
        std::string input;
        char last;

        std::cout << context;
        std::cin >> input;
        last = modifier != nullptr ? modifier( input.back( ) ) : input.back( );

        if( std::find( valid.begin( ), valid.end( ), last ) != valid.end( ) )
        {
            return last;
        }
    }
}
int InputPositiveInteger( std::string_view context )
{
    while( true )
    {
        std::string input;

        std::cout << context;
        std::cin >> input;

        if( input.size( ) < 10 &&
            std::all_of( input.begin( ), input.end( ), ::isdigit ) )
        {
            return std::stoi( input );
        }
    }
}
std::vector<Effect> LoadEffects( )
{
    const std::string name = "Dungeoncrawler_Dependency_Effects.txt";
    const std::vector<std::string> fileCache { std::istream_iterator<StringWrapper> { std::ifstream { name, std::ios::in } }, { } };
    constexpr int offset = 3;
    std::vector<Effect> effects;

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    for( int i = 0, limit = fileCache.size( ); i < limit; i += offset )
    {
        effects.push_back(
        {
            fileCache[i + 0],
            GetOptionalPower( fileCache[i + 1] ),
            std::stoi( fileCache[i + 2] )
        } );
    }

    return effects;
}
std::vector<Spell> LoadSpells( )
{
    const std::string name = "Dungeoncrawler_Dependency_Spells.txt";
    const std::vector<std::string> fileCache { std::istream_iterator<StringWrapper> { std::ifstream { name, std::ios::in } }, { } };
    constexpr int offset = 3;
    std::vector<Spell> spells;

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    for( int i = 0, limit = fileCache.size( ); i < limit; i += offset )
    {
        spells.push_back(
        {
            fileCache[i + 0],
            GetOptionalPower( fileCache[i + 1] ),
            GetBitmask( fileCache[i + 2] )
        } );
    }

    return spells;
}
std::vector<Character> LoadCharacters( )
{
    const std::string name = "Dungeoncrawler_Dependency_Characters.txt";
    const std::vector<std::string> fileCache { std::istream_iterator<StringWrapper> { std::ifstream { name, std::ios::in } }, { } };
    constexpr int offset = 8;
    std::vector<Character> characters;

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    for( int i = 0, limit = fileCache.size( ); i < limit; i += offset )
    {
        characters.push_back(
        {
            fileCache[i + 0],
            fileCache[i + 1].back( ),
            GetBitmask( fileCache[i + 2] ),
            {
                std::stoi( fileCache[i + 3] ),
                std::stoi( fileCache[i + 4] ),
                std::stoi( fileCache[i + 5] )
            },
            std::stoi( fileCache[i + 6] ),
            GetBitmask( fileCache[i + 7] )
        } );
    }

    return characters;
}
std::vector<Structure> LoadStructures( )
{
    const std::string name = "Dungeoncrawler_Dependency_Structures.txt";
    const std::vector<std::string> fileCache { std::istream_iterator<StringWrapper>{ std::ifstream { name, std::ios::in } }, { } };
    constexpr int offset = 3;
    std::vector<Structure> structures;

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    for( int i = 0, limit = fileCache.size( ); i < limit; i += offset )
    {
        structures.push_back(
        {
            fileCache[i + 0],
            fileCache[i + 1].back( ),
            GetBitmask( fileCache[i + 2] )
        } );
    }

    return structures;
}
Player LoadPlayerDefault( )
{
    const std::string name = "Dungeoncrawler_Dependency_PlayerDefault.txt";
    const std::vector<std::string> fileCache { std::istream_iterator<StringWrapper>{ std::ifstream { name, std::ios::in } }, { } };

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    return
    {
        fileCache[0],
        fileCache[1].back( ),
        GetBitmask( fileCache[2] ),
        {
            std::stoi( fileCache[3] ),
            std::stoi( fileCache[4] ),
            std::stoi( fileCache[5] )
        },
        std::stoi( fileCache[6] ),
        GetBitmask( fileCache[7] ),
        std::stoi( fileCache[8] ),
        GetBitmask( fileCache[9] )
    };
}