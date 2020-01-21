#include "EntityFactory.h"
#include "Functions.h"

Entity::Entity( const std::string& name, char icon, int attributes ) :
    name( name ),
    icon( icon ),
    attributes( attributes ),
    active( true ),
    position( { -1, -1 } )
{ }
Entity::~Entity( )
{

}
Entity* Entity::Clone( ) const
{
    return new Entity( *this );
}
void Entity::Interact( Player& player )
{

}

Door::Door( const std::string& name, char icon, int attributes ) :
    Entity( name, icon, attributes )
{ }
Door* Door::Clone( ) const
{
    return new Door( *this );
}
void Door::Interact( Player& player )
{
    player.next = connector;
}

Wall::Wall( const std::string& name, char icon, int attributes ) :
    Entity( name, icon, attributes )
{ }
Wall* Wall::Clone( ) const
{
    return new Wall( *this );
}
void Wall::Interact( Player& player )
{
    player.blocked = true;
}

Character::Character( const std::string& name, char icon, int attributes, Health health, int damage, int spells ) :
    Entity( name, icon, attributes ),
    health( health ),
    damage( damage ),
    spells( spells )
{ }
Character* Character::Clone( ) const
{
    return new Character( *this );
}
void Character::Update( )
{
    if( health.current > 0 )
    {
        health.current += health.regeneration;
    }
    else
    {
        active = false;
    }

    if( health.current > health.max )
    {
        health.current = health.max;
    }
}

Player::Player( const std::string& name, char icon, int attributes, Health health, int damage, int spells, int visionReach ) :
    Character( name, icon, attributes, health, damage, spells ),
    visionReach( visionReach ),
    blocked( false )
{ }

PlayerHandle::PlayerHandle( const Player& player ) :
    base( std::make_unique<Player>( player ) ),
    real( dynamic_cast<Player*>( base.get( ) ) )
{ }
void PlayerHandle::Reset( const Player& player )
{
    base.reset( new Player( player ) );
    real = dynamic_cast<Player*>( base.get( ) );
}

EntityFactory::EntityFactory( ) :
    _entities( [] ( std::vector<Character> characters, Player player )
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
    } ( LoadCharacters( ), LoadPlayerDefault( ) ) )
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