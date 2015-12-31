#pragma once
#include"cocos2d.h"
#include"Tower.h"
USING_NS_CC;
class Bullet :public Sprite
{
public:	
	Sprite* bullet;
	Tower*  Producted;
	virtual bool init();
	Bullet* initWithBullet(Bullet* copy);
	CREATE_FUNC(Bullet);
};
