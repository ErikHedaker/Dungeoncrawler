#pragma once

#include "Dungeon.h"
#include "Templates.h"
#include <array>
#include <memory>
#include <map>
#include <unordered_map>

class Game
{
	public:
		Game( );

		bool existingGame;

		void SetDungeonConfiguration( );
		void NewGame( );
		void GameLoop( );

	private:
		std::list<Dungeon> _dungeons;
		Graph<Dungeon*, Vector2i> _dungeonGraph;
		std::size_t _indexNodeCurrent;
		DungeonConfiguration _config;
		GameStatus _status;
		std::unique_ptr<Player> _player;

		void FullLinkDungeon( std::size_t indexNodeParent );
		void PlayerTurn( Dungeon& dungeon );
};