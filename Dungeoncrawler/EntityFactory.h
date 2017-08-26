#pragma once

#include "Vector2.h"
#include "Enums.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

struct Entity
{
    Entity( const std::string& name, char icon, int attributes ) :
        name( name ),
        icon( icon ),
        attributes( attributes ),
        position( { -1, -1 } ),
        positionPrevious( { -1, -1 } )
    { }

    virtual ~Entity( ) { };

    virtual Entity* Clone( ) const = 0;
    virtual void Update( )
    { };

    std::string name;
    char icon;
    int attributes;
    Vector2<int> position;
    Vector2<int> positionPrevious;
};

struct Ability : public Entity
{
    Ability( const std::string& name, char icon, int attributes, float damage ) :
        Entity( name, icon, attributes ),
        damage( damage )
    { }

    virtual Ability* Clone( ) const
    {
        return new Ability( *this );
    }

    float damage;
};

struct Character : public Entity
{
    Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegeneration, float damage, const std::vector<Ability>& abilities ) :
        Entity( name, icon, attributes ),
        health( health ),
        healthMax( healthMax ),
        healthRegeneration( healthRegeneration ),
        damage( damage ),
        abilities( abilities )
    { }

    virtual Character* Clone( ) const
    {
        return new Character( *this );
    }
    void Update( ) override
    {
        health += healthRegeneration;

        if( health > healthMax )
        {
            health = healthMax;
        }
    }

    int health;
    int healthMax;
    int healthRegeneration;
    float damage;
    std::vector<Ability> abilities;
};

struct Structure : public Entity
{
    Structure( const std::string& name, char icon, int attributes ) :
        Entity( name, icon, attributes )
    { }

    virtual Structure* Clone( ) const
    {
        return new Structure( *this );
    }
};

struct Player : public Character
{
    Player( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities, int visionReach, int states ) :
        Character( name, icon, attributes, health, healthMax, healthRegen, damage, abilities ),
        visionReach( visionReach ),
        states( states )
    { }

    int visionReach;
    int states;
};

struct Item : public Entity
{
    Item( const std::string& name, char icon, int attributes ) :
        Entity( name, icon, attributes )
    { }
    
    virtual Item* Clone( ) const
    {
        return new Item( *this );
    }

    ItemType type;
};

struct PlayerPair
{
    PlayerPair::PlayerPair( const Player& player ) :
        base( std::make_unique<Player>( player ) ),
        real( dynamic_cast<Player*>( base.get( ) ) )
    { }

    std::unique_ptr<Entity> base;
    Player* const real;
};

class EntityFactory
{
    public:
        EntityFactory( );

        const std::unique_ptr<Entity>& Get( const std::pair<EntityType::Enum, int>& id ) const;
        const std::unique_ptr<Entity>& Get( const std::string& name ) const;
        const std::unique_ptr<Entity>& Get( char icon ) const;

    private:
        const std::map<std::pair<EntityType::Enum, int>, std::unique_ptr<Entity>> _entities;
};