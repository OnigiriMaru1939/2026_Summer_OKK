#include "Boss1.h"


Boss1::Boss1(FileManager& fileMng, Stage* stage, float x, float y) : EnemyBase(fileMng, stage, x, y)
{
	SetImage("Resource/Image/RedBull.png");
	enemyType_ = ENEMY_TYPE::E_TYPE_BOSS_1;
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(1.0f, 0.0f);		//初期速度を設定
	hp_ = 200;
	hpMax_ = 200;
	width_ = 128;
	height_ = 128;
	jumpFlag = false;
}

Boss1::~Boss1()
{
}

void Boss1::Update()
{
	AddGravity();			//重力を加える
	BossMove();				//移動
	NoDamageCountDown();	//無敵時間のカウントダウン
}

void Boss1::Draw() const
{
	EnemyBase::Draw(); // 基底クラスの描画処理を呼び出す

	//HPが0になると描画しない
	if (hp_ > 0)
	{
		//HPゲージの描画
		DrawGauge(1300, 100, 500, 40, hp_, hpMax_, GetColor(255, 0, 0));		//ボスのHPゲージ
	}
}

//ボスの移動処理
void Boss1::BossMove()
{
	EnemyBase::Move();		//基底クラスの移動処理を呼び出す
	if (!GetJumpFlag())
	{
		Jump();					//ジャンプ処理を呼び出す
	}
}

//ジャンプ処理
void Boss1::Jump()
{
	if (GetJumpFlag()) return;		//二段ジャンプを防止

	vy_ = -20.0f;		//ジャンプの初速度を設定
	jumpFlag = true;	//ジャンプフラグを立てる
}