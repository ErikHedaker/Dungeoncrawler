/*
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
    PlayerEntity( Player& player );

    Player& player;
};

struct EntityLibrary
{
    EntityLibrary( );

    std::unique_ptr<PlayerEntity> player;
    const std::vector<Ability> abilities;
    const std::vector<Character> characters;
    const std::vector<Structure> structures;
};

class VariantinatorBase
{
    public:
        VariantinatorBase( ){ }
        virtual ~VariantinatorBase( ){ }
        virtual std::unique_ptr<BaseEntity> Create( ) = 0;
};

template<class T>
class Variantinator : public VariantinatorBase
{
    public:
        Variantinator( ){ }
        virtual ~Variantinator( ){ }
        virtual std::unique_ptr<BaseEntity> Create( )
        {
            return new T;
        }
};

class VariantFactory
{
    public:
        VariantFactory( )
        {
            // If you want, you can do all your Register() calls in here, and even
            // make the Register() function private.
        }

        void Register( std::pair<int, int> id, std::unique_ptr<VariantinatorBase> creator )
        {
            _entities[id] = creator;
        }

        std::unique_ptr<BaseEntity> Create( std::pair<int, int> id )
        {
            auto it = _entities.find( id );

            if( it == _entities.end( ) )
            {
                return nullptr;
            }
            
            return it->second->Create( );
        }

    private:
        std::map<std::pair<int, int>, std::unique_ptr<VariantinatorBase>> _entities;
};

//VariantFactory factory;
//factory.Register( 0, new Variantinator<VariantA> );
//factory.Register( 1, new Variantinator<VariantB> );
//factory.Register( 2, new Variantinator<VariantC> );
//
//std::unique_ptr<Variant> thing = factory.Create( switchValue );
*/