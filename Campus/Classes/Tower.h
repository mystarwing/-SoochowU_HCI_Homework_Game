#pragma once
#include"cocos2d.h"
USING_NS_CC;

class Tower :public Sprite
{
public:
	int CurHp;//����һ�μ���һ��
	int attack;//�̶�Ϊ3������û�й�����
	int kinds;//���֣��ҷ����������������з�����
	Sprite * sprite;
	Sprite* ToFire;
	virtual bool init();
	Tower* initWithTower(Tower* copy);
	CREATE_FUNC(Tower);
};
