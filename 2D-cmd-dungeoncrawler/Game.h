#pragma once

#include "Dungeon.h"
#include "Player.h"
#include "BattleSystem.h"
#include <memory>

class Game
{
	public:
		Game( );

		bool ExistingGame( ) const;
		void SetDungeonConfiguration( const GameConfig& type );
		void NewGame( );
		void GameLoop( );
		void SaveDungeons( );
		void LoadDungeons( );

	private:
		std::vector<Dungeon> _dungeons;
		std::size_t _indexCurrent;
		DungeonConfiguration _config;
		BattleSystem _battleSystem;
		Player _player;
		GameStatus _status;

		void PlayerTurn( Dungeon& dungeon );
		void CheckEventsPlayer( );
		void FullLinkDungeon( std::size_t indexDungeon );
		void SwitchDungeon( );
		void LinkExitsRotateClockwise( std::size_t index );
};