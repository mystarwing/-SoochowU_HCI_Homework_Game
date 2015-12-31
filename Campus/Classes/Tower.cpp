#include"Tower.h"
#include"MainScene.h"
bool Tower::init()
{
	if (!Sprite::init())
	{
		return false;
	}
	return true;
}

Tower* Tower::initWithTower(Tower* copy)
{
	return copy;
}

