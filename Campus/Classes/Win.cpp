#pragma once
#pragma execution_character_set("utf-8")
#include "Win.h"
USING_NS_CC;

Scene* Win::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = Win::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool Win::init()
{
	if (!Layer::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto startItem = MenuItemImage::create(
		"GameOver\\again.png",
		"GameOver\\againselected.png",
		CC_CALLBACK_1(Win::menuNextCallback, this));

	startItem->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2 + startItem->getContentSize().height));
	auto menu = Menu::create(startItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto closeItem = MenuItemImage::create(
		"GameOver\\CloseNormal.png",
		"GameOver\\CloseSelected.png",
		CC_CALLBACK_1(Win::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height / 2 + startItem->getContentSize().height - startItem->getContentSize().height));

	auto menuexit = Menu::create(closeItem, NULL);
	menuexit->setPosition(Vec2::ZERO);
	this->addChild(menuexit, 1);


	auto label = Label::createWithTTF("You Win!", "fonts/Marker Felt.ttf", 50);
	label->setColor(Color3B::GREEN);
	label->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - label->getContentSize().height - label->getContentSize().height));
	this->addChild(label, 1);

	auto spritew = Sprite::create("GameOver\\win.png");
	spritew->setPosition(Vec2(origin.x + visibleSize.width / 2,
		origin.y + visibleSize.height - 4* (label->getContentSize().height)));

	this->addChild(spritew,1);


	auto author = Label::createWithTTF("weibo: PrtScroll", "fonts/arial.ttf", 30);
	author->setColor(Color3B::BLACK);
	author->setPosition(Vec2(origin.x + 3 * (visibleSize.width / 4), origin.y + visibleSize.height / 4));
	this->addChild(author, 1);

	auto sprite = Sprite::create("GameOver\\welcome.jpg");
	sprite->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	this->addChild(sprite, 0);

	return true;
}


void Win::menuNextCallback(Ref* pSender)
{
	auto scene = MainScene::createScene();
	Director::getInstance()->replaceScene(scene);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

void Win::menuCloseCallback(cocos2d::Ref* pSender)
{

	Director::getInstance()->end();
}
