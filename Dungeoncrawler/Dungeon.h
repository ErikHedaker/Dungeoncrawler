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
    std::vector<std::unique_ptr<Entity>*> occupants;
    bool visible = false;
    char icon = '-';
};

class Dungeon
{
    public:
        Dungeon( PlayerPair& player, const EntityFactory& entityFactory, const DungeonConfiguration& config );
        Dungeon( PlayerPair& player, const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<bool>& vision, const std::vector<char>& icons );

        std::vector<Link> links;

        void Rotate( const Orientation::Enum& orientation );
        void PlayerPlace( const Vector2<int>& position );
        void MovementPlayer( const Orientation::Enum& orientation );
        void MovementRandom( );
        void NextTurn( );

        const Vector2<int>& GetSize( ) const;
        Orientation::Enum GetQuadrant( Vector2<int> position ) const;
        const Tile& GetTile( const Vector2<int>& position ) const;
        bool CheckTile( const Vector2<int>& position, int bitmask ) const;
        bool Surrounded( const Vector2<int>& position, int threshold ) const;
        bool Unoccupied( const Vector2<int>& position ) const;
        bool InBounds( const Vector2<int>& position ) const;
        bool IsCorner( const Vector2<int>& position ) const;

    private:
        Vector2<int> _size;
        std::vector<Tile> _tiles;
        std::list<std::unique_ptr<Entity>> _entities;
        PlayerPair& _player;

        void UpdateVision( const Vector2<int>& position, int visionReach );
        void UpdateTile( const Vector2<int>& position );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::pair<EntityType::Enum, int>& id );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::string& name );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, char icon );
        void EntityRemove( std::unique_ptr<Entity>& entity, const Vector2<int>& position );
        void OccupantAdd( std::unique_ptr<Entity>& entity, const Vector2<int>& position );
        void OccupantRemove( std::unique_ptr<Entity>& entity, const Vector2<int>& position );

        void GenerateDoors( const EntityFactory& entityFactory, int amount );
        void GenerateOuterWalls( const EntityFactory& entityFactory );
        void GenerateHiddenPath( const EntityFactory& entityFactory );
        void GenerateSourceWalls( const EntityFactory& entityFactory, int amount );
        void GenerateExtensionWalls( const EntityFactory& entityFactory, int amount );
        void GenerateFillerWalls( const EntityFactory& entityFactory, int amount );
        void GenerateMonsters( const EntityFactory& entityFactory, int amount );
};