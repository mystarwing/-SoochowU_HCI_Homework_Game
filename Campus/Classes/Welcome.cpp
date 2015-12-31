#include "Welcome.h"
USING_NS_CC;


Scene* Welcome::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = Welcome::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Welcome::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto startItem = MenuItemImage::create(
                                           "Welcome\\StartNormal.png",
                                           "Welcome\\StartSelected.png",
                                           CC_CALLBACK_1(Welcome::menuNextCallback, this));
    
	startItem->setPosition(Vec2(origin.x + visibleSize.width/2 ,
                                origin.y + visibleSize.height/2+ startItem->getContentSize().height));

    // create menu, it's an autorelease object
    auto menu = Menu::create(startItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);



	auto closeItem = MenuItemImage::create(
		"Welcome\\CloseNormal.png",
		"Welcome\\CloseSelected.png",
		CC_CALLBACK_1(Welcome::menuCloseCallback, this));

	closeItem->setPosition(Vec2(origin.x + visibleSize.width/2,
		origin.y + visibleSize.height / 2 + startItem->getContentSize().height-startItem->getContentSize().height));

	// create menu, it's an autorelease object
	auto menuexit = Menu::create(closeItem, NULL);
	menuexit->setPosition(Vec2::ZERO);
	this->addChild(menuexit, 1);


    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = Label::createWithTTF("Campus", "fonts/Marker Felt.ttf", 40);
	label->setColor(Color3B::BLACK);
    // position the label on the center of the screen
    label->setPosition(Vec2(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height -label->getContentSize().height- label->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(label, 1);


	auto author = Label::createWithTTF("weibo: PrtScroll", "fonts/arial.ttf", 30);
	author->setColor(Color3B::BLACK);
	author->setPosition(Vec2(origin.x + 3*(visibleSize.width / 4), origin.y + visibleSize.height/4));
	this->addChild(author, 1);




    // add "HelloWorld" splash screen"
    auto sprite = Sprite::create("Welcome\\welcome.jpg");

    // position the sprite on the center of the screen
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    // add the sprite as a child to this layer
    this->addChild(sprite, 0);
    
    return true;
}


void Welcome::menuNextCallback(Ref* pSender)
{
	auto scene = MainScene::createScene();
    Director::getInstance()->replaceScene(scene);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void Welcome::menuCloseCallback(cocos2d::Ref* pSender)
{

	Director::getInstance()->end();
}
