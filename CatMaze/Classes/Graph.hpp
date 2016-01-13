//
//  Graph.hpp
//  CatMaze
//
//  Created by Benny Khoo on 12/01/2016.
//
//

#ifndef Graph_hpp
#define Graph_hpp

class HelloWorldScene;

class Graph
{
public:
    struct Node
    {
        cocos2d::Vec2 _loc;
        
        Node() : _loc(cocos2d::Vec2 {-1, -1}) {}
        Node(cocos2d::Vec2 loc) : _loc(loc) {}
        Node(const Node &rhs) : _loc(rhs._loc) {}
        
        bool operator==(const Node &rhs) const
        {
            return _loc == rhs._loc;
        }
        bool operator!=(const Node &rhs) const
        {
            return _loc != rhs._loc;
        }
        
        static const Node INVALID;
        
    };
    struct Connection
    {
        Node _from, _to;
        
        Connection() : _from {}, _to {} {}
        Connection(cocos2d::Vec2 p0, cocos2d::Vec2 p1) : _from(p0), _to(p1) {}
        Connection(const Connection &rhs) : _from(rhs._from), _to(rhs._to) {}
        
        float getCost() const
        {
            auto tt = _from._loc.getDistance(_to._loc);
            return tt;
        }
        
        static const Connection NONE;
    };
    
    
public:
    static std::shared_ptr<Graph> createWithScene(HelloWorldScene *scene);
    
    std::vector<Connection> getConnections(Node &node);
    
private:
    HelloWorldScene *_scene;
};

#endif /* Graph_hpp */
