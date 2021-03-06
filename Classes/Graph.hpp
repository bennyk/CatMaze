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
        
        std::string getDesc() const
        {
            std::stringstream ss;
            ss << "{" << _loc.x << "," << _loc.y << "}";
            return ss.str();
        }
        
        static const Node INVALID;
        
    };
    struct Connection
    {
        Node _from, _to;
        float _baseCost;
        
        Connection() : _from {}, _to {}, _baseCost {0} {}
        Connection(cocos2d::Vec2 p0, cocos2d::Vec2 p1, float baseCost) : _from(p0), _to(p1), _baseCost(baseCost) {}
        Connection(const Connection &rhs) : _from(rhs._from), _to(rhs._to), _baseCost {rhs._baseCost} {}
        
        float getCost() const
        {
            auto tt = _from._loc.getDistance(_to._loc) + _baseCost;
            return tt;
        }
        
        std::string getDesc() const
        {
            std::stringstream ss;
            ss << _from.getDesc() << "->" << _to.getDesc();
            return ss.str();
        }
        
        static const Connection NONE;
    };
    
    
public:
    static std::shared_ptr<Graph> createWithScene(HelloWorldScene *scene);
    
    std::vector<Connection> getConnections(Node &node);
    
private:
    bool init(HelloWorldScene *scene);
    
private:
    HelloWorldScene *_scene;
};

#endif /* Graph_hpp */
