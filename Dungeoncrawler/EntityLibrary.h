#pragma once

#include "Enums.h"
#include <vector>
#include <string>

struct BaseEntity
{
    std::string name;
    char icon;
    int attributes;
};

struct Ability : public BaseEntity
{
    float damage;
};

struct Character : public BaseEntity
{
    int health;
    int healthMax;
    int healthRegen;
    float damage;
    std::vector<Ability> abilities;

    void Update( );
};

struct Structure : public BaseEntity
{ };

class EntityLibrary
{
    public:
        EntityLibrary( );

        const std::vector<Ability> abilities;
        const std::vector<Character> characters;
        const std::vector<Structure> structures;

        const BaseEntity& GetBaseEntity( const Category::CategoryType& category, int id ) const;

    private:
        std::vector<const BaseEntity*> _baseEntities;

        std::vector<Ability> LoadAbilities( ) const;
        std::vector<Character> LoadCharacters( ) const;
        std::vector<Structure> LoadStructures( ) const;
};