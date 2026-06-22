#include "Boss2.h"


Boss2::Boss2(FileManager& fileMng, Stage* stage, float x, float y) : EnemyBase(fileMng, stage, x, y)
{
	SetImage("Resource/Image/Monster.png");
	enemyType_ = ENEMY_TYPE::E_TYPE_BOSS_2;
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(-1.0f, 0.0f);        //初期速度を設定
	name_ = "Monster";                //名前を設定
	hp_ = 200;
	hpMax_ = 200;
	scale = 2.0f;
	jumpFlag = false;
	bossState_ = BOSS_STATE::NON;
}

Boss2::~Boss2()
{
}

void Boss2::Update()
{
	//ボス状態ステータス変化処理
	BossStateChange();

	//重力処理
	AddGravity();

	//出現処理
	if (isAppearing)
	{
		BossAppear();
		return;
	}

	switch (bossState_)
	{
		case Boss2::NON:
			break;
		case Boss2::MOVE:
			//横移動
			BossMove();
			break;
		case Boss2::JUMP:
			break;
		case Boss2::DASH:
			break;
		default:
			break;
	}

	//無敵時間のカウントダウン
	NoDamageCountDown();
}

void Boss2::Draw() const
{
	EnemyBase::Draw(); // 基底クラスの描画処理を呼び出す

	//HPが0になると描画しない
	if (hp_ > 0)
	{
		//HPゲージの描画
		//DrawGauge(1300, 100, 500, 40, hp_, hpMax_, GetColor(255, 0, 0));		//ボスのHPゲージ
	}
}

//ボスの出現処理
void Boss2::BossAppear()
{
	//落下だけ
	MoveY();
}

//ボス状態ステータス変化処理
void Boss2::BossStateChange()
{
	//ボス状態ステータスタイマーを進める
	stateChangeTimer++;

	if (stateChangeTimer > 180)
	{

	}
}

//ボスの移動処理
void Boss2::BossMove()
{
	EnemyBase::Move();		//基底クラスの移動処理を呼び出す
	if (!GetJumpFlag())
	{
		Jump();					//ジャンプ処理を呼び出す
	}
}

//ジャンプ処理
void Boss2::Jump()
{
	if (GetJumpFlag()) return;		//二段ジャンプを防止

	vy_ = -10.0f;		//ジャンプの初速度を設定
	jumpFlag = true;	//ジャンプフラグを立てる
}

//ダッシュ処理
void Boss2::Dash()
{

}