#include "Player.h"
#include "math.h"
#include "InputManager.h"
#include <DxLib.h>

Player::Player()
{

}

Player::~Player()
{

}

bool Player::SystemInit()
{
	sodaGauge = 100.0f;
	sodaGaugeMax = 100.0f;
	sodaShakeGauge = 0.0f;
	sodaShakeGaugeMax = 100.0f;
	aliveFlag = true;
	GetMousePoint(&prevMouseX, &prevMouseY);

	return true;
}

void Player::Update()
{
	SodaGaugeCharge();
	SodaShake();
}

void Player::Draw()
{
	DrawBox(19, 101, 500, 69, GetColor(255, 0, 0), FALSE);											//炭酸残量ゲージの枠線
	DrawBox(20, 100, 20 + (int)(sodaGauge * 4.8f), 70, GetColor(0, 255, 255), TRUE);				//炭酸残量ゲージの表示

	DrawBox(19, 101+100, 500, 69+100, GetColor(255, 0, 0), FALSE);									//炭酸蓄積ゲージの枠線
	DrawBox(20, 100+100, 20 + (int)(sodaShakeGauge * 4.8f), 70+100, GetColor(255, 0, 0), TRUE);		//炭酸蓄積ゲージの表示
}

//マウスを振ると炭酸ゲージが溜まる
void Player::SodaShake()
{
	int mouseX, mouseY;

	// 現在のマウス座標取得
	GetMousePoint(&mouseX, &mouseY);

	// 移動量
	int dx = mouseX - prevMouseX;
	int dy = mouseY - prevMouseY;

	// 距離（振った強さ）
	float dist = sqrtf(dx * dx + dy * dy);

	// ゲージ加算（調整値は好みで）
	sodaShakeGauge += dist * 0.002f;

	// 上限
	if (sodaShakeGauge > sodaShakeGaugeMax) sodaShakeGauge = sodaShakeGaugeMax;

	// 減衰（振らないと減る）
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

	// 保存
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