#include "Dungeon.h"
#include "Functions.h"
#include "AStarAlgorithm.h"
#include <algorithm>
#include <random>
#include <cmath>
#include <map>
#include <optional>
#include <memory>

DungeonConfiguration::DungeonConfiguration( ) :
    size( { false, { 0, 0 } } ),
    generate( { true, true, true, true, true, true, true } ),
    amount( { 0, 0, 0, 0, 0 } )
{ }
DungeonConfiguration::DungeonConfiguration( const std::vector<std::string>& data )
{
    size.determined          = std::stoi( data[0] ) != 0;
    size.dungeon             = { std::stoi( data[1] ), std::stoi( data[2] ) };
    generate.doors           = std::stoi( data[3] ) != 0;
    generate.wallsOuter      = std::stoi( data[4] ) != 0;
    generate.hiddenPath      = std::stoi( data[5] ) != 0;
    generate.wallsParents    = std::stoi( data[6] ) != 0;
    generate.wallsChildren   = std::stoi( data[7] ) != 0;
    generate.wallsFiller     = std::stoi( data[8] ) != 0;
    generate.enemies         = std::stoi( data[9] ) != 0;
    amount.doors             = std::stoi( data[10] );
    amount.wallsParents      = std::stoi( data[11] );
    amount.wallsChildren     = std::stoi( data[12] );
    amount.wallsFillerCycles = std::stoi( data[13] );
    amount.enemies           = std::stoi( data[14] );
}

Dungeon::Dungeon( PlayerHandle& player, const EntityFactory& entityFactory, const DungeonConfiguration& config ) :
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
    _tiles( _size ),
    _player( player )
{
    if( config.generate.doors )         GenerateDoors( entityFactory, config.amount.doors );
    if( config.generate.wallsOuter )    GenerateWallsOuter( entityFactory );
    if( config.generate.hiddenPath )    GenerateHiddenPath( entityFactory );
    if( config.generate.wallsParents )  GenerateWallsParents( entityFactory, config.amount.wallsParents );
    if( config.generate.wallsChildren ) GenerateWallsChildren( entityFactory, config.amount.wallsChildren );
    if( config.generate.wallsFiller )   GenerateWallsFiller( entityFactory, config.amount.wallsFillerCycles );
    if( config.generate.enemies )       GenerateEnemies( entityFactory, config.amount.enemies );
}
Dungeon::Dungeon( PlayerHandle& player, const EntityFactory& entityFactory, const Vector2<int>& size, const std::vector<char>& icons ) :
    _size( size ),
    _tiles( size ),
    _player( player )
{
    std::optional<Vector2<int>> place;
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < size.x; iterator.x++ )
        {
            const char icon = icons[( iterator.y * size.x ) + iterator.x];

            if( icon != '-' )
            {
                if( icon == '@' )
                {
                    place = iterator;
                }
                else
                {
                    EntityInsert( iterator, entityFactory.Get( icon )->Clone( ) );
                }
            }
        }
    }

    if( place )
    {
        PlayerPlace( *place );
    }
}

void Dungeon::PlayerPlace( const Vector2<int>& position )
{
    static constexpr std::array<Vector2<int>, 4> directions
    { {
        { 0, -1 },
        { 1,  0 },
        { 0,  1 },
        { -1,  0 }
        } };

    _player.real->position = InBounds( position, _size ) ? position : _size / 2;

    if( !TileLacking( position, Attributes::Obstacle ) )
    {
        for( const auto& direction : directions )
        {
            const Vector2<int> nearby = position + direction;

            if( InBounds( nearby, _size ) &&
                TileLacking( nearby, Attributes::Obstacle ) )
            {
                _player.real->position = nearby;

                break;
            }
        }
    }

    OccupantInsert( _player.real->position, _player.base.get( ) );
    BuildVision( _player.real->position, _player.real->visionReach );
}
void Dungeon::Events( const BattleSystem& battleSystem )
{
    /* Fight hostile entities on player position */
    for( auto& entity : _tiles[_player.real->position].occupants )
    {
        if( entity->attributes & Attributes::Hostile )
        {
            Character* enemy = dynamic_cast<Character*>( entity );

            battleSystem.Encounter( *_player.real, *enemy );
            _tiles[_player.real->position].icon = _player.real->icon;
        }
    }

    /* Swap dungeon if conditions meet */
    for( const auto& link : links )
    {
        if( _player.real->position == link.entrance )
        {
            _player.real->states |= States::Swapping;
            OccupantRemove( _player.real->position, _player.base.get( ) );
        }
    }


    /* Remove dead entities */
    for( unsigned int i = 0; i < _entities.size( ); i++ )
    {
        if( !_entities[i]->active )
        {
            const Vector2<int> position = _entities[i]->position;

            EntityRemove( i );
            UpdateTile( position );
        }
    }
}
void Dungeon::Rotate( const Orientation::Enum& orientation )
{
    const Vector2<int> sizeSwap = { _size.y, _size.x };
    const Vector2<int> sizePrev = { _size.x, _size.y };
    const Vector2<int> sizeNext = ( orientation + 2 ) % 2 ? sizeSwap : sizePrev;
    std::unordered_set<Vector2<int>, HasherVector2<int>> visionRotated;
    Array2D<Tile> tilesRotated( sizeNext );
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            const Vector2<int> rotation = PositionRotate( iterator, sizePrev, orientation );
            const int indexPrev = ( rotation.x * sizeNext.y ) + rotation.y;
            const int indexNext = ( iterator.x * sizePrev.y ) + iterator.y;

            tilesRotated[indexNext] = _tiles[indexPrev];
        }
    }

    for( const auto& position : _vision )
    {
        visionRotated.insert( PositionRotate( position, sizePrev, orientation ) );
    }

    for( auto& entity : _entities )
    {
        entity->position = PositionRotate( entity->position, sizePrev, orientation );
    }

    _player.real->position = PositionRotate( _player.real->position, sizePrev, orientation );
    _tiles = std::move( tilesRotated );
    _vision = visionRotated;
    _size = sizeNext;
}
void Dungeon::MovementPlayer( const Orientation::Enum& orientation )
{
    const Vector2<int> moving = PositionMove( _player.real->position, orientation );
    auto Door = [this]( const Tile& tile )
    {
        for( const auto& entity : tile.occupants )
        {
            if( entity->name == "Door" )
            {
                return true;
            }
        }

        return false;
    };

    if( InBounds( moving, _size ) &&
        ( TileLacking( moving, Attributes::Obstacle ) ||
          Door( _tiles[moving] ) ) )
    {
        OccupantRemove( _player.real->position, _player.base.get( ) );
        _player.real->position = moving;
        OccupantInsert( _player.real->position, _player.base.get( ) );
    }

    BuildVision( _player.real->position, _player.real->visionReach );
}
void Dungeon::MovementRandom( )
{
    for( int i = 0, limit = _entities.size( ); i < limit; i++ )
    {
        if( _entities[i]->attributes & Attributes::Movement )
        {
            const Vector2<int> moving = PositionMoveProbability( _entities[i]->position, 1, 1, 1, 1, 12 );

            if( InBounds( moving, _size ) &&
                TileLacking( moving, Attributes::Obstacle ) )
            {
                OccupantRemove( _entities[i]->position, _entities[i].get( ) );
                _entities[i]->position = moving;
                OccupantInsert( _entities[i]->position, _entities[i].get( ) );
            }
        }
    }
}

const Vector2<int>& Dungeon::GetSize( ) const
{
    return _size;
}
char Dungeon::GetIcon( const Vector2<int>& position ) const
{
    return _tiles[position].icon;
}
bool Dungeon::Visible( const Vector2<int>& position ) const
{
    return _vision.find( position ) != _vision.end( );
}
bool Dungeon::Unoccupied( const Vector2<int>& position ) const
{
    return _tiles[position].occupants.empty( );
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
    int count = 0;

    for( const auto& direction : directions )
    {
        const Vector2<int> neighbour = position + direction;

        if( !TileLacking( neighbour, Attributes::Obstacle ) )
        {
            count++;
        }
    }

    return count >= threshold;
}
bool Dungeon::TileLacking( const Vector2<int>& position, int bitmask ) const
{
    for( const auto& entity : _tiles[position].occupants )
    {
        if( entity->attributes & bitmask )
        {
            return false;
        }
    }

    return true;
}

void Dungeon::UpdateTile( const Vector2<int>& position )
{
    _tiles[position].icon = _tiles[position].occupants.empty( ) ? '-' : _tiles[position].occupants.back( )->icon;
}
void Dungeon::BuildVision( const Vector2<int>& position, int visionReach )
{
    static constexpr std::array<int, 2> polarity = { 1, -1 };
    static constexpr std::array<std::pair<Vector2<int>, std::pair<Vector2<int>, Vector2<int>>>, 4> neighbours
    { {
        { {  0, -1 }, { { -1, -1 }, {  1, -1 } } },
        { {  1,  0 }, { {  1, -1 }, {  1,  1 } } },
        { {  0,  1 }, { {  1,  1 }, { -1,  1 } } },
        { { -1,  0 }, { { -1,  1 }, { -1, -1 } } }
    } };
    auto LineOfSight = [this] ( const std::vector<Vector2<int>>& path )
    {
        for( const auto& current : path )
        {
            if( !InBounds( current, _size ) )
            {
                break;
            }

            _vision.insert( current );

            if( !TileLacking( current, Attributes::Obstacle ) )
            {
                break;
            }
        }
    };

    _vision.clear( );

    /* Base vision generation */
    for( const auto& endpoint : BresenhamCircle( position, visionReach ) )
    {
        LineOfSight( BresenhamLine( position, endpoint ) );
    }

    /* Fix vision artifacts by adding to parallell obstacles  */
    for( const auto& neighbour : neighbours )
    {
        const Vector2<int> direction = neighbour.first;
        const std::vector<Vector2<int>> straight = BresenhamLine( position, position + direction * visionReach );

        for( const auto& polar : polarity )
        {
            for( const auto& current : straight )
            {
                const Vector2<int> flip = { direction.y, direction.x };
                const Vector2<int> adjacent = current + flip * polar;

                if( InBounds( adjacent, _size ) &&
                    !TileLacking( adjacent, Attributes::Obstacle ) )
                {
                    _vision.insert( adjacent );
                }

                if( InBounds( current, _size ) &&
                    !TileLacking( current, Attributes::Obstacle ) )
                {
                    break;
                }
            }
        }
    }

    /* Fix vision artifacts by adding to surrounded deadspots */
    for( const auto& visible : _vision )
    {
        for( const auto& neighbour : neighbours )
        {
            const Vector2<int> adjacent = visible + neighbour.first;

            if( InBounds( adjacent, _size ) &&
                !Visible( adjacent ) )
            {
                const Vector2<int> neighbourOne = visible + neighbour.second.first;
                const Vector2<int> neighbourTwo = visible + neighbour.second.second;

                if( InBounds( neighbourOne, _size ) &&
                    InBounds( neighbourTwo, _size ) &&
                    Visible( neighbourOne ) &&
                    Visible( neighbourTwo ) )
                {
                    LineOfSight( BresenhamLine( position, adjacent ) );
                }
            }
        }
    }
}
void Dungeon::OccupantInsert( const Vector2<int>& position, Entity* entity )
{
    _tiles[position].occupants.push_back( entity );
    UpdateTile( position );
}
void Dungeon::OccupantRemove( const Vector2<int>& position, Entity* entity )
{
    _tiles[position].occupants.erase( std::remove(
        _tiles[position].occupants.begin( ),
        _tiles[position].occupants.end( ), entity ),
        _tiles[position].occupants.end( ) );
    UpdateTile( position );
}
void Dungeon::EntityInsert( const Vector2<int>& position, Entity* entity )
{
    _entities.emplace_back( entity );
    _entities.back( )->position = position;
    OccupantInsert( position, entity );
}
void Dungeon::EntityRemove( int index )
{
    if( index == _entities.size( ) - 1 )
    {
        OccupantRemove( _entities[index]->position, _entities[index].get( ) );
        _entities.pop_back( );
    }
    else
    {
        OccupantRemove( _entities[index]->position, _entities[index].get( ) );
        OccupantRemove( _entities.back( )->position, _entities.back( ).get( ) );
        std::swap( _entities[index], _entities.back( ) );
        _entities.pop_back( );
        OccupantInsert( _entities[index]->position, _entities[index].get( ) );
    }
}

void Dungeon::GenerateDoors( const EntityFactory& entityFactory, int amount )
{
    const int limit = amount ? amount : 3;
    const int start = RandomNumberGenerator( 0, 3 );
    const int sensitivity = static_cast<int>( std::ceil( ( std::sqrt( _size.x * _size.y ) + 6.0 ) / 10.0 ) - 1.0 );
    std::map<Orientation::Enum, std::vector<Vector2<int>>> sides;
    std::vector<int> spacing;
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _size.y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _size.x; iterator.x++ )
        {
            if( OnBorder( iterator, _size ) &&
                !InCorner( iterator, _size, sensitivity ) )
            {
                sides[RectQuadrant( iterator, _size )].push_back( iterator );
            }
        }
    }

    for( int i = 0; i < limit; i++ )
    {
        spacing.push_back( ( ( start + i ) % 4 ) - 1 );
    }

    for( const auto& value : spacing )
    {
        const Orientation::Enum side = static_cast<Orientation::Enum>( value );
        const int index = RandomNumberGenerator( 0, sides[side].size( ) - 1 );

        EntityInsert( sides[side][index], entityFactory.Get( "Door" )->Clone( ) );
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
            if( Unoccupied( iterator ) &&
                OnBorder( iterator, _size ) )
            {
                EntityInsert( iterator, entityFactory.Get( "Wall" )->Clone( ) );
            }
        }
    }
}
void Dungeon::GenerateHiddenPath( const EntityFactory& entityFactory )
{
    const Vector2<int> center = _size / 2;
    std::vector<Vector2<int>> obstacles;
    std::vector<std::pair<Vector2<int>, Vector2<int>>> redirection;
    auto PathAdd = [this, &entityFactory] ( const std::vector<Vector2<int>>& path )
    {
        for( const auto& position : path )
        {
            if( Unoccupied( position ) )
            {
                EntityInsert( position, entityFactory.Get( "Path" )->Clone( ) );
            }
        }
    };

    for( const auto& entity : _entities )
    {
        if( entity->attributes & Attributes::Obstacle &&
            entity->name != "Door" )
        {
            obstacles.push_back( entity->position );
        }
    }

    for( const auto& link : links )
    {
        while( true )
        {
            const Vector2<int> random
            {
                RandomNumberGenerator( 1, _size.x - 2 ),
                RandomNumberGenerator( 1, _size.y - 2 )
            };

            if( Unoccupied( random ) )
            {
                redirection.emplace_back( link.entrance, random );

                break;
            }
        }
    }

    for( const auto& pair : redirection )
    {
        PathAdd( AStarAlgorithm( pair.first, pair.second, _size, obstacles ) );
        PathAdd( AStarAlgorithm( pair.second, center, _size, obstacles ) );
    }
}
void Dungeon::GenerateWallsParents( const EntityFactory& entityFactory, int amount )
{
    int remaining = amount ? amount : ( _size.x * _size.y ) / 10;

    while( remaining > 0 )
    {
        const Vector2<int> position
        {
            RandomNumberGenerator( 1, _size.x - 2 ),
            RandomNumberGenerator( 1, _size.y - 2 )
        };

        if( Unoccupied( position ) )
        {
            EntityInsert( position, entityFactory.Get( "Wall" )->Clone( ) );
            remaining--;
        }
    }
}
void Dungeon::GenerateWallsChildren( const EntityFactory& entityFactory, int amount )
{
    static constexpr std::array<Vector2<int>, 4> directions
    { {
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
        { -1,  0 }
    } };
    int remaining = amount ? amount : ( _size.x * _size.y ) / 4;

    while( remaining > 0 )
    {
        for( int i = 0, limit = _entities.size( ); i < limit; i++ )
        {
            if( _entities[i]->attributes & Attributes::Obstacle )
            {
                const int index = RandomNumberGenerator( 0, directions.size( ) - 1 );
                const Vector2<int> position = _entities[i]->position + directions[index];

                if( InBounds( position, _size ) &&
                    Unoccupied( position ) )
                {
                    EntityInsert( position, entityFactory.Get( "Wall" )->Clone( ) );
                    remaining--;
                }
            }
        }
    }
}
void Dungeon::GenerateWallsFiller( const EntityFactory& entityFactory, int amount )
{
    const int limit = amount ? amount : 5;
    Vector2<int> iterator;

    for( int i = 0; i < limit; i++ )
    {
        for( iterator.y = 1; iterator.y < _size.y - 1; iterator.y++ )
        {
            for( iterator.x = 1; iterator.x < _size.x - 1; iterator.x++ )
            {
                if( Unoccupied( iterator ) &&
                    Surrounded( iterator, 5 ) )
                {
                    EntityInsert( iterator, entityFactory.Get( "Wall" )->Clone( ) );
                }
            }
        }
    }
}
void Dungeon::GenerateEnemies( const EntityFactory& entityFactory, int amount )
{
    static const std::vector<Entity*> enemies = entityFactory.Get( Attributes::Hostile | Attributes::Movement );
    const int limit = amount ? amount : ( _size.x * _size.y ) / 150;

    for( int i = 0; i < limit; i++ )
    {
        while( true )
        {
            const Vector2<int> position
            {
                RandomNumberGenerator( 1, _size.x - 2 ),
                RandomNumberGenerator( 1, _size.y - 2 )
            };

            if( Unoccupied( position ) )
            {
                const int index = RandomNumberGenerator( 0, enemies.size( ) - 1 );
                const std::string random = enemies[index]->name;

                EntityInsert( position, entityFactory.Get( random )->Clone( ) );

                break;
            }
        }
    }
}