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

Dungeon::Dungeon( PlayerType& player, const EntityFactory& entityFactory, const DungeonConfiguration& config ) :
    _size
    ( {
        config.sizeDungeonFixed ? config.sizeDungeon.x : RandomNumberGenerator( 30, 50 ),
        config.sizeDungeonFixed ? config.sizeDungeon.y : RandomNumberGenerator( 30, 50 )
    } ),
    _tiles( _size.x * _size.y ),
    _player( player )
{
    if( config.generateDoors ) GenerateDoors( entityFactory, config.amountDoors );
    if( config.generateOuterWalls ) GenerateOuterWalls( entityFactory );
    if( config.generateHiddenPath ) GenerateHiddenPath( entityFactory );
    if( config.generateSourceWalls ) GenerateSourceWalls( entityFactory, config.amountSourceWalls );
    if( config.generateExtensionWalls ) GenerateExtensionWalls( entityFactory, config.amountExtensionWalls );
    if( config.generateFillerWalls ) GenerateFillerWalls( entityFactory, config.amountFillerWallsCycles );
    if( config.generateMonsters ) GenerateMonsters( entityFactory, config.amountMonsters );
}
Dungeon::Dungeon( PlayerType& player, const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<bool>& vision, const std::vector<char>& icons ) :
    _size( size ),
    _tiles( size.x * size.y ),
    _player( player )
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
                    PlayerPlace( iterator );
                }
                else
                {
                    EntityAdd( entityFactory, iterator, icon );
                }
            }

            _tiles[( iterator.y * size.x ) + iterator.x].visible = vision[( iterator.y * size.x ) + iterator.x];
        }
    }
}

void Dungeon::Rotate( const Orientation::Enum& orientation )
{
    const Vector2<int> sizeOld = { _size.x, _size.y };
    const Vector2<int> sizeNew =
    {
        ( orientation + 2 ) % 2 == 0 ? _size.x : _size.y,
        ( orientation + 2 ) % 2 == 0 ? _size.y : _size.x
    };
    std::vector<Tile> transform = _tiles;
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            const Vector2<int> rotation = PositionRotate( iterator, sizeOld, orientation );
            const int indexOld = ( rotation.x * sizeNew.y ) + rotation.y;
            const int indexNew = ( iterator.x * sizeOld.y ) + iterator.y;

            transform[indexNew] = _tiles[indexOld];
        }
    }

    for( auto& entity : _entities )
    {
        entity->position = PositionRotate( entity->position, sizeOld, orientation );
    }

    _player.real->position = PositionRotate( _player.real->position, sizeOld, orientation );
    _tiles = transform;
    _size = sizeNew;
}
void Dungeon::PlayerPlace( const Vector2<int>& position )
{
    _player.real->position = InBounds( position ) ? position : _size / 2;

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
                _player.real->position = nearby;

                break;
            }
        }
    }

    OccupantAdd( _player.base, _player.real->position );
    UpdateVision( _player.real->position, _player.real->visionReach );
}
void Dungeon::MovementPlayer( const Orientation::Enum& orientation )
{
    const Vector2<int> move = PositionMove( _player.real->position, orientation );

    if( InBounds( move ) &&
        CheckTile( move, Attributes::PassablePlayer ) )
    {
        OccupantRemove( _player.base, _player.real->position );
        OccupantAdd( _player.base, move );
        _player.real->position = move;
    }

    UpdateVision( _player.real->position, _player.real->visionReach );
}
void Dungeon::MovementRandom( )
{
    for( auto& it = _entities.begin( ); it != _entities.end( ); it++ )
    {
        std::unique_ptr<Entity>& entity = *it;

        if( entity->attributes & Attributes::MovementRandom )
        {
            const Vector2<int> move = PositionMoveProbability( entity->position, 1, 1, 1, 1, 12 );

            if( InBounds( move ) &&
                CheckTile( move, Attributes::PassableOthers ) )
            {
                OccupantRemove( entity, entity->position );
                OccupantAdd( entity, move );
                entity->position = move;
            }
        }
    }
}
void Dungeon::NextTurn( )
{
    std::vector<std::unique_ptr<Entity>*> remove;
    auto& occupants = _tiles[( _player.real->position.y * _size.x ) + _player.real->position.x].occupants;
    
    /* Engage combative entity on players position */
    for( auto& it = occupants.begin( ); it != occupants.end( ); it++ )
    {
        std::unique_ptr<Entity>& entity = *(*it);

        if( entity->attributes & Attributes::Combative )
        {
            Character* const enemy = dynamic_cast<Character*>( entity.get( ) );

            Combat( *_player.real, *enemy );

            if( enemy->health < 0 )
            {
                remove.push_back( &entity );
            }
        }
    }

    /* Switch dungeon for player */
    for( const auto& link : links )
    {
        if( _player.real->position == link.entry )
        {
            _player.real->states |= States::Switch;
            OccupantRemove( _player.base, _player.real->position );
        }
    }

    /* Remove dead characters */
    for( auto& entity : remove )
    {
        EntityRemove( *entity, (*entity)->position );
    }
}

const Vector2<int>& Dungeon::GetSize( ) const
{
    return _size;
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
bool Dungeon::CheckTile( const Vector2<int>& position, int bitmask ) const
{
    int count = 0;

    for( auto occupants : GetTile( position ).occupants )
    {
        if( (*occupants)->attributes & bitmask )
        {
            count++;
        }
    }

    return count == GetTile( position ).occupants.size( );
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
        const Vector2<int> neighbour = position + direction;

        if( !CheckTile( neighbour, Attributes::PassablePlayer ) )
        {
            entities++;
        }
    }

    return entities >= threshold;
}
bool Dungeon::Unoccupied( const Vector2<int>& position ) const
{
    return _tiles[( position.y * _size.x ) + position.x].occupants.empty( );
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
                _tiles[( iterator.y * _size.x ) + iterator.x].visible = true;
            }
        }
    }
}
void Dungeon::UpdateTile( const Vector2<int>& position )
{
    Tile& tile = _tiles[( position.y * _size.x ) + position.x];

    tile.icon = tile.occupants.empty( ) ? '-' : (*tile.occupants.back( ))->icon;
}
void Dungeon::EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::pair<EntityType::Enum, int>& id )
{
    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( id )->Clone( ) ) );
    _entities.back( )->position = position;
    OccupantAdd( _entities.back( ), position );
}
void Dungeon::EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, const std::string& name )
{
    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( name )->Clone( ) ) );
    _entities.back( )->position = position;
    OccupantAdd( _entities.back( ), position );
}
void Dungeon::EntityAdd( const EntityFactory& entityFactory, const Vector2<int>& position, char icon )
{
    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( icon )->Clone( ) ) );
    _entities.back( )->position = position;
    OccupantAdd( _entities.back( ), position );
}
void Dungeon::EntityRemove( std::unique_ptr<Entity>& entity, const Vector2<int>& position )
{
    for( auto it = _entities.begin( ); it != _entities.end( ); it++ )
    {
        if( *it == entity )
        {
            OccupantRemove( entity, position );
            _entities.erase( it );

            break;
        }
    }
}
void Dungeon::OccupantAdd( std::unique_ptr<Entity>& entity, const Vector2<int>& position )
{
    auto& occupants = _tiles[( position.y * _size.x ) + position.x].occupants;

    occupants.push_back( &entity );
    UpdateTile( position );
}
void Dungeon::OccupantRemove( std::unique_ptr<Entity>& entity, const Vector2<int>& position )
{
   auto& occupants = _tiles[( position.y * _size.x ) + position.x].occupants;

    occupants.erase( std::remove_if( occupants.begin( ), occupants.end( ), [&entity]( const auto& it ){ return entity.get( ) == it->get( ); } ), occupants.end( ) );
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

    for( const auto& entity : _entities )
    {
        if( !( entity->attributes & Attributes::PassablePlayer ) )
        {
            obstacles.push_back( entity->position );
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
        const Vector2<int> position =
        {
            RandomNumberGenerator( 1, _size.x - 2 ),
            RandomNumberGenerator( 1, _size.y - 2 )
        };

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
        for( const auto& entity : _entities )
        {
            if( !( entity->attributes & Attributes::PassablePlayer ) &&
                !( entity->attributes & Attributes::PassableOthers ) )
            {
                const Vector2<int> position = entity->position + directions[RandomNumberGenerator( 0, directions.size( ) - 1 )];

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
    const int monstersAmount = amount ? amount : RandomNumberGenerator( min, max );

    for( int i = 0; i < monstersAmount; i++ )
    {
        while( true )
        {
            const Vector2<int> position
            {
                RandomNumberGenerator( 1, _size.x - 2 ),
                RandomNumberGenerator( 1, _size.y - 2 )
            };

            if( Unoccupied( position ) &&
                position != center )
            {
                const std::array<int, 3> valid = { 0, 1, 2 };

                EntityAdd( entityFactory, position, std::make_pair( EntityType::Character, valid[RandomNumberGenerator( 0, valid.size( ) - 1 )] ) );

                break;
            }
        }
    }
}