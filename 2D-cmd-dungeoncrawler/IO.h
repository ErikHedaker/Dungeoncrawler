#pragma once

#include "Dungeon.h"
#include "Enums.h"
#include <vector>

struct Input
{
	static void Enter( );
	static char ValidChar( const std::string& context, const std::vector<char>& validChoices );
	static int PositiveInteger( const std::string& context );
};

struct Output
{
	static void ClearScreen( );
	static void GameTypes( );
	static void String( const std::string& string );
	static void DungeonCentered( const Dungeon& dungeon, const Vector2i& dungeonSize, const Vector2i& positionCenter );
	static void DungeonFull( const Dungeon& dungeon, const Vector2i& dungeonSize );
	static void DungeonFullHidden( const Dungeon& dungeon, const Vector2i& dungeonSize );
	static void PlayerStatus( const Player& player );
	static void TurnOptions( );
	static void GameStatusEnd( const GameStatus& status );
	static void BattleScreenStart( const Character& attacker, const Character& defender );
	static void BattleScreenEnd( const Character& winner, const Character& loser );
	static void AsciiArtSpider( );
};