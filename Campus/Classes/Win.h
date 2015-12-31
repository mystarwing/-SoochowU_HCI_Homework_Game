#pragma once
#pragma execution_character_set("utf-8")
#include "cocos2d.h"
#include"MainScene.h"

class Win : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	void menuCloseCallback(cocos2d::Ref* pSender);
	void menuNextCallback(cocos2d::Ref* pSender);
	CREATE_FUNC(Win);
};


