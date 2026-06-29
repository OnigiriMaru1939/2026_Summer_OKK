#include "RemotePlayer.h"

void RemotePlayer::SyncState(const PlayerPacket& packet)
{
	// 受信したデータを自身の変数にコピー
	posX = packet.posX;
	posY = packet.posY;
	vx = packet.vx;
	vy = packet.vy;
	isAttack = packet.isAttack;
}

void RemotePlayer::SyncState(const SodaJumpPacket& packet)
{
}

void RemotePlayer::Draw(float scrollX, float scrollY)
{
	float drawX = posX - scrollX;
	float drawY = posY - scrollY;

	DrawCircle(static_cast<int>(drawX), static_cast<int>(drawY), 20, GetColor(255, 0, 0), TRUE);
}