#include "Enemy2.h"

Enemy2::Enemy2(FileManager& fileMng, Stage* stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng) : EnemyBase(fileMng, stage, sceneGame, x, y, pMng)
{
	SetImage("Resource/Image/Enemys/Wingbull.png");
	enemyType_ = ENEMY_TYPE::E_TYPE_2;
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(2.0f, 0.0f);		//初期速度を設定
	hp_ = 1;						//HPを1に設定
	scale = 1.5f;
	jumpFlag = false;
}

Enemy2::~Enemy2()
{
}

void Enemy2::Update()
{
	Move();					//移動
	AddGravity();			//重力を加える
	NoDamageCountDown();	//無敵時間のカウントダウン
}

void Enemy2::Move()
{
	EnemyBase::Move();

	if (GetJumpFlag()) return;		//二段ジャンプを防止

	vy_ = -15.0f;		//ジャンプの初速度を設定
	jumpFlag = true;	//ジャンプフラグを立てる
}