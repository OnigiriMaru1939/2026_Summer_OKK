#pragma once

#include <cmath>
#include <memory>
#include <string>
#include "Stage.h"


class FileManager;
class ImageFile;
class SoundFile;
class ParticleManager;
class ParticleEmitter;

class Player
{
public:
	static constexpr int PLAYER_WIDTH = 64;			//プレイヤーの横幅
	static constexpr int PLAYER_HEIGHT = 64;		//プレイヤーの縦幅
	static constexpr int SPEED = 15;				//プレイヤーの移動速度

	Player(FileManager& fileMng, Stage* stage);
	~Player();
	bool SystemInit();								//初期化
	bool SetImage(const std::string& path);			//画像のセット
	bool WillCollide(int newX, int newY);			//プレイヤーのステージとの衝突判定
	void Update();
	void Draw();
	void SodaShake();				//マウスを振ると炭酸蓄積ゲージが溜まる
	void SodaGaugeCharge();			//炭酸残量ゲージの自然回復
	void AddGravity();				//重力処理
	void SodaMove();				//炭酸移動処理
	void SpaceJump();				//スペースジャンプ処理
	void ClickSodaJump();           //クリックジャンプ処理
	void Rotate();					//回転処理
	void SodaAttack(int power);		//炭酸攻撃処理
	void Damage(float damage);		//ダメージ処理
	void PlayerShake();				//プレイヤーの振動処理
  
	float posX;					//プレイヤーのX座標
	float posY;					//プレイヤーのY座標
	float shakeOffsetX;			//振動時のX座標
	float shakeOffsetY;			//振動時のY座標

	float playerHp;				//プレイヤーのHP
	float playerHpMax;			//プレイヤーのHPの最大値
	float sodaGauge;			//炭酸残量ゲージ
	float sodaGaugeMax;			//炭酸残量ゲージの最大値
	float sodaRatio;            //炭酸残量ゲージ
	float sodaShakeGauge;		//炭酸蓄積ゲージ
	static constexpr float SODA_SHAKE_GAUGE_MAX = 1000.0f;	//炭酸蓄積ゲージの最大値

private:
	int GetWidth() const;											//プレイヤーの画像の幅を取得
	int GetHeight() const;											//プレイヤーの画像の高さを取得
	bool GetAttakFlag() const { return sodaAttackFlag; }			//炭酸攻撃フラグを取得
	bool GetAliveFlag() const { return aliveFlag; }					//生存フラグを取得
	bool GetJumpFlag() const { return jumpFlag; }					//ジャンプフラグを取得
	//ゲージの描画
	void DrawGauge(int x,
				   int y,
				   int width,
				   int height,
				   float value,
				   float maxValue,
				   int color,
				   int mode);

	std::unique_ptr<ParticleManager> pMng;
	FileManager& fileManager;					//ファイルマネージャー

	std::shared_ptr<ImageFile> image_;			//プレイヤーの画像

	std::shared_ptr<SoundFile> sodaAttackSE;		//炭酸攻撃のサウンド

	Stage* stage_;								//ステージへのポインタ
	std::weak_ptr<ParticleEmitter> sodaParticle;		// 炭酸攻撃のパーティクルエミッター weak_ptrにすることで、エミッターが削除された後も安全にアクセスできるようにする

	int width_;					//プレイヤーの画像の幅
	int height_;				//プレイヤーの画像の高さ
	int canvasX;				//プレイヤーの描画位置X
	int canvasY;				//プレイヤーの描画位置Y
	
	float sodaPower;			//炭酸攻撃の威力
	bool aliveFlag;				//生存フラグ
	bool jumpFlag;				//ジャンプフラグ
	bool sodaAttackFlag;		//炭酸攻撃フラグ
	float gravity;				//重力
	float velocityX;			//X軸の速度
	float velocityY;			//Y軸の速度
	float angle;				//回転角度
	float rotateSpeed;			//回転速度
	float jumpPower;			//ジャンプ力
	float AttckDamage;			//攻撃のダメージ
	float shakeMove;			//振動の移動量
	float playerShakePower;		//プレイヤーの振動量
};
