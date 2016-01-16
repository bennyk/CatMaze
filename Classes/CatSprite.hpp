//
//  CatSprite.hpp
//  CatMaze
//
//  Created by Benny Khoo on 12/01/2016.
//
//

#ifndef CatSprite_hpp
#define CatSprite_hpp

#include "Graph.hpp"
#include "HelloWorldScene.h"

class CatSprite : public cocos2d::Sprite
{
    using Connection = Graph::Connection;
    
public:
    static CatSprite* createWithScene(HelloWorldScene *scene);
    void moveToward(cocos2d::Vec2 target);
    void moveToward2(cocos2d::Vec2 target);
    
private:
    bool initWithScene(HelloWorldScene *scene);
    
    static cocos2d::Animation *createCatAnimation(std::string name);
    
    void runAnimation(cocos2d::Animation *animation);
    
private:
    HelloWorldScene *_scene;
    
    cocos2d::Animation *_facingForwardAnimation,
    *_facingBackAnimation,
    *_facingLeftAnimation,
    *_facingRightAnimation,
    *_curAnimation;
    
    cocos2d::ActionInterval *_curAnimateAction;

    int _numBones;
    
private:
    struct WalkingAnimator
    {
        CatSprite *_sprite;
        std::list<Connection> _conns;
        bool _stop;
                
        void runAnimation(std::function<bool (Connection &conn)> block);
        void pushBack(Connection &conn) { _conns.push_back(conn); }
        void clear() {
            _conns.clear();
            _stop = false;
        }
        bool isBusy() { return _conns.size() > 0 && !_stop; }
        void stop() { _stop = true; }
    };
    
    WalkingAnimator _walkingAnimator;

};

#endif /* CatSprite_hpp */
