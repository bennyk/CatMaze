//
//  PathFinder.hpp
//  CatMaze
//
//  Created by Benny Khoo on 12/01/2016.
//
//

#ifndef PathFinder_hpp
#define PathFinder_hpp

#include "Graph.hpp"

class PathFinder
{
    using Graph = Graph;
    using Node = Graph::Node;
    using Connection = Graph::Connection;
    
    class ManhattanDistance {
    public:
        ManhattanDistance(Node &target) : _target(target){}
        
        int operator()(Node &from) {
            return std::abs(_target._loc.x - from._loc.x) + std::abs(_target._loc.y - from._loc.y);
        }
    private:
        Node &_target;
    };
    
    struct NodeTrace {
        Node _node;
        Connection _connection;
        int _costSoFar, _estimatedTotalCost;
        
        NodeTrace() : _costSoFar(-1), _estimatedTotalCost(-1) {}
        
        NodeTrace(Node &node, const Connection &conn, int costSoFar, int estimatedTotalCost)
        : _node(node), _connection(conn), _costSoFar(costSoFar), _estimatedTotalCost(estimatedTotalCost) {}
        
        NodeTrace(const NodeTrace &rhs) : _node(rhs._node), _connection(rhs._connection), _costSoFar {0}, _estimatedTotalCost {0} {}
        
        int compare(NodeTrace &rhs) const
        {
            return _estimatedTotalCost - rhs._estimatedTotalCost;
        }
    };
    
    struct Compare {
        bool operator() (NodeTrace &a, NodeTrace &b)
        {
            return a.compare(b) > 0;
        }
    };
    
    class PriorityQueue : public std::priority_queue<NodeTrace, std::deque<NodeTrace>, Compare>
    {
    public:
        using Iterator = std::deque<NodeTrace>::iterator;

        Iterator find(Node &node)
        {
            auto first = c.begin();
            while (first!= c.end()) {
                if (first->_node == node) {
                    return first;
                }
                ++first;
            }
            return c.end();
        }
        
        bool contains(Node &node)
        {
            auto first = c.begin();
            auto last = c.end();
            while (first!=last) {
                if (first->_node == node)
                    return true;
                ++first;
            }
            
            return false;
        }
        
        void remove(Node &node)
        {
            auto iter = find(node);
            CC_ASSERT(iter != c.end());
            
            iter->_node = Node::INVALID;
        }
        
        const NodeTrace &lowestTrace()
        {
            while (!empty() && top()._node == Node::INVALID) {
                // pop to discard
                pop();
            }
            return top();
        }
        
    };
    
    
    int find(Graph &g, Node start, Node end, std::function<int(Node &)> heuristic);
    
private:
    
    PriorityQueue _open, _closed;
};

#endif /* PathFinder_hpp */
