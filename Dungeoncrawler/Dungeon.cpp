#include "Dungeon.h"
#include "Functions.h"
#include "AStarAlgorithm.h"
#include <math.h>
#include <algorithm>
#include <random>
#include <map>

DungeonConfiguration::DungeonConfiguration( ) :
    size( { false, { 0, 0 } } ),
    generate( { true, true, true, true, true, true, true } ),
    amount( { 0, 0, 0, 0, 0 } )
{ }
DungeonConfiguration::DungeonConfiguration( const std::vector<std::string>& data )
{
    try
    {
        size.determined          = std::stoi( data[0] ) != 0;
        size.dungeon             = { std::stoi( data[1] ), std::stoi( data[2] ) };
        generate.doors           = std::stoi( data[3] ) != 0;
        generate.wallsOuter      = std::stoi( data[4] ) != 0;
        generate.hiddenPath      = std::stoi( data[5] ) != 0;
        generate.wallsParents    = std::stoi( data[6] ) != 0;
        generate.wallsChildren   = std::stoi( data[7] ) != 0;
        generate.wallsFiller     = std::stoi( data[8] ) != 0;
        generate.monsters        = std::stoi( data[9] ) != 0;
        amount.doors             = std::stoi( data[10] );
        amount.wallsParents      = std::stoi( data[11] );
        amount.wallsChildren     = std::stoi( data[12] );
        amount.wallsFillerCycles = std::stoi( data[13] );
        amount.monsters          = std::stoi( data[14] );
    }
    catch( ... )
    {
        *this = DungeonConfiguration( );
    }
}

Dungeon::Dungeon( PlayerType& player, const EntityFactory& entityFactory, const DungeonConfiguration& config ) :
    _size( [&config] ( )
    {
        constexpr int min = 30;
        constexpr int max = 50;
        const Vector2<int> random
        {
            RandomNumberGenerator( min, max ),
            RandomNumberGenerator( min, max )
        };

        return config.size.determined ? config.size.dungeon : random;
    }( ) ),
    _tiles( _size.x * _size.y ),
    _player( player )
{
    if( config.generate.doors )         GenerateDoors( entityFactory, config.amount.doors );
    if( config.generate.wallsOuter )    GenerateWallsOuter( entityFactory );
    if( config.generate.hiddenPath )    GenerateHiddenPath( entityFactory );
    if( config.generate.wallsParents )  GenerateWallsParents( entityFactory, config.amount.wallsParents );
    if( config.generate.wallsChildren ) GenerateWallsChildren( entityFactory, config.amount.wallsChildren );
    if( config.generate.wallsFiller )   GenerateWallsFiller( entityFactory, config.amount.wallsFillerCycles );
    if( config.generate.monsters )      GenerateMonsters( entityFactory, config.amount.monsters );
}
Dungeon::Dungeon( PlayerType& player, const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<bool>& vision, const std::vector<char>& icons ) :
    _size( size ),
    _tiles( size.x * size.y ),
    _player( player )
{
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < size.x; iterator.x++ )
        {
            const char icon = icons[( iterator.y * size.x ) + iterator.x];

            _tiles[( iterator.y * size.x ) + iterator.x].visible = vision[( iterator.y * size.x ) + iterator.x];

            if( icon != '-' )
            {
                if( icon == '@' )
                {
                    PlayerPlace( iterator );
                }
                else
                {
                    EntityAdd( iterator, entityFactory, icon );
                }
            }
        }
    }
}

void Dungeon::Rotate( const Orientation::Enum& orientation )
{
    const Vector2<int> sizeSwp = { _size.y, _size.x };
    const Vector2<int> sizeOld = { _size.x, _size.y };
    const Vector2<int> sizeNew = ( orientation + 2 ) % 2 ? sizeSwp : sizeOld;
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
    static constexpr std::array<Vector2<int>, 4> directions
    { {
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
        { -1,  0 }
     } };

    _player.real->position = InBounds( position ) ? position : _size / 2;

    if( !TileContains( position, Attributes::PassableOthers ) )
    {
        for( const auto& direction : directions )
        {
            const Vector2<int> nearby = position + direction;

            if( InBounds( nearby ) &&
                TileContains( nearby, Attributes::PassableOthers ) )
            {
                _player.real->position = nearby;

                break;
            }
        }
    }

    OccupantAdd( _player.real->position, _player.base );
    UpdateVision( _player.real->position, _player.real->visionReach );
}
void Dungeon::MovementPlayer( const Orientation::Enum& orientation )
{
    const Vector2<int> moving = PositionMove( _player.real->position, orientation );

    if( InBounds( moving ) &&
        TileContains( moving, Attributes::PassablePlayer ) )
    {
        OccupantRemove(_player.real->position, _player.base );
        _player.real->position = moving;
        OccupantAdd( _player.real->position, _player.base );
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
            const Vector2<int> moving = PositionMoveProbability( entity->position, 1, 1, 1, 1, 12 );

            if( InBounds( moving ) &&
                TileContains( moving, Attributes::PassableOthers ) )
            {
                OccupantRemove( entity->position, entity );
                entity->position = moving;
                OccupantAdd( entity->position, entity );
            }
        }
    }
}
void Dungeon::Events( )
{
    std::vector<std::unique_ptr<Entity>*> removable;
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
                removable.push_back( &entity );
            }
        }
    }

    /* Switch dungeon for player */
    for( const auto& link : links )
    {
        if( _player.real->position == link.entry )
        {
            _player.real->states |= States::Switch;
            OccupantRemove( _player.real->position, _player.base );
        }
    }

    /* Remove dead characters */
    for( auto& entity : removable )
    {
        EntityRemove( (*entity)->position, *entity );
    }
}

const Vector2<int>& Dungeon::GetSize( ) const
{
    return _size;
}
const Tile& Dungeon::GetTile( const Vector2<int>& position ) const
{
    return _tiles[( position.y * _size.x ) + position.x];
}
Orientation::Enum Dungeon::GetQuadrant( Vector2<int> position ) const
{
    static const std::map<std::pair<bool, bool>, Orientation::Enum> quadrants
    {
        { { true,  false }, Orientation::North },
        { { true,  true  }, Orientation::East  },
        { { false, true  }, Orientation::South },
        { { false, false }, Orientation::West  }
    };
    const Vector2<float> positionf = position;
    const Vector2<float> sizef     = _size;
    const Vector2<float> ratiof    = sizef / sizef.y;
    const bool rightOfMainDiagonal = positionf.x > ( positionf.y * ratiof.x );
    const bool rightOfAntiDiagonal = positionf.x > ( sizef.x - positionf.y * ratiof.x - 1 );

    return quadrants.at( { rightOfMainDiagonal, rightOfAntiDiagonal } );
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
    return _tiles[( position.y * _size.x ) + position.x].occupants.empty( );
}
bool Dungeon::Surrounded( const Vector2<int>& position, int threshold ) const
{
    static constexpr std::array<Vector2<int>, 8> directions
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

        if( !TileContains( neighbour, Attributes::PassablePlayer ) )
        {
            entities++;
        }
    }

    return entities >= threshold;
}
bool Dungeon::TileContains( const Vector2<int>& position, int bitmask ) const
{
    for( auto occupants : GetTile( position ).occupants )
    {
        if( !( (*occupants)->attributes & bitmask ) )
        {
            return false;
        }
    }

    return true;
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
void Dungeon::EntityAdd( const Vector2<int>& position, const EntityFactory& entityFactory, const std::pair<EntityType::Enum, int>& id )
{
    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( id )->Clone( ) ) );
    _entities.back( )->position = position;
    OccupantAdd( position, _entities.back( ) );
}
void Dungeon::EntityAdd( const Vector2<int>& position, const EntityFactory& entityFactory, const std::string& name )
{
    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( name )->Clone( ) ) );
    _entities.back( )->position = position;
    OccupantAdd( position, _entities.back( ) );
}
void Dungeon::EntityAdd( const Vector2<int>& position, const EntityFactory& entityFactory, char icon )
{
    _entities.push_back( std::unique_ptr<Entity>( entityFactory.Get( icon )->Clone( ) ) );
    _entities.back( )->position = position;
    OccupantAdd( position, _entities.back( ) );
}
void Dungeon::EntityRemove( const Vector2<int>& position, std::unique_ptr<Entity>& entity )
{
    for( auto it = _entities.begin( ); it != _entities.end( ); it++ )
    {
        if( *it == entity )
        {
            OccupantRemove( position, entity );
            _entities.erase( it );

            break;
        }
    }
}
void Dungeon::OccupantAdd( const Vector2<int>& position, std::unique_ptr<Entity>& entity )
{
    auto& occupants = _tiles[( position.y * _size.x ) + position.x].occupants;

    occupants.push_back( &entity );
    UpdateTile( position );
}
void Dungeon::OccupantRemove( const Vector2<int>& position, std::unique_ptr<Entity>& entity )
{
   auto& occupants = _tiles[( position.y * _size.x ) + position.x].occupants;

    occupants.erase( std::remove_if( occupants.begin( ), occupants.end( ), [&entity] ( const auto& it ) { return entity.get( ) == it->get( ); } ), occupants.end( ) );
    UpdateTile( position );
}

void Dungeon::GenerateDoors( const EntityFactory& entityFactory, int amount )
{
    const int doors = amount ? amount : 3;
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

    for( int i = 0; i < doors; i++ )
    {
        const int index = RandomNumberGenerator( 0, valid.size( ) - 1 );

        EntityAdd( valid[index], entityFactory, "Door" );
        links.push_back( { -1, -1, { -1, -1 }, valid[index] } );
        valid.erase( valid.begin( ) + index );
    }
}
void Dungeon::GenerateWallsOuter( const EntityFactory& entityFactory )
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
                    EntityAdd( iterator, entityFactory, "Wall" );
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
                EntityAdd( position, entityFactory, "Path" );
            }
        }
    }
}
void Dungeon::GenerateWallsParents( const EntityFactory& entityFactory, int amount )
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
            EntityAdd( position, entityFactory, "Wall" );
            remaining--;
        }
    }
}
void Dungeon::GenerateWallsChildren( const EntityFactory& entityFactory, int amount )
{
    static constexpr std::array<Vector2<int>, 4> directions
    { {
        {  0, -1 },
        { -1,  0 },
        {  0,  1 },
        {  1,  0 }
    } };
    const Vector2<int> center = _size / 2;
    int remaining = amount ? amount : ( _size.x * _size.y ) / 5;

    while( remaining > 0 )
    {
        for( const auto& entity : _entities )
        {
            if( !( entity->attributes & Attributes::PassablePlayer ) &&
                !( entity->attributes & Attributes::PassableOthers ) )
            {
                const int index = RandomNumberGenerator( 0, directions.size( ) - 1 );
                const Vector2<int> position = entity->position + directions[index];

                if( InBounds( position ) &&
                    Unoccupied( position ) &&
                    position != center )
                {
                    EntityAdd( position, entityFactory, "Wall" );
                    remaining--;
                }
            }
        }
    }
}
void Dungeon::GenerateWallsFiller( const EntityFactory& entityFactory, int amount )
{
    const Vector2<int> center = _size / 2;
    const int walls = amount ? amount : 5;
    Vector2<int> iterator;

    for( int i = 0; i < walls; i++ )
    {
        for( iterator.y = 1; iterator.y < _size.y - 1; iterator.y++ )
        {
            for( iterator.x = 1; iterator.x < _size.x - 1; iterator.x++ )
            {
                if( Unoccupied( iterator ) &&
                    Surrounded( iterator, 5 ) &&
                    iterator != center )
                {
                    EntityAdd( iterator, entityFactory, "Wall" );
                }
            }
        }
    }
}
void Dungeon::GenerateMonsters( const EntityFactory& entityFactory, int amount )
{
    static const std::vector<Character> characters = LoadCharacters( LoadAbilities( ) );
    const Vector2<int> center = _size / 2;
    const int min = static_cast<int>( sqrt( _size.x * _size.y ) / 3.0 );
    const int max = static_cast<int>( sqrt( _size.x * _size.y ) / 1.5 );
    const int monsters = amount ? amount : RandomNumberGenerator( min, max );

    for( int i = 0; i < monsters; i++ )
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
                const int index = RandomNumberGenerator( 0, characters.size( ) - 1 );
                const std::string random = characters[index].name;

                EntityAdd( position, entityFactory, random );

                break;
            }
        }
    }
}