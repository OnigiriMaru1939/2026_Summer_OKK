#include "Stage.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "Application.h"
#include <DxLib.h>
#include <fstream>
#include <sstream>
#include <algorithm>


Stage::Stage(FileManager& fileMng):fileMng_(fileMng)
{
	// マップチップ画像の読み込み
	bgImg_ = fileMng_.LoadImageFM("Resource/Image/Game/Stage1_bg.png"); 
	if (!bgImg_)
	{
		printfDx("背景画像の読み込みに失敗しました\n");
	}
	//bgImg_ = fileMng_.LoadImageFM("Resource/Image/Stage1_bg.png"); 
	// マップチップ画像の読み込み
	//chipImg_ = fileMng_.LoadImageFM("Resource/MapChip/Mapchip_def.png"); 

	chipInfo_.resize(CHIP_HEIGHT * CHIP_WIDTH + 1);

	chipInfo_[0] = { false };
	chipInfo_[1] = { true };
	chipInfo_[2] = { true };
	chipInfo_[3] = { true };
	chipInfo_[4] = { true };
	chipInfo_[5] = { true };
	chipInfo_[6] = { true };
	chipInfo_[7] = { true };
	chipInfo_[8] = { true };
	chipInfo_[9] = { true };
	chipInfo_[10] = { true };
	chipInfo_[11] = { true };
	chipInfo_[12] = { true };
	chipInfo_[13] = { true };
	chipInfo_[14] = { true };
	chipInfo_[15] = { true };

	scrollX = 0;
	scrollY = 0;
	bgScrollX = 0;
	bgScrollY = 0;

	mapWidth = 0;
	mapHeight = 0;
}

Stage::~Stage()
{
}

//bool Stage::LoadTileSet(const std::string& path)
//{
//	tileset_ = fileManager_.LoadImageFM(path);
//	return tileset_ != nullptr;
//}

bool Stage::Load(const std::string& path)
{
	std::ifstream ifs(path);
	if (!ifs.is_open()) return false;

	tileMap.clear();

	std::string line;
	while (std::getline(ifs, line))
	{
		if (line.find("\"width\"") != std::string::npos)
		{
			sscanf_s(line.c_str(), "%*[^0-9]%d", &mapWidth);
		}
		if (line.find("\"height\"") != std::string::npos)
		{
			sscanf_s(line.c_str(), " %*[^0-9]%d", &mapHeight);
		}
		// tilesizeはこちらのソリューションで決定
		//if (line.find("\"tile_size\"") != std::string::npos)
		//{
		//	sscanf_s(line.c_str(), " %*[^0-9]%d", &tileSize);
		//}

		// タイル配列
		if (line.find("[") != std::string::npos &&
			line.find("]") != std::string::npos &&
			line.find(",") != std::string::npos)
		{
			// 二次元配列にpushする用の配列
			std::vector<int> row;
			// 行のコピー取得
			char buf[512];
			strcpy_s(buf, line.c_str());

			char* ctx = nullptr;
			// 行の分解
			char* token = strtok_s(buf, "[], ", &ctx);
			while (token)
			{
				int v;
				if (sscanf_s(token, "%d", &v) == 1)
				{
					row.push_back(v);
				}
				token = strtok_s(nullptr, "[], ", &ctx);
			}
			if (!row.empty())
			{
				tileMap.push_back(row);
			}
		}
	}
	return true;
}


//void Stage::SetMap(const std::vector<int>& tiles, int cols, int rows)
//{
//	cols_ = cols;
//	rows_ = rows;
//	tiles_ = tiles;
//	// サイズチェック
//	if (static_cast<int>(tiles_.size()) != cols_ * rows_)
//	{
//		tiles_.resize(cols_ * rows_, 0);
//	}
//}

void Stage::Update()
{
	// GetMaxScrollX/Yでスクロールの最大値を取得して、現在のスクロール位置から背景のスクロール位置を計算
	if (GetMaxScrollX() > 0)
	{
		bgScrollX = static_cast<int>((static_cast<float>(scrollX) / GetMaxScrollX()) * (Application::SCREEN_WID - bgImg_->GetWidth()));
	}
	else
	{
		bgScrollX = 0;
	}
	if (GetMaxScrollY() > 0)
	{
		 bgScrollY = static_cast<int>((static_cast<float>(scrollY) / GetMaxScrollY()) * (Application::SCREEN_HIG - bgImg_->GetHeight()));
	}
	else
	{
		bgScrollY = 0;
	}
}


void Stage::Draw()
{
	int x, y;

	// 背景の描画
	DrawExtendGraph(bgScrollX, bgScrollY, bgScrollX + bgImg_->GetWidth(), bgScrollY + bgImg_->GetHeight(), bgImg_->GetHandle(), true);

	// マップデータの参照位置
	int startX = static_cast<int>(scrollX / CHIP_SIZE);
	int startY = static_cast<int>(scrollY / CHIP_SIZE);

	int offsetX = scrollX % CHIP_SIZE;
	int offsetY = scrollY % CHIP_SIZE;
	int drawCols = Application::SCREEN_WID / CHIP_SIZE + 2;
	int drawRows = Application::SCREEN_HIG / CHIP_SIZE + 2;

	for (int y = 0; y < drawRows; y++)
	{
		for (int x = 0; x < drawCols; x++)
		{
			int mapX = startX + x;
			int mapY = startY + y;

			if (mapX < 0 || mapY < 0 ||
				mapX >= mapWidth || mapY >= mapHeight)
				continue;

			int chipId = tileMap[mapY][mapX];
			if (chipId == 0) continue;

			DrawRectGraph(x * CHIP_SIZE - offsetX, y * CHIP_SIZE - scrollY % CHIP_SIZE,
						  (chipId % CHIP_WIDTH) * CHIP_SIZE, (chipId / CHIP_WIDTH) * CHIP_SIZE, CHIP_SIZE, CHIP_SIZE, chipImg_->GetHandle(), true);
		}
	}
}

//bool Stage::IsSolidAt(int col, int row) const
//{
//	// 範囲外は空タイル扱い
//	if (col < 0 || row < 0 || col >= cols_ || row >= rows_) return false;
//	int v = tiles_[row * cols_ + col];
//
//	return v != 0;
//}

bool Stage::CheckWall(int worldX, int worldY) const
{
	int cx = worldX / CHIP_SIZE;
	int cy = worldY / CHIP_SIZE;

	//printfDx("%d, %d\n", mapHeight, mapWidth);
	if (cx < 0 || cx >= mapWidth || cy < 0 || cy >= mapHeight)
	{
		return true;
	}

	int chip = tileMap[cy][cx];
	return chipInfo_[chip].isWall;
}

void Stage::SetScrollX(int s)
{
	scrollX = s;
}

void Stage::SetScrollY(int s)
{
	scrollY = s;
}

void Stage::SetBgImage(const std::string& path)
{
	bgImg_ = fileMng_.LoadImageFM(path);
}

void Stage::SetMchipImage(const std::string& path)
{
	chipImg_ = fileMng_.LoadImageFM(path);
	printfDx("マップチップ画像の読み込みに%s。\n Handle = %d", chipImg_ ? "成功しました" : "失敗しました", chipImg_->GetHandle());
}

int Stage::GetScrollX()
{
	return scrollX;
}

int Stage::GetScrollY()
{
	return scrollY;
}

int Stage::GetMaxScrollX()
{
	return mapWidth * CHIP_SIZE - Application::SCREEN_WID;
}

int Stage::GetMaxScrollY()
{
	return mapHeight * CHIP_SIZE - Application::SCREEN_HIG;
}

//当たり判定
bool Stage::CheckHitWallRect(int x, int y, int width, int height)
{
	// チップサイズを考慮して、矩形の四隅の座標を計算
	int left = x - width / 2;
	int top = y - height / 2;
	int right = x + width / 2 - 1;
	int bottom = y + height / 2 - 1;
	// 四隅＋辺の中央＋さらに半分の座標で壁をチェック
	if (CheckWall(left, top)) return true;
	if (CheckWall((left + right) / 4, top)) return true;
	if (CheckWall((left + right) / 2, top)) return true;
	if (CheckWall((left + right) / 4 * 3, top)) return true;
	if (CheckWall(right, top)) return true;
	if (CheckWall(left, (top + bottom) / 4)) return true;
	if (CheckWall(left, (top + bottom) / 2)) return true;
	if (CheckWall(left, (top + bottom) / 4 * 3)) return true;
	if (CheckWall(left, bottom)) return true;
	if (CheckWall((left + right) / 4, bottom)) return true;
	if (CheckWall((left + right) / 2, bottom)) return true;
	if (CheckWall((left + right) / 4 * 3, bottom)) return true;
	if (CheckWall(right, bottom)) return true;
	if (CheckWall(right, (top + bottom) / 4)) return true;
	if (CheckWall(right, (top + bottom) / 2)) return true;
	if (CheckWall(right, (top + bottom) / 4 * 3)) return true;

	return false;
}

// マップチップを設定
void Stage::SetChip(int chipX, int chipY, int chipId)
{
	if (chipX < 0 || chipX >= mapWidth || chipY < 0 || chipY >= mapHeight)
	{
		return; // 範囲外
	}

	tileMap[chipY][chipX] = chipId;
}

// マップチップを取得
int Stage::GetChip(int chipX, int chipY) const
{
	if (chipX < 0 || chipX >= mapWidth || chipY < 0 || chipY >= mapHeight)
	{
		return 0; // 範囲外
	}

	return tileMap[chipY][chipX];
}

// 矩形範囲（左、上、右、下）に壁があるかチェック
//bool Stage::CheckCollision(float left, float top, float right, float bottom) const
//{
//	//// 座標をタイルインデックスに変換
//	//int minCol = static_cast<int>(left) / tileW_;
//	//int maxCol = static_cast<int>(right) / tileW_;
//	//int minRow = static_cast<int>(top) / tileH_;
//	//int maxRow = static_cast<int>(bottom) / tileH_;
//
//	//// 範囲内のタイルをすべてループで回す
//	//for (int r = minRow; r <= maxRow; ++r)
//	//{
//	//	for (int c = minCol; c <= maxCol; ++c)
//	//	{
//	//		if (IsSolidAt(c, r)) return true;
//	//	}
//	//}
//	//return false;
//}

//bool Stage::IsSolidWorld(float x, float y) const
//{
//	if (tileW_ <= 0 || tileH_ <= 0) return false;
//	int col = static_cast<int>(x) / tileW_;
//	int row = static_cast<int>(y) / tileH_;
//	return IsSolidAt(col, row);
//}