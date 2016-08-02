#include "EntityLibrary.h"
#include "Functions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

BaseEntity::BaseEntity( const std::string& name, char icon, int attributes ) :
    name( name ),
    icon( icon ),
    attributes( attributes )
{ }

Ability::Ability( const std::string& name, char icon, int attributes, float damage ) :
    BaseEntity( name, icon, attributes ),
    damage( damage )
{ }

Character::Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities ) :
    BaseEntity( name, icon, attributes ),
    health( health ),
    healthMax( healthMax ),
    healthRegen( healthRegen ),
    damage( damage ),
    abilities( abilities )
{ }

Structure::Structure( const std::string& name, char icon, int attributes ) :
    BaseEntity( name, icon, attributes )
{ }

Player::Player( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities, int visionReach, int states ) :
    Character( name, icon, attributes, health, healthMax, healthRegen, damage, abilities ),
    visionReach( visionReach ),
    states( states )
{ }

PlayerEntity::PlayerEntity( Player& player ) :
    BaseEntity( player.name, player.icon, player.attributes ),
    player( player )
{ }

void Character::Update( )
{
    health += healthRegen;

    if( health > healthMax )
    {
        health = healthMax;
    }
}

EntityLibrary::EntityLibrary( ) :
    abilities( LoadAbilities( ) ),
    characters( LoadCharacters( ) ),
    structures( LoadStructures( ) )
{ }

std::vector<Ability> EntityLibrary::LoadAbilities( ) const
{
    const std::vector<Ability>& abilitiesLibrary = abilities;
    const int offset = 4;
    const std::string nameFile = "Dungeoncrawler_Category_Ability.txt";
    std::ifstream inFile( nameFile, std::ios::in );
    std::string line;
    std::vector<std::string> cacheFile;
    std::vector<Ability> result;
    auto GetBitmask = [] ( const std::string& line )
    {
        std::stringstream sstream( line );
        std::string value;
        int bitmask = 0;

        while( std::getline( sstream, value, ',' ) )
        {
            bitmask |= 1 << std::stoi( value );
        }

        return bitmask;
    };

    if( !inFile.is_open( ) )
    {
        throw std::exception( std::string( "Could not open file " + nameFile ).c_str( ) );
    }

    while( std::getline( inFile, line ) )
    {
        cacheFile.push_back( line );
    }

    cacheFile.erase( std::remove_if( cacheFile.begin( ), cacheFile.end( ), [] ( const std::string& line ) { return line[0] != ':'; } ), cacheFile.end( ) );
    std::for_each( cacheFile.begin( ), cacheFile.end( ), [] ( std::string& line ) { line.erase( 0, 1 ); } );

    for( unsigned int i = 0; i < cacheFile.size( ); i += offset )
    {
        result.emplace_back( cacheFile[0 + i],
                             cacheFile[1 + i].back( ),
                 GetBitmask( cacheFile[2 + i] ),
                  std::stof( cacheFile[3 + i] ) );
    }

    return result;
}
std::vector<Character> EntityLibrary::LoadCharacters( ) const
{
    const std::vector<Ability>& abilitiesLibrary = abilities;
    const int offset = 8;
    const std::string nameFile = "Dungeoncrawler_Category_Character.txt";
    std::ifstream inFile( nameFile, std::ios::in );
    std::string line;
    std::vector<std::string> cacheFile;
    std::vector<Character> result;
    auto GetBitmask = [] ( const std::string& line )
    {
        std::stringstream sstream( line );
        std::string value;
        int bitmask = 0;

        while( std::getline( sstream, value, ',' ) )
        {
            bitmask |= 1 << std::stoi( value );
        }

        return bitmask;
    };
    auto GetAbilities = [&abilitiesLibrary] ( const std::string& line )
    {
        std::stringstream sstream( line );
        std::string value;
        std::vector<Ability> abilities;

        while( std::getline( sstream, value, ',' ) )
        {
            abilities.push_back( abilitiesLibrary[std::stoi( value )] );
        }

        return abilities;
    };

    if( !inFile.is_open( ) )
    {
        throw std::exception( std::string( "Could not open file " + nameFile ).c_str( ) );
    }

    while( std::getline( inFile, line ) )
    {
        cacheFile.push_back( line );
    }

    cacheFile.erase( std::remove_if( cacheFile.begin( ), cacheFile.end( ), [] ( const std::string& line ) { return line[0] != ':'; } ), cacheFile.end( ) );
    std::for_each( cacheFile.begin( ), cacheFile.end( ), [] ( std::string& line ) { line.erase( 0, 1 ); } );

    for( unsigned int i = 0; i < cacheFile.size( ); i += offset )
    {
        result.emplace_back( cacheFile[0 + i],
                             cacheFile[1 + i].back( ),
                 GetBitmask( cacheFile[2 + i] ),
                  std::stoi( cacheFile[3 + i] ),
                  std::stoi( cacheFile[4 + i] ),
                  std::stoi( cacheFile[5 + i] ),
                  std::stof( cacheFile[6 + i] ),
               GetAbilities( cacheFile[7 + i] ) );
    }

    return result;
}
std::vector<Structure> EntityLibrary::LoadStructures( ) const
{
    const std::vector<Ability>& abilitiesLibrary = abilities;
    const int offset = 3;
    const std::string nameFile = "Dungeoncrawler_Category_Structure.txt";
    std::ifstream inFile( nameFile, std::ios::in );
    std::string line;
    std::vector<std::string> cacheFile;
    std::vector<Structure> result;
    auto GetBitmask = [] ( const std::string& line )
    {
        std::stringstream sstream( line );
        std::string value;
        int bitmask = 0;

        while( std::getline( sstream, value, ',' ) )
        {
            bitmask |= 1 << std::stoi( value );
        }

        return bitmask;
    };

    if( !inFile.is_open( ) )
    {
        throw std::exception( std::string( "Could not open file " + nameFile ).c_str( ) );
    }

    while( std::getline( inFile, line ) )
    {
        cacheFile.push_back( line );
    }

    cacheFile.erase( std::remove_if( cacheFile.begin( ), cacheFile.end( ), [] ( const std::string& line ) { return line[0] != ':'; } ), cacheFile.end( ) );
    std::for_each( cacheFile.begin( ), cacheFile.end( ), [] ( std::string& line ) { line.erase( 0, 1 ); } );

    for( unsigned int i = 0; i < cacheFile.size( ); i += offset )
    {
        result.emplace_back( cacheFile[0 + i],
                             cacheFile[1 + i].back( ),
                 GetBitmask( cacheFile[2 + i] ) );
    }

    return result;
}