#pragma once

#include "Enums.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

struct BaseEntity
{
    BaseEntity( const std::string& name, char icon, int attributes ) :
        name( name ),
        icon( icon ),
        attributes( attributes )
    { }

    virtual ~BaseEntity( ) { };

    virtual BaseEntity* Clone( ) const = 0;

    std::string name;
    char icon;
    int attributes;
};

struct Ability : public BaseEntity
{
    Ability( const std::string& name, char icon, int attributes, float damage ) :
        BaseEntity( name, icon, attributes ),
        damage( damage )
    { }

    virtual Ability* Clone( ) const
    {
        return new Ability( *this );
    }

    float damage;
};

struct Character : public BaseEntity
{
    Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities ) :
        BaseEntity( name, icon, attributes ),
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

struct Structure : public BaseEntity
{
    Structure( const std::string& name, char icon, int attributes ) :
        BaseEntity( name, icon, attributes )
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

struct PlayerEntity : public BaseEntity
{
    PlayerEntity( Player& player ) :
        BaseEntity( player.name, player.icon, player.attributes ),
        player( player )
    { }

    virtual PlayerEntity* Clone( ) const
    {
        return new PlayerEntity( *this );
    }

    Player& player;
};

class EntityFactory
{
    public:
        EntityFactory( Player& player );

        const std::unique_ptr<BaseEntity>& Get( const std::pair<Category::CategoryType, int>& id ) const;
        const std::unique_ptr<BaseEntity>& Get( const std::string& name ) const;
        const std::unique_ptr<BaseEntity>& Get( char icon ) const;

    private:
        const std::map<std::pair<Category::CategoryType, int>, std::unique_ptr<BaseEntity>> _entities;
};