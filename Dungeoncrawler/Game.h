#pragma once

#include "Dungeon.h"
#include "EntityFactory.h"

class Game
{
    public:
        Game( );

        void Menu( );
        bool Exist( ) const;

    private:
        const EntityFactory _entityFactory;
        std::vector<Dungeon> _dungeons;
        DungeonConfiguration _config;
        PlayerType _player;
        int _index;

        bool PlayerTurn( );
        void Reset( );
        void Start( );
        void DungeonSwap( );
        void DungeonLink( int indexDungeonCurrent );
        void DungeonRotate( int indexDungeon, const Orientation::Enum& orientation );
};