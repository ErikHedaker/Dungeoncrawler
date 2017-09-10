#pragma once

#include "Enums.h"
#include "Vector2.h"
#include "EntityFactory.h"
#include <vector>
#include <utility>
#include <memory>
#include <list>

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
        bool monsters;
    } generate;

    struct Amount
    {
        int doors;
        int wallsParents;
        int wallsChildren;
        int wallsFillerCycles;
        int monsters;
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
    View::Enum vision = View::Shrouded;
    char icon = '-';
};

class Dungeon
{
    public:
        Dungeon( PlayerType& player, const EntityFactory& entityFactory, const DungeonConfiguration& config );
        Dungeon( PlayerType& player, const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<int>& view, const std::vector<char>& icons );

        std::vector<Link> links;

        void Rotate( const Orientation::Enum& orientation );
        void PlayerPlace( const Vector2<int>& position );
        void MovementPlayer( const Orientation::Enum& orientation );
        void MovementRandom( );
        void Events( );

        const Vector2<int>& GetSize( ) const;
        const Tile& GetTile( const Vector2<int>& position ) const;
        Orientation::Enum GetQuadrant( Vector2<int> position ) const;
        bool Exterior( const Vector2<int>& position, int layer = 0 ) const;
        bool InCorner( const Vector2<int>& position, int sensitivity = 0 ) const;
        bool InBounds( const Vector2<int>& position ) const;
        bool Unoccupied( const Vector2<int>& position ) const;
        bool Surrounded( const Vector2<int>& position, int threshold ) const;
        bool TileContains( const Vector2<int>& position, int bitmask ) const;

    private:
        Vector2<int> _size;
        std::vector<Tile> _tiles;
        std::list<std::unique_ptr<Entity>> _entities;
        PlayerType& _player;

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
        void GenerateMonsters( const EntityFactory& entityFactory, int amount );
};