#pragma once

#include "Enums.h"
#include "Player.h"
#include "Vector2.h"
#include <vector>
#include <utility>

struct DungeonConfiguration
{
    bool sizeDungeonFixed = false;
    Vector2<int> sizeDungeon = { 0, 0 };

    bool generateDoors = true;
    bool generateOuterWalls = true;
    bool generatePath = true;
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
    bool active;
    std::size_t indexDungeon;
    Vector2<int> exit;
    Vector2<int> entry;
};

struct Components
{
    std::size_t indexCount = 0;
    std::vector<char> icon;
    std::vector<Vector2<int>> position;
    std::vector<Vector2<int>> positionPrevious;
    std::vector<int> attributes;

    std::size_t Add( );
    void Delete( std::size_t index );
};

struct Tile
{
    std::vector<std::size_t> indexOccupants;
    char icon = Icon::Ground;
};

class Dungeon
{
    public:
        Dungeon( const DungeonConfiguration& config );
        Dungeon( const Vector2<int>& size, const std::vector<bool>& visionMap, const std::vector<char>& iconMap, Player& player );

        std::vector<Link> links;

        void CreatePlayerLocal( const Vector2<int>& position, Player& player );
        void RotateClockwise( );

        const Vector2<int>& GetSize( ) const;
        const Tile& GetTile( const Vector2<int>& position ) const;
        bool GetVision( const Vector2<int>& position ) const;

        /* Game loop */
        void PlayerMovement( const Orientation& orientation );
        void RandomMovement( );
        void CheckEvents( Player& player );

        /* Helper functions */
        bool CheckTile( const Vector2<int>& position, int bitmask ) const;
        bool InBounds( const Vector2<int>& position ) const;
        bool IsCorner( const Vector2<int>& position ) const;
        bool Unoccupied( const Vector2<int>& position ) const;
        bool Surrounded( const Vector2<int>& position, int threshold ) const;

    private:
        Vector2<int> _size;
        Components _components;
        std::size_t _indexPlayerLocal;

        /* 1D arrays interpreted as 2D arrays */
        std::vector<Tile> _tileMap;
        std::vector<bool> _visionMap;

        void UpdateVision( const Vector2<int>& position, int visionReach );
        void UpdateTile( const Vector2<int>& position );
        void OccupantAdd( std::size_t index );
        void OccupantRemove( std::size_t index );
        void DeleteEntity( std::size_t index );

        /* Preset entities */
        void DoorAdd( const Vector2<int>& position );
        void WallAdd( const Vector2<int>& position );
        void StepAdd( const Vector2<int>& position );
        void MonsterAdd( const Vector2<int>& position );

        /* Called in constructor */
        void GenerateDoors( bool generate, int amount );
        void GenerateOuterWalls( bool generate );
        void GeneratePath( bool generate );
        void GenerateSourceWalls( bool generate, int amount );
        void GenerateExtensionWalls( bool generate, int amount );
        void GenerateFillerWalls( bool generate, int amount );
        void GenerateMonsters( bool generater, int amount );
};