#include"MainScene.h"
#include"SimpleAudioEngine.h"
USING_NS_CC;
using namespace CocosDenshion;
using namespace std;

MainSceneHub* MainScene::SceneHub = NULL;

Scene* MainScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MainScene::create();
	scene->addChild(layer);
	SceneHub = MainSceneHub::create();
	scene->addChild(SceneHub);
	return scene;
}
void MainScene::InitWayPoint()
{
	TMXObjectGroup *objects = CampusMap->getObjectGroup("object");
	int x = 0, y = 0;
	for (int i = 1; i <= 16; i++)
	{
		const char* str = CCString::createWithFormat("WayPoint%d", i)->getCString();
		auto temp = objects->getObject(str);
		x = temp["x"].asInt();
		y = temp["y"].asInt();
		auto way = WayPoint::create();
		way->value.x = x;
		way->value.y = y;
		CCLOG("x:%d,y:%d", x, y);
		WayPoints.pushBack(way);
	}
	auto way2 = WayPoint::create();
	way2->value.x = (OurTower->getPosition()).x;
	way2->value.y = (OurTower->getPosition()).y;
	WayPoints.pushBack(way2);
}


bool MainScene::init()
{
	if (!Layer::init())
	{
		return false;
	}
	
	SimpleAudioEngine::getInstance()->preloadEffect("Sound\\hurt.mp3");
	SimpleAudioEngine::getInstance()->preloadEffect("Sound\\hit.mp3");
	SimpleAudioEngine::getInstance()->playBackgroundMusic("Sound\\BackMusic.mp3",true);

	HeroHp = 10;
	count = 0;
	counttime = 0;
	CurMoney = 40;
    lock = 0;
	OurTowerHP = 60;
	EnemyTowerHp = 60;
	selectsche = 0;
	string file = "Map\\map.tmx";
	auto str = String::createWithContentsOfFile(FileUtils::getInstance()->fullPathForFilename(file.c_str()).c_str());
	CampusMap = TMXTiledMap::createWithXML(str->getCString(), "");
	BaseLayer = CampusMap->layerNamed("base");
	UpperLayer = CampusMap->layerNamed("upper");
	ScoreLayer = CampusMap->getLayer("score");
	
	BarrierLayer = CampusMap->getLayer("barrier");
	BarrierLayer->setVisible(false);


	TMXObjectGroup *objects = CampusMap->getObjectGroup("object");
	auto HeroBirth = objects->getObject("HeroBirth");
	int x = HeroBirth["x"].asInt();
	int y = HeroBirth["y"].asInt();
	Hero = Sprite::create("HeroSprite\\Hero_1.png");
	Hero->setPosition(x, y);
	NextRolePosition = Point(x, y);
	addChild(Hero);
	SetCenterCamera(Hero->getPosition());
	addChild(CampusMap, -1);

	auto otb = objects->getObject("OurTower");
	x = otb["x"].asInt();
	y = otb["y"].asInt();
	OurTower = Sprite::create("Tower\\MainTower.png");
	OurTower->setPosition(x, y);
	addChild(OurTower);

	auto etb = objects->getObject("EnemyTower");
	x = etb["x"].asInt();
	y = etb["y"].asInt();
	EnemyTower = Sprite::create("Tower\\MainTower.png");
	EnemyTower->setPosition(x, y);
	addChild(EnemyTower);
	Enemy* maintower = Enemy::create();
	maintower->sprite = EnemyTower;
	maintower->kinds = 5;
	maintower->CurHp = EnemyTowerHp;
	Enemies.pushBack(maintower);

	InitWayPoint();

	this->schedule(schedule_selector(MainScene::wave1), 7.0f);
	this->schedule(schedule_selector(MainScene::wave2), 8.0f);
	this->schedule(schedule_selector(MainScene::testCollisions));
	this->schedule(schedule_selector(MainScene::TowerLogic),0.3f);
	this->schedule(schedule_selector(MainScene::SetCountTime), 0.5f);
	this->scheduleUpdate();

	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = [&](Touch *touch, Event *unused_event)->bool { return true; };
	listener->onTouchEnded = CC_CALLBACK_2(MainScene::onTouchEnded, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
	
	this->setKeyboardEnabled(true);
	auto listenerkeyPad = EventListenerKeyboard::create();
	listenerkeyPad->onKeyPressed = [&](EventKeyboard::KeyCode keycode, Event * key_event)->bool {return true; };
	listenerkeyPad->onKeyReleased = CC_CALLBACK_2(MainScene::onKeyReleased, this);
	this->_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerkeyPad, this);
	

	return true;
}

void MainScene::SetCenterCamera(Point position)
{
	auto winSize = Director::getInstance()->getWinSize();

	int x = MAX(position.x, winSize.width / 2);
	int y = MAX(position.y, winSize.height / 2);
	x = MIN(x, (CampusMap->getMapSize().width * this->CampusMap->getTileSize().width) - winSize.width / 2);
	y = MIN(y, (CampusMap->getMapSize().height * CampusMap->getTileSize().height) - winSize.height / 2);
	auto actualPosition = Point(x, y);

	auto centerOfView = Point(winSize.width / 2, winSize.height / 2);
	auto viewPoint = centerOfView - actualPosition;
	this->setPosition(viewPoint);
}

void MainScene::setPlayerPosition(Point position)
{
	
	Point tileCoord = this->tileCoordForPosition(position);
	int tileGid = BarrierLayer->getTileGIDAt(tileCoord);
	if (tileGid) {
		auto properties = CampusMap->getPropertiesForGID(tileGid).asValueMap();
		if (!properties.empty()) {
			auto collision = properties["Collidable"].asString();
			if ("true" == collision) {
				return;
			}
			auto collectable = properties["Collectable"].asString();
			if ("true" == collectable) {
				BarrierLayer->removeTileAt(tileCoord);
				ScoreLayer->removeTileAt(tileCoord);
				//Hp+10
				HeroHp=HeroHp+10;
				this->SceneHub->numCollectedChanged(HeroHp,CurMoney,OurTowerHP,EnemyTowerHp);
			}
		}
	}

	Hero->setPosition(position);
}


bool MainScene::IsAbleToSetTowerPosition(Point pos)
{
	auto distance = Hero->getPosition() - pos;
	if (abs(distance.x) > 180 || abs(distance.y) > 180)return false;
	if (CurMoney < 10)return false;
	Point tileCoord = this->tileCoordForPosition(pos);
	int tileGid = BarrierLayer->getTileGIDAt(tileCoord);
	if (tileGid) {
		auto properties = CampusMap->getPropertiesForGID(tileGid).asValueMap();
		if (!properties.empty()) {
			auto collision = properties["Collidable"].asString();
			if ("true" == collision) {
				return false;
			}
		}
	}
	return true;
}


void MainScene::onTouchEnded(Touch *touch, Event *unused_event)
{
	if (count == 0)
	{
		auto touchLocation = touch->getLocation();
		touchLocation = this->convertToNodeSpace(touchLocation);
		NextRolePosition = touchLocation;
		Direction();
	}
	if(count==1)
	{
		auto touchLocation = touch->getLocation();
		touchLocation = this->convertToNodeSpace(touchLocation);

		auto projectile = Sprite::create("Bullet\\Bullet.png");
		projectile->setPosition(Hero->getPosition());
		this->addChild(projectile);

		int realX;
		auto diff = touchLocation - Hero->getPosition();
		if (diff.x > 0)
		{
			realX = (CampusMap->getMapSize().width * CampusMap->getTileSize().width) +
				(projectile->getContentSize().width / 2);
		}
		else {
			realX = -(CampusMap->getMapSize().width * CampusMap->getTileSize().width) -
				(projectile->getContentSize().width / 2);
		}
		float ratio = (float)diff.y / (float)diff.x;
		int realY = ((realX - projectile->getPosition().x) * ratio) + projectile->getPosition().y;
		auto realDest = Point(realX, realY);

		int offRealX = realX - projectile->getPosition().x;
		int offRealY = realY - projectile->getPosition().y;
		float length = sqrtf((offRealX*offRealX) + (offRealY*offRealY));
		float velocity = 480 / 1; 
		float realMoveDuration = length / velocity;

		auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(MainScene::projectileMoveFinished, this));
		projectile->runAction(Sequence::create(MoveTo::create(realMoveDuration, realDest), actionMoveDone, NULL));

		Projectiles.pushBack(projectile);
	}
	
	if (count == -1)
	{
		auto touchLocation = touch->getLocation();
		touchLocation = this->convertToNodeSpace(touchLocation);
		if (IsAbleToSetTowerPosition(touchLocation))
		{
			auto Tower = Tower::create();
			Tower->sprite = Sprite::create("Tower\\AttackTower.png");
			Tower->CurHp = 10;
			Tower->kinds = 0;
			Tower->sprite->setPosition(touchLocation);
			this->addChild(Tower->sprite);
			Towers.pushBack(Tower);
			CurMoney = CurMoney - 10;
			this->SceneHub->numCollectedChanged(HeroHp, CurMoney,OurTowerHP,EnemyTowerHp);
		}
		count = 0;
	}
	
}

bool MainScene::collisionWithCircle(cocos2d::Point circlePoint, float radius, cocos2d::Point circlePointTwo, float radiusTwo)
{
	float xdif = circlePoint.x - circlePointTwo.x;
	float ydif = circlePoint.y - circlePointTwo.y;
	float distance = sqrt(xdif * xdif + ydif * ydif);
	if (distance <= radius + radiusTwo)
	{
		return true;
	}
	return false;
}


void MainScene::update(float delta)
{
	auto playerPos = Hero->getPosition();
	if(collisionWithCircle(playerPos,1,NextRolePosition,1))
	{
		Hero->stopAllActions();
	}
	else
	{
		auto diff = NextRolePosition - playerPos;
		if (abs(diff.x) > abs(diff.y)) {
			if (diff.x > 0) {
				playerPos.x += CampusMap->getTileSize().width / 32;
			}
			else {
				playerPos.x -= CampusMap->getTileSize().width / 32;
			}
		}
		else {
			if (diff.y > 0) {
				playerPos.y += CampusMap->getTileSize().height / 32;
			}
			else {
				playerPos.y -= CampusMap->getTileSize().height / 32;
			}
		}

		if (playerPos.x <= (CampusMap->getMapSize().width * CampusMap->getMapSize().width) &&
			playerPos.y <= (CampusMap->getMapSize().height * CampusMap->getMapSize().height) &&
			playerPos.y >= 0 &&
			playerPos.x >= 0)
		{
			this->setPlayerPosition(playerPos);
		}
		
		
		this->SetCenterCamera(Hero->getPosition());
	}
}



Point MainScene::tileCoordForPosition(Point position)
{
	int x = position.x / CampusMap->getTileSize().width;
	int y = ((CampusMap->getMapSize().height * CampusMap->getTileSize().height) - position.y) / CampusMap->getTileSize().height;
	return Point(x, y);
}


void MainScene::Direction()
{
	auto diff2 = NextRolePosition - Hero->getPosition();
	if (abs(diff2.x) > abs(diff2.y)) {
		if (diff2.x > 0) {
			Hero->stopAllActions();
			Vector<SpriteFrame*> splashFrames;
			for (int i = 5; i <= 6; i++) {
				const char* str = CCString::createWithFormat("HeroSprite\\Hero_%d.png", i)->getCString();
				SpriteFrame* frame = SpriteFrame::create(str, Rect(0, 0, 30, 33));
				splashFrames.pushBack(frame);
			}
			splashAnim = Animation::createWithSpriteFrames(splashFrames, 0.15f);
			Hero->runAction(RepeatForever::create(Spawn::create(Animate::create(splashAnim), NULL)));


		}
		else {
			Hero->stopAllActions();
			Vector<SpriteFrame*> splashFrames;
			for (int i = 3; i <= 4; i++) {

				const char* str = CCString::createWithFormat("HeroSprite\\Hero_%d.png", i)->getCString();
				SpriteFrame* frame = SpriteFrame::create(str, Rect(0, 0, 30, 33));
				splashFrames.pushBack(frame);
			}
			splashAnim = Animation::createWithSpriteFrames(splashFrames, 0.15f);
			Hero->runAction(RepeatForever::create(Spawn::create(Animate::create(splashAnim), NULL)));

		}
	}
	else {
		if (diff2.y > 0) {
			Hero->stopAllActions();
			Vector<SpriteFrame*> splashFrames;
			for (int i = 7; i <= 8; i++) {

				const char* str = CCString::createWithFormat("HeroSprite\\Hero_%d.png", i)->getCString();
				SpriteFrame* frame = SpriteFrame::create(str, Rect(0, 0, 30, 33));
				splashFrames.pushBack(frame);
			}
			splashAnim = Animation::createWithSpriteFrames(splashFrames, 0.15f);
			Hero->runAction(RepeatForever::create(Spawn::create(Animate::create(splashAnim), NULL)));

		}
		else {
			Hero->stopAllActions();
			Vector<SpriteFrame*> splashFrames;
			for (int i = 1; i <= 2; i++) {

				const char* str = CCString::createWithFormat("HeroSprite\\Hero_%d.png", i)->getCString();
				SpriteFrame* frame = SpriteFrame::create(str, Rect(0, 0, 30, 33));
				splashFrames.pushBack(frame);
			}
			splashAnim = Animation::createWithSpriteFrames(splashFrames, 0.15f);
			Hero->runAction(RepeatForever::create(Spawn::create(Animate::create(splashAnim), NULL)));

		}
	}
}



void MainScene::AddWalkEnemyAtPos(Point pos,int i)
{
	const char* str = CCString::createWithFormat("Enemy\\Walk_%d.png", i)->getCString();
	auto enemy = Sprite::create(str);
	enemy->setPosition(pos);
	this->addChild(enemy);
	this->AnimateEnemyDefence(enemy);
	auto Ene = Enemy::create();
	Ene->sprite = enemy;
	Ene->CurHp = 15;
	Ene->kinds = 1;
	Enemies.pushBack(Ene);
	
	
}

void MainScene::AddFlyEnemyAtPos(Point pos, int i)
{
	const char* str = CCString::createWithFormat("Enemy\\Fly_%d.png", i)->getCString();
	auto enemy = Sprite::create(str);
	enemy->setPosition(pos);
	this->addChild(enemy);
	this->AnimateEnemyDefence(enemy);
	auto Ene = Enemy::create();
	Ene->sprite = enemy;
	Ene->CurHp = 30;
	Ene->kinds = 2;
	Enemies.pushBack(Ene);
	
}

void MainScene::AddAttackEnemyAtPos(Point pos, int i)
{
	const char* str = CCString::createWithFormat("Enemy\\Walk_%d.png", i)->getCString();
	auto enemy = Sprite::create(str);
	enemy->setPosition(pos);
	this->addChild(enemy);
	auto Ene = Enemy::create();
	Ene->sprite = enemy;
	Ene->CurHp = 10;
	Ene->kinds = 3;
	Ene->CurPoint = WayPoints.front()->value;
	Ene->NextPoint = WayPoints.front()->value;
	this->AnimateAttack(Ene);
	Enemies.pushBack(Ene);
}

void MainScene::AfterAnimateAttack(Object* pSender)
{
	Sprite* Senemy = (Sprite *)pSender;
	Enemy* enemy = NULL;
	for (Enemy* temp : Enemies)
	{
		if (temp->sprite == Senemy)
		{
			enemy = temp;
			break;
		}

	}

	this->AnimateAttack(enemy);
}


void MainScene::AnimateAttack(Enemy* enemy)
{
	if (Enemies.size() >= 30)
	{
		unschedule(schedule_selector(MainScene::wave1));
		unschedule(schedule_selector(MainScene::wave2));
	}
	if (Enemies.size() < 30 && !isScheduled(schedule_selector(MainScene::wave1))&&selectsche==1)
	{
		schedule(schedule_selector(MainScene::wave1),7.0f);
		selectsche = 0;
	}
	if (Enemies.size() < 30 && !isScheduled(schedule_selector(MainScene::wave2))&&selectsche==0)
	{
		schedule(schedule_selector(MainScene::wave2), 8.0f);
		selectsche = 1;
	}
	if (collisionWithCircle(enemy->NextPoint, 5, enemy->sprite->getPosition(), 5))
	{
		enemy->CurPoint = enemy->NextPoint;
		int flag = 0;
		for (WayPoint* temp : WayPoints)
		{
			if (temp->value == enemy->NextPoint)
			{
				flag++;
				continue;
			}
			
			if (flag != 0)
			{
				enemy->NextPoint = temp->value;
				flag = 0;
				break;
			}
		}
		
	}

	auto position = (enemy->NextPoint - enemy->sprite->getPosition());
	position.normalize();
	position = position * 10;
	auto actionMove = MoveBy::create(0.3f, position);
	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(MainScene::AfterAnimateAttack, this));
	enemy->sprite->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
		
}

void MainScene::wave1(float dt)
{
	TMXObjectGroup *objects = CampusMap->getObjectGroup("object");
	int x = 0, y = 0;
	int m = 1;
	for (auto& eSpawnPoint : objects->getObjects()) {
		ValueMap& dict = eSpawnPoint.asValueMap();
		if (dict["Enemy"].asInt() == 1) {
			x = dict["x"].asInt();
			y = dict["y"].asInt();
			this->AddAttackEnemyAtPos(Point(x, y), m);
			m++;
		}
	}
	m = 1;
}

void MainScene::wave2(float dt)
{
	TMXObjectGroup *objects = CampusMap->getObjectGroup("object");
	int x = 0, y = 0;
	int m = 1;
	for (auto& eSpawnPoint : objects->getObjects()) {
		ValueMap& dict = eSpawnPoint.asValueMap();
		if (dict["Enemy"].asInt() == 1) {
			x = dict["x"].asInt();
			y = dict["y"].asInt();
			this->AddWalkEnemyAtPos(Point(x, y), m);
			m++;
		}
	}
	m = 1;
	for (auto& eSpawnPoint : objects->getObjects()) {
		ValueMap& dict = eSpawnPoint.asValueMap();
		if (dict["Enemy"].asInt() == 2) {
			x = dict["x"].asInt();
			y = dict["y"].asInt();
			this->AddFlyEnemyAtPos(Point(x, y), m);
			m++;
		}
	}
	m = 1;

}



void MainScene::AfterEnemyDefence(Object* pSender)
{
	Sprite *enemy = (Sprite *)pSender;

	this->AnimateEnemyDefence(enemy);
}


void MainScene::AnimateEnemyDefence(Sprite * enemy)
{
	auto diffeh = enemy->getPosition() - Hero->getPosition();
	
	if (abs(diffeh.x) > 160 || abs(diffeh.y) > 160 )
	{
		auto position = (enemy->getPosition() - enemy->getPosition());
		position.normalize();
		position = position * 10;
		auto actionMove = MoveBy::create(0.3f, position);
		auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(MainScene::AfterEnemyDefence, this));
		enemy->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
	}
	else
	{
		auto position = (Hero->getPosition() - enemy->getPosition());
		position.normalize();
		position = position * 10;
		auto actionMove = MoveBy::create(0.3f, position);
		auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(MainScene::AfterEnemyDefence, this));
		enemy->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
	}
	
}


void MainScene::onKeyReleased(EventKeyboard::KeyCode keycode, Event * key_event)
{
	if (keycode == EventKeyboard::KeyCode::KEY_A)
	{
		count = 1;
	}
	if (keycode == EventKeyboard::KeyCode::KEY_S)
	{
		count = 0;
	}
	if (keycode == EventKeyboard::KeyCode::KEY_D)
	{
		count = -1;
	}
}

void MainScene::projectileMoveFinished(Object* bullet)
{
	Sprite * sprite = (Sprite*)bullet;
	Projectiles.eraseObject(sprite);
	this->removeChild(sprite);
}

void MainScene::testCollisions(float dt)
{
	while (lock != 0)
	{
		CCLOG("testCollisions:%d",lock);
	}
	lock = 1;
	for (Enemy *target : Enemies) {
		auto targetRect = Rect(
			target->sprite->getPosition().x -(target->sprite->getContentSize().width/2),
			target->sprite->getPosition().y -(target->sprite->getContentSize().height/2),
			target->sprite->getContentSize().width,
			target->sprite->getContentSize().height);
		
		if (targetRect.containsPoint(Hero->getPosition())&&counttime==0) {
			SimpleAudioEngine::getInstance()->playEffect("Sound\\hurt.mp3");
			if (target->kinds == 1)
			{
				this->HeroHp--;
				this->HeroHp--;
			}
			if (target->kinds == 2)
			{
				this->HeroHp--;
				this->HeroHp--;
				this->HeroHp--;
				this->HeroHp--;
			
			}
			if (target->kinds == 3)
			{
				this->HeroHp--;
			}
			if (this->HeroHp <= 0)lose();
			
			this->SceneHub->numCollectedChanged(HeroHp,CurMoney,OurTowerHP,EnemyTowerHp);
			counttime = 1;
		}
	}

	for (Enemy *target : Enemies) {
		auto targetRect = Rect(
			target->sprite->getPosition().x - (target->sprite->getContentSize().width / 2),
			target->sprite->getPosition().y - (target->sprite->getContentSize().height / 2),
			target->sprite->getContentSize().width,
			target->sprite->getContentSize().height);

		if (targetRect.containsPoint(OurTower->getPosition()) && counttime == 0) {
			SimpleAudioEngine::getInstance()->playEffect("Sound\\hurt.mp3");
			this->OurTowerHP--;	
			if (this->OurTowerHP <= 0)lose();
			this->SceneHub->numCollectedChanged(HeroHp, CurMoney, OurTowerHP, EnemyTowerHp);
			counttime = 1;
		}
	}

	Vector<Sprite*> projectilesToDelete;
	for (Sprite *projectile : Projectiles) {

		auto projectileRect = Rect(
			projectile->getPositionX() - projectile->getContentSize().width / 2,
			projectile->getPositionY() - projectile->getContentSize().height / 2,
			projectile->getContentSize().width,
			projectile->getContentSize().height);

		Vector<Enemy*> targetsToDelete;
		int counthit = 0;
		for (Enemy *target : Enemies) {
			auto targetRect = Rect(
				target->sprite->getPositionX() - target->sprite->getContentSize().width / 2,
				target->sprite->getPositionY() - target->sprite->getContentSize().height / 2,
				target->sprite->getContentSize().width,
				target->sprite->getContentSize().height);

			auto distance2 = target->sprite->getPosition() - Hero->getPosition();
			if (abs(distance2.x)>96 || abs(distance2.y)>96)continue;

			if (projectileRect.intersectsRect(targetRect)) {
				
				if (target->kinds == 5)
				{
					EnemyTowerHp--;
					(target->CurHp)--;
					if (EnemyTowerHp <= 0)win();
					this->SceneHub->numCollectedChanged(HeroHp, CurMoney, OurTowerHP, EnemyTowerHp);
				}
				else
				{
					(target->CurHp)--;
				}
				SimpleAudioEngine::getInstance()->playEffect("Sound\\hit.mp3");
				counthit++;
				if ((target->CurHp) <= 0)
				{
					targetsToDelete.pushBack(target);
					CurMoney = CurMoney + 5;
					this->SceneHub->numCollectedChanged(HeroHp, CurMoney,OurTowerHP,EnemyTowerHp);
				}
				
			}
		}
		for (Enemy *target : targetsToDelete) {
			Enemies.eraseObject(target);
			this->removeChild(target->sprite);
		}
		if (counthit > 0) {
			projectilesToDelete.pushBack(projectile);
		}
		auto distance = projectile->getPosition() - Hero->getPosition();
		if (abs(distance.x)>96 || abs(distance.y)>96)projectilesToDelete.pushBack(projectile);

		targetsToDelete.clear();
	}

	for (Sprite *projectile : projectilesToDelete) {
		Projectiles.eraseObject(projectile);
		this->removeChild(projectile);
	}
	projectilesToDelete.clear();


	Vector<Tower*> TowerToDelete;
	Vector<Bullet*> BulletsToDelete;
	for (Bullet *projectile : Bullets) {
		if (projectile->Producted == NULL)continue;
		if (projectile->Producted->CurHp <= 0) 
		{
			BulletsToDelete.pushBack(projectile);
			continue;
		}

		auto projectileRect = Rect(
			projectile->bullet->getPositionX() - projectile->bullet->getContentSize().width / 2,
			projectile->bullet->getPositionY() - projectile->bullet->getContentSize().height / 2,
			projectile->bullet->getContentSize().width,
			projectile->bullet->getContentSize().height);

		Vector<Enemy*> targetsToDelete;
		int counthit = 0;
		for (Enemy *target : Enemies) {
			auto targetRect = Rect(
				target->sprite->getPositionX() - target->sprite->getContentSize().width / 2,
				target->sprite->getPositionY() - target->sprite->getContentSize().height / 2,
				target->sprite->getContentSize().width,
				target->sprite->getContentSize().height);
			
			auto distance2 = target->sprite->getPosition() - projectile->Producted->sprite->getPosition();
			if (abs(distance2.x)>245 || abs(distance2.y)>245)continue;
		
			if (projectileRect.intersectsRect(targetRect)) {

				if (target->kinds == 5)
				{
					EnemyTowerHp--;
					(target->CurHp)--;
					if (EnemyTowerHp <= 0)win();
					this->SceneHub->numCollectedChanged(HeroHp, CurMoney, OurTowerHP, EnemyTowerHp);
				}
				else
				{
					(target->CurHp) = (target->CurHp) - 3;
				}

				projectile->Producted->CurHp--;
				SimpleAudioEngine::getInstance()->playEffect("Sound\\hit.mp3");
				counthit++;
				if ((target->CurHp) <= 0)
				{
					targetsToDelete.pushBack(target);
					if (target->kinds == 1)
					{
						CurMoney = CurMoney + 6;
					}
					if (target->kinds == 2)
					{
						CurMoney = CurMoney + 12;
					}
					if (target->kinds == 3)
					{
						CurMoney = CurMoney + 3;
					}
					this->SceneHub->numCollectedChanged(HeroHp, CurMoney,OurTowerHP,EnemyTowerHp);
				}

			}
		}
		for (Enemy *target : targetsToDelete) {
			Enemies.eraseObject(target);
			this->removeChild(target->sprite);
		}
		if (counthit > 0) {
			BulletsToDelete.pushBack(projectile);
		}
		if (projectile->Producted->CurHp <= 0)
		{
			TowerToDelete.pushBack(projectile->Producted);
		}
		auto distance = projectile->bullet->getPosition() - projectile->Producted->sprite->getPosition();
		if (abs(distance.x)>245 || abs(distance.y)>245)BulletsToDelete.pushBack(projectile);

		targetsToDelete.clear();
	}

	for (Bullet *projectile : BulletsToDelete) {
		Bullets.eraseObject(projectile);
		this->removeChild(projectile->bullet);
	}
	BulletsToDelete.clear();
		
		for (Tower* tower : TowerToDelete)
		{
			Towers.eraseObject(tower);
			this->removeChild(tower->sprite);
		}
		TowerToDelete.clear();
		lock = 0;
}

void MainScene::SetCountTime(float dt)
{
	counttime = 0;
}

void MainScene::TowerLogic(float dt)
{
	while (lock!=0)
	{
		CCLOG("TowerLogic:%d", lock);;
	}
	lock = 1;
	for (Tower * atower : Towers)
	{
		Vector<Enemy*> targetsToFire;
		for (Enemy* target : Enemies)
		{
			auto distance = atower->sprite->getPosition() - target->sprite->getPosition();
			if (abs(distance.x) < 245 && abs(distance.y) < 245)
				targetsToFire.pushBack(target);
		}

		if (targetsToFire.size() > 0)
		{
			for (Enemy* target : targetsToFire)
			{
				auto projectile = Sprite::create("Bullet\\Bullet.png");
				auto bullet = Bullet::create();
				bullet->bullet = projectile;
				bullet->Producted = atower;
				bullet->bullet->setPosition(bullet->Producted->sprite->getPosition());
				this->addChild(bullet->bullet);
				bullet->Producted->ToFire = target;
				int realX;
				auto diff = target->sprite->getPosition() - atower->sprite->getPosition();
				if (diff.x > 0)
				{
					realX = (CampusMap->getMapSize().width * CampusMap->getTileSize().width) +
						(bullet->bullet->getContentSize().width / 2);
				}
				else {
					realX = -(CampusMap->getMapSize().width * CampusMap->getTileSize().width) -
						(bullet->bullet->getContentSize().width / 2);
				}
				float ratio = (float)diff.y / (float)diff.x;
				int realY = ((realX - bullet->bullet->getPosition().x) * ratio) + bullet->bullet->getPosition().y;
				auto realDest = Point(realX, realY);

				int offRealX = realX - bullet->bullet->getPosition().x;
				int offRealY = realY - bullet->bullet->getPosition().y;
				float length = sqrtf((offRealX*offRealX) + (offRealY*offRealY));
				float velocity = 480 / 1; 
				float realMoveDuration = length / velocity;

				auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(MainScene::BulletMoveFinished, this));
				bullet->bullet->runAction(Sequence::create(MoveTo::create(realMoveDuration, realDest), actionMoveDone, NULL));
				Bullets.pushBack(bullet);
				break;
			}
		}
		targetsToFire.clear();
	}
	lock = 0;
}


void MainScene::BulletMoveFinished(Object* bullet)
{
	Bullet * sprite = (Bullet*)bullet;
	Bullets.eraseObject(sprite);
	this->removeChild(sprite->bullet);
}

void MainScene::win()
{
	auto scene = Win::createScene();
	Director::getInstance()->replaceScene(scene);
}
void MainScene::lose()
{
	auto scene = Lose::createScene();
	Director::getInstance()->replaceScene(scene);
}