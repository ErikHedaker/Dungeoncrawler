#pragma once

#include "Vector2.h"
#include "Enums.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

struct Entity
{
    Entity( const std::string& name, char icon, int attributes );
    virtual ~Entity( ) { }

    virtual Entity* Clone( ) const = 0;
    virtual void Update( ) { }

    const std::string name;
    const char icon;
    const int attributes;
    bool active;
    Vector2<int> position;

};

struct Ability : public Entity
{
    Ability( const std::string& name, char icon, int attributes, float damage );

    Ability* Clone( ) const override
    {
        return new Ability( *this );
    }

    float damage;
};

struct Character : public Entity
{
    Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegeneration, float damage, const std::vector<Ability>& abilities );

    Character* Clone( ) const override
    {
        return new Character( *this );
    }
    void Update( ) override
    {
        if( health > 0 )
        {
            health += healthRegeneration;
        }

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
    Structure( const std::string& name, char icon, int attributes );

    Structure* Clone( ) const override
    {
        return new Structure( *this );
    }
};

struct Item : public Entity
{
    Item( const std::string& name, char icon, int attributes );

    Item* Clone( ) const override
    {
        return new Item( *this );
    }

    ItemType type;
};

struct Player : public Character
{
    Player( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities, int visionReach, int states );

    int visionReach;
    int states;
};

struct PlayerHandle
{
    PlayerHandle( const Player& player );

    void Reset( const Player& player );

    std::unique_ptr<Entity> base;
    Player* real;
};

class EntityFactory
{
    public:
        EntityFactory( );
        
        const std::vector<Ability> abilities;
        const std::vector<Structure> structures;
        const std::vector<Character> characters;
        const Player player;

        const std::unique_ptr<Entity>& Get( const std::string& name ) const;
        const std::unique_ptr<Entity>& Get( char icon ) const;

    private:
        const std::map<std::string, std::unique_ptr<Entity>> _entities;
};