#include "EntityFactory.h"
#include "Functions.h"
#include <algorithm>

Entity::Entity( const std::string& name, char icon, int attributes ) :
    name( name ),
    icon( icon ),
    attributes( attributes ),
    active( true ),
    position( { -1, -1 } )
{ }
Ability::Ability( const std::string& name, char icon, int attributes, float damage ) :
    Entity( name, icon, attributes ),
    damage( damage )
{ }
Character::Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegeneration, float damage, const std::vector<Ability>& abilities ) :
    Entity( name, icon, attributes ),
    health( health ),
    healthMax( healthMax ),
    healthRegeneration( healthRegeneration ),
    damage( damage ),
    abilities( abilities )
{ }
Structure::Structure( const std::string& name, char icon, int attributes ) :
    Entity( name, icon, attributes )
{ }
Item::Item( const std::string& name, char icon, int attributes ) :
    Entity( name, icon, attributes )
{ }
Player::Player( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities, int visionReach, int states ) :
    Character( name, icon, attributes, health, healthMax, healthRegen, damage, abilities ),
    visionReach( visionReach ),
    states( states )
{ }
PlayerHandle::PlayerHandle( const Player& player ) :
    base( std::make_unique<Player>( player ) ),
    real( dynamic_cast<Player*>( base.get( ) ) )
{ }
EntityFactory::EntityFactory( ) :
    _entities( []( )
    {
        const std::vector<Ability> abilities = LoadAbilities( );
        const std::vector<Character> characters = LoadCharacters( abilities );
        const std::vector<Structure> structures = LoadStructures( );
        std::map<std::string, std::unique_ptr<Entity>> entities;

        for( const auto& ability : abilities )
        {
            entities[ability.name] = std::make_unique<Ability>( ability );
        }

        for( const auto& character : characters )
        {
            entities[character.name] = std::make_unique<Character>( character );
        }

        for( const auto& structure : structures )
        {
            entities[structure.name] = std::make_unique<Structure>( structure );
        }

        return entities;
    }( ) )
{ }

void PlayerHandle::Reset( const Player& player )
{
    base.reset( new Player( player ) );
    real = dynamic_cast<Player*>( base.get( ) );
}

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