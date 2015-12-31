#include"Enemy.h"
USING_NS_CC;

bool Enemy::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	return true;
}

Enemy* Enemy::initWithEnemy(Enemy* copy)
{
	return copy;
}