#pragma once

#include "Vector2.h"
#include "Enums.h"
#include "Dungeon.h"
#include "BattleSystem.h"
#include <vector>
#include <string>
#include <iostream>

double RandomNumberGenerator( double min, double max );
int RandomNumberGenerator( int min, int max );
int GetPositiveInteger( const std::string& context );
char GetValidChar( const std::string& context, const std::vector<char>& validChoices );
void GetEnter( );
void PrintDungeonCentered( const Dungeon& dungeon, int visionReach, const Vector2<int>& center, const Vector2<int>& screenSize = { 40, 20 } );
void PrintCombatantInformation( const Combatant& combatant );
Vector2<int> PositionRotateClockwise( const Vector2<int>& position, int width );
Vector2<int> PositionMove( const Vector2<int>& position, const Orientation& orientation );
Vector2<int> PositionMoveProbability( const Vector2<int>& position, int north, int west, int south, int east, int still );