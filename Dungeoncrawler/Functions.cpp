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
#include <functional>

int GetRNG( int min, int max )
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
        accumulate += GetRNG( 1, power.sides );
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
    return std::string( std::to_string( power.rolls ) + "d" + std::to_string( power.sides ) + "+" + std::to_string( power.modifier ) );
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
std::string GetStringEffects( const std::vector<std::reference_wrapper<const Effect>>& effects )
{
    std::string output;

    for( int i = 0, limit = effects.size( ); i < limit; i++ )
    {
        output.append( effects[i].get( ).name );

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
    const std::map<Orientation::Enum, Vector2<int>> result
    {
        { Orientation::North, position },
        { Orientation::East,  { size.y - position.y - 1, position.x } },
        { Orientation::South, { size.x - position.x - 1, size.y - position.y - 1 } },
        { Orientation::West,  { position.y, size.x - position.x - 1 } }
    };

    return result.at( rotation );
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
    const int random = GetRNG( 0, north + east + south + west + still - 1 );

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
    int errorNext = 2 - 2 * radius;

    while( current.x < 0 )
    {
        const int errorPrev = errorNext;

        result.push_back( { center.x - current.x, center.y + current.y } );
        result.push_back( { center.x - current.y, center.y - current.x } );
        result.push_back( { center.x + current.x, center.y - current.y } );
        result.push_back( { center.x + current.y, center.y + current.x } );

        if( errorPrev <= current.y )
        {
            current.y++;
            errorNext += current.y * 2 + 1;
        }

        if( errorPrev > current.x ||
            errorNext > current.y )
        {
            current.x++;
            errorNext += current.x * 2 + 1;
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
    int errorNext = delta.x + delta.y;

    while( true )
    {
        const int errorPrev = errorNext * 2;

        result.push_back( current );

        if( current == end )
        {
            break;
        }

        if( errorPrev >= delta.y )
        {
            errorNext += delta.y;
            current.x += offset.x;
        }

        if( errorPrev <= delta.x )
        {
            errorNext += delta.x;
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
    const bool rightOfAntiDiagonal = positionf.x > ( sizef.x - positionf.y * ratiof.x - 1.0f );

    return quadrants.at( { rightOfMainDiagonal, rightOfAntiDiagonal } );
}
DungeonConfiguration SelectDungeonConfiguration( )
{
    DungeonConfiguration config;
    auto Select = [] ( )
    {
        return 'Y' == SelectChar( { 'Y', 'N' }, std::toupper );
    };

    std::cout << "\nFixed dungeon size, [Y/N]: ";
    config.size.determined = Select( );
    std::cout << "Generate doors, [Y/N]: ";
    config.generate.doors = Select( );
    std::cout << "Generate outer walls, [Y/N]: ";
    config.generate.wallsOuter = Select( );
    std::cout << "Generate hidden path, [Y/N]: ";
    config.generate.hiddenPath = Select( );
    std::cout << "Generate parent walls, [Y/N]: ";
    config.generate.wallsParents = Select( );
    std::cout << "Generate children walls, [Y/N]: ";
    config.generate.wallsChildren = Select( );
    std::cout << "Generate filler walls, [Y/N]: ";
    config.generate.wallsFiller = Select( );
    std::cout << "Generate monsters, [Y/N]: ";
    config.generate.enemies = Select( );
    std::cout << "\n";

    if( config.size.determined )
    {
        std::cout << "Enter dungeon width: ";
        config.size.dungeon.x = SelectPositiveInteger( );
        std::cout << "Enter dungeon height: ";
        config.size.dungeon.y = SelectPositiveInteger( );
    }

    if( config.generate.doors )
    {
        std::cout << "Enter amount of doors: ";
        config.amount.doors = SelectPositiveInteger( );
    }
    
    if( config.generate.wallsParents )
    {
        std::cout << "Enter amount of parent walls: ";
        config.amount.wallsParents = SelectPositiveInteger( );
    }

    if( config.generate.wallsChildren )
    {
        std::cout << "Enter amount of children walls: ";
        config.amount.wallsChildren = SelectPositiveInteger( );
    }

    if( config.generate.wallsFiller )
    {
        std::cout << "Enter amount of filler wall cycles: ";
        config.amount.wallsFillerCycles = SelectPositiveInteger( );
    }

    if( config.generate.enemies )
    {
        std::cout << "Enter amount of enemies: ";
        config.amount.enemies = SelectPositiveInteger( );
    }

    return config;
}
int SelectPositiveInteger( )
{
    while( true )
    {
        std::string input;

        std::cin >> input;

        if( input.size( ) < 10 &&
            std::all_of( input.begin( ), input.end( ), ::isdigit ) )
        {
            return std::stoi( input );
        }
    }
}
char SelectChar( const std::vector<char>& valid, std::function<int( int )> modifier )
{
    while( true )
    {
        std::string input;
        char last;

        std::cin >> input;
        last = modifier != nullptr ? modifier( input.back( ) ) : input.back( );

        if( std::find( valid.begin( ), valid.end( ), last ) != valid.end( ) )
        {
            return last;
        }
    }
}
void SelectEnter( )
{
    std::cin.ignore( std::numeric_limits<std::streamsize>::max( ), '\n' );
    std::cin.get( );
}
void ClearScreen( bool clear )
{
    if( clear )
    {
        #ifdef _WIN32
        std::system("CLS");
        #else
        std::system("clear");
        #endif
    }
}
std::vector<Character> LoadCharacters( )
{
    /*
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
    */

    return
    {
        Character( "Zombie",   'Z', GetBitmask( "1,2" ), { 120, 120, 5 }, 20, GetBitmask( "" )  ),
        Character( "Skeleton", 'S', GetBitmask( "1,2" ), { 80, 80, 0 },   10, GetBitmask( "1" ) ),
        Character( "Lunatic",  'L', GetBitmask( "1,2" ), { 70, 70, 0 },   10, GetBitmask( "" )  )
    };
}
std::vector<Effect> LoadEffects( )
{
    /*
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
    */

    return
    {
        Effect( "Freeze", GetOptionalPower( "0,1,4,0" ), 5 ),
        Effect( "Ignite", GetOptionalPower( "0,1,4,1" ), 3 ),
        Effect( "Poison", GetOptionalPower( "0,3,4,0" ), 4 )
    };
}
std::vector<Spell> LoadSpells( )
{
    /*
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
    */

    return
    {
        Spell( "Iceblast",     GetOptionalPower( "0,2,10,2" ), GetBitmask( "0" )     ),
        Spell( "Fireball",     GetOptionalPower( "0,1,20,4" ), GetBitmask( "1" )     ),
        Spell( "Frostfire",    GetOptionalPower( "0,2,6,3" ),  GetBitmask( "0,1" )   ),
        Spell( "Throw Poison", GetOptionalPower( "0,1,4,3" ),  GetBitmask( "2" )     ),
        Spell( "Infliction",   GetOptionalPower( "" ),         GetBitmask( "0,1,2" ) )
    };
}
Player LoadPlayerDefault( )
{
    /*
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
            std::stoi( fileCache[8] )
        };
    */

    return
    {
        "Player",
        '@',
        GetBitmask( "" ),
        { 100, 100, 1 },
        50,
        GetBitmask( "0,1,2,3,4" ),
        8
    };
}