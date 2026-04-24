#pragma once
#include <memory>
#include <string>

class FileManager;
class ImageFile;

class EnemyBase
{
public:
	EnemyBase(FileManager& fileMng, float x = 0.0f, float y = 0.0f);
	virtual ~EnemyBase();

	virtual void Update() = 0; // 更新処理
	virtual void Draw() const;   // 描画処理
	bool SetImage(const std::string& path); // 画像読み込み

	// 基本操作
	void SetPosition(float x, float y);
	void SetVelocity(float vx, float vy);
	void Move(float dx, float dy);

	// ダメージ管理
	void ApplyDamage(int dmg);
	bool IsAlive() const;

	// 情報取得
	float GetX() const;
	float GetY() const;
	int GetWidth() const;
	int GetHeight() const;
protected:
	FileManager& fileManager_;
	std::shared_ptr<ImageFile> image_; // 画像データ

	float x_; //x座標
	float y_; //y座標
	float vx_; //x方向の速度
	float vy_; //y方向の速度

	int width_; // 画像の幅
	int height_; // 画像の高さ

	int hp_; //HP
	bool isAlive_; // 生存フラグ

};

