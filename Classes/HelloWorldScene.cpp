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
    _tileMap->addChild(_cat);
    
    
    auto winSize = Director::getInstance()->getWinSize();
    _bonesCount = Label::createWithSystemFont("Bones: 0", "Arial", 24);
    _bonesCount->setPosition(winSize.width * .1, winSize.height * .95);
    addChild(_bonesCount);
    
    // create a graph representation for our path finder.
    _graph = Graph::createWithScene(this);
    
    auto eventListener = EventListenerTouchOneByOne::create();
    eventListener->onTouchBegan = CC_CALLBACK_2(HelloWorldScene::onTouchBegan, this);
    eventListener->onTouchMoved = CC_CALLBACK_2(HelloWorldScene::onTouchMoved, this);
    eventListener->onTouchEnded = CC_CALLBACK_2(HelloWorldScene::onTouchEnded, this);
    eventListener->onTouchCancelled = CC_CALLBACK_2(HelloWorldScene::onTouchCancelled, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(eventListener, this);
    
    scheduleUpdate();
    
    // init vars
    _won = false;
    _gameOver = false;
    
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
    _tileMap->setPosition(viewPoint);
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
    _bonesCount->setString("Bones: " + std::to_string(numBones));
}

void HelloWorldScene::winGame()
{
    CCLOG("winGame()");
    
    _gameOver = true;
    _won = true;
    
    SimpleAudioEngine::getInstance()->playEffect("win.wav");
    endScene();
}

void HelloWorldScene::loseGame()
{
    CCLOG("loseGame()");
    
    _gameOver = true;
    _won = false;
    
    SimpleAudioEngine::getInstance()->playEffect("lose.wav");
    endScene();
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
    if (_gameOver)
        return false;
    
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

void HelloWorldScene::endScene()
{
    _cat->runAction(Sequence::create(
                                     ScaleBy::create(.5, 3.0),
                                     DelayTime::create(1.0),
                                     ScaleTo::create(.5, 0),
                                     CallFunc::create([this]() {
        this->showRestartMenu();
    }), nullptr));
    
    _cat->runAction(RepeatForever::create(RotateBy::create(.5, 360)));
}

void HelloWorldScene::showRestartMenu()
{
    std::string message;
    if (_won) {
        message = "You win!";
    } else {
        message = "You lose!";
    }
    
    auto label = Label::createWithSystemFont(message, "Arial", 24);
    label->setScale(.1);
    
    auto winSize = Director::getInstance()->getWinSize();
    label->setPosition(winSize.width/2, winSize.height * 0.6);
    addChild(label);
    
    auto restartLabel = Label::createWithSystemFont("Restart", "Arial", 24);
    
    auto restartItem = MenuItemLabel::create(restartLabel, CC_CALLBACK_1(HelloWorldScene::restartTapped, this));
    restartItem->setPosition(winSize.width/2, winSize.height * 0.4);
    restartItem->setScale(.1);
    
    auto menu = Menu::createWithItem(restartItem);
    menu->setPosition(Vec2::ZERO);
    addChild(menu, 10);
    
    restartItem->runAction(ScaleTo::create(.5, 1.0));
    label->runAction(ScaleTo::create(.5, 1.0));
}

void HelloWorldScene::restartTapped(cocos2d::Ref *sender)
{
    auto newScene = HelloWorldScene::createScene();
    Director::getInstance()->replaceScene(TransitionZoomFlipX::create(.5, newScene));
}


