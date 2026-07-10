#include "Boss4.h"


Boss4::Boss4(FileManager& fileMng, Stage* stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng) : EnemyBase(fileMng, stage, sceneGame, x, y, pMng)
{
	SetImage("Resource/Image/Enemys/Cola.png");
	enemyType_ = ENEMY_TYPE::E_TYPE_BOSS_4;
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(-5.0f, 0.0f);		//初期速度を設定
	name_ = "Cola";              //名前を設定
	hp_ = 300;
	hpMax_ = 300;
	scale = 2.0f;
	jumpFlag = false;
	bossState_ = BOSS_STATE::NON;
	stateChangeTimer = 0;
	rotateSpeed = 0.0f;
}

Boss4::~Boss4()
{
}

void Boss4::Update()
{
	//ボス状態ステータス変化処理
	BossStateChange();

	//重力処理
	AddGravity();

	//出現処理
	if (isAppearing)
	{
		BossAppear();
		bossState_ = BOSS_STATE::MOVE;
		stateChangeTimer = 0;
		return;
	}

	switch (bossState_)
	{
		case Boss4::WAIT:
			Wait();
			break;
		case Boss4::MOVE:
			//横移動
			Move();
			break;
		case Boss4::JUMP:
			if (!GetJumpFlag())
			{
				Jump();
			}
			Move();
			break;
		case Boss4::DASH:
			Dash();
			break;
		default:
			break;
	}

	//無敵時間のカウントダウン
	NoDamageCountDown();
}

void Boss4::Draw() const
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
void Boss4::BossAppear()
{
	//落下だけ
	MoveY();
}

//ボス状態ステータス変化処理
void Boss4::BossStateChange()
{
	//ボス状態ステータスタイマーを進める
	stateChangeTimer++;

	switch (bossState_)
	{
		case BOSS_STATE::WAIT:

			if (stateChangeTimer > 90)
			{
				EnemyResetShake();
				SelectNextState();
			}
			break;

		case BOSS_STATE::MOVE:

			if (stateChangeTimer > 120)
			{
				bossState_ = BOSS_STATE::WAIT;
				stateChangeTimer = 0;
			}

			break;

		case BOSS_STATE::JUMP:

			if (stateChangeTimer > 240)
			{
				bossState_ = BOSS_STATE::MOVE;
				stateChangeTimer = 0;
			}

			break;

		case BOSS_STATE::DASH:

			if (stateChangeTimer > 180)
			{
				vx_ = (vx_ > 0) ? 5.0f : -5.0f;
				angle = 0.0f;

				bossState_ = BOSS_STATE::MOVE;
				stateChangeTimer = 0;
			}

			break;
	}
}

//次の行動パターンを決める関数
void Boss4::SelectNextState()
{
	int randomStateNum_ = rand() % (BOSS_STATE::MAX - 2);

	switch (randomStateNum_)
	{
		case 0:
			bossState_ = BOSS_STATE::JUMP;
			break;
		case 1:
			bossState_ = BOSS_STATE::DASH;
			break;
		default:
			break;
	}

	//ボス状態ステータスタイマーリセット
	stateChangeTimer = 0;
}

//ボス待機
void Boss4::Wait()
{
	MoveY();
	EnemyShake();
}

//ボスの移動処理
void Boss4::Move()
{
	EnemyBase::Move();		//基底クラスの移動処理を呼び出す
}

//ジャンプ処理
void Boss4::Jump()
{
	if (GetJumpFlag()) return;		//二段ジャンプを防止

	vy_ = -25.0f;		//ジャンプの初速度を設定
	jumpFlag = true;	//ジャンプフラグを立てる
}

//ダッシュ処理
void Boss4::Dash()
{
	rotateSpeed = 0.2f;

	angle += rotateSpeed;

	Move();

	//高速移動
	if (vx_ > 0)
	{
		vx_ = 20.0f;
	}
	else
	{
		vx_ = -20.0f;
	}
}