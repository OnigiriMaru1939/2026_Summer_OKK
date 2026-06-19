#include "GimmickTeleport.h"

void GimmickTeleport::Update()
{
}

void GimmickTeleport::OnCollidePlayer(Player& player)
{
	// プレイヤーの位置を強制変更
	player.SetPosition(targetX_, targetY_);
	// テレポート陣は消えないようにする場合は isAlive_ は true のまま
}