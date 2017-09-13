#pragma once

#include "Enums.h"
#include "Vector2.h"
#include "EntityFactory.h"
#include <vector>
#include <utility>
#include <memory>
#include <list>
#include <unordered_set>

struct DungeonConfiguration
{
    DungeonConfiguration( );
    DungeonConfiguration( const std::vector<std::string>& data );
    
    struct Size
    {
        bool determined;
        Vector2<int> dungeon;
    } size;

    struct Generate
    {
        bool doors;
        bool wallsOuter;
        bool hiddenPath;
        bool wallsParents;
        bool wallsChildren;
        bool wallsFiller;
        bool enemies;
    } generate;

    struct Amount
    {
        int doors;
        int wallsParents;
        int wallsChildren;
        int wallsFillerCycles;
        int enemies;
    } amount;
};

struct Link
{
    int indexDungeon;
    int indexLink;
    Vector2<int> entrance;
    Vector2<int> exit;
};

struct Tile
{
    std::vector<std::unique_ptr<Entity>*> occupants = { };
    char icon = '-';
};

class Dungeon
{
    public:
        Dungeon( PlayerType& player, const EntityFactory& entityFactory, const DungeonConfiguration& config );
        Dungeon( PlayerType& player, const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<char>& icons );

        std::unordered_set<Vector2<int>, HasherVector2<int>> vision;
        std::vector<Link> links;

        void Rotate( const Orientation::Enum& orientation );
        void PlayerPlace( const Vector2<int>& position );
        void MovementPlayer( const Orientation::Enum& orientation );
        void MovementRandom( );
        void Events( );

        const Vector2<int>& GetSize( ) const;
        const Tile& GetTile( const Vector2<int>& position ) const;
        bool Unoccupied( const Vector2<int>& position ) const;
        bool Surrounded( const Vector2<int>& position, int threshold ) const;
        bool TileLacking( const Vector2<int>& position, int bitmask ) const;

    private:
        Vector2<int> _size;
        std::vector<Tile> _tiles;
        std::list<std::unique_ptr<Entity>> _entities;
        PlayerType& _player;

        void LineOfSight( const std::vector<Vector2<int>>& line );
        void UpdateVision( const Vector2<int>& position, int visionReach );
        void UpdateTile( const Vector2<int>& position );
        void EntityAdd( const Vector2<int>& position, Entity* entity );
        void EntityRemove( const Vector2<int>& position, std::unique_ptr<Entity>& entity );
        void OccupantAdd( const Vector2<int>& position, std::unique_ptr<Entity>& entity );
        void OccupantRemove( const Vector2<int>& position, std::unique_ptr<Entity>& entity );

        void GenerateDoors( const EntityFactory& entityFactory, int amount );
        void GenerateWallsOuter( const EntityFactory& entityFactory );
        void GenerateHiddenPath( const EntityFactory& entityFactory );
        void GenerateWallsParents( const EntityFactory& entityFactory, int amount );
        void GenerateWallsChildren( const EntityFactory& entityFactory, int amount );
        void GenerateWallsFiller( const EntityFactory& entityFactory, int amount );
        void GenerateEnemies( const EntityFactory& entityFactory, int amount );
};