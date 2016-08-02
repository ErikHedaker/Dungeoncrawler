#pragma once

#include "Enums.h"
#include <vector>
#include <string>
#include <memory>

struct BaseEntity
{
    BaseEntity( const std::string& name, char icon, int attributes );

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
    PlayerEntity( Player& player );

    Player& player;
};

class EntityLibrary
{
    public:
        EntityLibrary( );

        std::unique_ptr<PlayerEntity> player;
        const std::vector<Ability> abilities;
        const std::vector<Character> characters;
        const std::vector<Structure> structures;

    private:
        std::vector<Ability> LoadAbilities( ) const;
        std::vector<Character> LoadCharacters( ) const;
        std::vector<Structure> LoadStructures( ) const;
};