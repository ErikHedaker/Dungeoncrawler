#pragma once

#include "Dungeon.h"
#include "Player.h"
#include "BattleSystem.h"
#include <memory>

class Game
{
	public:
		Game( );

		void Menu( );
		bool Exist( ) const;
		void SetDungeonConfiguration( const GameConfig& type );
		void Reset( );
		void Loop( );
		void Save( );
		bool Load( );

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