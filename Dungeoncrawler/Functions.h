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
int GetBitmask( const std::string& line );
int GetPositiveInteger( const std::string& context );
char GetChar( const std::string& context, const std::vector<char>& valid, std::function<int(int)> modifier = nullptr );
void GetEnter( );
void ClearScreen( );
DungeonConfiguration GetDungeonConfiguration( );
Vector2<int> PositionRotate( const Vector2<int>& position, const Vector2<int>& size, const Orientation::Enum& rotation );
Vector2<int> PositionMove( const Vector2<int>& position, const Orientation::Enum& orientation );
Vector2<int> PositionMoveProbability( const Vector2<int>& position, int north, int west, int south, int east, int still );
Ability GetAbility( const std::vector<Ability>& abilities );
std::string UseAbility( Character& attacker, Character& target, const Ability& ability );
std::string UseWeapon( Character& attacker, Character& target );
std::string TurnPlayer( Character& player, Character& AI );
std::string TurnAI( Character& player, Character& AI );
void PrintDungeon( const Dungeon& dungeon, int visionReach, const Vector2<int>& center, const Vector2<int>& sizeScreen );
void PrintHealth( const Character& character );
void Fight( Character& player, Character& AI );
std::vector<Ability> LoadAbilities( );
std::vector<Character> LoadCharacters( const std::vector<Ability>& abilitiesLibrary );
std::vector<Structure> LoadStructures( );
Player LoadPlayerDefault( const std::vector<Ability>& abilities );
void SaveGameConfig( const DungeonConfiguration& config );
void SaveGameDungeons( const std::vector<Dungeon>& dungeons, int index );
DungeonConfiguration LoadGameConfig( );
std::vector<Dungeon> LoadGameDungeons( PlayerHandle& player, const EntityFactory& entityFactory, int& index );
std::vector<Vector2<int>> BresenhamCircle( const Vector2<int>& center, int radius );
std::vector<Vector2<int>> BresenhamLine( const Vector2<int>& start, const Vector2<int>& end );
bool OnBorder( const Vector2<int>& position, const Vector2<int>& size, const Vector2<int>& origo = { 0, 0 }, int layerFrom = 0, int layerTo = 0 );
bool InCorner( const Vector2<int>& position, const Vector2<int>& size, int sensitivity = 0 );
bool InBounds( const Vector2<int>& position, const Vector2<int>& size );
Orientation::Enum Quadrant( const Vector2<int>& position, const Vector2<int>& size );

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