#pragma once
#include"cocos2d.h"
USING_NS_CC;

class WayPoint : public CCNode
{
public:
	Point value;
	virtual bool init();
	CREATE_FUNC(WayPoint);
};
