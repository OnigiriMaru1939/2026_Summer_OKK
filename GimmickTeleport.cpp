#include "GimmickTeleport.h"
#include <DxLib.h>
#define _USE_MATH_DEFINES
#include <math.h>

void GimmickTeleport::Update()
{
	angle_ += 0.01f;

	// 角度が 2π (360度) を超えたら、数値をリセットしてオーバーフローを防ぐ
	if (angle_ >= 6.283185f) // 2 * M_PI の値
	{
		angle_ -= 6.283185f;
	}
	Draw();
}

void GimmickTeleport::Draw() const
{
	DrawRotaGraph(
		static_cast<int>(x_ - stage_->GetScrollX()), // スクロールを加味した画面上のX中心
		static_cast<int>(y_ - stage_->GetScrollY()), // スクロールを加味した画面上のY中心
		1.0,                                         // 拡大率 (1.0で等倍)
		angle_,                                      // 回転角度
		image_->GetHandle(),                         // 画像ハンドル  
		TRUE                                         // 透過処理を行うか
	);
}

void GimmickTeleport::OnCollidePlayer(Player& player)
{
	// プレイヤーの位置を強制変更
	player.SetPosition(targetX_, targetY_);
	// テレポート陣は消えないようにする場合は isAlive_ は true のまま
}