#include "Stage.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "Application.h"
#include <DxLib.h>
#ifdef max
#undef max
#endif
#include <fstream>
#include <sstream>
#include <algorithm>


Stage::Stage(FileManager& fileMng, int TileWidth, int TileHeight)
	: fileManager_(fileMng)
	, tileW_(TileWidth)
	, tileH_(TileHeight)
{
}

Stage::~Stage()
{
}

bool Stage::LoadTileSet(const std::string& path)
{
	tileset_ = fileManager_.LoadImageFM(path);
	return tileset_ != nullptr;
}

bool Stage::LoadMapFromCSV(const std::string& path)
{
	std::ifstream ifs(path);
	if (!ifs.is_open()) return false;

	std::vector<int> data;
	std::string line;
	int detectedCols = 0;
	int detectedRows = 0;

	while (std::getline(ifs, line))
	{
		if (line.empty()) continue;
		std::istringstream ss(line);
		std::string cell;
		int colCount = 0;
		while (std::getline(ss, cell, ','))
		{
			// 空文字は 0 にする
			try
			{
				int v = 0;
				if (!cell.empty())
				{
					v = std::stoi(cell);
				}
				data.push_back(v);
			}
			catch (...)
			{
				data.push_back(0);
			}
			++colCount;
		}
		detectedCols = std::max(detectedCols, colCount);
		++detectedRows;
	}

	if (detectedCols == 0 || detectedRows == 0) return false;

	// 行ごとに列数が揃っていない場合は 0 を埋める
	std::vector<int> normalized;
	normalized.reserve(detectedCols * detectedRows);
	for (int r = 0; r < detectedRows; ++r)
	{
		for (int c = 0; c < detectedCols; ++c)
		{
			int idx = r * detectedCols + c;
			if (idx < static_cast<int>(data.size()))
				normalized.push_back(data[idx]);
			else
				normalized.push_back(0);
		}
	}

	SetMap(normalized, detectedCols, detectedRows);
	return true;
}

void Stage::SetMap(const std::vector<int>& tiles, int cols, int rows)
{
	cols_ = cols;
	rows_ = rows;
	tiles_ = tiles;
	// サイズチェック
	if (static_cast<int>(tiles_.size()) != cols_ * rows_)
	{
		tiles_.resize(cols_ * rows_, 0);
	}
}

void Stage::Update()
{
}


void Stage::Draw() const
{
	if (!tileset_) return;
	int handle = tileset_->GetHandle();
	int tilesetW = tileset_->GetWidth();
	if (tileW_ <= 0 || tileH_ <= 0) return;
	int tilesPerRow = std::max(1, tilesetW / tileW_);

	// ステージ全体のサイズ
	int stageWidth = cols_ * tileW_;
	int stageHeight = rows_ * tileH_;

	int offsetX = (Application::SCREEN_WID - stageWidth) / 2;
	int offsetY = (Application::SCREEN_HIG - stageHeight) / 2;

	// ステージが画面より大きい場合は 0 に固定（左上合わせ）
	if (offsetX < 0) offsetX = 0;
	if (offsetY < 0) offsetY = 0;

	for (int r = 0; r < rows_; ++r)
	{
		for (int c = 0; c < cols_; ++c)
		{
			int t = tiles_[r * cols_ + c];
			if (t <= 0) continue; // 0 を空タイル扱い 
			int ti = t; // タイルインデックス
			// タイルインデックスが 1 始まりなら -1 する 
		

			int sx = (ti % tilesPerRow) * tileW_;
			int sy = (ti / tilesPerRow) * tileH_;
			int dx = offsetX + (c * tileW_);
			int dy = offsetY + (r * tileH_);
			// 描画
			DrawRectGraph(dx, dy,
						  sx, sy, tileW_, tileH_,
						  handle,
						  TRUE, FALSE);
		}
	}
}

bool Stage::IsSolidAt(int col, int row) const
{
	// 範囲外は空タイル扱い
	if (col < 0 || row < 0 || col >= cols_ || row >= rows_) return false;
	int v = tiles_[row * cols_ + col];

	return v != 0;
}

// 矩形範囲（左、上、右、下）に壁があるかチェック
bool Stage::CheckCollision(float left, float top, float right, float bottom) const
{
	// 座標をタイルインデックスに変換
	int minCol = static_cast<int>(left) / tileW_;
	int maxCol = static_cast<int>(right) / tileW_;
	int minRow = static_cast<int>(top) / tileH_;
	int maxRow = static_cast<int>(bottom) / tileH_;

	// 範囲内のタイルをすべてループで回す
	for (int r = minRow; r <= maxRow; ++r)
	{
		for (int c = minCol; c <= maxCol; ++c)
		{
			if (IsSolidAt(c, r)) return true;
		}
	}
	return false;
}

bool Stage::IsSolidWorld(float x, float y) const
{
	if (tileW_ <= 0 || tileH_ <= 0) return false;
	int col = static_cast<int>(x) / tileW_;
	int row = static_cast<int>(y) / tileH_;
	return IsSolidAt(col, row);
}