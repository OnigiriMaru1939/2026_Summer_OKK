#include "Boss3.h"
#include "SceneGame.h"
#include "Player.h"



Boss3::Boss3(FileManager& fileMng, Stage* stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng) : EnemyBase(fileMng, stage, sceneGame, x, y, pMng)
{
	SetImage("Resource/Image/Enemys/Fantagy.png");
	enemyType_ = ENEMY_TYPE::E_TYPE_BOSS_3;
	SetPosition(x, y);				//初期位置を設定
	SetVelocity(-3.0f, 0.0f);		//初期速度を設定
	name_ = "Fantagy";              //名前を設定
	hp_ = 400;
	hpMax_ = 400;
	scale = 2.0f;
	jumpFlag = false;
	bossState_ = BOSS_STATE::NON;
	stateChangeTimer = 0;
}

Boss3::~Boss3()
{
}

void Boss3::Update()
{
	DrawFormatString(100, 100, GetColor(255, 0, 0), "Boss3 Update");
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
		case Boss3::NON:
			break;
		case Boss3::WAIT:
			Wait();
			break;
		case Boss3::MOVE:
			//横移動
			Move();
			break;
		case Boss3::SHOT:
			ShotAttack();
			break;
		default:
			break;
	}

	//無敵時間のカウントダウン
	NoDamageCountDown();
}

void Boss3::Draw() const
{
	EnemyBase::Draw();
}

//ボスの出現処理
void Boss3::BossAppear()
{
	//落下だけ
	MoveY();
}

//ボス状態ステータス変化処理
void Boss3::BossStateChange()
{
	//ボス状態ステータスタイマーを進める
	stateChangeTimer++;

	switch (bossState_)
	{
		case BOSS_STATE::WAIT:

			if (stateChangeTimer > 90)
			{
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

		case BOSS_STATE::SHOT:

			if (stateChangeTimer > 180)
			{
				bossState_ = BOSS_STATE::WAIT;
				stateChangeTimer = 0;
			}
			break;
	}
}

//次の行動パターンを決める関数
void Boss3::SelectNextState()
{
	int randomStateNum_ = rand() % 2;

	switch (randomStateNum_)
	{
		case 0:
			bossState_ = BOSS_STATE::MOVE;
			break;
		case 1:
			bossState_ = BOSS_STATE::SHOT;
			break;
		default:
			break;
	}

	//ボス状態ステータスタイマーリセット
	stateChangeTimer = 0;
}

//ボス待機
void Boss3::Wait()
{
	MoveY();
	EnemyShake();
}

//ボスの移動処理
void Boss3::Move()
{
	EnemyBase::Move();		//基底クラスの移動処理を呼び出す
}

void Boss3::ShotAttack()
{
	shotTimer++;

	if (shotTimer >= 0.1)
	{
		shotTimer = 0;

		Player* player = sceneGame_->GetPlayer();

		float dx = player->GetWorldX() - x_;
		float dy = player->GetWorldY() - y_;

		float len = sqrtf(dx * dx + dy * dy);

		if (len > 0.0f)
		{
			dx /= len;
			dy /= len;

			Shot(BulletBase::BULLET_TYPE::B_TYPE_1, dx, dy, 0.5f);
		}
	}
}