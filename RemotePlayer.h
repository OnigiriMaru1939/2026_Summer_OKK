#pragma once
#include <DxLib.h>
#include "Protocol.h"

class RemotePlayer
{
private:
	float posX, posY;
	float vx, vy;
	bool isAttack;
	int bossEventState_ = 0;

public:
	RemotePlayer() : posX(0), posY(0), vx(0), vy(0), isAttack(false) {}

	// 通信パケットを受け取って状態を更新するメソッド
	void SyncState(const PlayerPacket& packet);
	void SyncState(const SodaJumpPacket& packet);

	void SetBossEventState(int state) { bossEventState_ = state; }
	int GetBossEventState() const { return bossEventState_; }

	void Draw(float scrollX, float scrollY); // 描画処理
};