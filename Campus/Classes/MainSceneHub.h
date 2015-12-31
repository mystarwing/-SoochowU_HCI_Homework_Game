#pragma once
#pragma execution_character_set("utf-8")
#include"cocos2d.h"

class MainSceneHub: public cocos2d::Layer
{
public:
	void numCollectedChanged(int numCollected,int money,int othp,int ethp);
	virtual bool init();
	CREATE_FUNC(MainSceneHub);
	cocos2d::LabelTTF *label;
};
