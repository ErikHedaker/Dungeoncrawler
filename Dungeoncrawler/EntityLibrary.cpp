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
{ }

Ability::Ability( const std::string& name, char icon, int attributes, float damage ) :
    BaseEntity( name, icon, attributes ),
    damage( damage )
{ }

Character::Character( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities ) :
    BaseEntity( name, icon, attributes ),
    health( health ),
    healthMax( healthMax ),
    healthRegen( healthRegen ),
    damage( damage ),
    abilities( abilities )
{ }

Structure::Structure( const std::string& name, char icon, int attributes ) :
    BaseEntity( name, icon, attributes )
{ }

Player::Player( const std::string& name, char icon, int attributes, int health, int healthMax, int healthRegen, float damage, const std::vector<Ability>& abilities, int visionReach, int states ) :
    Character( name, icon, attributes, health, healthMax, healthRegen, damage, abilities ),
    visionReach( visionReach ),
    states( states )
{ }

PlayerEntity::PlayerEntity( Player* player ) :
    BaseEntity( player->name, player->icon, player->attributes ),
    player( player )
{ }

void Character::Update( )
{
    health += healthRegen;

    if( health > healthMax )
    {
        health = healthMax;
    }
}

EntityLibrary::EntityLibrary( Player* player ) :
    playerEntity( player ),
    abilities( LoadAbilities( ) ),
    characters( LoadCharacters( abilities ) ),
    structures( LoadStructures( ) )
{ }

//EntityFactory::EntityFactory( Player* player )
//{
//    auto abilities = LoadAbilities( );
//    auto characters = LoadCharacters( abilities );
//    auto structures = LoadStructures( );
//
//    for( int i = 0; i < abilities.size( ); i++ )
//    {
//        _entities.emplace( std::make_pair( Category::Ability, i ), std::make_unique<Ability>( abilities[i] ) );
//    }
//
//    for( int i = 0; i < characters.size( ); i++ )
//    {
//        _entities.emplace( std::make_pair( Category::Character, i ), std::make_unique<Character>( characters[i] ) );
//    }
//
//    for( int i = 0; i < structures.size( ); i++ )
//    {
//        _entities.emplace( std::make_pair( Category::Structure, i ), std::make_unique<Structure>( structures[i] ) );
//    }
//
//    _entities.emplace( std::make_pair( Category::PlayerEntity, 0 ), std::make_unique<PlayerEntity>( PlayerEntity( player ) ) );
//}
//
//const std::unique_ptr<BaseEntity>& EntityFactory::Get( const std::pair<Category::CategoryType, int>& id ) const
//{
//    return _entities.at( id );
//}
//const std::unique_ptr<BaseEntity>& EntityFactory::Get( const std::string& name ) const
//{
//    for( const auto& it : _entities )
//    {
//        if( it.second->name == name )
//        {
//            return it.second;
//        }
//    }
//
//    throw;
//}
//const std::unique_ptr<BaseEntity>& EntityFactory::Get( char icon ) const
//{
//    for( const auto& it : _entities )
//    {
//        if( it.second->icon == icon )
//        {
//            return it.second;
//        }
//    }
//
//    throw;
//}