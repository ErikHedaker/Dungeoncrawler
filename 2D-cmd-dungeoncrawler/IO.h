#pragma once

#include "Dungeon.h"
#include "Enums.h"
#include <vector>

namespace Input
{
	void Enter( );
	char ValidChar( const std::string& context, const std::vector<char>& validChoices );
	int PositiveInteger( const std::string& context );
};

namespace Output
{
	void ClearScreen( );
	void GameTypes( );
	void String( const std::string& string );
	void DungeonCentered( const Dungeon& dungeon, const Vector2i& dungeonSize, const Vector2i& positionCenter );
	void DungeonFull( const Dungeon& dungeon, const Vector2i& dungeonSize );
	void DungeonFullHidden( const Dungeon& dungeon, const Vector2i& dungeonSize );
	void PlayerStatus( const Player& player );
	void TurnOptions( );
	void GameStatusEnd( const GameStatus& status );
	void BattleScreenStart( const Character& attacker, const Character& defender );
	void BattleScreenEnd( const Character& winner, const Character& loser );
	void AsciiArtSpider( );
	void AsciiArtSwords( );
};