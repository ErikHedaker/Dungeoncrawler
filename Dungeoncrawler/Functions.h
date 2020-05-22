#pragma once

#include "Vector2.h"
#include "Enums.h"
#include <vector>
#include <string>
#include <optional>
#include <string_view>

class Dungeon;
struct Power;
struct DungeonConfiguration;
struct Effect;
struct Spell;
struct Health;
struct Character;
struct Player;

/*
class StringWrapper
{
    public:
        friend std::istream& operator>>( std::istream& stream, StringWrapper& line )
        {
            while( std::getline( stream, line._data ) )
            {
                if( !line._data.empty( ) && line._data[0] == ':' )
                {
                    line._data = line._data.substr( 1, line._data.find( '#' ) );

                    break;
                }
            }

            return stream;
        }

        operator std::string( ) const
        {
            return _data;
        }

    private:
        std::string _data;
};
*/

int GetRNG( int min, int max );
int GetPowerDiceRoll( const Power& power );
int GetBitmask( const std::string& line );
std::optional<Power> GetOptionalPower( const std::string& line );
std::string GetStringPower( const Power& power );
std::string GetStringHealth( const Health& health );
std::string GetStringEffects( const std::vector<std::reference_wrapper<const Effect>>& effects );
std::string GetStringDungeon( const Dungeon& dungeon, const Vector2<int>& center, const Vector2<int>& sizeScreen );
Vector2<int> PositionRotate( const Vector2<int>& position, const Vector2<int>& size, Orientation rotation );
Vector2<int> PositionMove( const Vector2<int>& position, Orientation orientation );
Vector2<int> PositionMoveProbability( const Vector2<int>& position, int north, int west, int south, int east, int still );
std::vector<Vector2<int>> BresenhamCircle( const Vector2<int>& center, int radius );
std::vector<Vector2<int>> BresenhamLine( const Vector2<int>& start, const Vector2<int>& end );
bool OnBorder( const Vector2<int>& position, const Vector2<int>& size, const Vector2<int>& origo = { 0, 0 }, int minLayer = 0, int maxLayer = 0 );
bool InCorner( const Vector2<int>& position, const Vector2<int>& size, int sensitivity = 0 );
bool InBounds( const Vector2<int>& position, const Vector2<int>& size );
Orientation RectQuadrant( const Vector2<int>& position, const Vector2<int>& size );
Orientation RectQuadrantArithmetic( int orientation );
int RectQuadrantArithmetic( Orientation orientation );
DungeonConfiguration SelectDungeonConfiguration( );
int SelectPositiveInteger( );
char SelectChar( const std::vector<char>& valid, std::function<int( int )> modifier = nullptr );
void SelectEnter( );
void ClearScreen( bool clear );
std::vector<Character> LoadCharacters( );
std::vector<Effect> LoadEffects( );
std::vector<Spell> LoadSpells( );
Player LoadPlayerDefault( );