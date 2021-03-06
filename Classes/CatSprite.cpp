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
        
        // init walking animator
        _walkingAnimator._sprite = this;
        _walkingAnimator.clear();
        
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
    if (_walkingAnimator.isBusy()) {
        CCLOG("Walking animator is busy. Stopping for good.");
        _walkingAnimator.stop();
        return;
    }
    auto fromTileCoord = _scene->tileCoordForPosition(getPosition());
    auto toTileCoord = _scene->tileCoordForPosition(target);
    if (fromTileCoord == toTileCoord) {
        CCLOG("you're already there!");
        return;
    }
    
    if (_scene->isWallAtTileCoord(toTileCoord)) {
        SimpleAudioEngine::getInstance()->playEffect("hitWall.wav");
        return;
    }
    
    _walkingAnimator.clear();
    _scene->findPath(fromTileCoord, toTileCoord, [this](Graph::Connection &conn) {
        _walkingAnimator.pushBack(conn);
    });
    
    _walkingAnimator.runAnimation([this] (Connection &conn) {
        
        auto toTileCoord = conn._to._loc;
        if (_scene->isWallAtTileCoord(toTileCoord)) {
            SimpleAudioEngine::getInstance()->playEffect("hitWall.wav");
        }
        
        if (_scene->isBoneAtTilecoord(toTileCoord)) {
            SimpleAudioEngine::getInstance()->playEffect("pickup.wav");
            _numBones++;
            _scene->showNumBones(_numBones);
            _scene->removeObjectAtTileCoord(toTileCoord);
        }
        else if (_scene->isDogAtTilecoord(toTileCoord)) {
            if (_numBones == 0) {
                _scene->loseGame();
                return false;
            } else {
                _numBones--;
                _scene->showNumBones(_numBones);
                _scene->removeObjectAtTileCoord(toTileCoord);
                SimpleAudioEngine::getInstance()->playEffect("catAttack.wav");
            }
        }
        else if (_scene->isExitAtTilecoord(toTileCoord)) {
            _scene->winGame();
        } else {
            SimpleAudioEngine::getInstance()->playEffect("step.wav");
        }
        
        return true;
    });
}

void CatSprite::moveToward2(cocos2d::Vec2 target)
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

// WalkingAnimator

void CatSprite::WalkingAnimator::runAnimation(std::function<bool (Connection &conn)> block)
{
    if (_stop) {
        CCLOG("Animator has been interrupted. Stopping...");
        return;
    }
    
    auto conn = _conns.front();
    _conns.pop_front();
    
    // update game logics for the current connection.
    if (!block(conn)) {
        return;
    }
    
    auto delta = conn._to._loc - conn._from._loc;
    if (delta.x > 0) {
        _sprite->runAnimation(_sprite->_facingRightAnimation);
    }
    else if (delta.x < 0) {
        _sprite->runAnimation(_sprite->_facingLeftAnimation);
    }
    
    if (delta.y > 0) {
        _sprite->runAnimation(_sprite->_facingForwardAnimation);
    }
    else if (delta.y < 0) {
        _sprite->runAnimation(_sprite->_facingBackAnimation);
    }
    
    Vec2 toLoc = conn._to._loc;
    auto move = MoveTo::create(.2, _sprite->_scene->positionForTileCoord(conn._to._loc));
    
    Vector<FiniteTimeAction *> seq;
    seq.pushBack(move);
    
    if (_conns.size() > 0) {
        seq.pushBack(CallFunc::create([this, block]() {
            this->runAnimation(block);
        }));
    }
    _sprite->runAction(Sequence::create(seq));
}

