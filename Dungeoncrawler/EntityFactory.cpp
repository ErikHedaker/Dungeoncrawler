/*
#include "EntityLibrary.h"
#include "Functions.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

BaseEntity::BaseEntity( const std::string& name, char icon, int attributes ) :
    name( name ),
    icon( icon ),
    attributes( attributes )
{
}

Ability::Ability( const std::string& name, char icon, int attributes, float damage ) :
    BaseEntity( name, icon, attributes ),
    damage( damage )
{
}

Character::Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities ) :
    BaseEntity( name, icon, attributes ),
    health( health ),
    healthMax( healthMax ),
    healthRegen( healthRegen ),
    damage( damage ),
    abilities( abilities )
{
}

Structure::Structure( const std::string& name, char icon, int attributes ) :
    BaseEntity( name, icon, attributes )
{
}

Player::Player( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities, int visionReach, int states ) :
    Character( name, icon, attributes, health, healthMax, healthRegen, damage, abilities ),
    visionReach( visionReach ),
    states( states )
{
}

PlayerEntity::PlayerEntity( Player& player ) :
    BaseEntity( player.name, player.icon, player.attributes ),
    player( player )
{
}

void Character::Update( )
{
    health += healthRegen;

    if( health > healthMax )
    {
        health = healthMax;
    }
}

EntityLibrary::EntityLibrary( ) :
    abilities( LoadAbilities( ) ),
    characters( LoadCharacters( abilities ) ),
    structures( LoadStructures( ) )
{
}
*/