#pragma once

#include "Dungeon.h"
#include "EntityLibrary.h"

class Game
{
    public:
        Game( );

        void Menu( );

        bool Exist( ) const;

    private:
        EntityLibrary _entityLibrary;
        Player _player;
        GameStatus _status;
        DungeonConfiguration _config;
        std::vector<Dungeon> _dungeons;
        int _indexCurrent;

        void SetDungeonConfiguration( const GameConfig& type );
        void Reset( );
        void Start( );

        void SaveDungeons( );
        bool LoadDungeons( );
        Player LoadPlayer( Load::LoadType load = Load::Default );

        void TurnPlayer( Dungeon& dungeon );
        void DungeonSwitch( );
        void DungeonLink( int indexDungeonCurrent );
        void LinksRotateClockwise( int indexDungeon );
};