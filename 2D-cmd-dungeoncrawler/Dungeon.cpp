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
    _maxCol( config.fixedDungeonSize ? config.maxCol : RandomNumberGenerator( 50, 60 ) ),
    _maxRow( config.fixedDungeonSize ? config.maxRow : RandomNumberGenerator( 50, 60 ) ),
    _tileMap( _maxCol * _maxRow ),
    _visionMap( _maxCol * _maxRow, false )

{
    GenerateDoors( config.generateDoors, config.amountDoors );
    GenerateOuterWalls( config.generateOuterWalls );
    GeneratePath( config.generatePath );
    GenerateSourceWalls( config.generateSourceWalls, config.amountSourceWalls );
    GenerateExtensionWalls( config.generateExtensionWalls, config.amountExtensionWalls );
    GenerateFillerWalls( config.generateFillerWalls, config.amountFillerWallsCycles );
    GenerateMonsters( config.generateMonsters, config.amountMonsters );
}
Dungeon::Dungeon( int maxCol, int maxRow, const std::vector<bool>& visionMap, const std::vector<char>& iconMap, Player& player ) :
    _maxCol( maxCol ),
    _maxRow( maxRow ),
    _tileMap( maxCol * maxRow ),
    _visionMap( visionMap )
{
    Vector2i iterator;

    for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
    {
        for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
        {
            switch( iconMap[( iterator.row * _maxCol ) + iterator.col] )
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

void Dungeon::CreatePlayerLocal( const Vector2i& position, Player& player )
{
    _indexPlayerLocal = _components.Add( );
    _components.position[_indexPlayerLocal] = { _maxCol / 2, _maxRow / 2 };
    _components.positionPrevious[_indexPlayerLocal] = { _maxCol / 2, _maxRow / 2 };
    _components.attributes[_indexPlayerLocal] = Attributes::PassableOthers | Attributes::PassablePlayer;
    _components.icon[_indexPlayerLocal] = Icon::Player;

    if( CheckTile( position, Attributes::PassableOthers ) )
    {
        _components.position[_indexPlayerLocal] = position;
        _components.positionPrevious[_indexPlayerLocal] = position;
    }
    else
    {
        const std::array<Vector2i, 4> directions =
        { {
            {  0, -1 },
            { -1,  0 },
            {  0,  1 },
            {  1,  0 }
        } };

        for( const auto& direction : directions )
        {
            Vector2i nearby = position + direction;

            if( InBounds( nearby ) &&
                CheckTile( nearby, Attributes::PassablePlayer ) )
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
    Vector2i iterator;

    std::swap( _maxCol, _maxRow );

    for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
    {
        for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
        {
            tileRotated[( iterator.row * _maxCol ) + iterator.col] = _tileMap[( iterator.col * _maxRow ) + iterator.row];
            visionRotated[( iterator.row * _maxCol ) + iterator.col] = _visionMap[( iterator.col * _maxRow ) + iterator.row];
        }

        auto& tileColoumBegin   = tileRotated.begin( )   + iterator.row * _maxCol;
        auto& tileColoumEnd     = tileRotated.begin( )   + iterator.row * _maxCol + _maxCol;
        auto& visionColoumBegin = visionRotated.begin( ) + iterator.row * _maxCol;
        auto& visionColoumEnd   = visionRotated.begin( ) + iterator.row * _maxCol + _maxCol;

        std::reverse( tileColoumBegin, tileColoumEnd );
        std::reverse( visionColoumBegin, visionColoumEnd );
    }

    _tileMap = tileRotated;
    _visionMap = visionRotated;

    for( std::size_t index = 0; index < _components.indexCount; index++ )
    {
        _components.position[index] = PositionRotateClockwise( _components.position[index], _maxCol );
        _components.positionPrevious[index] = PositionRotateClockwise( _components.position[index], _maxCol );
    }

    for( auto& link : links )
    {
        link.entry = PositionRotateClockwise( link.entry, _maxCol );
    }
}

const Tile& Dungeon::GetTile( const Vector2i& position ) const
{
    return _tileMap[( position.row * _maxCol ) + position.col];
}
bool Dungeon::GetVision( const Vector2i& position ) const
{
    return _visionMap[( position.row * _maxCol ) + position.col];
}
std::pair<int, int> Dungeon::GetSize( ) const
{
    return std::make_pair( _maxCol, _maxRow );
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
    
    /* Fight aggressive entities on player's position */
    for( auto index : _tileMap[( _components.position[_indexPlayerLocal].row * _maxCol ) + _components.position[_indexPlayerLocal].col].indexOccupants )
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

bool Dungeon::CheckTile( const Vector2i& position, int bit ) const
{
    const auto& indexes = GetTile( position ).indexOccupants;
    int count = 0;

    for( auto index : indexes )
    {
        if( _components.attributes[index] & bit )
        {
            count++;
        }
    }

    return count == indexes.size( );
}
bool Dungeon::InBounds( const Vector2i& position ) const
{
    return
        position.col >= 0 &&
        position.row >= 0 &&
        position.col < _maxCol &&
        position.row < _maxRow;
}
bool Dungeon::IsCorner( const Vector2i& position ) const
{
    return
        ( position.col == 0 || position.col == _maxCol - 1 ) &&
        ( position.row == 0 || position.row == _maxRow - 1 );
}
bool Dungeon::Unoccupied( const Vector2i& position ) const
{
    return _tileMap[( position.row * _maxCol ) + position.col].indexOccupants.empty( );
}
bool Dungeon::Surrounded( const Vector2i& position, int threshold ) const
{
    static const std::array<Vector2i, 8> directions =
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
        Vector2i neighbor = position + direction;

        if( !Unoccupied( neighbor ) )
        {
            surroundingEntity++;
        }
    }

    return surroundingEntity >= threshold;
}

void Dungeon::UpdateVision( const Vector2i& position, int visionReach )
{
    const Vector2i iteratorBegin = position - visionReach;
    const Vector2i iteratorEnd   = position + visionReach;
    Vector2i iterator;

    for( iterator.row = iteratorBegin.row; iterator.row <= iteratorEnd.row; iterator.row++ )
    {
        for( iterator.col = iteratorBegin.col; iterator.col <= iteratorEnd.col; iterator.col++ )
        {
            if( InBounds( iterator ) )
            {
                _visionMap[( iterator.row * _maxCol ) + iterator.col] = true;
            }
        }
    }
}
void Dungeon::UpdateTile( const Vector2i& position )
{
    auto& tile = _tileMap[( position.row * _maxCol ) + position.col];

    tile.icon = Icon::Ground;

    for( auto index : tile.indexOccupants )
    {
        if( !( _components.attributes[index] & Attributes::Hidden ) )
        {
            tile.icon = _components.icon[index];
        }
    }
}
void Dungeon::OccupantAdd( std::size_t index )
{
    const Vector2i position = _components.position[index];
    auto& indexes = _tileMap[( position.row * _maxCol ) + position.col].indexOccupants;

    indexes.push_back( index );
    UpdateTile( position );
}
void Dungeon::OccupantRemove( std::size_t index )
{
    const Vector2i position = _components.position[index];
    auto& indexes = _tileMap[( position.row * _maxCol ) + position.col].indexOccupants;

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

void Dungeon::DoorAdd( const Vector2i& position )
{
    const std::size_t index = _components.Add( );

    _components.icon[index] = Icon::Door;
    _components.position[index] = position;
    _components.attributes[index] = Attributes::PassablePlayer;
    OccupantAdd( index );
}
void Dungeon::WallAdd( const Vector2i& position )
{
    const std::size_t index = _components.Add( );

    _components.icon[index] = Icon::Wall;
    _components.position[index] = position;
    _components.attributes[index] = 0;
    OccupantAdd( index );
}
void Dungeon::StepAdd( const Vector2i& position )
{
    const std::size_t index = _components.Add( );

    _components.position[index] = position;
    _components.attributes[index] = Attributes::Hidden |
                                    Attributes::PassableOthers |
                                    Attributes::PassablePlayer;
    OccupantAdd( index );
}
void Dungeon::MonsterAdd( const Vector2i& position )
{
    const std::size_t index = _components.Add( );

    _components.icon[index] = Icon::Monster;
    _components.position[index] = position;
    _components.positionPrevious[index] = position;
    _components.attributes[index] = Attributes::Monster |
                                    Attributes::MovementRandom |
                                    Attributes::PassablePlayer |
                                    Attributes::PassableOthers;
    OccupantAdd( index );
}

void Dungeon::GenerateDoors( bool generate, int amount )
{
    if( generate )
    {
        const int amountDoors = amount ? amount : 3;
        std::vector<Vector2i> valid;
        Vector2i iterator;

        for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
        {
            for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
            {
                if( iterator.col == 0 ||
                    iterator.row == 0 ||
                    iterator.col == _maxCol - 1 ||
                    iterator.row == _maxRow - 1 )
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
            int index = RandomNumberGenerator( 0, valid.size( ) - 1 );

            DoorAdd( valid[index] );
            links.push_back( { 0, { -1, -1 }, valid[index] } );
            valid.erase( valid.begin( ) + index );
        }
    }
}
void Dungeon::GenerateOuterWalls( bool generate )
{
    if( generate )
    {
        Vector2i iterator;

        for( iterator.row = 0; iterator.row < _maxRow; iterator.row++ )
        {
            for( iterator.col = 0; iterator.col < _maxCol; iterator.col++ )
            {
                if( Unoccupied( iterator ) )
                {
                    if( iterator.col == 0 ||
                        iterator.row == 0 ||
                        iterator.col == _maxCol - 1 ||
                        iterator.row == _maxRow - 1 )
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
        const Vector2i center = { _maxCol / 2, _maxRow / 2 };
        std::vector<Vector2i> obstacles;

        for( std::size_t index = 0; index < _components.indexCount; index++ )
        {
            if( !( _components.attributes[index] & Attributes::PassableOthers ) )
            {
                obstacles.push_back( _components.position[index] );
            }
        }

        for( const auto& link : links )
        {
            std::vector<Vector2i> path = AStarAlgorithm( link.entry, center, _maxCol, _maxRow, obstacles );

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
        const Vector2i center = { _maxCol / 2, _maxRow / 2 };
        int sourceWallsLeft = amount ? amount : ( _maxCol * _maxRow ) / 20;

        while( sourceWallsLeft > 0 )
        {
            Vector2i position;

            position.col = RandomNumberGenerator( 1, _maxCol - 2 );
            position.row = RandomNumberGenerator( 1, _maxRow - 2 );

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
        const Vector2i center = { _maxCol / 2, _maxRow / 2 };
        const std::array<Vector2i, 4> directions =
        { {
            {  0, -1 },
            { -1,  0 },
            {  0,  1 },
            {  1,  0 }
        } };
        int extensionWallsLeft = amount ? amount : ( _maxCol * _maxRow ) / 5;

        while( extensionWallsLeft > 0 )
        {
            for( std::size_t index = 0; index < _components.indexCount; index++ )
            {
                if( !( _components.attributes[index] & Attributes::PassablePlayer ) &&
                    !( _components.attributes[index] & Attributes::PassableOthers ) )
                {
                    const int indexDirection = RandomNumberGenerator( 0, 3 );
                    const Vector2i position = _components.position[index] + directions[indexDirection];

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
        const Vector2i center = { _maxCol / 2, _maxRow / 2 };
        const int amountWalls = amount ? amount : 5;
        Vector2i iterator;

        for( int i = 0; i < amountWalls; i++ )
        {
            for( iterator.row = 1; iterator.row < _maxRow - 1; iterator.row++ )
            {
                for( iterator.col = 1; iterator.col < _maxCol - 1; iterator.col++ )
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
        const Vector2i center = { _maxCol / 2, _maxRow / 2 };
        const int min = static_cast<int>( sqrt( _maxCol * _maxRow ) / 3.0 );
        const int max = static_cast<int>( sqrt( _maxCol * _maxRow ) / 1.5 );
        const int amountMonsters = amount ? amount : RandomNumberGenerator( min, max );

        for( int i = 0; i < amountMonsters; i++ )
        {
            while( true )
            {
                Vector2i position;

                position.col = RandomNumberGenerator( 1, _maxCol - 2 );
                position.row = RandomNumberGenerator( 1, _maxRow - 2 );

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