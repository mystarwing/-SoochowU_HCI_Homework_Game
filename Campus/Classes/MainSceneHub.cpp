#include"MainSceneHub.h"
#pragma execution_character_set("utf-8")
USING_NS_CC;

bool MainSceneHub::init()
{
	if (!Layer::init())
	{
		return false;
	}

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	label = LabelTTF::create("当前 HP:10 Money:40 我方Tower:60 敌方Tower:60", "fonts/Microsoft Yahei Bold.ttf", 20.0f, Size(800, 20), TextHAlignment::LEFT);
	label->setColor(Color3B::BLACK);
	int margin = 10;
	label->setPosition(origin.x+label->getContentSize().width/2,
		origin.y+visibleSize.height-label->getContentSize().height);

	this->addChild(label);
	return true;
}

void MainSceneHub::numCollectedChanged(int numCollected,int money,int ourhp,int enemyhp)
{
	char showStr[100];
	sprintf(showStr, "当前 HP:%d Money:%d 我方Tower:%d 敌方Tower:%d", numCollected,money,ourhp,enemyhp);
	label->setString(showStr);
}
