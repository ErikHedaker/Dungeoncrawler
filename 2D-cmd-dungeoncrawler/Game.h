#pragma once

#include "Dungeon.h"
#include "DungeonConfiguration.h"
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
		void NewGame( const DungeonType& type );
		void GameLoop( );

	private:
		std::unique_ptr<Player> _player;
		std::map<Entity*, std::pair<Dungeon*, Entity*>> _links;
		std::list<Dungeon> _dungeons;
		DungeonConfiguration _config;
		Dungeon* _dungeonCurrent;
		GameStatus _status;
		DungeonType _type;

		bool DungeonFullyLinked( Dungeon* dungeon ) const;
		void AddAndLinkDungeon( Dungeon* dungeonParent, Entity* doorParent );
		void FullLinkDungeon( Dungeon* dungeon );

		void PlayerTurn( Dungeon& dungeon );
};