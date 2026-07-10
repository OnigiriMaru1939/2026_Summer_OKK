#include "EnemyBase.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "SceneGame.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include <DxLib.h>

constexpr auto ENEMY_KILL_PARTICLE_PATH = "Resource/ParticleJsonData/killParameter.json";

void EnemyBase::SyncNetworkState(float x, float y, int hp, bool isAlive, int noDamageTime)
{
	x_ = x;
	y_ = y;
	hp_ = hp;
	isAlive_ = isAlive;
	noDamageTime_ = noDamageTime;
}

EnemyBase::EnemyBase(FileManager& fileMng, Stage* stage, SceneGame* sceneGame, float x, float y, ParticleManager& pMng)
	: fileManager_(fileMng)
	, stage_(stage)
	, sceneGame_(sceneGame)
	, particleManager(pMng)
	, x_(x)
	, y_(y)
	, vx_(0.0f)
	, vy_(0.0f)
	, canvasX(0)
	, canvasY(0)
	, shakeOffsetX(0)
	, shakeOffsetY(0)
	, scale(1.0f)
	, angle(0.0f)
	, gravity(0.5f)
	, AttckDamage(20)
	, noDamageTime_(0)
	, noDamageMaxTime(60)
	, width_(0)
	, height_(0)
	, hp_(1)
	, hpMax_(200)
	, isAlive_(true)
	, jumpFlag(false)
	, hitPlayerAlready_(false)
	, isAppearing(false)
	, noDamageFlag(false)
	, enemyType_(ENEMY_TYPE::E_TYPE_NON)
{
	particleManager.RegisterConfig(ENEMY_KILL_PARTICLE_PATH);
}

EnemyBase::~EnemyBase()
{
}

bool EnemyBase::SetImage(const std::string& path)
{
	auto img = fileManager_.LoadImageFM(path);
	if (!img)
	{
		image_.reset();
		width_ = height_ = 0;
		return false;
	}

	image_ = img;
	
	width_ = image_->GetWidth();
	height_ = image_->GetHeight();
	return true;
}

void EnemyBase::SetPosition(float x, float y)
{
	x_ = x; // x座標を設定
	y_ = y; // y座標を設定
}

void EnemyBase::SetVelocity(float vx, float vy)
{
	vx_ = vx; // x方向の速度を設定
	vy_ = vy; // y方向の速度を設定
}

void EnemyBase::Update()
{
	AddGravity();		//重力を加える
	Move();				//移動
}

void EnemyBase::Move()
{
	MoveX();
	MoveY();
}

void EnemyBase::MoveX()
{
	int signX = (vx_ > 0) ? 1 : ((vx_ < 0) ? -1 : 0);
	int loopX = (int)std::abs(vx_);
	while (loopX > 0)
	{
		if (WillCollide(x_ + signX, y_))
		{
			//壁に当たると反転
			vx_ = -vx_;
			break;
		}
		x_ += signX;
		loopX--;
	}
}

void EnemyBase::MoveY()
{
	int signY = (vy_ > 0) ? 1 : ((vy_ < 0) ? -1 : 0);
	int loopY = (int)std::abs(vy_);
	while (loopY > 0)
	{
		if (WillCollide(x_, signY + y_))
		{
			if (signY > 0)
			{
				jumpFlag = false;
			}
			vy_ = 0;
			break;
		}
		y_ += signY;
		loopY--;
	}
}

void EnemyBase::AddGravity()
{
	vy_ += gravity; //y方向の速度に重力を加算
}

//敵の弾を撃つ処理
void EnemyBase::Shot(BulletBase::BULLET_TYPE type, float dirX, float dirY, float scale)
{
	float shotSpeed = 8.0f;

	sceneGame_->AddEnemyShot(type, x_, y_, dirX * shotSpeed, dirY * shotSpeed, scale);
}

void EnemyBase::NoDamageCountDown()
{
	if (noDamageTime_ > 0)
	{
		noDamageTime_--;

		if (noDamageTime_ == 0)
		{
			noDamageFlag = false;
		}
	}
}

void EnemyBase::SetNoDamageFlag(bool flag)
{
	noDamageFlag = flag;

	if (!flag)
	{
		noDamageTime_ = 0;
	}
}

//敵の振動処理
void EnemyBase::EnemyShake()
{
	shakeOffsetX = (GetRand(20) - 10);
	shakeOffsetY = (GetRand(20) - 10);
}

//敵の振動リセット処理
void EnemyBase::EnemyResetShake()
{
	shakeOffsetX = 0;
	shakeOffsetY = 0;
}

void EnemyBase::ApplyDamage(int dmg)
{
	if (!isAlive_) 	return;				//生存していない場合はダメージを受けない
	if (GetNoDamageFlag()) return;		//無敵時間中はダメージを受けない

	hp_ -= dmg; // ダメージを適用
	if (hp_ <= 0)
	{
		isAlive_ = false;
		hp_ = 0;
	}

	//無敵時間開始
	noDamageTime_ = noDamageMaxTime;
	noDamageFlag = true;
}

bool EnemyBase::IsAlive() const
{
	
	return isAlive_; // 生存フラグを返す
}

//衝突判定
bool EnemyBase::WillCollide(int newX, int newY)
{
	return stage_->CheckHitWallRect(
		newX,
		newY,
		(int)(width_ * scale),
		(int)(height_ * scale)
	);
}

//敵の矩形衝突判定
RECT EnemyBase::GetRect() const
{
	RECT rect;
	int hitWidth = static_cast<int>(width_ * scale);
	int hitHeight = static_cast<int>(height_ * scale);

	rect.left = static_cast<LONG>(x_ - hitWidth / 2);
	rect.top = static_cast<LONG>(y_ - hitHeight / 2);
	rect.right = static_cast<LONG>(x_ + hitWidth / 2);
	rect.bottom = static_cast<LONG>(y_ + hitHeight / 2);

	return rect;
}

void EnemyBase::Draw() const
{
	if (!isAlive_ || !image_) return; // 生存していないか画像がない場合は描画しない

#ifdef _DEBUG
	//当たり判定の矩形を描画
	RECT rc = GetRect();

	DrawBox(
		rc.left - stage_->GetScrollX(),
		rc.top - stage_->GetScrollY(),
		rc.right - stage_->GetScrollX(),
		rc.bottom - stage_->GetScrollY(),
		GetColor(0, 255, 0),
		FALSE
	);
#endif
	//無敵時間中は点滅させる
	if (noDamageTime_ > 0)
	{
		if ((noDamageTime_ / 4) % 2 == 0)
		{
			return;
		}
	}

	int handle = 0;
	handle = image_->GetHandle(); //画像のハンドルを取得
	DrawRotaGraph(
		x_ - stage_->GetScrollX() + shakeOffsetX,
		y_ - stage_->GetScrollY() + shakeOffsetY,
		scale,
		angle,
		handle,
		TRUE
	);
}

//横ゲージの描画
void EnemyBase::DrawGauge(
	int x,
	int y,
	int width,
	int height,
	float value,
	float maxValue,
	int color) const
{
	//ゲージの枠
	DrawBox(x - 1,
			y - 1,
			x + width + 1,
			y + height + 1,
			GetColor(255, 0, 0),
			FALSE
	);

	//横ゲージ割合
	int barWidth = (int)((value / maxValue) * width);

	//中身
	DrawBox(
		x,
		y,
		x + barWidth,
		y + height,
		color,
		TRUE
	);
}

void EnemyBase::KillEffect()
{
	const ParticleConfig* masterCfg = particleManager.GetConfig(ENEMY_KILL_PARTICLE_PATH);
	if (masterCfg)
	{
		ParticleConfig customCfg = *masterCfg;
		float life = 20.0f;

		eKillParticle = particleManager.PlayParticle(customCfg, x_, y_, life);
	}
}