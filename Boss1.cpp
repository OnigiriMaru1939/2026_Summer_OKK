#include "Boss1.h"


Boss1::Boss1(FileManager& fileMng, Stage* stage, float x, float y) : EnemyBase(fileMng, stage, x, y)
{
	SetImage("Resource/Image/RedBull.png");
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(0.0f, 0.0f);		//初期速度を設定
	hp_ = 100;						//HP
}

Boss1::~Boss1	()
{
}

void Boss1::Update()
{
	Move();					//移動
	AddGravity();			//重力を加える
	NoDamageCountDown();	//無敵時間のカウントダウン
}