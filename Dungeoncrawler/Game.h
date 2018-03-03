#pragma once

#include "Enums.h"
#include "BattleSystem.h"
#include "EntityFactory.h"
#include "Dungeon.h"

class Game
{
    public:
        Game( );

        bool Exist( ) const;
        void Menu( );

    private:
        const BattleSystem _battleSystem;
        const EntityFactory _entityFactory;
        std::vector<Dungeon> _dungeons;
        DungeonConfiguration _config;
        PlayerHandle _player;
        int _index;

        bool TurnPlayer( );
        void Reset( );
        void Start( );
        void DungeonSwap( const Connector& connector );
        void DungeonLink( int indexDungeonCurrent );
        void Save( );
        void Load( );
};