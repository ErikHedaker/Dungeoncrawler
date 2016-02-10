#pragma once

#include "Dungeon.h"
#include "Templates.h"
#include <memory>

class Game
{
	public:
		Game( );

		bool existingGame;

		void SetDungeonConfiguration( const ConfigType& type );
		void NewGame( );
		void GameLoop( );
		void SaveDungeons( );
		void LoadDungeons( );

	private:
		std::list<Dungeon> _dungeons;
		Graph<Dungeon*, Vector2i> _dungeonGraph;
		std::size_t _indexNodeCurrent;
		DungeonConfiguration _config;
		GameStatus _status;

		void FullLinkDungeon( std::size_t indexNodeParent );
		void SwitchDungeon( Dungeon* dungeon );
		void PlayerTurn( Dungeon& dungeon );
};