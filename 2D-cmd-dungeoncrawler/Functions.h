#pragma once

#include "Vector2i.h"
#include "Enums.h"
#include <algorithm>
#include <vector>

int RandomNumberGenerator( int min, int max );
int GetPositiveInteger( const std::string& context );
char GetValidChar( const std::string& context, const std::vector<char>& validChoices );
void GetEnter( );
void AsciiArtSpider( );
void AsciiArtSwords( );
void Battle( int* attackerHealth, int* attackerDamage, int* defenderHealth, int* defenderDamage );
Vector2i PositionRotateClockwise( const Vector2i& position, int maxCol );
Vector2i MoveEntity( const Vector2i& position, const Orientation& orientation );
Vector2i MoveEntityProbability( const Vector2i& position, int north, int south, int west, int east, int still );