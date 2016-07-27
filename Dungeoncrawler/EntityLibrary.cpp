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

EntityLibrary::EntityLibrary( )
{
    LoadCharacters( );
    LoadStructures( );
}

const Character& EntityLibrary::GetCharacter( int id ) const
{
    return _characters[id];
}
const Structure& EntityLibrary::GetStructure( int id ) const
{
    return _structures[id];
}
char EntityLibrary::GetIcon( Category::CategoryType category, int id ) const
{
    switch( category )
    {
        case Category::Character:
        {
            return _characters[id].icon;
        }

        case Category::Structure:
        {
            return _structures[id].icon;
        }
    }

    return '?';
}
int EntityLibrary::GetAttribute( Category::CategoryType category, int id ) const
{
    switch( category )
    {
        case Category::Character:
        {
            return _characters[id].attributes;
        }

        case Category::Structure:
        {
            return _structures[id].attributes;
        }
    }

    return 0;
}

void EntityLibrary::LoadCharacters( )
{
    const std::string fileName = "Dungeoncrawler_Category_Characters.txt";
    std::ifstream inFile( fileName, std::ios::in );
    std::string line;

    if( !inFile.is_open( ) )
    {
        throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
    }

    _characters.clear( );

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

            character.spells = 0;
            character.attributes = 0;

            std::vector<int> spellArgs( ( std::istream_iterator<int>( std::stringstream( args[6] ) ) ), std::istream_iterator<int>( ) );
            std::vector<int> attributeArgs( ( std::istream_iterator<int>( std::stringstream( args[7] ) ) ), std::istream_iterator<int>( ) );

            for( auto spell : spellArgs )
            {
                character.spells |= 1 << spell;
            }

            for( auto attribute : attributeArgs )
            {
                character.attributes |= 1 << attribute;
            }

            _characters.push_back( character );
        }
    }
}
void EntityLibrary::LoadStructures( )
{
    const std::string fileName = "Dungeoncrawler_Category_Structures.txt";
    std::ifstream inFile( fileName, std::ios::in );
    std::string line;

    if( !inFile.is_open( ) )
    {
        throw std::exception( std::string( "Could not open file " + fileName ).c_str( ) );
    }

    _structures.clear( );

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

            _structures.push_back( structure );
        }
    }
}