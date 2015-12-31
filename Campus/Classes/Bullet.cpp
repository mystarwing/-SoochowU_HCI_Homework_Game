#include"Bullet.h"
USING_NS_CC;
bool Bullet::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	return true;
}

Bullet * Bullet::initWithBullet(Bullet* copy)
{
	return copy;
}

