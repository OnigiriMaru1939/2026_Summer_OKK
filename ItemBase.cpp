#include "ItemBase.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "Player.h"

ItemBase::ItemBase(FileManager& fileMng, Stage* stage, float x, float y)
	: fileManager_(fileMng)
	, stage_(stage)
	, x_(x)
	, y_(y)
	, width_(0)
	, height_(0)
	, alive_(true)
	,posX_(0)
	,posY_(0)
{

}

ItemBase::~ItemBase()
{

}

bool ItemBase::SetImage(const std::string& path)
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

void ItemBase::SetPosition(float x, float y)
{
	x_ = x; // x座標を設定
	y_ = y; // y座標を設定
}


//アイテムの矩形衝突判定
RECT ItemBase::GetRect() const
{
	RECT rect;
	rect.left = static_cast<LONG>(x_ - width_ / 2);
	rect.top = static_cast<LONG>(y_ - height_ / 2);
	rect.right = static_cast<LONG>(x_ + width_ / 2);
	rect.bottom = static_cast<LONG>(y_ + height_ / 2);
	return rect;
}

void ItemBase::Update()
{

}

void ItemBase::Draw()
{
	//生存していないか画像がない場合は描画しない
	if (!alive_ || !image_) return;

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

	int handle = 0;
	handle = image_->GetHandle();
	DrawExtendGraph(
		x_ - width_ / 2 - stage_->GetScrollX(),
		y_ - height_ / 2 - stage_->GetScrollY(),
		x_ + width_ / 2 - stage_->GetScrollX(),
		y_ + height_ / 2 - stage_->GetScrollY(),
		handle,
		TRUE
	);
}

void ItemBase::OnGet(Player& player)
{

}