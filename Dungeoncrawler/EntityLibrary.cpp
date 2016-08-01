#include "EntityLibrary.h"
#include "Functions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>

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
{
    for( const auto& character : characters )
    {
        _baseEntities.push_back( &character );
    }

    for( const auto& structure : structures )
    {
        _baseEntities.push_back( &structure );
    }
}

const BaseEntity& EntityLibrary::GetBaseEntity( const Category::CategoryType& category, int id ) const
{
    const int index = id + ( category == Category::Structure ? characters.size( ) : 0 );

    return *_baseEntities[index];
}

std::vector<Ability> EntityLibrary::LoadAbilities( ) const
{
    const std::string fileName = "Dungeoncrawler_Category_Skills.txt";
    std::ifstream inFile( fileName, std::ios::in );
    std::string line;
    std::vector<Ability> temp;

    if( !inFile.is_open( ) )
    {
        throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
    }

    while( std::getline( inFile, line ) )
    {
        if( !line.empty( ) && line[0] == ':' )
        {
            line.erase( 0, 1 );
            std::vector<std::string> args( { line } );
            Ability ability;

            for( int i = 0; i < 2; i++ )
            {
                std::getline( inFile, line );
                line.erase( 0, 1 );
                args.push_back( line );
            }

            ability.name = args[0];
            ability.icon = args[1][0];
            ability.attributes = 0;
            ability.damage = std::stoi( args[3] );

            std::vector<int> attributeArgs( ( std::istream_iterator<int>( std::stringstream( args[2] ) ) ), std::istream_iterator<int>( ) );

            for( auto attribute : attributeArgs )
            {
                ability.attributes |= 1 << attribute;
            }

            temp.push_back( ability );
        }
    }

    return temp;
}
std::vector<Character> EntityLibrary::LoadCharacters( ) const
{
    const std::string fileName = "Dungeoncrawler_Category_Characters.txt";
    std::ifstream inFile( fileName, std::ios::in );
    std::string line;
    std::vector<Character> temp;

    if( !inFile.is_open( ) )
    {
        throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
    }

    while( std::getline( inFile, line ) )
    {
        if( !line.empty( ) && line[0] == ':' )
        {
            line.erase( 0, 1 );
            std::vector<std::string> args( { line } );
            Character character;

            for( int i = 0; i < 7; i++ )
            {
                std::getline( inFile, line );
                line.erase( 0, 1 );
                args.push_back( line );
            }

            character.name = args[0];
            character.icon = args[1][0];
            character.health = std::stoi( args[2] );
            character.healthMax = std::stoi( args[3] );
            character.healthRegen = std::stoi( args[4] );
            character.damage = std::stoi( args[5] );
            character.attributes = 0;

            std::vector<int> abilityArgs( ( std::istream_iterator<int>( std::stringstream( args[6] ) ) ), std::istream_iterator<int>( ) );
            std::vector<int> attributeArgs( ( std::istream_iterator<int>( std::stringstream( args[7] ) ) ), std::istream_iterator<int>( ) );

            for( auto i : abilityArgs )
            {
                character.abilities.push_back( abilities[i] );
            }

            for( auto attribute : attributeArgs )
            {
                character.attributes |= 1 << attribute;
            }

            temp.push_back( character );
        }
    }

    return temp;
}
std::vector<Structure> EntityLibrary::LoadStructures( ) const
{
    const std::string fileName = "Dungeoncrawler_Category_Structures.txt";
    std::ifstream inFile( fileName, std::ios::in );
    std::string line;
    std::vector<Structure> temp;

    if( !inFile.is_open( ) )
    {
        throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
    }

    while( std::getline( inFile, line ) )
    {
        if( !line.empty( ) && line[0] == ':' )
        {
            line.erase( 0, 1 );
            std::vector<std::string> args( { line } );
            Structure structure;

            for( int i = 0; i < 2; i++ )
            {
                std::getline( inFile, line );
                line.erase( 0, 1 );
                args.push_back( line );
            }

            structure.name = args[0];
            structure.icon = args[1][0];
            structure.attributes = 0;

            std::vector<int> attributeArgs( ( std::istream_iterator<int>( std::stringstream( args[2] ) ) ), std::istream_iterator<int>( ) );

            for( auto attribute : attributeArgs )
            {
                structure.attributes |= 1 << attribute;
            }

            temp.push_back( structure );
        }
    }

    return temp;
}