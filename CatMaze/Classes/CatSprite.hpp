//
//  CatSprite.hpp
//  CatMaze
//
//  Created by Benny Khoo on 12/01/2016.
//
//

#ifndef CatSprite_hpp
#define CatSprite_hpp

class HelloWorldScene;

class CatSprite : public cocos2d::Sprite
{
public:
    static CatSprite* createWithScene(HelloWorldScene *scene);
    void moveToward(cocos2d::Vec2 target);
    
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

};

#endif /* CatSprite_hpp */
