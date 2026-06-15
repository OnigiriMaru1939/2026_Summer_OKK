#pragma once
#include "GimmickBase.h"
#include "FileManager.h"
#include "ImageFile.h"
#include "Stage.h"
#include "Player.h"


class GimmickTeleport : public GimmickBase
{
public:
	GimmickTeleport(FileManager& fileMng, Stage* stage, float x, float y, float targetX, float targetY)
		: GimmickBase(fileMng, stage, x, y, GIMMICK_TYPE::TELEPORT)
		, targetX_(targetX), targetY_(targetY)
	{
	}
	~GimmickTeleport() override = default;
	void Update() override; // 更新処理
	void OnCollidePlayer(Player& player) override;

private:
	float targetX_, targetY_; // ワープ先座標
};