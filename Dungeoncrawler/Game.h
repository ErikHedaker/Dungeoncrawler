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
        PlayerType _player;
        GameStatus _status;
        DungeonConfiguration _config;
        std::vector<Dungeon> _dungeons;
        int _index;

        void Reset( );
        void Start( );
        void TurnPlayer( Dungeon& dungeon );
        void DungeonLink( int indexDungeonCurrent );
        void DungeonRotate( int indexDungeon, const Orientation::Enum& orientation );
        void DungeonSwitch( );
};