#include "Enemy1.h"

Enemy1::Enemy1(FileManager& fileMng, Stage* stage, float x, float y) : EnemyBase(fileMng, stage, x, y)
{
	SetImage("Resource/Image/WingBull.png");
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(2.0f, 0.0f);		//初期速度を設定
	hp_ = 1; // HPを1に設定
}

Enemy1::~Enemy1()
{
}

void Enemy1::Update()
{
	AddGravity();		//重力を加える
	Move();				//移動
}