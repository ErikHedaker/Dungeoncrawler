#include "Dungeon.h"
#include "Functions.h"
#include "AStarAlgorithm.h"
#include <math.h>
#include <algorithm>
#include <random>

Dungeon::Dungeon( const DungeonConfiguration& config, const EntityLibrary& entityLibrary ) :
    _size
    ( {
        config.sizeDungeonFixed ? config.sizeDungeon.x : RandomNumberGenerator( 30, 50 ),
        config.sizeDungeonFixed ? config.sizeDungeon.y : RandomNumberGenerator( 30, 50 )
    } ),
    _entityLibrary( entityLibrary ),
    _tileMap( _size.x * _size.y ),
    _visionMap( _size.x * _size.y, false )

{
    GenerateDoors( config.generateDoors, config.amountDoors );
    GenerateOuterWalls( config.generateOuterWalls );
    GenerateHiddenPath( config.generateHiddenPath );
    GenerateSourceWalls( config.generateSourceWalls, config.amountSourceWalls );
    GenerateExtensionWalls( config.generateExtensionWalls, config.amountExtensionWalls );
    GenerateFillerWalls( config.generateFillerWalls, config.amountFillerWallsCycles );
    GenerateMonsters( config.generateMonsters, config.amountMonsters );
}
Dungeon::Dungeon( const Vector2<int>& size, const std::vector<bool>& visionMap, const std::vector<char>& iconMap, const EntityLibrary& entityLibrary, Player& player ) :
    _size( size ),
    _entityLibrary( entityLibrary ),
    _tileMap( _size.x * _size.y ),
    _visionMap( visionMap )
{
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            switch( iconMap[( iterator.y * _size.x ) + iterator.x] )
            {
                case Icon::Player:
                {
                    EntityPlayerAdd( iterator, player );

                    break;
                }
                case Icon::Monster:
                {
                    EntityAdd( iterator, Category::Character, 1 );

                    break;
                }
                case Icon::Door:
                {
                    EntityAdd( iterator, Category::Structure, 1 );

                    break;
                }
                case Icon::Wall:
                {
                    EntityAdd( iterator, Category::Character, 0 );

                    break;
                }
            }
        }
    }
}

void Dungeon::EntityPlayerAdd( const Vector2<int>& position, Player& player )
{
    EntityAdd( position, Category::Character, 0 );
    _indexEntityPlayer = _entities.size( ) - 1;

    if( CheckTile( position, Attributes::PassableOthers ) )
    {
        _entities[_indexEntityPlayer].position = position;
        _entities[_indexEntityPlayer].positionPrevious = position;
    }
    else
    {
        const std::array<Vector2<int>, 4> directions =
        { {
            {  0, -1 },
            { -1,  0 },
            {  0,  1 },
            {  1,  0 }
        } };

        for( const auto& direction : directions )
        {
            const Vector2<int> nearby = position + direction;

            if( InBounds( nearby ) &&
                CheckTile( nearby, Attributes::PassableOthers ) )
            {
                _entities[_indexEntityPlayer].position = nearby;
                _entities[_indexEntityPlayer].positionPrevious = nearby;

                break;
            }
        }
    }

    player.position = _entities[_indexEntityPlayer].position;
    UpdateVision( player.position, player.visionReach );
    OccupantAdd( _indexEntityPlayer );
}
void Dungeon::RotateClockwise( )
{
    auto tileMapRotated   = _tileMap;
    auto visionMapRotated = _visionMap;
    Vector2<int> iterator;

    std::swap( _size.x, _size.y );

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            tileMapRotated[  ( iterator.y * _size.x ) + iterator.x] = _tileMap[  ( iterator.x * _size.y ) + iterator.y];
            visionMapRotated[( iterator.y * _size.x ) + iterator.x] = _visionMap[( iterator.x * _size.y ) + iterator.y];
        }

        auto& tileColoumBegin   = tileMapRotated.begin( )   + iterator.y * _size.x;
        auto& tileColoumEnd     = tileMapRotated.begin( )   + iterator.y * _size.x + _size.x;
        auto& visionColoumBegin = visionMapRotated.begin( ) + iterator.y * _size.x;
        auto& visionColoumEnd   = visionMapRotated.begin( ) + iterator.y * _size.x + _size.x;

        std::reverse( tileColoumBegin, tileColoumEnd );
        std::reverse( visionColoumBegin, visionColoumEnd );
    }

    _tileMap   = tileMapRotated;
    _visionMap = visionMapRotated;

    for( auto& entity : _entities )
    {
        entity.position = PositionRotateClockwise( entity.position, _size );
        entity.positionPrevious = PositionRotateClockwise( entity.positionPrevious, _size );
    }
}

void Dungeon::MovementPlayer( const Orientation& orientation )
{
    OccupantRemove( _indexEntityPlayer );
    _entities[_indexEntityPlayer].positionPrevious = _entities[_indexEntityPlayer].position;
    _entities[_indexEntityPlayer].position = PositionMove( _entities[_indexEntityPlayer].position, orientation );
    OccupantAdd( _indexEntityPlayer );
}
void Dungeon::MovementRandom( )
{
    for( unsigned int i = 0; i < _entities.size( ); i++ )
    {
        if( _entityLibrary.GetAttribute( _entities[i].category, _entities[i].id ) & Attributes::MovementRandom )
        {
            OccupantRemove( i );
            _entities[i].positionPrevious = _entities[i].position;
            _entities[i].position = PositionMoveProbability( _entities[i].position, 1, 1, 1, 1, 12 );
            OccupantAdd( i );
        }
    }
}
void Dungeon::CheckEvents( Player& player )
{
    std::vector<int> indexesEntityRemove;

    /* Collision detection for player */
    if( !CheckTile( _entities[_indexEntityPlayer].position, Attributes::PassablePlayer ) )
    {
        OccupantRemove( _indexEntityPlayer );
        std::swap( _entities[_indexEntityPlayer].position, _entities[_indexEntityPlayer].positionPrevious );
        OccupantAdd( _indexEntityPlayer );
    }

    /* Collision detection for other entities */
    for( unsigned int i = 0; i < _entities.size( ); i++ )
    {
        if( _entityLibrary.GetAttribute( _entities[i].category, _entities[i].id ) & Attributes::MovementRandom &&
            !CheckTile( _entities[i].position, Attributes::PassableOthers ) )
        {
            OccupantRemove( i );
            std::swap( _entities[i].position, _entities[i].positionPrevious );
            OccupantAdd( i );
        }
    }

    player.position = _entities[_indexEntityPlayer].position;
    UpdateVision( player.position, player.visionReach );
    
    /* Engage aggressive entity on players position */
    for( auto i : _tileMap[( _entities[_indexEntityPlayer].position.y * _size.x ) + _entities[_indexEntityPlayer].position.x].indexOccupants )
    {
        if( _entityLibrary.GetAttribute( _entities[i].category, _entities[i].id ) & Attributes::Combative )
        {
            indexesEntityRemove.push_back( i );
            player.states |= States::Combat;
        }
    }

    /* Switch dungeon for player */
    for( const auto& link : links )
    {
        if( _entities[_indexEntityPlayer].position == link.entry )
        {
            indexesEntityRemove.push_back( _indexEntityPlayer );
            player.states |= States::Switch;
        }
    }

    for( auto i : indexesEntityRemove )
    {
        EntityRemove( i );
    }
}

const Vector2<int>& Dungeon::GetSize( ) const
{
    return _size;
}
const Tile& Dungeon::GetTile( const Vector2<int>& position ) const
{
    return _tileMap[( position.y * _size.x ) + position.x];
}
bool Dungeon::GetVision( const Vector2<int>& position ) const
{
    return _visionMap[( position.y * _size.x ) + position.x];
}

bool Dungeon::CheckTile( const Vector2<int>& position, int bitmask ) const
{
    const auto& indexes = GetTile( position ).indexOccupants;
    int count = 0;

    for( auto i : indexes )
    {
        if( _entityLibrary.GetAttribute( _entities[i].category, _entities[i].id ) & bitmask )
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
    return _tileMap[( position.y * _size.x ) + position.x].indexOccupants.empty( );
}
bool Dungeon::Surrounded( const Vector2<int>& position, int threshold ) const
{
    static const std::array<Vector2<int>, 8> directions =
    { {
        {  0, -1 },
        { -1, -1 },
        { -1,  0 },
        { -1,  1 },
        {  0,  1 },
        {  1,  1 },
        {  1,  0 },
        {  1, -1 }
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
                _visionMap[( iterator.y * _size.x ) + iterator.x] = true;
            }
        }
    }
}
void Dungeon::UpdateTile( const Vector2<int>& position )
{
    auto& tile = _tileMap[( position.y * _size.x ) + position.x];

    tile.icon = Icon::Ground;

    for( auto i : tile.indexOccupants )
    {
        tile.icon = _entityLibrary.GetIcon( _entities[i].category, _entities[i].id );
    }
}
void Dungeon::EntityAdd( const Vector2<int>& position, Category::CategoryType category, int id )
{
    _entities.push_back( { position, position, category, id } );
    OccupantAdd( _entities.size( ) - 1 );
}
void Dungeon::EntityRemove( int index )
{
    const int indexLast = _entities.size( ) - 1;

    OccupantRemove( indexLast );
    OccupantRemove( index );

    std::swap( _entities[index], _entities[_entities.size( ) - 1] );
    _entities.pop_back( );

    if( index != indexLast )
    {
        OccupantAdd( index );
    }

    if( _indexEntityPlayer == indexLast )
    {
        _indexEntityPlayer = index;
    }
}
void Dungeon::OccupantAdd( int index )
{
    const Vector2<int> position = _entities[index].position;
    auto& indexes = _tileMap[( position.y * _size.x ) + position.x].indexOccupants;

    indexes.push_back( index );
    UpdateTile( position );
}
void Dungeon::OccupantRemove( int index )
{
    const Vector2<int> position = _entities[index].position;
    auto& indexes = _tileMap[( position.y * _size.x ) + position.x].indexOccupants;

    indexes.erase( std::remove( indexes.begin( ), indexes.end( ), index ), indexes.end( ) );
    UpdateTile( position );
}

void Dungeon::GenerateDoors( bool generate, int amount )
{
    if( generate )
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

            EntityAdd( valid[index], Category::Structure, 3 );
            links.push_back( { -1, -1, { -1, -1 }, valid[index] } );
            valid.erase( valid.begin( ) + index );
        }
    }
}
void Dungeon::GenerateOuterWalls( bool generate )
{
    if( generate )
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
                        EntityAdd( iterator, Category::Structure, 2 );
                    }
                }
            }
        }
    }
}
void Dungeon::GenerateHiddenPath( bool generate )
{
    if( generate )
    {
        const Vector2<int> center = _size / 2;
        std::vector<Vector2<int>> obstacles;

        for( int i = 0; i < _entities.size( ); i++ )
        {
            if( !( _entityLibrary.GetAttribute( _entities[i].category, _entities[i].id ) & Attributes::PassablePlayer ) )
            {
                obstacles.push_back( _entities[i].position );
            }
        }

        for( const auto& link : links )
        {
            const std::vector<Vector2<int>> path = AStarAlgorithm( link.entry, center, _size, obstacles );

            for( const auto& position : path )
            {
                if( Unoccupied( position ) )
                {
                    EntityAdd( position, Category::Structure, 1 );
                }
            }
        }
    }
}
void Dungeon::GenerateSourceWalls( bool generate, int amount )
{
    if( generate )
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
                EntityAdd( position, Category::Structure, 2 );
                remaining--;
            }
        }
    }
}
void Dungeon::GenerateExtensionWalls( bool generate, int amount )
{
    if( generate )
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
            for( int i = 0; i < _entities.size( ); i++ )
            {
                if( !( _entityLibrary.GetAttribute( _entities[i].category, _entities[i].id ) & Attributes::PassablePlayer ) &&
                    !( _entityLibrary.GetAttribute( _entities[i].category, _entities[i].id ) & Attributes::PassableOthers ) )
                {
                    const int indexDirection = RandomNumberGenerator( 0, 3 );
                    const Vector2<int> position = _entities[i].position + directions[indexDirection];

                    if( InBounds( position ) &&
                        Unoccupied( position ) &&
                        position != center )
                    {
                        EntityAdd( position, Category::Structure, 2 );
                        remaining--;
                    }
                }
            }
        }
    }
}
void Dungeon::GenerateFillerWalls( bool generate, int amount )
{
    if( generate )
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
                        EntityAdd( iterator, Category::Structure, 2 );
                    }
                }
            }
        }
    }
}
void Dungeon::GenerateMonsters( bool generate, int amount )
{
    if( generate )
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
                    std::vector<int> possibilities( { 1, 2, 3 } );

                    EntityAdd( position, Category::Character, possibilities[RandomNumberGenerator( 0, possibilities.size( ) - 1 )] );

                    break;
                }
            }
        }
    }
}