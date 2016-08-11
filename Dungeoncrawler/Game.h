#pragma once

#include "Dungeon.h"
#include "EntityLibrary.h"

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
        DungeonSystem _dungeonSystem;
        EntityLibrary _entityLibrary;
        Player _player;

        void SetDungeonConfiguration( const GameConfig& type );
        void Reset( );
        void Start( );

        void TurnPlayer( Dungeon& dungeon );
        void DungeonLink( int indexDungeonCurrent );
        void DungeonRotate( int indexDungeon, const Orientation::OrientationType& orientation );
        void DungeonSwitch( );
};