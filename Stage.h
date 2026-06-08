#pragma once

#include <vector>
#include <string>
#include <memory>
#include"Application.h"
#include <DxLib.h>

class FileManager;
class ImageFile;
class Stage
{
public:
	static constexpr int CHIP_SIZE = 40;
	static constexpr int CHIP_WIDTH = 4;
	static constexpr int CHIP_HEIGHT = 4;
	static constexpr int SCROLL_SPEED = 6;

	Stage(FileManager& fileMng);
	~Stage();
	bool Load(const std::string& path);
	void Update();
	void Draw();

	bool CheckWall(int cx, int cy) const;
	
	void SetScrollX(int s);
	void SetScrollY(int s);
	void SetBgImage(const std::string& path);
	void SetMchipImage(const std::string& path);
	int GetScrollX();
	int GetScrollY();
	int GetMaxScrollX();
	int GetMaxScrollY();
	// あたり判定
	bool CheckHitWallRect(int x, int y, int width, int height);

	// マップチップを設定・取得（LightWallGimmick用）
	void SetChip(int chipX, int chipY, int chipId);
	int GetChip(int chipX, int chipY) const;

	// ワールド座標をチップ座標に変換
	int WorldToChipX(int worldX) const { return worldX / CHIP_SIZE; }
	int WorldToChipY(int worldY) const { return worldY / CHIP_SIZE; }
private:
	struct ChipInfo
	{
		bool isWall;
	};

	std::vector<ChipInfo> chipInfo_;
	int scrollX;
	int scrollY;

	int bgScrollX;
	int bgScrollY;

	int mapWidth;
	int mapHeight;

	std::shared_ptr<ImageFile> chipImg_;
	std::shared_ptr<ImageFile> bgImg_;
	std::vector<std::vector<int>>tileMap;

	FileManager& fileMng_;
};

