#include "BulletBase.h"
#include "FileManager.h"
#include "ImageFile.h"


BulletBase::BulletBase(FileManager& fileMng, Stage* stage)
	: fileManager_(fileMng)
	, stage_(stage)
	, x_(0.0f)
	, y_(0.0f)
	, vx_(0.0f)
	, vy_(0.0f)
	, width_(0)
	, height_(0)
	, isAlive_(true)
{
}

BulletBase::~BulletBase()
{
}

bool BulletBase::SetImage(const std::string& path)
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

void BulletBase::SetPosition(float x, float y)
{
	x_ = x;
	y_ = y;
}

void BulletBase::SetVelocity(float vx, float vy)
{
	vx_ = vx;
	vy_ = vy;
}

void  BulletBase::Update()
{

}

void BulletBase::Draw() const
{
	if (!isAlive_) return;
	if (image_)
	{
		DrawGraph(
			static_cast<int>(x_ - width_ / 2) - stage_->GetScrollX(),
			static_cast<int>(y_ - height_ / 2) - stage_->GetScrollY(),
			image_->GetHandle(),
			TRUE
		);
	}
	else
	{
		DrawCircle(
			static_cast<int>(x_) - stage_->GetScrollX(),
			static_cast<int>(y_) - stage_->GetScrollY(),
			5,
			GetColor(255, 255, 0),
			TRUE
		);
	}
}

void BulletBase::ApplyDamage(int dmg)
{
	// ダメージ処理はここで行う
	// 例えば、弾がダメージを受けた場合の処理を実装する
}

bool BulletBase::WillCollide(int newX, int newY)
{
	return stage_->CheckHitWallRect(
		newX,
		newY,
		width_,
		height_
	);
}