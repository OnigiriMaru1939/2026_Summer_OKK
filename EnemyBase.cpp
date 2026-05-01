#include "EnemyBase.h"
#include "FileManager.h"
#include "ImageFile.h"
#include <DxLib.h>

EnemyBase::EnemyBase(FileManager& fileMng, float x, float y)
	: fileManager_(fileMng)
	, x_(x)
	, y_(y)
	, vx_(0.0f)
	, vy_(0.0f)
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

void EnemyBase::Move(float dx, float dy)
{
	x_ += dx; // x座標を移動
	y_ += dy; // y座標を移動
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

float EnemyBase::GetX() const { return x_; } 
float EnemyBase::GetY() const { return y_; }
int EnemyBase::GetWidth() const { return width_; }
int EnemyBase::GetHeight() const { return height_; }

void EnemyBase::Draw() const
{
	if(!isAlive_ || !image_) return; // 生存していないか画像がない場合は描画しない
	

	int handle = 0;
	handle = image_->GetHandle(); // 画像のハンドルを取得
	DrawGraph(static_cast<int>(x_), static_cast<int>(y_), handle, TRUE); // 画像を描画

}