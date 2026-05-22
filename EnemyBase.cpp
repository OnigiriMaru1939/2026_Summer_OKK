#include "EnemyBase.h"
#include "FileManager.h"
#include "ImageFile.h"
#include <DxLib.h>


EnemyBase::EnemyBase(FileManager& fileMng, Stage* stage, float x, float y)
	: fileManager_(fileMng)
	, stage_(stage)
	, x_(x)
	, y_(y)
	, vx_(0.0f)
	, vy_(0.0f)
	, gravity(0.5f)
	, width_(0)
	, height_(0)
	, hp_(1)
	, isAlive_(true)
{
}

EnemyBase::~EnemyBase()
{
}

bool EnemyBase::SetImage(const std::string& path)
{
	auto img = fileManager_.LoadImageFM(path);
	if (!img)
	{
		image_.reset();
		width_ = height_ = 0;
		return false;
	}

	image_ = img;
	
	width_ = image_->GetWidth();
	height_ = image_->GetHeight();
	return true;
}

void EnemyBase::SetPosition(float x, float y)
{
	x_ = x; // x座標を設定
	y_ = y; // y座標を設定
}

void EnemyBase::SetVelocity(float vx, float vy)
{
	vx_ = vx; // x方向の速度を設定
	vy_ = vy; // y方向の速度を設定
}

void EnemyBase::Move()
{
	int signX = (vx_ > 0) ? 1 : ((vx_ < 0) ? -1 : 0);
	int loopX = (int)std::abs(vx_);
	while (loopX > 0)
	{
		if (WillCollide(x_ + signX, y_))
		{
			//壁に当たると反転
			vx_ = -vx_;
			break;
		}
		x_ += signX;
		loopX--;
	}

	int signY = (vy_ > 0) ? 1 : ((vy_ < 0) ? -1 : 0);
	int loopY = (int)std::abs(vy_);
	while (loopY > 0)
	{
		if (WillCollide(x_, signY + y_))
		{
			vy_ = 0;
			break;
		}
		y_ += signY;
		loopY--;
	}
}

void EnemyBase::AddGravity()
{
	vy_ += gravity; //y方向の速度に重力を加算
}

void EnemyBase::ApplyDamage(int dmg)
{
	if (!isAlive_) 	return;
	hp_ -= dmg; // ダメージを適用
	if (hp_ <= 0)
	{
		isAlive_ = false;
		hp_ = 0;
	}
}

bool EnemyBase::IsAlive() const
{
	return isAlive_; // 生存フラグを返す
}

//衝突判定
bool EnemyBase::WillCollide(int newX, int newY)
{
	int left = newX - width_ / 2;
	int top = newY - height_ / 2;
	int right = newX + width_ / 2;
	int bottom = newY + height_ / 2;

	// 八か所の座標で壁をチェック（マップチップの壁 + LightWallGimmickの壁チップ）
	if (stage_->CheckWall(left, top)) return true;
	if (stage_->CheckWall((left + right) / 2 - 1, top)) return true;
	if (stage_->CheckWall(right - 1, top)) return true;
	if (stage_->CheckWall(left, (top + bottom) / 2 - 1)) return true;
	if (stage_->CheckWall(left, bottom - 1)) return true;
	if (stage_->CheckWall((left + right) / 2 - 1, bottom - 1)) return true;
	if (stage_->CheckWall(right - 1, bottom - 1)) return true;
	if (stage_->CheckWall(right - 1, (top + bottom) / 2 - 1)) return true;

	return false;
}

float EnemyBase::GetX() const { return x_; } 
float EnemyBase::GetY() const { return y_; }
int EnemyBase::GetWidth() const { return width_; }
int EnemyBase::GetHeight() const { return height_; }

void EnemyBase::Draw() const
{
	if(!isAlive_ || !image_) return; // 生存していないか画像がない場合は描画しない
	

	int handle = 0;
	handle = image_->GetHandle(); // 画像のハンドルを取得
	DrawGraph(static_cast<int>(x_ - width_ / 2 - stage_->GetScrollX()), static_cast<int>(y_ - height_ / 2 - stage_->GetScrollY()), handle, TRUE); // 画像を描画
}