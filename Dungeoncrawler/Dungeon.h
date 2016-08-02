#pragma once

#include "Enums.h"
#include "Vector2.h"
#include "EntityLibrary.h"
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

struct Entity
{
    std::unique_ptr<BaseEntity> base;
    Category::CategoryType category;
    Vector2<int> position;
    Vector2<int> positionPrevious;
};

struct Tile
{
    std::vector<int> indexOccupants;
    char icon = '-';
};

class Dungeon
{
    public:
        Dungeon( const EntityLibrary& entityLibrary, const DungeonConfiguration& config );
        Dungeon( const EntityLibrary& entityLibrary, const Vector2<int>& size, const std::vector<bool>& visionMap, const std::vector<char>& iconMap );

        std::vector<Link> links;

        void PlayerAdd( const Vector2<int>& position );
        void RotateClockwise( );

        void MovementPlayer( const Orientation& orientation );
        void MovementRandom( );
        void Events( );

        const Vector2<int>& GetSize( ) const;
        const Vector2<int>& GetPlayerPosition( ) const;
        const Tile& GetTile( const Vector2<int>& position ) const;
        bool GetVision( const Vector2<int>& position ) const;

        bool CheckTile( const Vector2<int>& position, int bitmask ) const;
        bool InBounds( const Vector2<int>& position ) const;
        bool IsCorner( const Vector2<int>& position ) const;
        bool Unoccupied( const Vector2<int>& position ) const;
        bool Surrounded( const Vector2<int>& position, int threshold ) const;

    private:
        Vector2<int> _size;
        const EntityLibrary& _entityLibrary;
        std::vector<Entity> _entities;
        int _indexPlayer;

        /* 1D arrays interpreted as 2D arrays */
        std::vector<Tile> _tileMap;
        std::vector<bool> _visionMap;

        void UpdateVision( const Vector2<int>& position, int visionReach );
        void UpdateTile( const Vector2<int>& position );
        void EntityAdd( const Vector2<int>& position, Category::CategoryType category, int id );
        void EntityRemove( int index );
        void OccupantAdd( int index );
        void OccupantRemove( int index );

        void GenerateDoors( bool generate, int amount );
        void GenerateOuterWalls( bool generate );
        void GenerateHiddenPath( bool generate );
        void GenerateSourceWalls( bool generate, int amount );
        void GenerateExtensionWalls( bool generate, int amount );
        void GenerateFillerWalls( bool generate, int amount );
        void GenerateMonsters( bool generater, int amount );
};