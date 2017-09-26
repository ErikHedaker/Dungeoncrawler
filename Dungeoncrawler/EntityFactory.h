#pragma once

#include "Vector2.h"
#include "Enums.h"
#include "BattleSystem.h"
#include <string>
#include <memory>
#include <map>

struct Health
{
    int max;
    int current;
    int regeneration;
};

struct Entity
{
    Entity( const std::string& name, char icon, int attributes ) :
        name( name ),
        icon( icon ),
        attributes( attributes ),
        active( true ),
        position( { -1, -1 } )
    { }
    virtual ~Entity( ) { }

    virtual Entity* Clone( ) const = 0;

    const std::string name;
    const char icon;
    const int attributes;
    bool active;
    Vector2<int> position;
};

struct Item : public Entity
{
    Item( const std::string& name, char icon, int attributes ) :
        Entity( name, icon, attributes )
    { }

    Item* Clone( ) const override
    {
        return new Item( *this );
    }

    ItemType type;
};

struct Structure : public Entity
{
    Structure( const std::string& name, char icon, int attributes ) :
        Entity( name, icon, attributes )
    { }

    Structure* Clone( ) const override
    {
        return new Structure( *this );
    }
};

struct Character : public Entity
{
    Character( const std::string& name, char icon, int attributes, Health health, int damage, int spells ) :
        Entity( name, icon, attributes ),
        health( health ),
        damage( damage ),
        spells( spells )
    { }

    Character* Clone( ) const override
    {
        return new Character( *this );
    }
    void Update( )
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

    Health health;
    int damage;
    int spells;
    std::map<std::string, Effect> effects;
};

struct Player : public Character
{
    Player( const std::string& name, char icon, int attributes, Health health, int damage, int spells, int visionReach, int states ) :
        Character( name, icon, attributes, health, damage, spells ),
        visionReach( visionReach ),
        states( states )
    { }

    int visionReach;
    int states;
};

struct PlayerHandle
{
    PlayerHandle( const Player& player ) :
        base( std::make_unique<Player>( player ) ),
        real( dynamic_cast<Player*>( base.get( ) ) )
    { }

    void Reset( const Player& player )
    {
        base.reset( new Player( player ) );
        real = dynamic_cast<Player*>( base.get( ) );
    }

    std::unique_ptr<Entity> base;
    Player* real;
};

class EntityFactory
{
    public:
        EntityFactory( );

        const std::unique_ptr<Entity>& Get( const std::string& name ) const;
        const std::unique_ptr<Entity>& Get( char icon ) const;
        const std::vector<Entity*> Get( int bitmask ) const;
        const Player PlayerDefault( ) const;

    private:
        const std::map<std::string, std::unique_ptr<Entity>> _entities;
};