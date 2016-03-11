#include "Dungeon.h"
#include "Functions.h"
#include "AStarAlgorithm.h"
#include <math.h>
#include <algorithm>

std::size_t Components::Add( )
{
    indexCount++;
    icon.emplace_back( );
    position.emplace_back( );
    positionPrevious.emplace_back( );
    attributes.emplace_back( );

    return indexCount - 1;
}
void Components::Delete( std::size_t index )
{
    const std::size_t indexLast = indexCount - 1;

    std::swap( icon[index], icon[indexLast] );
    std::swap( position[index], position[indexLast] );
    std::swap( positionPrevious[index], positionPrevious[indexLast] );
    std::swap( attributes[index], attributes[indexLast] );

    indexCount--;
    icon.pop_back( );
    position.pop_back( );
    positionPrevious.pop_back( );
    attributes.pop_back( );
}

Dungeon::Dungeon( const DungeonConfiguration& config ) :
    _size
    ( {
        config.sizeDungeonFixed ? config.sizeDungeon.x : RandomNumberGenerator( 50, 60 ),
        config.sizeDungeonFixed ? config.sizeDungeon.y : RandomNumberGenerator( 50, 60 )
    } ),
    _tileMap( _size.x * _size.y ),
    _visionMap( _size.x * _size.y, false )

{
    GenerateDoors( config.generateDoors, config.amountDoors );
    GenerateOuterWalls( config.generateOuterWalls );
    GeneratePath( config.generatePath );
    GenerateSourceWalls( config.generateSourceWalls, config.amountSourceWalls );
    GenerateExtensionWalls( config.generateExtensionWalls, config.amountExtensionWalls );
    GenerateFillerWalls( config.generateFillerWalls, config.amountFillerWallsCycles );
    GenerateMonsters( config.generateMonsters, config.amountMonsters );
}
Dungeon::Dungeon( const Vector2<int>& size, const std::vector<bool>& visionMap, const std::vector<char>& iconMap, Player& player ) :
    _size( size ),
    _tileMap( size.x * size.y ),
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
                    CreatePlayerLocal( iterator, player );

                    break;
                }
                case Icon::Monster:
                {
                    MonsterAdd( iterator );

                    break;
                }
                case Icon::Door:
                {
                    DoorAdd( iterator );

                    break;
                }
                case Icon::Wall:
                {
                    WallAdd( iterator );

                    break;
                }
            }
        }
    }
}

void Dungeon::CreatePlayerLocal( const Vector2<int>& position, Player& player )
{
    _indexPlayerLocal = _components.Add( );
    _components.position[_indexPlayerLocal] = _size / 2;
    _components.positionPrevious[_indexPlayerLocal] = _size / 2;
    _components.attributes[_indexPlayerLocal] = Attributes::PassableOthers | Attributes::PassablePlayer;
    _components.icon[_indexPlayerLocal] = Icon::Player;

    if( CheckTile( position, Attributes::PassableOthers ) )
    {
        _components.position[_indexPlayerLocal] = position;
        _components.positionPrevious[_indexPlayerLocal] = position;
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
                _components.position[_indexPlayerLocal] = nearby;
                _components.positionPrevious[_indexPlayerLocal] = nearby;

                break;
            }
        }
    }

    player.position = _components.position[_indexPlayerLocal];
    UpdateVision( player.position, player.visionReach );
    OccupantAdd( _indexPlayerLocal );
}
void Dungeon::RotateClockwise( )
{
    auto tileRotated = _tileMap;
    auto visionRotated = _visionMap;
    Vector2<int> iterator;

    std::swap( _size.x, _size.y );

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            tileRotated[( iterator.y * _size.x ) + iterator.x] = _tileMap[( iterator.x * _size.y ) + iterator.y];
            visionRotated[( iterator.y * _size.x ) + iterator.x] = _visionMap[( iterator.x * _size.y ) + iterator.y];
        }

        auto& tileColoumBegin   = tileRotated.begin( )   + iterator.y * _size.x;
        auto& tileColoumEnd     = tileRotated.begin( )   + iterator.y * _size.x + _size.x;
        auto& visionColoumBegin = visionRotated.begin( ) + iterator.y * _size.x;
        auto& visionColoumEnd   = visionRotated.begin( ) + iterator.y * _size.x + _size.x;

        std::reverse( tileColoumBegin, tileColoumEnd );
        std::reverse( visionColoumBegin, visionColoumEnd );
    }

    _tileMap = tileRotated;
    _visionMap = visionRotated;

    for( std::size_t index = 0; index < _components.indexCount; index++ )
    {
        _components.position[index] = PositionRotateClockwise( _components.position[index], _size.x );
        _components.positionPrevious[index] = PositionRotateClockwise( _components.position[index], _size.x );
    }
}

void Dungeon::PlayerMovement( const Orientation& orientation )
{
    OccupantRemove( _indexPlayerLocal );
    _components.positionPrevious[_indexPlayerLocal] = _components.position[_indexPlayerLocal];
    _components.position[_indexPlayerLocal] = PositionMove( _components.position[_indexPlayerLocal], orientation );
    OccupantAdd( _indexPlayerLocal );
}
void Dungeon::RandomMovement( )
{
    for( std::size_t index = 0; index < _components.indexCount; index++ )
    {
        if( _components.attributes[index] & Attributes::MovementRandom )
        {
            OccupantRemove( index );
            _components.positionPrevious[index] = _components.position[index];
            _components.position[index] = PositionMoveProbability( _components.position[index], 1, 1, 1, 1, 12 );
            OccupantAdd( index );
        }
    }
}
void Dungeon::CheckEvents( Player& player )
{
    std::vector<std::size_t> deleteEntites;

    /* Collision detection for player */
    if( !CheckTile( _components.position[_indexPlayerLocal], Attributes::PassablePlayer ) )
    {
        OccupantRemove( _indexPlayerLocal );
        std::swap( _components.position[_indexPlayerLocal], _components.positionPrevious[_indexPlayerLocal] );
        OccupantAdd( _indexPlayerLocal );
    }

    /* Collision detection for other entities */
    for( std::size_t index = 0; index < _components.indexCount; index++ )
    {
        if( _components.attributes[index] & Attributes::MovementRandom &&
            !CheckTile( _components.position[index], Attributes::PassableOthers ) )
        {
            OccupantRemove( index );
            std::swap( _components.position[index], _components.positionPrevious[index] );
            OccupantAdd( index );
        }
    }

    player.position = _components.position[_indexPlayerLocal];
    UpdateVision( player.position, player.visionReach );
    
    /* Engage aggressive entity on player's position */
    for( auto index : _tileMap[( _components.position[_indexPlayerLocal].y * _size.x ) + _components.position[_indexPlayerLocal].x].indexOccupants )
    {
        if( _components.attributes[index] & Attributes::Monster )
        {
            deleteEntites.push_back( index );
            player.status = PlayerStatus::Combat;
        }
    }

    /* Switch dungeon for player */
    for( const auto& link : links )
    {
        if( _components.position[_indexPlayerLocal] == link.entry )
        {
            deleteEntites.push_back( _indexPlayerLocal );
            player.status = PlayerStatus::Traveling;
        }
    }

    for( auto index : deleteEntites )
    {
        DeleteEntity( index );
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

    for( auto index : indexes )
    {
        if( _components.attributes[index] & bitmask )
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
    int surroundingEntity = 0;

    for( const auto& direction : directions )
    {
        const Vector2<int> neighbor = position + direction;

        if( !CheckTile( neighbor, Attributes::PassablePlayer ) )
        {
            surroundingEntity++;
        }
    }

    return surroundingEntity >= threshold;
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

    for( auto index : tile.indexOccupants )
    {
        tile.icon = _components.icon[index];
    }
}
void Dungeon::OccupantAdd( std::size_t index )
{
    const Vector2<int> position = _components.position[index];
    auto& indexes = _tileMap[( position.y * _size.x ) + position.x].indexOccupants;

    indexes.push_back( index );
    UpdateTile( position );
}
void Dungeon::OccupantRemove( std::size_t index )
{
    const Vector2<int> position = _components.position[index];
    auto& indexes = _tileMap[( position.y * _size.x ) + position.x].indexOccupants;

    indexes.erase( std::remove( indexes.begin( ), indexes.end( ), index ), indexes.end( ) );
    UpdateTile( position );
}
void Dungeon::DeleteEntity( std::size_t index )
{
    const std::size_t indexLast = _components.indexCount - 1;

    OccupantRemove( indexLast );
    OccupantRemove( index );
    _components.Delete( index );
    
    if( index != indexLast )
    {
        OccupantAdd( index );
    }

    if( _indexPlayerLocal == indexLast )
    {
        _indexPlayerLocal = index;
    }
}

void Dungeon::DoorAdd( const Vector2<int>& position )
{
    const std::size_t index = _components.Add( );

    _components.icon[index] = Icon::Door;
    _components.position[index] = position;
    _components.positionPrevious[index] = position;
    _components.attributes[index] = Attributes::PassablePlayer;
    OccupantAdd( index );
}
void Dungeon::WallAdd( const Vector2<int>& position )
{
    const std::size_t index = _components.Add( );

    _components.icon[index] = Icon::Wall;
    _components.position[index] = position;
    _components.positionPrevious[index] = position;
    _components.attributes[index] = 0;
    OccupantAdd( index );
}
void Dungeon::StepAdd( const Vector2<int>& position )
{
    const std::size_t index = _components.Add( );
    
    _components.icon[index] = Icon::Ground;
    _components.position[index] = position;
    _components.positionPrevious[index] = position;
    _components.attributes[index] = Attributes::PassablePlayer |
                                    Attributes::PassableOthers;
    OccupantAdd( index );
}
void Dungeon::MonsterAdd( const Vector2<int>& position )
{
    const std::size_t index = _components.Add( );

    _components.icon[index] = Icon::Monster;
    _components.position[index] = position;
    _components.positionPrevious[index] = position;
    _components.attributes[index] = Attributes::PassablePlayer |
                                    Attributes::PassableOthers |
                                    Attributes::MovementRandom |
                                    Attributes::Monster;
    OccupantAdd( index );
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

            DoorAdd( valid[index] );
            links.push_back( { false, 0, { -1, -1 }, valid[index] } );
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
                        WallAdd( iterator );
                    }
                }
            }
        }
    }
}
void Dungeon::GeneratePath( bool generate )
{
    if( generate )
    {
        const Vector2<int> center = _size / 2;
        std::vector<Vector2<int>> obstacles;

        for( std::size_t index = 0; index < _components.indexCount; index++ )
        {
            if( !( _components.attributes[index] & Attributes::PassablePlayer ) )
            {
                obstacles.push_back( _components.position[index] );
            }
        }

        for( const auto& link : links )
        {
            std::vector<Vector2<int>> path = AStarAlgorithm( link.entry, center, _size, obstacles );

            for( const auto& position : path )
            {
                if( Unoccupied( position ) )
                {
                    StepAdd( position );
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
        int sourceWallsLeft = amount ? amount : ( _size.x * _size.y ) / 20;

        while( sourceWallsLeft > 0 )
        {
            Vector2<int> position;

            position.x = RandomNumberGenerator( 1, _size.x - 2 );
            position.y = RandomNumberGenerator( 1, _size.y - 2 );

            if( Unoccupied( position ) &&
                position != center )
            {
                WallAdd( position );
                sourceWallsLeft--;
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
        int extensionWallsLeft = amount ? amount : ( _size.x * _size.y ) / 5;

        while( extensionWallsLeft > 0 )
        {
            for( std::size_t index = 0; index < _components.indexCount; index++ )
            {
                if( !( _components.attributes[index] & Attributes::PassablePlayer ) &&
                    !( _components.attributes[index] & Attributes::PassableOthers ) )
                {
                    const int indexDirection = RandomNumberGenerator( 0, 3 );
                    const Vector2<int> position = _components.position[index] + directions[indexDirection];

                    if( InBounds( position ) &&
                        Unoccupied( position ) &&
                        position != center )
                    {
                        WallAdd( position );
                        extensionWallsLeft--;
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
                        WallAdd( iterator );
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
                    MonsterAdd( position );

                    break;
                }
            }
        }
    }
}