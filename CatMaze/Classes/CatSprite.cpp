//
//  CatSprite.cpp
//  CatMaze
//
//  Created by Benny Khoo on 12/01/2016.
//
//

#include "CatSprite.hpp"

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
using namespace CocosDenshion;

CatSprite* CatSprite::createWithScene(HelloWorldScene *scene)
{
    CatSprite *ret = new (std::nothrow) CatSprite();
    if (ret && ret->initWithScene(scene))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool CatSprite::initWithScene(HelloWorldScene *scene)
{
    if (initWithSpriteFrameName("cat_forward_1.png")) {
        _scene = scene;
        _facingForwardAnimation = createCatAnimation("forward");
        _facingBackAnimation = createCatAnimation("back");
        _facingLeftAnimation = createCatAnimation("left");
        _facingRightAnimation = createCatAnimation("right");
        _numBones = 0;
        return true;
    }
    return false;
}

Animation *CatSprite::createCatAnimation(std::string name)
{
    auto animation = Animation::create();
    for (int i = 1; i <= 2; i++) {
        std::stringstream ss;
        ss << "cat_" << name << "_" << i << ".png";
        animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(ss.str()));
    }
    animation->setDelayPerUnit(.2);
    animation->retain();
    return animation;
}

void CatSprite::runAnimation(cocos2d::Animation *animation)
{
    if (_curAnimation == animation) return;
    _curAnimation = animation;
    
    if (_curAnimation != nullptr) {
        stopAction(_curAnimateAction);
    }
    
    _curAnimateAction = RepeatForever::create(Animate::create(animation));
    runAction(_curAnimateAction);
}

void CatSprite::moveToward(cocos2d::Vec2 target)
{
    Vec2 diff = target - getPosition();
    Vec2 desiredTileCoord = _scene->tileCoordForPosition(getPosition());
    
    if (fabsf(diff.x) > fabsf(diff.y)) {
        if (diff.x > 0) {
            desiredTileCoord.x += 1;
            runAnimation(_facingRightAnimation);
        } else {
            desiredTileCoord.x -= 1;
            runAnimation(_facingLeftAnimation);
        }
    } else {
        if (diff.y > 0) {
            desiredTileCoord.y -= 1;
            runAnimation(_facingBackAnimation);
        } else {
            desiredTileCoord.y += 1;
            runAnimation(_facingForwardAnimation);
        }
    }
    
    if (_scene->isWallAtTileCoord(desiredTileCoord)) {
        SimpleAudioEngine::getInstance()->playEffect("hitWall.wav");
    } else {
        setPosition(_scene->positionForTileCoord(desiredTileCoord));
    }
    
    if (_scene->isBoneAtTilecoord(desiredTileCoord)) {
        SimpleAudioEngine::getInstance()->playEffect("pickup.wav");
        _numBones++;
        _scene->showNumBones(_numBones);
        _scene->removeObjectAtTileCoord(desiredTileCoord);
    }
    else if (_scene->isDogAtTilecoord(desiredTileCoord)) {
        if (_numBones == 0) {
            _scene->loseGame();
        } else {
            _numBones--;
            _scene->showNumBones(_numBones);
            _scene->removeObjectAtTileCoord(desiredTileCoord);
            SimpleAudioEngine::getInstance()->playEffect("catAttack.wav");
        }
    }
    else if (_scene->isExitAtTilecoord(desiredTileCoord)) {
        _scene->winGame();
    } else {
        SimpleAudioEngine::getInstance()->playEffect("step.wav");
    }
}
