#pragma once

#include "Enums.h"
#include "BattleSystem.h"
#include "EntityFactory.h"
#include "Dungeon.h"

class Game
{
    public:
        Game( bool clear, bool save, bool exit, bool config );

        bool Exist( ) const;
        void Menu( );

    private:
        const bool _clearOutput;
        const bool _saveToFile;
        const bool _exitable;
        const bool _customConfig;
        const BattleSystem _battleSystem;
        const EntityFactory _entityFactory;
        std::vector<Dungeon> _dungeons;
        DungeonConfiguration _config;
        PlayerHandle _player;
        int _index;

        bool Turn( );
        void Reset( );
        void Start( );
        void DungeonAlign( const Connector& connector );
        void DungeonConnect( int index );
        void Save( );
        void Load( );
};