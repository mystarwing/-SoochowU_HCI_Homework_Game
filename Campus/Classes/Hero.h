#pragma once
#include"cocos2d.h"
USING_NS_CC;
class MySprite : public CCSprite
{
public:
	MySprite();
	~MySprite();

public:

	CCRect rect();
	bool initWithTexture(CCTexture2D* aTexture);
	void onEnter();
	void onExit();
	bool containsTouchLocation(CCTouch* touch);
	void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);

	CCObject* copyWithZone(CCZone *pZone);
	void touchDelegateRetain();
	void touchDelegateRelease();

	static MySprite *paddleWithTexture(CCTexture2D* aTexture);
	static MySprite *create(CCTexture2D *pTexture);



}; //  end of MySprite

