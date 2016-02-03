#include <vector>

template<class T>
struct Edge
{
	T data;
	std::size_t indexNode;
};

template<class T>
struct Node
{
	T data;
	std::vector<std::size_t> indexEdgesIn;
	std::vector<std::size_t> indexEdgesOut;
};

template<class NodeType, class EdgeType>
struct Graph
{
	std::vector<Node<NodeType>> nodes;
	std::vector<Edge<EdgeType>> edges;
};