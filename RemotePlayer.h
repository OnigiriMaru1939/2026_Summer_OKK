#pragma once
#include <DxLib.h>
#include "Protocol.h"

class FileManager;
class ImageFile;
class SoundFile;
class ParticleManager;
class ParticleEmitter;

class RemotePlayer
{
private:
	float posX, posY;
	float vx, vy;
	float angle;
	float scale;
	bool isAttack;
	int bossEventState_ = 0;
	float sodaShakeGauge = 0.0f;
	float sodaHeatShakeGauge = 0.0f;
	float sodaRatio = 0.0f;
	float sodaHeatRatio = 0.0f;

	float shakeOffsetX = 0.0f;
	float shakeOffsetY = 0.0f;

	std::shared_ptr<ImageFile> _image;
	std::shared_ptr<SoundFile> sodaAttackSE;		//炭酸攻撃のサウンド
	ParticleManager& _pMng;
	std::weak_ptr<ParticleEmitter> sodaParticle;		// 炭酸攻撃のパーティクルエミッター 

	int _width;
	int _height;

	bool _isActive = false;
public:
	RemotePlayer(FileManager& fileMng, ParticleManager& pMng);
	~RemotePlayer();

	// 通信パケットを受け取って状態を更新するメソッド
	void SyncState(const PlayerPacket& packet);
	void SyncState(const SodaJumpPacket& packet);

	void SetBossEventState(int state) { bossEventState_ = state; }
	int GetBossEventState() const { return bossEventState_; }

	float GetPosX() const { return posX; }
	float GetPosY() const { return posY; }
	bool IsActive() const { return _isActive; }

	void Update();
	void Draw(float scrollX, float scrollY); // 描画処理
};