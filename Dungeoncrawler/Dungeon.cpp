#include "Dungeon.h"
#include "Functions.h"
#include "AStarAlgorithm.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <map>

DungeonConfiguration::DungeonConfiguration( ) :
    sizeDungeonFixed( false ),
    sizeDungeon( { 0, 0 } ),
    generateDoors( true ),
    generateOuterWalls( true ),
    generateHiddenPath( true ),
    generateSourceWalls( true ),
    generateExtensionWalls( true ),
    generateFillerWalls( true ),
    generateMonsters( true ),
    amountDoors( 0 ),
    amountSourceWalls( 0 ),
    amountExtensionWalls( 0 ),
    amountFillerWallsCycles( 0 ),
    amountMonsters( 0 )
{ }
DungeonConfiguration::DungeonConfiguration( bool a, Vector2<int> b, bool c, bool d, bool e, bool f, bool g, bool h, bool i, int j, int k, int l, int m, int n ) :
    sizeDungeonFixed( a ),
    sizeDungeon( b ),
    generateDoors( c ),
    generateOuterWalls( d ),
    generateHiddenPath( e ),
    generateSourceWalls( f ),
    generateExtensionWalls( g ),
    generateFillerWalls( h ),
    generateMonsters( i ),
    amountDoors( j ),
    amountSourceWalls( k ),
    amountExtensionWalls( l ),
    amountFillerWallsCycles( m ),
    amountMonsters( n )
{ }

Dungeon::Dungeon( const EntityFactory& entityFactory, const DungeonConfiguration& config ) :
    _size
    ( {
        config.sizeDungeonFixed ? config.sizeDungeon.x : RandomNumberGenerator( 30, 50 ),
        config.sizeDungeonFixed ? config.sizeDungeon.y : RandomNumberGenerator( 30, 50 )
    } ),
    _tiles( _size.x * _size.y ),
    _vision( _size.x * _size.y, false ),
    _indexPlayer( -1 )

{
    if( config.generateDoors ) GenerateDoors( entityFactory, config.amountDoors );
    if( config.generateOuterWalls ) GenerateOuterWalls( entityFactory );
    if( config.generateHiddenPath ) GenerateHiddenPath( entityFactory );
    if( config.generateSourceWalls ) GenerateSourceWalls( entityFactory, config.amountSourceWalls );
    if( config.generateExtensionWalls ) GenerateExtensionWalls( entityFactory, config.amountExtensionWalls );
    if( config.generateFillerWalls ) GenerateFillerWalls( entityFactory, config.amountFillerWallsCycles );
    if( config.generateMonsters ) GenerateMonsters( entityFactory, config.amountMonsters );
}
Dungeon::Dungeon( const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<bool>& vision, const std::vector<char>& icons ) :
    _size( size ),
    _tiles( size.x * size.y ),
    _vision( vision ),
    _indexPlayer( -1 )
{
    Vector2<int> iterator;
    std::pair<EntityType::Enum, int> entity;

    for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < size.x; iterator.x++ )
        {
            char icon = icons[( iterator.y * size.x ) + iterator.x];

            if( icon != '-' )
            {
                if( icon == '@' )
                {
                    PlayerAdd( entityFactory, iterator );
                }
                else
                {
                    EntityAdd( entityFactory, iterator, icon );
                }
            }
        }
    }
}

void Dungeon::Rotate( const Orientation::Enum& orientation )
{
    const Vector2<int> sizeOld = _size;

    for( int i = 0; i < orientation; i++ )
    {
        auto tilesRotated = _tiles;
        auto visionRotated = _vision;
        Vector2<int> iterator;

        std::swap( _size.x, _size.y );

        for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
        {
            for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
            {
                tilesRotated[( iterator.y * _size.x ) + iterator.x] = _tiles[( iterator.x * _size.y ) + iterator.y];
                visionRotated[( iterator.y * _size.x ) + iterator.x] = _vision[( iterator.x * _size.y ) + iterator.y];
            }

            auto& tileColoumBegin = tilesRotated.begin( ) + iterator.y * _size.x;
            auto& tileColoumEnd = tilesRotated.begin( ) + iterator.y * _size.x + _size.x;
            auto& visionColoumBegin = visionRotated.begin( ) + iterator.y * _size.x;
            auto& visionColoumEnd = visionRotated.begin( ) + iterator.y * _size.x + _size.x;

            std::reverse( tileColoumBegin, tileColoumEnd );
            std::reverse( visionColoumBegin, visionColoumEnd );
        }

        _tiles = tilesRotated;
        _vision = visionRotated;
    }

    for( auto& entity : _entities )
    {
        entity->position = PositionRotate( entity->position, sizeOld, orientation );
        entity->positionPrevious = PositionRotate( entity->positionPrevious, sizeOld, orientation );
    }
}
void Dungeon::PlayerAdd( const EntityFactory& entityFactory, const Vector2<int>& position )
{
    Vector2<int> positionPlayer = position;

    if( !CheckTile( position, Attributes::PassableOthers ) )
    {
        const std::array<Vector2<int>, 4> directions =
        { {
            {  0, -1 },
            {  1,  0 },
            {  0,  1 },
            { -1,  0 }
        } };

        for( const auto& direction : directions )
        {
            const Vector2<int> nearby = position + direction;

            if( InBounds( nearby ) &&
                CheckTile( nearby, Attributes::PassableOthers ) )
            {
                positionPlayer = nearby;

                break;
            }
        }
    }

    if( _indexPlayer >= 0 )
    {
        EntityRemove( _indexPlayer );
    }

    EntityAdd( entityFactory, positionPlayer, std::make_pair( EntityType::PlayerEntity, 0 ) );
    _indexPlayer = _entities.size( ) - 1;
    UpdateVision( _entities[_indexPlayer]->position, static_cast<PlayerLocal*>( _entities[_indexPlayer].get( ) )->player.visionReach );
}
void Dungeon::MovementPlayer( const Orientation::Enum& orientation )
{
    OccupantRemove( _indexPlayer );
    _entities[_indexPlayer]->positionPrevious = _entities[_indexPlayer]->position;
    _entities[_indexPlayer]->position = PositionMove( _entities[_indexPlayer]->position, orientation );
    OccupantAdd( _indexPlayer );
}
void Dungeon::MovementRandom( )
{
    for( unsigned int i = 0; i < _entities.size( ); i++ )
    {
        if( _entities[i]->attributes & Attributes::MovementRandom )
        {
            OccupantRemove( i );
            _entities[i]->positionPrevious = _entities[i]->position;
            _entities[i]->position = PositionMoveProbability( _entities[i]->position, 1, 1, 1, 1, 12 );
            OccupantAdd( i );
        }
    }
}
void Dungeon::NextTurn( )
{
    Player& player = static_cast<PlayerLocal*>( _entities[_indexPlayer].get( ) )->player;
    std::vector<int> indexesEntityRemove;

    /* Collision detection for player */
    if( !CheckTile( _entities[_indexPlayer]->position, Attributes::PassablePlayer ) )
    {
        OccupantRemove( _indexPlayer );
        std::swap( _entities[_indexPlayer]->position, _entities[_indexPlayer]->positionPrevious );
        OccupantAdd( _indexPlayer );
    }

    /* Collision detection for other entities */
    for( unsigned int i = 0; i < _entities.size( ); i++ )
    {
        if( _entities[i]->attributes & Attributes::MovementRandom &&
            !CheckTile( _entities[i]->position, Attributes::PassableOthers ) )
        {
            OccupantRemove( i );
            std::swap( _entities[i]->position, _entities[i]->positionPrevious );
            OccupantAdd( i );
        }
    }
    
    /* Engage combative entity on players position */
    for( auto i : _tiles[( _entities[_indexPlayer]->position.y * _size.x ) + _entities[_indexPlayer]->position.x].indexOccupants )
    {
        if( _entities[i]->attributes & Attributes::Combative )
        {
            Combat( player, *static_cast<Character*>( _entities[i].get( ) ) );

            if( player.health > 0 )
            {
                indexesEntityRemove.push_back( i );
            }
        }
    }

    /* Switch dungeon for player */
    for( const auto& link : links )
    {
        if( _entities[_indexPlayer]->position == link.entry )
        {
            player.states |= States::Switch;
            OccupantRemove( _indexPlayer );
        }
    }

    for( auto i : indexesEntityRemove )
    {
        EntityRemove( i );
    }

    UpdateVision( _entities[_indexPlayer]->position, player.visionReach );
}

const Vector2<int>& Dungeon::GetSize( ) const
{
    return _size;
}
const Vector2<int>& Dungeon::GetPlayerPosition( ) const
{
    return _entities[_indexPlayer]->position;
}
Orientation::Enum Dungeon::GetQuadrant( Vector2<int> position ) const
{
    const std::map<std::pair<bool, bool>, Orientation::Enum> quadrants
    {
        { { true,  false }, Orientation::North },
        { { true,  true  }, Orientation::East  },
        { { false, true  }, Orientation::South },
        { { false, false }, Orientation::West  }
    };
    const Vector2<float> positionf = position;
    const Vector2<float> sizef = _size;
    const Vector2<float> ratiof = sizef / sizef.y;
    const bool rightOfMainDiagonal = positionf.x > ( positionf.y * ratiof.x );
    const bool rightOfAntiDiagonal = positionf.x > ( sizef.x - positionf.y * ratiof.x - 1 );

    return quadrants.at( { rightOfMainDiagonal, rightOfAntiDiagonal } );
}
const Tile& Dungeon::GetTile( const Vector2<int>& position ) const
{
    return _tiles[( position.y * _size.x ) + position.x];
}
bool Dungeon::GetVision( const Vector2<int>& position ) const
{
    return _vision[( position.y * _size.x ) + position.x];
}
bool Dungeon::CheckTile( const Vector2<int>& position, int bitmask ) const
{
    const auto& indexes = GetTile( position ).indexOccupants;
    int count = 0;

    for( auto i : indexes )
    {
        if( _entities[i]->attributes & bitmask )
        {
            count++;
        }
    }

    return count == indexes.size( );
}
bool Dungeon::InBounds( const Vector2<int>& position ) const
{
    return
        position.x >= 0 &&
        position.y >= 0 &&
        position.x < _size.x &&
        position.y < _size.y;
}
bool Dungeon::IsCorner( const Vector2<int>& position ) const
{
    return
        ( position.x == 0 || position.x == _size.x - 1 ) &&
        ( position.y == 0 || position.y == _size.y - 1 );
}
bool Dungeon::Unoccupied( const Vector2<int>& position ) const
{
    return _tiles[( position.y * _size.x ) + position.x].indexOccupants.empty( );
}
bool Dungeon::Surrounded( const Vector2<int>& position, int threshold ) const
{
    static const std::array<Vector2<int>, 8> directions =
    { {
        {  0, -1 },
        {  1, -1 },
        {  1,  0 },
        {  1,  1 },
        {  0,  1 },
        { -1,  1 },
        { -1,  0 },
        { -1, -1 }
    } };
    int entities = 0;

    for( const auto& direction : directions )
    {
        const Vector2<int> neighbor = position + direction;

        if( !CheckTile( neighbor, Attributes::PassablePlayer ) )
        {
            entities++;
        }
    }

    return entities >= threshold;
}

void Dungeon::UpdateVision( const Vector2<int>& position, int visionReach )
{
    const Vector2<int> iteratorBegin = position - visionReach;
    const Vector2<int> iteratorEnd   = position + visionReach;
    Vector2<int> iterator;

    for( iterator.y = iteratorBegin.y; iterator.y <= iteratorEnd.y; iterator.y++ )
    {
        for( iterator.x = iteratorBegin.x; iterator.x <= iteratorEnd.x; iterator.x++ )
        {
            if( InBounds( iterator ) )
            {
                _vision[( iterator.y * _size.x ) + iterator.x] = true;
            }
        }
    }
}
void Dungeon::UpdateTile( const Vector2<int>& position )
{
    Tile& tile = _tiles[( position.y * _size.x ) + position.x];

    tile.icon = '-';

    for( auto i : tile.indexOccupants )
    {
        tile.icon = _entities[i]->icon;
    }
}
void Dungeon::EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::pair<EntityType::Enum, int>& id )
{
    const int index = _entities.size( );

    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( id )->Clone( ) ) );
    _entities[index]->position = position;
    _entities[index]->positionPrevious = position;
    OccupantAdd( index );
}
void Dungeon::EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::string& name )
{
    const int index = _entities.size( );

    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( name )->Clone( ) ) );
    _entities[index]->position = position;
    _entities[index]->positionPrevious = position;
    OccupantAdd( index );
}
void Dungeon::EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, char icon )
{
    const int index = _entities.size( );

    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( icon )->Clone( ) ) );
    _entities[index]->position = position;
    _entities[index]->positionPrevious = position;
    OccupantAdd( index );
}
void Dungeon::EntityRemove( int index )
{
    const int indexLast = _entities.size( ) - 1;

    OccupantRemove( indexLast );
    OccupantRemove( index );
    std::swap( _entities[index], _entities[indexLast] );
    _entities.pop_back( );

    if( index != indexLast )
    {
        OccupantAdd( index );
    }

    if( _indexPlayer == indexLast )
    {
        _indexPlayer = index;
    }
}
void Dungeon::OccupantAdd( int index )
{
    const Vector2<int> position = _entities[index]->position;
    std::vector<int>& indexes = _tiles[( position.y * _size.x ) + position.x].indexOccupants;

    indexes.push_back( index );
    UpdateTile( position );
}
void Dungeon::OccupantRemove( int index )
{
    const Vector2<int> position = _entities[index]->position;
    std::vector<int>& indexes = _tiles[( position.y * _size.x ) + position.x].indexOccupants;

    indexes.erase( std::remove( indexes.begin( ), indexes.end( ), index ), indexes.end( ) );
    UpdateTile( position );
}

void Dungeon::GenerateDoors( const EntityFactory& entityFactory, int amount )
{
    const int amountDoors = amount ? amount : 3;
    std::vector<Vector2<int>> valid;
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            if( iterator.x == 0 ||
                iterator.y == 0 ||
                iterator.x == _size.x - 1 ||
                iterator.y == _size.y - 1 )
            {
                if( !IsCorner( iterator ) )
                {
                    valid.push_back( iterator );
                }
            }
        }
    }

    for( int i = 0; i < amountDoors; i++ )
    {
        const int index = RandomNumberGenerator( 0, valid.size( ) - 1 );

        EntityAdd( entityFactory, valid[index], std::make_pair( EntityType::Structure, 3 ) );
        links.push_back( { -1, -1, { -1, -1 }, valid[index] } );
        valid.erase( valid.begin( ) + index );
    }
}
void Dungeon::GenerateOuterWalls( const EntityFactory& entityFactory )
{
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            if( Unoccupied( iterator ) )
            {
                if( iterator.x == 0 ||
                    iterator.y == 0 ||
                    iterator.x == _size.x - 1 ||
                    iterator.y == _size.y - 1 )
                {
                    EntityAdd( entityFactory, iterator, std::make_pair( EntityType::Structure, 2 ) );
                }
            }
        }
    }
}
void Dungeon::GenerateHiddenPath( const EntityFactory& entityFactory )
{
    const Vector2<int> center = _size / 2;
    std::vector<Vector2<int>> obstacles;

    for( unsigned int i = 0; i < _entities.size( ); i++ )
    {
        if( !( _entities[i]->attributes & Attributes::PassablePlayer ) )
        {
            obstacles.push_back( _entities[i]->position );
        }
    }

    for( const auto& link : links )
    {
        const std::vector<Vector2<int>> path = AStarAlgorithm( link.entry, center, _size, obstacles );

        for( const auto& position : path )
        {
            if( Unoccupied( position ) )
            {
                EntityAdd( entityFactory, position, std::make_pair( EntityType::Structure, 1 ) );
            }
        }
    }
}
void Dungeon::GenerateSourceWalls( const EntityFactory& entityFactory, int amount )
{
    const Vector2<int> center = _size / 2;
    int remaining = amount ? amount : ( _size.x * _size.y ) / 20;

    while( remaining > 0 )
    {
        Vector2<int> position;

        position.x = RandomNumberGenerator( 1, _size.x - 2 );
        position.y = RandomNumberGenerator( 1, _size.y - 2 );

        if( Unoccupied( position ) &&
            position != center )
        {
            EntityAdd( entityFactory, position, std::make_pair( EntityType::Structure, 2 ) );
            remaining--;
        }
    }
}
void Dungeon::GenerateExtensionWalls( const EntityFactory& entityFactory, int amount )
{
    const Vector2<int> center = _size / 2;
    const std::array<Vector2<int>, 4> directions =
    { {
        {  0, -1 },
        { -1,  0 },
        {  0,  1 },
        {  1,  0 }
    } };
    int remaining = amount ? amount : ( _size.x * _size.y ) / 5;

    while( remaining > 0 )
    {
        for( unsigned int i = 0; i < _entities.size( ); i++ )
        {
            if( !( _entities[i]->attributes & Attributes::PassablePlayer ) &&
                !( _entities[i]->attributes & Attributes::PassableOthers ) )
            {
                const int indexDirection = RandomNumberGenerator( 0, 3 );
                const Vector2<int> position = _entities[i]->position + directions[indexDirection];

                if( InBounds( position ) &&
                    Unoccupied( position ) &&
                    position != center )
                {
                    EntityAdd( entityFactory, position, std::make_pair( EntityType::Structure, 2 ) );
                    remaining--;
                }
            }
        }
        }
}
void Dungeon::GenerateFillerWalls( const EntityFactory& entityFactory, int amount )
{
    const Vector2<int> center = _size / 2;
    const int amountWalls = amount ? amount : 5;
    Vector2<int> iterator;

    for( int i = 0; i < amountWalls; i++ )
    {
        for( iterator.y = 1; iterator.y < _size.y - 1; iterator.y++ )
        {
            for( iterator.x = 1; iterator.x < _size.x - 1; iterator.x++ )
            {
                if( Unoccupied( iterator ) &&
                    Surrounded( iterator, 5 ) &&
                    iterator != center )
                {
                    EntityAdd( entityFactory, iterator, std::make_pair( EntityType::Structure, 2 ) );
                }
            }
        }
    }
}
void Dungeon::GenerateMonsters( const EntityFactory& entityFactory, int amount )
{
    const Vector2<int> center = _size / 2;
    const int min = static_cast<int>( sqrt( _size.x * _size.y ) / 3.0 );
    const int max = static_cast<int>( sqrt( _size.x * _size.y ) / 1.5 );
    const int amountMonsters = amount ? amount : RandomNumberGenerator( min, max );

    for( int i = 0; i < amountMonsters; i++ )
    {
        while( true )
        {
            Vector2<int> position;

            position.x = RandomNumberGenerator( 1, _size.x - 2 );
            position.y = RandomNumberGenerator( 1, _size.y - 2 );

            if( Unoccupied( position ) &&
                position != center )
            {
                std::vector<int> possibilities( { 0, 1, 2 } );

                EntityAdd( entityFactory, position, std::make_pair( EntityType::Character, possibilities[RandomNumberGenerator( 0, possibilities.size( ) - 1 )] ) );

                break;
            }
        }
    }
}