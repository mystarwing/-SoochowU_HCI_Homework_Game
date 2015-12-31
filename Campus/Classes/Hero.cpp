#include "Hero.h"


MySprite::MySprite() {

}

MySprite::~MySprite() {

}

CCRect MySprite::rect() {
	CCSize s = getTexture()->getContentSize();
	return CCRectMake(s.width / 2, -s.height / 2, s.width, s.height);
}

bool MySprite::initWithTexture(CCTexture2D* aTexture) {
	if (CCSprite::initWithTexture(aTexture))
	{
		return true;
	}
	return false;
}

MySprite *MySprite::create(CCTexture2D *pTexture) {
	MySprite *pobSprite = new MySprite();
	if (pobSprite && pobSprite->initWithTexture(pTexture))
	{
		pobSprite->autorelease();
		return pobSprite;
	}
	CC_SAFE_DELETE(pobSprite);
}

void MySprite::onEnter() {

	CCSprite::onEnter();
}

void MySprite::onExit() {
	CCSprite::onExit();
}

bool MySprite::containsTouchLocation(CCTouch* touch) {
	return true;
}

void MySprite::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {

}

void MySprite::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {

}

void MySprite::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {

}

CCObject* MySprite::copyWithZone(CCZone *pZone) {
	this->retain();
	return this;
}

void MySprite::touchDelegateRetain() {
	this->retain();
}

void MySprite::touchDelegateRelease() {
	this->release();
}

MySprite* paddleWithTexture(CCTexture2D* aTexture) {
	MySprite* pPaddle = new MySprite();
	pPaddle->initWithTexture(aTexture);
	pPaddle->autorelease();
	return pPaddle;
}
