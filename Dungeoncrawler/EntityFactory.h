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
    Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities ) :
        Entity( name, icon, attributes ),
        health( health ),
        healthMax( healthMax ),
        healthRegen( healthRegen ),
        damage( damage ),
        abilities( abilities )
    { }

    virtual Character* Clone( ) const
    {
        return new Character( *this );
    }

    void Update( )
    {
        health += healthRegen;

        if( health > healthMax )
        {
            health = healthMax;
        }
    }

    int health;
    int healthMax;
    int healthRegen;
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

    virtual Player* Clone( ) const
    {
        return new Player( *this );
    }

    int visionReach;
    int states;
};

struct PlayerLocal : public Entity
{
    PlayerLocal( Player& player ) :
        Entity( player.name, player.icon, player.attributes ),
        player( player )
    { }

    virtual PlayerLocal* Clone( ) const
    {
        return new PlayerLocal( *this );
    }

    Player& player;
};

class EntityFactory
{
    public:
        EntityFactory( Player& player );

        const std::unique_ptr<Entity>& Get( const std::pair<Category::CategoryType, int>& id ) const;
        const std::unique_ptr<Entity>& Get( const std::string& name ) const;
        const std::unique_ptr<Entity>& Get( char icon ) const;

    private:
        const std::map<std::pair<Category::CategoryType, int>, std::unique_ptr<Entity>> _entities;
};