#include "Enemy3.h"


Enemy3::Enemy3(FileManager& fileMng, Stage* stage, float x, float y, ParticleManager& pMng) : EnemyBase(fileMng, stage, x, y, pMng)
{
	SetImage("Resource/Image/Enemys/Fantagy.png");
	enemyType_ = ENEMY_TYPE::E_TYPE_3;
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(2.0f, 0.0f);		//初期速度を設定
	hp_ = 1;						//HPを1に設定
	scale = 1.5f;
}

Enemy3::~Enemy3()
{
}

void Enemy3::Update()
{
	Move();					//移動
	AddGravity();			//重力を加える
	NoDamageCountDown();	//無敵時間のカウントダウン
}