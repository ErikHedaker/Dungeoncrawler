#include "EntityFactory.h"
#include "Functions.h"

void Entity::Interact( Player* player )
{

}
void Door::Interact( Player* player )
{
    player->next = connector;
}
void Wall::Interact( Player* player )
{
    player->blocked = true;
}
void Character::Interact( Player* player )
{

}
void Player::Interact( Player* player )
{

}

EntityFactory::EntityFactory( ) :
    _entities( [](
    std::vector<Character> characters,
    Player player )
    {
        std::map<std::string, std::unique_ptr<Entity>> temp;

        for( const auto& character : characters )
        {
            temp.insert_or_assign( character.name, std::make_unique<Character>( character ) );
        }

        temp.insert_or_assign( "Hidden", std::make_unique<Entity>( "Hidden", '-', 0 ) );
        temp.insert_or_assign( "Door", std::make_unique<Door>( "Door", '+', Attributes::Obstacle ) );
        temp.insert_or_assign( "Wall", std::make_unique<Wall>( "Wall", '#', Attributes::Obstacle ) );
        temp.insert_or_assign( "PlayerDefault", std::make_unique<Player>( player ) );

        return std::move( temp );
    }(
    LoadCharacters( ),
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