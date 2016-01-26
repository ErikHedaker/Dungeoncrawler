#pragma once

#include "Vector2i.h"
#include "Enums.h"
#include <algorithm>
#include <vector>

class Dungeon;
class Player;
class Character;

template<class T>
std::vector<T> TransposeArray1D( const std::vector<T>& arrayOld, const Vector2i& sizeOld, const Vector2i& sizeNew )
{
	auto arrayNew = arrayOld;
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < sizeNew.row; iterator.row++ )
	{
		for( iterator.col = 0; iterator.col < sizeNew.col; iterator.col++ )
		{
			arrayNew[( iterator.row * sizeNew.col ) + iterator.col] = arrayOld[( iterator.col * sizeOld.col ) + iterator.row];
		}
	}

	return arrayNew;
}

template<class T>
std::vector<T> ReverseColoumsArray1D( const std::vector<T>& arrayOld, const Vector2i& size )
{
	auto arrayNew = arrayOld;
	Vector2i iterator;

	for( iterator.row = 0; iterator.row < size.row; iterator.row++ )
	{
		auto coloumBegin = arrayNew.begin( ) + iterator.row * size.col;
		auto coloumEnd   = arrayNew.begin( ) + iterator.row * size.col + size.col;

		std::reverse( coloumBegin, coloumEnd );
	}

	return arrayNew;
}

int RandomNumberGenerator( int min, int max );

Vector2i RotatePositionClockwise( const Vector2i& position, int dungeonMaxCol );

void OutputClearScreen( );
void OutputDungeonCentered( const Dungeon& dungeon, const Vector2i& center, int visionReach );
void OutputDungeonFull( const Dungeon& dungeon, const Vector2i& dungeonSize );
void OutputDungeonFullHidden( const Dungeon& dungeon, const Vector2i& dungeonSize );
void OutputCharacterStatus( const Character& character );
void OutputTurnOptions( );
void OutputGameStatusEnd( const GameStatus& status );
void OutputBattleScreenStart( const Character& attacker, const Character& defender );
void OutputBattleScreenEnd( const Character& winner, const Character& loser );
void OutputAsciiArtSpider( );
void OutputAsciiArtSwords( );

void InputEnter( );
char InputValidChar( const std::string& context, const std::vector<char>& validChoices );
int InputPositiveInteger( const std::string& context );