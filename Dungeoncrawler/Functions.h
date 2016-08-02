#pragma once

#include "Game.h"
#include "Vector2.h"
#include "Enums.h"
#include "Dungeon.h"
#include <vector>
#include <string>
#include <iostream>

double RandomNumberGenerator( double min, double max );
int RandomNumberGenerator( int min, int max );
int GetPositiveInteger( const std::string& context );
char GetValidChar( const std::string& context, const std::vector<char>& valid );
void GetEnter( );
void PrintDungeonCentered( const Dungeon& dungeon, int visionReach, const Vector2<int>& center, const Vector2<int>& sizeScreen = { 40, 20 } );
void PrintHealth( const Character& character );
Vector2<int> PositionMove( const Vector2<int>& position, const Orientation& orientation );
Vector2<int> PositionMoveProbability( const Vector2<int>& position, int north, int west, int south, int east, int still );
Vector2<int> PositionRotateClockwise( const Vector2<int>& position, const Vector2<int>& sizeGrid );
Ability GetAbility( const std::vector<Ability>& abilities );
std::string UseAbility( const Ability& ability, Character& caster, Character& target );
std::string UseWeapon( Character& attacker, Character& target );
std::string TurnPlayer( Character& player, Character& AI );
std::string TurnAI( Character& player, Character& AI );
void Combat( Character& player, Character& AI );
void SaveDungeonSystem( const DungeonSystem& dungeonSystem );
DungeonSystem LoadDungeonSystem( const EntityLibrary& entityLibrary );
Player LoadPlayer( const std::vector<Ability>& abilitiesLibrary, Load::LoadType load = Load::Default );
std::vector<Ability> LoadAbilities( );
std::vector<Character> LoadCharacters( const std::vector<Ability>& abilitiesLibrary );
std::vector<Structure> LoadStructures( );