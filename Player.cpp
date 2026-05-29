#define NOMINMAX
#include "Player.h"
#include "math.h"
#include "InputManager.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "SoundFile.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include <DxLib.h>
#include <memory>
#include <algorithm>


constexpr auto WATER_PARTICLE_PATH = "Resource/ParticleJsonData/waterParameter.json";
Player::Player(FileManager& fileMng, Stage* stage) : fileManager(fileMng), stage_(stage)
{
	SetImage("Resource/Image/Monster.png");

	pMng = std::make_unique<ParticleManager>(fileMng);
	pMng->RegisterConfig(WATER_PARTICLE_PATH);

	sodaAttackSE = fileMng.LoadSoundFM("Resource/Sound/SE/Soda_SE.wav");
	sodaChargeSE = fileMng.LoadSoundFM("Resource/Sound/SE/Soda_Charge_SE.wav");

	InputManager::GetInstance().SetTriggerCallback(ActionID::Jump, [this]() { SpaceJump(); });
	InputManager::GetInstance().SetTriggerCallback(ActionID::SJump, [this]() { ClickSodaJump(); });
	 //マウス・スティック振り
	InputManager::GetInstance().SetAxisCallback(ActionID::Shake, [this]() { SodaShake(); });
	// 回転
	InputManager::GetInstance().SetPressCallback(ActionID::Rotate, [this]() { Rotate(); });

	//位置と物理の初期化
	posX = 200.0f;
	posY = 200.0f;
	canvasX = 0;
	canvasY = 0;
	gravity = 0.5f;
	velocityX = 0.0f;
	velocityY = 0.0f;
	shakeMove = 0.0f;
	shakeOffsetX = 0.0f;
	shakeOffsetY = 0.0f;
	angle = 0.0f;
	sodaRatio = 0.0f;
	rotateSpeed = 0.05f;
	jumpPower = 12.0f;

	//体力を初期化
	playerHpMax = 100.0f;
	playerHp = playerHpMax;

	//ゲージ等の初期化
	sodaGauge = 1000.0f;
	sodaGaugeMax = 1000.0f;
	sodaShakeGauge = 0.0f;
	playerShakePower = 0.0f;
	sodaPower = 20.0f;
	noDamageTime = 0;
	noDamageMaxTime = 180;
	attackTimer = 0;
	attackDamage = 20.0f;

	//フラグの初期化
	aliveFlag = true;
	jumpFlag = false;
	sodaAttackFlag = false;
}

Player::~Player()
{
	InputManager::GetInstance().StopVibration();
}

//画像読み込み
bool Player::SetImage(const std::string& path)
{
	auto img = fileManager.LoadImageFM(path);
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

void Player::SetVelocity(float vx, float vy)
{
	velocityX = vx; // x方向の速度を設定
	velocityY = vy; // y方向の速度を設定
}

void Player::Update()
{
	//無敵時間のカウントダウン
	if (noDamageTime > 0)
	{
		noDamageTime--;
	}

	//攻撃時間のカウントダウン
	if (attackTimer > 0)
	{
		attackTimer--;
	}
	else
	{
		sodaAttackFlag = false;
	}

	AddGravity();
	SodaMove();
	
	SodaGaugeCharge();
	/*SodaShake();*/
	//減衰（振らないと減る）
	sodaShakeGauge -= 10.0f;
	if (sodaShakeGauge < 0.0f)
	{
		sodaShakeGauge = 0;
		InputManager::GetInstance().StopVibration();
		sodaChargeSE->Stop();
	}
	else
	{
		float shakeRatio = sodaShakeGauge / SODA_SHAKE_GAUGE_MAX;
		int power = (int)(shakeRatio * 1000);
		InputManager::GetInstance().StartVibration(power, 100, 0);
		sodaChargeSE->SetVolume((int)(shakeRatio * 255));
		sodaChargeSE->PlayLoop();
	}
	if (auto emitter = sodaParticle.lock())
	{
		emitter->SetPosition(posX - sinf(angle) * width_, posY + cosf(angle) * height_);
	}
	else
	{
		//sodaParticle.reset();
	}
	pMng->UpdateAll();


	//プレイヤー画面スクロール処理
	canvasX = posX - stage_->GetScrollX();
	canvasY = posY - stage_->GetScrollY();
	if (canvasX < Stage::CHIP_SIZE * 10)
	{
		stage_->SetScrollX(static_cast<int>(posX) - Stage::CHIP_SIZE * 10);
		if (stage_->GetScrollX() < 0)
		{
			stage_->SetScrollX(0);
		}
	}
	if (canvasX > Stage::CHIP_SIZE * 38)
	{
		int newScrollX = static_cast<int>(posX) - Stage::CHIP_SIZE * 38;
		newScrollX = std::min(newScrollX, stage_->GetMaxScrollX());
		stage_->SetScrollX(newScrollX);
	}

	if (canvasY < Stage::CHIP_SIZE * 5)
	{
		stage_->SetScrollY(static_cast<int>(posY) - Stage::CHIP_SIZE * 5);
		if (stage_->GetScrollY() < 0)
		{
			stage_->SetScrollY(0);
		}
	}

	if (canvasY > Stage::CHIP_SIZE * 22)
	{
		int newScrollY = static_cast<int>(posY) - Stage::CHIP_SIZE * 22;
		newScrollY = std::min(newScrollY, stage_->GetMaxScrollY());
		stage_->SetScrollY(newScrollY);
	}
}

void Player::Draw()
{
	//DrawFormatString(1000, 1000, GetColor(255,0, 0), "HP %d", (int)playerHp);
	//プレイヤーが死んでいる又は画像が読み込まれていないときは表示しない
	if (!aliveFlag || !image_) return;

	pMng->DrawAll(stage_->GetScrollX(), stage_->GetScrollY());

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

	//ゲージの描画
	DrawGauge(20, 50, 500, 40, playerHp, playerHpMax, GetColor(0, 255, 0), 0);		//プレイヤーのHPゲージ
	//炭酸蓄積ゲージが0以上の場合のみ描画
	if (sodaShakeGauge > 0)
	{
		DrawGauge(static_cast<int>(canvasX) - 75, static_cast<int>(canvasY) - 50, 20, 100, sodaShakeGauge, SODA_SHAKE_GAUGE_MAX, GetColor(0, 0, 255), 1);		//炭酸蓄積ゲージ
	}
	DrawCircle(static_cast<int>(canvasX), static_cast<int>(canvasY), 3, 0X0000ff);
	// デバッグ
	DrawFormatString(1000, 1000, GetColor(255, 0, 0), "SodaGauge: %d", static_cast<int>(sodaShakeGauge));
	DrawFormatString(1000, 1020, GetColor(255, 0, 0), "noDamageTime: %d", static_cast<int>(noDamageTime));
	DrawFormatString(1000, 1040, GetColor(255, 0, 0), "sodaAttackFlag: %d", sodaAttackFlag);

	//プレイヤーの振動処理
	PlayerShake();

	//無敵時間中は点滅させる
	if (noDamageTime > 0)
	{
		if ((noDamageTime / 4) % 2 == 0)
		{
			return;
		}
	}

	//プレイヤー画像を描画(回転可)
	int handle = image_->GetHandle();
	DrawRotaGraph(
		(int)canvasX + (int)shakeOffsetX,
		(int)canvasY + (int)shakeOffsetY,
		1.0,
		angle,
		handle,
		TRUE
	);
}

//マウスを振ったり、スティックを動かすと炭酸ゲージが溜まる
void Player::SodaShake()
{
  	//距離
	float dist = InputManager::GetInstance().GetActionAxis(ActionID::Shake);
	shakeMove = dist;

	//ゲージ加算
	sodaShakeGauge += shakeMove * 0.25f;

	//上限
	if (sodaShakeGauge > SODA_SHAKE_GAUGE_MAX) sodaShakeGauge = SODA_SHAKE_GAUGE_MAX;
}

//炭酸残量ゲージを自動回復
void Player::SodaGaugeCharge()
{
	sodaGauge += 0.1f;
	//上限リミッター
	if (sodaGauge > 100.0f) sodaGauge = 100.0f;
}

//重力処理
void Player::AddGravity()
{
	velocityY += gravity;	//重力を速度に加算
	//posX += velocityX;		//速度を位置に加算
	//posY += velocityY;

	velocityX *= 0.98f;	//空気抵抗（X軸の速度を減衰）

	// 地面判定（仮）
	//if (posY > 500)
	//{
	//	posY = 500;
	//	velocityX = 0;
	//	velocityY = 0;
	//	jumpFlag = false; 
	//}
}

//ジャンプ処理・移動処理
void Player::SodaMove()
{
	//炭酸を吹き出したときにジャンプ・移動する処理
	int signX = (velocityX > 0) ? 1 : ((velocityX < 0) ? -1 : 0);
	int loopX = (int)std::abs(velocityX);
	while (loopX > 0)
	{
		if (WillCollide(static_cast<int>(posX + signX), static_cast<int>(posY)))
		{
			velocityX = 0;
			break;
		}
		posX += signX;
		loopX--;
	}

	int signY = (velocityY > 0) ? 1 : ((velocityY < 0) ? -1 : 0);
	int loopY = (int)std::abs(velocityY);
	while (loopY > 0)
	{
		if (WillCollide(static_cast<int>(posX), static_cast<int>(signY + posY)))
		{
			if (signY > 0)
			{
				jumpFlag = false;
			}
			velocityX = 0;
			velocityY = 0;
			break;
		}
		posY += signY;
		loopY--;
	}

}

void Player::SpaceJump()
{
	//二段ジャンプを防止
	if (GetJumpFlag()) return;

	//プレイヤーの向いている方向にジャンプ
	//DX_PI_F / 2はジャンプ方向を90度補正するための値
	velocityX = cos(angle - DX_PI_F / 2) * jumpPower;
	velocityY = sin(angle - DX_PI_F / 2) * jumpPower;
	jumpFlag = true;
}

//プレイヤー回転処理
void Player::Rotate()
{
	//回転速度
	angle += rotateSpeed * (InputManager::GetInstance().GetActionValue(ActionID::Rotate) + InputManager::GetInstance().GetActionAxis(ActionID::Rotate));
}

//衝突判定
bool Player::WillCollide(int newX, int newY)
{
	return stage_->CheckHitWallRect(
		newX,
		newY,
		width_,
		height_
	);
}

//炭酸攻撃処理
void Player::SodaAttack(float power)
{
	sodaAttackFlag = true;
	attackDamage = power;

	attackTimer = 30; //攻撃時間をリセット
}

//ダメージ処理
void Player::Damage(float dmg)
{
	//無敵時間中はダメージを受けない
	if (noDamageTime > 0)return;

	playerHp -= dmg;
	if (playerHp <= 0)
	{
		playerHp = 0;
		aliveFlag = false; //プレイヤーが死んだ
	}

	noDamageTime = noDamageMaxTime; //無敵時間をリセット
}

void Player::ClickSodaJump()
{
	

	//炭酸蓄積ゲージが0より大きい場合、炭酸蓄積ゲージを減らす
	if (sodaShakeGauge > 0)
	{
		sodaRatio = sodaShakeGauge / sodaGaugeMax;
		//炭酸蓄積ゲージの割合を炭酸攻撃の威力に変換
		sodaPower = sodaRatio * 20.0f;

		//プレイヤーが向いている方向に移動する
		velocityX += cos(angle - DX_PI_F / 2) * sodaPower;
		velocityY += sin(angle - DX_PI_F / 2) * sodaPower;

		//炭酸攻撃処理
		SodaAttack(sodaPower);
		sodaGauge -= 20.0f;
		sodaShakeGauge = 0;
		//下限リミッター
		if (sodaGauge < 0) sodaGauge = 0;
	}

	//ParticleConfig::acceleration = 0;
	const ParticleConfig* masterCfg = pMng->GetConfig(WATER_PARTICLE_PATH);
	if (masterCfg)
	{
		// ParticleConfig構造体のコピー
		ParticleConfig customCfg = *masterCfg;

		float plusAngle = angle * (180.0f / DX_PI_F);
		while (plusAngle < 0) plusAngle += 360.0f;
		while (plusAngle >= 360.0f) plusAngle -= 360.0f; // 0～360の範囲に収める
		customCfg.initDir += plusAngle;
		customCfg.startScale *= sodaRatio;
		customCfg.initSpeed *= sodaRatio;
		sodaParticle = pMng->PlayParticle(customCfg, posX - sinf(angle) * width_, posY + cosf(angle) * height_);

		sodaAttackSE->SetVolume((int)(sodaRatio * 255)); // 音量を炭酸ゲージの割合に応じて変化させる
		sodaAttackSE->PlayOneShot();
	}
}

//プレイヤーの振動処理
void Player::PlayerShake()
{
	playerShakePower = sodaShakeGauge / SODA_SHAKE_GAUGE_MAX;

	shakeOffsetX = (GetRand(10) - 5) * playerShakePower;
	shakeOffsetY = (GetRand(10) - 5) * playerShakePower;
}

//プレイヤーのノックバック処理
void Player::PlayerKnockBack(float enemyX, float power)
{
	//敵より右にいる
	if (posX > enemyX)
	{
		velocityX = power;
	}
	else
	{
		velocityX = -power;
	}
	//上方向へ吹っ飛ばす
	velocityY = -power;
}

//プレイヤーの矩形衝突判定
RECT Player::GetRect() const
{
	RECT rc;

	rc.left = (LONG)(posX - width_ / 2);
	rc.right = (LONG)(posX + width_ / 2);
	rc.top = (LONG)(posY - height_ / 2);
	rc.bottom = (LONG)(posY + height_ / 2);

	return rc;
}

//横ゲージの描画
void Player::DrawGauge(
	int x,
	int y,
	int width,
	int height,
	float value,
	float maxValue,
	int color,
	int mode)			//mode 0:横ゲージ、1:縦ゲージ
{
	//ゲージの枠
	DrawBox(x - 1,
			y - 1,
			x + width + 1,
			y + height + 1,
			GetColor(255, 0, 0),
			FALSE
	);

	if (mode == 0)
	{
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

	else if (mode == 1)
	{
		//縦ゲージ割合
		int barHeight = (int)((value / maxValue) * height);
		//中身
		DrawBox(
			x,
			y + height - barHeight,
			x + width,
			y + height,
			color,
			TRUE
		);
	}
}