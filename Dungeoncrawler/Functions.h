#pragma once

#include "Game.h"
#include "Vector2.h"
#include "Enums.h"
#include "Dungeon.h"
#include <vector>
#include <string>
#include <iostream>
#include <functional>

double RandomNumberGenerator( double min, double max );
int RandomNumberGenerator( int min, int max );
int GetPositiveInteger( const std::string& context );
char GetChar( const std::string& context, const std::vector<char>& valid, std::function<int(int)> modifier = nullptr );
void GetEnter( );
DungeonConfiguration GetDungeonConfiguration( const GameConfig& type = GameConfig::Default );
Vector2<int> PositionRotate( const Vector2<int>& position, const Vector2<int>& size, const Orientation::Enum& orientation );
Vector2<int> PositionMove( const Vector2<int>& position, const Orientation::Enum& orientation );
Vector2<int> PositionMoveProbability( const Vector2<int>& position, int north, int west, int south, int east, int still );
Ability GetAbility( const std::vector<Ability>& abilities );
std::string UseAbility( const Ability& ability, Character& caster, Character& target );
std::string UseWeapon( Character& attacker, Character& target );
std::string TurnPlayer( Character& player, Character& AI );
std::string TurnAI( Character& player, Character& AI );
void PrintDungeonCentered( const Dungeon& dungeon, int visionReach, const Vector2<int>& center, const Vector2<int>& sizeScreen = { 40, 20 } );
void PrintHealth( const Character& character );
void Combat( Character& player, Character& AI );
void SaveDungeonSystem( const DungeonSystem& dungeonSystem );
DungeonSystem LoadDungeonSystem( const EntityFactory& entityLibrary );
std::vector<Ability> LoadAbilities( );
std::vector<Character> LoadCharacters( const std::vector<Ability>& abilitiesLibrary );
std::vector<Structure> LoadStructures( );
Player LoadPlayerDefault( const std::vector<Ability>& abilities );

class StringWrapper
{
    public:
        friend std::istream &operator>>( std::istream &stream, StringWrapper &line )
        {
            while( std::getline( stream, line.data ) )
            {
                if( !line.data.empty( ) && line.data[0] == ':' )
                {
                    line.data = line.data.substr( 1, line.data.find( '#' ) );

                    break;
                }
            }

            return stream;
        }

        operator std::string( ) const
        {
            return data;
        }

    private:
        std::string data;   
};