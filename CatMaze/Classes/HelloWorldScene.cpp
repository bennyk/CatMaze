#include "HelloWorldScene.h"

#include "CatSprite.hpp"
#include "PathFinder.hpp"

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

USING_NS_CC;

Scene* HelloWorldScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorldScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorldScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    

    _tileMap = TMXTiledMap::create("CatMaze.tmx");
    _tileMap->retain();
    addChild(_tileMap);
    
    Vec2 spawnTileCoord = Vec2 {24, 0};
    Vec2 spawnPos = positionForTileCoord(spawnTileCoord);
    setViewPointCenter(spawnPos);
    
    auto audio = SimpleAudioEngine::getInstance();
    audio->playBackgroundMusic("SuddenDefeat.mp3", true);
    audio->preloadEffect("pickup.wav");
    audio->preloadEffect("catAttack.wav");
    audio->preloadEffect("hitWall.wav");
    audio->preloadEffect("lose.wav");
    audio->preloadEffect("step.wav");
    audio->preloadEffect("win.wav");
    
    _bgLayer = _tileMap->getLayer("Background");
    _objectLayer = _tileMap->getLayer("Objects");
    
    auto cache = SpriteFrameCache::getInstance();
    cache->addSpriteFramesWithFile("CatMaze.plist", "CatMaze.png");
    
    _cat = CatSprite::createWithScene(this);
    _cat->retain();
    _cat->setPosition(spawnPos);
    addChild(_cat);
    
    // create a graph representation for our path finder.
    _graph = Graph::createWithScene(this);
    
    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorldScene::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(HelloWorldScene::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(HelloWorldScene::onTouchEnded, this);
    eventListener->onTouchCancelled = CC_CALLBACK_2(HelloWorldScene::onTouchCancelled, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    scheduleUpdate();
    
    return true;
}

void HelloWorldScene::setViewPointCenter(Vec2 position)
{
    auto winSize = Director::getInstance()->getWinSize();
    
    float x = MAX(position.x, winSize.width/2);
    float y = MAX(position.y, winSize.height/2);
    x = MIN(x, (_tileMap->getMapSize().width * _tileMap->getTileSize().width) - winSize.width / 2);
    y = MIN(y, (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - winSize.height/2);
    
    auto actualPosition = Vec2 {x, y};
    auto centerOfView = Vec2(winSize.width/2, winSize.height/2);
    Vec2 viewPoint = centerOfView - actualPosition;
    setPosition(viewPoint);
}

Vec2 HelloWorldScene::tileCoordForPosition(cocos2d::Vec2 position)
{
    int x = position.x / _tileMap->getTileSize().width;
    int y = ((_tileMap->getMapSize().height * _tileMap->getTileSize().height) - position.y) / _tileMap->getTileSize().height;
    return Vec2(x, y);
}

Vec2 HelloWorldScene::positionForTileCoord(cocos2d::Vec2 tileCoord)
{
    int x = (tileCoord.x * _tileMap->getTileSize().width) + _tileMap->getTileSize().width/2;
    int y = (_tileMap->getMapSize().height * _tileMap->getTileSize().height) - (tileCoord.y * _tileMap->getTileSize().height) - _tileMap->getTileSize().height/2;
    return Vec2(x, y);
}

bool HelloWorldScene::isValidTileCoord(cocos2d::Vec2 tileCoord)
{
    if (tileCoord.x < 0 || tileCoord.y < 0 ||
        tileCoord.x >= _tileMap->getMapSize().width ||
        tileCoord.y >= _tileMap->getMapSize().height) {
        return false;
    } else {
        return true ;
    }
}

bool HelloWorldScene::isWallAtTileCoord(cocos2d::Vec2 tileCoord)
{
    return hasProperty("Wall", tileCoord, _bgLayer);
}

bool HelloWorldScene::isBoneAtTilecoord(cocos2d::Vec2 tileCoord)
{
    return hasProperty("Bone", tileCoord, _objectLayer);
}

bool HelloWorldScene::isDogAtTilecoord(cocos2d::Vec2 tileCoord)
{
    return hasProperty("Dog", tileCoord, _objectLayer);
}

bool HelloWorldScene::isExitAtTilecoord(cocos2d::Vec2 tileCoord)
{
    return hasProperty("Exit", tileCoord, _objectLayer);
}

bool HelloWorldScene::hasProperty(std::string name, cocos2d::Vec2 tileCoord, cocos2d::TMXLayer *layer)
{
    if (!isValidTileCoord(tileCoord))
        return false;
    
    int gid = layer->getTileGIDAt(tileCoord);
    Value properties = _tileMap->getPropertiesForGID(gid);
    if (properties.getType() != Value::Type::NONE) {
        auto valueMap = properties.asValueMap();
        auto v = valueMap.find(name);
        return v != valueMap.end() ? true : false;
    }
    return false;
}

void HelloWorldScene::showNumBones(int numBones)
{
    CCLOG("showNumBones(%d)", numBones);
}

void HelloWorldScene::winGame()
{
    CCLOG("winGame()");
}

void HelloWorldScene::loseGame()
{
    CCLOG("loseGame()");
}

void HelloWorldScene::findPath(cocos2d::Vec2 startTileCoord, cocos2d::Vec2 toTileCoord, std::function<void (Graph::Connection &conn)> block)
{
    PathFinder finder;
    finder.find(_graph.get(), startTileCoord, toTileCoord, block);
}

void HelloWorldScene::removeObjectAtTileCoord(cocos2d::Vec2 tileCoord)
{
    _objectLayer->removeTileAt(tileCoord);
}

bool HelloWorldScene::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
//    if (_gameOver) return NO;
    
    Vec2 touchLocation = _tileMap->convertTouchToNodeSpace(touch);
    _cat->moveToward(touchLocation);
    return true;
}

void HelloWorldScene::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
}

void HelloWorldScene::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
}

void HelloWorldScene::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *unused_event)
{
    
}

void HelloWorldScene::update(float delta)
{
    setViewPointCenter(_cat->getPosition());
}

void HelloWorldScene::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

