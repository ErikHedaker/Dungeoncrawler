#include "EntityFactory.h"
#include "Functions.h"

EntityFactory::EntityFactory( ) :
    _entities( [] ( )
{
    std::map<std::string, std::unique_ptr<Entity>> entities;

    for( const auto& character : LoadCharacters( ) )
    {
        entities[character.name] = std::make_unique<Character>( character );
    }

    for( const auto& structure : LoadStructures( ) )
    {
        entities[structure.name] = std::make_unique<Structure>( structure );
    }

    entities["PlayerDefault"] = std::make_unique<Player>( LoadPlayerDefault( ) );

    return std::move( entities );
}( ) )
{ }

const std::unique_ptr<Entity>& EntityFactory::Get( const std::string& name ) const
{
    return _entities.at( name );
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
const std::vector<Entity*> EntityFactory::Get( int bitmask ) const
{
    std::vector<Entity*> matching;

    for( const auto& it : _entities )
    {
        if( it.second->attributes & bitmask )
        {
            matching.push_back( it.second.get( ) );
        }
    }

    return matching;
}
const Player EntityFactory::PlayerDefault( ) const
{
    return *dynamic_cast<Player*>( Get( "PlayerDefault" ).get( ) );
}