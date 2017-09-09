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
        DungeonConfiguration _config;
        std::vector<Dungeon> _dungeons;
        PlayerType _player;
        bool _playing;
        int _index;

        void Reset( );
        void Start( );
        void NextTurn( Dungeon& dungeon );
        void DungeonLink( int indexDungeonCurrent );
        void DungeonRotate( int indexDungeon, const Orientation::Enum& orientation );
        void DungeonSwap( );
};