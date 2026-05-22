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
		E_TYPE_1,

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
	void AddGravity();

	// ダメージ管理
	void ApplyDamage(int dmg);
	bool IsAlive() const;

	//衝突判定
	bool WillCollide(int newX, int newY);
	
	// 情報取得
	float GetX() const;
	float GetY() const;
	int GetWidth() const;
	int GetHeight() const;
protected:
	FileManager& fileManager_;
	std::shared_ptr<ImageFile> image_;	//画像データ
	Stage* stage_;						//ステージへのポインタ

	float x_;		//x座標
	float y_;		//y座標
	float vx_;		//x方向の速度
	float vy_;		//y方向の速度
	float gravity;	//重力加速度

	int width_;		//画像の幅
	int height_;	//画像の高さ
	int canvasX;	//敵の描画位置X
	int canvasY;	//敵の描画位置Y

	int hp_; //HP
	bool isAlive_; // 生存フラグ

};

