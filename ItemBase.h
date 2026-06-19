#pragma once

#include <memory>
#include <string>
#include "Stage.h"

class FileManager;
class ImageFile;
class Player;

class ItemBase
{
public:
	enum class ITEM_TYPE
	{
		I_TYPE_NON,
		MENTOS,

		I_TYPE_MAX
	};

	ItemBase(FileManager& fileMng, Stage* stage, float x, float y);
	virtual ~ItemBase();
	bool SetImage(const std::string& path);		//画像読み込み
	virtual void Update() = 0;					//更新処理
	virtual void Draw();						//描画処理
	virtual RECT GetRect() const;				//アイテムの衝突判定処理
	virtual void OnGet(Player& player) = 0;		//プレイヤーが取得したときの処理

	//アイテムの位置を設定
	void SetPosition(float x, float y);

	bool IsAlive() const { return alive_; }

protected:
	FileManager& fileManager_;
	std::shared_ptr<ImageFile> image_;	//画像データ
	Stage* stage_;						//ステージへのポインタ
	ITEM_TYPE itemTypp_;				//アイテムの種類

	float x_;			//x座標
	float y_;			//y座標
	int width_;		//画像の幅
	int height_;	//画像の高さ
	bool alive_;
	float posX_;
	float posY_;
};

