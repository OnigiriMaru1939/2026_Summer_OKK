#pragma once
#include <vector>
#include <string>
#include <memory>

class FileManager;
class ImageFile;
class Stage
{
public:
	Stage(FileManager& fileMng, int TileWidth, int TileHeight);
	~Stage();
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
	// ワールド座標（ピクセル）
	bool IsSolidWorld(float x, float y) const;

	int GetCols() const { return cols_; }
	int GetRows() const { return rows_; }
	int GetTileWidth() const { return tileW_; }
	int GetTileHeight() const { return tileH_; }

private:
	FileManager& fileManager_;
	std::shared_ptr<ImageFile> tileset_;

	int tileW_;
	int tileH_;

	int cols_ = 0;
	int rows_ = 0;
	// tiles_[row * cols_ + col] = タイルインデックス（0 を空に使う）
	std::vector<int> tiles_;
};

