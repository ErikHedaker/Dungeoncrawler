#pragma once

#include "Vector2.h"
#include "Enums.h"
#include "BattleSystem.h"
#include <string>
#include <memory>
#include <map>

struct Player;

struct Connector
{
    int indexDungeon;
    int indexDoor;
};

struct Health
{
    int max;
    int current;
    int regeneration;
};

struct Entity
{
    Entity( const std::string& name, char icon, int attributes );
    virtual ~Entity( );

    virtual Entity* Clone( ) const;
    virtual void Interact( Player& player );

    const std::string name;
    const char icon;
    const int attributes;
    bool active;
    Vector2<int> position;
};

struct Door : public Entity
{
    Door( const std::string& name, char icon, int attributes );

    Door* Clone( ) const override;
    void Interact( Player& player ) override;

    std::optional<Connector> connector;
};

struct Wall : public Entity
{
    Wall( const std::string& name, char icon, int attributes );

    Wall* Clone( ) const override;
    void Interact( Player& player ) override;
};

struct Character : public Entity
{
    Character( const std::string& name, char icon, int attributes, Health health, int damage, int spells );

    Character* Clone( ) const override;
    void Update( );

    Health health;
    int damage;
    int spells;
    std::map<std::string, Effect> effects;
};

struct Player : public Character
{
    Player( const std::string& name, char icon, int attributes, Health health, int damage, int spells, int visionReach );

    int visionReach;
    bool blocked;
    std::optional<Connector> next;
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

        const std::unique_ptr<Entity>& Get( const std::string& name ) const;
        const std::unique_ptr<Entity>& Get( char icon ) const;
        const std::vector<Entity*> Get( int bitmask ) const;
        const Player PlayerDefault( ) const;

    private:
        const std::map<std::string, std::unique_ptr<Entity>> _entities;
};