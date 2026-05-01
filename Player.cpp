#include "Player.h"
#include "math.h"
#include "InputManager.h"
#include "FileManager.h"
#include "ImageFile.h"
#include <DxLib.h>

Player::Player(FileManager& fileMng) : fileManager(fileMng)
{

}

Player::~Player()
{

}

//初期化
bool Player::SystemInit()
{
	posX = 200.0f;
	posY = 200.0f;

	sodaGauge = 100.0f;
	sodaGaugeMax = 100.0f;
	sodaShakeGauge = 0.0f;
	sodaShakeGaugeMax = 100.0f;
	aliveFlag = true;
	jumpFlag = false;
	angle = 0.0f;

	gravity = 0.5f;
	velocityY = 0.0f;

	GetMousePoint(&prevMouseX, &prevMouseY);

	return true;
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

void Player::Update()
{
	AddGravity();
	Jump();
	Rotate();

	//Aキーを押すと左移動
	if (InputManager::GetInstance().IsKeyPressed(30))
	{
		posX -= SPEED;
	}
	//Dキーを押すと右移動
	if (InputManager::GetInstance().IsKeyPressed(32))
	{
		posX += SPEED;
	}

	SodaGaugeCharge();
	SodaShake();
}

void Player::Draw()
{
	//プレイヤーが死んでいる又は画像が読み込まれていないときは表示しない
	//if (!aliveFlag || !image_) return;
	
	if (image_)
	{
		int handle = image_->GetHandle();
		DrawGraph((int)posX, (int)posY, handle, true);
	}

	//デバック用で赤い四角のプレイヤー表示
	DrawBox((int)posX, (int)posY, (int)posX + 100, (int)posY + 100, GetColor(255, 0, 0), true);

	DrawBox(19, 101, 500, 69, GetColor(255, 0, 0), FALSE);											//炭酸残量ゲージの枠線
	DrawBox(20, 100, 20 + (int)(sodaGauge * 4.8f), 70, GetColor(0, 255, 255), TRUE);				//炭酸残量ゲージの表示

	DrawBox(19, 101+100, 500, 69+100, GetColor(255, 0, 0), FALSE);									//炭酸蓄積ゲージの枠線
	DrawBox(20, 100+100, 20 + (int)(sodaShakeGauge * 4.8f), 70+100, GetColor(0, 0, 255), TRUE);		//炭酸蓄積ゲージの表示
}

//マウスを振ると炭酸ゲージが溜まる
void Player::SodaShake()
{
	int mouseX, mouseY;

	//現在のマウス座標取得
	GetMousePoint(&mouseX, &mouseY);

	//移動量
	int dx = mouseX - prevMouseX;
	int dy = mouseY - prevMouseY;

	//距離
	float dist = sqrtf(dx * dx + dy * dy);

	// ゲージ加算
	sodaShakeGauge += dist * 0.002f;

	//上限
	if (sodaShakeGauge > sodaShakeGaugeMax) sodaShakeGauge = sodaShakeGaugeMax;

	//減衰（振らないと減る）
	sodaShakeGauge -= 0.2f;
	if (sodaShakeGauge < 0) sodaShakeGauge = 0;

	//左クリックすると炭酸残量ゲージを減らす
	if (InputManager::GetInstance().IsMouseTriggered(MouseButton::Left))
	{
		//炭酸蓄積ゲージが0より大きい場合、炭酸蓄積ゲージを減らす
		if (sodaShakeGauge > 0)
		{
			sodaGauge -= 20.0f;
			sodaShakeGauge = 0;
			//下限リミッター
			if (sodaGauge < 0) sodaGauge = 0;
		}
	}

	//保存
	prevMouseX = mouseX;
	prevMouseY = mouseY;
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
	posY += velocityY;		//速度を位置に加算
	// 地面判定（仮）
	if (posY > 500)
	{
		posY = 500;
		velocityY = 0;
		jumpFlag = false; 
	}
}

//ジャンプ処理
void Player::Jump()
{
	//SPACEキーを押すとジャンプ
	if (InputManager::GetInstance().IsKeyTriggered(57) && !jumpFlag)
	{
		velocityY = -12.0f;				// ジャンプの初速度
		jumpFlag = true;
	}
}

//プレイヤー回転処理
void Player::Rotate()
{
	float sensitivity = 0.01f; //感度

	int dx = InputManager::GetInstance().GetMouseDX();

	angle += dx * sensitivity;
}

//炭酸攻撃処理
void Player::SodaAttack()
{

}