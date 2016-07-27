#pragma once

#include "Enums.h"
#include "Player.h"
#include "Vector2.h"
#include "EntityLibrary.h"
#include <vector>
#include <utility>

struct DungeonConfiguration
{
    bool sizeDungeonFixed = false;
    Vector2<int> sizeDungeon = { 0, 0 };

    bool generateDoors = true;
    bool generateOuterWalls = true;
    bool generateHiddenPath = true;
    bool generateSourceWalls = true;
    bool generateExtensionWalls = true;
    bool generateFillerWalls = true;
    bool generateMonsters = true;

    int amountDoors = 0;
    int amountSourceWalls = 0;
    int amountExtensionWalls = 0;
    int amountFillerWallsCycles = 0;
    int amountMonsters = 0;
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
    Vector2<int> position;
    Vector2<int> positionPrevious;
    Category::CategoryType category;
    int id;
};

struct Tile
{
    std::vector<int> indexOccupants;
    char icon = Icon::Ground;
};

class Dungeon
{
    public:
        Dungeon( const DungeonConfiguration& config, const EntityLibrary& entityLibrary );
        Dungeon( const Vector2<int>& size, const std::vector<bool>& visionMap, const std::vector<char>& iconMap, const EntityLibrary& entityLibrary, Player& player );

        std::vector<Link> links;

        void EntityPlayerAdd( const Vector2<int>& position, Player& player );
        void RotateClockwise( );

        void MovementPlayer( const Orientation& orientation );
        void MovementRandom( );
        void CheckEvents( Player& player );

        const Vector2<int>& GetSize( ) const;
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
        int _indexEntityPlayer;

        /* 1D arrays interpreted as 2D arrays */
        std::vector<Tile> _tileMap;
        std::vector<bool> _visionMap;

        void UpdateVision( const Vector2<int>& position, int visionReach );
        void UpdateTile( const Vector2<int>& position );
        void EntityAdd( const Vector2<int>& position, Category::CategoryType category, int id );
        void EntityRemove( int index );
        void OccupantAdd( int index );
        void OccupantRemove( int index );

        /* Called in constructor */
        void GenerateDoors( bool generate, int amount );
        void GenerateOuterWalls( bool generate );
        void GenerateHiddenPath( bool generate );
        void GenerateSourceWalls( bool generate, int amount );
        void GenerateExtensionWalls( bool generate, int amount );
        void GenerateFillerWalls( bool generate, int amount );
        void GenerateMonsters( bool generater, int amount );
};