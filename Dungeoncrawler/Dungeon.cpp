#include "Dungeon.h"
#include "Functions.h"
#include "AStarAlgorithm.h"
#include <algorithm>
#include <random>
#include <cmath>
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
                    EntityAdd( iterator, entityFactory.Get( icon )->Clone( ) );
                }
            }
        }
    }
}

void Dungeon::Rotate( const Orientation::Enum& orientation )
{
    const Vector2<int> sizeSwap = { _size.y, _size.x };
    const Vector2<int> sizePrev = { _size.x, _size.y };
    const Vector2<int> sizeNext = ( orientation + 2 ) % 2 ? sizeSwap : sizePrev;
    std::vector<Tile> transform = _tiles;
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            const Vector2<int> rotation = PositionRotate( iterator, sizePrev, orientation );
            const int indexPrev = ( rotation.x * sizeNext.y ) + rotation.y;
            const int indexNext = ( iterator.x * sizePrev.y ) + iterator.y;

            transform[indexNext] = _tiles[indexPrev];
        }
    }

    for( auto& entity : _entities )
    {
        entity->position = PositionRotate( entity->position, sizePrev, orientation );
    }

    _player.real->position = PositionRotate( _player.real->position, sizePrev, orientation );
    _tiles = transform;
    _size = sizeNext;
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

            Fight( *_player.real, *enemy );

            if( enemy->health < 0 )
            {
                removable.push_back( &entity );
            }
        }
    }

    /* Swap dungeon for player */
    for( const auto& link : links )
    {
        if( _player.real->position == link.entrance )
        {
            _player.real->states |= States::Swapping;
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
bool Dungeon::Exterior( const Vector2<int>& position, int layer ) const
{
    return
        position.x == layer ||
        position.y == layer ||
        position.x == _size.x - layer - 1 ||
        position.y == _size.y - layer - 1;
}
bool Dungeon::InCorner( const Vector2<int>& position, int sensitivity ) const
{
    return
        ( position.x <= sensitivity || position.x >= _size.x - sensitivity - 1 ) &&
        ( position.y <= sensitivity || position.y >= _size.y - sensitivity - 1 );
}
bool Dungeon::InBounds( const Vector2<int>& position ) const
{
    return
        position.x >= 0 &&
        position.y >= 0 &&
        position.x < _size.x &&
        position.y < _size.y;
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
void Dungeon::EntityAdd( const Vector2<int>& position, Entity* entity )
{
    _entities.push_back( std::unique_ptr<Entity>( entity ) );
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
    const int start = RandomNumberGenerator( 0, 3 );
    const int sensitivity = static_cast<int>( std::ceil( ( std::sqrt( _size.x * _size.y ) + 6.0 ) / 10.0 ) - 1.0 );
    std::map<Orientation::Enum, std::vector<Vector2<int>>> sides;
    std::vector<int> spacing;
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            if( Exterior( iterator ) &&
                !InCorner( iterator, sensitivity ) )
            {
                sides[GetQuadrant( iterator )].push_back( iterator );
            }
        }
    }

    for( int i = 0; i < doors; i++ )
    {
        const int value = ( start + i ) % 4;

        spacing.push_back( value - 1 );
    }

    for( const auto value : spacing )
    {
        const Orientation::Enum side = static_cast<Orientation::Enum>( value );
        const int index = RandomNumberGenerator( 0, sides[side].size( ) - 1 );

        EntityAdd( sides[side][index], entityFactory.Get( "Door" )->Clone( ) );
        links.push_back( { -1, -1, sides[side][index], { -1, -1 } } );
        sides[side].erase( sides[side].begin( ) + index );
    }
}
void Dungeon::GenerateWallsOuter( const EntityFactory& entityFactory )
{
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            if( Exterior( iterator ) &&
                Unoccupied( iterator ) )
            {
                EntityAdd( iterator, entityFactory.Get( "Wall" )->Clone( ) );
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
        const std::vector<Vector2<int>> path = AStarAlgorithm( link.entrance, center, _size, obstacles );

        for( const auto& position : path )
        {
            if( Unoccupied( position ) )
            {
                EntityAdd( position, entityFactory.Get( "Path" )->Clone( ) );
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
            EntityAdd( position, entityFactory.Get( "Wall" )->Clone( ) );
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
                    EntityAdd( position, entityFactory.Get( "Wall" )->Clone( ) );
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
                    EntityAdd( iterator, entityFactory.Get( "Wall" )->Clone( ) );
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

                EntityAdd( position, entityFactory.Get( random )->Clone( ) );

                break;
            }
        }
    }
}