#pragma once
#include <memory>
#include <string>
#include "Stage.h"


class FileManager;
class ImageFile;

class BulletBase
{
public:
	BulletBase(FileManager& fileMng, Stage* stage);
	virtual ~BulletBase();

	enum class BULLET_TYPE
	{
		B_TYPE_NON,
		B_TYPE_1,
		B_TYPE_2,
		B_TYPE_3,

		B_TYPE_MAX
	};

	virtual void Update() = 0;					//更新処理
	virtual void Draw() const;					//描画処理
	bool SetImage(const std::string& path);		//画像読み込み

	//基本操作
	void SetPosition(float x, float y);
	void SetVelocity(float vx, float vy);

	//攻撃
	void ApplyDamage(int dmg);

	//衝突判定
	bool WillCollide(int newX, int newY);

protected:
	FileManager& fileManager_;
	std::shared_ptr<ImageFile> image_;	//画像データ
	Stage* stage_;						//ステージへのポインタ
	BULLET_TYPE bulletType_;			//弾の種類

	float x_;			//x座標
	float y_;			//y座標
	float vx_;			//x方向の速度
	float vy_;			//y方向の速度
	int width_;			//画像の幅
	int height_;		//画像の高さ
	bool isAlive_;		//生存フラグ
};