#include "BulletBase.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "Stage.h"
#include <DxLib.h>


BulletBase::BulletBase(FileManager& fileMng, Stage* stage, float x, float y, float vx, float vy, float scale)
	: fileManager(fileMng), stage_(stage), x_(x), y_(y), vx_(vx), vy_(vy), scale_(scale), isAlive_(true)
{
}

BulletBase::~BulletBase()
{
}

//画像のセット
bool BulletBase::SetImage(const std::string& path)
{
	auto img = fileManager.LoadImageFM(path);

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

//位置を設定
void BulletBase::SetPosition(float x, float y)
{
	x_ = x;
	y_ = y;
}

//速度を設定
void BulletBase::SetVelocity(float vx, float vy)
{
	vx_ = vx;
	vy_ = vy;
}

//更新処理
void BulletBase::Update()
{
	if (!isAlive_) return;

	x_ += vx_;
	y_ += vy_;
	if (stage_->CheckHitWallRect(static_cast<int>(x_), static_cast<int>(y_), width_, height_))
	{
		isAlive_ = false;
	}
}

//描画処理
void BulletBase::Draw() const
{
	if (!isAlive_ || !image_) return;

	int handle = image_->GetHandle();
	DrawRotaGraph(
		static_cast<int>(x_) - stage_->GetScrollX(),
		static_cast<int>(y_) - stage_->GetScrollY(),
		scale_,
		0.0,
		handle,
		true
	);
}

//射撃処理
void BulletBase::Shot(float dirX, float dirY, float scale)
{
	float length = std::sqrt(dirX * dirX + dirY * dirY);
	if (length != 0)
	{
		vx_ = (dirX / length) * scale;
		vy_ = (dirY / length) * scale;
	}
	else
	{
		vx_ = 0;
		vy_ = 0;
	}
}

//生存フラグを取得
bool BulletBase::IsAlive() const
{
	return isAlive_;
}

//衝突判定
bool BulletBase::WillCollide(int newX, int newY)
{
	return stage_->CheckHitWallRect(
		newX,
		newY,
		static_cast<int>(width_ * scale_),
		static_cast<int>(height_ * scale_)
	);
}

RECT BulletBase::GetRect() const
{
	RECT rect;

	int hitWidth = static_cast<int>(width_ * scale_);
	int hitHeight = static_cast<int>(height_ * scale_);

	rect.left = static_cast<LONG>(x_ - hitWidth / 2);
	rect.right = static_cast<LONG>(x_ + hitWidth / 2);
	rect.top = static_cast<LONG>(y_ - hitHeight / 2);
	rect.bottom = static_cast<LONG>(y_ + hitHeight / 2);

	return rect;
}