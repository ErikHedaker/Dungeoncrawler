#include "AStarAlgorithm.h"
#include <unordered_map>
#include <queue>

ObstacleGrid::ObstacleGrid( const Vector2<int>& size, const std::vector<Vector2<int>>& obstacles ) :
    _size( size ),
    _obstacles( obstacles.begin( ), obstacles.end( ) )
{ }
bool ObstacleGrid::InBounds( const Vector2<int>& position ) const
{
    return
        position.x >= 0 &&
        position.y >= 0 &&
        position.x < _size.x &&
        position.y < _size.y;
}
bool ObstacleGrid::Passable( const Vector2<int>& position ) const
{
    return !_obstacles.count( position );
}
std::vector<Vector2<int>> ObstacleGrid::GetNeighbours( const Vector2<int>& position ) const
{
    const std::array<Vector2<int>, 4> directions =
    { {
        {  0, -1 },
        { -1,  0 },
        {  0,  1 },
        {  1,  0 }
    } };
    std::vector<Vector2<int>> result;

    for( const auto& direction : directions )
    {
        Vector2<int> neighbor = position + direction;

        if( InBounds( neighbor ) &&
            Passable( neighbor ) )
        {
            result.push_back( neighbor );
        }

        if( ( position.x + position.y ) % 2 == 0 )
        {
            /* Aesthetic improvement on square grids according to article author */
            std::reverse( result.begin( ), result.end( ) );
        }
    }

    return result;
}

Node::Node( const Vector2<int>& position, int priority ) :
    position( position ),
    priority( priority )
{ }

bool NodeCompare::operator()( const Node& lhs, const Node& rhs ) const
{
    return lhs.priority > rhs.priority;
}

int Heuristic( const Vector2<int>& start, const Vector2<int>& end )
{
    return abs( start.x - end.x ) + abs( start.y - end.y );
}

std::vector<Vector2<int>> AStarAlgorithm( const Vector2<int>& start, const Vector2<int>& end, const Vector2<int>& size, const std::vector<Vector2<int>>& obstacles )
{
    /*
        http://www.redblobgames.com/pathfinding/a-star/implementation.html
        My implementation of the A* algorithm is based on this article
    */

    const ObstacleGrid grid( size, obstacles );
    std::priority_queue<Node, std::vector<Node>, NodeCompare> nodesActive;
    std::unordered_map<Vector2<int>, Vector2<int>, HasherVector2<int>> positionCameFrom;
    std::unordered_map<Vector2<int>, int,          HasherVector2<int>> positionCost;
    std::vector<Vector2<int>> path { end };

    nodesActive.emplace( start, 0 );
    positionCameFrom[start] = start;
    positionCost[start] = 0;

    while( !nodesActive.empty( ) )
    {
        const Node current = nodesActive.top( );

        nodesActive.pop( );

        if( current.position == end )
        {
            break;
        }

        for( const auto& neighbour : grid.GetNeighbours( current.position ) )
        {
            const int newCost = positionCost[current.position] + 1;

            if( !positionCost.count( neighbour ) ||
                newCost < positionCost[neighbour] )
            {
                const int priority = newCost + Heuristic( neighbour, end );

                nodesActive.emplace( neighbour, priority );
                positionCameFrom[neighbour] = current.position;
                positionCost[neighbour] = newCost;
            }
        }
    }

    for( auto current = end; current != start; )
    {
        try
        {
            current = positionCameFrom.at( current );
            path.push_back( current );
        }
        catch( ... )
        {
            break;
        }
    }

    std::reverse( path.begin( ), path.end( ) );

    return path;
}