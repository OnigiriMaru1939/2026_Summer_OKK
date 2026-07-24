#include "Boss4.h"
#include "SceneGame.h"
#include "Player.h"

Boss4::Boss4(FileManager& fileMng, Stage& stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng) : EnemyBase(fileMng, stage, sceneGame, x, y, pMng)
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
	chargeDashTimer = 0;
	chargeDashVX_ = 0.0f;
	chargeDashVY_ = 0.0f;
	stunFlag = false;
	stunTimer = 0;
	laserTimer = 0;
	laserAngle = 0.0f;
	laserFlag = false;
	lockLaserFlag = false;
	laserState_ = LASER_STATE::AIM;
	laserTimer = 0;
	laserDirX_ = 0.0f;
	laserDirY_ = 0.0f;
}

Boss4::~Boss4()
{
}

void Boss4::Update()
{
	//ボス状態ステータス変化処理
	BossStateChange();

	//重力処理
	if (bossState_ != BOSS_STATE::CHARGEDASH)
	{
		AddGravity();
	}

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
		case Boss4::CHARGEDASH:
			ChargeDash();
			break;
		case Boss4::STUN:
			Stun();
			break;
		case Boss4::LASER:
			LaserAttack();
			break;
		default:
			break;
	}

	//無敵時間のカウントダウン
	NoDamageCountDown();
}

void Boss4::Draw() const
{
	if (stunFlag)
	{
		if ((GetNowCount() / 100) % 2 == 0)
		{
			return;
		}

		SetDrawBright(255, 255, 0);
	}

	//レーザー描画
	int laserLength = 3000;

	int startX = static_cast<int>(x_) - stage_.GetScrollX();
	int startY = static_cast<int>(y_) - stage_.GetScrollY();

	//保存しておいたレーザー方向から角度を計算
	float laserAngle = atan2f(laserDirY_, laserDirX_);

	int endX = startX + static_cast<int>(cosf(laserAngle) * laserLength);
	int endY = startY + static_cast<int>(sinf(laserAngle) * laserLength);


	if (lockLaserFlag && laserState_ == LASER_STATE::AIM)
	{
		DrawLine(startX, startY, endX, endY, GetColor(255, 255, 0), 20);
	}

	if (lockLaserFlag && laserState_ == LASER_STATE::LOCK_ON)
	{
		if ((GetNowCount() / 50) % 2 == 0)
		{
			DrawLine(startX, startY, endX, endY, GetColor(255, 255, 0), 20);
		}
	}

	if (laserFlag)
	{
		DrawLine(startX, startY, endX, endY, GetColor(255, 0, 0), 50);
	}

	EnemyBase::Draw(); //基底クラスの描画処理を呼び出す

	SetDrawBright(255, 255, 255);
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
				angle_ = 0.0f;

				bossState_ = BOSS_STATE::MOVE;
				stateChangeTimer = 0;
			}

			break;

		case BOSS_STATE::CHARGEDASH:

			if (stateChangeTimer > 240)
			{
				bossState_ = BOSS_STATE::MOVE;
				stateChangeTimer = 0;
			}

			break;

		case BOSS_STATE::STUN:
			break;

		case BOSS_STATE::LASER:
			break;
	}
}

//次の行動パターンを決める関数
void Boss4::SelectNextState()
{
	int randomStateNum_ = rand() % 4;

	switch (randomStateNum_)
	{
		case 0:
			bossState_ = BOSS_STATE::JUMP;
			break;
		case 1:
			bossState_ = BOSS_STATE::DASH;
			break;
		case 2:
			bossState_ = BOSS_STATE::CHARGEDASH;
			//チャージダッシュ開始時に初期化
			chargeDashState_ = CHARGEDASH_STATE::AIM;
			chargeDashTimer = 0;

			//一旦停止
			vx_ = 0.0f;
			vy_ = 0.0f;

			break;
		case 3:
			bossState_ = BOSS_STATE::LASER;
			laserTimer = 0;

			vx_ = 0.0f;
			vy_ = 0.0f;
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

	angle_ += rotateSpeed;

	Move();
	Jump();

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

void Boss4::ChargeDash()
{
	chargeDashTimer++;

	switch (chargeDashState_)
	{
		case CHARGEDASH_STATE::AIM:
		{
			EnemyShake();

			vx_ = 0.0f;
			vy_ = 0.0f;

			Player* player = sceneGame_->GetPlayer();

			if (!player)
			{
				return;
			}

			//プレイヤー方向を計算
			float dx = player->GetWorldX() - x_;
			float dy = player->GetWorldY() - y_;

			float len = sqrtf(dx * dx + dy * dy);

			if (len > 0.0f)
			{
				dx /= len;
				dy /= len;

				//プレイヤーの方向を向く
				angle_ = atan2f(dy, dx) + DX_PI_F / 2.0f;

				//突撃方向を記録
				chargeDashVX_ = dx * 50.0f;
				chargeDashVY_ = dy * 50.0f;
			}

			if (chargeDashTimer >= 60)
			{
				chargeDashTimer = 0;

				//突撃開始
				vx_ = chargeDashVX_;
				vy_ = chargeDashVY_;

				chargeDashState_ = CHARGEDASH_STATE::DASH;
			}

			break;
		}

		case CHARGEDASH_STATE::DASH:

			int nextX = static_cast<int>(x_ + vx_);
			int nextY = static_cast<int>(y_ + vy_);

			if (stage_.CheckHitWallRect(
				nextX,
				nextY,
				static_cast<int>(width_ * scale),
				static_cast<int>(height_ * scale)))
			{
				//壁に激突
				vx_ = 0.0f;
				vy_ = 0.0f;

				stunFlag = true;
				stunTimer = 0;

				bossState_ = BOSS_STATE::STUN;
				stateChangeTimer = 0;

				return;
			}

			//壁に当たらなければ移動
			MoveX();
			MoveY();

			break;
	}
}

void Boss4::Stun()
{
	//動きを止める
	vx_ = 0.0f;
	MoveY();

	stunTimer++;

	if (stunTimer >= 120)
	{
		stunTimer = 0;
		stunFlag = false;
		angle_ = 0.0f;

		//通常状態へ
		vx_ = (vx_ >= 0.0f) ? 5.0f : -5.0f;
		bossState_ = BOSS_STATE::WAIT;
		stateChangeTimer = 0;
	}
}

void Boss4::LaserAttack()
{
	laserTimer++;

	Player* player = sceneGame_->GetPlayer();

	if (!player)
	{
		return;
	}

	switch (laserState_)
	{
		case LASER_STATE::AIM:
		{
			lockLaserFlag = true;

			vx_ = 0.0f;
			vy_ = 0.0f;

			//プレイヤーの方向を計算
			float dx = player->GetWorldX() - x_;
			float dy = player->GetWorldY() - y_;

			float len = sqrtf(dx * dx + dy * dy);

			if (len > 0.0f)
			{
				dx /= len;
				dy /= len;

				//プレイヤーを追いかけて照準
				laserDirX_ = dx;
				laserDirY_ = dy;

				//ボスの向きも変更
				angle_ = atan2f(dy, dx) + DX_PI_F / 2.0f;
			}

			if (laserTimer >= 120)
			{
				laserTimer = 0;

				//ここで照準を固定
				laserState_ = LASER_STATE::LOCK_ON;
			}

			break;
		}

		case LASER_STATE::LOCK_ON:
		{
			laserTimer++;

			if (laserTimer >= 120)
			{
				lockLaserFlag = false;
				laserFlag = true;

				laserTimer = 0;

				laserState_ = LASER_STATE::FIRE;
			}

			break;
		}

		case LASER_STATE::FIRE:
		{
			//レーザーとプレイヤーの当たり判定
			CheckLaserHit(*player);

			laserTimer++;

			//発射終了
			if (laserTimer >= 120)
			{
				laserTimer = 0;

				laserFlag = false;
				laserState_ = LASER_STATE::AIM;

				//通常移動速度に戻す
				vx_ = (vx_ >= 0.0f) ? 5.0f : -5.0f;
				angle_ = 0.0f;

				bossState_ = BOSS_STATE::WAIT;
				stateChangeTimer = 0;
			}

			break;
		}
	}
}

void Boss4::CheckLaserHit(Player& player)
{
	//レーザーの始点
	float startX = x_;
	float startY = y_;

	//レーザー方向
	float dirX = laserDirX_;
	float dirY = laserDirY_;

	//プレイヤー位置
	float playerX = player.GetWorldX();
	float playerY = player.GetWorldY();

	//レーザー始点からプレイヤーまでのベクトル
	float dx = playerX - startX;
	float dy = playerY - startY;

	//レーザー方向への射影
	float distance = dx * dirX + dy * dirY;

	//レーザーの後ろにいる場合は当たらない
	if (distance < 0.0f)
	{
		return;
	}

	//レーザー上の最も近い点
	float closestX = startX + dirX * distance;
	float closestY = startY + dirY * distance;

	//最近点とプレイヤーの距離
	float diffX = playerX - closestX;
	float diffY = playerY - closestY;

	float distanceSquared = diffX * diffX + diffY * diffY;

	//レーザーの太さ
	float laserRadius = 50.0f;

	//プレイヤーの半径
	float playerRadius = 32.0f;

	float hitDistance = laserRadius + playerRadius;

	if (distanceSquared <= hitDistance * hitDistance)
	{
		player.Damage(50.0f);
	}
}