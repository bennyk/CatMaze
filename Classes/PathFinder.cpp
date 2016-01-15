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
    bool verbose = false;
    
    if (verbose)
        CCLOG("start %s -> end %s", start.getDesc().c_str(), end.getDesc().c_str());
    
    NodeTrace startTrace (start,
        Connection::NONE,
        0, heuristic(start)
    );
    
    _open.push(startTrace);
    
    NodeTrace currentTrace;
    while (_open.size() > 0)
    {
        currentTrace = _open.lowestTrace();
        
        if (verbose)
            CCLOG("currentTrace %s", currentTrace.getDesc().c_str());
        
        if (currentTrace._node == end) {
            if (verbose)
                CCLOG("found goal node: %s", end.getDesc().c_str() );
            break;
        }
        
        auto connections = g->getConnections(currentTrace._node);
        for (auto &connection : connections) {
            auto toNode = connection._to;
            auto toNodeCost = currentTrace._costSoFar + connection.getCost();
            
            if (verbose)
                CCLOG("toNode %s toNodeCost %.2f", toNode.getDesc().c_str(), toNodeCost);
            
            int toNodeHeuristic;
            NodeTrace toNodeTrace;
            if (_closed.contains(toNode)) {
                
                if (verbose)
                    CCLOG("toNode is in CLOSED list");
                
                auto iter = _closed.find(toNode);
                toNodeTrace = *iter;
                
                if (verbose)
                    CCLOG("toNodeTrace %s", toNodeTrace.getDesc().c_str());
                
                if (toNodeTrace._costSoFar <= toNodeCost) {
                    if (verbose)
                        CCLOG("ignoring toNode coz costSoFar %d is lesser than toNodeCost %.2f", toNodeTrace._costSoFar, toNodeCost);
                    continue;
                }
                
                _closed.remove(toNode);
                
                toNodeHeuristic = toNodeTrace._estimatedTotalCost - toNodeTrace._costSoFar;
            }
            else if (_open.contains(toNode)) {
                if (verbose)
                    CCLOG("toNode is in OPEN list");
                
                auto iter = _open.find(toNode);
                toNodeTrace = *iter;
                
                if (verbose)
                    CCLOG("toNodeTrace %s", toNodeTrace.getDesc().c_str());
                
                if (toNodeTrace._costSoFar <= toNodeCost) {
                    if (verbose)
                        CCLOG("ignoring toNode coz costSoFar %d is lesser than toNodeCost %.2f", toNodeTrace._costSoFar, toNodeCost);
                    continue;
                }
                
                toNodeHeuristic = toNodeTrace._estimatedTotalCost - toNodeTrace._costSoFar;
            }
            else {
                if (verbose)
                    CCLOG("toNode is new");
                
                toNodeTrace._node = toNode;
                toNodeHeuristic = heuristic(toNode);
            }
            
            toNodeTrace._costSoFar = toNodeCost;
            toNodeTrace._connection = connection;
            toNodeTrace._estimatedTotalCost = toNodeCost + toNodeHeuristic;
            
            if (verbose)
                CCLOG("final toNodeTrace %s toNodeHeuristic %d", toNodeTrace.getDesc().c_str(), toNodeHeuristic);
            
            if (!_open.contains(toNode)) {
                if (verbose)
                    CCLOG("inserting trace to OPEN list");
                _open.push(toNodeTrace);
            }
            
        }
        
        _open.remove(currentTrace._node);
        _closed.push(currentTrace);
        
    }
    
    if (currentTrace._node != end) {
        // not found
        if (verbose)
            CCLOG("couldn't find goal node on the paths: %s", end.getDesc().c_str() );
        
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