#pragma once

#include <cmath>
#include <memory>
#include <string>
#include "Stage.h"
#include "EnemyBase.h"

class SceneGame;
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
	static constexpr float ATTACK_SPEED_THRESHOLD = 10.0f;	//攻撃可能な最低限の速度
	static constexpr float AIR_RESUSTANCE = 0.98f;			//空気抵抗
	static constexpr float WALL_BOUNCE_X = 0.3f;			//横壁の反動係数
	static constexpr float FLOOR_BOUNCE_Y = 0.3f;			//床の反動係数
	static constexpr float FLOOR_FRICTION = 0.9f;			//床の摩擦係数
	
	Player(FileManager& fileMng, Stage* stage, SceneGame& game);
	~Player();
	bool SetImage(const std::string& path);			//画像のセット
	void SetPosition(float x, float y);				//プレイヤーの位置を設定
	void SetVelocity(float vx, float vy);			//プレイヤーのX軸の速度を設定
	float VelocityCalc() const;						//プレイヤーの速度計算
	bool WillCollide(int newX, int newY);
	//プレイヤーのステージとの衝突判定
	void Update();
	void Draw();
	void SodaShake();				//マウスを振ると炭酸蓄積ゲージが溜まる
	void SodaGaugeCharge();			//炭酸残量ゲージの自然回復
	void AddGravity();				//重力処理
	void SodaMove();				//炭酸移動処理
	void MoveX();					//Xの移動
	void MoveY();					//Yの移動
	void SpaceJump();				//スペースジャンプ処理
	void ClickSodaJump();           //クリックジャンプ処理
	void Rotate();					//回転処理
	void RotateAxis();				//回転処理（軸入力）
	void SodaAttack(float power);	//炭酸攻撃処理
	void Damage(float damage);		//ダメージ処理
	void PlayerShake();				//プレイヤーの振動処理
	void PlayerKnockBack(float enemyX, float enemyY, float power);			//プレイヤーのノックバック処理
	RECT GetRect() const;			//プレイヤーの当たり判定の矩形を取得
	bool CollisionHpBar();          //プレイヤーがHPバーに触れているか
  
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
	int noDamageTime;			//無敵時間
	int noDamageMaxTime;		//無敵時間の最大値
	int attackTimer;			//攻撃時間

	static constexpr float SODA_SHAKE_GAUGE_MAX = 1000.0f;	//炭酸蓄積ゲージの最大値

	float GetX() const { return canvasX; }
	float GetY() const { return canvasY; }
	int GetWidth() const { return width_; }				//プレイヤーの画像の幅を取得
	int GetHeight() const { return height_; }			//プレイヤーの画像の高さを取得
	float GetSpeed() const { return playerSpeed; };		//プレイヤーの速度を取得
	//プレイヤーの当たり判定の座標を取得
	int GetLeft() const { return static_cast<int>(canvasX - width_ / 2); }
	int GetRight() const { return static_cast<int>(canvasX + width_ / 2); }
	int GetTop() const { return static_cast<int>(canvasY - height_ / 2); }
	int GetBottom() const { return static_cast<int>(canvasY + height_ / 2); }

	bool GetAttakFlag() const { return sodaAttackFlag; }			//炭酸攻撃フラグを取得
	float GetAttackDamage() const { return attackDamage; }			//攻撃のダメージを取得
	bool GetAliveFlag() const { return aliveFlag; }					//生存フラグを取得
	//移動可能フラグを設定・取得
	bool GetCanMoveFlag() const { return canMoveFlag; }
	void SetCanMoveFlag(bool flag) { canMoveFlag = flag; }
	//プレイヤーの位置に応じてステージのスクロールを更新
	void UpdateStageScroll();
private:
	
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
	SceneGame& sceneGame;

	std::shared_ptr<ImageFile> image_;			//プレイヤーの画像

	std::shared_ptr<SoundFile> sodaAttackSE;		//炭酸攻撃のサウンド
	std::shared_ptr<SoundFile> sodaChargeSE;		//炭酸蓄積のサウンド

	Stage* stage_;								//ステージへのポインタ
	std::weak_ptr<ParticleEmitter> sodaParticle;		// 炭酸攻撃のパーティクルエミッター weak_ptrにすることで、エミッターが削除された後も安全にアクセスできるようにする

	int width_;					//プレイヤーの画像の幅
	int height_;				//プレイヤーの画像の高さ
	float canvasX;				//プレイヤーの描画位置X
	float canvasY;				//プレイヤーの描画位置Y
	
	float sodaPower;			//炭酸攻撃の威力
	bool aliveFlag;				//生存フラグ
	bool canMoveFlag;			//移動可能フラグ
	bool jumpFlag;				//ジャンプフラグ
	bool sodaAttackFlag;		//炭酸攻撃フラグ
	float gravity;				//重力
	float playerSpeed;			//プレイヤー速度
	float velocityX;			//X軸のプレイヤー速度
	float velocityY;			//Y軸のプレイヤー速度
	float angle;				//回転角度
	float rotateSpeed;			//回転速度
	float jumpPower;			//ジャンプ力
	float attackDamage;			//攻撃のダメージ
	float shakeMove;			//振動の移動量
	float playerShakePower;		//プレイヤーの振動量

	int hpFontHandle;
};
