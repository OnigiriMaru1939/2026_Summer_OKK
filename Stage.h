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
	int GetScrollX();
	int GetScrollY();
	int GetMaxScrollX();
	int GetMaxScrollY();

	// マップチップを設定・取得（LightWallGimmick用）
	void SetChip(int chipX, int chipY, int chipId);
	int GetChip(int chipX, int chipY) const;

	// ワールド座標をチップ座標に変換
	int WorldToChipX(int worldX) const { return worldX / CHIP_SIZE; }
	int WorldToChipY(int worldY) const { return worldY / CHIP_SIZE; }


/*
	// タイルセット画像を読み込む
	bool LoadTileSet(const std::string& path);

	// CSVでマップを読み込む
	bool LoadMapFromCSV(const std::string& path);

	// 外部から直接マップデータを設定する場合
	void SetMap(const std::vector<int>& tiles, int cols, int rows);


	void Update();
	void Draw() const;

	// あたり判定
	bool IsSolidAt(int col, int row) const; 

	bool CheckCollision(float x, float y, float width, float height) const;

	// ワールド座標（ピクセル）
	bool IsSolidWorld(float x, float y) const;

	int GetCols() const { return cols_; }
	int GetRows() const { return rows_; }
	int GetTileWidth() const { return tileW_; }
	int GetTileHeight() const { return tileH_; }
	*/
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

	/*
	int tileW_;
	int tileH_;

	int cols_ = 0;
	int rows_ = 0;
	// tiles_[row * cols_ + col] = タイルインデックス（0 を空に使う）
	std::vector<int> tiles_;
	*/
};

