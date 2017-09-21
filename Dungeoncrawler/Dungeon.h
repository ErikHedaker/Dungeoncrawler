#pragma once

#include "Vector2.h"
#include "Array2D.h"
#include "Enums.h"
#include "EntityFactory.h"
#include <vector>
#include <utility>
#include <memory>
#include <unordered_set>
#include <vector>

struct DungeonConfiguration
{
    DungeonConfiguration( );
    DungeonConfiguration( const std::vector<std::string>& data );
    
    struct
    {
        bool determined;
        Vector2<int> dungeon;
    } size;

    struct
    {
        bool doors;
        bool wallsOuter;
        bool hiddenPath;
        bool wallsParents;
        bool wallsChildren;
        bool wallsFiller;
        bool enemies;
    } generate;

    struct
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
    std::vector<Entity*> occupants = { };
    char icon = '-';
};

class Dungeon
{
    public:
        Dungeon( Dungeon&& dungeon ) = default;
        Dungeon( const Dungeon& dungeon ) = delete;
        Dungeon( PlayerHandle& player, const EntityFactory& entityFactory, const DungeonConfiguration& config );
        Dungeon( PlayerHandle& player, const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<char>& icons );

        std::vector<Link> links;

        void Rotate( const Orientation::Enum& orientation );
        void PlayerPlace( const Vector2<int>& position );
        void MovementPlayer( const Orientation::Enum& orientation );
        void MovementRandom( );
        void Events( );

        const Vector2<int>& GetSize( ) const;
        const Tile& GetTile( const Vector2<int>& position ) const;
        bool Visible( const Vector2<int>& position ) const;
        bool Unoccupied( const Vector2<int>& position ) const;
        bool Surrounded( const Vector2<int>& position, int threshold ) const;
        bool TileLacking( const Vector2<int>& position, int bitmask ) const;

    private:
        Vector2<int> _size;
        Array2D<Tile> _tiles;
        std::vector<std::unique_ptr<Entity>> _entities;
        std::unordered_set<Vector2<int>, HasherVector2<int>> _vision;
        PlayerHandle& _player;

        void UpdateTile( const Vector2<int>& position );
        void BuildVision( const Vector2<int>& position, int visionReach );
        void OccupantInsert( const Vector2<int>& position, Entity* entity );
        void OccupantRemove( const Vector2<int>& position, Entity* entity );
        void EntityInsert( const Vector2<int>& position, Entity* entity );
        void EntityRemove( int index );

        void GenerateDoors( const EntityFactory& entityFactory, int amount );
        void GenerateWallsOuter( const EntityFactory& entityFactory );
        void GenerateHiddenPath( const EntityFactory& entityFactory );
        void GenerateWallsParents( const EntityFactory& entityFactory, int amount );
        void GenerateWallsChildren( const EntityFactory& entityFactory, int amount );
        void GenerateWallsFiller( const EntityFactory& entityFactory, int amount );
        void GenerateEnemies( const EntityFactory& entityFactory, int amount );
};