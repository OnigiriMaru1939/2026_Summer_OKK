#include "GimmickBase.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "Player.h"
#include <DxLib.h>

GimmickBase::GimmickBase(FileManager& fileMng, Stage* stage, float x, float y,GIMMICK_TYPE gimmicktype)
	: fileManager_(fileMng)
	, stage_(stage)
	, x_(x)
	, y_(y)
	, canvasX(0)
	, canvasY(0)
	, width_(0)
	, height_(0)
	, isAlive_(true)
	, gimmickType_(gimmicktype)
{
}

GimmickBase::~GimmickBase()
{
}

bool GimmickBase::SetImage(const std::string& path)
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

void GimmickBase::SetPosition(float x, float y)
{
	x_ = x; // x座標を設定
	y_ = y; // y座標を設定
}

bool GimmickBase::IsAlive() const
{
	return isAlive_; // 生存フラグを返す
}

//衝突判定
bool GimmickBase::WillCollide(int newX, int newY)
{
	return stage_->CheckHitWallRect(
		newX,
		newY,
		width_,
		height_
	);
}

//敵の矩形衝突判定
RECT GimmickBase::GetRect() const
{
	RECT rect;
	rect.left = static_cast<LONG>(x_ - width_ / 2);
	rect.top = static_cast<LONG>(y_ - height_ / 2);
	rect.right = static_cast<LONG>(x_ + width_ / 2);
	rect.bottom = static_cast<LONG>(y_ + height_ / 2);
	return rect;
}

void GimmickBase::Draw() const
{
	if (!isAlive_ /*|| !image_*/) return; // 生存していないか画像がない場合は描画しない

#ifdef _DEBUG
	//当たり判定の矩形を描画
	RECT rc = GetRect();

	DrawBox(
		rc.left - stage_->GetScrollX(),
		rc.top - stage_->GetScrollY(),
		rc.right - stage_->GetScrollX(),
		rc.bottom - stage_->GetScrollY(),
		GetColor(0, 255, 0),
		FALSE
	);
#endif

	if (!image_) return;


	int handle = 0;
	handle = image_->GetHandle(); // 画像のハンドルを取得
	DrawExtendGraph(
		x_ - width_ / 2 - stage_->GetScrollX(),
		y_ - height_ / 2 - stage_->GetScrollY(),
		x_ + width_ / 2 - stage_->GetScrollX(),
		y_ + height_ / 2 - stage_->GetScrollY(),
		handle,
		TRUE
	);
}