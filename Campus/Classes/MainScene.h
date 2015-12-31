#pragma once
#include"cocos2d.h"
#include"MainSceneHub.h"
#include"Enemy.h"
#include"Tower.h"
#include"Bullet.h"
#include"WayPoint.h"
#include"Win.h"
#include"Lose.h"
USING_NS_CC;

class MainScene :public cocos2d::Layer
{
public:
	static cocos2d::Scene * createScene();
	virtual bool init();
	CREATE_FUNC(MainScene);
    void SetCenterCamera(Point position);
	void setPlayerPosition(Point position);
	void onTouchEnded(Touch *touch, Event *unused_event);
	void onKeyReleased(EventKeyboard::KeyCode keycode, Event * key_event);
	virtual void update(float delta);
	bool collisionWithCircle(cocos2d::Point circlePoint, float radius, cocos2d::Point circlePointTwo, float radiusTwo);
	Point tileCoordForPosition(Point Position);
	void Direction();
	void AddWalkEnemyAtPos(Point pos,int i);
	void AddFlyEnemyAtPos(Point pos, int i);
	void AddAttackEnemyAtPos(Point pos, int i);
	void AfterEnemyDefence(Object* pSender);
	void AnimateEnemyDefence(Sprite* enemy);
	void projectileMoveFinished(Object* bullet);
	void testCollisions(float dt);
	void SetCountTime(float dt);
	void TowerLogic(float dt);
	bool IsAbleToSetTowerPosition(Point pos);
	void BulletMoveFinished(Object* bullet);
	void AnimateAttack(Enemy* enemy);
	void AfterAnimateAttack(Object* pSender);
	void wave1(float dt);
	void wave2(float dt);
	void InitWayPoint();
	void win();
	void lose();
private:
	TMXTiledMap *CampusMap;
	TMXLayer* BaseLayer;
	TMXLayer* UpperLayer;
	TMXLayer* BarrierLayer;
	TMXLayer* ScoreLayer;
	Sprite* Hero ;
	Sprite* OurTower;
	Sprite* EnemyTower;
	Point NextRolePosition;
	Animation *splashAnim;
	int OurTowerHP;
	int EnemyTowerHp;
	int HeroHp;
	int count;
	int counttime;
	int CurMoney;
	int lock;
	int selectsche;
   static MainSceneHub *SceneHub;
   Vector<Enemy *> Enemies;
   Vector<Sprite *> Projectiles;
   Vector<Tower *> Towers;
   Vector<Bullet*> Bullets;
   Vector<WayPoint*> WayPoints;
};