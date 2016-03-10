#pragma once

#include "Vector2.h"
#include <unordered_set>
#include <array>
#include <vector>

class Grid
{
    public:
        Grid( const Vector2<int>& size, const std::vector<Vector2<int>>& obstacles );

        bool InBounds( const Vector2<int>& position ) const;
        bool Passable( const Vector2<int>& position ) const;
        std::vector<Vector2<int>> GetNeighbors( const Vector2<int>& position ) const;

    private:
        const Vector2<int> _size;
        const std::unordered_set<Vector2<int>, HasherVector2<int>> _obstacles;
};

struct Node
{
    Node( const Vector2<int>& position, int priority );

    Vector2<int> position;
    int priority;
};

struct CompareNodes
{
    bool operator()( const Node& lhs, const Node& rhs ) const;
};

int Heuristic( const Vector2<int>& from, const Vector2<int>& to );

std::vector<Vector2<int>> AStarAlgorithm( const Vector2<int>& start, const Vector2<int>& goal, const Vector2<int>& sizeDungeon, const std::vector<Vector2<int>>& obstacles );