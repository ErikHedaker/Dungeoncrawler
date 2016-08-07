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

Dungeon::Dungeon( const EntityLibrary& entityLibrary, const DungeonConfiguration& config ) :
    _size
    ( {
        config.sizeDungeonFixed ? config.sizeDungeon.x : RandomNumberGenerator( 30, 50 ),
        config.sizeDungeonFixed ? config.sizeDungeon.y : RandomNumberGenerator( 30, 50 )
    } ),
    _entityLibrary( entityLibrary ),
    _tileMap( _size.x * _size.y ),
    _visionMap( _size.x * _size.y, false ),
    _indexPlayer( -1 )

{
    GenerateDoors( config.generateDoors, config.amountDoors );
    GenerateOuterWalls( config.generateOuterWalls );
    GenerateHiddenPath( config.generateHiddenPath );
    GenerateSourceWalls( config.generateSourceWalls, config.amountSourceWalls );
    GenerateExtensionWalls( config.generateExtensionWalls, config.amountExtensionWalls );
    GenerateFillerWalls( config.generateFillerWalls, config.amountFillerWallsCycles );
    GenerateMonsters( config.generateMonsters, config.amountMonsters );
}
Dungeon::Dungeon( const EntityLibrary& entityLibrary, const Vector2<int>& size, const std::vector<bool>& visionMap, const std::vector<char>& iconMap ) :
    _size( size ),
    _entityLibrary( entityLibrary ),
    _tileMap( size.x * size.y ),
    _visionMap( visionMap ),
    _indexPlayer( -1 )
{
    Vector2<int> iterator;
    std::pair<Category::CategoryType, int> entity;
    auto GetEntity = [&entityLibrary] ( char icon )
    {
        auto CheckBase = [icon] ( const BaseEntity& base )
        {
            return base.icon == icon;
        };
        auto ability = std::find_if( entityLibrary.abilities.begin( ), entityLibrary.abilities.end( ), CheckBase );
        auto character = std::find_if( entityLibrary.characters.begin( ), entityLibrary.characters.end( ), CheckBase );
        auto structure = std::find_if( entityLibrary.structures.begin( ), entityLibrary.structures.end( ), CheckBase );

        if( ability != entityLibrary.abilities.end( ) )
        {
            return std::make_pair( Category::Ability, std::distance( entityLibrary.abilities.begin( ), ability ) );
        }

        if( character != entityLibrary.characters.end( ) )
        {
            return std::make_pair( Category::Character, std::distance( entityLibrary.characters.begin( ), character ) );
        }

        if( structure != entityLibrary.structures.end( ) )
        {
            return std::make_pair( Category::Structure, std::distance( entityLibrary.structures.begin( ), structure ) );
        }

        return std::make_pair( Category::Ability, 0 );
    };

    for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < size.x; iterator.x++ )
        {
            char icon = iconMap[( iterator.y * size.x ) + iterator.x];

            if( icon != '-' )
            {
                if( icon == '@' )
                {
                    PlayerAdd( iterator );
                }
                else
                {
                    entity = GetEntity( icon );
                    EntityAdd( iterator, entity.first, entity.second );
                }
            }
        }
    }
}

void Dungeon::Rotate( const Orientation::OrientationType& orientation )
{
    for( int i = 0; i < orientation; i++ )
    {
        auto tileMapRotated = _tileMap;
        auto visionMapRotated = _visionMap;
        Vector2<int> iterator;

        std::swap( _size.x, _size.y );

        for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
        {
            for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
            {
                tileMapRotated[( iterator.y * _size.x ) + iterator.x] = _tileMap[( iterator.x * _size.y ) + iterator.y];
                visionMapRotated[( iterator.y * _size.x ) + iterator.x] = _visionMap[( iterator.x * _size.y ) + iterator.y];
            }

            auto& tileColoumBegin = tileMapRotated.begin( ) + iterator.y * _size.x;
            auto& tileColoumEnd = tileMapRotated.begin( ) + iterator.y * _size.x + _size.x;
            auto& visionColoumBegin = visionMapRotated.begin( ) + iterator.y * _size.x;
            auto& visionColoumEnd = visionMapRotated.begin( ) + iterator.y * _size.x + _size.x;

            std::reverse( tileColoumBegin, tileColoumEnd );
            std::reverse( visionColoumBegin, visionColoumEnd );
        }

        _tileMap = tileMapRotated;
        _visionMap = visionMapRotated;
    }

    for( auto& entity : _entities )
    {
        entity.position = PositionRotate( entity.position, _size, orientation );
        entity.positionPrevious = PositionRotate( entity.positionPrevious, _size, orientation );
    }
}
const Orientation::OrientationType& Dungeon::RotateOnSwitch( const Vector2<int>& position )
{
    const std::map<Orientation::OrientationType, Vector2<int>> directions =
    {
        { Orientation::North, {  0, -1 } },
        { Orientation::East,  {  1,  0 } },
        { Orientation::South, {  0,  1 } },
        { Orientation::West,  { -1,  0 } }
    };
    Orientation::OrientationType orientation;

    for( const auto& direction : directions )
    {
        const Vector2<int> nearby = position + direction.second;

        if( !InBounds( nearby ) )
        {
            orientation = direction.first;
            Rotate( direction.first );

            break;
        }
    }

    return orientation;
}
void Dungeon::PlayerAdd( const Vector2<int>& position )
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

    EntityAdd( positionPlayer, Category::PlayerEntity, 0 );
    _indexPlayer = _entities.size( ) - 1;
    UpdateVision( _entities[_indexPlayer].position, static_cast<PlayerEntity*>( _entities[_indexPlayer].base.get( ) )->player.visionReach );
    OccupantAdd( _indexPlayer );
}

void Dungeon::MovementPlayer( const Orientation::OrientationType& orientation )
{
    OccupantRemove( _indexPlayer );
    _entities[_indexPlayer].positionPrevious = _entities[_indexPlayer].position;
    _entities[_indexPlayer].position = PositionMove( _entities[_indexPlayer].position, orientation );
    OccupantAdd( _indexPlayer );
}
void Dungeon::MovementRandom( )
{
    for( unsigned int i = 0; i < _entities.size( ); i++ )
    {
        if( _entities[i].base->attributes & Attributes::MovementRandom )
        {
            OccupantRemove( i );
            _entities[i].positionPrevious = _entities[i].position;
            _entities[i].position = PositionMoveProbability( _entities[i].position, 1, 1, 1, 1, 12 );
            OccupantAdd( i );
        }
    }
}
void Dungeon::Events( )
{
    Player& player = static_cast<PlayerEntity*>( _entities[_indexPlayer].base.get( ) )->player;
    std::vector<int> indexesEntityRemove;

    /* Collision detection for player */
    if( !CheckTile( _entities[_indexPlayer].position, Attributes::PassablePlayer ) )
    {
        OccupantRemove( _indexPlayer );
        std::swap( _entities[_indexPlayer].position, _entities[_indexPlayer].positionPrevious );
        OccupantAdd( _indexPlayer );
    }

    /* Collision detection for other entities */
    for( unsigned int i = 0; i < _entities.size( ); i++ )
    {
        if( _entities[i].base->attributes & Attributes::MovementRandom &&
            !CheckTile( _entities[i].position, Attributes::PassableOthers ) )
        {
            OccupantRemove( i );
            std::swap( _entities[i].position, _entities[i].positionPrevious );
            OccupantAdd( i );
        }
    }
    
    /* Engage combative entity on players position */
    for( auto i : _tileMap[( _entities[_indexPlayer].position.y * _size.x ) + _entities[_indexPlayer].position.x].indexOccupants )
    {
        if( _entities[i].base->attributes & Attributes::Combative )
        {
            Combat( player, *static_cast<Character*>( _entities[i].base.get( ) ) );

            if( player.health > 0 )
            {
                indexesEntityRemove.push_back( i );
            }
        }
    }

    /* Switch dungeon for player */
    for( const auto& link : links )
    {
        if( _entities[_indexPlayer].position == link.entry )
        {
            player.states |= States::Switch;
            OccupantRemove( _indexPlayer );
        }
    }

    for( auto i : indexesEntityRemove )
    {
        EntityRemove( i );
    }

    UpdateVision( _entities[_indexPlayer].position, player.visionReach );
}

const Vector2<int>& Dungeon::GetSize( ) const
{
    return _size;
}
const Vector2<int>& Dungeon::GetPlayerPosition( ) const
{
    return _entities[_indexPlayer].position;
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
        if( _entities[i].base->attributes & bitmask )
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

    tile.icon = '-';

    for( auto i : tile.indexOccupants )
    {
        tile.icon = _entities[i].base->icon;
    }
}
void Dungeon::EntityAdd( const Vector2<int>& position, Category::CategoryType category, int id )
{
    switch( category )
    {
        case Category::Ability:
        {
            _entities.push_back( { std::unique_ptr<BaseEntity>( new Ability( _entityLibrary.abilities[id] ) ), category, position, position } );

            break;
        }
        case Category::Character:
        {
            _entities.push_back( { std::unique_ptr<BaseEntity>( new Character( _entityLibrary.characters[id] ) ), category, position, position } );

            break;
        }
        case Category::Structure:
        {
            _entities.push_back( { std::unique_ptr<BaseEntity>( new Structure( _entityLibrary.structures[id] ) ), category, position, position } );

            break;
        }
        case Category::PlayerEntity:
        {
            _entities.push_back( { std::unique_ptr<BaseEntity>( new PlayerEntity( *_entityLibrary.player ) ), category, position, position } );

            break;
        }
        default:
        {
            abort( );
        }
    }

    OccupantAdd( _entities.size( ) - 1 );
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

        for( unsigned int i = 0; i < _entities.size( ); i++ )
        {
            if( !( _entities[i].base->attributes & Attributes::PassablePlayer ) )
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
            for( unsigned int i = 0; i < _entities.size( ); i++ )
            {
                if( !( _entities[i].base->attributes & Attributes::PassablePlayer ) &&
                    !( _entities[i].base->attributes & Attributes::PassableOthers ) )
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
                    std::vector<int> possibilities( { 0, 1, 2 } );

                    EntityAdd( position, Category::Character, possibilities[RandomNumberGenerator( 0, possibilities.size( ) - 1 )] );

                    break;
                }
            }
        }
    }
}