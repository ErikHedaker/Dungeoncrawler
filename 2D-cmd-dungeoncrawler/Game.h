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
		std::size_t _indexCurrent;
		DungeonConfiguration _config;
		GameStatus _status;

		void PlayerTurn( Dungeon& dungeon );
		void FullLinkDungeon( std::size_t indexDungeon );
		void SwitchDungeon( );
		void LinkExitsRotateClockwise( std::size_t index );
};