#pragma once
#include"cocos2d.h"
USING_NS_CC;

class Enemy :public Sprite
{
public:
	int CurHp;
	int kinds;
	Point CurPoint;
	Point NextPoint;
	Sprite* sprite;
	virtual bool init();
	Enemy* initWithEnemy(Enemy* copy);
	CREATE_FUNC(Enemy);
};