#include "EntityFactory.h"
#include "Functions.h"
#include <algorithm>

EntityFactory::EntityFactory( Player& player ) :
    _entities( [&player] ( )
    {
        std::map<std::pair<Category::CategoryType, int>, std::unique_ptr<BaseEntity>> entities;
        auto abilities = LoadAbilities( );
        auto characters = LoadCharacters( abilities );
        auto structures = LoadStructures( );

        entities.emplace( std::make_pair( Category::PlayerEntity, 0 ), std::make_unique<PlayerEntity>( PlayerEntity( player ) ) );

        for( unsigned int i = 0; i < abilities.size( ); i++ )
        {
            entities.emplace( std::make_pair( Category::Ability, i ), std::make_unique<Ability>( abilities[i] ) );
        }

        for( unsigned int i = 0; i < characters.size( ); i++ )
        {
            entities.emplace( std::make_pair( Category::Character, i ), std::make_unique<Character>( characters[i] ) );
        }

        for( unsigned int i = 0; i < structures.size( ); i++ )
        {
            entities.emplace( std::make_pair( Category::Structure, i ), std::make_unique<Structure>( structures[i] ) );
        }

        return entities;
    }( ) )
{ }

const std::unique_ptr<BaseEntity>& EntityFactory::Get( const std::pair<Category::CategoryType, int>& id ) const
{
    return _entities.at( id );
}
const std::unique_ptr<BaseEntity>& EntityFactory::Get( const std::string& name ) const
{
    for( const auto& it : _entities )
    {
        if( it.second->name == name )
        {
            return it.second;
        }
    }

    throw;
}
const std::unique_ptr<BaseEntity>& EntityFactory::Get( char icon ) const
{
    for( const auto& it : _entities )
    {
        if( it.second->icon == icon )
        {
            return it.second;
        }
    }

    throw;
}