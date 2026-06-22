#pragma once
#include <memory>
#include <string>
#include "Stage.h"

class FileManager;
class ImageFile;

class EnemyBase
{
public:
	enum class ENEMY_TYPE
	{
		E_TYPE_NON,
		E_TYPE_1,
		E_TYPE_2,
		E_TYPE_3,
		E_TYPE_BOSS_1,
		E_TYPE_BOSS_2,
		E_TYPE_BOSS_3,

		E_TYPE_MAX,
	};

	EnemyBase(FileManager& fileMng, Stage* stage, float x, float y);
	virtual ~EnemyBase();

	virtual void Update() = 0; // 更新処理
	virtual void Draw() const;   // 描画処理
	bool SetImage(const std::string& path); // 画像読み込み

	// 基本操作
	void SetPosition(float x, float y);
	void SetVelocity(float vx, float vy);
	void Move();
	void MoveX();
	void MoveY();
	void AddGravity();
	void NoDamageCountDown(); //無敵時間のカウントダウン

	//ダメージ管理
	void ApplyDamage(int dmg);
	bool IsAlive() const;

	//衝突判定
	bool WillCollide(int newX, int newY);
	RECT GetRect() const;
	
	// 情報取得
	std::string GetName() const { return name_; }
	float GetX() const { return x_; }
	float GetY() const { return y_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	ENEMY_TYPE GetEnemyType() const { return enemyType_; }
	//敵の当たり判定の座標を取得
	int GetLeft() const { return static_cast<int>(x_ - width_ / 2); }
	int GetRight() const { return static_cast<int>(x_ + width_ / 2); }
	int GetTop() const { return static_cast<int>(y_ - height_ / 2); }
	int GetBottom() const { return static_cast<int>(y_ + height_ / 2); }
	//攻撃力を取得
	int GetAttackDamage() const { return AttckDamage; }
	//HPを取得
	int GetHp() const { return hp_; }
	int GetHpMax() const { return hpMax_; }
	//フラグを取得
	bool GetJumpFlag() const { return jumpFlag; }
	bool GetAliveFlag() const { return isAlive_; }
	void SetAppearFlag(bool flag) { isAppearing = flag; } //出現フラグを設定する関数
protected:
	FileManager& fileManager_;
	std::shared_ptr<ImageFile> image_;	//画像データ
	Stage* stage_;						//ステージへのポインタ
	ENEMY_TYPE enemyType_;				//敵の種類
	
	//ゲージの描画
	void DrawGauge(int x, int y, int width, int height, float value, float maxValue, int color) const;

	float x_;			//x座標
	float y_;			//y座標
	float vx_;			//x方向の速度
	float vy_;			//y方向の速度
	float scale;		//敵の大きさ
	float angle;		//回転角度
	float gravity;		//重力加速度
	int AttckDamage;	//攻撃力
	int noDamageTime;	//無敵時間
	int noDamageMaxTime;//無敵時間の最大値

	int width_;		//画像の幅
	int height_;	//画像の高さ
	int canvasX;	//敵の描画位置X
	int canvasY;	//敵の描画位置Y

	int hp_;				//HP
	int hpMax_;				//HPの最大値
	bool isAlive_;			//生存フラグ
	bool jumpFlag;			//ジャンプフラグ
	bool isAppearing;       //ボスが出現中かどうかのフラグ
	std::string name_;		//敵の名前
};

