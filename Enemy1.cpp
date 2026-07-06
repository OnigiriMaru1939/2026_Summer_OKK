#include "Enemy1.h"

Enemy1::Enemy1(FileManager& fileMng, Stage* stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng) : EnemyBase(fileMng, stage, sceneGame, x, y, pMng)
{
	SetImage("Resource/Image/Enemys/Hope.png");
	enemyType_ = ENEMY_TYPE::E_TYPE_1;
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(2.0f, 0.0f);		//初期速度を設定
	hp_ = 1;						//HPを1に設定
}

Enemy1::~Enemy1()
{
}

void Enemy1::Update()
{
	Move();				//移動
	AddGravity();		//重力を加える
	NoDamageCountDown();	//無敵時間のカウントダウン
}