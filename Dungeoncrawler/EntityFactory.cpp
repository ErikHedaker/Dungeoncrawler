#include "EntityFactory.h"
#include "Functions.h"
#include <algorithm>

EntityFactory::EntityFactory( Player& player, const std::vector<Ability>& abilities, const std::vector<Character>& characters, const std::vector<Structure>& structures ) :
    _entities( [&player, &abilities, &characters, &structures] ( )
    {
        std::map<std::pair<EntityType::Enum, int>, std::unique_ptr<Entity>> entities;

        entities.emplace( std::make_pair( EntityType::PlayerEntity, 0 ), std::make_unique<PlayerLocal>( PlayerLocal( player ) ) );

        for( unsigned int i = 0; i < abilities.size( ); i++ )
        {
            entities.emplace( std::make_pair( EntityType::Ability, i ), std::make_unique<Ability>( abilities[i] ) );
        }

        for( unsigned int i = 0; i < characters.size( ); i++ )
        {
            entities.emplace( std::make_pair( EntityType::Character, i ), std::make_unique<Character>( characters[i] ) );
        }

        for( unsigned int i = 0; i < structures.size( ); i++ )
        {
            entities.emplace( std::make_pair( EntityType::Structure, i ), std::make_unique<Structure>( structures[i] ) );
        }

        return entities;
    }( ) )
{ }

const std::unique_ptr<Entity>& EntityFactory::Get( const std::pair<EntityType::Enum, int>& id ) const
{
    return _entities.at( id );
}
const std::unique_ptr<Entity>& EntityFactory::Get( const std::string& name ) const
{
    for( const auto& it : _entities )
    {
        if( it.second->name == name )
        {
            return it.second;
        }
    }

    throw std::exception( "Entity does not exist!" );
}
const std::unique_ptr<Entity>& EntityFactory::Get( char icon ) const
{
    for( const auto& it : _entities )
    {
        if( it.second->icon == icon )
        {
            return it.second;
        }
    }

    throw std::exception( "Entity does not exist!" );
}