#define NOMINMAX
#include "Player.h"
#include "math.h"
#include "InputManager.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "ParticleManager.h"
#include <DxLib.h>
#include <memory>
#include <algorithm>


constexpr auto WATER_PARTICLE_PATH = "Resource/ParticleJsonData/waterParameter.json";
Player::Player(FileManager& fileMng, Stage* stage) : fileManager(fileMng), stage_(stage)
{
	pMng = std::make_unique<ParticleManager>(fileMng);
	pMng->RegisterConfig(WATER_PARTICLE_PATH);

	InputManager::GetInstance().SetTriggerCallback(ActionID::Jump, [this]() { SpaceJump(); });
	InputManager::GetInstance().SetTriggerCallback(ActionID::SJump, [this]() { ClickSodaJump(); });
	InputManager::GetInstance().SetTriggerCallback(ActionID::Shake, [this]() { SodaShake(); });
	InputManager::GetInstance().SetPressCallback(ActionID::Rotate, [this]() { Rotate(); });

}

Player::~Player()
{

}

//初期化
bool Player::SystemInit()
{
	//位置と物理の初期化
	posX = 200.0f;
	posY = 200.0f;
	gravity = 0.5f;
	velocityX = 0.0f;
	velocityY = 0.0f;
	shakeMove = 0.0f;

	//体力を初期化
	playerHpMax = 100.0f;
	playerHp = playerHpMax;

	//ゲージ等の初期化
	sodaGauge = 100.0f;
	sodaGaugeMax = 100.0f;
	sodaShakeGauge = 0.0f;
	sodaShakeGaugeMax = 100.0f;
	playerShakePower = 0.0f;

	//フラグの初期化
	aliveFlag = true;
	jumpFlag = false;
	angle = 0.0f;

	return true;
}

//画像読み込み
bool Player::SetImage(const std::string& path)
{
	auto img = fileManager.LoadImageFM(path);
	if (!img)
	{
		printfDx("画像読み込み失敗: %s\n", path.c_str());

		image_.reset();
		width_ = height_ = 0;
		return false;
	}

	printfDx("画像読み込み成功\n");

	image_ = img;

	width_ = image_->GetWidth();
	height_ = image_->GetHeight();

	return true;
}

void Player::Update()
{
	AddGravity();
	SodaMove();
	
	SodaGaugeCharge();
	SodaShake();
	pMng->UpdateAll();

	//プレイヤー画面スクロール処理
	canvasX = posX - stage_->GetScrollX();
	canvasY = posY - stage_->GetScrollY();
	if (canvasX < Stage::CHIP_SIZE * 10)
	{
		stage_->SetScrollX(posX - Stage::CHIP_SIZE * 10);
		if (stage_->GetScrollX() < 0)
		{
			stage_->SetScrollX(0);
		}
	}
	if (canvasX > Stage::CHIP_SIZE * 38)
	{
		int newScrollX = posX - Stage::CHIP_SIZE * 38;
		newScrollX = std::min(newScrollX, stage_->GetMaxScrollX());
		stage_->SetScrollX(newScrollX);
	}

	if (canvasY < Stage::CHIP_SIZE * 5)
	{
		stage_->SetScrollY(posY - Stage::CHIP_SIZE * 5);
		if (stage_->GetScrollY() < 0)
		{
			stage_->SetScrollY(0);
		}
	}

	if (canvasY > Stage::CHIP_SIZE * 38)
	{
		int newScrollY = posY - Stage::CHIP_SIZE * 38;
		newScrollY = std::min(newScrollY, stage_->GetMaxScrollY());
		stage_->SetScrollY(newScrollY);
	}
	
}

void Player::Draw()
{
	//プレイヤーが死んでいる又は画像が読み込まれていないときは表示しない
	if (!aliveFlag || !image_) return;

	pMng->DrawAll();

	if (image_)
	{
		//プレイヤーの振動処理
		PlayerShake();

		//プレイヤー画像を描画(回転可)
		int handle = image_->GetHandle();
		DrawRotaGraph(
			(int)canvasX + shakeOffsetX,
			(int)canvasY + shakeOffsetY,
			2.0,
			angle,
			handle,
			TRUE
		);
	}
	//ゲージの描画
	DrawGauge(20, 50, 500, 40, playerHp, playerHpMax, GetColor(0, 255, 0), 0);		//プレイヤーのHPゲージ
	//炭酸蓄積ゲージが0以上の場合のみ描画
	if (sodaShakeGauge > 0)
	{
		DrawGauge(canvasX - 100, canvasY - 50, 20, 100, sodaShakeGauge, sodaShakeGaugeMax, GetColor(0, 0, 255), 1);		//炭酸蓄積ゲージ
	}
	DrawCircle(canvasX, canvasY, 3, 0X0000ff);
	//デバック用で赤い四角のプレイヤー表示
	//DrawBox((int)posX, (int)posY, (int)posX + 100, (int)posY + 100, GetColor(255, 0, 0), true);
	//
	//DrawBox(19, 69, 500, 101, GetColor(255, 0, 0), FALSE);
	//DrawBox(20, 100, 20 + (int)(playerHp * 4.8f), 70, GetColor(0, 255, 0), TRUE);
	//
	//DrawBox(19, 69, 500, 101, GetColor(255, 0, 0), FALSE);											//炭酸残量ゲージの枠線
	//DrawBox(20, 100, 20 + (int)(sodaGauge * 4.8f), 70, GetColor(0, 255, 255), TRUE);					//炭酸残量ゲージの表示
	//
	//DrawBox(19, 69+100, 500, 101+100, GetColor(255, 0, 0), FALSE);									//炭酸蓄積ゲージの枠線
	//DrawBox(20, 100+100, 20 + (int)(sodaShakeGauge * 4.8f), 70+100, GetColor(0, 0, 255), TRUE);		//炭酸蓄積ゲージの表示
}

//マウスを振ったり、スティックを動かすと炭酸ゲージが溜まる
void Player::SodaShake()
{
	//一旦保留
	/*
	//マウスを振るときの処理
	if ()
	{
		//マウスの移動量を取得
		int dx = InputManager::GetInstance().GetMouseDX();
		int dy = InputManager::GetInstance().GetMouseDY();

		//マウスの座標距離
		float mousedist = sqrtf(dx * dx + dy * dy);
		shakeMove = mousedist;
	}
	//ゲームパッドのスティックを動かすときの処理
	else
	{
		//ゲームパッドのRスティックの移動量を取得
		int px = InputManager::GetInstance().GetPadStickRX(0);
		int py = InputManager::GetInstance().GetPadStickRY(0);

		//スティックの座標距離
		float stickdist = sqrtf(px * px + py * py);
		shakeMove = stickdist;
	}
	*/
	//マウスの移動量を取得
	int dx = InputManager::GetInstance().GetMouseDX();
	int dy = InputManager::GetInstance().GetMouseDY();

	//マウスの座標距離
	float mousedist = sqrtf(dx * dx + dy * dy);
	shakeMove = mousedist;

	//ゲージ加算
	sodaShakeGauge += shakeMove * 0.005f;

	//上限
	if (sodaShakeGauge > sodaShakeGaugeMax) sodaShakeGauge = sodaShakeGaugeMax;

	//減衰
	sodaShakeGauge -= 0.2f;
	if (sodaShakeGauge < 0) sodaShakeGauge = 0;
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
	if (GetAttakFlag())
	{
		//プレイヤーが向いている方向とは逆に移動する
		velocityX += cos(angle - DX_PI_F / 2) * sodaPower;
		velocityY += sin(angle - DX_PI_F / 2) * sodaPower;

		sodaAttackFlag = false; //攻撃フラグをリセット
	}

	int signX = (velocityX > 0) ? 1 : ((velocityX < 0) ? -1 : 0);
	int loopX = abs(velocityX);
	while (loopX > 0)
	{
		if (WillCollide(posX + signX, posY))
		{
			velocityX = 0;
			break;
		}
		posX += signX;
		loopX--;
	}

	int signY = (velocityY > 0) ? 1 : ((velocityY < 0) ? -1 : 0);
	int loopY = abs(velocityY);
	while (loopY > 0)
	{
		if (WillCollide(posX, signY + posY))
		{
			velocityX = 0;
			velocityY = 0;
			jumpFlag = false;
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

	float jumpPower = 12.0f;

	//プレイヤーの向いている方向にジャンプ
	//DX_PI_F / 2はジャンプ方向を90度補正するための値
	velocityX += cos(angle - DX_PI_F / 2) * jumpPower;
	velocityY += sin(angle - DX_PI_F / 2) * jumpPower;

	jumpFlag = true;
}

//プレイヤー回転処理
void Player::Rotate()
{
	//回転速度
	float rotateSpeed = 0.05f;

	angle += InputManager::GetInstance().GetActionValue(ActionID::Rotate) * rotateSpeed;
}

bool Player::WillCollide(int newX, int newY)
{
	int left = newX - width_ / 2;
	int top = newY - height_ / 2;
	int right = newX + width_ / 2;
	int bottom = newY + height_ / 2;

	// 八か所の座標で壁をチェック（マップチップの壁 + LightWallGimmickの壁チップ）
	if (stage_->CheckWall(left, top)) return true;
	if (stage_->CheckWall((left + right) / 2 - 1, top)) return true;
	if (stage_->CheckWall(right - 1, top)) return true;
	if (stage_->CheckWall(left, (top + bottom) / 2 - 1)) return true;
	if (stage_->CheckWall(left, bottom - 1)) return true;
	if (stage_->CheckWall((left + right) / 2 - 1, bottom - 1)) return true;
	if (stage_->CheckWall(right - 1, bottom - 1)) return true;
	if (stage_->CheckWall(right - 1, (top + bottom) / 2 - 1)) return true;

	return false;
}

//炭酸攻撃処理
void Player::SodaAttack(int power)
{
	sodaAttackFlag = true;
	AttckDamage = power;
}

//ダメージ処理
void Player::Damage(float damage)
{
	playerHp -= damage;
	if (playerHp < 0)
	{
		playerHp = 0;
		aliveFlag = false; //プレイヤーが死んだ
	}
}

void Player::ClickSodaJump()
{
	sodaRatio = sodaShakeGauge / sodaGaugeMax;
  
	//炭酸蓄積ゲージが0より大きい場合、炭酸蓄積ゲージを減らす
	if (sodaShakeGauge > 0)
	{
		//炭酸蓄積ゲージの割合を炭酸攻撃の威力に変換
		sodaPower = sodaRatio * 20.0f;
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

		customCfg.startScale *= sodaRatio;
		customCfg.initSpeed *= sodaRatio;
		pMng->PlayParticle(customCfg, posX, posY);
	}
}

//プレイヤーの振動処理
void Player::PlayerShake()
{
	playerShakePower = sodaShakeGauge / sodaShakeGaugeMax;

	shakeOffsetX = (GetRand(10) - 5) * playerShakePower;
	shakeOffsetY = (GetRand(10) - 5) * playerShakePower;
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