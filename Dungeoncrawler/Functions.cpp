#include "Functions.h"
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
int GetPositiveInteger( const std::string& context )
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
char GetChar( const std::string& context, const std::vector<char>& valid, std::function<int(int)> modifier )
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
void GetEnter( )
{
    std::cin.ignore( std::numeric_limits<std::streamsize>::max( ), '\n' );
    std::cin.get( );
}
void ClearScreen( )
{
    #ifdef _WIN32
        std::system( "CLS" );
    #else
        std::system( "clear" );
    #endif
}
DungeonConfiguration GetDungeonConfiguration( )
{
    DungeonConfiguration config;
    auto ToBool = [] ( char input ) -> bool
    {
        return
            input == 'Y' ||
            input == 'y';
    };

    std::cout << "\n";
    config.size.determined        = ToBool( GetChar( "Fixed dungeon size, [Y/N]: ",       { 'Y', 'N' }, std::toupper ) );
    config.generate.doors         = ToBool( GetChar( "Generate doors, [Y/N]: ",           { 'Y', 'N' }, std::toupper ) );
    config.generate.wallsOuter    = ToBool( GetChar( "Generate outer walls, [Y/N]: ",     { 'Y', 'N' }, std::toupper ) );
    config.generate.hiddenPath    = ToBool( GetChar( "Generate hidden path, [Y/N]: ",     { 'Y', 'N' }, std::toupper ) );
    config.generate.wallsParents  = ToBool( GetChar( "Generate parent walls, [Y/N]: ",    { 'Y', 'N' }, std::toupper ) );
    config.generate.wallsChildren = ToBool( GetChar( "Generate children walls, [Y/N]: ",  { 'Y', 'N' }, std::toupper ) );
    config.generate.wallsFiller   = ToBool( GetChar( "Generate filler walls, [Y/N]: ",    { 'Y', 'N' }, std::toupper ) );
    config.generate.enemies       = ToBool( GetChar( "Generate monsters, [Y/N]: ",        { 'Y', 'N' }, std::toupper ) );
    std::cout << "\n";

    if( config.size.determined )
    {
        config.size.dungeon.x                                           = GetPositiveInteger( "Enter dungeon width: " );
        config.size.dungeon.y                                           = GetPositiveInteger( "Enter dungeon height: " );
    }
    if( config.generate.doors )         config.amount.doors             = GetPositiveInteger( "Enter amount of doors: " );
    if( config.generate.wallsParents )  config.amount.wallsParents      = GetPositiveInteger( "Enter amount of parent walls: " );
    if( config.generate.wallsChildren ) config.amount.wallsChildren     = GetPositiveInteger( "Enter amount of children walls: " );
    if( config.generate.wallsFiller )   config.amount.wallsFillerCycles = GetPositiveInteger( "Enter amount of filler wall cycles: " );
    if( config.generate.enemies )       config.amount.enemies           = GetPositiveInteger( "Enter amount of enemies: " );
    
    return config;
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
Ability GetAbility( const std::vector<Ability>& abilities )
{
    std::map<char, Ability> pairs;
    std::vector<char> valid;
    char key = '1';
    
    std::cout << "\nChoose an ability:\n";

    for( const auto& ability : abilities )
    {
        pairs.emplace( key, ability );
        valid.push_back( key );
        std::cout << "[" << key << "] " << pairs.at( key ).name << "\n";
        key++;
    }

    std::cout << "\n";
    
    return pairs.at( GetChar( "Enter choice: ", valid ) );
}
std::string UseAbility( Character& attacker, Character& target, const Ability& ability )
{
    constexpr double min = 0.9;
    constexpr double max = 1.1;
    const int healthOld = target.health;

    target.health -= static_cast<int>( ability.damage * RandomNumberGenerator( min, max ) );

    return std::string( attacker.name + " casts " + ability.name + " on " + target.name + ", which dealt " + std::to_string( healthOld - target.health ) + " damage!" );
}
std::string UseWeapon( Character& attacker, Character& target )
{
    constexpr double min = 0.9;
    constexpr double max = 1.1;
    const int healthOld = target.health;

    target.health -= static_cast<int>( attacker.damage * RandomNumberGenerator( min, max ) );

    return std::string( attacker.name + " attacks " + target.name + " with a weapon, which dealt " + std::to_string( healthOld - target.health ) + " damage!" );
}
std::string TurnPlayer( Character& player, Character& AI )
{
    std::string result;
    bool done = false;

    while( !done )
    {
        std::cout << "\nChoose an action:\n";
        std::cout << "[1] Attack with weapon\n";
        std::cout << "[2] Attack with ability\n\n";
        const char choice = GetChar( "Enter choice: ", { '1', '2' }, std::toupper );

        switch( choice )
        {
            case '1':
            {
                result = UseWeapon( player, AI );
                done = true;

                break;
            }
            case '2':
            {
                if( player.abilities.size( ) > 0 )
                {
                    result = UseAbility( player, AI, GetAbility( player.abilities ) );
                    done = true;
                }

                break;
            }
        }
    }

    return result;
}
std::string TurnAI( Character& player, Character& AI )
{
    const int number = RandomNumberGenerator( 0, AI.abilities.size( ) );

    return number == AI.abilities.size( ) ? UseWeapon( AI, player ) : UseAbility( AI, player, AI.abilities[number] );
}
void PrintDungeon( const Dungeon& dungeon, int visionReach, const Vector2<int>& center, const Vector2<int>& sizeScreen )
{
    const Vector2<int> origoCamera = center - sizeScreen / 2;
    const Vector2<int> iteratorBegin = origoCamera - 1;
    const Vector2<int> iteratorEnd = origoCamera + 2 + sizeScreen;
    Vector2<int> iterator;

    for( iterator.y = iteratorBegin.y; iterator.y < iteratorEnd.y; iterator.y++ )
    {
        for( iterator.x = iteratorBegin.x; iterator.x < iteratorEnd.x; iterator.x++ )
        {
            if( OnBorder( iterator, iteratorEnd, iteratorBegin ) )
            {
                std::cout << '\\';
            }
            else if( InBounds( iterator, dungeon.GetSize( ) ) &&
                     dungeon.Visible( iterator ) )
            {
                std::cout << dungeon.GetTile( iterator ).icon;
            }
            else
            {
                std::cout << ' ';
            }
        }

        std::cout << '\n';
    }

    std::cout << '\n';
}
void PrintHealth( const Character& combatant )
{
    std::cout << combatant.name << " HP: " << combatant.health << " (";

    if( combatant.healthRegeneration > 0 )
    {
        std::cout << "+";
    }

    std::cout << combatant.healthRegeneration << ")\n";
}
void Fight( Character& player, Character& AI )
{
    std::string previousTurnPlayer;
    std::string previousTurnAI;

    while( true )
    {
        ClearScreen( );
        std::cout << "You've been engaged in combat with a " << AI.name << "!\n";
        std::cout << "\n-----\n\n";

        player.Update( );
        AI.Update( );

        PrintHealth( player );
        PrintHealth( AI );

        std::cout << "\n-----\n";
        std::cout << "\n" << previousTurnPlayer << "\n";
        std::cout << "\n" << previousTurnAI << "\n";
        std::cout << "\n-----\n";

        if( player.health <= 0 || AI.health <= 0 )
        {
            std::cout << "\n" << ( player.health <= 0 ? player.name : AI.name ) << " died!";
            std::cout << "\n\nPress enter to continue: ";
            GetEnter( );

            break;
        }

        previousTurnPlayer = TurnPlayer( player, AI );
        previousTurnAI = TurnAI( player, AI );
    }
}
std::vector<Ability> LoadAbilities( )
{
    constexpr int offset = 4;
    const std::string name = "Dungeoncrawler_Dependency_Abilities.txt";
    std::vector<std::string> fileCache { std::istream_iterator<StringWrapper> { std::ifstream { name, std::ios::in } }, { } };
    std::vector<Ability> abilities;

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    for( unsigned int i = 0; i < fileCache.size( ); i += offset )
    {
        abilities.emplace_back( fileCache[0 + i],
                                fileCache[1 + i].back( ),
                    GetBitmask( fileCache[2 + i] ),
                     std::stof( fileCache[3 + i] ) );
    }

    return abilities;
}
std::vector<Character> LoadCharacters( const std::vector<Ability>& abilities )
{
    constexpr int offset = 8;
    const std::string name = "Dungeoncrawler_Dependency_Characters.txt";
    std::vector<std::string> fileCache { std::istream_iterator<StringWrapper> { std::ifstream { name, std::ios::in } }, { } };
    std::vector<Character> characters;
    auto GetAbilities = [&abilities] ( const std::string& line ) -> std::vector<Ability>
    {
        std::stringstream sstream( line );
        std::string value;
        std::vector<Ability> values;

        while( std::getline( sstream, value, ',' ) )
        {
            values.push_back( abilities[std::stoi( value )] );
        }

        return values;
    };

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    for( unsigned int i = 0; i < fileCache.size( ); i += offset )
    {
        characters.emplace_back( fileCache[0 + i],
                                 fileCache[1 + i].back( ),
                     GetBitmask( fileCache[2 + i] ),
                      std::stoi( fileCache[3 + i] ),
                      std::stoi( fileCache[4 + i] ),
                      std::stoi( fileCache[5 + i] ),
                      std::stof( fileCache[6 + i] ),
                   GetAbilities( fileCache[7 + i] ) );
    }

    return characters;
}
std::vector<Structure> LoadStructures( )
{
    constexpr int offset = 3;
    const std::string name = "Dungeoncrawler_Dependency_Structures.txt";
    std::vector<std::string> fileCache { std::istream_iterator<StringWrapper>{ std::ifstream{ name, std::ios::in } }, { } };
    std::vector<Structure> structures;

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    for( unsigned int i = 0; i < fileCache.size( ); i += offset )
    {
        structures.emplace_back( fileCache[0 + i],
                                 fileCache[1 + i].back( ),
                     GetBitmask( fileCache[2 + i] ) );
    }

    return structures;
}
Player LoadPlayerDefault( const std::vector<Ability>& abilities )
{
    const std::string name = "Dungeoncrawler_Dependency_Player.txt";
    std::vector<std::string> fileCache { std::istream_iterator<StringWrapper>{ std::ifstream{ name, std::ios::in } }, { } };
    auto GetAbilities = [&abilities] ( const std::string& line ) -> std::vector<Ability>
    {
        std::stringstream sstream( line );
        std::string value;
        std::vector<Ability> values;

        while( std::getline( sstream, value, ',' ) )
        {
            values.push_back( abilities[std::stoi( value )] );
        }

        return values;
    };

    if( fileCache.empty( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    return Player( fileCache[0],
                   fileCache[1].back( ),
       GetBitmask( fileCache[2] ),
        std::stoi( fileCache[3] ),
        std::stoi( fileCache[4] ),
        std::stoi( fileCache[5] ),
        std::stof( fileCache[6] ),
     GetAbilities( fileCache[7] ),
        std::stoi( fileCache[8] ),
       GetBitmask( fileCache[9] ) );
}
void SaveGameConfig( const DungeonConfiguration& config )
{
    const std::string name = "Dungeoncrawler_Save_Config.txt";
    std::ofstream fileOut( name, std::ios::out | std::ios::trunc );

    if( !fileOut.is_open( ) )
    {
        throw std::exception( std::string( "Unable to open file: " + name ).c_str( ) );
    }

    fileOut << config.size.determined << '\n';
    fileOut << config.size.dungeon.x << '\n';
    fileOut << config.size.dungeon.y << '\n';
    fileOut << config.generate.doors << '\n';
    fileOut << config.generate.wallsOuter << '\n';
    fileOut << config.generate.hiddenPath << '\n';
    fileOut << config.generate.wallsParents << '\n';
    fileOut << config.generate.wallsChildren << '\n';
    fileOut << config.generate.wallsFiller << '\n';
    fileOut << config.generate.enemies << '\n';
    fileOut << config.amount.doors << '\n';
    fileOut << config.amount.wallsParents << '\n';
    fileOut << config.amount.wallsChildren << '\n';
    fileOut << config.amount.wallsFillerCycles << '\n';
    fileOut << config.amount.enemies << '\n';
}
void SaveGameDungeons( const std::vector<Dungeon>& dungeons, int index )
{
    const std::string name = "Dungeoncrawler_Save_Dungeons.txt";
    std::ofstream fileOut( name, std::ios::out | std::ios::trunc );

    if( !fileOut.is_open( ) )
    {
        throw std::exception( std::string( "Unable to open file: " + name ).c_str( ) );
    }

    fileOut << index << '\n';
    fileOut << dungeons.size( ) << '\n';

    for( const auto& dungeon : dungeons )
    {
        const Vector2<int> size = dungeon.GetSize( );
        Vector2<int> iterator;

        fileOut << size.x << ',';
        fileOut << size.y << '\n';

        for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
        {
            for( iterator.x = 0; iterator.x < size.x; iterator.x++ )
            {
                fileOut << dungeon.GetTile( iterator ).icon;
            }

            fileOut << '\n';
        }

        fileOut << dungeon.links.size( ) << '\n';

        for( const auto& link : dungeon.links )
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
DungeonConfiguration LoadGameConfig( )
{
    const std::string name = "Dungeoncrawler_Save_Config.txt";
    std::ifstream fileIn( name, std::ios::in );

    if( !fileIn.is_open( ) )
    {
        throw std::exception( std::string( "Missing file: " + name ).c_str( ) );
    }

    return DungeonConfiguration( std::vector<std::string> { std::istream_iterator<std::string> { fileIn }, { } } );
}
std::vector<Dungeon> LoadGameDungeons( PlayerHandle& player, const EntityFactory& entityFactory, int& index )
{
    const std::string name = "Dungeoncrawler_Save_Dungeons.txt";
    std::ifstream fileIn( name, std::ios::in );
    std::string line;
    std::vector<Dungeon> dungeons;
    int amountDungeon;
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

    std::getline( fileIn, line );
    index = std::stoi( line );
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

        dungeons.emplace_back( player, entityFactory, size, icons );

        std::getline( fileIn, line );
        amountLink = std::stoi( line );

        for( int indexLink = 0; indexLink < amountLink; indexLink++ )
        {
            std::getline( fileIn, line );
            dungeons.back( ).links.push_back( GetLink( line ) );
        }
    }

    return dungeons;
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

        if( temp > current.x || error > current.y )
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
        std::abs( end.y - start.y ) * ( -1 )
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
bool OnBorder( const Vector2<int>& position, const Vector2<int>& size, const Vector2<int>& origo, int layerFrom, int layerTo )
{
    return
        position.x + layerTo <= origo.x + layerFrom ||
        position.y + layerTo <= origo.y + layerFrom ||
        position.x - layerTo >= size.x  - layerFrom - 1 ||
        position.y - layerTo >= size.y  - layerFrom - 1;
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
Orientation::Enum Quadrant( const Vector2<int>& position, const Vector2<int>& size )
{
    static const std::map<std::pair<bool, bool>, Orientation::Enum> quadrants
    {
        { { true,  false }, Orientation::North },
        { { true,  true  }, Orientation::East },
        { { false, true  }, Orientation::South },
        { { false, false }, Orientation::West }
    };
    const Vector2<float> positionf = position;
    const Vector2<float> sizef     = size;
    const Vector2<float> ratiof    = sizef / sizef.y;
    const bool rightOfMainDiagonal = positionf.x > ( positionf.y * ratiof.x );
    const bool rightOfAntiDiagonal = positionf.x > ( sizef.x - positionf.y * ratiof.x - 1 );

    return quadrants.at( { rightOfMainDiagonal, rightOfAntiDiagonal } );
}