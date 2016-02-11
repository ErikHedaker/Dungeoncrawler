#pragma once

#include "Dungeon.h"
#include "Templates.h"
#include <memory>

class Game
{
	public:
		bool ExistingGame( ) const;
		void SetDungeonConfiguration( const ConfigType& type );
		void NewGame( );
		void GameLoop( );
		void SaveDungeons( );
		void LoadDungeons( );

	private:
		std::vector<Dungeon> _dungeons;

		//std::list<Dungeon> _dungeons;

		Graph<Dungeon*, Vector2i> _dungeonGraph;
		std::size_t _indexCurrent;
		DungeonConfiguration _config;
		GameStatus _status;

		void FullLinkDungeon( std::size_t indexDungeon );
		void SwitchDungeon( );
		void PlayerTurn( Dungeon& dungeon );
};