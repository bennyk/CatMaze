//
//  Graph.cpp
//  CatMaze
//
//  Created by Benny Khoo on 12/01/2016.
//
//

#include "Graph.hpp"

#include "HelloWorldScene.h"

using Connection = Graph::Connection;

const Connection Connection::NONE {Vec2{-1,-1}, Vec2{-1,-1}, 0};
const Graph::Node Graph::Node::INVALID {Vec2{-1,-1}};

std::shared_ptr<Graph>
Graph::createWithScene(HelloWorldScene *scene)
{
    auto result = std::make_shared<Graph>();
    if (result && result->init(scene))
    {
        return result;
    }
    CC_ASSERT(false);
}

bool
Graph::init(HelloWorldScene *scene)
{
    _scene = scene;
    return true;
}

std::vector<Connection>
Graph::getConnections(Node &node)
{
    std::vector<Connection> result;
    std::vector<Vec2> toLocs;
    
    toLocs.push_back(Vec2 {node._loc.x + 1, node._loc.y} );
    toLocs.push_back(Vec2 {node._loc.x, node._loc.y + 1} );
    toLocs.push_back(Vec2 {node._loc.x - 1, node._loc.y} );
    toLocs.push_back(Vec2 {node._loc.x, node._loc.y - 1} );
    
    for (Vec2 toLoc : toLocs) {
        if (_scene->isValidTileCoord(toLoc) && !_scene->isWallAtTileCoord(toLoc)) {
            // avoid the dog at all cost by biasing the base cost.
            float baseCost = _scene->isDogAtTilecoord(toLoc) ? 99.0 : 0.0;
            result.push_back(Connection(node._loc, toLoc, baseCost));
        }
    }
    
    return result;
}