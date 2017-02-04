#pragma once

#include "Dungeon.h"
#include "EntityFactory.h"

struct DungeonSystem
{
    DungeonConfiguration config;
    std::vector<Dungeon> dungeons;
    int indexCurrent;
};

class Game
{
    public:
        Game( );

        void Menu( );
        bool Exist( ) const;

    private:
        GameStatus _status;
        Player _player;
        EntityFactory _entityFactory;
        DungeonSystem _dungeonSystem;

        void Reset( );
        void Start( );
        void TurnPlayer( Dungeon& dungeon );
        void DungeonLink( int indexDungeonCurrent );
        void DungeonRotate( int indexDungeon, const Orientation::Enum& orientation );
        void DungeonSwitch( );
};