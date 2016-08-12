#pragma once

#include "Enums.h"
#include <vector>
#include <string>
#include <memory>
#include <map>

struct BaseEntity
{
    BaseEntity( const std::string& name, char icon, int attributes );

    virtual ~BaseEntity( ) = default;

    std::string name;
    char icon;
    int attributes;
};

struct Ability : public BaseEntity
{
    Ability( const std::string& name, char icon, int attributes, float damage );

    float damage;
};

struct Character : public BaseEntity
{
    Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities );

    int health;
    int healthMax;
    int healthRegen;
    float damage;
    std::vector<Ability> abilities;

    void Update( );
};

struct Structure : public BaseEntity
{
    Structure( const std::string& name, char icon, int attributes );
};

struct Player : public Character
{
    Player( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities, int visionReach, int states );

    int visionReach;
    int states;
};

struct PlayerEntity : public BaseEntity
{
    PlayerEntity( Player* player );

    Player* player;
};

struct EntityLibrary
{
    EntityLibrary( Player* player );

    const PlayerEntity playerEntity;
    const std::vector<Ability> abilities;
    const std::vector<Character> characters;
    const std::vector<Structure> structures;
};

//class EntityFactory
//{
//    public:
//        EntityFactory( Player* player );
//
//        const std::unique_ptr<BaseEntity>& Get( const std::pair<Category::CategoryType, int>& id ) const;
//        const std::unique_ptr<BaseEntity>& Get( const std::string& name ) const;
//        const std::unique_ptr<BaseEntity>& Get( char icon ) const;
//
//    private:
//        std::map<std::pair<Category::CategoryType, int>, std::unique_ptr<BaseEntity>> _entities;
//};