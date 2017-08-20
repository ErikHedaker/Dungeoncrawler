#pragma once

#include "Enums.h"
#include "Vector2.h"
#include "EntityFactory.h"
#include <vector>
#include <utility>
#include <memory>

struct DungeonConfiguration
{
    DungeonConfiguration( );
    DungeonConfiguration( bool a, Vector2<int> b, bool c, bool d, bool e, bool f, bool g, bool h, bool i, int j, int k, int l, int m, int n );

    bool sizeDungeonFixed;
    Vector2<int> sizeDungeon;

    bool generateDoors;
    bool generateOuterWalls;
    bool generateHiddenPath;
    bool generateSourceWalls;
    bool generateExtensionWalls;
    bool generateFillerWalls;
    bool generateMonsters;

    int amountDoors;
    int amountSourceWalls;
    int amountExtensionWalls;
    int amountFillerWallsCycles;
    int amountMonsters;
};

struct Link
{
    int indexDungeon;
    int indexLink;
    Vector2<int> exit;
    Vector2<int> entry;
};

struct Tile
{
    std::vector<int> indexOccupants;
    char icon = '-';
};

class Dungeon
{
    public:
        Dungeon( const EntityFactory& entityLibrary, const DungeonConfiguration& config );
        Dungeon( const EntityFactory& entityLibrary, const Vector2<int>& size, const std::vector<bool>& visionMap, const std::vector<char>& iconMap );

        std::vector<Link> links;

        void Rotate( const Orientation::Enum& orientation );
        void PlayerAdd( const EntityFactory& entityFactory, const Vector2<int>& position );

        void MovementPlayer( const Orientation::Enum& orientation );
        void MovementRandom( );
        void NextTurn( );

        const Vector2<int>& GetSize( ) const;
        const Vector2<int>& GetPlayerPosition( ) const;
        Orientation::Enum GetQuadrant( Vector2<int> position ) const;
        const Tile& GetTile( const Vector2<int>& position ) const;
        bool GetVision( const Vector2<int>& position ) const;
        bool CheckTile( const Vector2<int>& position, int bitmask ) const;
        bool InBounds( const Vector2<int>& position ) const;
        bool IsCorner( const Vector2<int>& position ) const;
        bool Unoccupied( const Vector2<int>& position ) const;
        bool Surrounded( const Vector2<int>& position, int threshold ) const;

    private:
        Vector2<int> _size;
        int _indexPlayer;
        std::vector<std::unique_ptr<Entity>> _entities;

        /* 1D arrays interpreted as 2D arrays */
        std::vector<Tile> _tileMap;
        std::vector<bool> _visionMap;

        void UpdateVision( const Vector2<int>& position, int visionReach );
        void UpdateTile( const Vector2<int>& position );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::pair<EntityType::Enum, int>& id );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::string& name );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, char icon );
        void EntityRemove( int index );
        void OccupantAdd( int index );
        void OccupantRemove( int index );

        void GenerateDoors( const EntityFactory& entityFactory, int amount );
        void GenerateOuterWalls( const EntityFactory& entityFactory );
        void GenerateHiddenPath( const EntityFactory& entityFactory );
        void GenerateSourceWalls( const EntityFactory& entityFactory, int amount );
        void GenerateExtensionWalls( const EntityFactory& entityFactory, int amount );
        void GenerateFillerWalls( const EntityFactory& entityFactory, int amount );
        void GenerateMonsters( const EntityFactory& entityFactory, int amount );
};