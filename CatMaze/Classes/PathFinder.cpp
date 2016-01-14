//
//  PathFinder.cpp
//  CatMaze
//
//  Created by Benny Khoo on 12/01/2016.
//
//

#include "PathFinder.hpp"

using namespace cocos2d;

int PathFinder::find(Graph *g, Node start, Node end, std::function<void (Connection &conn)> block)
{
    return find(g, start, end, ManhattanDistance(end), block);
}

int PathFinder::find(Graph *g, Node start, Node end, std::function<int(Node &)> heuristic, std::function<void (Connection &conn)> block)
{
    bool verbose = true;
    
    NodeTrace startTrace (start,
        Connection::NONE,
        0, heuristic(start)
    );
    
    _open.push(startTrace);
    
    NodeTrace currentTrace;
    while (_open.size() > 0)
    {
        currentTrace = _open.lowestTrace();
        
        if (currentTrace._node == end) {
            break;
        }
        
        auto connections = g->getConnections(currentTrace._node);
        for (auto &connection : connections) {
            auto toNode = connection._to;
            auto toNodeCost = currentTrace._costSoFar + connection.getCost();
            
            int toNodeHeuristic;
            NodeTrace toNodeTrace;
            if (_closed.contains(toNode)) {
                auto iter = _closed.find(toNode);
                toNodeTrace = *iter;
                
                if (toNodeTrace._costSoFar <= toNodeCost) {
                    continue;
                }
                
                _closed.remove(toNode);
                
                toNodeHeuristic = toNodeTrace._estimatedTotalCost - toNodeTrace._costSoFar;
            }
            else if (_open.contains(toNode)) {
                auto iter = _open.find(toNode);
                toNodeTrace = *iter;

                if (toNodeTrace._costSoFar <= toNodeCost) {
                    continue;
                }
                
                toNodeHeuristic = toNodeTrace._estimatedTotalCost - toNodeTrace._costSoFar;
            }
            else {
                toNodeTrace._node = toNode;
                toNodeHeuristic = heuristic(toNode);
            }
            
            toNodeTrace._costSoFar = toNodeCost;
            toNodeTrace._connection = connection;
            toNodeTrace._estimatedTotalCost = toNodeCost + toNodeHeuristic;
            
            if (!_open.contains(toNode)) {
                _open.push(toNodeTrace);
            }
            
        }
        
        _open.remove(currentTrace._node);
        _closed.push(currentTrace);
        
    }
    
    if (currentTrace._node != end) {
        // not found
        
        return -1;
    }
    // compile the list
    
    std::stack<Connection> stack;
    auto currentNode = currentTrace._node;
    while (currentNode != start) {
//        CCLOG("%s", currentTrace._connection.getDesc().c_str());
        stack.push(currentTrace._connection);
        currentNode = currentTrace._connection._from;
        auto iter = _closed.find(currentNode);
        currentTrace = *iter;
    }
    
    while (stack.size()) {
        auto conn = stack.top();
        CCLOG("%s", conn.getDesc().c_str());
        block(conn);
        stack.pop();
    }
    
    return 0;
}