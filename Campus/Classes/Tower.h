#pragma once
#include"cocos2d.h"
USING_NS_CC;

class Tower :public Sprite
{
public:
	int CurHp;//攻击一次减少一点
	int attack;//固定为3，主塔没有攻击力
	int kinds;//三种，我方主塔，防御塔，敌方主塔
	Sprite * sprite;
	Sprite* ToFire;
	virtual bool init();
	Tower* initWithTower(Tower* copy);
	CREATE_FUNC(Tower);
};
