#pragma once

#include "Vector2i.h"
#include "Enums.h"
#include "Dungeon.h"
#include "BattleSystem.h"
#include <vector>
#include <string>
#include <iostream>

int RandomNumberGenerator( int min, int max );
int GetPositiveInteger( const std::string& context );
char GetValidChar( const std::string& context, const std::vector<char>& validChoices );
void GetEnter( );
void PrintDungeonCentered( const Dungeon& dungeon, int visionReach, const Vector2i& center, const Vector2i& screenSize = { 40, 20 } );
void PrintCombatantInformation( const Combatant& combatant );
Vector2i PositionRotateClockwise( const Vector2i& position, int maxCol );
Vector2i PositionMove( const Vector2i& position, const Orientation& orientation );
Vector2i PositionMoveProbability( const Vector2i& position, int north, int south, int west, int east, int still );