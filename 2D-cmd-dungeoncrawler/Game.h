#pragma once

#include "Dungeon.h"
#include "Player.h"
#include "BattleSystem.h"

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
        BattleSystem _battleSystem;
        DungeonConfiguration _config;
        std::vector<Dungeon> _dungeons;
        std::size_t _indexCurrent;

        /* Called in Menu */
        void SetDungeonConfiguration( const GameConfig& type );
        void Reset( );
        void Loop( );

        /* File IO */
        void Save( );
        bool Load( );

        /* Various functions */
        void PlayerTurn( Dungeon& dungeon );
        void CheckEventsPlayer( );
        void SwitchDungeon( );
        void FullLinkDungeon( std::size_t indexDungeon );
        void LinksRotateClockwise( std::size_t indexDungeon );
};