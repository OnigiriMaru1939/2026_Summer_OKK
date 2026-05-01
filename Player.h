#pragma once
class Player
{
public:
	static constexpr int PLAYER_WIDTH = 64;			//プレイヤーの横幅
	static constexpr int PLAYER_HEIGHT = 64;		//プレイヤーの縦幅
	static constexpr int SPEED = 15;					//プレイヤーの移動速度
	static constexpr int PLAYER_SUM = 3;			//プレイヤーの残機数

	Player();
	~Player();
	bool SystemInit();			//初期化
	void Update();
	void Draw();
	void SodaShake();		//マウスを振ると炭酸蓄積ゲージが溜まる
	void SodaGaugeCharge();		//炭酸残量ゲージの自然回復


	float sodaGauge;			//炭酸残量ゲージ
	float sodaGaugeMax;			//炭酸残量ゲージの最大値
	float sodaShakeGauge;		//炭酸蓄積ゲージ
	float sodaShakeGaugeMax;	//炭酸蓄積ゲージの最大値
	bool aliveFlag;				//生存フラグ
	float posX;					//プレイヤーのX座標
	float posY;					//プレイヤーのY座標

	int prevMouseX;				//前回のマウスX座標
	int prevMouseY;				//前回のマウスY座標

private:
	
};
