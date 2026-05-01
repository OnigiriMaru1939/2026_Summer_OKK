#include "Stage.h"
#include "FileManager.h"
#include "ImageFile.h"
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
			int dx = c * tileW_;
			int dy = r * tileH_;
			// DrawRectGraph の第8引数は透過フラグ
			DrawRectGraph(dx, dy,
						  sx, sy, sx + tileW_ - 1, sy + tileH_ - 1,
						  handle,
						  TRUE, FALSE);
		}
	}
}

bool Stage::IsSolidAt(int col, int row) const
{
	if (col < 0 || row < 0 || col >= cols_ || row >= rows_) return false;
	int v = tiles_[row * cols_ + col];
	// シンプルに 0 を空、0 以外を衝突あり とする
	return v != 0;
}

bool Stage::IsSolidWorld(float x, float y) const
{
	if (tileW_ <= 0 || tileH_ <= 0) return false;
	int col = static_cast<int>(x) / tileW_;
	int row = static_cast<int>(y) / tileH_;
	return IsSolidAt(col, row);
}