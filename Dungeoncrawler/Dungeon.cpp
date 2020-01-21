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
    _grid( [&config]( )
    {
        constexpr int min = 30;
        constexpr int max = 50;
        const Vector2<int> random
        {
            GetRNG( min, max ),
            GetRNG( min, max )
        };

        return config.size.determined ? config.size.dungeon : random;
    }( ) ),
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
Dungeon::Dungeon( PlayerHandle& player, const EntityFactory& entityFactory, const Grid<char>& icons, const std::vector<Door>& doors ) :
    _grid( icons.Size( ) ),
    _player( player )
{
    std::optional<Vector2<int>> positionPlayer;
    Vector2<int> iterator;

    for( const auto& door : doors )
    {
        EntityInsert( door.position, door.Clone( ) );
        _indexDoors.push_back( _entities.size( ) - 1 );
    }

    for( iterator.y = 0; iterator.y < _grid.Size( ).y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _grid.Size( ).x; iterator.x++ )
        {
            if( icons[iterator] != '-' &&
                icons[iterator] != '+' )
            {
                if( icons[iterator] == '@' )
                {
                    positionPlayer = iterator;
                }
                else
                {
                    EntityInsert( iterator, entityFactory.Get( icons[iterator] )->Clone( ) );
                }
            }
        }
    }

    if( positionPlayer )
    {
        _player.real->position = *positionPlayer;
        OccupantInsert( _player.real->position, _player.base.get( ) );
        BuildVision( _player.real->position, _player.real->visionReach );
    }
}

void Dungeon::Connect( const Connector& connector, int index )
{
    dynamic_cast<Door*>( _entities[_indexDoors[index]].get( ) )->connector = connector;
}
void Dungeon::PlayerSet( const std::optional<int>& index )
{
    static constexpr std::array<Vector2<int>, 4> directions
    { {
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
        { -1,  0 }
    } };

    if( index )
    {
        for( const auto& direction : directions )
        {
            const Vector2<int> neighbour = _entities[_indexDoors[*index]]->position + direction;

            if( InBounds( neighbour, _grid.Size( ) ) &&
                TileLacking( neighbour, Attributes::Obstacle ) )
            {
                _player.real->position = neighbour;

                break;
            }
        }
    }
    else
    {
        _player.real->position = _grid.Size( ) / 2;
    }

    OccupantInsert( _player.real->position, _player.base.get( ) );
    BuildVision( _player.real->position, _player.real->visionReach );
}
void Dungeon::Events( const BattleSystem& battleSystem )
{
    /* Fight hostile entities on player position */
    for( auto& entity : _grid[_player.real->position].occupants )
    {
        if( entity->attributes & Attributes::Hostile )
        {
            Character* enemy = dynamic_cast<Character*>( entity );

            battleSystem.Encounter( *_player.real, *enemy );
            _grid[_player.real->position].icon = _player.real->icon;
        }
    }

    /* Swap dungeon if conditions meet */
    if( _player.real->next )
    {
        OccupantRemove( _player.real->position, _player.base.get( ) );
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
    std::unordered_set<Vector2<int>, HasherVector2<int>> vision;

    for( const auto& position : _vision )
    {
        vision.insert( PositionRotate( position, _grid.Size( ), orientation ) );
    }

    for( auto& entity : _entities )
    {
        entity->position = PositionRotate( entity->position, _grid.Size( ), orientation );
    }

    _player.real->position = PositionRotate( _player.real->position, _grid.Size( ), orientation );
    _vision = std::move( vision );
    _grid.Rotate( orientation );
}
void Dungeon::MovementPlayer( const Orientation::Enum& orientation )
{
    const Vector2<int> moving = PositionMove( _player.real->position, orientation );

    if( InBounds( moving, _grid.Size( ) ) )
    {
        for( auto& entity : _grid[moving].occupants )
        {
            entity->Interact( *_player.real );
        }

        if( !_player.real->blocked )
        {
            OccupantRemove( _player.real->position, _player.base.get( ) );
            _player.real->position = moving;
            OccupantInsert( _player.real->position, _player.base.get( ) );
        }

        _player.real->blocked = false;
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

            if( InBounds( moving, _grid.Size( ) ) &&
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
    return _grid.Size( );
}
const std::vector<Door*> Dungeon::GetDoors( ) const
{
    std::vector<Door*> doors;

    for( auto i : _indexDoors )
    {
        doors.push_back( dynamic_cast<Door*>( _entities[i].get( ) ) );
    }

    return doors;
}
char Dungeon::GetIcon( const Vector2<int>& position ) const
{
    return _grid[position].icon;
}
bool Dungeon::Visible( const Vector2<int>& position ) const
{
    return _vision.find( position ) != _vision.end( );
}
bool Dungeon::Unoccupied( const Vector2<int>& position ) const
{
    return _grid[position].occupants.empty( );
}
bool Dungeon::TileLacking( const Vector2<int>& position, int bitmask ) const
{
    for( const auto& entity : _grid[position].occupants )
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
    _grid[position].icon = _grid[position].occupants.empty( ) ? '-' : _grid[position].occupants.back( )->icon;
}
void Dungeon::BuildVision( const Vector2<int>& position, int visionReach )
{
    _vision.clear( );

    for( const auto& endpoint : BresenhamCircle( position, visionReach * 10 ) )
    {
        LineOfSight( BresenhamLine( position, endpoint ), visionReach, position );
    }

    //for( const auto& endpoint : BresenhamCircle( position, visionReach ) )
    //{
    //    LineOfSight( BresenhamLine( position, endpoint ) );
    //}

    //FixVisionNearbyWalls( position, visionReach );
    //FixVisionDeadspots( position );
}
void Dungeon::FixVisionNearbyWalls( const Vector2<int>& position, int visionReach )
{
    static constexpr std::array<Vector2<int>, 4> directions
    { {
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
        { -1,  0 }
    } };

    for( const auto& direction : directions )
    {
        const std::vector<Vector2<int>> straight = BresenhamLine( position, position + direction * visionReach );

        for( const auto& polarity : { 1, -1 } )
        {
            for( const auto& current : straight )
            {
                const Vector2<int> flip = { direction.y, direction.x };
                const Vector2<int> adjacent = current + flip * polarity;

                if( InBounds( adjacent, _grid.Size( ) ) &&
                    !TileLacking( adjacent, Attributes::Obstacle ) )
                {
                    _vision.insert( adjacent );
                }

                if( InBounds( current, _grid.Size( ) ) &&
                    !TileLacking( current, Attributes::Obstacle ) )
                {
                    break;
                }
            }
        }
    }
}
void Dungeon::FixVisionDeadspots( const Vector2<int>& position )
{
    static constexpr std::array<std::pair<Vector2<int>, std::pair<Vector2<int>, Vector2<int>>>, 4> neighbours
    { {
        { {  0, -1 }, { { -1, -1 }, {  1, -1 } } },
        { {  1,  0 }, { {  1, -1 }, {  1,  1 } } },
        { {  0,  1 }, { {  1,  1 }, { -1,  1 } } },
        { { -1,  0 }, { { -1,  1 }, { -1, -1 } } }
    } };

    for( const auto& visible : _vision )
    {
        for( const auto& neighbour : neighbours )
        {
            const Vector2<int> adjacent = visible + neighbour.first;

            if( InBounds( adjacent, _grid.Size( ) ) &&
                !Visible( adjacent ) )
            {
                const Vector2<int> neighbourOne = visible + neighbour.second.first;
                const Vector2<int> neighbourTwo = visible + neighbour.second.second;

                if( InBounds( neighbourOne, _grid.Size( ) ) &&
                    InBounds( neighbourTwo, _grid.Size( ) ) &&
                    Visible( neighbourOne ) &&
                    Visible( neighbourTwo ) )
                {
                    LineOfSight( BresenhamLine( position, adjacent ) );
                }
            }
        }
    }
}
void Dungeon::LineOfSight( const std::vector<Vector2<int>>& path, int visionReach, Vector2<int> start )
{
    auto distanceSquared = [] ( Vector2<int> start, Vector2<int> end ) -> int
    {
        return ( start.x - end.x ) * ( start.x - end.x ) + ( start.y - end.y ) * ( start.y - end.y );
    };

    for( const auto& current : path )
    {
        if( !InBounds( current, _grid.Size( ) ) ||
            visionReach * visionReach < distanceSquared( start, current ) )
        {
            break;
        }

        _vision.insert( current );

        if( !TileLacking( current, Attributes::Obstacle ) )
        {
            break;
        }
    }
}
void Dungeon::LineOfSight( const std::vector<Vector2<int>>& path )
{
    for( const auto& current : path )
    {
        if( !InBounds( current, _grid.Size( ) ) )
        {
            break;
        }

        _vision.insert( current );

        if( !TileLacking( current, Attributes::Obstacle ) )
        {
            break;
        }
    }
}
void Dungeon::OccupantInsert( const Vector2<int>& position, Entity* entity )
{
    _grid[position].occupants.push_back( entity );
    UpdateTile( position );
}
void Dungeon::OccupantRemove( const Vector2<int>& position, Entity* entity )
{
    _grid[position].occupants.erase( std::remove(
        _grid[position].occupants.begin( ),
        _grid[position].occupants.end( ), entity ),
        _grid[position].occupants.end( ) );
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
    const int start = GetRNG( 0, 3 );
    const int cornerOffset = static_cast<int>( std::ceil( ( std::sqrt( _grid.Size( ).x * _grid.Size( ).y ) + 6.0 ) / 10.0 ) - 1.0 );
    std::map<Orientation::Enum, std::vector<Vector2<int>>> valid;
    std::vector<Orientation::Enum> sides;
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _grid.Size( ).y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _grid.Size( ).x; iterator.x++ )
        {
            if( OnBorder( iterator, _grid.Size( ) ) &&
                !InCorner( iterator, _grid.Size( ), cornerOffset ) )
            {
                valid[RectQuadrant( iterator, _grid.Size( ) )].push_back( iterator );
            }
        }
    }

    for( int i = 0; i < limit; i++ )
    {
        sides.push_back( static_cast<Orientation::Enum>( ( ( start + i ) % 4 ) - 1 ) );
    }

    for( const auto& side : sides )
    {
        const int index = GetRNG( 0, valid[side].size( ) - 1 );

        EntityInsert( valid[side][index], entityFactory.Get( "Door" )->Clone( ) );
        valid[side].erase( valid[side].begin( ) + index );
        _indexDoors.push_back( _entities.size( ) - 1 );
    }
}
void Dungeon::GenerateWallsOuter( const EntityFactory& entityFactory )
{
    Vector2<int> iterator;

    for( iterator.y = 0; iterator.y < _grid.Size( ).y; iterator.y++ )
    {
        for( iterator.x = 0; iterator.x < _grid.Size( ).x; iterator.x++ )
        {
            if( Unoccupied( iterator ) &&
                OnBorder( iterator, _grid.Size( ) ) )
            {
                EntityInsert( iterator, entityFactory.Get( "Wall" )->Clone( ) );
            }
        }
    }
}
void Dungeon::GenerateHiddenPath( const EntityFactory& entityFactory )
{
    const Vector2<int> center = _grid.Size( ) / 2;
    std::vector<Vector2<int>> obstacles;
    std::vector<std::pair<Vector2<int>, Vector2<int>>> redirection;
    auto PathAdd = [this, &entityFactory] ( const std::vector<Vector2<int>>& path )
    {
        for( const auto& position : path )
        {
            if( Unoccupied( position ) )
            {
                EntityInsert( position, entityFactory.Get( "Hidden" )->Clone( ) );
            }
        }
    };

    for( const auto& entity : _entities )
    {
        if( entity->attributes & Attributes::Obstacle )
        {
            obstacles.push_back( entity->position );
        }
    }

    for( const auto& door : GetDoors( ) )
    {
        while( true )
        {
            const Vector2<int> random
            {
                GetRNG( 1, _grid.Size( ).x - 2 ),
                GetRNG( 1, _grid.Size( ).y - 2 )
            };

            if( Unoccupied( random ) )
            {
                redirection.emplace_back( door->position, random );

                break;
            }
        }
    }

    for( const auto& pair : redirection )
    {
        PathAdd( AStarAlgorithm( pair.first, pair.second, _grid.Size( ), obstacles ) );
        PathAdd( AStarAlgorithm( pair.second,     center, _grid.Size( ), obstacles ) );
    }
}
void Dungeon::GenerateWallsParents( const EntityFactory& entityFactory, int amount )
{
    int remaining = amount ? amount : ( _grid.Size( ).x * _grid.Size( ).y ) / 10;

    while( remaining > 0 )
    {
        const Vector2<int> position
        {
            GetRNG( 1, _grid.Size( ).x - 2 ),
            GetRNG( 1, _grid.Size( ).y - 2 )
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
    static const std::array<Vector2<int>, 4> directions
    { {
        {  0, -1 },
        {  1,  0 },
        {  0,  1 },
        { -1,  0 }
    } };
    int remaining = amount ? amount : ( _grid.Size( ).x * _grid.Size( ).y ) / 4;

    while( remaining > 0 )
    {
        for( int i = 0, limit = _entities.size( ); i < limit; i++ )
        {
            if( _entities[i]->attributes & Attributes::Obstacle )
            {
                const int index = GetRNG( 0, directions.size( ) - 1 );
                const Vector2<int> position = _entities[i]->position + directions[index];

                if( InBounds( position, _grid.Size( ) ) &&
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
    auto Surrounded = [this] ( const Vector2<int>& position, int threshold ) -> bool
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
            if( !( this->TileLacking( position + direction, Attributes::Obstacle ) ) )
            {
                count++;
            }
        }

        return count >= threshold;
    };

    for( int i = 0; i < limit; i++ )
    {
        for( iterator.y = 1; iterator.y < _grid.Size( ).y - 1; iterator.y++ )
        {
            for( iterator.x = 1; iterator.x < _grid.Size( ).x - 1; iterator.x++ )
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
    const std::vector<Entity*> enemies = entityFactory.Get( Attributes::Hostile | Attributes::Movement );
    const int limit = amount ? amount : ( _grid.Size( ).x * _grid.Size( ).y ) / 150;

    for( int i = 0; i < limit; i++ )
    {
        while( true )
        {
            const Vector2<int> position
            {
                GetRNG( 1, _grid.Size( ).x - 2 ),
                GetRNG( 1, _grid.Size( ).y - 2 )
            };

            if( Unoccupied( position ) )
            {
                const int index = GetRNG( 0, enemies.size( ) - 1 );

                EntityInsert( position, enemies[index]->Clone( ) );

                break;
            }
        }
    }
}