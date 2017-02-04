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
int GetPositiveInteger( const std::string& context )
{
    while( true )
    {
        std::string choice;

        std::cout << context;
        std::cin >> choice;

        if( choice.size( ) < 10 &&
            std::all_of( choice.begin( ), choice.end( ), ::isdigit ) )
        {
            return std::stoi( choice );
        }
    }
}
char GetChar( const std::string& context, const std::vector<char>& valid, std::function<int(int)> modifier )
{
    while( true )
    {
        std::string choice;
        char last;

        std::cout << context;
        std::cin >> choice;
        last = ( modifier != nullptr ) ? modifier( choice.back( ) ) : choice.back( );

        if( std::find( valid.begin( ), valid.end( ), last ) != valid.end( ) )
        {
            return last;
        }
    }
}
void GetEnter( )
{
    /* First one doesn't wait for user input */
    std::cin.get( );
    std::cin.get( );
}
DungeonConfiguration GetDungeonConfiguration( const GameConfig& type )
{
    if( type == GameConfig::Configure )
    {
        DungeonConfiguration config;
        auto ToBool = [] ( char input ) -> bool
        {
            return
                input == 'Y' ||
                input == 'y';
        };

        std::cout << "\n";
        config.sizeDungeonFixed       = ToBool( GetChar( "Fixed dungeon size, [Y/N]: ",       { 'Y', 'N' }, std::toupper ) );
        config.generateDoors          = ToBool( GetChar( "Generate doors, [Y/N]: ",           { 'Y', 'N' }, std::toupper ) );
        config.generateOuterWalls     = ToBool( GetChar( "Generate outer walls, [Y/N]: ",     { 'Y', 'N' }, std::toupper ) );
        config.generateHiddenPath     = ToBool( GetChar( "Generate hidden path, [Y/N]: ",     { 'Y', 'N' }, std::toupper ) );
        config.generateSourceWalls    = ToBool( GetChar( "Generate source walls, [Y/N]: ",    { 'Y', 'N' }, std::toupper ) );
        config.generateExtensionWalls = ToBool( GetChar( "Generate extension walls, [Y/N]: ", { 'Y', 'N' }, std::toupper ) );
        config.generateFillerWalls    = ToBool( GetChar( "Generate filler walls, [Y/N]: ",    { 'Y', 'N' }, std::toupper ) );
        config.generateMonsters       = ToBool( GetChar( "Generate monsters, [Y/N]: ",        { 'Y', 'N' }, std::toupper ) );
        std::cout << "\n";

        if( config.sizeDungeonFixed )
        {
            config.sizeDungeon.x           = GetPositiveInteger( "Enter dungeon width: " );
            config.sizeDungeon.y           = GetPositiveInteger( "Enter dungeon height: " );
        }
        if( config.generateDoors )
            config.amountDoors             = GetPositiveInteger( "Enter amount of doors: " );
        if( config.generateSourceWalls )
            config.amountSourceWalls       = GetPositiveInteger( "Enter amount of source walls: " );
        if( config.generateExtensionWalls )
            config.amountExtensionWalls    = GetPositiveInteger( "Enter amount of extension walls: " );
        if( config.generateFillerWalls )
            config.amountFillerWallsCycles = GetPositiveInteger( "Enter amount of filler wall cycles: " );
        if( config.generateMonsters )
            config.amountMonsters          = GetPositiveInteger( "Enter amount of monsters: " );

        return config;
    }

    return DungeonConfiguration( );
}
Vector2<int> PositionRotate( const Vector2<int>& position, const Vector2<int>& sizeOld, const Orientation::Enum& orientation )
{
    Vector2<int> positionRotation = position;
    Vector2<int> sizeRotation = sizeOld;

    for( int i = 0; i < orientation; i++ )
    {
        std::swap( sizeRotation.x, sizeRotation.y );
        positionRotation = { sizeRotation.x - positionRotation.y - 1, positionRotation.x };
    }

    return positionRotation;
}
Vector2<int> PositionMove( const Vector2<int>& position, const Orientation::Enum& orientation )
{
    static const std::map<Orientation::Enum, Vector2<int>> directions =
    {
        { Orientation::North, {  0, -1 } },
        { Orientation::East,  {  1,  0 } },
        { Orientation::South, {  0,  1 } },
        { Orientation::West,  { -1,  0 } },
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
    std::map<char, Ability> abilitiesMap;
    std::vector<char> choices;
    char id = '1';
    
    std::cout << "\nChoose an ability:\n";

    for( const auto& ability : abilities )
    {
        abilitiesMap.emplace( id, ability );
        choices.push_back( id );
        std::cout << "[" << id << "] " << abilitiesMap.at( id ).name << "\n";
        id++;
    }

    std::cout << "\n";
    
    return abilitiesMap.at( GetChar( "Enter choice: ", choices ) );
}
std::string UseAbility( const Ability& ability, Character& caster, Character& target )
{
    const int healthOld = target.health;

    target.health -= static_cast<int>( ability.damage * RandomNumberGenerator( 0.9, 1.1 ) );

    return std::string( caster.name + " casts " + ability.name + " on " + target.name + ", which dealt " + std::to_string( healthOld - target.health ) + " damage!" );
}
std::string UseWeapon( Character& attacker, Character& target )
{
    const int healthOld = target.health;

    target.health -= static_cast<int>( attacker.damage * RandomNumberGenerator( 0.9, 1.1 ) );

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
                    result = UseAbility( GetAbility( player.abilities ), player, AI );
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

    return ( number == AI.abilities.size( ) ? UseWeapon( AI, player ) : UseAbility( AI.abilities[number], AI, player ) );
}
void PrintDungeonCentered( const Dungeon& dungeon, int visionReach, const Vector2<int>& center, const Vector2<int>& sizeScreen )
{
    const Vector2<int> origoCamera = center - sizeScreen / 2;
    const Vector2<int> iteratorBegin = origoCamera - 1;
    const Vector2<int> iteratorEnd = origoCamera + 1 + sizeScreen;
    auto InsideVisionReach = [visionReach, center] ( const Vector2<int>& iterator ) -> bool
    {
        return
            iterator >= center - visionReach &&
            iterator <= center + visionReach;
    };
    Vector2<int> iterator;

    for( iterator.y = iteratorBegin.y; iterator.y <= iteratorEnd.y; iterator.y++ )
    {
        for( iterator.x = iteratorBegin.x; iterator.x <= iteratorEnd.x; iterator.x++ )
        {
            if( iterator.x == iteratorBegin.x ||
                iterator.y == iteratorBegin.y ||
                iterator.x == iteratorEnd.x ||
                iterator.y == iteratorEnd.y )
            {
                std::cout << '\\';
            }
            else if( dungeon.InBounds( iterator ) &&
                     InsideVisionReach( iterator ) )
            {
                std::cout << dungeon.GetTile( iterator ).icon;
            }
            else if( dungeon.InBounds( iterator ) &&
                     dungeon.GetVision( iterator ) )
            {
                std::cout << ':';
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

    if( combatant.healthRegen > 0 )
    {
        std::cout << "+";
    }

    std::cout << combatant.healthRegen << ")\n";
}
void Combat( Character& player, Character& AI )
{
    std::string previousTurnPlayer;
    std::string previousTurnAI;

    while( true )
    {
        system( "CLS" );
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
void SaveDungeonSystem( const DungeonSystem& dungeonSystem )
{
    std::ofstream outFile( "Dungeoncrawler_Save_DungeonSystem.txt", std::ios::out | std::ios::trunc );

    if( !outFile.is_open( ) )
    {
        return;
    }

    outFile << dungeonSystem.config.sizeDungeonFixed << ',';
    outFile << dungeonSystem.config.sizeDungeon.x << ',';
    outFile << dungeonSystem.config.sizeDungeon.x << ',';
    outFile << dungeonSystem.config.generateDoors << ',';
    outFile << dungeonSystem.config.generateOuterWalls << ',';
    outFile << dungeonSystem.config.generateHiddenPath << ',';
    outFile << dungeonSystem.config.generateSourceWalls << ',';
    outFile << dungeonSystem.config.generateExtensionWalls << ',';
    outFile << dungeonSystem.config.generateFillerWalls << ',';
    outFile << dungeonSystem.config.generateMonsters << ',';
    outFile << dungeonSystem.config.amountDoors << ',';
    outFile << dungeonSystem.config.amountSourceWalls << ',';
    outFile << dungeonSystem.config.amountExtensionWalls << ',';
    outFile << dungeonSystem.config.amountFillerWallsCycles << ',';
    outFile << dungeonSystem.config.amountMonsters << '\n';

    outFile << dungeonSystem.indexCurrent << '\n';
    outFile << dungeonSystem.dungeons.size( ) << '\n';

    for( const auto& dungeon : dungeonSystem.dungeons )
    {
        const Vector2<int> size = dungeon.GetSize( );
        Vector2<int> iterator;

        outFile << size.x << ',';
        outFile << size.y << '\n';

        for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
        {
            for( iterator.x = 0; iterator.x < size.x * 2; iterator.x++ )
            {
                if( iterator.x < size.x )
                {
                    outFile << dungeon.GetTile( iterator ).icon;
                }
                else
                {
                    outFile << dungeon.GetVision( { iterator.x % size.x, iterator.y } );
                }
            }

            outFile << '\n';
        }

        outFile << dungeon.links.size( ) << '\n';

        for( const auto& link : dungeon.links )
        {
            outFile << link.indexDungeon << ',';
            outFile << link.indexLink << ',';
            outFile << link.exit.x << ',';
            outFile << link.exit.y << ',';
            outFile << link.entry.x << ',';
            outFile << link.entry.y << '\n';
        }
    }
}
DungeonSystem LoadDungeonSystem( const EntityFactory& entityLibrary )
{
    std::ifstream inFile( "Dungeoncrawler_Save_DungeonSystem.txt", std::ios::in );
    std::string line;
    DungeonSystem dungeonSystem;
    int dungeonAmount;
    auto GetConfig = [] ( const std::string& line )
    {
        std::stringstream sstream( line );
        std::string value;
        std::vector<int> values;

        while( std::getline( sstream, value, ',' ) )
        {
            values.push_back( std::stoi( value ) );
        }

        return DungeonConfiguration
        (
            values[0] != 0,
            { values[1], values[2] },
            values[3] != 0,
            values[4] != 0,
            values[5] != 0,
            values[6] != 0,
            values[7] != 0,
            values[8] != 0,
            values[9] != 0,
            values[10],
            values[11],
            values[12],
            values[13],
            values[14]
        );
    };
    auto GetVector2 = [] ( const std::string& line )
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
    auto GetLink = [] ( const std::string& line )
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

    if( !inFile.is_open( ) )
    {
        throw std::exception( "Missing save files!" );
    }

    std::getline( inFile, line );
    dungeonSystem.config = GetConfig( line );
    std::getline( inFile, line );
    dungeonSystem.indexCurrent = std::stoi( line );
    std::getline( inFile, line );
    dungeonAmount = std::stoi( line );

    for( int index = 0; index < dungeonAmount; index++ )
    {
        std::vector<char> iconMap;
        std::vector<bool> visionMap;
        Vector2<int> size;
        Vector2<int> iterator;
        int linkAmount;
    
        std::getline( inFile, line );
        size = GetVector2( line );
        iconMap.resize( size.x * size.y );
        visionMap.resize( size.x * size.y );

        for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
        {
            std::getline( inFile, line );

            for( iterator.x = 0; iterator.x < size.x * 2; iterator.x++ )
            {
                if( iterator.x >= size.x )
                {
                    visionMap[( iterator.y * size.x ) + ( iterator.x % size.x )] = ( line[iterator.x] == '1' );
                }
                else
                {
                    iconMap[( iterator.y * size.x ) + iterator.x] = line[iterator.x];
                }
            }
        }

        dungeonSystem.dungeons.emplace_back( entityLibrary, size, visionMap, iconMap );
        std::getline( inFile, line );
        linkAmount = std::stoi( line );

        for( int indexLink = 0; indexLink < linkAmount; indexLink++ )
        {
            std::getline( inFile, line );
            dungeonSystem.dungeons.back( ).links.push_back( GetLink( line ) );
        }
    }

    return dungeonSystem;
}
std::vector<Ability> LoadAbilities( )
{
    const int offset = 4;
    std::vector<std::string> cacheFile{ std::istream_iterator<StringWrapper>{ std::ifstream{ "Dungeoncrawler_Dependency_Abilities.txt", std::ios::in } }, { } };
    std::vector<Ability> abilities;
    auto GetBitmask = [] ( const std::string& line )
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

    if( cacheFile.empty( ) )
    {
        throw std::exception( "Missing important files!" );
    }

    for( unsigned int i = 0; i < cacheFile.size( ); i += offset )
    {
        abilities.emplace_back( cacheFile[0 + i],
                                cacheFile[1 + i].back( ),
                    GetBitmask( cacheFile[2 + i] ),
                     std::stof( cacheFile[3 + i] ) );
    }

    return abilities;
}
std::vector<Character> LoadCharacters( const std::vector<Ability>& abilities )
{
    const int offset = 8;
    std::vector<std::string> cacheFile{ std::istream_iterator<StringWrapper>{ std::ifstream{ "Dungeoncrawler_Dependency_Characters.txt", std::ios::in } }, { } };
    std::vector<Character> characters;
    auto GetBitmask = [] ( const std::string& line )
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
    auto GetAbilities = [&abilities] ( const std::string& line )
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

    if( cacheFile.empty( ) )
    {
        throw std::exception( "Missing important files!" );
    }

    for( unsigned int i = 0; i < cacheFile.size( ); i += offset )
    {
        characters.emplace_back( cacheFile[0 + i],
                                 cacheFile[1 + i].back( ),
                     GetBitmask( cacheFile[2 + i] ),
                      std::stoi( cacheFile[3 + i] ),
                      std::stoi( cacheFile[4 + i] ),
                      std::stoi( cacheFile[5 + i] ),
                      std::stof( cacheFile[6 + i] ),
                   GetAbilities( cacheFile[7 + i] ) );
    }

    return characters;
}
std::vector<Structure> LoadStructures( )
{
    const int offset = 3;
    std::vector<std::string> cacheFile{ std::istream_iterator<StringWrapper>{ std::ifstream{ "Dungeoncrawler_Dependency_Structures.txt", std::ios::in } }, { } };
    std::vector<Structure> structures;
    auto GetBitmask = [] ( const std::string& line )
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

    if( cacheFile.empty( ) )
    {
        throw std::exception( "Missing important files!" );
    }

    for( unsigned int i = 0; i < cacheFile.size( ); i += offset )
    {
        structures.emplace_back( cacheFile[0 + i],
                                 cacheFile[1 + i].back( ),
                     GetBitmask( cacheFile[2 + i] ) );
    }

    return structures;
}
Player LoadPlayerDefault( const std::vector<Ability>& abilities )
{
    std::vector<std::string> cacheFile{ std::istream_iterator<StringWrapper>{ std::ifstream{ "Dungeoncrawler_Dependency_Player.txt", std::ios::in } }, { } };
    auto GetBitmask = [] ( const std::string& line )
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
    auto GetAbilities = [&abilities] ( const std::string& line )
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

    if( cacheFile.empty( ) )
    {
        throw std::exception( "Missing important files!" );
    }

    return Player( cacheFile[0],
                   cacheFile[1].back( ),
       GetBitmask( cacheFile[2] ),
        std::stoi( cacheFile[3] ),
        std::stoi( cacheFile[4] ),
        std::stoi( cacheFile[5] ),
        std::stof( cacheFile[6] ),
     GetAbilities( cacheFile[7] ),
        std::stoi( cacheFile[8] ),
       GetBitmask( cacheFile[9] ) );
}