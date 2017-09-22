#pragma once

#include "Game.h"
#include "Vector2.h"
#include "Enums.h"
#include "Dungeon.h"
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <chrono>
#include <deque>

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
std::vector<Character> LoadCharacters( );
std::vector<Structure> LoadStructures( );
Player LoadPlayerDefault( );
std::vector<Vector2<int>> BresenhamCircle( const Vector2<int>& center, int radius );
std::vector<Vector2<int>> BresenhamLine( const Vector2<int>& start, const Vector2<int>& end );
bool OnBorder( const Vector2<int>& position, const Vector2<int>& size, const Vector2<int>& origo = { 0, 0 }, int layerFrom = 0, int layerTo = 0 );
bool InCorner( const Vector2<int>& position, const Vector2<int>& size, int sensitivity = 0 );
bool InBounds( const Vector2<int>& position, const Vector2<int>& size );
Orientation::Enum RectQuadrant( const Vector2<int>& position, const Vector2<int>& size );

class FileString
{
    public:
        friend std::istream& operator>>( std::istream& stream, FileString& line )
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

class Stopwatch
{
    public:
        Stopwatch( ) :
            _current( 0 )
        { }

        void Start( )
        {
            _start = std::chrono::high_resolution_clock::now( );
        }
        void Stop( )
        {
            _stop = std::chrono::high_resolution_clock::now( );
            _current = std::chrono::duration_cast<std::chrono::microseconds>( _stop - _start );
            _average.push_back( _current );

            if( _average.size( ) > 100 )
            {
                _average.pop_front( );
            }
        }
        long long int Microseconds( ) const
        {
            return _current.count( );
        }
        long long int MicrosecondsAverage( ) const
        {
            long long int total = 0;

            for( const auto& value : _average )
            {
                total += value.count( );
            }

            return total / ( _average.size( ) ? _average.size( ) : 1 );
        }
        double FPS( )
        {
            return 1.0 / ( static_cast<double>( _current.count( ) ) / 1000000.0 );
        }

    private:
        std::chrono::microseconds _current;
        std::deque<std::chrono::microseconds> _average;
        std::chrono::time_point<std::chrono::steady_clock> _start;
        std::chrono::time_point<std::chrono::steady_clock> _stop;
};