#pragma once
#include "FileManager.h"
#include <memory>
#include <string>
#include "Stage.h"


class FileManager;
class ImageFile;
class Player;

class GimmickBase
{
public:
	enum class GIMMICK_TYPE
	{
		TELEPORT,

		
		E_TYPE_MAX,
	};


	GimmickBase(FileManager& fileMng, Stage* stage, float x, float y, GIMMICK_TYPE gimmickType);
	virtual ~GimmickBase();

	virtual void Update() = 0; // 更新処理
	virtual void Draw() const;   // 描画処理
	bool SetImage(const std::string& path); // 画像読み込み

	void SetPosition(float x, float y);
	bool IsAlive() const;

	//衝突判定
	bool WillCollide(int newX, int newY);
	RECT GetRect() const;

	GIMMICK_TYPE GetType() const { return gimmickType_; }

	// プレイヤーが衝突した時の純粋仮想関数（派生クラスで必ず実装する）
	virtual void OnCollidePlayer(Player& player) = 0;

	// 情報取得
	std::string GetName() const { return name_; }
	float GetX() const { return x_; }
	float GetY() const { return y_; }
	int GetWidth() const { return width_; }
	int GetHeight() const { return height_; }
	GIMMICK_TYPE GetEnemyType() const { return gimmickType_; }
	//敵の当たり判定の座標を取得
	int GetLeft() const { return static_cast<int>(x_ - width_ / 2); }
	int GetRight() const { return static_cast<int>(x_ + width_ / 2); }
	int GetTop() const { return static_cast<int>(y_ - height_ / 2); }
	int GetBottom() const { return static_cast<int>(y_ + height_ / 2); }

	bool GetAliveFlag() const { return isAlive_; }

protected:
	FileManager& fileManager_;
	std::shared_ptr<ImageFile> image_;	//画像データ
	Stage* stage_;						//ステージへのポインタ
	GIMMICK_TYPE gimmickType_;				//敵の種類

	float x_;		//敵のx座標
	float y_;		//敵のy座標

	int width_;		//画像の幅
	int height_;	//画像の高さ
	int canvasX;	//ギミックの描画位置X
	int canvasY;	//ギミックの描画位置Y

	int hp_;				//HP
	int hpMax_;				//HPの最大値
	bool isAlive_;			//生存フラグ
	std::string name_;		//敵の名前
};

