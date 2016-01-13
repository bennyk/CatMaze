#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

using namespace cocos2d;

class CatSprite;

class HelloWorldScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorldScene);
    
    void setViewPointCenter(Vec2 position);
    Vec2 tileCoordForPosition(Vec2 position);
    Vec2 positionForTileCoord(Vec2 tileCoord);
    
    bool isValidTileCoord(Vec2 tileCoord);
    bool isWallAtTileCoord(Vec2 tileCoord);
    bool isBoneAtTilecoord(Vec2 tileCoord);
    bool isDogAtTilecoord(Vec2 tileCoord);
    bool isExitAtTilecoord(Vec2 tileCoord);
    
    void showNumBones(int numBones);
    
    void removeObjectAtTileCoord(Vec2 tileCoord);
    
    void winGame();
    void loseGame();
    
    virtual void update(float delta) override;
    
private:
    bool hasProperty(std::string name, Vec2 tileCoord, cocos2d::TMXLayer *layer);
    
    bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event);
    
private:
    TMXTiledMap *_tileMap;
    TMXLayer *_bgLayer, *_objectLayer;
    CatSprite *_cat;
};

#endif // __HELLOWORLD_SCENE_H__
