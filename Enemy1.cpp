#include "Enemy1.h"

Enemy1::Enemy1(FileManager& fileMng, float x, float y) : EnemyBase(fileMng, x, y)
{
	SetImage("Resource/Image/WingBull.png");
	hp_ = 1; // HPを1に設定
}

Enemy1::~Enemy1()
{
}

void Enemy1::Update()
{
	
}