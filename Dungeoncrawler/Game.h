#pragma once

#include "Dungeon.h"
#include "Player.h"
#include "EntityLibrary.h"

class Game
{
    public:
        Game( );

        /* Called in main */
        void Menu( );

        /* Helper functions */
        bool Exist( ) const;

    private:
        Player _player;
        GameStatus _status;
        EntityLibrary _entityLibrary;
        DungeonConfiguration _config;
        std::vector<Dungeon> _dungeons;
        int _indexCurrent;

        /* Called in Menu */
        void SetDungeonConfiguration( const GameConfig& type );
        void Reset( );
        void Start( );

        /* File IO */
        void SaveDungeons( );
        bool LoadDungeons( );
        Player LoadPlayer( );

        /* Various functions */
        void TurnPlayer( Dungeon& dungeon );
        void UpdatePlayerStates( );
        void SwitchDungeon( );
        void FullLinkDungeon( int indexDungeonCurrent );
        void LinksRotateClockwise( int indexDungeon );
};