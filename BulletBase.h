#pragma once

#include <DxLib.h>
#include <memory>
#include <string>

class FileManager;
class ImageFile;
class Stage;

class BulletBase
{
public:
	enum class BULLET_TYPE
	{
		B_TYPE_NON,
		B_TYPE_1,
		B_TYPE_2,
		B_TYPE_3,
		B_TYPE_MAX
	};

	BulletBase(FileManager& fileMng, Stage* stage, float x, float y, float vx, float vy, float scale);
	virtual ~BulletBase();

	virtual void Update();						//更新処理
	virtual void Draw() const;					//描画処理
	bool SetImage(const std::string& path);		//画像のセット

	//基本操作
	void SetPosition(float x, float y);
	void SetVelocity(float vx, float vy);
	void Move();
	void MoveX();
	void MoveY();
	void Shot(float dirX, float dirY, float scale);		//敵の弾を撃つ処理

	//生存フラグを取得
	bool IsAlive() const;

	//衝突判定
	bool WillCollide(int newX, int newY);
	RECT GetRect() const;

	//情報取得
	float GetX() const { return x_; }
	float GetY() const { return y_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	//攻撃力を取得
	int GetApplyDamage() const { return damage_; }
	//生存フラグを取得
	bool GetAliveFlag() const { return isAlive_; }
	void SetAliveFlag(bool alive) { isAlive_ = alive; }

protected:
	Stage* stage_;
	FileManager& fileManager;
	std::shared_ptr<ImageFile> image_;

	BULLET_TYPE bulletType_;

	float x_;
	float y_;
	float vx_;
	float vy_;
	int width_;
	int height_;
	float scale_;

	bool isAlive_;
	int damage_;
};