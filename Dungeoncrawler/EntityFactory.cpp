#include "EntityFactory.h"
#include "Functions.h"

EntityFactory::EntityFactory( ) :
    _entities( [](
    std::vector<Character> characters,
    std::vector<Structure> structures,
    Player player )
    {
        std::map<std::string, std::unique_ptr<Entity>> temp;

        for( const auto& character : characters )
        {
            temp[character.name] = std::make_unique<Character>( character );
        }

        for( const auto& structure : structures )
        {
            temp[structure.name] = std::make_unique<Structure>( structure );
        }

        temp["PlayerDefault"] = std::make_unique<Player>( player );

        return std::move( temp );
    }(
    LoadCharacters( ),
    LoadStructures( ),
    LoadPlayerDefault( ) ) )
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