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
    Vector2<int> exit;
    Vector2<int> entry;
};

struct Tile
{
    std::vector<std::unique_ptr<Entity>*> occupants = { };
    bool visible = false;
    char icon = '-';
};

class Dungeon
{
    public:
        Dungeon( PlayerType& player, const EntityFactory& entityFactory, const DungeonConfiguration& config );
        Dungeon( PlayerType& player, const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<bool>& vision, const std::vector<char>& icons );

        std::vector<Link> links;

        void Rotate( const Orientation::Enum& orientation );
        void PlayerPlace( const Vector2<int>& position );
        void MovementPlayer( const Orientation::Enum& orientation );
        void MovementRandom( );
        void Events( );

        const Vector2<int>& GetSize( ) const;
        const Tile& GetTile( const Vector2<int>& position ) const;
        Orientation::Enum GetQuadrant( Vector2<int> position ) const;
        bool InBounds( const Vector2<int>& position ) const;
        bool IsCorner( const Vector2<int>& position ) const;
        bool Unoccupied( const Vector2<int>& position ) const;
        bool Surrounded( const Vector2<int>& position, int threshold ) const;
        bool CheckTile( const Vector2<int>& position, int bitmask ) const;

    private:
        Vector2<int> _size;
        std::vector<Tile> _tiles;
        std::list<std::unique_ptr<Entity>> _entities;
        PlayerType& _player;

        void UpdateVision( const Vector2<int>& position, int visionReach );
        void UpdateTile( const Vector2<int>& position );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::pair<EntityType::Enum, int>& id );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::string& name );
        void EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, char icon );
        void EntityRemove( std::unique_ptr<Entity>& entity, const Vector2<int>& position );
        void OccupantAdd( std::unique_ptr<Entity>& entity, const Vector2<int>& position );
        void OccupantRemove( std::unique_ptr<Entity>& entity, const Vector2<int>& position );

        void GenerateDoors( const EntityFactory& entityFactory, int amount );
        void GenerateWallsOuter( const EntityFactory& entityFactory );
        void GenerateHiddenPath( const EntityFactory& entityFactory );
        void GenerateWallsParents( const EntityFactory& entityFactory, int amount );
        void GenerateWallsChildren( const EntityFactory& entityFactory, int amount );
        void GenerateWallsFiller( const EntityFactory& entityFactory, int amount );
        void GenerateMonsters( const EntityFactory& entityFactory, int amount );
};